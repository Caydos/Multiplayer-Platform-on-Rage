#include "JavaScriptEngine.h"
#include "v8.h"
#include "libplatform/libplatform.h"
#include "V8DefaultFunctions.h"
#include <json.hpp>

using namespace v8;
using json = nlohmann::json;
std::unique_ptr<Platform> jsPlatform;
Script* scripts;

std::string JsEngine::FileToString(const std::string& _filename)
{
    std::ifstream file_stream(_filename);
    if (!file_stream)
    {
        std::cerr << "Cannot open file: " << _filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

void JsEngine::Initialize(void)
{
    V8::InitializeICUDefaultLocation("");
    V8::InitializeExternalStartupData("");
    jsPlatform = v8::platform::NewDefaultPlatform();
    V8::InitializePlatform(jsPlatform.get());
    V8::Initialize();
}


void JsEngine::Dispose(void)
{
    V8::Dispose();
    V8::DisposePlatform();
}