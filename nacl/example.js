// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

function moduleDidLoad() {
    common.hideModule();
}

// Called by the common.js module.
function domContentLoaded(name, tc, config, width, height) {
    cluster_text = "";
    potentials_text = "";
    event = 'teste';
    common.attachDefaultListeners();
    common.createNaClModule(name, tc, config, width, height);

    setDefaultValues(10, 50, 80, 60);
    initGraph();
    // 
}

function setDefaultValues(gn, sz, cr, mt)
{
    document.getElementById("generations").value = gn;
    document.getElementById("size_population").value = sz;
    document.getElementById("crossover_prob").value = cr;
    document.getElementById("mutation_prob").value = mt;
}


// // Called by the common.js module.
function attachListeners() {
    document.getElementById("start").addEventListener('click', process);
    document.getElementById("energy").addEventListener('click', calc_energy);

    document.getElementById("atomic_cluster").onchange = function (e) 
    { 
	element = e.srcElement;
	var output;
	var fr = new FileReader();
	fr.onload = function (f) {
            cluster_text = f.target.result;
	}
	var text = element.files[0];
	fr.readAsText(text);
    }

    document.getElementById("potentials").onchange = function (e) 
    { 
	element = e.srcElement;
	var output;
	var fr = new FileReader();
	fr.onload = function (f) {
            potentials_text = f.target.result;
	}
	var text = element.files[0];
	fr.readAsText(text);
    }
}
function getTextFromFile(element)
{
    var output;
    var fr = new FileReader();
    fr.onload = function (e) {
        output = e.target.result;
    }
    var text = element.files[0];
    fr.readAsText(text);
}

function process()
{
    var msg =  potentials_text + '\001' + cluster_text + '\001' +
	document.getElementById("generations").value + '\001' +
	document.getElementById("size_population").value + '\001' +
	document.getElementById("crossover_prob").value + '\001' +
	document.getElementById("mutation_prob").value + '\001'

    if (document.getElementById("oti_global").checked)
    {
    	msg = msg + "0\001"
	console.log('global');
    }
    else
    {
    	msg = msg + "1\001"	
	console.log('local');
    }


    updateCalc("PERFORMING");
    nacl_module.postMessage(msg);

}

function calc_energy()
{
    var msg =  potentials_text + '\001' + cluster_text + '\001' +
	document.getElementById("generations").value + '\001' +
	document.getElementById("size_population").value + '\001' +
	document.getElementById("crossover_prob").value + '\001' +
	document.getElementById("mutation_prob").value + '\001' +
	'2\001'

    nacl_module.postMessage(msg);
}

//   var radioEls = document.querySelectorAll('input[type="radio"]');
//   for (var i = 0; i < radioEls.length; ++i) {
//     radioEls[i].addEventListener('click', onRadioClicked);
//   }

//   // Wire up the 'click' event for each function's button.
//   var functionEls = document.querySelectorAll('.function');
//   for (var i = 0; i < functionEls.length; ++i) {
//     var functionEl = functionEls[i];
//     var id = functionEl.getAttribute('id');
//     var buttonEl = functionEl.querySelector('button');

//     // The function name matches the element id.
//     var func = window[id];
//     buttonEl.addEventListener('click', func);
//   }
// }

// function onRadioClicked(e) {
//   var divId = this.id.slice(5);  // skip "radio"
//   var functionEls = document.querySelectorAll('.function');
//   for (var i = 0; i < functionEls.length; ++i) {
//     var visible = functionEls[i].id === divId;
//     if (functionEls[i].id === divId)
//       functionEls[i].removeAttribute('hidden');
//     else
//       functionEls[i].setAttribute('hidden', '');
//   }

// function addNameToSelectElements(cssClass, handle, name) {
//   var text = '[' + handle + '] ' + name;
//   var selectEls = document.querySelectorAll(cssClass);
//   for (var i = 0; i < selectEls.length; ++i) {
//     var optionEl = document.createElement('option');
//     optionEl.setAttribute('value', handle);
//     optionEl.appendChild(document.createTextNode(text));
//     selectEls[i].appendChild(optionEl);
//   }
// }

// function removeNameFromSelectElements(cssClass, handle) {
//   var optionEls = document.querySelectorAll(cssClass + ' > option');
//   for (var i = 0; i < optionEls.length; ++i) {
//     var optionEl = optionEls[i];
//     if (optionEl.value == handle) {
//       var selectEl = optionEl.parentNode;
//       selectEl.removeChild(optionEl);
//     }
//   }
// }


/**
 * Return true when |s| starts with the string |prefix|.
 *
 * @param {string} s The string to search.
 * @param {string} prefix The prefix to search for in |s|.
 * @return {boolean} Whether |s| starts with |prefix|.
 */
