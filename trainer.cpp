#include "trainer.h"
#include "csv_reader.h"
#include "tool_log.h"
#include <fstream>

using namespace std;

HANDLE m_exeProc;
PVOID m_baseAddress;
extern BYTE *m_expandedBytes;
extern size_t expandedBytesSize;

BOOL InitTrainer()
{
    //HANDLE openProc = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE |
    //                             PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD |
    //                             PROCESS_QUERY_INFORMATION, 
    //                             FALSE, GetCurrentProcessId());
    HANDLE openProc = GetCurrentProcess(); //increase performance, have PROCESS_ALL_ACCESS
    if (openProc == NULL) { MessageBoxA(NULL, "Current Process Is Null", NULL, 0); return FALSE; }
    HMODULE hModules[100] = {0};
    DWORD dwRet = 0;
    BOOL bRet = EnumProcessModules(openProc, (HMODULE *)hModules, sizeof(hModules), &dwRet);
    if (bRet == FALSE)  {  MessageBoxA(NULL, "Current Moudle Is Null", NULL, 0);  return FALSE;  }

    wchar_t *procName = new wchar_t[100];
    GetModuleBaseName(openProc, hModules[0], procName, sizeof(procName));
    wstring procNameWSTR(procName);
    if (!procNameWSTR._Equal(YS1_PROCESS_NAME) && !procNameWSTR._Equal(YS2_PROCESS_NAME)) { return FALSE; }
    //only run when process is ys1 or ys2
    m_exeProc = openProc;
    m_baseAddress = GetBaseAddressByHandle(openProc);
    return TRUE;
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
            Log("\n!!! Need .CSV file!!!");
        }
        else
        {
            Log("line: " + to_string(lineFlag) + ", in .CSV file who has wrong format !!!");
        }
        return FALSE;
    }
    vector<YS1TextVO> ys1list(csvData.size());
    GetYS1TextVO(csvData, ys1list);
    long failedChar;
    DWORD failedLine = TranslateAllText(ys1list, failedChar);

    if (failedChar > 0)
    {
        Log("\nTranslated Failed Line Number : " + to_string(failedLine) + " pieces !");
        Log("Translated Failed Char Number : " + to_string(failedChar) + " pieces !!!");
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
    bool succeedFlag;
    for (int i = 0; i < ys1tSize; i++)
    {
        temp = list[i];
        if (temp.TranslatedTxt.size() == 0 || temp.TranslatedTxt == " ") { noConvertdLine++; continue; }

        long nccFlag = 0;
        vector<BYTE> tBytes = GetCustomBytesFromText(temp.TranslatedTxt.c_str(), temp.FontStyle, nccFlag);
        ncc += nccFlag;
        succeedFlag = WriteBytes2GameByOriginal(tBytes, temp);
        //oversize, need expanded space
        if (!succeedFlag)
        {
            bytes2Heap.push_back(tBytes);
            vo2Heap.push_back(temp);
        }
    }
    if (bytes2Heap.size() != 0)
    {
        WriteBytesCollection2GameByExpanded(bytes2Heap, vo2Heap, noConvertdLine);
    }
    noConvertedChar = ncc;
    return noConvertdLine;
}

BOOL WriteBytes2GameByOriginal(std::vector<BYTE> bytes, const YS1TextVO &vo)
{
    int overSizeOffset = vo.TSize - bytes.size();
    //fill in zeros to ensure that the bytes is the same as the original game text
    if (overSizeOffset > 0) //leave one position for \0
    {
        for (int i = 0; i < overSizeOffset; i++)
        {
            bytes.push_back(0);
        }
        bool result = WriteBytes2Address(bytes.data(), vo.TSize, (LPVOID)vo.Address);
        return result;
    }
    return false;
}

