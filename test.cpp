#include "framework.h"
extern HANDLE m_exeProc;

void Test(LPVOID address, LPCSTR text)
{
    DWORD aSoleDortorAtTHAddress = 0x004DBBE4;
    DWORD buf;
    BOOL bRet = ReadProcessMemory(m_exeProc, (LPVOID)aSoleDortorAtTHAddress, &buf, 4, NULL);

    LPCSTR mystr = "this is my txt";
    //将文本写入内存
    DWORD size = strlen(mystr) + 1;
    SIZE_T dwHasWrite;
    LPVOID mystrAddress = VirtualAllocEx(m_exeProc, NULL, size, MEM_COMMIT, PAGE_READWRITE);
    if (mystrAddress != 0)
    {
        DWORD oldPro, newPro;
        BOOL mmRet = VirtualProtect(mystrAddress, size, PAGE_EXECUTE_READWRITE, &oldPro);
        BOOL bRet = WriteProcessMemory(m_exeProc, mystrAddress, mystr, size, &dwHasWrite);
        LPCSTR str = "this is my txt";
        BOOL pRet = VirtualProtect((LPVOID)aSoleDortorAtTHAddress, 13, PAGE_EXECUTE_READWRITE, &oldPro);
        BOOL wRet = WriteProcessMemory(m_exeProc, (LPVOID)aSoleDortorAtTHAddress, str, 4, &dwHasWrite);
        pRet = VirtualProtect((LPVOID)aSoleDortorAtTHAddress, 13, oldPro, &newPro);
    }
    //将文本地址写入目标
    DWORD bb;
    BOOL bfRet = ReadProcessMemory(m_exeProc, (LPVOID)aSoleDortorAtTHAddress, &bb, 4, NULL);
}