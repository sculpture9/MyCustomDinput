#include "trainer.h"
#include "csv_reader.h"

using namespace std;

HANDLE m_exeProc;
PVOID m_baseAddress;
extern BYTE *m_expandedBytes;
extern size_t expandedBytesSize;

void InitTrainer()
{
    //HANDLE openProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE |
    //                             PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD |
    //                             PROCESS_QUERY_INFORMATION, 
    //                             FALSE, GetCurrentProcessId());
    HANDLE openProc = GetCurrentProcess(); //increase performance, have PROCESS_ALL_ACCESS
    if (openProc != NULL)
    {
        m_exeProc = openProc;
        m_baseAddress = GetBaseAddressByHandle(openProc);
    }
}

PVOID GetBaseAddressByHandle(HANDLE hprocess)
{
    if (hprocess == NULL)
    {
        MessageBoxA(NULL, "Current Process Is Null", NULL, 0);
        return NULL;
    }

    PVOID baseAddress = NULL;
    //travel modules
    HMODULE hModules[100] = { 0 };
    DWORD dwRet = 0;
    BOOL bRet = EnumProcessModules(hprocess, (HMODULE *)hModules, sizeof(hModules), &dwRet);
    if (bRet == FALSE)
    {
        MessageBoxA(NULL, "EnumProcessModules Failed", NULL, 0);
    }
    else
    {
        baseAddress = hModules[0];
    }
    return baseAddress;
}

PVOID GetBaseAddressByPID(DWORD pid)
{
    PVOID baseAddress = NULL;
    MODULEENTRY32 me32 = {0};
    me32.dwSize = sizeof(MODULEENTRY32);

    //get all modules of specifie process
    HANDLE snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (snapShot == INVALID_HANDLE_VALUE)
    {
        MessageBoxA(NULL, "CreateSnapModule32 Failed", NULL, 0);
        return NULL;
    }
    //base address of process is the first module address of process
    bool ret = Module32First(snapShot, &me32);
    if (ret) baseAddress = (PVOID)me32.modBaseAddr;
    CloseHandle(snapShot);
    return baseAddress;
}

BOOL Translate()
{
    InitTrainer();
    PVOID address = GetBaseAddressByHandle(m_exeProc);
    vector<vector<string>> csvData;
    bool csvResult;
    long lineFlag;
    csvResult = ReadDataFromCSV(csvData, YS1_EXE_CSV_PATH, lineFlag);
    if (csvData.size() == 0)
    {
        csvResult = ReadDataFromCSV(csvData, YS2_EXE_CSV_PATH, lineFlag);
    }
    if (!csvResult)
    {
        if (lineFlag == -1)
        {
            cout << "\n!!! Need .CSV file!!!" << endl;
        }
        else
        {
            cout << "line: " << lineFlag << ", in .CSV file who has wrong format !!!" << endl;
        }
        return FALSE;
    }
    vector<YS1TextVO> ys1list(csvData.size());
    GetYS1TextVO(csvData, ys1list);
    long failedChar;
    DWORD failedLine = TranslateAllText(ys1list, failedChar);

    if (failedChar > 0)
    {
        cout << "\nTranslated Failed Line Number : " << failedLine << " pieces !" << endl;
        cout << "Translated Failed Char Number : " << failedChar << " pieces !!!" << endl;
        return FALSE;
    }
    return TRUE;
}

DWORD TranslateAllText(const vector<YS1TextVO> &list, long &noConvertedChar)
{
    int ys1tSize = list.size();
    if (ys1tSize == 0) return FALSE;
    DWORD noConvertdLine = 0;
    BOOL result = FALSE; 
    long ncc = 0;
    YS1TextVO temp;
    vector<vector<BYTE>> bytes2Heap;
    vector<YS1TextVO> vo2Heap;
    for (int i = 0; i < ys1tSize; i++)
    {
        temp = list[i];
        if (temp.TranslatedTxt.size() == 0 || temp.TranslatedTxt == " ") { noConvertdLine++; continue; }

        long nccFlag = 0;
        vector<BYTE> tBytes = GetCustomBytesFromText(temp.TranslatedTxt.c_str(), temp.FontStyle, nccFlag);
        ncc += nccFlag;
        bool succeedFlag = WriteBytes2OriginalAddress(tBytes, temp);
        if (!succeedFlag)
        {
            bytes2Heap.push_back(tBytes);
            vo2Heap.push_back(temp);
        }
        else{ noConvertdLine++; }
    }
    WriteBytesList2ExpandedAddress(bytes2Heap, vo2Heap);
    noConvertedChar = ncc;
    return noConvertdLine;
}