BOOL WriteBytesCollection2GameByExpanded(std::vector<std::vector<BYTE>> bytesCollection, const vector<YS1TextVO> &vos, DWORD &noConvertedLine)
{
    //if the translated text size out of original game text size
    //we need use expanded space to save data
    size_t bytesCollectionSize = bytesCollection.size();
    if (bytesCollectionSize != vos.size() || bytesCollectionSize == 0) { return FALSE; }

    //malloc a heap
    size_t memoryBytes = SizeOfBytesCollection(bytesCollection);
    bool mallocFlag = Malloc4BytesHeap(memoryBytes);
    if (!mallocFlag) { return FALSE; }

    //save dato to expanded space
    //and write the address of expanded space to game 
    size_t bytesSize, cur;
    size_t lineBegin = 0, zeroCounter = 0;
    vector<BYTE> lineAddress = {};
    bool writeFlag;
    int expandedAddress;
    for (int i = 0; i < bytesCollectionSize; i++)
    {
        if (vos[i].AddressUsedByCaller == -1) 
        {
            Log("Missing Address Of Caller£¡ Line: " + to_string(vos[i].ID) + " is oversize, but there is no address of caller to override.");
            noConvertedLine++;
            continue;
        }

        auto bytes = bytesCollection[i];
        bytesSize = bytes.size();
        cur = lineBegin;
        //add data of line to expanded space
        for (int j = 0; j < bytesSize; j++)
        {
            Add2BytesHeap(bytes[j], cur);
            cur++;
        }
        //add '\0' at the end of line.
        Add2BytesHeap(0, cur);
        zeroCounter++;

        //write address of line in heap to game
        expandedAddress = AddressOfBytesHeap(lineBegin);
        lineAddress = Int2BytesBigEndian(expandedAddress, 4);  //one pointer use 4 bytes.
        reverse(lineAddress.begin(), lineAddress.end());  //Windows use small Endian
        writeFlag = WriteBytes2Address(lineAddress.data(), 4, (LPVOID)vos[i].AddressUsedByCaller);
        
        if (writeFlag) { Log("Wirte Expanded Succeed! Line: " + to_string(vos[i].ID) + ", AUB:" + to_string(vos[i].AddressUsedByCaller)) ; }
        else { Log("Wirte Expanded Wrong! Line: " + to_string(vos[i].ID) + ", AUB:" + to_string(vos[i].AddressUsedByCaller)) ; }
        
        //record line begin pos in BytesHeap
        lineBegin += bytesSize + 1;

        lineAddress.clear();
    }
    return true;
}

BOOL WriteBytes2Address(BYTE *textBytes, DWORD tSize, LPVOID tgtAddress)
{
    DWORD oop, nop, hasWrite;
    BOOL isSucceed = VirtualProtect(tgtAddress, tSize, PAGE_EXECUTE_READWRITE, &oop);
    if (!isSucceed) 
    {
        DWORD error = GetLastError();
        Log("\nVirtualProtect ERROR(unlock): " + to_string(error));
        return FALSE;
    }
    
    isSucceed = WriteProcessMemory(m_exeProc, tgtAddress, textBytes, tSize, &hasWrite);
    if (!isSucceed)
    {
        DWORD error = GetLastError();
        Log("\nWriteProcessMemory ERROR: " + to_string(error));
        return FALSE;
    }

    isSucceed = VirtualProtect(tgtAddress, tSize, oop, &nop);
    if (!isSucceed)
    {
        DWORD error = GetLastError();
        Log("\nVirtualProtect ERROR(lock): " + to_string(error));
        return FALSE;
    }
    return TRUE;
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

size_t SizeOfBytesCollection(std::vector<std::vector<BYTE>> bytesCollection)
{
    size_t bytesListSize = bytesCollection.size();
    size_t needSize = 0;
    //malloc a heap
    for (int i = 0; i < bytesListSize; i++)
    {
        needSize += bytesCollection[i].size();
        //Leave a spot for '\0', we need add '\0' at the end of line.
        needSize += 1;
    }
    return needSize;
}

BOOL Malloc4BytesHeap(size_t size)
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

BOOL Add2BytesHeap(BYTE data, size_t pos)
{
    if (pos < expandedBytesSize) 
    { 
        *(m_expandedBytes + pos) = data; 
        return true;
    }
    return false;
}

int AddressOfBytesHeap(size_t pos)
{
    return (int)(m_expandedBytes + pos);
}

void FreeBytesHeap()
{
    if (m_expandedBytes != NULL)
    {
        free(m_expandedBytes);
        m_expandedBytes = NULL;
    }
}