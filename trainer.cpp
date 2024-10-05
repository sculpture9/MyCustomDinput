#include "trainer.h"

HANDLE m_exeProc;
PVOID m_baseAddress;

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
    //InitTrainer();
    //PVOID address = GetBaseAddressByHandle(m_exeProc);
    //vector<vector<string>> csvData = ReadDataFromCSV(YS1_CSV);
    //vector<YS1TextValueObject> ys1list = GetYS1TextVO(csvData);
    //BOOL ret = TranslateAllText(ys1list);

    wchar_t sss = L'Œ“';
    wstring s = { sss };
    wchar_t c = s[0];
    int i = c;
    vector<BYTE> ttt = Int2Bytes(i, 2);
    return true;
    //return ret;
}

BOOL TranslateAllText(vector<YS1TextValueObject> list)
{
    BOOL result = TRUE;
    //int c = sizeof(list);
    //int a = sizeof(list[0]);
    //int count = sizeof(list) / sizeof(list[0]);
    YS1TextValueObject temp;
    for (int i = 0; i < 3; i++)
    {
        temp = list[i];
        vector<BYTE> tByteVs = NewBytesFromText(temp.TranslatedTxt, temp.TSize);
        BYTE *bytes(tByteVs.data());
        result = WriteBytes2Address(bytes, temp.TSize, (LPVOID)temp.AddressInYS1);
        if (!result) return FALSE;
    }
    return result;
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