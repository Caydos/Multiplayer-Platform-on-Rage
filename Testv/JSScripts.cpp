#include "JSScripts.h"
#include "JavaScriptEngine.h"
#include <shared_mutex>
#include <json.hpp>
#include "Fiber.h"

using json = nlohmann::json;

static JSScripts::Resource* scripts = nullptr;
static unsigned int scriptCount = 0;
static std::shared_mutex mutex;

void ConsoleLog(const v8::FunctionCallbackInfo<v8::Value>& _args)
{
	if (_args.Length() < 1) return;
	v8::HandleScope scope(_args.GetIsolate());
	v8::Local<v8::Value> arg = _args[0];
	v8::String::Utf8Value value(_args.GetIsolate(), arg);
	std::cout << *value << std::endl;
}

void CreateThread(const v8::FunctionCallbackInfo<v8::Value>& _args)
{
	v8::Isolate* isolate = _args.GetIsolate();
	v8::HandleScope handle_scope(isolate);

	// Check if there's a function argument
	if (_args.Length() > 0 && _args[0]->IsFunction())
	{
		v8::Local<v8::Function> jsFunction = v8::Local<v8::Function>::Cast(_args[0]);
		std::shared_lock<std::shared_mutex> lock(mutex);
		for (unsigned int scriptId = 0;  scriptId < scriptCount;  scriptId++)
		{
			if (scripts[scriptId].isolate == isolate)
			{
				scripts[scriptId].AllocateNewFunction()->Reset(isolate, jsFunction);
				std::string fiberName = scripts[scriptId].name;
				fiberName += ':#';
				fiberName += std::to_string(scripts[scriptId].functionsCount - 1);
				Fibers::Create(fiberName.c_str(), [] {});
			}
		}
	}
}

void Suspend(const v8::FunctionCallbackInfo<v8::Value>& _args)
{
	if (_args.Length() < 1) 
	{
		Fibers::Suspend(0);
		return;
	}
	if (!_args[0]->IsInt32() && !_args[0]->IsNumber())
	{
		Fibers::Suspend(0);
		return;
	}
	int argValue = _args[0]->Int32Value(_args.GetIsolate()->GetCurrentContext()).FromJust();
	Fibers::Suspend(argValue);
}


void JSScripts::Resource::AddFunctionToGbsObj(std::string name, FunctionCallback callback)
{
	v8::Isolate* isolate = this->isolate;

	v8::Local<v8::Context> context = isolate->GetCurrentContext();
	v8::Local<v8::Function> func = v8::Function::New(context, callback).ToLocalChecked();

	gbsObj->Set(context, v8::String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked(), func).FromJust();
}

void JSScripts::Resource::Run(void)
{
	v8::Isolate::Scope isolate_scope(this->isolate);
	v8::HandleScope handle_scope(this->isolate);

	// Create a new context
	v8::Local<v8::Context> context = v8::Context::New(this->isolate);

	{
		v8::Context::Scope context_scope(context);

		this->gbsObj = v8::Object::New(this->isolate);

		this->AddFunctionToGbsObj("log", ConsoleLog);
		this->AddFunctionToGbsObj("createThread", CreateThread);


		context->Global()->Set(context,
			v8::String::NewFromUtf8(isolate, "gbs").ToLocalChecked(),
			this->gbsObj).FromJust();

		v8::Local<v8::String> source = v8::String::NewFromUtf8(this->isolate, this->client.c_str()).ToLocalChecked();
		v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
		script->Run(context);
	}
}

v8::Persistent<v8::Function>* JSScripts::Resource::AllocateNewFunction()
{
	if (this->functionsCount)
	{
		v8::Persistent<v8::Function>* tempHolder = new v8::Persistent<v8::Function>[this->functionsCount + 1];
		std::memcpy(tempHolder, this->storedFunctions, this->functionsCount * sizeof(v8::Persistent<v8::Function>));
		delete[] this->storedFunctions;
		this->storedFunctions = tempHolder;
	}
	else
	{
		this->storedFunctions = new v8::Persistent<v8::Function>[1];
	}

	this->functionsCount++;
	return &this->storedFunctions[this->functionsCount - 1];
}


