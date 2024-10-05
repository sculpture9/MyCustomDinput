#pragma once
#include "framework.h"
#include "sstream"
#include "iostream"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

using namespace std;

typedef struct YS1TextValueObject
{
    DWORD ID;
    LPCSTR OriginTxt;
    LPCSTR TranslatedTxt;
    DWORD TSize;
    DWORD CharSize;
    DWORD AddressInYS1;
};

void Init();

BOOL NewMapFromINI(const LPCSTR &iniPath);

vector<YS1TextValueObject> GetYS1TextVO(const vector<vector<string>> &csvData);

vector<BYTE> NewBytesFromText(const LPCSTR &test, DWORD charCount);

BOOL MapInsert(map<DWORD, DWORD> &m_map, DWORD key, DWORD kvalue);

vector<YS1TextValueObject> GetData();

BOOL StringSplit(const string &str, const string &splitStr, vector<string> &result);

/// <summary>
/// string convert to wstring
/// </summary>
/// <param name="ori"></param>
/// <returns></returns>
bool Utf82Unicode(const LPCSTR &str8, LPCWSTR &strUnicode);

bool Unicode2Custom(const LPCWSTR &strUnicode, LPCSTR &strTgt, unsigned int codePage);

vector<BYTE> Int2Bytes(int code, int byteSize);

long Char2Code(const LPCSTR &charStr, int charSize);