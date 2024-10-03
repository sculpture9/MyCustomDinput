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
/// <param name="exe_handle">: Instance handle to the EXE/DLL is creating the DirectInput object.</param>
/// <param name="version">: Number of DirectInput for which the application is designed.</param>
/// <param name="r_iid">: Unique identifier of the desired interface.</param>
/// <param name="ppvOut">: Address of a pointer to a variable to receive the interface pointer if the call succeeds.</param>
/// <param name="punkOuter">: Pointer to the address of the controlling object's IUnknown interface for COM aggregation.</param>
/// <returns></returns>
EXTERN_C DINPUT8_EXPORT HRESULT WINAPI DirectInput8Create(HINSTANCE exe_handle, DWORD version, const IID &r_iid, LPVOID *ppvOut, LPUNKNOWN punkOuter);

/// <summary> Initialize original dinput8 to ensure normal operation</summary>/// <returns></returns>
VOID InitOriginalDinput8();