function startsWith(s, prefix) {
  // indexOf would search the entire string, lastIndexOf(p, 0) only checks at
  // the first index. See: http://stackoverflow.com/a/4579228
  return s.lastIndexOf(prefix, 0) === 0;
}

// Called by the common.js module.
function handleMessage(message_event) {
  var msg = message_event.data;

    if (startsWith(msg, "results:"))
    {
	var params = msg.substring(9).split('\001');
	
	plotData(params[1], params[2], params[3]);
	//TODO: Aqui estao os parametros 
	
    }
    else
    {
	common.logMessage(msg);
    }
  // if (startsWith(msg, 'Error:')) {
  //   common.logMessage(msg);
  // } else {
  //   // Result from a function call.
  //   var params = msg.split('\1');
  //   var funcName = params[0];
  //   var funcResultName = funcName + 'Result';
  //   var resultFunc = window[funcResultName];

  //   if (!resultFunc) {
  //     common.logMessage('Error: Bad message ' + funcName +
  //                       ' received from NaCl module.');
  //     return;
  //   }

  //   resultFunc.apply(null, params.slice(1));
  // }
}

function initGraph()
{
    output=document.getElementById("output_pre");
    //output.style.width="40%";
    // output.style.border="ridge"
    graph_div=document.createElement("div");
    graph_div.id="graph_div";
    graph_div.style.position="absolute";
    // graph_div.style.top=document.body.getBoundingClientRect().top.toString();
    graph_div.style.top="50px";
    graph_div.style.rigth="10px";
    graph_div.style.left="25%";
    //graph_div.style.float="right";
    graph_div.style.width="70%";
    graph_div.style.height="80%";
    document.body.appendChild(graph_div);

    choiceContainer = $("#choices");
    ch = document.getElementById("choices");
    ch.style.float="center";

    choiceContainer.append("<br/><input type='checkbox' name='" + "melhor" +
			       "' checked='checked' id='id" + "melhor" + "'></input>" +
			       "<label for='id" + "melhor" + "'>"
			       + "Best" + "</label>");

    choiceContainer.append("<br/><input type='checkbox' name='" + "media" +
			       "'  id='id" + "media" + "'></input>" +
			       "<label for='id" + "media" + "'>"
			       + "Average" + "</label>");

    choiceContainer.append("<br/><input type='checkbox' name='" + "pior" +
			       "' id='id" + "pior" + "'></input>" +
			       "<label for='id" + "pior" + "'>"
			       + "Worst" + "</label>");

    // $.each(datasets, function(key, val) {
    // 	choiceContainer.append("<br/><input type='checkbox' name='" + key +
    // 			       "' checked='checked' id='id" + key + "'></input>" +
    // 			       "<label for='id" + key + "'>"
    // 			       + val.label + "</label>");
    // });
    
    choiceContainer.find("input").click(plotAccordingToChoices);
}

function plotData(b, m, w)
{

    if (b == undefined || b == "null") 
    {
	return;
    }
    var melhor = b.split(',');
    var media = m.split(',');
    var pior = w.split(',');

    //console.log(strName);
    var dataset1=[];
    var dataset2=[];
    var dataset3=[];

    for (var i=0;i<melhor.length;i++)
    {
        dataset1.push([i, melhor[i]]);
    }
    for (var i=0;i<media.length;i++)
    {
        dataset2.push([i, media[i]]);
    }
    for (var i=0;i<pior.length;i++)
    {
        dataset3.push([i, pior[i]]);
    }

    datasets = {
	'melhor': {
	    data:dataset1, label:"Best", color:"#00FF00"
	},
	'media': {
	    data:dataset2, label:"Average", color:"#0000FF"
	},
	'pior': {
	    data:dataset3, label:"Worst", color:"#FF0000"
	},
    }
    updateCalc("DONE");
    plotAccordingToChoices();
}

// insert checkboxes 

function plotAccordingToChoices() {

    var data = [];

    choiceContainer.find("input:checked").each(function () {
	var key = $(this).attr("name");
	if (key && datasets[key]) {
	    data.push(datasets[key]);
	}
    });
    if (data.length > 0) {
	$.plot("#graph_div", data, {
	    // yaxis: {
	    // 	min: -1000
	    // },
	    xaxis: {
		tickDecimals: 0
	    }
	});
    }
}

function updateCalc(opt_message) {
    if (opt_message) {
	var calcText = opt_message;
    }
    var calcField = document.getElementById('calculations');
    if (calcField) {
	calcField.innerHTML = calcText;
    }
}

    // var plot_params=[datasets];

    // $.plot($("#graph_div"), plot_params);



// function postMessage(msg)
// {
//         nacl_module.postMessage(msg);
// }
