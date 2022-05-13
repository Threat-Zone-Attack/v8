#pragma comment(lib, "winhttp.lib")

char *readResponse(HINTERNET hRequest, int readBytes){
	char *lpBuffer;
	DWORD lpdwNumberOfBytesAvailable, lpdwNumberOfBytesRead;

	lpBuffer = (char*)safeMalloc(sizeof(char) * (readBytes + 1));

	if (!WinHttpQueryDataAvailable(hRequest, &lpdwNumberOfBytesAvailable)){
		free(lpBuffer);
		return NULL;
	} else {
		if (lpdwNumberOfBytesAvailable > readBytes)
			lpdwNumberOfBytesAvailable = readBytes;

		while (WinHttpReadData(hRequest, lpBuffer, lpdwNumberOfBytesAvailable, &lpdwNumberOfBytesRead)){
			if (lpdwNumberOfBytesRead > 0 && lpdwNumberOfBytesRead <= readBytes){
				lpBuffer[lpdwNumberOfBytesRead] = '\0';
				return lpBuffer;
			} else
				break;
		}
		free(lpBuffer);
		return NULL;
	}
}

BOOL initConnection(HINTERNET *hSession, HINTERNET *hConnect, char *userAgent, char *domain){
	wchar_t *userAgentWidechar, *domainWidechar;

	userAgentWidechar = charToWidechar(userAgent);
	domainWidechar = charToWidechar(domain);

	*hSession = WinHttpOpen(userAgentWidechar, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	*hConnect = WinHttpConnect(*hSession, domainWidechar, INTERNET_DEFAULT_HTTP_PORT, 0);

	free(userAgentWidechar);
	free(domainWidechar);

	if (*hSession != NULL && *hConnect != NULL)
		return TRUE;
	else {
		WinHttpCloseHandle(*hSession);
		WinHttpCloseHandle(*hConnect);
		return FALSE;
	}
}

char *readInternetFile(HINTERNET hConnect, char *gateURL, char *POSTData, int readBytes){
	HINTERNET hRequest;
	wchar_t *gateURLWidechar;
	DWORD lpdwNumberOfBytesRead, dwBytesWritten, dwNumberOfBytesToWrite;
	BOOL bSendReqestResult, bAddRequestHeaders, bRecieveResponseResult;
	char *response;

	bSendReqestResult = bAddRequestHeaders = bRecieveResponseResult = FALSE;

	gateURLWidechar = charToWidechar(gateURL);

	dwNumberOfBytesToWrite = strlen(POSTData);

	hRequest = WinHttpOpenRequest(hConnect, L"POST", gateURLWidechar, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	free(gateURLWidechar);
	
	if (hRequest == NULL)
		return NULL;
	else 
		bAddRequestHeaders = WinHttpAddRequestHeaders(hRequest, L"Content-Type: application/x-www-form-urlencoded", 47, WINHTTP_ADDREQ_FLAG_ADD);

	if (bAddRequestHeaders == FALSE)
		return NULL;
	else 
		bSendReqestResult = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, (LPVOID)POSTData, dwNumberOfBytesToWrite, dwNumberOfBytesToWrite, 0);

	if (bSendReqestResult == FALSE)
		return NULL;
	else
		bRecieveResponseResult = WinHttpReceiveResponse(hRequest, NULL);

	if (bRecieveResponseResult == FALSE)
		return NULL;
	else
		response = readResponse(hRequest, readBytes);

	if (hRequest != NULL)
		WinHttpCloseHandle(hRequest);

	return response;
}

void closeInternetHandles(HINTERNET hSession, HINTERNET hConnect){
	WinHttpCloseHandle(hSession);
	WinHttpCloseHandle(hConnect);
}