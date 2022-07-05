#include "stdafx.h"

#include <VersionHelpers.h>
#include <process.h>

// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>

// Initialized on startup
XRCORE_API Fmatrix Fidentity;
XRCORE_API Dmatrix Didentity;
XRCORE_API CRandom Random;

#ifdef _M_AMD64
u16 getFPUsw() { return 0; }

namespace FPU
{
XRCORE_API void m24(void)
{
#ifdef _M_IX86
    _control87(_PC_24, MCW_PC);
#endif
    _control87(_RC_CHOP, MCW_RC);
}
XRCORE_API void m24r(void)
{
#ifdef _M_IX86
    _control87(_PC_24, MCW_PC);
#endif
    _control87(_RC_NEAR, MCW_RC);
}
XRCORE_API void m53(void)
{
#ifdef _M_IX86
    _control87(_PC_53, MCW_PC);
#endif
    _control87(_RC_CHOP, MCW_RC);
}
XRCORE_API void m53r(void)
{
#ifdef _M_IX86
    _control87(_PC_53, MCW_PC);
#endif
    _control87(_RC_NEAR, MCW_RC);
}
XRCORE_API void m64(void)
{
#ifdef _M_IX86
    _control87(_PC_64, MCW_PC);
#endif
    _control87(_RC_CHOP, MCW_RC);
}
XRCORE_API void m64r(void)
{
#ifdef _M_IX86
    _control87(_PC_64, MCW_PC);
#endif
    _control87(_RC_NEAR, MCW_RC);
}

void initialize() {}
};
#else
u16 getFPUsw()
{
    u16 SW;
    __asm fstcw SW;
    return SW;
}

namespace FPU
{
u16 _24 = 0;
u16 _24r = 0;
u16 _53 = 0;
u16 _53r = 0;
u16 _64 = 0;
u16 _64r = 0;

XRCORE_API void m24()
{
    u16 p = _24;
    __asm fldcw p;
}
XRCORE_API void m24r()
{
    u16 p = _24r;
    __asm fldcw p;
}
XRCORE_API void m53()
{
    u16 p = _53;
    __asm fldcw p;
}
XRCORE_API void m53r()
{
    u16 p = _53r;
    __asm fldcw p;
}
XRCORE_API void m64()
{
    u16 p = _64;
    __asm fldcw p;
}
XRCORE_API void m64r()
{
    u16 p = _64r;
    __asm fldcw p;
}

void initialize()
{
    _clear87();

    _control87(_PC_24, MCW_PC);
    _control87(_RC_CHOP, MCW_RC);
    _24 = getFPUsw(); // 24, chop
    _control87(_RC_NEAR, MCW_RC);
    _24r = getFPUsw(); // 24, rounding

    _control87(_PC_53, MCW_PC);
    _control87(_RC_CHOP, MCW_RC);
    _53 = getFPUsw(); // 53, chop
    _control87(_RC_NEAR, MCW_RC);
    _53r = getFPUsw(); // 53, rounding

    _control87(_PC_64, MCW_PC);
    _control87(_RC_CHOP, MCW_RC);
    _64 = getFPUsw(); // 64, chop
    _control87(_RC_NEAR, MCW_RC);
    _64r = getFPUsw(); // 64, rounding

#ifndef XRCORE_STATIC

    m24r();

#endif //XRCORE_STATIC

    ::Random.seed(u32(CPU::GetCLK() % (1i64 << 32i64)));
}
};
#endif

namespace CPU
{
XRCORE_API u64 clk_per_second;
XRCORE_API u64 clk_per_milisec;
XRCORE_API u64 clk_per_microsec;
XRCORE_API u64 clk_overhead;
XRCORE_API float clk_to_seconds;
XRCORE_API float clk_to_milisec;
XRCORE_API float clk_to_microsec;
XRCORE_API u64 qpc_freq = 0;
XRCORE_API u64 qpc_overhead = 0;
XRCORE_API u32 qpc_counter = 0;

XRCORE_API _processor_info ID;

XRCORE_API u64 QPC()
{
    u64 _dest;
    QueryPerformanceCounter((PLARGE_INTEGER)&_dest);
    qpc_counter++;
    return _dest;
}

#ifdef M_BORLAND
u64 __fastcall GetCLK(void)
{
    _asm db 0x0F;
    _asm db 0x31;
}
#endif

void Detect()
{
    // Timers & frequency
    u64 start, end;
    u32 dwStart, dwTest;

    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

    // Detect Freq
    dwTest = timeGetTime();
    do { dwStart = timeGetTime(); }
    while (dwTest == dwStart);
    start = GetCLK();
    while (timeGetTime() - dwStart < 1000);
    end = GetCLK();
    clk_per_second = end - start;

    // Detect RDTSC Overhead
    clk_overhead = 0;
    u64 dummy = 0;
    for (int i = 0; i < 256; i++)
    {
        start = GetCLK();
        clk_overhead += GetCLK() - start - dummy;
    }
    clk_overhead /= 256;

    // Detect QPC Overhead
    QueryPerformanceFrequency((PLARGE_INTEGER)&qpc_freq);
    qpc_overhead = 0;
    for (int i = 0; i < 256; i++)
    {
        start = QPC();
        qpc_overhead += QPC() - start - dummy;
    }
    qpc_overhead /= 256;

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    clk_per_second -= clk_overhead;
    clk_per_milisec = clk_per_second / 1000;
    clk_per_microsec = clk_per_milisec / 1000;

#ifndef _WIN64
    _control87(_PC_64, MCW_PC);
#endif
    // _control87 ( _RC_CHOP, MCW_RC );
    double a, b;
    a = 1;
    b = double(clk_per_second);
    clk_to_seconds = float(double(a / b));
    a = 1000;
    b = double(clk_per_second);
    clk_to_milisec = float(double(a / b));
    a = 1000000;
    b = double(clk_per_second);
    clk_to_microsec = float(double(a / b));
}
};

