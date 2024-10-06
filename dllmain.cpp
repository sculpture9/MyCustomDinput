// dllmain.cpp : Defines the entry point for the DLL application.

#include "dinput8.h"

extern HANDLE m_exeProc;

VOID InitOriginalDinput8()
{
	CHAR dinput8Path[MAX_PATH] = { };
	HMODULE hModule = NULL;

	if (NULL == curDirectInput8Create)
	{
		GetSystemDirectoryA(dinput8Path, MAX_PATH);
		strcat_s(dinput8Path, "\\dinput8.dll");
		hModule = LoadLibraryA(dinput8Path);

		if (hModule > (HMODULE)31)
		{
			curDirectInput8Create = (CustomDirectInput8Create)GetProcAddress(hModule, "DirectInput8Create");
		}
	}
}

HRESULT WINAPI DirectInput8Create(HINSTANCE exe_handle, DWORD version, const IID &r_iid, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	if (curDirectInput8Create)
	{
		return curDirectInput8Create(exe_handle, version, r_iid, ppvOut, punkOuter);
	}

	return S_OK;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        InitOriginalDinput8();
		MessageBoxA(NULL, "Successfully run my dinput8.dll.", "Mission Complete !", 0);
		Translate();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


