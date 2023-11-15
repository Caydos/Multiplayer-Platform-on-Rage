#ifndef JS_SCRIPTS_H
#define JS_SCRIPTS_H
#include "v8.h"
#include <string>
#include <functional>
#include "Fiber.h"

namespace JSScripts
{
	typedef void (*FunctionCallback)(const v8::FunctionCallbackInfo<v8::Value>& args);

	struct Resource
	{
		std::string name;
		std::string author;
		std::string description;

		std::string client;
		std::string server;
		std::string shared;

		v8::Isolate::CreateParams params;
		v8::Isolate* isolate;
		v8::Local<v8::Object> gbsObj;

		void AddFunctionToGbsObj(std::string name, FunctionCallback callback);
		void Run(void);

		v8::Persistent<v8::Function>* storedFunctions;
		std::function<void()>* functions;
		fiber* fibers;
		unsigned int functionsCount;
		v8::Persistent<v8::Function>* AllocateNewFunction();
	};


	Resource Get(const char* _name);

	void Start(const char* _name);
	void Stop(const char* _name);
	void Ensure(const char* _name);
}

#endif // !JS_SCRIPTS_H