#include "trainer.h"
#include <string>

HANDLE m_exeProc;

BOOL Translate()
{
    InitTrainer();
    //PVOID address = GetBaseAddressByPID(GetCurrentProcessId());
    PVOID address = GetBaseAddressByHandle(m_exeProc);
    TranslateText("text", address);
    return TRUE;
}

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

BOOL TranslateText(LPCSTR text, PVOID baseAddress)
{
    DWORD a = (DWORD)0x004DBBE4 - (DWORD)baseAddress;
    DWORD b = (DWORD)0x000DBBE4;
    DWORD address = (DWORD)(0x004DBBE4);
    //char* tgtAddress = *(char**)(0x004DBBE4);
    //char temp[] = "布鲁多医生";
    //*(char **)tgtAddress = temp;
    char buf[100] = {};
    BOOL bRet = ReadProcessMemory(m_exeProc, (LPVOID)address, &buf, 100, NULL);
    std::string str = buf;
    return TRUE;
}