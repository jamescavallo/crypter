#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>

void handleErrors(void){
  ERR_print_errors_fp(stderr);
  abort();
}

int main(){
  //read in the exe file and convert its bytes to a string
  FILE *inFile;
  inFile = fopen("Quasar.exe","rb");
  if (inFile == NULL){
    perror("Failed to read in file\n");
  }

  printf("File read in complete!\n");

  if(fseek(inFile , 0 , SEEK_END) == -1){
    perror("Offset error\n");
  };
	unsigned long lSize = ftell(inFile);
  if (lSize == -1){
    perror("Size error\n");
  }
	rewind(inFile);

  unsigned char *unencryptedText = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
	fread(unencryptedText,1,lSize,inFile);
	fclose(inFile);

  unsigned char *encryptedText = (unsigned char*) malloc (3 *(sizeof(unsigned char)*lSize));

  //encrypt these bytes with open ssl
  printf("Encrypting...\n");
  int outlen, tmplen;
  unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  unsigned char iv[] = {1,2,3,4,5,6,7,8};
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

  EVP_CIPHER_CTX_init(ctx);
  EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);

  if(EVP_EncryptUpdate(ctx, encryptedText, &outlen, unencryptedText, lSize *sizeof(char)) == 0){
    ERR_print_errors_fp(stderr);
    return 0;
  }
      
  if(EVP_EncryptFinal_ex(ctx, encryptedText + outlen, &tmplen) == 0){
    ERR_print_errors_fp (stderr);
    return 0;
  }
  outlen += tmplen;
  EVP_CIPHER_CTX_cleanup(ctx);
  printf("Encrypt Success!\n");

  char absPath[500];
	strcpy (absPath, "encrypted.dat");
	FILE *outFile = fopen(absPath, "wb");
	fwrite(encryptedText, lSize, 1, outFile);
	fclose (outFile);


//decrypt
unsigned char *decryptedText = (unsigned char*) malloc (sizeof(unsigned char)*lSize);
EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);

if(EVP_DecryptUpdate(ctx, decryptedText, &outlen, encryptedText, lSize) == 0){
  ERR_print_errors_fp (stderr);
  return 0;
}

EVP_CIPHER_CTX_cleanup(ctx);
printf("Decrypt Success!\n");


 //write
char absPath2[500];
strcpy (absPath2, "decrypted.exe");
FILE *outFile2 = fopen(absPath2, "wb");
fwrite(decryptedText, lSize, 1, outFile);
fclose (outFile2);
return 0;
}