BOOL WriteBytes2OriginalAddress(std::vector<BYTE> bytes, const YS1TextVO &vo)
{
    int overSizeOffset = vo.TSize - bytes.size();
    //fill in zeros to ensure that the bytes is the same as the original game text
    if (overSizeOffset >= 0)
    {
        BYTE zero = Int2BytesBigEndian(Char2Code("\0"), 1)[0];
        for (int i = 0; i < overSizeOffset; i++)
        {
            bytes.push_back(zero);
        }
        bool result = WriteBytes2Address(bytes.data(), vo.TSize, (LPVOID)vo.Address);
        return result;
    }
    //Oversize
    return false;
}

BOOL WriteBytesList2ExpandedAddress(std::vector<std::vector<BYTE>> bytesList, const vector<YS1TextVO> &vos)
{
    //if the translated text size out of original game text size
    //we need use expanded space to save data
    size_t bytesListSize = bytesList.size();
    size_t vosSize = vos.size();
    size_t needSize = 0;
    if (bytesListSize != vosSize || bytesListSize == 0) { return false; }
    for (int i = 0; i < bytesListSize; i++)
    {
        needSize += bytesList[i].size();
        //Leave a spot for '\0', we need add '\0' at the end of line.
        needSize += 1;  
    }
    bool mallocFlag = Malloc4BytesHeap(needSize);
    if (!mallocFlag) { return false; }

    size_t bytesSize, lineBegin = 0, zeroCounter = 0;
    for (int i = 0; i < bytesListSize; i++)
    {
        auto bytes = bytesList[i];
        bytesSize = bytes.size();
        size_t cur = lineBegin;
        for (int j = 0; j < bytesSize; j++)
        {
            Write2BytesHeap(bytes[j], cur);
            cur++;
        }
        //add '\0' at the end of line.
        Write2BytesHeap(Int2BytesBigEndian(Char2Code("\0"), 1)[0], cur);
        zeroCounter++;
        //write address of line in heap to game
        if (vos[i].AddressUsedByCaller == -1) return false;
        BYTE *lbPointer = BytesHeapPointer(lineBegin);
        vector<BYTE> lbPointerAddress = Int2BytesBigEndian((int)&lbPointer, 4);  //one pointer use 4 bytes.
        WriteBytes2Address(BytesHeapPointer(lineBegin), bytesSize + 1, (LPVOID)vos[i].AddressUsedByCaller);
        lineBegin += bytesSize + 1;
    }
    return true;
}

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress)
{
    DWORD oop, nop, hasWrite;
    BOOL isSucceed = VirtualProtect(tgtAddress, tSize, PAGE_EXECUTE_READWRITE, &oop);
    if (!isSucceed) return FALSE;
    
    isSucceed = WriteProcessMemory(m_exeProc, tgtAddress, textBytes, tSize, &hasWrite);
    if (!isSucceed) return FALSE;

    isSucceed = VirtualProtect(tgtAddress, tSize, oop, &nop);
    return isSucceed;
}

BOOL AllocCustomConsole()
{
    BOOL bRet = AllocConsole();
    FILE *fDummy;
    freopen_s(&fDummy, "CONIN$", "r", stdin);    //Console Input
    freopen_s(&fDummy, "CONOUT$", "w", stderr);  //Console Error
    freopen_s(&fDummy, "CONOUT$", "w", stdout);  //Console Output
    return bRet;
}

BOOL FreeCustomConsole()
{
    return FreeConsole();
}

bool Malloc4BytesHeap(size_t size)
{
    if (m_expandedBytes != NULL)
    {
        free(m_expandedBytes);
        expandedBytesSize = 0;
    }
    m_expandedBytes = (BYTE *)malloc(size * sizeof(BYTE));
    if (m_expandedBytes != NULL)
    {
        memset(m_expandedBytes, 0, size);
        expandedBytesSize = size;
        return true;
    }
    return false;
}

bool Write2BytesHeap(BYTE date, size_t pos)
{
    if (pos < expandedBytesSize) 
    { 
        *(m_expandedBytes + pos) = date; 
        cout << "use pos : " << pos << endl;
        return true;
    }

    cout << "Out of BytesHeap" << endl;
    return false;
}

BYTE *BytesHeapPointer(size_t pos)
{
    return m_expandedBytes + pos;
}

void FreeBytesHeap()
{
    free(m_expandedBytes);
}
