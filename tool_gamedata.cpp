#include "tool_gamedata.h"
#include "tool_log.h"
#include <algorithm>
using namespace std;

map<DWORD, DWORD> font_psp_map;
map<DWORD, DWORD> font_dia_map;
void InitINIFileData()
{
    long duplicateKeys1 = 0;
    long duplicateKeys2 = 0;
    NewMapFromINI(font_psp_map, YS1_FONT_INI, duplicateKeys1);
    NewMapFromINI(font_dia_map, YS1_FONT_DIA_INI, duplicateKeys2, 2);
    if (duplicateKeys1 != 0 || duplicateKeys2 != 0)
    {
        Log("\ntext.ini have duplicate keys: " + to_string(duplicateKeys1) + " pieces !!!");
        Log("text2.ini have duplicate keys: " + to_string(duplicateKeys2) + " pieces !!!");
    }
    Log("\nSuccessfully Initialize Data of text.ini and text2.ini.");
}

BOOL NewMapFromINI(map<DWORD, DWORD> &fs_map, const LPCSTR &iniPath, long &duplicateKeys, int mapId)
{
    if (fs_map.size() != 0)
    {
        string iniF = mapId == 0 ? YS_FONT_SYTLE_PSP : YS_FONT_SYTLE_DIA;
        Log(iniF + " Data Have Been Already Init !");
        return FALSE;
    }

    std::ifstream readFile;
    readFile.open(iniPath);
    if (!readFile.is_open())
    {
        string iniF = mapId == 0 ? YS_FONT_SYTLE_PSP : YS_FONT_SYTLE_DIA;
        iniF = "Can't Open " + iniF;
        MessageBoxA(NULL, iniF.c_str(), NULL, 0);
        readFile.close();
        return FALSE;
    }

    //Read data from ini
    std::string line;
    vector<string> result;
    long dk = 0;
    while (std::getline(readFile, line))
    {
        result.clear();
        //std::Log << line << std::endl;
        //there only two string in one line.
        if (StringSplit(line, YS1_INI_SPLIT, result))
        {
            DWORD key = atoi(result[0].c_str());
            DWORD kvalue = atoi(result[1].c_str());
            //Do not add ascii key
            if (key >= ASCIIKeyBegin && key <= ASCIIKeyEnd)
            {
                continue;
            }
            BOOL bret = MapInsert(fs_map, key, kvalue);
            if (!bret)
            {
                dk++;
            }
        }
    }
    duplicateKeys = dk;
    readFile.close();
    return TRUE;
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

BOOL GetYS1TextVO(const vector<vector<string>> &csvData, vector<YS1TextVO> &result)
{
    if (csvData.size() == 0) return FALSE;

    YS1TextVO ysTVO;
    for (int i = 0; i < csvData.size(); i++)
    {
        vector<string> tempLine = csvData[i];
        //We stipulate that CSV has 7 columns
        if (tempLine.size() <  YS_CSV_COL_NUM)
        {
            return FALSE;
        }
        //id, origintxt, translatedTxt, tsize, charsize, address, addressUsedByCaller
        //id belong to type: int
        ysTVO.ID = atoi(tempLine[0].c_str());
        //origintext belong to type: string
        ysTVO.OriginTxt = tempLine[1];
        //translatedtext belong to type: string
        ysTVO.TranslatedTxt = tempLine[2];
        //font style belong to type: string
        ysTVO.FontStyle = tempLine[3];
        //tsize belong to int
        ysTVO.TSize = atoi(tempLine[4].c_str());
        //charsize belong to int
        ysTVO.CharSize = atoi(tempLine[5].c_str());
        //we have already converted the address to decimal
        ysTVO.Address = atoi(tempLine[6].c_str());
        ysTVO.AddressUsedByCaller = -1;
        if (tempLine.size() == YS_CSV_COL_NUM_WITH_EXPANDADDRESS)
        {
            ysTVO.AddressUsedByCaller = atoi(tempLine[7].c_str());
        }
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

vector<BYTE> GetCustomBytesFromText(const LPCSTR &text, string fontStyle, long &noConvertedChar)
{
    wstring strUni;
    vector<BYTE> result;
    Utf82Unicode(text, strUni);
    int uniSize = wcslen(strUni.c_str());  //in unicode, the number of chinese word equal size
    long ncc = 0;
    EFontStyle fstyle;
    if (fontStyle == YS_FONT_SYTLE_PSP) { fstyle = EFSPSP; }
    else if (fontStyle == YS_FONT_SYTLE_DIA) { fstyle = EFSDIA; }
    else { fstyle = EFSDIA; }

    //add text bytes to vector
    for (int i = 0; i < uniSize; i++)
    {
        wchar_t wChar = strUni[i];
        long nccFlag = 0;
        PushWCharToBytes(wChar, fstyle, result, nccFlag);
        ncc += nccFlag;
    }
    noConvertedChar = ncc;
    return result;
}

int PushWCharToBytes(wchar_t wchar, int fontStyle, vector<BYTE> &store, long &noConvertedChar)
{
    const wstring wcstr = {wchar};
    string charStr;
    int charSize;
    int charCode = (int)wchar;
    long pushByteCounter;
    long ncc = 0;
    bool asciiKeyFlag = charCode >= ASCIIKeyBegin && charCode <= ASCIIKeyEnd;
    if (!asciiKeyFlag)
    {
        //unicode == utf32
        //if utf32 code is the key of INI file
        charCode = GetChar32WithStyle(charCode, fontStyle, charSize);
        //or not
        if (charSize == -1)
        {
            //Show error log
            string fontStyleTxt = (EFontStyle)fontStyle == EFSPSP ? YS_FONT_SYTLE_PSP : YS_FONT_SYTLE_DIA;
            string errorChar;
            Unicode2Custom(wcstr, errorChar, CP_ACP);
            Log("\nThe char: \"" + errorChar + "\", Unicode: \"" + to_string(charCode) + "\" is not in " + fontStyleTxt);
            Log(". Program will use utf-8 code!!!");
            //use utf8 code
            charStr = "";
            Unicode2Custom(wcstr, charStr, YS_UTF8);
            charSize = charStr.length();
            charCode = Char2Code(charStr);
            ncc++;
        }
    }
    else { charSize = 1; }

    vector<BYTE> c32Bytes = Int2BytesBigEndian(charCode, charSize);
    for (auto b : c32Bytes)
    {
        store.push_back(b);
    }
    noConvertedChar = ncc;
    return charSize;
}

int GetChar32WithStyle(int charCode, int fontStyle, int &changedSize)
{
    int result = charCode;
    switch (fontStyle)
    {
    case EFSPSP:
        if (font_psp_map.find(charCode) != font_psp_map.end())
        {
            result = font_psp_map[charCode];
            changedSize = 2; //one unicode use two bytes
        }
        else
        {
            changedSize = -1;
        }
        break;
    case EFSDIA:
        if (font_dia_map.find(charCode) != font_dia_map.end())
        {
            result = font_dia_map[charCode];
            changedSize = 2; //one unicode use two bytes
        }
        else
        {
            changedSize = -1;
        }
        break;
    default:
        changedSize = -1;
        break;
    }
    return result;
}

long Char2Code(const string &charStr)
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
/// Big Endian
/// </summary>
/// <param name="code"></param>
/// <param name="byteSize"></param>
/// <returns></returns>
vector<BYTE> Int2BytesBigEndian(int code, int byteSize)
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

