#pragma once

#include "framework.h"
#include "Unknwn.h"

#ifdef  DINPUT8_EXPORTS
#define DINPUT8_EXPORT __declspec(dllexport)
#else
#define DINPUT8_EXPORT __declspec(dllimport)
#endif //  DINPUT8_EXPORT

//A function pointer used to receive the return of GetProcAddress, then it was called by DirectInput8Create
typedef HRESULT(WINAPI *CustomDirectInput8Create)(HINSTANCE, DWORD, const IID &, LPVOID *, LPUNKNOWN);
CustomDirectInput8Create curDirectInput8Create = NULL;

/// <summary>
/// the injected program will call method of the same name in dinput8.dll.
/// so we need to override it.
/// </summary>
/// <param name="inst_handle"></param>
/// <param name="version"></param>
/// <param name="r_iid"></param>
/// <param name="out_wrapper"></param>
/// <param name="p_unk"></param>
/// <returns></returns>
EXTERN_C DINPUT8_EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE inst_handle, DWORD version, const IID &r_iid, LPVOID *out_wrapper, LPUNKNOWN p_unk);

/// <summary> Initialize original dinput8 to ensure normal operation</summary>/// <returns></returns>
VOID InitOriginalDinput8();