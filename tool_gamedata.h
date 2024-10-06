#pragma once
#include "framework.h"
#include "sstream"
#include "iostream"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cwchar>

typedef struct YS1TextValueObject
{
    long ID;
    std::string OriginTxt;
    std::string TranslatedTxt;
    long TSize;
    long CharSize;
    long AddressInYS1;
};

void InitINIFileData();

BOOL NewMapFromINI(const LPCSTR &iniPath);

BOOL GetYS1TextVO(const std::vector<std::vector<std::string>> &csvData, std::vector<YS1TextValueObject> &result);

std::vector<BYTE> GetCustomBytesFromText(const LPCSTR &test, DWORD charCount);

BOOL MapInsert(std::map<DWORD, DWORD> &m_map, DWORD key, DWORD kvalue);

BOOL StringSplit(const std::string &str, const std::string &splitStr, std::vector<std::string> &result);

/// <summary>
/// string convert to wstring
/// </summary>
/// <param name="ori"></param>
/// <returns></returns>
bool Utf82Unicode(const std::string &str8, std::wstring &strUnicode);

bool Unicode2Custom(const std::wstring &strUnicode, std::string &strTgt, unsigned int codePage);

std::vector<BYTE> Int2Bytes(int code, int byteSize);

long Char2Code(const std::string &charStr, int charSize);

int PushWCharToByteVector(wchar_t wchar, std::vector<BYTE> &store);