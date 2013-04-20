"use strict";

var keyboard = require('./build/Release/kbd.node');
//var keyboard = require('kbd');
var util = require('util');

// getLineSync

util.puts("getLineSync ? ");
util.puts("you type this line : " + keyboard.getLineSync());

// getLineSync without echo

util.puts("set echo off");
keyboard.setEcho(false);
util.puts("enter a line ? ");
util.puts("you type this line : " + keyboard.getLineSync());

// getKey (asynchrone) without echo

var fasl = function (erreur, reponse) {
	util.puts("you type this line |" + reponse + "|");
	util.puts("length |" + reponse.length + "|");
	util.puts("status " + erreur);
	if(reponse !== "end") {
		util.puts("getLine asynchronously (type 'end' to stop asking) ? ");
		keyboard.getLine(fasl);
	}
};

var fask = function (erreur, reponse) {
	keyboard.setCanonical(true);
	keyboard.setEcho(true);
	util.puts("set echo on, normal mode");
	util.puts("you type this key " + reponse);
	util.puts("status " + erreur);
	util.puts("getLine asynchronously (type 'end' to stop asking) ? ");
	keyboard.getLine(fasl);
};

keyboard.setCanonical(false);
util.puts("set canonical false");
util.puts("type a key ? ");
keyboard.getKey(fask);
