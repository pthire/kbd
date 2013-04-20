#kbd

kbd module provide utilities for keyboard :

```javascript
var kbd = require('kbd');
```

```javascript
// synchronously read one line on kbd

var line = kbd.getLineSync();
```

```javascript
// asynchronously read one line on kbd

var cb = function (error, line) {
	// line variable contain kbd entered line
};

var line = kbd.getLine(cb);
```

```javascript
// synchronously read one key on kbd in non canonical mode

kbd.setEcho(false);             // no echo mode
kbd.setCanonical(false);        // non canonical mode
var char = kbd.getKeySync();
kbd.setEcho(true);              // restablish echo mode
kbd.setCanonical(true);         // restablish canonical mode
```

```javascript
// asynchronously read key on kbd

var cb = function (error, key) {
	// key variable contain pressed key
	// dont forget to reset echo and canonical mode before the end of script
	// if you forgot, type shell 'reset' command 
};

kbd.setEcho(false);             // no echo mode
kbd.setCanonical(false);        // non canonical mode
keyboard.getKey(fask);
```
