void swapElements(unsigned int *arr, int pos1, int pos2){
	unsigned int buffer;

	buffer = arr[pos1];
	arr[pos1] = arr[pos2];
	arr[pos2] = buffer;
}

void keystreamXOR(unsigned int *state, unsigned char *string, int stringLength, unsigned char *output){
	int i, j, k;

	i = j = 0;

	for (k=0; k<stringLength; k++){
		i = (i+1) % 256;
		j = (j+state[i]) % 256;
		swapElements(state, i, j);
		output[k] = string[k] ^ state[(state[i]+state[j]) % 256];
	}
	output[k] = '\0';
}

void scheduleKey(unsigned int *state, char *key, int keyLength){
	int i, j;

	j = 0;
	for (i=0; i<256; i++){
		j = (j + state[i] + key[i%keyLength]) % 256;
		swapElements(state, i, j);
	}
}

void initState(unsigned int *state){
	int i;
	for (i=0; i<256; i++)
		state[i] = i;
}

char *rc4(char *key, unsigned char *string, int unhexInput, int hexOutput){
	int keyLength, stringLength;
	unsigned int state[256];
	unsigned char *input, *output, *outputHexed;

	keyLength = strlen(key);
	stringLength = strlen((char*)string);
	
	if (unhexInput == 1){
		input = hexToString((char*)string, stringLength);
		stringLength /= 2;
	} else {
		input = (unsigned char*)safeMalloc(sizeof(unsigned char) * (stringLength + 1));
		arrCopy((char*)input, (char*)string, stringLength + 1);
	}

	initState(state);
	scheduleKey(state, key, keyLength);

	output = (unsigned char*)safeMalloc(sizeof(unsigned char)*(stringLength + 1));

	keystreamXOR(state, input, stringLength, output);

	free(input);

	if (hexOutput == 1){
		outputHexed = stringToHex(output, stringLength);
		free(output);
		return (char*)outputHexed;
	} else
		return (char*)output;
}