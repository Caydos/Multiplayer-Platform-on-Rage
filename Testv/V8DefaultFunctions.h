#ifndef V8_DEFAULT_FUNCTIONS
#define V8_DEFAULT_FUNCTIONS
#include "v8.h"
#include <iostream>

//void ConsoleLog(const v8::FunctionCallbackInfo<v8::Value>& _args)
//{
//    if (_args.Length() < 1) return;
//    v8::HandleScope scope(_args.GetIsolate());
//    v8::Local<v8::Value> arg = _args[0];
//    v8::String::Utf8Value value(_args.GetIsolate(), arg);
//    std::cout << *value << std::endl;
//}

#endif // !V8_DEFAULT_FUNCTIONS