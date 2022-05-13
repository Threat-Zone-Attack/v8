void showDebugMessage(TCHAR *message, int outputDebug);

int power(int num, int pow){
	return pow > 0 ? num * power(num, pow-1) : 1;
}

void *safeMalloc(int size){
	void *result;
	result = malloc(size);
	if (result != NULL)
		return result;
	else {
		showDebugMessage(L"cannot allocate more space!", 0);
		ExitProcess(0);
	}
}

void *safeRealloc(void *pointer, int size){
	void *result;
	result = realloc(pointer, size);
	if (result != NULL)
		return result;
	else {
		showDebugMessage(L"cannot allocate more space!", 0);
		ExitProcess(0);
	}
}

char *combineArrs(char *s1, int len1, char *s2, int len2){
	char *combineResult;
	int i, j;

	combineResult = (char*)safeMalloc(sizeof(char)*(len1+len2+2));

	for (i=0; i<len1; i++)
		combineResult[i] = s1[i];
	for (j=0; j<len2; j++)
		combineResult[i + j] = s2[j];
	combineResult[i + j] = '\0';

	return combineResult;
}

void combineStackArrs(char *result, char *arr1, int len1, char *arr2, int len2){
	int i, j;

	for (i=0; i<len1; i++)
		result[i] = arr1[i];
	for (j=0; j<len2; j++)
		result[i + j] = arr2[j];
}

void joinStringsWithSeparator(char **str1, int len1, char separator, char *str2, int len2, BOOL freeStr2){
	int i;

	*str1 = (char*)safeRealloc(*str1, sizeof(char) * (len1 + len2 + 2));
	(*str1)[len1] = separator;
	for (i = len1 + 1; i < len1 + len2 + 2; i ++)
		(*str1)[i] = str2[i - 1 - len1];

	if (freeStr2)
		free(str2);
}

int compareArrays(char *arr1, char *arr2, int start1, int length){
	int i;
	for (i=0; i<length; i++){
		if (arr1[start1+i] != arr2[i])
			return 0;
	}
	return 1;
}

int findMatchInStringArrs(char *string, int startPos, int stringLen, char **stringArrs, int stringArrsSize){
	int i, currStringFromArrLen;

	for (i=0; i<stringArrsSize; i++){
		currStringFromArrLen = strlen(stringArrs[i]);
		if (stringLen == currStringFromArrLen && compareArrays(string, stringArrs[i], startPos, stringLen) == 1)
			return 1;
	}
	return 0;
}

int findNeedle(char *haystack, char *needle){
	/*Haystack should always be larger than the needle!*/
	int i;

	for (i=0; i<2048; i++){
		if (compareArrays(haystack, needle, i, strlen(needle)) == 1)
			return 1;
	}
	return 0;
}

wchar_t *charToWidechar(char *charArray){
    wchar_t *wString = (wchar_t*)safeMalloc(sizeof(wchar_t)*(strlen(charArray)+1));
    mbstowcs(wString, charArray, strlen(charArray)+1);
    return wString;
}

void arrCopy(char *arr1, char *arr2, int length){
	int i;
	for (i=0; i<length; i++){
		arr1[i] = arr2[i];
	}
}

char *intToString(int num){
	char *string;
	int countPowers, numCounter;

	countPowers = 0;
	numCounter = num;
	while (numCounter > 0){
		numCounter /= 10;
		countPowers++;
	}

	string = (char*)safeMalloc(sizeof(char) * (countPowers+1));
	string[countPowers] = '\0';

	while (num > 0){
		countPowers--;
		string[countPowers] = num % 10 + '0';
		num /= 10;
	}

	return string;
}

int stringToInt(char *string){
	int stringLen, i, num;

	stringLen = strlen(string);
	num = 0;
	for (i=0; i<stringLen; i++){
		num += (string[stringLen-i-1]-'0') * power(10, i);
	}
	return num;
}

int hexToInt(char hexSym){
	if (hexSym >= 'a' && hexSym <= 'f')
		return hexSym - 87;
	else
		return hexSym - '0';
}

char intToHex(unsigned int num){
	if (num >= 10 && num <= 15)
		return num + 87;
	else
		return num + '0';
}

unsigned char *hexToString(char *hex, int hexLength){
	unsigned char *output;
	int i;

	output = (unsigned char*)safeMalloc(sizeof(unsigned char)*(hexLength/2 + 1));
	for (i=1; i<hexLength; i+=2)
		output[i/2] = hexToInt(hex[i-1])*16 + hexToInt(hex[i]);
	output[i/2] = '\0';

	return output;
}

unsigned char *stringToHex(unsigned char *string, int stringLength){
	unsigned char *output;
	int i;

	output = (unsigned char*)safeMalloc(sizeof(unsigned char)*(stringLength * 2 + 1));
	for (i=0; i<stringLength; i++){
		output[i*2 + 1] = intToHex(string[i] % 16);
		if(string[i] < 16)
			output[i*2] = '0';
		else
			output[i*2] = intToHex(string[i] / 16);
	}
	output[i*2] = '\0';

	return output;
}