char *getActivationKey(){
	HKEY keyHandle;
	TCHAR output[2048];
	TCHAR lpData[1024];
	
	DWORD lpcbData = 2048;
	int i, j, idx, value, keyInd = 0, keyHex[16];
	char keyChars[25]  = "BCDFGHJKMPQRTVWXY2346789";
	char *key;

	key = (char*)safeMalloc(sizeof(char) * 30);

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &keyHandle) != ERROR_SUCCESS){
		/*MessageBox(NULL, L"fail", L"title", MB_OK);*/
		return "";
	} else if (RegQueryValueEx(keyHandle, L"DigitalProductId", NULL, NULL, (LPBYTE)lpData, &lpcbData) != ERROR_SUCCESS){
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_64KEY, &keyHandle) != ERROR_SUCCESS){
			/*MessageBox(NULL, L"fail", L"title", MB_OK);*/
			return "";
		} else if (RegQueryValueEx(keyHandle, L"DigitalProductId", NULL, NULL, (LPBYTE)lpData, &lpcbData) != ERROR_SUCCESS){
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE | KEY_WOW64_32KEY, &keyHandle) != ERROR_SUCCESS){
				/*MessageBox(NULL, L"fail", L"title", MB_OK);*/
				return "";
			} else if (RegQueryValueEx(keyHandle, L"DigitalProductId", NULL, NULL, (LPBYTE)lpData, &lpcbData) != ERROR_SUCCESS){
				/*MessageBox(NULL, L"fail", L"title", MB_OK);*/
				return "";
			}
		}	
	}

	for (i=0; i*2+1<lpcbData; i++){
		output[i*2] = lpData[i]-(lpData[i]>>8)*256;
		output[i*2 + 1] = lpData[i]>>8;
	}
	for (i=52; i<67; i++)
		keyHex[i-52] = output[i];
	for (i = 28; i >= 0; i--)
		switch ((i + 1) % 6){
			case 0: /*if the calculation is 0 (zero) then add the seperator*/
				key[i] = '-';
				break;
			default:
				idx = 0;
 
				for (j = 14; j >= 0; j--){
					value = (idx << 8) | keyHex[j];
					keyHex[j] = (value/24);
					idx = value % 24;
					key[i] = keyChars[idx];
				}
				break;
		}
	key[29] = '\0';

	return key;
}

int getLength(char *buffer, int startPos, int bufferLen){
	int i;
	for (i=startPos; buffer[i]!='\0' && i<bufferLen; i++);
	return i-startPos;
}

char *getHardDriveSerialNumber(){
	HANDLE hPhysicalDriveIOCTL = INVALID_HANDLE_VALUE;
	TCHAR driveName[256] = L"\\\\.\\PhysicalDrive0";

	STORAGE_PROPERTY_QUERY query;
	STORAGE_DEVICE_DESCRIPTOR *descriptor;
	DWORD cbBytesReturned = 0;
	char buffer[10000], *result;
	int serialNumLength, i;

	query.PropertyId = StorageDeviceProperty;
	query.QueryType = PropertyStandardQuery;
	 *(query.AdditionalParameters) = 0;
	/*memset(buffer, 0, sizeof(buffer));*/

	/*sprintf(driveName, "\\\\.\\PhysicalDrive%d", drive);*/
	/*  Windows NT, Windows 2000, Windows XP - admin rights not required*/
	hPhysicalDriveIOCTL = CreateFile(driveName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE){
		/*MessageBox(NULL, L"successfully opened drive", L"title", MB_OK);*/
		if (DeviceIoControl(hPhysicalDriveIOCTL, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), &buffer, 10000, &cbBytesReturned, NULL)){
			descriptor = (STORAGE_DEVICE_DESCRIPTOR*)&buffer;
			serialNumLength = getLength(buffer, descriptor->SerialNumberOffset, 10000);
			result = (char*)safeMalloc(sizeof(char) * (serialNumLength+1));
			for (i=0; i<serialNumLength; i++)
				result[i] = buffer[descriptor->SerialNumberOffset+i];
			result[i] = '\0';

			CloseHandle(hPhysicalDriveIOCTL);
			return result;
		} else {
			CloseHandle(hPhysicalDriveIOCTL);
			return "";
		}
	} else {
		CloseHandle(hPhysicalDriveIOCTL);
		return "";
	}
}

char *generateSystemID(){
	char *windowsActivationKey, *hardDriveSerial, *combinedInfo, *combinedInfoHashed;
	int windowsActivationKeyLength, hardDriveSerialLength;

	windowsActivationKey = getActivationKey();
	windowsActivationKeyLength = strlen(windowsActivationKey);

	hardDriveSerial = getHardDriveSerialNumber();
	hardDriveSerialLength = strlen(hardDriveSerial);

	combinedInfo = combineArrs(windowsActivationKey, windowsActivationKeyLength, hardDriveSerial, hardDriveSerialLength);
	free(windowsActivationKey);
	free(hardDriveSerial);

	combinedInfoHashed = md5(combinedInfo);
	free(combinedInfo);
	return combinedInfoHashed;
}