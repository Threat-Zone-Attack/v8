int getValidPANLength(char *haystack, int haystackLength, int startPos){
	int i, checkDigit, sum, multipliedNum, length;

	for (i=startPos; i-startPos+1<=19 && haystack[i]>='0' && haystack[i]<='9'; i++){
		if (i == haystackLength-1)
			return 0;
	}

	sum = 0;
	checkDigit = haystack[i-1] - '0';
	length = i - startPos;

	if (length < 13)
		return 0;
	else {
		for (i-=2; i>=startPos; i-=2){
			multipliedNum = (haystack[i] - '0') * 2;
			if (multipliedNum > 9)
				multipliedNum = multipliedNum%10 + multipliedNum/10;

			sum += multipliedNum;
			if (i > startPos)
				sum += (haystack[i-1] - '0');
		}

		if (checkDigit == (sum * 9) % 10 || checkDigit == 10 - sum%10)
			return length;
		else
			return 0;
	}
}

int getNameLength(char *haystack, int haystackLength, int startPos){
	int i;
	for (i=startPos; i-startPos+1<=29 && haystack[i]!='^'; i++){	/*26 for maximum name length + 3 for country code, just in case*/
		if (i == haystackLength-1)
			return 0;
	}
	return i-startPos;
}

int getValidExpDateLength(char *haystack, int haystackLength, int startPos){
	if (startPos + 4 < haystackLength && haystack[startPos] >= '0' && haystack[startPos] <= '9' && haystack[startPos+1] >= '0' &&
			haystack[startPos+1] <= '9' && 
			((haystack[startPos+2] == '0' && haystack[startPos+3] >= '0' && haystack[startPos+3] <= '9') ||
				(haystack[startPos+2] == '1' && haystack[startPos+3] >= '0' && haystack[startPos+3] <= '2')))
		return 4;
	else if (((haystack[startPos] == '^' && haystack[startPos-1] == '^') || (haystack[startPos] == '=' && haystack[startPos-1] == '=')) &&
			recordNoExpOrSCodeEnabled == 1)
		return 1;
	else
		return 0;
}

int getValidServiceCodeLength(char *haystack, int haystackLength, int startPos){
	/*if (startPos + 3 < haystackLength && (haystack[startPos] == '1' || haystack[startPos] == '2' || (haystack[startPos] >= '5' && haystack[startPos] <= '7')) && (haystack[startPos+1] == '0' || haystack[startPos+1] == '2' || haystack[startPos+1] == '4') && haystack[startPos+2] >= '0' && haystack[startPos+2] <= '7')*/
	if (startPos + 3 < haystackLength && (haystack[startPos] >= '0' && haystack[startPos] <= '9') && (haystack[startPos+1] >= '0' &&
			haystack[startPos+1] <= '9') && (haystack[startPos+2] >= '0' && haystack[startPos+2] <= '9') && 
			(checkServiceCodes == 0 || (checkServiceCodes == 1 && findMatchInStringArrs(haystack, startPos, 3, serviceCodes, SERVICECODESNUM) == 1)))
		return 3;
	else if (((haystack[startPos] == '^' && haystack[startPos-2] == '^') || (haystack[startPos] == '=' && haystack[startPos-2] == '=')) && 
			recordNoExpOrSCodeEnabled == 1)
		return 1;
	else
		return 0;
}

int getValidDiscrDataLength(char *haystack, int haystackLength, int startPos){
	int i;
	for (i=startPos; i<haystackLength && haystack[i]>='0' && haystack[i]<='9'; i++);
	if (i-startPos > 0)
		return i-startPos;
	else
		return 0;
}

int getValidTrack1Length(char *haystack, int haystackLength, int startPos){
	int currPos, PANLength, nameLength, expDateLength, serviceCodeLength, discrDataLength;

	PANLength = getValidPANLength(haystack, haystackLength, startPos);

	if (PANLength == 0 || haystack[startPos + PANLength] != '^')
		return 0;
	else {
		currPos = startPos + PANLength + 1;
		nameLength = getNameLength(haystack, haystackLength, currPos);
		if (nameLength == 0 || haystack[currPos + nameLength] != '^')
			return 0;
		else {
			currPos += nameLength + 1;
			expDateLength = getValidExpDateLength(haystack, haystackLength, currPos);
			if (expDateLength == 0)
				return 0;
			else {
				currPos += expDateLength;
				serviceCodeLength = getValidServiceCodeLength(haystack, haystackLength, currPos);
				if (serviceCodeLength == 0)
					return 0;
				else {
					discrDataLength = getValidDiscrDataLength(haystack, haystackLength, currPos);
					if (discrDataLength == 0)
						return 0;
					else {
						currPos += discrDataLength;
						return currPos - startPos;
					}
				}
			}
		}
	}
}

int getValidTrack2Length(char *haystack, int haystackLength, int startPos){
	int currPos, PANLength, expDateLength, serviceCodeLength, discrDataLength;

	PANLength = getValidPANLength(haystack, haystackLength, startPos);

	if (PANLength == 0 || haystack[startPos + PANLength] != '=')
		return 0;
	else {
		currPos = startPos + PANLength + 1;
		expDateLength = getValidExpDateLength(haystack, haystackLength, currPos);
		if (expDateLength == 0)
			return 0;
		else {
			currPos += expDateLength;
			serviceCodeLength = getValidServiceCodeLength(haystack, haystackLength, currPos);
			if (serviceCodeLength == 0)
				return 0;
			else {
				currPos += serviceCodeLength;
				discrDataLength = getValidDiscrDataLength(haystack, haystackLength, currPos);
				if (discrDataLength == 0)
					return 0;
				else {
					currPos += discrDataLength;
					return currPos - startPos;
				}
			}
		}
	}
}

void findTracks(TCHAR *lpFileName, int procID, char *haystack, int haystackLength){
	int i, startPos, lengthResult;

	for (i=0; i<haystackLength; i++){
		lengthResult = 0;
		startPos = i;
		if (haystack[i] == 'B' || (haystack[i] == '%' && haystack[i+1] == 'B')){
			if (haystack[i] == 'B')
				startPos = i+1;
			else
				startPos = i+2;

			lengthResult = getValidTrack1Length(haystack, haystackLength, startPos) + 1;

			startPos --;
		} else if ((haystack[i] >= '0' && haystack[i] <= '9') || (haystack[i] == ';' && haystack[i+1] >= '0' && haystack[i+1] <= '9')){
			if (haystack[i] == ';')
				startPos = i+1;
			else
				startPos = i;

			lengthResult = getValidTrack2Length(haystack, haystackLength, startPos);
		}

		if (lengthResult > 1)
			recordTrack(lpFileName, procID, haystack, lengthResult, startPos);

		i += lengthResult;
	}
}