static void AddToArray(JSScripts::Resource _resource)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	if (scriptCount)
	{
		JSScripts::Resource* tempScripts = new JSScripts::Resource[scriptCount + 1];
		std::memcpy(tempScripts, scripts, scriptCount * sizeof(JSScripts::Resource));
		delete[] scripts;
		scripts = tempScripts;
	}
	else
	{
		scripts = new JSScripts::Resource[1];
	}
	scripts[scriptCount] = _resource;
	//Create isolate etc....
	scripts[scriptCount].params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	scripts[scriptCount].isolate = v8::Isolate::New(scripts[scriptCount].params);
	scripts[scriptCount].Run();
	scripts[scriptCount].functionsCount = 0;
	scripts[scriptCount].fibers = nullptr;
	scripts[scriptCount].storedFunctions = nullptr;
	scriptCount++;
}

static void RemoveFromArray(const char* _name)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
	{
		if (strcmp(scripts[scriptId].name.c_str(), _name) == 0)
		{
			scripts[scriptId].isolate->Dispose();
			delete scripts[scriptId].params.array_buffer_allocator;

			for (unsigned int flSciptId = scriptId; flSciptId < scriptCount - 1; flSciptId++)
			{
				scripts[flSciptId] = scripts[flSciptId + 1];
			}
			if (scriptCount - 1 > 0)
			{
				JSScripts::Resource* tempScripts = new JSScripts::Resource[scriptCount - 1];
				std::memcpy(tempScripts, scripts, (scriptCount - 1) * sizeof(JSScripts::Resource));
				delete[] scripts;
				scripts = tempScripts;
			}
			else
			{
				delete[] scripts;
				scripts = nullptr;
			}
			scriptCount--;
			return;
		}
	}
	std::cout << "Script not found" << std::endl;
}

JSScripts::Resource JSScripts::Get(const char* _name)
{
	Resource resource;
	resource.name = _name;

	std::string resourcePath = _name;
	resourcePath += "/manifest.json";
	std::string manifest = JsEngine::FileToString(resourcePath);

	auto jsonObject = json::parse(manifest);

	resource.author = jsonObject["author"];
	resource.description = jsonObject["description"];

	for (const auto& file : jsonObject["shared"])
	{
		std::string filePath = _name;
		filePath += '/';
		filePath += file.get<std::string>();

		std::string sharedContent = JsEngine::FileToString(filePath);

		resource.client += sharedContent;
		resource.server += sharedContent;
	}

	for (const auto& file : jsonObject["client"])
	{
		std::string filePath = _name;
		filePath += '/';
		filePath += file.get<std::string>();

		resource.client += JsEngine::FileToString(filePath);
	}

	for (const auto& file : jsonObject["server"])
	{
		std::string filePath = _name;
		filePath += '/';
		filePath += file.get<std::string>();

		resource.server += JsEngine::FileToString(filePath);
	}

	return resource;
}

void JSScripts::Start(const char* _name)
{
	{
		std::shared_lock<std::shared_mutex> lock(mutex);
		for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
		{
			if (strcmp(scripts[scriptId].name.c_str(), _name) == 0)
			{
				std::cout << "Ressource is already running !" << std::endl;
				return;
			}
		}
	}
	AddToArray(Get(_name));
}

void JSScripts::Stop(const char* _name)
{
	Resource* script = nullptr;
	{
		std::shared_lock<std::shared_mutex> lock(mutex);
		for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
		{
			if (strcmp(scripts[scriptId].name.c_str(), _name) == 0)
			{
				script = &scripts[scriptId];
			}
		}
		if (script == nullptr)
		{
			std::cout << "Ressource not found !" << std::endl;
			return;
		}
	}
	RemoveFromArray(script->name.c_str());
}

void JSScripts::Ensure(const char* _name)
{
	Resource* script = nullptr;
	{
		std::shared_lock<std::shared_mutex> lock(mutex);
		for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
		{
			if (strcmp(scripts[scriptId].name.c_str(), _name) == 0)
			{
				script = &scripts[scriptId];
			}
		}
		if (script == nullptr)
		{
			lock.unlock();
			AddToArray(Get(_name));
			return;
		}
	}
	RemoveFromArray(_name);
	AddToArray(Get(_name));
}

