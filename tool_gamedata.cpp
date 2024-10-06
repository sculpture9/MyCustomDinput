#include "tool_gamedata.h"
#include <algorithm>
using namespace std;

map<DWORD, DWORD> m_iniMap;
void InitINIFileData()
{
    NewMapFromINI(YS1_FONT_DIA_INI);
}

BOOL NewMapFromINI(const LPCSTR &iniPath)
{
    if (m_iniMap.size() != 0)
    {
        MessageBoxA(NULL, "Map Already Init", NULL, 0);
        return FALSE;
    }

    std::ifstream readFile;
    readFile.open(iniPath);
    if (!readFile.is_open())
    {
        MessageBoxA(NULL, "Can't Open .INI File", NULL, 0);
        readFile.close();
        return FALSE;
    }

    //Read data from ini
    std::string line;
    vector<string> result;
    while (std::getline(readFile, line))
    {
        result.clear();
        std::cout << line << std::endl;
        //there only two string in one line.
        if (StringSplit(line, YS1_INI_SPLIT, result))
        {
            DWORD key = atoi(result[0].c_str());
            DWORD kvalue = atoi(result[1].c_str());
            BOOL bret = MapInsert(m_iniMap, key, kvalue);
            if (!bret)
            {
                readFile.close();
                return FALSE;
            }
        }
    }
    readFile.close();
    return FALSE;
}

BOOL MapInsert(map<DWORD, DWORD> &m_map, DWORD key, DWORD kvalue)
{
    //insert when no key in map
    if (m_map.find(key) == m_map.end())
    {
        m_map.insert(pair<DWORD, DWORD>(key, kvalue));
        return TRUE;
    }
    return FALSE;
}

BOOL StringSplit(const string &str, const string &splitStr, vector<string> &result)
{
    if (str == "") return FALSE;
    string temp = str + splitStr;
    size_t sspos = temp.find(splitStr);
    size_t ssl = splitStr.size();
    while (sspos != string::npos)
    {
        string res = temp.substr(0, sspos);
        result.push_back(res);
        temp = temp.substr(sspos + ssl, temp.size());
        sspos = temp.find(splitStr);
    }
}

BOOL GetYS1TextVO(const vector<vector<string>> &csvData, vector<YS1TextValueObject> &result)
{
    if (csvData.size() == 0) return FALSE;

    YS1TextValueObject ysTVO;
    for (int i = 0; i < csvData.size(); i++)
    {
        vector<string> tempLine = csvData[i];
        //We stipulate that CSV has 6 columns
        //id, origintxt, translatedTxt, tsize, charsize, address
        //id belong to type: int
        ysTVO.ID = atoi(tempLine[0].c_str());
        //origintext belong to type: string
        ysTVO.OriginTxt = tempLine[1];
        //translatedtext belong to type: wstring
        ysTVO.TranslatedTxt = tempLine[2];
        //tsize belong to int
        ysTVO.TSize = atoi(tempLine[3].c_str());
        //charsize belong to int
        ysTVO.CharSize = atoi(tempLine[4].c_str());
        //we have already converted the address to decimal
        ysTVO.AddressInYS1 = atoi(tempLine[5].c_str());
        //address belong to hex
        //ysTVO.AddressInYS1 = strtol(tempLine[5].c_str(), NULL, 16);
        result[i] = ysTVO;
    }
    return FALSE;
}

bool Utf82Unicode(const string &ori, wstring &wstr)
{
    int strLength = ori.length();
    int wstrLength = MultiByteToWideChar(YS_UTF8, 0, ori.data(), strLength, nullptr, 0);
    wstring temp;
    temp.resize(wstrLength);
    MultiByteToWideChar(YS_UTF8, 0, ori.data(), strLength, &temp[0], wstrLength);
    wstr = temp;
    return true;
}

bool Unicode2Custom(const wstring &strUnicode, string &strTgt, unsigned int codePage)
{
    int wstrLength = strUnicode.length();
    int strLength = WideCharToMultiByte(codePage, 0, strUnicode.data(), wstrLength, nullptr, 0, nullptr, nullptr);
    string temp;
    temp.resize(strLength);
    WideCharToMultiByte(codePage, 0, strUnicode.data(), wstrLength, &temp[0], strLength, nullptr, nullptr);
    strTgt = temp;
    int fff = strTgt.length();
    return true;
}

vector<BYTE> GetCustomBytesFromText(const LPCSTR &test, DWORD charCount)
{
    wstring strUni;
    vector<BYTE> result;
    int usedBytes = 0;
    Utf82Unicode(test, strUni);
    int uniSize = wcslen(strUni.c_str());  //in unicode, the number of chinese word equal size
    //if the translated text size out of original game text size
    //if (offset < 0)  
    //{
    //    return result;
    //}

    //add text bytes to vector
    for (int i = 0; i < uniSize; i++)
    {
        wchar_t wChar = strUni[i];
        usedBytes += PushWCharToByteVector(wChar, result);
    }
    int offset = charCount - usedBytes;  //one unicode char have two bytes
    //fill in zeros to ensure that the bytes is the same as the original game text
    if (offset > 0)
    {
        BYTE zero = Int2Bytes(Char2Code("\0", 1), 1)[0];
        for (int i = 0; i < offset; i++)
        {
            result.push_back(zero);
        }
    }
    return result;
}

int PushWCharToByteVector(wchar_t wchar, vector<BYTE> &store)
{
    const wstring wcstr = {wchar};
    string charStr;
    int charStrSize;
    int charCode = (int)wchar;
    long pushByteCounter;
    //unicode == utf32
    //if utf32 code is the key of INI file
    if (m_iniMap.find(charCode) != m_iniMap.end())
    {
        charCode = m_iniMap[charCode];
        charStrSize = 2;  //one unicode use two bytes
    }
    else
    {
        //use utf8 code
        charStr = "";
        Unicode2Custom(wcstr, charStr, YS_UTF8);
        charStrSize = charStr.length();
        charCode = Char2Code(charStr, charStrSize);
    }
    vector<BYTE> c32Bytes = Int2Bytes(charCode, charStrSize);
    for (auto b : c32Bytes)
    {
        store.push_back(b);
    }
    return charStrSize;
}

long Char2Code(const string &charStr, int charSize)
{
    string hexStr;
    const unsigned char *pstr32 = (unsigned char *)charStr.c_str();  //utf8/utf32 code is unsigned type
    int char32Size = charStr.length();
    for (int i = 0; i < char32Size; i++)
    {
        int b = *(pstr32 + i);
        stringstream ss;
        ss << hex << b;
        hexStr += ss.str();
        ss.clear();
    }
    long c32Code = strtol(hexStr.c_str(), NULL, 16);
    return c32Code;
}

/// <summary>
/// YS1 use Big Endian
/// </summary>
/// <param name="code"></param>
/// <param name="byteSize"></param>
/// <returns></returns>
vector<BYTE> Int2Bytes(int code, int byteSize)
{
    vector<BYTE> result;
    for (int i = 0; i < byteSize; i++)
    {
        BYTE b = (BYTE)((code >> (i * 8)) & 0xFF);
        result.push_back(b);
    }
    reverse(result.begin(), result.end());
    return result;
}

