#include <stdio.h>
#include <iostream>
#include "Fiber.h"
#include "JavaScriptEngine.h"
#include "JSScripts.h"
#include <Windows.h>

void ScriptLauncher(void)
{
    while (true)
    {
        std::cout << "done" << std::endl;
        Fibers::Suspend(10000);
    }
    std::cout << "Called" << std::endl;
    JSScripts::Ensure("Test");
}

void TestFk()
{
    auto funk = [] {
        while (true)
        {
            std::cout << "Fleur" << std::endl; Fibers::Suspend(3000);
        }};

    Fibers::Create("fiberName.c_str()", funk);
}

int main(int argc, char* argv[]) 
{
    JsEngine::Initialize();
    Fibers::Create("ScriptLauncher", &ScriptLauncher);

    TestFk();
    auto funk = [] {
        while (true)
        {
            std::cout << "Fleur V2" << std::endl; Fibers::Suspend(3000);
        }};

    Fibers::Create("fiberName.c_str()2", funk);

    while (true)
    {
        Fibers::TickAll();
    }
    //JSScripts::Stop("Test");
    JsEngine::Dispose();
    system("pause");
    return 0;
}