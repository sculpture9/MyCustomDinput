#pragma once
#include "framework.h"
#include "csv_reader.h"
#include "tool_gamedata.h"
#include <string>

void InitTrainer();

BOOL Translate();

PVOID GetBaseAddressByPID(DWORD pid);

PVOID GetBaseAddressByHandle(HANDLE hprocess);

BOOL TranslateAllText(vector<YS1TextValueObject> list);

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress);