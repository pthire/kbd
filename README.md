#kbd

kbd module provides various utilities for keyboard. I wanted to have a node equivalent for printf, scanf, puts, gets for my students to whom I teach programming with node. kdb provides these functions in both synchrone and asynchrone mode. 
kdb also gives the opportunity to set/unset echo, set unset canonical mode.

Thanks to Konstantin Käfer for his work on cpp-module (https://github.com/kkaefer/node-cpp-modules). I learned a lot about writing C++ node module by reading his code. Sorry for any bad english ;)

```javascript
var kbd = require('kbd');
```

```javascript
// synchronously reads one line from keyboard

var line = kbd.getLineSync();
```

```javascript
// asynchronously reads one line from keyboard

var cb = function (error, line) {
	// line variable contains keyboard entered line
};

var line = kbd.getLine(cb);
```

```javascript
// synchronously reads one key from keyboard in non canonical mode without echo

kbd.setEcho(false);             // no echo mode
kbd.setCanonical(false);        // non canonical mode
var char = kbd.getKeySync();
kbd.setEcho(true);              // reestablish echo mode
kbd.setCanonical(true);         // reestablish canonical mode
```

```javascript
// asynchronously reads key from keyboard

var cb = function (error, key) {
	// key variable contains pressed key
	// dont forget to reset echo and canonical mode before the end of script
	// if you forgot, type shell 'reset' command 
};

kbd.setEcho(false);             // no echo mode
kbd.setCanonical(false);        // non canonical mode
kbd.getKey(cb);            // put callback
```
