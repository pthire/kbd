//===========================================================================
//
// kbd module for node
//
// Author : P. Thiré
// Date : 19/03/2014 - Version 0.0.6
//
// This version is the first one using "nan" (Native Abstractions for Node)
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

#include <nan.h>

#include "kbd_f.h"  // NOLINT(build/include)

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::Function;
using v8::Local;
using v8::Null;
using v8::Number;
using v8::Value;
using v8::String;

//=======================================================================
// SYNCHRONOUS FUNCTIONS
//=======================================================================

NAN_METHOD(GetLineSync) {
	NanScope();
	std::string est = GetLine();
	NanReturnValue(NanNew<String>(est));
}

NAN_METHOD(GetKeySync) {
	NanScope();
	char* est = GetKey();
	NanReturnValue(NanNew<String>(est));
}

NAN_METHOD(SetCanonical) {
	NanScope();
	if (args.Length() < 1) {
		NanThrowError("First argument must be a true or false");
		NanReturnValue(NanNew<Number>(-1));
	}
	int new_status = args[0]->Uint32Value();
	new_status = SetCanonical(new_status);
	NanReturnValue(NanNew<Number>(new_status));
}

NAN_METHOD(SetEcho) {
	NanScope();
	if (args.Length() < 1) {
		NanThrowError("First argument must be a true or false");
		NanReturnValue(NanNew<Number>(-1));
	}
	int new_status = args[0]->Uint32Value();
	new_status = SetEcho(new_status);
	NanReturnValue(NanNew<Number>(new_status));
}

//=======================================================================
// ASYNCHRONOUS FUNCTIONS
//=======================================================================

//-----------------------------------------------------------------------
// Asynchronous access to the `getLine()` function
//-----------------------------------------------------------------------

class GetLineWorker : public NanAsyncWorker {
	public:
		GetLineWorker(NanCallback *callback)
			: NanAsyncWorker(callback), line("") {}
		~GetLineWorker() {}

		// Executed inside the worker-thread.
		// It is not safe to access V8, or V8 data structures
		// here, so everything we need for input and output
		// should go on `this`.

		void Execute () {
			line = GetLine();
		}

		// Executed when the async work is complete
		// this function will be run inside the main event loop
		// so it is safe to use V8 again

		void HandleOKCallback () {
			NanScope();

			Local<Value> argv[] = {
				NanNull()
					, NanNew<String>(line)
			};

			callback->Call(2, argv);
		}

	private:
		std::string line;
};

NAN_METHOD(GetLine) {
	NanScope();

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new GetLineWorker(callback));
	NanReturnUndefined();
}

//-----------------------------------------------------------------------
// Asynchronous access to the `getKey()` function
//-----------------------------------------------------------------------

class GetKeyWorker : public NanAsyncWorker {
	public:
		GetKeyWorker(NanCallback *callback)
			: NanAsyncWorker(callback) {}
		~GetKeyWorker() {}

		// Executed inside the worker-thread.
		// It is not safe to access V8, or V8 data structures
		// here, so everything we need for input and output
		// should go on `this`.

		void Execute () {
			key = GetKey();
		}

		// Executed when the async work is complete
		// this function will be run inside the main event loop
		// so it is safe to use V8 again

		void HandleOKCallback () {
			NanScope();

			Local<Value> argv[] = {
				NanNull()
					, NanNew<String>(key)
			};

			callback->Call(2, argv);
		}

	private:
		char* key;
};

NAN_METHOD(GetKey) {
	NanScope();

	NanCallback *callback = new NanCallback(args[0].As<Function>());

	NanAsyncQueueWorker(new GetKeyWorker(callback));
	NanReturnUndefined();
}
//====================================================================
// Expose addon properties
//====================================================================

void InitAll(Handle<Object> exports) {
  exports->Set(NanNew<String>("getLineSync"),
    NanNew<FunctionTemplate>(GetLineSync)->GetFunction());

  exports->Set(NanNew<String>("getKeySync"),
    NanNew<FunctionTemplate>(GetKeySync)->GetFunction());

  exports->Set(NanNew<String>("getLine"),
    NanNew<FunctionTemplate>(GetLine)->GetFunction());

  exports->Set(NanNew<String>("getKey"),
    NanNew<FunctionTemplate>(GetKey)->GetFunction());

  exports->Set(NanNew<String>("setEcho"),
    NanNew<FunctionTemplate>(SetEcho)->GetFunction());

  exports->Set(NanNew<String>("setCanonical"),
    NanNew<FunctionTemplate>(SetCanonical)->GetFunction());
}

NODE_MODULE(kbd, InitAll)
