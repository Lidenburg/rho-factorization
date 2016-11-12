// Compile with: gcc -o rho rsa.c -Wall -lcrypto -O3

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "BN_rsa.h"

int main(int argc, char *argv[]){
	srand(time(NULL));	// Safe rand ftw!
	ERR_load_crypto_strings();
	BN_CTX *ctx = BN_CTX_new();
	BIGNUM *n = NULL, *c = NULL;
	char *str = NULL;

	if(argc != 2){
		printf("[!] Please provide a number to factorize!\n");
		exit(1);
	}

	if(!BN_dec2bn(&n, argv[1])){
		printf("[!] BN_dec2bn failed!\n");
		exit(1);
	}

	printf("factoring %s\n", argv[1]);

	c = BN_rho(n);
	str = BN_bn2dec(c);

	// n = n/c
	if(!BN_div(n, NULL, n, c, ctx)) ;
		// BN_error("failed first div!");

	printf("factors: %s", str);

	str = BN_bn2dec(n);
	// printf("n: %s\n", str);

	while(!BN_isPrime(n)){
		c = BN_rho(n);
		// n = n/c
		if(!BN_div(n, NULL, n, c, ctx)) ;
			// BN_error("failed dividing!");

		str = BN_bn2dec(c);
		printf("\t%s", str);

		str = BN_bn2dec(n);
		// printf("n: %s\n", str);
	}

	str = BN_bn2dec(n);
	if(strcmp(str, "1"))
		printf("\t%s\n", str);

	return 0;
}
