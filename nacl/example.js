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
    glmol02x = "";
    common.attachDefaultListeners();
    common.createNaClModule(name, tc, config, width, height);

    setDefaultValues(10, 50, 80, 60);
    Gl_init();
    //initGraph();
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

    document.getElementById("atomic_cluster").onclick = function (e) 
    {
	this.value = null;
    }
    document.getElementById("atomic_cluster").onchange = function (e) 
    { 
	element = e.srcElement;
	var output;
	var fr = new FileReader();
	fr.onload = function (f) {
            cluster_text = f.target.result;
	    Gl_source(cluster_text);
	    glmol02x.loadMolecule();
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
	params = msg.substring(9).split('\001');
	Gl_source(params[0]);
	glmol02x.loadMolecule();
	
	//plotData(params[1], params[2], params[3]);
	//TODO: Aqui estao os parametros 
	updateCalc("DONE");	
    }
    else
    {
	common.logMessage(msg);
    }
}

function Gl_source(text_src)
{
    var text = document.createTextNode(text_src);

    e = document.getElementById("glmol02_src");

    if (e) e.remove();

    var gl_text = document.createElement("textarea");
    gl_text.id="glmol02_src";
    gl_text.style.display = 'none';
    gl_text.appendChild(text);

    document.body.appendChild(gl_text);
    

}

function Gl_init()
{

    glmol02x = new GLmol('glmol02', true);

    glmol02x.defineRepresentation = function() {
	var all = this.getAllAtoms();
	var hetatm = this.removeSolvents(this.getHetatms(all));
	this.colorByAtom(all, {});
	this.colorByChain(all);

	this.drawAtomsAsSphere(this.modelGroup, hetatm, this.sphereRadius, false, 0.2); 
	this.drawMainchainCurve(this.modelGroup, all, this.curveWidth, 'P');
	this.drawCartoon(this.modelGroup, all, this.curveWidth);
    };

}






////////////////////////////////////////////////////////////////////////////
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
