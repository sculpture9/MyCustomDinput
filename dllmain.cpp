// dllmain.cpp : Defines the entry point for the DLL application.

#include "dinput8.h"
#include "trainer.h"
#include "tool_gamedata.h"
#include "conio.h"

extern HANDLE m_exeProc;
extern PVOID m_baseAddress;
BYTE *m_expandedBytes;
size_t expandedBytesSize;

HRESULT WINAPI DirectInput8Create(HINSTANCE exe_handle, DWORD version, const IID &r_iid, LPVOID *ppvOut, LPUNKNOWN punkOuter)
{
	if (curDirectInput8Create)
	{
		return curDirectInput8Create(exe_handle, version, r_iid, ppvOut, punkOuter);
	}
	return S_OK;
}

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

void InstallTranslation()
{
	AllocCustomConsole();
	std::cout << "Successfully Run Translation dinput8.dll. " << std::endl;
	InitINIFileData();
	Translate();
	std::cout << "\nInput any key to start game." << std::endl;
	_getch();
	FreeCustomConsole();
}

void UninstallTranslation()
{
	FreeBytesHeap();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	int s = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		InitOriginalDinput8();
		InstallTranslation();
		break;
    case DLL_THREAD_ATTACH:
		break;
    case DLL_THREAD_DETACH:
		break;
    case DLL_PROCESS_DETACH:
		UninstallTranslation();
        break;
    }
    return TRUE;
}


