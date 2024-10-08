#include "trainer.h"
#include "csv_reader.h"

using namespace std;

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
    InitTrainer();
    PVOID address = GetBaseAddressByHandle(m_exeProc);
    vector<vector<string>> csvData;
    bool readCsv;
    readCsv = ReadDataFromCSV(YS1_EXE_CSV_PATH, csvData);
    if (csvData.size() == 0)
    {
        readCsv = ReadDataFromCSV(YS2_EXE_CSV_PATH, csvData);
    }
    if (!readCsv)
    {
        cout << "\n!!! Need .CSV file!!!" << endl;
        return FALSE;
    }
    vector<YS1TextValueObject> ys1list(csvData.size());
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

DWORD TranslateAllText(const vector<YS1TextValueObject> &list, long &noConvertedChar)
{
    int ys1tSize = list.size();
    if (ys1tSize == 0) return FALSE;
    DWORD noConvertdLine = 0;
    BOOL result = FALSE; 
    long ncc = 0;
    YS1TextValueObject temp;
    for (int i = 0; i < ys1tSize; i++)
    {
        temp = list[i];
        if (temp.TranslatedTxt.size() != 0 &&temp.TranslatedTxt != " ")
        {
            long nccFlag = 0;
            vector<BYTE> tByte = GetCustomBytesFromText(temp.TranslatedTxt.c_str(), temp.FontStyle, temp.TSize, nccFlag);
            ncc += nccFlag;
            if (tByte.size() != 0)
            {
                BYTE *bytes(tByte.data());
                result = WriteBytes2Address(bytes, temp.TSize, (LPVOID)temp.AddressInYS1);
            }
            if (!result)
            {
                noConvertdLine++;
            }
        }
        else { noConvertdLine++; }
    }
    noConvertedChar = ncc;
    return noConvertdLine;
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
