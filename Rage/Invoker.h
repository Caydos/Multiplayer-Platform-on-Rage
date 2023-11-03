#ifndef INVOKER_H
#define INVOKER_H
#include "Process.h"
#include <cstdint>

typedef uint64_t nativeHash;
typedef struct scrNativeCallContext
{
	uint64_t* result;
	uint32_t arg_count;
	uint64_t* arguments;
	uint32_t data_count;
	uint8_t data[0xC0];
} scrNativeCallContext;

typedef struct invoker_context
{
	scrNativeCallContext base;
	uint64_t result_stack[3];
	uint64_t argument_stack[64];
} invoker_context;

extern invoker_context g_context;
extern nativeHash g_hash;


typedef struct scrNativeRegistrationTable scrNativeRegistrationTable;
extern scrNativeRegistrationTable* native_registration_table;

#define NATIVE_CXT_ARG_SIZE 8


namespace Natives
{
	nativeHash GetNewHash(nativeHash oldHash);

	template <typename t>
	void PushArg(t _value)
	{
		uint64_t val;
		*(t*)(&val) = _value;
		g_context.argument_stack[g_context.base.arg_count++] = val;
	}

	void* NativeCall();

	void DumpEntryPoints(char* _outputFileName);

	template<typename ret, class...arg>
	ret Invoke(nativeHash _oldHash, arg const&..._args)
	{
		g_hash = _oldHash;
		g_context.base.arg_count = 0;
		g_context.base.data_count = 0;

		// Reset native data
		((PushArg(_args)), ...);

		return *(ret*)(NativeCall());
	};


	void Initialize();
}



//class Natives
//{
//
//private:
//
//	static nativeHash GetNewHash(nativeHash oldHash);
//	template <typename t>
//	static void PushArg(t _value)
//	{
//		uint64_t val;
//		*(t*)(&val) = _value;
//		g_context.argument_stack[g_context.base.arg_count++] = val;
//	}
//
//	static void* NativeCall();
//
//public:
//	static void DumpEntryPoints(char* _outputFileName);
//
//	template<typename ret, class...arg>
//	static ret Invoke(nativeHash _oldHash, arg const&..._args)
//	{
//		g_hash = _oldHash;
//		g_context.base.arg_count = 0;
//		g_context.base.data_count = 0;
//
//		// Reset native data
//		((PushArg(_args)), ...);
//
//		return *(ret*)(NativeCall());
//	};
//
//
//	static void Initialize();
//};


#endif // !INVOKER_H


