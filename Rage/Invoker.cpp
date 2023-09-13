#include "pch.h"
#include "Invoker.h"
#include "CrossMap.h"


invoker_context g_context;
nativeHash g_hash;

scrNativeRegistrationTable* native_registration_table;


void (*set_data_results)(scrNativeCallContext* callContext);
void (*(*get_native_handler)(scrNativeRegistrationTable* table, uint64_t hash))(scrNativeCallContext*);

nativeHash Natives::GetNewHash(nativeHash oldHash)
{
	for (unsigned i = 0; i < NATIVE_NB; i++)
	{
		if (crossmap[i][0] == oldHash)
		{
			return crossmap[i][1];
		}
	}
	return 0;
}

void* Natives::NativeCall()
{
	void (*handler)(scrNativeCallContext*);

	handler = get_native_handler(native_registration_table, GetNewHash(g_hash));

	if (!handler)
	{
		printf("Failed to find native 0x%016llX\n", GetNewHash(g_hash));
	}

	handler(&g_context.base);

	set_data_results(&g_context.base); // AKA FixVectors

	return &g_context.result_stack[0];
}


void Natives::DumpEntryPoints(char* _outputFileName)
{
	/* EntryPoints dumper */
	FILE* fptr;
	fptr = fopen(_outputFileName, "w+");
	if (fptr == NULL)
	{
		printf("Failed to find entrypoints.txt");
	}
	for (unsigned i = 0; i < NATIVE_NB; i++)
	{
		uint64_t newHash = GetNewHash(crossmap[i][1]);
		fprintf(fptr, "0x%x : 0x%x\n", newHash, get_native_handler(native_registration_table, newHash));
	}
	fclose(fptr);
	/* */
};




void Natives::Initialize()
{
	char* ptr;

	ptr = ScanPattern(scan_base, scan_length,
		"\x76\x32\x48\x8B\x53\x40\x48\x8D\x0D",
		"xxxxxxxxx");

	if (!ptr)
	{
		printf("Failed to find native invoker pattern.");
	}

	/* Read as RIP-relative offset */
	get_native_handler = (void (*(*)(scrNativeRegistrationTable * table, uint64_t hash))(scrNativeCallContext*)) (ptr + 18 + *(int32_t*)(ptr + 18) + sizeof(int32_t));
	native_registration_table = (scrNativeRegistrationTable*)(ptr + 9 + *(int32_t*)(ptr + 9) + sizeof(int32_t));

	ptr = ScanPattern(scan_base, scan_length,
		"\x83\x79\x18\x00\x48\x8B\xD1\x74\x4A\xFF\x4A\x18\x48\x63\x4A\x18\x48\x8D\x41\x04\x48\x8B\x4C\xCA",
		"xxxxxxxxxxxxxxxxxxxxxxxx");
	if (!ptr)
	{
		printf("Failed to find set data results pattern.");
	}

	set_data_results = (void (*)(scrNativeCallContext * callContext)) ptr;

	g_context.base.result = &g_context.result_stack[0];
	g_context.base.arguments = &g_context.argument_stack[0];

}