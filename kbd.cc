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

using namespace Nan; // NOLINT(build/namespaces)

using v8::FunctionTemplate;

NAN_METHOD(nothing);
NAN_METHOD(aString);
NAN_METHOD(aBoolean);
NAN_METHOD(aNumber);
NAN_METHOD(anObject);
NAN_METHOD(anArray);
NAN_METHOD(callback);

//=======================================================================
// SYNCHRONOUS FUNCTIONS
//=======================================================================

NAN_METHOD(GetLineSync) {
	std::string est = GetLine();
	//NanReturnValue(NanNew<String>(est));
	info.GetReturnValue().Set(Nan::New(est).ToLocalChecked());
}

NAN_METHOD(GetKeySync) {
    std::string est = GetKey();
    //NanReturnValue(NanNew<String>(est));
	info.GetReturnValue().Set(Nan::New(est).ToLocalChecked());
}

NAN_METHOD(SetCanonical) {
	Nan::HandleScope scope;
    if (info.Length() < 1) {
        //NanThrowError("First argument must be a true or false");
        //NanReturnValue(NanNew<Number>(-1));
		return Nan::ThrowError("First argument must be a true or false");
    }
    int new_status = info[0]->Uint32Value();
    new_status = SetCanonical(new_status);
    //NanReturnValue(NanNew<Number>(new_status));
	info.GetReturnValue().Set(new_status);
}

NAN_METHOD(SetEcho) {
	Nan::HandleScope scope;
    if (info.Length() < 1) {
        //NanThrowError("First argument must be a true or false");
        //NanReturnValue(NanNew<Number>(-1));
		return Nan::ThrowError("First argument must be a true or false");
    }
    int new_status = info[0]->Uint32Value();
    new_status = SetEcho(new_status);
    //NanReturnValue(NanNew<Number>(new_status));
	info.GetReturnValue().Set(new_status);
}

//-----------------------------------------------------------------------
// Asynchronous access to the `getLine()` function
//-----------------------------------------------------------------------

class GetLineWorker : public Nan::AsyncWorker {
    public:
        GetLineWorker(Callback *callback)
            : Nan::AsyncWorker(callback), line("") {}
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

	protected:
        void HandleOKCallback () {
			Nan::HandleScope scope;

            v8::Local<v8::Value> argv[] = {
                Nan::Null()
                    , Nan::New(line).ToLocalChecked()
            };

            callback->Call(2, argv);
        }

    private:
        std::string line;

};

NAN_METHOD(GetLine) {
	Nan::HandleScope scope;

	Callback *callback = new Callback(info[0].As<v8::Function>());

	AsyncQueueWorker(new GetLineWorker(callback));
	info.GetReturnValue().SetUndefined();
	
}

//-----------------------------------------------------------------------
// Asynchronous access to the `getKey()` function
//-----------------------------------------------------------------------

class GetKeyWorker : public Nan::AsyncWorker {
    public:
        GetKeyWorker(Callback *callback)
            : Nan::AsyncWorker(callback) {}
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

	protected:
        void HandleOKCallback () {
			Nan::HandleScope scope;

            v8::Local<v8::Value> argv[] = {
                Nan::Null()
                    , Nan::New(key).ToLocalChecked()
            };

            callback->Call(2, argv);
        }

    private:
        std::string key;

};

NAN_METHOD(GetKey) {
	Nan::HandleScope scope;

	Callback *callback = new Callback(info[0].As<v8::Function>());

	AsyncQueueWorker(new GetKeyWorker(callback));
	info.GetReturnValue().SetUndefined();
	
}

//====================================================================
// Expose addon properties
//====================================================================

NAN_MODULE_INIT(InitAll) {
	Nan::Set(target, Nan::New("getLineSync").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(GetLineSync)).ToLocalChecked());

	Nan::Set(target, Nan::New("getKeySync").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(GetKeySync)).ToLocalChecked());

	Nan::Set(target, Nan::New("setEcho").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(SetEcho)).ToLocalChecked());

	Nan::Set(target, Nan::New("setCanonical").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(SetCanonical)).ToLocalChecked());

	Nan::Set(target, Nan::New("getLine").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(GetLine)).ToLocalChecked());

	Nan::Set(target, Nan::New("getKey").ToLocalChecked(),
	  Nan::GetFunction(Nan::New<FunctionTemplate>(GetKey)).ToLocalChecked());

}

NODE_MODULE(kbd, InitAll)
