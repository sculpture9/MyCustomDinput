#include "tool_gamedata.h"

std::map<DWORD, DWORD> m_iniMap;
void Init()
{
    BOOL isInit = NewMapFromINI(YS1_TEXT_INI);
    if (!isInit)
    {
        //TODO
    }
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
    while (sspos != temp.npos)
    {
        string res = temp.substr(0, sspos);
        result.push_back(res);
        temp.substr(sspos + ssl, temp.size());
        sspos = temp.find(splitStr);
    }
}

vector<YS1TextValueObject> GetYS1TextVO(const vector<vector<string>> &csvData)
{
    vector<YS1TextValueObject> result;
    for (int i = 0; i < csvData.size(); i++)
    {
        YS1TextValueObject ysTVO;
        vector<string> tempLine = csvData[i];
        string idStr, address, tsize, charSize, oriText;
        //We stipulate that CSV has 6 columns
        //id, origintxt, translatedTxt, tsize, charsize, address
        //id belong to type: int
        //wstr2str(tempLine[0], idStr);
        ysTVO.ID = atoi(idStr.c_str());
        //origintext belong to type: string
        //wstr2str(tempLine[1], oriText);
        ysTVO.OriginTxt = oriText.c_str();
        //translatedtext belong to type: wstring
        ysTVO.TranslatedTxt = tempLine[2].c_str();
        //tsize belong to int
        //wstr2str(tempLine[3], tsize);
        ysTVO.TSize = atoi(tsize.c_str());
        //charsize belong to int
        //wstr2str(tempLine[4], charSize);
        ysTVO.CharSize = atoi(charSize.c_str());
        //address belong to int
        //wstr2str(tempLine[5], address);
        ysTVO.AddressInYS1 = atoi(address.c_str());

        result.push_back(ysTVO);
    }
    return result;
}

bool Utf82Unicode(const LPCSTR &ori, LPCWSTR &wstr)
{
    int strLength = strlen(ori);
    int wstrLength = MultiByteToWideChar(YS_UTF8, 0, ori, strLength, nullptr, 0);
    wchar_t *temp = new wchar_t[wstrLength];
    MultiByteToWideChar(YS_UTF8, 0, ori, strLength, temp, wstrLength);
    LPCWSTR result(temp);
    wstr = result;
    delete[](temp);
    return true;
}

bool Unicode2Custom(const LPCWSTR &strUnicode, LPCSTR &strTgt, unsigned int codePage)
{
    int wstrLength = wcslen(strUnicode);
    int strLength = WideCharToMultiByte(codePage, 0, strUnicode, wstrLength, nullptr, 0, nullptr, nullptr);
    char *temp = new char[strLength];
    WideCharToMultiByte(codePage, 0, strUnicode, wstrLength, temp, strLength, nullptr, nullptr);
    //temp[strLength] = '\0';
    LPCSTR result(temp);
    strTgt = result;
    delete[] temp;
    return true;
}

vector<BYTE> NewBytesFromText(const LPCSTR &test, DWORD charCount)
{
    LPCSTR str32;
    LPCWSTR strUni;
    vector<BYTE> result;
    Utf82Unicode(test, strUni);
    int uniSize = wcslen(strUni);  //in unicode, the number of chinese word equal size
    for (int i = 0; i < uniSize; i++)
    {
        wchar_t s = strUni[i];
    }

    const unsigned char *pstr32 = (unsigned char *)strUni;  //utf8 code is unsigned type
    int testSize = strlen(test);
    for (int i = 0; i < testSize; i++)
    {

    }

    BYTE *b = new BYTE[15]{ 0xe5, 0x8c, 0xbb, 0xe7,0x94,0x9f, 0xe5,0xb8, 0x83, 0xe9 ,0xb2, 0x81, 0xe5, 0xa4,0x9a };
    return result;
}

BOOL PushBytesWithCustom(wchar_t wchar, vector<BYTE> &store)
{
    const wstring wcstr = {wchar};
    LPCSTR char32;
    string strHex;
    Unicode2Custom(wcstr.c_str(), char32, YS_UTF32);
    int char32Size = strlen(char32);
    int c32Code = Char2Code(char32, char32Size);
    //if c32Code is the key of INI file
    if (m_iniMap.find(c32Code) != m_iniMap.end())
    {
        c32Code = m_iniMap[c32Code];
        vector<BYTE> c32Bytes = Int2Bytes(c32Code, char32Size);
        for (auto b : c32Bytes)
        {
            store.push_back(b);
        }
    }
    return true;
}

long Char2Code(const LPCSTR &charStr, int charSize)
{
    string hexStr;
    const unsigned char *pstr32 = (unsigned char *)charStr;  //utf32 code is unsigned type
    int char32Size = strlen(charStr);
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
/// use Little Endian
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
    return result;
}

//test
vector<YS1TextValueObject>GetData()
{
    YS1TextValueObject ys = { 1, NULL, "布鲁多医师", 13, 1, 0x004DBBE4};
    YS1TextValueObject ys2 = { 1, NULL, "护士艾拉", 11, 1, 0x004DBBF4};
    YS1TextValueObject ys3 = { 1, NULL, "斯拉夫", 11, 1, 0x004DBC00};
    vector<YS1TextValueObject> yslist;
    yslist.push_back(ys);
    yslist.push_back(ys2);
    yslist.push_back(ys3);
    return yslist;
}

