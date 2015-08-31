"use strict";

var addon = require('./build/Release/kbd');
var k;

console.log("This script test canonical mode");

// synchronous read with canonical mode off

addon.setCanonical(false);
console.log("Canonical mode is now off, press one key");
console.log("Synchronous read key pending...");
k = addon.getKeySync();
console.log("\nYou pressed '" + k + "'");

// asynchronous read with canonical mode off

addon.getKey(function(error, key) {
	console.log("\nYou pressed '" + key + "'");
	addon.setCanonical(true);
	console.log("Canonical mode is now on");
});
console.log("Asynchronous read key pending...");
