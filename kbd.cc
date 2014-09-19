//===========================================================================
//
// kbd module for node
//
// Author : P. Thiré
// Date : 19/09/2014
// Version : 0.0.2
//
// This module give differents oriented keyboard synchrone/asynchrone 
// functions :
//
// - getLine() (asynchronously get line from keyboard)
// - getKey() (asynchronously get key from keyboard)
// - getLineSync() (synchronously get line from keyboard)
// - getKeySync  (synchronously get key from keyboard)
// - setEcho(true) / setEcho(false)
// - setCanonical(true) / setCanonical(false)
//
//===========================================================================

#include <node.h>
#include <v8.h>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

using namespace v8;

// We use a struct to store information about the asynchronous "work request".

struct Baton {
	// This handle holds the callback function we'll call after the work request
	// has been completed in a threadpool thread. It's persistent so that V8
	// doesn't garbage collect it away while our request waits to be processed.
	// This means that we'll have to dispose of it later ourselves.
	Persistent<Function> callback;

	// Tracking errors that happened in the worker function. You can use any
	// variables you want. E.g. in some cases, it might be useful to report
	// an error number.
	bool error;
	std::string error_message;

	// Custom data you can pass through.
	char chaine[1000];
};

//===========================================================================
// setEcho
//===========================================================================

Handle<Value> SetEcho(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1) {
		// No argument was passed. Throw an exception to alert the user to
		// incorrect usage. Alternatively, we could just use 0.
		return ThrowException(
			Exception::TypeError(String::New("First argument must be a true or false"))
		);
	}

	Local<Integer> integer = args[0]->ToInteger();
	int32_t state = integer->Value();

	struct termios t;

	tcgetattr(0, &t);
	if(state == 0)
	{
		t.c_lflag &= ~(ECHO);
		tcsetattr(0, TCSANOW, &t);
	}
	else
	{
		t.c_lflag |= ECHO;
		tcsetattr(0, TCSANOW, &t);
	}

	return scope.Close(Integer::New(state));
}

//===========================================================================
// setCanonical
//===========================================================================

Handle<Value> SetCanonical(const Arguments& args) {
	HandleScope scope;
	if (args.Length() < 1) {
		// No argument was passed. Throw an exception to alert the user to
		// incorrect usage. Alternatively, we could just use 0.
		return ThrowException(
			Exception::TypeError(String::New("First argument must be a true or false"))
		);
	}

	Local<Integer> integer = args[0]->ToInteger();
	int32_t state = integer->Value();

	struct termios t;

	tcgetattr(0, &t);
	if(state == 0) {
		t.c_lflag &= ~(ICANON);
		tcsetattr(0, TCSANOW, &t);
	} else {
		t.c_lflag |= ICANON;
		tcsetattr(0, TCSANOW, &t);
	}

	return scope.Close(Integer::New(state));
}

//===========================================================================
// getKey : SYNCHRONE VERSION
//===========================================================================

Handle<Value> GetKeySync(const Arguments& args) {
	HandleScope scope;
	struct termios t;
	char chaine[1000];
	int len;

	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ICANON);

	len = read (0, chaine, 1);

	t.c_lflag |= (ECHO | ICANON);

	if(len == -1) {
		chaine[0] = '\0';
	} else {
		chaine[len] = '\0';
	}

	return scope.Close(String::New(chaine));
}

//===========================================================================
// getKey : ASYNCHRONE VERSION
//===========================================================================

// This function is executed in another thread at some point after it has been
// scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
// will crash randomly and you'll have a lot of fun debugging.
// If you want to use parameters passed into the original call, you have to
// convert them to PODs or some other fancy method.
void getKeyAsyncWork(uv_work_t* req) {
	Baton* baton = static_cast<Baton*>(req->data);

	// Do work in threadpool here.

	struct termios t;
	int len;

	tcgetattr(0, &t);
	t.c_lflag &= ~(ECHO | ICANON);

	len = read (0, baton->chaine, 1);
	if(len == -1)
	{
		baton->chaine[0] = '\0';
	}
	else
	{
		baton->chaine[len] = '\0';
	}

	t.c_lflag |= (ECHO | ICANON);

	// If the work we do fails, set baton->error_message to the error string
	// and baton->error to true.
}

