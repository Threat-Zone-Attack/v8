unsigned int leftRotate(unsigned int x, int n){
    return ((x) << n) | ((x) >> (32 - n));
}

unsigned int shiftAmounts[64] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
								 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
								 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
								 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

unsigned int partsOfSines[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
								 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
								 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
								 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
								 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
								 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
								 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
								 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
								 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
								 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
								 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
								 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
								 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
								 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
								 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
								 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

unsigned int a0 = 0x67452301,
			 b0 = 0xefcdab89,
			 c0 = 0x98badcfe,
			 d0 = 0x10325476;

void generateLastChunk(int stringLen, unsigned int *message){
	int i;
	for (i=0; i<14; i++)
		message[i] = 0;
	message[14] = stringLen*8; /* = (stringLen*8) % 0xffffffff;*/
	message[15] = 0; /* = (stringLen*8) - message[14];*/
}

int generateFinalChunk(char *string, int start, int stringLen, unsigned int *message){
	int i, j, k;

	for (i=0; i<16; i++)
		message[i] = 0;

	for (i=0; i<16; i++){
		k = start;
		for (j=start; j<stringLen && j-start < 4; j++){
			message[i] += string[j]%16 * power(16, k-start) + string[j]/16 * power(16, k-start+1);
			k += 2;
		}
		if (stringLen-start < 4 && i < 14){
			message[i] += 8*power(16, k-start+1);
			message[14] = stringLen*8; /* = (stringLen*8) % 0xffffffff;*/
			message[15] = 0; /* = (stringLen*8) - message[14];*/

			return 1;
		} else if (stringLen-start < 4 && i >= 14){
			message[i] += 8*power(16, k-start+1);

			return 0;
		} else
			start = j;
	}

	return 0;
}

void generateChunk(char *string, int start, int end, unsigned int *message){
	int i, j, k;

	for (i=0; i<16; i++){
		message[i] = 0;
		k = start;
		for (j=start; j<end && j-start < 4; j++){
			message[i] += string[j]%16 * power(16, k-start) + string[j]/16 * power(16, k-start+1);
			k += 2;
		}
		start = j;
	}
}

/*MD5 function made to work with ASCII strings shorter than 0xffffffff (in bits)*/
char *md5(char *string){
	unsigned int A, B, C, D,
				 i, F, g, bufD, n;
	unsigned int message[16];
	int j, stringLen, finalIsLast;

	char *result;
	
	stringLen = strlen(string);
	finalIsLast = 1;
	result = (char*)safeMalloc(sizeof(char)*33);
	
	for (j=0; j<stringLen || finalIsLast == 0 || (j == stringLen && stringLen%64 == 0); j+=64){
		if (j+64 <= stringLen)
			generateChunk(string, j, j+64, message);
		else if (finalIsLast == 1 || j == stringLen)
			finalIsLast = generateFinalChunk(string, j, stringLen, message);
		else if (finalIsLast == 0){
			generateLastChunk(stringLen, message);
			finalIsLast = 1;
		}

		A = a0;
		B = b0;
		C = c0;
		D = d0;

		for (i=0; i<64; i++){
			if (i < 16){
				F = (B & C) | (~B & D);
				g = i;
			} else if (i >= 16 && i < 32){
				F = (D & B) | (~D & C);
				g = (5*i + 1) % 16;
			} else if (i >= 32 && i < 48){
				F = B ^ C ^ D;
				g = (3*i + 5) % 16;
			} else if (i >= 48){
				F = C ^ (B | ~D);
				g = (7*i) % 16;
			}
			bufD = D;
			D = C;
			C = B;
			B += leftRotate((A + F + partsOfSines[i] + message[g]), shiftAmounts[i]);
			A = bufD;
		}
		a0 += A;
		b0 += B;
		c0 += C;
		d0 += D;
	}

	/*printReverseEndian(a0);
	printReverseEndian(b0);
	printReverseEndian(c0);
	printReverseEndian(d0);*/

	for (i=0; i<32; i+=8){
		if (i < 8)
			n = a0;
		else if (i >= 8 && i < 16)
			n = b0;
		else if (i >= 16 && i < 24)
			n = c0;
		else if (i >= 24)
			n = d0;
		result[i] = intToHex((n & 0x000000ff) / 16);
		result[i+1] = intToHex((n & 0x000000ff) % 16);
		result[i+2] = intToHex(((n>>8) & 0x000000ff) / 16);
		result[i+3] = intToHex(((n>>8) & 0x000000ff) % 16);
		result[i+4] = intToHex(((n>>16) & 0x000000ff) / 16);
		result[i+5] = intToHex(((n>>16) & 0x000000ff) % 16);
		result[i+6] = intToHex((n>>24) / 16);
		result[i+7] = intToHex((n>>24) % 16);
	}
	result[i] = '\0';
	return result;
}