/*
	This file was generated by 'wrappit' [1], with the original GTA2
	dmavideo.dll as input - and then customized (mostly added comments).
	
	What goes on here is basically the original dmavideo.dll (renamed to
	dmavideo_original.dll) gets loaded and all functions get 'proxied'
	through (assembly jump preserves the stack and everything!).
	On the attach event, we launch the function input_server (see
	input_server.cpp) as thread.
		
	Note: dmavideo.dll gets attached, when GTA2.exe (and also the
		gta2_manager.exe, be careful!) starts and doesn't get deattached,
		before the exe-file quits. That way it is perfect as gateway
		for custom controller code.

	[1]: http://www.codeproject.com/Articles/16541/Create-your-Proxy-DLLs-automatically
*/

#include <windows.h>
#include <process.h>

// Speed up the build process
// http://stackoverflow.com/a/11040290
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

// set package alignment to one byte
// this probably needed to make the
// proxied functions work
#pragma pack(1)

// TCP server code:
#include "input_server.h"

HINSTANCE hLThis = 0;
HINSTANCE hL = 0;
FARPROC p[22] = {0};

BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		// Load the real dmavideo.dll library (which must be named dmavideo_original.dll now)
		hLThis = hInst;
		hL = LoadLibrary("dmavideo_original.dll");
		if (!hL) return false;

		// Get all function addresses in that DLL-file
		p[0] = GetProcAddress(hL,"Vid_CheckMode");
		p[1] = GetProcAddress(hL,"Vid_ClearScreen");
		p[2] = GetProcAddress(hL,"Vid_CloseScreen");
		p[3] = GetProcAddress(hL,"Vid_DisableWrites");
		p[4] = GetProcAddress(hL,"Vid_EnableWrites");
		p[5] = GetProcAddress(hL,"Vid_FindDevice");
		p[6] = GetProcAddress(hL,"Vid_FindFirstMode");
		p[7] = GetProcAddress(hL,"Vid_FindMode");
		p[8] = GetProcAddress(hL,"Vid_FindNextMode");
		p[9] = GetProcAddress(hL,"Vid_FlipBuffers");
		p[10] = GetProcAddress(hL,"Vid_FreeSurface");
		p[11] = GetProcAddress(hL,"Vid_GetSurface");
		p[12] = GetProcAddress(hL,"Vid_GetVersion");
		p[13] = GetProcAddress(hL,"Vid_GrabSurface");
		p[14] = GetProcAddress(hL,"Vid_InitDLL");
		p[15] = GetProcAddress(hL,"Vid_Init_SYS");
		p[16] = GetProcAddress(hL,"Vid_ReleaseSurface");
		p[17] = GetProcAddress(hL,"Vid_SetDevice");
		p[18] = GetProcAddress(hL,"Vid_SetGamma");
		p[19] = GetProcAddress(hL,"Vid_SetMode");
		p[20] = GetProcAddress(hL,"Vid_ShutDown_SYS");
		p[21] = GetProcAddress(hL,"Vid_WindowProc");

		// Show a debug message
		MessageBox(NULL, "DLL Attached!", "Hello!", NULL);
	
		// Start the input TCP server
		_beginthread(input_server,0,NULL);
	}
	if (reason == DLL_PROCESS_DETACH)
	{
		// Unload dmavideo_original.dll
		FreeLibrary(hL);
	}

	return 1;
}

/*
	The code below is inline assembly that redirects all
	function calls to the same functions of the original
	dll (which gets loaded, as soon as this DLL gets
	attached). The assembly jump makes sure, that the
	full stack gets preserved, just as if the original
	function was called.
*/

// Vid_CheckMode
extern "C" __declspec(naked) void __stdcall __E__0__()
	{
	__asm
		{
		jmp p[0*4];
		}
	}

// Vid_ClearScreen
extern "C" __declspec(naked) void __stdcall __E__1__()
	{
	__asm
		{
		jmp p[1*4];
		}
	}

// Vid_CloseScreen
extern "C" __declspec(naked) void __stdcall __E__2__()
	{
	__asm
		{
		jmp p[2*4];
		}
	}

// Vid_DisableWrites
extern "C" __declspec(naked) void __stdcall __E__3__()
	{
	__asm
		{
		jmp p[3*4];
		}
	}

// Vid_EnableWrites
extern "C" __declspec(naked) void __stdcall __E__4__()
	{
	__asm
		{
		jmp p[4*4];
		}
	}

// Vid_FindDevice
extern "C" __declspec(naked) void __stdcall __E__5__()
	{
	__asm
		{
		jmp p[5*4];
		}
	}

// Vid_FindFirstMode
extern "C" __declspec(naked) void __stdcall __E__6__()
	{
	__asm
		{
		jmp p[6*4];
		}
	}

// Vid_FindMode
extern "C" __declspec(naked) void __stdcall __E__7__()
	{
	__asm
		{
		jmp p[7*4];
		}
	}

// Vid_FindNextMode
extern "C" __declspec(naked) void __stdcall __E__8__()
	{
	__asm
		{
		jmp p[8*4];
		}
	}

// Vid_FlipBuffers
extern "C" __declspec(naked) void __stdcall __E__9__()
	{
	__asm
		{
		jmp p[9*4];
		}
	}

// Vid_FreeSurface
extern "C" __declspec(naked) void __stdcall __E__10__()
	{
	__asm
		{
		jmp p[10*4];
		}
	}

// Vid_GetSurface
extern "C" __declspec(naked) void __stdcall __E__11__()
	{
	__asm
		{
		jmp p[11*4];
		}
	}

// Vid_GetVersion
extern "C" __declspec(naked) void __stdcall __E__12__()
	{
	__asm
		{
		jmp p[12*4];
		}
	}

// Vid_GrabSurface
extern "C" __declspec(naked) void __stdcall __E__13__()
	{
	__asm
		{
		jmp p[13*4];
		}
	}

// Vid_InitDLL
extern "C" __declspec(naked) void __stdcall __E__14__()
	{
	__asm
		{
		jmp p[14*4];
		}
	}

// Vid_Init_SYS
extern "C" __declspec(naked) void __stdcall __E__15__()
	{
	__asm
		{
		jmp p[15*4];
		}
	}

// Vid_ReleaseSurface
extern "C" __declspec(naked) void __stdcall __E__16__()
	{
	__asm
		{
		jmp p[16*4];
		}
	}

// Vid_SetDevice
extern "C" __declspec(naked) void __stdcall __E__17__()
	{
	__asm
		{
		jmp p[17*4];
		}
	}

// Vid_SetGamma
extern "C" __declspec(naked) void __stdcall __E__18__()
	{
	__asm
		{
		jmp p[18*4];
		}
	}

// Vid_SetMode
extern "C" __declspec(naked) void __stdcall __E__19__()
	{
	__asm
		{
		jmp p[19*4];
		}
	}

// Vid_ShutDown_SYS
extern "C" __declspec(naked) void __stdcall __E__20__()
	{
	__asm
		{
		jmp p[20*4];
		}
	}

// Vid_WindowProc
extern "C" __declspec(naked) void __stdcall __E__21__()
	{
	__asm
		{
		jmp p[21*4];
		}
	}

