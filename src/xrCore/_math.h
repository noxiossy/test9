#pragma once

#include <thread>
#include "cpuid.h"

namespace FPU
{
XRCORE_API void m24(void);
XRCORE_API void m24r(void);
XRCORE_API void m53(void);
XRCORE_API void m53r(void);
XRCORE_API void m64(void);
XRCORE_API void m64r(void);
};
namespace CPU
{
XRCORE_API extern u64 clk_per_second;
XRCORE_API extern u64 clk_per_milisec;
XRCORE_API extern u64 clk_per_microsec;
XRCORE_API extern u64 clk_overhead;
XRCORE_API extern float clk_to_seconds;
XRCORE_API extern float clk_to_milisec;
XRCORE_API extern float clk_to_microsec;

XRCORE_API extern u64 qpc_freq;
XRCORE_API extern u64 qpc_overhead;
XRCORE_API extern u32 qpc_counter;

XRCORE_API extern _processor_info ID;
XRCORE_API extern u64 QPC();

#ifdef M_VISUAL
#ifndef _M_AMD64
#pragma warning(push)
#pragma warning(disable:4035)
IC u64 GetCLK(void)
{
    _asm _emit 0x0F;
    _asm _emit 0x31;
}
#pragma warning(pop)
#else
IC u64 GetCLK(void)
{
    return __rdtsc();
}
#endif
#endif

#ifdef M_BORLAND
XRCORE_API u64 __fastcall GetCLK (void);
#endif
};

XRCORE_API void _initialize_cpu();
XRCORE_API void set_current_thread_name(const char* threadName);
XRCORE_API void set_thread_name(const char* threadName, std::thread& thread);