// This function is executed in the main V8/JavaScript thread. That means it's
// safe to use V8 functions again. Don't forget the HandleScope!
#if(UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR == 8)
void getKeyAsyncAfter(uv_work_t* req) {
#else
void getKeyAsyncAfter(uv_work_t* req, int x) {
#endif

	HandleScope scope;
	Baton* baton = static_cast<Baton*>(req->data);

	if (baton->error) {
		Local<Value> err = Exception::Error(String::New(baton->error_message.c_str()));

		// Prepare the parameters for the callback function.
		const unsigned argc = 1;
		Local<Value> argv[argc] = { err };

		// Wrap the callback function call in a TryCatch so that we can call
		// node's FatalException afterwards. This makes it possible to catch
		// the exception from JavaScript land using the
		// process.on('uncaughtException') event.
		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
	} else {
		// In case the operation succeeded, convention is to pass null as the
		// first argument before the result arguments.
		// In case you produced more complex data, this is the place to convert
		// your plain C++ data structures into JavaScript/V8 data structures.
		const unsigned argc = 2;
		Local<Value> argv[argc] = {
			Local<Value>::New(Null()),
			Local<Value>::New(String::New(baton->chaine))
		};

		// Wrap the callback function call in a TryCatch so that we can call
		// node's FatalException afterwards. This makes it possible to catch
		// the exception from JavaScript land using the
		// process.on('uncaughtException') event.
		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
	}

	// The callback is a permanent handle, so we have to dispose of it manually.
	baton->callback.Dispose();

	// We also created the baton and the work_req struct with new, so we have to
	// manually delete both.
	delete baton;
	delete req;
}

// This is the function called directly from JavaScript land. It creates a
// work request object and schedules it for execution.
Handle<Value> GetKey(const Arguments& args) {
	HandleScope scope;

	if (!args[0]->IsFunction()) {
		return ThrowException(Exception::TypeError(
			String::New("First argument must be a callback function")));
	}
	// There's no ToFunction(), use a Cast instead.
	Local<Function> callback = Local<Function>::Cast(args[0]);

	// The baton holds our custom status information for this asynchronous call,
	// like the callback function we want to call when returning to the main
	// thread and the status information.
	Baton* baton = new Baton();
	baton->error = false;
	baton->callback = Persistent<Function>::New(callback);

	// This creates the work request struct.
	uv_work_t *req = new uv_work_t();
	req->data = baton;

	// Schedule our work request with libuv. Here you can specify the functions
	// that should be executed in the threadpool and back in the main thread
	// after the threadpool function completed.
	int status = uv_queue_work(uv_default_loop(), req, getKeyAsyncWork, getKeyAsyncAfter);
	assert(status == 0);

	return Undefined();
}
//===========================================================================
// GetLineSync
//===========================================================================

Handle<Value> GetLineSync(const Arguments& args) {
	HandleScope scope;
	char chaine[1000];
	char* s;

	s = fgets(chaine, 1000, stdin);
	if(s == NULL) {
		chaine[0] = '\0';
	} else {
		chaine[strlen(chaine)-1] = '\0';
	}

	return scope.Close(String::New(chaine));
}

//===========================================================================
// GetLine
//===========================================================================

// This function is executed in another thread at some point after it has been
// scheduled. IT MUST NOT USE ANY V8 FUNCTIONALITY. Otherwise your extension
// will crash randomly and you'll have a lot of fun debugging.
// If you want to use parameters passed into the original call, you have to
// convert them to PODs or some other fancy method.

void AsyncWork(uv_work_t* req) {
	Baton* baton = static_cast<Baton*>(req->data);
	char* s;

	// Do work in threadpool here.

	s = fgets(baton->chaine, 1000, stdin);
	if(s != NULL) {
		baton->chaine[strlen(baton->chaine)-1] = '\0';
	} else {
		baton->chaine[0] = '\0';
	}

	// If the work we do fails, set baton->error_message to the error string
	// and baton->error to true.
}

// This function is executed in the main V8/JavaScript thread. That means it's
// safe to use V8 functions again. Don't forget the HandleScope!

#if(UV_VERSION_MAJOR == 0 && UV_VERSION_MINOR == 8)
void AsyncAfter(uv_work_t* req) {
#else
void AsyncAfter(uv_work_t* req, int x) {
#endif
	HandleScope scope;
	Baton* baton = static_cast<Baton*>(req->data);

	if (baton->error) {
		Local<Value> err = Exception::Error(String::New(baton->error_message.c_str()));

		// Prepare the parameters for the callback function.
		const unsigned argc = 1;
		Local<Value> argv[argc] = { err };

		// Wrap the callback function call in a TryCatch so that we can call
		// node's FatalException afterwards. This makes it possible to catch
		// the exception from JavaScript land using the
		// process.on('uncaughtException') event.
		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
	} else {
		// In case the operation succeeded, convention is to pass null as the
		// first argument before the result arguments.
		// In case you produced more complex data, this is the place to convert
		// your plain C++ data structures into JavaScript/V8 data structures.
		const unsigned argc = 2;
		Local<Value> argv[argc] = {
			Local<Value>::New(Null()),
			//Local<Value>::New(String::New(baton->chaine))
			Local<Value>::New(String::New(baton->chaine))
		};

		// Wrap the callback function call in a TryCatch so that we can call
		// node's FatalException afterwards. This makes it possible to catch
		// the exception from JavaScript land using the
		// process.on('uncaughtException') event.
		TryCatch try_catch;
		baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);
		if (try_catch.HasCaught()) {
			node::FatalException(try_catch);
		}
	}

	// The callback is a permanent handle, so we have to dispose of it manually.
	baton->callback.Dispose();

	// We also created the baton and the work_req struct with new, so we have to
	// manually delete both.
	delete baton;
	delete req;
}

// This is the function called directly from JavaScript land. It creates a
// work request object and schedules it for execution.

Handle<Value> GetLine(const Arguments& args) {
	HandleScope scope;

	if (!args[0]->IsFunction()) {
		return ThrowException(Exception::TypeError(
			String::New("First argument must be a callback function")));
	}
	// There's no ToFunction(), use a Cast instead.
	Local<Function> callback = Local<Function>::Cast(args[0]);

	// The baton holds our custom status information for this asynchronous call,
	// like the callback function we want to call when returning to the main
	// thread and the status information.
	Baton* baton = new Baton();
	baton->error = false;
	baton->callback = Persistent<Function>::New(callback);

	// This creates the work request struct.
	uv_work_t *req = new uv_work_t();
	req->data = baton;

	// Schedule our work request with libuv. Here you can specify the functions
	// that should be executed in the threadpool and back in the main thread
	// after the threadpool function completed.
	int status = uv_queue_work(uv_default_loop(), req, AsyncWork, AsyncAfter);
	assert(status == 0);

	return Undefined();
}
//============================================================================
//  MODULE DECLARATION
//============================================================================

void init(Handle<Object> target) {
	// 2 usable methods :
	// NODE_SET_METHOD (target, "moduleMethode", c_function);
	//	or
	// target->Set(String::NewSymbol("moduleMethod"),
	//   FunctionTemplate::New(c_function)->GetFunction());

	NODE_SET_METHOD(target, "getLineSync", GetLineSync);

	target->Set(String::NewSymbol("getLine"),
		FunctionTemplate::New(GetLine)->GetFunction());

	target->Set(String::NewSymbol("getKeySync"),
		FunctionTemplate::New(GetKeySync)->GetFunction());

	target->Set(String::NewSymbol("getKey"),
		FunctionTemplate::New(GetKey)->GetFunction());

	target->Set(String::NewSymbol("setEcho"),
		FunctionTemplate::New(SetEcho)->GetFunction());

	target->Set(String::NewSymbol("setCanonical"),
		FunctionTemplate::New(SetCanonical)->GetFunction());
}

NODE_MODULE(kbd, init);
