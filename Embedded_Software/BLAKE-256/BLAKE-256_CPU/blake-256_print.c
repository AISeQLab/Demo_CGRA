#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "sph_blake.h"
#include "sph_blake.c"

unsigned int LitToBigEndian(unsigned int x)
{
	return (((x>>24) & 0x000000ff) | ((x>>8) & 0x0000ff00) | ((x<<8) & 0x00ff0000) | ((x<<24) & 0xff000000));
}


int main(int argc, char** argv)
{
	sph_blake256_context   ctx_blake256;
   
	char *datatest_file = "../Vector_Datatest/datatest.txt";
	
    FILE *fp = fopen(datatest_file, "r");
	 if (fp == NULL)
    {
        printf("Error: could not open file %s", datatest_file);
        return 1;
    }
	
	char* mess_in;
	int i,j, count;
	uint32_t hash_out[8];	
	
	const unsigned MAX_LENGTH = 2048;
    char buffer[MAX_LENGTH];
	count = 0;
    while (fgets(buffer, MAX_LENGTH, fp)){
		printf("\nMessage_in[%d] = %s", count, buffer);
		if (buffer[strlen(buffer)-1] == '\n'){
			mess_in = buffer;
			sph_blake256_set_rounds(14);
			sph_blake256_init(&ctx_blake256);
			sph_blake256 (&ctx_blake256, mess_in, strlen(mess_in)-1);
			sph_blake256_close(&ctx_blake256, hash_out);

			printf("Output[%d] = ",count);
			for(i =0; i <8; i++){
			printf("%08x ",LitToBigEndian(hash_out[i]));
			}
			printf("\n");
		}
		else {
			mess_in = buffer;
			sph_blake256_set_rounds(14);
			sph_blake256_init(&ctx_blake256);
			sph_blake256 (&ctx_blake256, mess_in, strlen(mess_in)-1);
			sph_blake256_close(&ctx_blake256, hash_out);

			printf("Output[%d] = ",count);
			for(i =0; i <8; i++){
			printf("%08x ",LitToBigEndian(hash_out[i]));
			}
			printf("\n");
		}
		count++;
        
	}
    // close the file
    fclose(fp);
	
	return 0;
}