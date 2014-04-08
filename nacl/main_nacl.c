#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>

#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"

#include "queue.h"
#include "simulador.h"

static PP_Instance g_instance = 0;
static PPB_GetInterface get_browser_interface = NULL;
static PPB_Messaging* ppb_messaging_interface = NULL;
static PPB_Var* ppb_var_interface = NULL;

/** A handle to the thread the handles messages. */
static pthread_t g_handle_message_thread;


#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/**
 * Create a new PP_Var from a C string.
 * @param[in] str The string to convert.
 * @return A new PP_Var with the contents of |str|.
 */
struct PP_Var CStrToVar(const char* str) {
     if (ppb_var_interface != NULL) {
	  return ppb_var_interface->VarFromUtf8(str, strlen(str));
     }
     return PP_MakeUndefined();
}

/**
 * Printf to a newly allocated C string.
 * @param[in] format A printf format string.
 * @param[in] args The printf arguments.
 * @return The newly constructed string. Caller takes ownership. */
char* VprintfToNewString(const char* format, va_list args) {
     va_list args_copy;
     int length;
     char* buffer;
     int result;

     va_copy(args_copy, args);
     length = vsnprintf(NULL, 0, format, args);
     buffer = (char*)malloc(length + 1); /* +1 for NULL-terminator. */
     result = vsnprintf(&buffer[0], length + 1, format, args_copy);
     if (result != length) {
	  assert(0);
	  return NULL;
     }
     return buffer;
}

/**
 * Printf to a newly allocated C string.
 * @param[in] format A print format string.
 * @param[in] ... The printf arguments.
 * @return The newly constructed string. Caller takes ownership.
 */
char* PrintfToNewString(const char* format, ...) {
     va_list args;
     char* result;
     va_start(args, format);
     result = VprintfToNewString(format, args);
     va_end(args);
     return result;
}

/**
 * Printf to a new PP_Var.
 * @param[in] format A print format string.
 * @param[in] ... The printf arguments.
 * @return A new PP_Var.
 */
struct PP_Var PrintfToVar(const char* format, ...) {
     if (ppb_var_interface != NULL) {
	  char* string;
	  va_list args;
	  struct PP_Var var;

	  va_start(args, format);
	  string = VprintfToNewString(format, args);
	  va_end(args);

	  var = ppb_var_interface->VarFromUtf8(string, strlen(string));
	  free(string);

	  return var;
     }

     return PP_MakeUndefined();
}

/**
 * Convert a PP_Var to a C string, given a buffer.
 * @param[in] var The PP_Var to convert.
 * @param[out] buffer The buffer to write to.
 * @param[in] length The length of |buffer|.
 * @return The number of characters written.
 */
uint32_t VarToCStr(struct PP_Var var, char* buffer, uint32_t length) {
     if (ppb_var_interface != NULL) {
	  uint32_t var_length;
	  const char* str = ppb_var_interface->VarToUtf8(var, &var_length);
	  /* str is NOT NULL-terminated. Copy using memcpy. */
	  uint32_t min_length = MIN(var_length, length - 1);
	  memcpy(buffer, str, min_length);
	  buffer[min_length] = 0;

	  return min_length;
     }

     return 0;
}

static void HandleMessage(char* message) 
{

//     ppb_messaging_interface->PostMessage(g_instance, CStrToVar("Starting calculations\n"));

     /* Calls the GA with the params. */
     char *result = NULL;
     double energy = 0;
     newmain(message, &result, &energy);
     ppb_messaging_interface->PostMessage(g_instance, PrintfToVar("results: %s", result));
     free(result);

     ppb_messaging_interface->PostMessage(g_instance, PrintfToVar("Energy = %f\n", energy));
}

/**
 * A worker thread that handles messages from JavaScript.
 * @param[in] user_data Unused.
 * @return unused.
 */
void* HandleMessageThread(void* user_data) {
     while (1) {
	  char* message = DequeueMessage();
	  HandleMessage(message);
	  free(message);
     }
}


static PP_Bool Instance_DidCreate(PP_Instance instance,
                                  uint32_t argc,
                                  const char* argn[],
                                  const char* argv[]) {
     g_instance = instance;
     pthread_create(&g_handle_message_thread, NULL, &HandleMessageThread, NULL);
     InitializeMessageQueue();

     return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance) {}

static void Instance_DidChangeView(PP_Instance instance,
                                   PP_Resource view_resource) {}

static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus) {}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance,
                                           PP_Resource url_loader) {
     /* NaCl modules do not need to handle the document load function. */
     return PP_FALSE;
}

static void Messaging_HandleMessage(PP_Instance instance,
                                    struct PP_Var message) {
     char buffer[10240];
     VarToCStr(message, &buffer[0], 1024);
//     ppb_messaging_interface->PostMessage(g_instance, CStrToVar("teste\n"));
     EnqueueMessage(strdup(buffer));


	  /* struct PP_Var var; */
	  /* var = PrintfToVar( */
	  /*      "Warning: dropped message \"%s\" because the queue was full.", message); */
	  /* ppb_messaging_interface->PostMessage(g_instance, var); */

}

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id,
                                       PPB_GetInterface get_browser) {
     get_browser_interface = get_browser;
     ppb_messaging_interface =
	  (PPB_Messaging*)(get_browser(PPB_MESSAGING_INTERFACE));
     ppb_var_interface = (PPB_Var*)(get_browser(PPB_VAR_INTERFACE));

     return PP_OK;
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name) {
     if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
	  static PPP_Instance instance_interface = {
	       &Instance_DidCreate,
	       &Instance_DidDestroy,
	       &Instance_DidChangeView,
	       &Instance_DidChangeFocus,
	       &Instance_HandleDocumentLoad,
	  };
	  return &instance_interface;
     } else if (strcmp(interface_name, PPP_MESSAGING_INTERFACE) == 0) {
	  static PPP_Messaging messaging_interface = {
	       &Messaging_HandleMessage,
	  };
	  return &messaging_interface;
     }
     return NULL;
}

PP_EXPORT void PPP_ShutdownModule() {}
