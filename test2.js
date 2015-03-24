"use strict";

var addon = require('./build/Release/kbd');
var line;

console.log("This script test echo mode and synchronous/asynchronous read");

// synchronous read without echo

addon.setEcho(false);
console.log("Echo mode is now off");
console.log("Enter line ended by return key (you won't keystrokes)");
console.log("Synchronous read pending...");
line = addon.getLineSync();
console.log("You type : '" + line + "'");

// asynchronous read with echo

addon.setEcho(true);
console.log("Echo mode is now on");
console.log("Enter line ended by return key (you must see keystrokes)");
addon.getLine(function (error, line){
	console.log("You type : '" + line + "'");
});
console.log("Asynchronous read pending...");
