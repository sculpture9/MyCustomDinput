#pragma once
#include "framework.h"

void InitTrainer();

BOOL Translate();

PVOID GetBaseAddressByPID(DWORD pid);

PVOID GetBaseAddressByHandle(HANDLE hprocess);

BOOL TranslateText(LPCSTR text, PVOID baseAddress);