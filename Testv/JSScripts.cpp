#include "JSScripts.h"
#include "JavaScriptEngine.h"
#include <shared_mutex>
#include <json.hpp>
#include <vector>
#include "Fiber.h"

using json = nlohmann::json;
static std::vector<JSScripts::Resource> resources;
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
		v8::Local<v8::Function> jsFunction = _args[0].As<v8::Function>();
		//std::shared_lock<std::shared_mutex> lock(mutex);
		for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
		{
			if (scripts[scriptId].isolate == isolate)
			{
				unsigned int scriptFunctionId = scripts[scriptId].AllocateNewFunction();
				scripts[scriptId].storedFunctions[scriptFunctionId].Reset(isolate, jsFunction);

				std::string fiberName = scripts[scriptId].name;
				fiberName += ':#';
				fiberName += std::to_string(scripts[scriptId].functionsCount - 1);
				void** array = new void* [3];
				array[0] = scripts[scriptId].isolate;
				array[1] = &scripts[scriptId].storedFunctions[scriptFunctionId];
				array[2] = nullptr;
				Fibers::Create(fiberName.c_str(), [] {
					while (true)
					{
						std::cout << "Ticking" << std::endl;
						void** fiberContent = Fibers::GetContent();
						if (fiberContent != nullptr)
						{
							v8::Isolate* lcIsolate = (v8::Isolate*)fiberContent[0];
							v8::Persistent<v8::Function>* storedFunction = (v8::Persistent<v8::Function>*)fiberContent[1];
							if (fiberContent[2] != nullptr)
							{
								std::cout << "Stoping ma biche" << std::endl;
								lcIsolate->TerminateExecution();
								lcIsolate->Dispose();
								break;
							}

							v8::Isolate::Scope isolate_scope(lcIsolate);
							v8::HandleScope handle_scope(lcIsolate);
							v8::Local<v8::Context> context = v8::Context::New(lcIsolate);
							v8::Context::Scope context_scope(context);

							v8::Local<v8::Function> function = v8::Local<v8::Function>::New(lcIsolate, *storedFunction);

							//// Prepare for function call
							const int argc = 0;
							v8::Local<v8::Value>* argv = nullptr;  // Even if argc is 0, this needs to be defined

							//// Call the function
							v8::MaybeLocal<v8::Value> result = function->Call(context, context->Global(), argc, argv);

							// Handle the result (if necessary)
							if (result.IsEmpty())
							{
								std::cout << "V8 - Result handle is empty" << std::endl;
							}
							Fibers::Suspend(0);
						}
					}}, array);
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
		this->AddFunctionToGbsObj("suspend", Suspend);


		context->Global()->Set(context,
			v8::String::NewFromUtf8(isolate, "gbs").ToLocalChecked(),
			this->gbsObj).FromJust();

		v8::Local<v8::String> source = v8::String::NewFromUtf8(this->isolate, this->client.c_str()).ToLocalChecked();
		v8::Local<v8::Script> script = v8::Script::Compile(context, source).ToLocalChecked();
		script->Run(context);
	}
}

unsigned int JSScripts::Resource::AllocateNewFunction()
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
	return this->functionsCount - 1;
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
	scriptCount++;
	unsigned int scriptId = scriptCount - 1;
	scripts[scriptId].params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
	scripts[scriptId].isolate = v8::Isolate::New(scripts[scriptId].params);
	scripts[scriptId].functionsCount = 0;
	scripts[scriptId].storedFunctions = nullptr;
	scripts[scriptId].Run();
}

static void RemoveFromArray(const char* _name)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	for (unsigned int scriptId = 0; scriptId < scriptCount; scriptId++)
	{
		if (strcmp(scripts[scriptId].name.c_str(), _name) == 0)
		{
			if (scripts[scriptId].storedFunctions != nullptr)
			{
				for (unsigned int functionId = 0; functionId < scripts[scriptId].functionsCount; functionId++)
				{
					std::string fiberName = scripts[scriptId].name;
					fiberName += "#";
					fiberName += std::to_string(functionId);
					int val;
					Fibers::SetContentValue(fiberName.c_str(), 2, &val);//Internal Kill

					Fibers::DeleteFromName(fiberName.c_str());
					scripts[scriptId].storedFunctions[functionId].Clear();
				}
				delete[] scripts[scriptId].storedFunctions;
			}
			scripts[scriptId].gbsObj.Clear();

			std::cout << "Gotcha" << std::endl;
			delete scripts[scriptId].params.array_buffer_allocator;

			scriptCount--;
			for (unsigned int flsciptid = scriptId; flsciptid < scriptCount; flsciptid++)
			{
				scripts[flsciptid] = scripts[flsciptid + 1];
			}
			if (scriptCount <= 0)
			{
				delete[] scripts;
				scripts = nullptr;
			}
			else
			{
				//if (scripts != nullptr)
				{
					JSScripts::Resource* tempResources = new JSScripts::Resource[scriptCount];
					std::memcpy(tempResources, scripts, scriptCount * sizeof(JSScripts::Resource));
					delete[] scripts;// error there fucking script deletion
					scripts = tempResources;
				}
			}
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

