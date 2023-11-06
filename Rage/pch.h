// pch.h : Il s'agit d'un fichier d'en-tête précompilé.
// Les fichiers listés ci-dessous sont compilés une seule fois, ce qui améliore les performances de génération des futures builds.
// Cela affecte également les performances d'IntelliSense, notamment la complétion du code et de nombreuses fonctionnalités de navigation du code.
// Toutefois, les fichiers listés ici sont TOUS recompilés si l'un d'entre eux est mis à jour entre les builds.
// N'ajoutez pas de fichiers fréquemment mis à jour ici, car cela annule les gains de performance.

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



void unload();




#include <thread>
#include <iostream>
#include <string.h>

#define SERVER_PORT 55000
#define SERVER_IP "162.19.67.68"

#define EVENT_BUFFER_SIZE 5

bool InitSocket(void);
bool Connect(void);
void CloseConnection(void);

#endif //PCH_H
