DWORD WINAPI clingfishModeThread(LPVOID pID){
	int procID;
	TCHAR *exeFullPath;
	HANDLE hProcess;

	exeFullPath = (TCHAR*)safeMalloc(sizeof(TCHAR) * (MAX_PATH + 1));

	procID = (int)pID;
	hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, procID);
	if (hProcess == NULL || getFullPath(procID, exeFullPath) == 0){
		free(exeFullPath);
		CloseHandle(hProcess);
		return 0;
	} else {
		showDebugMessage(L"Clingfish mode activated!", 0);
		while(1){
			scanProcMemory(hProcess, exeFullPath, procID);
			Sleep(clingfishPause);
		}
	}
}

int checkClingfishUsage(int pID){
	int i;
	for (i=0; i<clingfishProcsCount; i++)
		if (clingfishProcs[i] == pID)
			return 1;

	return 0;
}

void clingfishModeActivate(int pID){
	if (clingfishProcsCount == 0){
		clingfishProcs = (int*)safeMalloc(sizeof(int));
		clingfishThreads = (HANDLE*)safeMalloc(sizeof(HANDLE));

		scanPause = scanPauseAfterClingfishMode;
	} else {
		clingfishProcs = (int*)safeRealloc(clingfishProcs, sizeof(int) * (clingfishProcsCount + 1));
		clingfishThreads = (HANDLE*)safeRealloc(clingfishThreads, sizeof(HANDLE) * (clingfishProcsCount + 1));
	}

	clingfishProcs[clingfishProcsCount] = pID;
	clingfishThreads[clingfishProcsCount] = CreateThread(NULL, 0, clingfishModeThread, (LPVOID)pID, 0, NULL);

	clingfishProcsCount ++;
}