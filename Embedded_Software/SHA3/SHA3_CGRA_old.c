#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

#define N 4

#define Config_File_Name "Config.txt"
#define LDM_File_Name "LDM.txt"

void Padding(uint8_t input[], int *in_len, int *absorb_times){
	*absorb_times = *in_len / 72;
    int add_last = *in_len % 72;
    if (72 - add_last == 1) {
        input[*in_len] = 0x86;
    }
    else {
        input[*in_len] = 0x06;
        for (int i = 1; i < (71 - add_last); i++){         
            input[*in_len + i] = 0x00;      
            // printf("%02x", input[*in_len + i]);
        }   
        input[*in_len + (71 - add_last)] = 0x80; 
        *in_len = *in_len + (72 - add_last);
    }

    *absorb_times += 1;
}

int main(){
	int k,i,j, LOOP;
	U32 OP;
	U64 OP_Immidiate;
	LOOP = 1;
	
	U64 ***LMM;

	LMM = (U64***) calloc(NUM_PE, sizeof(U64**));
	for(i = 0; i < NUM_PE; i++) {
		LMM[i] = (U64**) calloc(NUM_LMM, sizeof(U64*));
		for(j = 0; j < NUM_LMM; j++) {
			LMM[i][j] = (U64*) calloc(LMM_DEP, sizeof(U64));
		}
	}
	
	
	U64 **PE_out;
		PE_out =  (U64 **) calloc(LOOP*NUM_PE,sizeof(U64));
	for (i = 0; i < LOOP*NUM_PE; i ++){
		PE_out[i] = (U64 *)calloc(NUM_PE_INOUT,sizeof(U64));
	}
	//printf("You are here!\n");
	#if defined(ARMZYNQ)
	


	#elif defined(VIVADOSIL)
	CGRA_info.Config_File = fopen(Config_File_Name,"w");
	CGRA_info.LDM_File = fopen(LDM_File_Name,"w");
	
	for (i = 0; i < LOOP*16; i ++){	
		for (j = 0; j < 12; j ++){
			PE_out[i][j] = (i%4)*12+j;
		}
	}

	for(k = 0; k < LOOP; k++){
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",k);
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
	for (i = 0; i < NUM_PE; i++){
			printf ("PE_out[%d] =",i);
			for (j = 0; j < 12; j++){
				printf (" %016lx",PE_out[i+k*NUM_PE][j]);
			}
			printf ("\n");
			if( ((i+1) % 4) == 0){
				printf ("-----------------------------------------------------------------------------------------------------------------------\n");
			}
		}
		}
	#endif

	// U64 H[8] = {0x6a09e6676a09e667,0xbb67ae85bb67ae85,0x3c6ef3723c6ef372,0xa54ff53aa54ff53a, 0x510e527f510e527f, 0x9b05688c9b05688c, 0x1f83d9ab1f83d9ab, 0x5be0cd195be0cd19};
	
	// U64 W[16] = {0x6162638061626380,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018};
	// U32 W[16] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf, 0x10};
	// U32 W[16] = {0x71776572,0x74797569,0x6f706173,0x64666768,0x6a6b6c7a,0x78637662,0x6e6d3132,0x33343536,0x37383971,0x77657274,0x79637662,0x6e800000,0x00000000,0x00000000,0x00000000,0x00000168};
	// static const U32 K[64] = {
	// 0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	// 0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	// 0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	// 0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	// 0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	// 0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	// 0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	// 0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
// };

	int max_input_length = 4096; // Maximum length of the input
    int block_size = 72; // Block size for padding
    uint8_t input[max_input_length + block_size]; // Input buffer
    memset(input, 0, sizeof(input)); // Initialize input buffer with zeros

    // printf("Type data: ");
	// fgets((char*)input, max_input_length, stdin);
    // Remove newline character from fgets
    // input[strcspn((char*)input, "\n")] = 0;
    // int len = strlen((char*)input);
    // int absorb_times = 0;
	
	// printf("Length of data: %d\n", len);
    // Padding(input, &len, &absorb_times);
	
	 for (int i = 0; i < 72; i++) {
		input[i] = i;
        printf("%02x", input[i]);
    }
	printf("\n");
	
	for (i = 0; i < 9; i++){
	 LMM[i/2][i%2][0] = 0;
	 for (j = 0; j < 8; ++j) {
            LMM[i/2][i%2][0] |= ((U64)input[i*8+j]) << (8 * j);
        }
		printf("LMM[%d][%d][0] = %016lx\n",i/2,i%2,LMM[i/2][i%2][0]);
	}
	printf("-------\n");
	uint64_t S[5][5] = {0};
	
	for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            S[i][j] = i*5+j+1;
        }
    }
	
	
	for (i = 10; i < 19; i++){
	 LMM[i/2][i%2][0] = 0;
	 for (j = 0; j < 8; ++j) {
            LMM[i/2][i%2][0] = S[i/2-5][i%2];
        }
		printf("LMM[%d][%d][0] = %016lx\n",i/2,i%2,LMM[i/2][i%2][0]);
	}
	
	#if defined(ARMZYNQ)

	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[8][0][0],LMM[8][1][0],LMM[9][0][0],LMM[9][1][0],LMM[10][0][0],LMM[10][1][0],LMM[11][0][0],LMM[11][1][0]);
			fprintf(CGRA_info.LDM_File,"%016lx_%016lx%016lx%016lx%016lx%016lx%016lx%016lx%016lx\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[11][0][0],LMM[11][1][0],LMM[10][0][0],LMM[10][1][0],LMM[9][0][0],LMM[9][1][0],LMM[8][0][0],LMM[8][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;		
	#endif
	
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			// /*** row 0 ***///
			mop64(OP_LDW, 0, 0, LMM[0], PE_out[0+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[1], PE_out[1+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[2], PE_out[2+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[3], PE_out[3+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			mop64(OP_LDW, 0, 0, LMM[4], PE_out[4+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[5], PE_out[5+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0], PE_out[2][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[6], PE_out[6+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, 0, LMM[7], PE_out[7+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0], PE_out[3][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*** row 2 ***///
			mop64(OP_LDW, 0, 0, LMM[8], PE_out[8+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][0], 0, 0, 0, /*BUFF 8to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7]);
			mop64(OP_LDW, 0, 0, LMM[9], PE_out[9+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ PE_out[5][4], PE_out[5][4], 0, 0);
			// mop64(OP_LDW, 0, 0, LMM[10], PE_out[10+i*NUM_PE], /*ALU in*/ 0,0,PE_out[4][0], PE_out[4][1], /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// mop64(OP_LDW, 0, 0, LMM[11], PE_out[11+i*NUM_PE], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			/*** row 3 ***///

			// exe64(OP_NOP,0, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,PE_out[8][0],PE_out[8][1], /*BUFF 16to1 in*/ 0,0,0,PE_out[8][4], /*BUFF 8to1 in*/ PE_out[8][5], PE_out[8][6], PE_out[8][7], 0);
			// exe64(OP_NOP,0, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][4], /*BUFF 8to1 in*/ PE_out[9][5], PE_out[9][6], PE_out[9][7], 0);
			// exe64(OP_NOP,0, PE_out[14+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][2], PE_out[10][3], PE_out[11][2], PE_out[11][3], /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			// exe64(OP_NOP,0, PE_out[15+i*NUM_PE], /*ALU in*/ PE_out[10][0], PE_out[10][1],PE_out[9][0],PE_out[9][1], /*BUFF 16to1 in*/ 0, 0, PE_out[11][0], PE_out[11][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);		
			}		
	}
	
	#if defined(ARMSIL)
	for(k = 0; k < LOOP; k++){
		printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",k);
		printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		for (i = 0; i < NUM_PE; i++){
			printf ("PE_out[%d] =",i);
			for (j = 0; j < NUM_PE_INOUT; j++){
				printf (" %016lx",PE_out[i+k*NUM_PE][j]);
			}
			printf ("\n");
			if( ((i+1) % 4) == 0){
				printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			}
		}
	}
	// printf("\n Hash output =");
	// for(i = 0; i < 2; i++){
		// printf(" %016lx",LMM[0][i][0]);
	// }
	// for(i = 0; i < 2; i++){
		// printf(" %016lx",LMM[1][i][0]);
	// }
	// for(i = 0; i < 2; i++){
		// printf(" %016lx",LMM[2][i][0]);
	// }
	// for(i = 0; i < 2; i++){
		// printf(" %016lx",LMM[3][i][0]);
	// }
	// printf("\n");
	
	#elif defined(VIVADOSIL)	
		printf("Successfully write the configuration data to %s file\n",Config_File_Name);
	#endif
	
	for (i = 0; i < LOOP * NUM_PE; i++) {
        free(PE_out[i]);
    }
    free(PE_out);
	return 0;
}
