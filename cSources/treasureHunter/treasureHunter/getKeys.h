void saveKeys(char *response, int responseLength){
	int i, j, k, keyLength;

	keyLength = (responseLength - 10) / 3;

	levelOneRC4Key = (char*)safeMalloc(sizeof(char) * (keyLength + 1));
	levelTwoPirateCodeKey = (char*)safeMalloc(sizeof(char) * (keyLength + 1));
	levelThreeRC4Key = (char*)safeMalloc(sizeof(char) * (keyLength + 1));

	for (i=8; response[i]!=':'; i++)
		levelOneRC4Key[i - 8] = response[i];
	levelOneRC4Key[i - 8] = '\0';

	for (j=i+1; response[j]!=':'; j++)
		levelTwoPirateCodeKey[j - i - 1] = response[j];
	levelTwoPirateCodeKey[j - i - 1] = '\0';

	for (k=j+1; k<responseLength; k++)
		levelThreeRC4Key[k - j - 1] = response[k];
	levelThreeRC4Key[k - j - 1] = '\0';
}

BOOL getKeys(){
	char *requestPart1, *requestPart2, *requestPart3, *requestPart4, *requestPart5, *request, *requestCall, *response, *rc4Decrypted, *pirateCodeDecrypted;
	int requestLength, firstEncKeyLength, reportLength;
	HINTERNET hSession, hConnect;

	reportLength = 106;

	requestCall = "GETKEYS";

	requestPart1 = rc4(firstEncKey, (unsigned char*)requestCall, 0, 1);

	requestLength = strlen(requestCall)*2;
	firstEncKeyLength = strlen(firstEncKey);

	requestPart2 = combineArrs("request=", 8, requestPart1, requestLength);
	free(requestPart1);
	requestPart3 = combineArrs(requestPart2, requestLength+8, "&use=", 5);
	free(requestPart2);
	requestPart4 = combineArrs(requestPart3, requestLength+13, firstEncKey, firstEncKeyLength);
	free(requestPart3);
	requestPart5 = combineArrs(requestPart4, requestLength+firstEncKeyLength+13, "&id=", 4);
	free(requestPart4);
	request = combineArrs(requestPart5, requestLength+firstEncKeyLength+17, systemID, 32);
	free(requestPart5);
	request[requestLength + firstEncKeyLength + 50] = '\0';

	if (initConnection(&hSession, &hConnect, userAgentRequest, gateDomain)){
		response = readInternetFile(hConnect, gateRequestURL, request, reportLength*2);
		free(request);
		closeInternetHandles(hSession, hConnect);

		rc4Decrypted = rc4(firstEncKey, (unsigned char*)response, 1, 0);
		free(response);

		pirateCodeDecrypted = pirateCodeDecrypt(firstEncKey, (unsigned char*)rc4Decrypted, 0);

		free(rc4Decrypted);

		if (strstr(pirateCodeDecrypted, "success") != NULL){
			saveKeys(pirateCodeDecrypted, 106);
			free(pirateCodeDecrypted);
			return TRUE;
		} else {
			free(pirateCodeDecrypted);
			return FALSE;
		}
	} else {
		free(request);
		return FALSE;
	}
}