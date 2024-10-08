#pragma once
#include "framework.h"
#include "tool_gamedata.h"
#include <vector>
#include <string>
#include <vector>

void InitTrainer();

BOOL Translate();

PVOID GetBaseAddressByPID(DWORD pid);

PVOID GetBaseAddressByHandle(HANDLE hprocess);

DWORD TranslateAllText(const std::vector<YS1TextValueObject> &list, long &noConvertedChar);

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress);

BOOL AllocCustomConsole();

BOOL FreeCustomConsole();