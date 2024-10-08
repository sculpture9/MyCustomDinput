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
    std::string FontStyle;
    long TSize;
    long CharSize;
    long AddressInYS1;
};

enum EFontStyle
{
    EFSPSP = 1,
    EFSDIA = 2
};

void InitINIFileData();

BOOL NewMapFromINI(std::map<DWORD, DWORD> &fs_map, const LPCSTR &iniPath, int mapId = 0);

BOOL GetYS1TextVO(const std::vector<std::vector<std::string>> &csvData, std::vector<YS1TextValueObject> &result);

std::vector<BYTE> GetCustomBytesFromText(const LPCSTR &test, std::string fontStyle, DWORD charCount);

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

int PushWCharToByteVector(wchar_t wchar, int fontStyle, std::vector<BYTE> &store);

int GetChar32WithStyle(int charCode, int fontStyle, int &changedSize);