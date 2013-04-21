"use strict";

var keyboard = require('./build/Release/kbd.node');
//var keyboard = require('kbd');
var util = require('util');

// getLineSync

util.puts("Enter a line (characters ended by return key)");
util.puts("You type this line : " + keyboard.getLineSync());

// getLineSync without echo

util.puts("I set echo off");
keyboard.setEcho(false);
util.puts("Enter a line (you wil not see it)");
util.puts("You type this line : " + keyboard.getLineSync());

// getKey (asynchrone) without echo

var fasl = function (erreur, reponse) {
	util.puts("You type this line : " + reponse);
	if(reponse !== "end") {
		util.puts("Enter a line (enter 'end' to stop asking)");
		keyboard.getLine(fasl);
	}
};

var fask = function (erreur, reponse) {
	keyboard.setCanonical(true);
	keyboard.setEcho(true);
	util.puts("set echo on, normal mode");
	util.puts("You type this key : " + reponse);
	util.puts("Enter a line (enter 'end' to stop asking)");
	keyboard.getLine(fasl);
};

util.puts("I put canonical mode off");
keyboard.setCanonical(false);
util.puts("Type a key (just one, you will not see it)");
keyboard.getKey(fask);
