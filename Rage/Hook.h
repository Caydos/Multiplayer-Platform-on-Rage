#pragma once
#ifndef HOOK_H
#define HOOK_H
//#include "Process.h"

typedef struct GtaThread GtaThread;
typedef struct GtaThreadVtbl GtaThreadVtbl;

struct GtaThreadVtbl
{
	void* (*__vecDelDtor)(GtaThread* that, unsigned int);
	int(*Reset)(GtaThread* that, uint32_t scriptHash, void* args, uint32_t arg_count);
	int(*Run)(GtaThread* that);
	int(*Tick)(GtaThread* that, uint32_t ops_to_execute);
	void(*Kill)(GtaThread* that);
};

struct GtaThread
{
	GtaThreadVtbl* vfptr;
	uint32_t thread_id;
	uint32_t script_hash;
};

class Hook
{
private:

public:
	static void Initialize();
	static void Uninitialize();
};


#endif // !HOOK_H