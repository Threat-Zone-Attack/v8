char *pirateCodeEncrypt(char *key, char *string, int hexOutput){
	int i, j, keyLen, stringLen, currSym;
	char *outputHexed;
	unsigned char *result;

	keyLen = strlen(key);
	stringLen = strlen(string);
	result = (unsigned char*)safeMalloc(sizeof(unsigned char)* (stringLen*2 + 1));

	for (i=0; i<stringLen; i+=keyLen){
		for (j=i; j<i+keyLen && j<stringLen; j++){
			currSym = string[j] + key[j-i];
			result[j] = currSym;
		}
	}
	result[j] = '\0';

	if (hexOutput == 1){
		outputHexed = (char*)stringToHex(result, stringLen);
		free(result);
		return outputHexed;
	} else
		return (char*)result;
}

char *pirateCodeDecrypt(char *key, unsigned char *encrypted, int unhexInput){
	int i, j, keyLen, encryptedLength, currSym;
	char *result;
	unsigned char *input;

	keyLen = strlen(key);
	encryptedLength = strlen((char*)encrypted);

	if (unhexInput == 1){
		input = hexToString((char*)encrypted, encryptedLength);
		encryptedLength /= 2;
	} else {
		input = (unsigned char*)safeMalloc(sizeof(unsigned char) * (encryptedLength + 1));
		arrCopy((char*)input, (char*)encrypted, encryptedLength + 1);
	}

	result = (char*)safeMalloc(sizeof(char) * encryptedLength + 1);

	for (i=0; i<encryptedLength; i+=keyLen){
		for (j=i; j<i+keyLen && j<encryptedLength; j++){
			currSym = input[j] - key[j-i];
			result[j] = currSym;
		}
	}
	result[j] = '\0';
	free(input);

	return result;
}