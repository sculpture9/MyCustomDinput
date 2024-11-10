#pragma once
#include "framework.h"
#include "tool_gamedata.h"
#include <vector>
#include <string>

BOOL InitTrainer();

BOOL Translate();

PVOID GetBaseAddressByPID(DWORD pid);

PVOID GetBaseAddressByHandle(HANDLE hprocess);

DWORD TranslateAllText(const std::vector<YS1TextVO> &list, long &noConvertedChar);

BOOL WriteBytes2GameByOriginal(std::vector<BYTE> bytes, const YS1TextVO &vo);

BOOL WriteBytesCollection2GameByExpanded(std::vector<std::vector<BYTE>> bytesCollection, const std::vector<YS1TextVO> &vos, DWORD &noConvertedLine);

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress);

BOOL AllocCustomConsole();

BOOL FreeCustomConsole();

size_t SizeOfBytesCollection(std::vector<std::vector<BYTE>> bytesCollection);

BOOL Malloc4BytesHeap(size_t size);

BOOL Add2BytesHeap(BYTE date, size_t pos);

int AddressOfBytesHeap(size_t pos);

void FreeBytesHeap();