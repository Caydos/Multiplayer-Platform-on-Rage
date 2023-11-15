// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H


// ajouter les en-têtes à précompiler ici
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>


#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <psapi.h>
#include <stdbool.h>
#include <cstdint>
#include <shared_mutex>

#include <thread>
#include <iostream>
#include <string.h>



#endif //PCH_H
