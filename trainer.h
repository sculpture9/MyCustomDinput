#pragma once
#include "framework.h"
#include "tool_gamedata.h"
#include <vector>
#include <string>

void InitTrainer();

BOOL Translate();

PVOID GetBaseAddressByPID(DWORD pid);

PVOID GetBaseAddressByHandle(HANDLE hprocess);

DWORD TranslateAllText(const std::vector<YS1TextVO> &list, long &noConvertedChar);

BOOL WriteBytes2OriginalAddress(std::vector<BYTE> bytes, const YS1TextVO &vo);

BOOL WriteBytesList2ExpandedAddress(std::vector<std::vector<BYTE>> bytesList, const std::vector<YS1TextVO> &vos);

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress);

BOOL AllocCustomConsole();

BOOL FreeCustomConsole();

bool Malloc4BytesHeap(size_t size);

bool Write2BytesHeap(BYTE date, size_t pos);

BYTE *BytesHeapPointer(size_t pos);

void FreeBytesHeap();