//bool g_initialize_cpu_called = false;

//------------------------------------------------------------------------------------
void _initialize_cpu()
{
    Msg("* Detected CPU: %s [%s], F%d/M%d/S%d, %.2f mhz, %d-clk 'rdtsc'",
		CPU::ID.brand, CPU::ID.vendor,
        CPU::ID.family, CPU::ID.model, CPU::ID.stepping,
        float(CPU::clk_per_second / u64(1000000)),
        u32(CPU::clk_overhead)
       );

	if (strstr(Core.Params, "-x86"))
		CPU::ID.clearFeatures();

	string256 features;
	xr_strcpy(features, sizeof(features), "RDTSC");
	if (CPU::ID.hasMMX()) xr_strcat(features, ", MMX");
	if (CPU::ID.has3DNOWExt()) xr_strcat(features, ", 3DNowExt!");
	if (CPU::ID.has3DNOW()) xr_strcat(features, ", 3DNow!");
	if (CPU::ID.hasSSE()) xr_strcat(features, ", SSE");
	if (CPU::ID.hasSSE2()) xr_strcat(features, ", SSE2");
	R_ASSERT(CPU::ID.hasSSE2());
	if (CPU::ID.hasSSE3()) xr_strcat(features, ", SSE3");
	if (CPU::ID.hasMWAIT()) xr_strcat(features, ", MONITOR/MWAIT");
	if (CPU::ID.hasSSSE3()) xr_strcat(features, ", SSSE3");
	if (CPU::ID.hasSSE41()) xr_strcat(features, ", SSE4.1");
	if (CPU::ID.hasSSE42()) xr_strcat(features, ", SSE4.2");
	if (CPU::ID.hasSSE4a()) xr_strcat(features, ", SSE4a");
	if (CPU::ID.hasAVX()) xr_strcat(features, ", AVX");
	if (CPU::ID.hasAVX2()) xr_strcat(features, ", AVX2");
	Msg("* CPU features: %s", features);
	Msg("* CPU cores: [%u], threads: [%u]", CPU::ID.coresCount, CPU::ID.threadCount);


    Fidentity.identity(); // Identity matrix
    Didentity.identity(); // Identity matrix
    pvInitializeStatics(); // Lookup table for compressed normals
    FPU::initialize();
 //   _initialize_cpu_thread();

 //   g_initialize_cpu_called = true;
}

// threading API
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO {
    DWORD dwType;
    LPCSTR szName;
    DWORD dwThreadID;
    DWORD dwFlags;
} THREADNAME_INFO;
#pragma pack(pop)

static void set_thread_name(HANDLE ThreadHandle, const char* threadName) {
	if (IsWindows10OrGreater())
	{
		static HMODULE KernelLib = GetModuleHandle("kernel32.dll");
		using FuncSetThreadDescription = HRESULT(WINAPI*)(HANDLE, PCWSTR);
		static auto pSetThreadDescription = (FuncSetThreadDescription)GetProcAddress(KernelLib, "SetThreadDescription");

		if (pSetThreadDescription)
		{
			wchar_t buf[64]{};
			mbstowcs(buf, threadName, std::size(buf));

			pSetThreadDescription(ThreadHandle, buf);

			return;
		}
		else if (!IsDebuggerPresent()) return;
	}
	else if (!IsDebuggerPresent()) return;

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = GetThreadId(ThreadHandle);
	info.dwFlags = 0;

	__try {
		constexpr DWORD MS_VC_EXCEPTION = 0x406D1388;
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

void set_current_thread_name(const char* threadName) {
	set_thread_name(GetCurrentThread(), threadName);
}

void set_thread_name(const char* threadName, std::thread& thread) {
	set_thread_name(static_cast<HANDLE>(thread.native_handle()), threadName);
}
