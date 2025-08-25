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


int main(){
	int k,i,j, LOOP;
	
	#if defined(VIVADOSIL)
	CGRA_info.Config_File = fopen(Config_File_Name,"w");
	CGRA_info.LDM_File = fopen(LDM_File_Name,"w");
	#endif
	
	// LOOP = 67;
	LOOP = 67;
	uint64_t OP;
	uint32_t ***LMM;

	LMM = (uint32_t***) calloc(16, sizeof(uint32_t**));
	for(i = 0; i < 16; i++) {
		LMM[i] = (uint32_t**) calloc(2, sizeof(uint32_t*));
    for(j = 0; j < 2; j++) {
        LMM[i][j] = (uint32_t*) calloc(1024, sizeof(uint32_t));
		}
	}

	uint32_t OP_Immidiate;
	//uint32_t PE_out[LOOP*16][12];
	uint32_t **PE_out;
		PE_out =  (uint32_t **) calloc(LOOP*4*16,sizeof(uint32_t));
	for (i = 0; i < LOOP*4*16; i ++){
		PE_out[i] = (uint32_t *)calloc(12,sizeof(uint32_t));
	}
	
	#if defined(ARMZYNQ)

	#elif defined(VIVADOSIL)
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
				printf (" %08x",PE_out[i+k*NUM_PE][j]);
			}
			printf ("\n");
			if( ((i+1) % 4) == 0){
				printf ("-----------------------------------------------------------------------------------------------------------------------\n");
			}
		}
		}
	#endif

	uint32_t H[8] = {0x7380166f,0x4914b2b9,0x172442d7,0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e};
	
	uint32_t W[16] = {0x61626380,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018};
	// uint32_t W[16] = {0x61626380,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe, 0x00000018};
	uint32_t T[64];
	uint32_t K[64];
	// Initialize T
    for (j = 0; j < 16; j++) {
        T[j] = 0x79CC4519;
    }
    for (j = 16; j < 64; j++) {
        T[j] = 0x7A879D8A;
    }

    // Calculate K
    for (j = 0; j < 64; j++) {
        // You would need to use a different expression for K[j] when j is between 0 and 15
        // if there's supposed to be a different calculation or initialization there.
        K[j] = ROTL(T[j], j);
    }

    // Print K
    for (j = 0; j < 64; j++) {
        printf("K[%d] = %08X \t", j, K[j]);
		if(j%4==3){
			printf("\n");
		}
	}
	
	// static const uint32_t K[64] = {
	// 0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	// 0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	// 0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	// 0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	// 0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	// 0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	// 0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	// 0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2};

	LMM[0][0][0] = H[0];
	LMM[0][1][0] = H[1];
	
	LMM[1][0][0] = H[2];
	LMM[1][1][0] = H[3];

	LMM[2][0][0] = H[4];
	LMM[2][1][0] = H[5];
	
	LMM[3][0][0] = H[6];
	LMM[3][1][0] = H[7];

	LMM[4][0][0] = W[0];
	LMM[4][1][0] = W[1];
	
	LMM[5][0][0] = W[2];
	LMM[5][1][0] = W[3];

	LMM[6][0][0] = W[4];
	LMM[6][1][0] = W[5];
	
	LMM[7][0][0] = W[6];
	LMM[7][1][0] = W[7];	
	
	LMM[8][0][0] = W[8];
	LMM[8][1][0] = W[9];
	
	LMM[9][0][0] = W[10];
	LMM[9][1][0] = W[11];

	LMM[10][0][0] = W[12];
	LMM[10][1][0] = W[13];
	
	LMM[11][0][0] = W[14];
	LMM[11][1][0] = W[15];
	
	#if defined(ARMZYNQ)

	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[8][0][0],LMM[8][1][0],LMM[9][0][0],LMM[9][1][0],LMM[10][0][0],LMM[10][1][0],LMM[11][0][0],LMM[11][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[11][0][0],LMM[11][1][0],LMM[10][0][0],LMM[10][1][0],LMM[9][0][0],LMM[9][1][0],LMM[8][0][0],LMM[8][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;		
	#endif
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			/*** row 0 ***///
			mop(OP_LDW, 0, 0, LMM[0], PE_out[0+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[1], PE_out[1+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[2], PE_out[2+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[3], PE_out[3+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 1 ***///
			mop(OP_LDW, 0, 0, LMM[4], PE_out[4+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[5], PE_out[5+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0], PE_out[2][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[6], PE_out[6+i*NUM_PE], /*ALU in*/ 0,0,PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[7], PE_out[7+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 2 ***///
			mop(OP_LDW, 0, 0, LMM[8], PE_out[8+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5],PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[9], PE_out[9+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[6][2], PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[10], PE_out[10+i*NUM_PE], /*ALU in*/ 0,0,PE_out[4][0], PE_out[4][1], /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[11], PE_out[11+i*NUM_PE], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			///*** row 3 ***///

			exe(OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5], PE_out[8][6], PE_out[8][7]);
			exe(OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5], PE_out[9][6], PE_out[9][7]);
			exe(OP_NOP, PE_out[14+i*NUM_PE], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][2], PE_out[10][3], PE_out[11][2], PE_out[11][3], /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			exe(OP_NOP, PE_out[15+i*NUM_PE], /*ALU in*/ PE_out[9][0],PE_out[9][1],PE_out[10][0],PE_out[10][1], /*BUFF 16to1 in*/ PE_out[8][0],PE_out[8][1], PE_out[11][0], PE_out[11][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);		
			
			// printf("PE 12: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+12][0], PE_out[i*NUM_PE+12][1], PE_out[i*NUM_PE+12][2], PE_out[i*NUM_PE+12][3]);
			// printf("PE 13: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+13][0], PE_out[i*NUM_PE+13][1], PE_out[i*NUM_PE+13][2], PE_out[i*NUM_PE+13][3]);
			// printf("PE 14: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+14][0], PE_out[i*NUM_PE+14][1], PE_out[i*NUM_PE+14][2], PE_out[i*NUM_PE+14][3]);
			// printf("PE 15: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+15][0], PE_out[i*NUM_PE+15][1], PE_out[i*NUM_PE+15][2], PE_out[i*NUM_PE+15][3]);
			}
		else if(i == 65){
			// /*** row 0 ***///
			mop(OP_LDW, 0, 0, LMM[0], PE_out[0+i*NUM_PE], /*ALU in*/ 0,0,PE_out[12+(i-1)*NUM_PE][0],PE_out[12+(i-1)*NUM_PE][8], /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ 0,0,0,0);
			mop(OP_LDW, 0, 0, LMM[1], PE_out[1+i*NUM_PE], /*ALU in*/ 0,0,PE_out[12+(i-1)*NUM_PE][9],PE_out[12+(i-1)*NUM_PE][10], /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][0], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[2], PE_out[2+i*NUM_PE],/*ALU in*/ 0,0,PE_out[13+(i-1)*NUM_PE][7],PE_out[13+(i-1)*NUM_PE][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_LDW, 0, 0, LMM[3], PE_out[3+i*NUM_PE],/*ALU in*/ 0,0,PE_out[13+(i-1)*NUM_PE][9],PE_out[13+(i-1)*NUM_PE][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
			exe(OP, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][0],PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[0+i*NUM_PE][5], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][0],PE_out[1+i*NUM_PE][1],PE_out[0+i*NUM_PE][6],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][0],PE_out[2+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[1+i*NUM_PE][5], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][0],PE_out[3+i*NUM_PE][1],PE_out[1+i*NUM_PE][6],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe(OP_NOP, PE_out[8+i*NUM_PE],/*ALU in*/  PE_out[4+i*NUM_PE][2] ,0,0,PE_out[4+i*NUM_PE][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[9+i*NUM_PE],/*ALU in*/  PE_out[5+i*NUM_PE][2] ,0,0,PE_out[5+i*NUM_PE][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][2] ,0,0,PE_out[6+i*NUM_PE][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][2] ,0,0,PE_out[7+i*NUM_PE][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe(OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ PE_out[8+i*NUM_PE] [1],0,0,PE_out[8+i*NUM_PE] [2],/*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ PE_out[9+i*NUM_PE] [1],0,0,PE_out[9+i*NUM_PE] [2],/*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ PE_out[10+i*NUM_PE][1],0,0,PE_out[10+i*NUM_PE][2],/*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe(OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ PE_out[11+i*NUM_PE][1],0,0,PE_out[11+i*NUM_PE][2],/*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i == 66){
			mop(OP_STW, 0, 0, LMM[0], PE_out[0+i*NUM_PE], /*ALU in*/ PE_out[12+(i-1)*NUM_PE][1],PE_out[12+(i-1)*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_STW, 0, 0, LMM[1], PE_out[1+i*NUM_PE], /*ALU in*/ PE_out[13+(i-1)*NUM_PE][1],PE_out[13+(i-1)*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_STW, 0, 0, LMM[2], PE_out[2+i*NUM_PE], /*ALU in*/ PE_out[14+(i-1)*NUM_PE][1],PE_out[14+(i-1)*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop(OP_STW, 0, 0, LMM[3], PE_out[3+i*NUM_PE], /*ALU in*/ PE_out[15+(i-1)*NUM_PE][1],PE_out[15+(i-1)*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else{
			///*** row 0 ***///
			if( i == 1) {
				/*** row 0 ***///
				// /**/OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/12,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[0+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][8],PE_out[14+(i-1)*NUM_PE][4],0,PE_out[14+(i-1)*NUM_PE][8], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8],PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[1+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][8],0,PE_out[12+(i-1)*NUM_PE][9],PE_out[12+(i-1)*NUM_PE][10], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[2+i*NUM_PE],/*ALU in*/ PE_out[13+(i-1)*NUM_PE][8],0,PE_out[13+(i-1)*NUM_PE][9],PE_out[13+(i-1)*NUM_PE][10], /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR,/*OP_IM*/0);	// E^F^G
				exe(OP, PE_out[3+i*NUM_PE],/*ALU in*/ PE_out[15+(i-1)*NUM_PE][1], PE_out[14+(i-1)*NUM_PE][4], 0, PE_out[14+(i-1)*NUM_PE][11], /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][0], PE_out[15+(i-1)*NUM_PE][1], PE_out[15+(i-1)*NUM_PE][2], PE_out[15+(i-1)*NUM_PE][3], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5],PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7]);

				// /*** row 1 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/K[i-1]);
				exe(OP, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][2],0, PE_out[1+i*NUM_PE][8], PE_out[0+i*NUM_PE][2], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][11],PE_out[1+i*NUM_PE][1],0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[1+i*NUM_PE][11], PE_out[2+i*NUM_PE][4], 0, /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR,/*OP_IM*/0);
				// printf("6164e387 ROTL 15 = %08x, 6164e387 ROTL 15 = %08x, XOR = %08x\n", ROTL(0x6164e387,15), ROTL(0x6164e387,23), ROTL(0x6164e387,15)^ ROTL(0x6164e387,23));
				exe(OP, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1], 0, PE_out[3+i*NUM_PE][1], PE_out[3+i*NUM_PE][1], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);


				// /*** row 2 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][1],PE_out[5+i*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/9,/*SRU2*/OP_ROL,/*SRU2_IM*/19,/*LU3*/OP_NOP,/*OP_IM*/0); // Find new C and new G
				exe(OP, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][9],0, 0, PE_out[5+i*NUM_PE][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][2],PE_out[6+i*NUM_PE][2],0,0, /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][4], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_ROL,/*SRU2_IM*/7,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0], 0, PE_out[7+i*NUM_PE][1], PE_out[6+i*NUM_PE][7],/*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][4], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);


				// /*** row 3 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[12+i*NUM_PE],/*ALU in*/ 0, 0, PE_out[10+i*NUM_PE][2], 0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][2],PE_out[8+i*NUM_PE][8],PE_out[9+i*NUM_PE][2],PE_out[8+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);	// Wait C
				// printf("\n%08x\n", P0(0xc067debb));
				/*--*/OP = CUSTOM_OP(OP_P0);
				exe(OP, PE_out[13+i*NUM_PE],/*ALU in*/ PE_out[10+i*NUM_PE][2],0,0,0, /*BUFF 16to1 in*/ 0,PE_out[9+i*NUM_PE][8],PE_out[9+i*NUM_PE][1],PE_out[9+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0); 
				exe(OP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0, 0, 0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][5], PE_out[10+i*NUM_PE][6], PE_out[10+i*NUM_PE][7], PE_out[11+i*NUM_PE][4], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[15+i*NUM_PE],/*ALU in*/ PE_out[11+i*NUM_PE][1], 0, PE_out[11+i*NUM_PE][6], 0,/*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][5], PE_out[11+i*NUM_PE][6], PE_out[11+i*NUM_PE][7], 0,/*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]); // 0xd317e0c4 ^ 0x0000000a

				
				// printf("PE 12: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+12][0], PE_out[i*NUM_PE+12][1], PE_out[i*NUM_PE+12][2], PE_out[i*NUM_PE+12][3]);
				// printf("PE 13: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+13][0], PE_out[i*NUM_PE+13][1], PE_out[i*NUM_PE+13][2], PE_out[i*NUM_PE+13][3]);
				// printf("PE 14: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+14][0], PE_out[i*NUM_PE+14][1], PE_out[i*NUM_PE+14][2], PE_out[i*NUM_PE+14][3]);
				// printf("PE 15: %08x %08x %08x %08x\n", PE_out[i*NUM_PE+15][0], PE_out[i*NUM_PE+15][1], PE_out[i*NUM_PE+15][2], PE_out[i*NUM_PE+15][3]);
			}
			else if(i<=16){
					/*** row 0 ***///
				// /**/OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/12,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[0+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][4],PE_out[14+(i-1)*NUM_PE][4],0,PE_out[14+(i-1)*NUM_PE][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][4],PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[1+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][4],0,PE_out[12+(i-1)*NUM_PE][5],PE_out[12+(i-1)*NUM_PE][6], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][0], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[2+i*NUM_PE],/*ALU in*/ PE_out[13+(i-1)*NUM_PE][0],0,PE_out[13+(i-1)*NUM_PE][5],PE_out[13+(i-1)*NUM_PE][6], /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][8], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11], PE_out[14+(i-1)*NUM_PE][7]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR,/*OP_IM*/0);	// E^F^G
				exe(OP, PE_out[3+i*NUM_PE],/*ALU in*/ PE_out[15+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][4], 0, PE_out[14+(i-1)*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][2], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9],PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);

				// /*** row 1 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/K[i-1]);
				exe(OP, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][2],0, PE_out[1+i*NUM_PE][8], PE_out[0+i*NUM_PE][2], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][11],PE_out[1+i*NUM_PE][1],0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[1+i*NUM_PE][11], PE_out[2+i*NUM_PE][4], 0, /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR,/*OP_IM*/0);
				// printf("6164e387 ROTL 15 = %08x, 6164e387 ROTL 15 = %08x, XOR = %08x\n", ROTL(0x6164e387,15), ROTL(0x6164e387,23), ROTL(0x6164e387,15)^ ROTL(0x6164e387,23));
				exe(OP, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1], 0, PE_out[3+i*NUM_PE][1], PE_out[3+i*NUM_PE][1], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][8], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11], PE_out[3+i*NUM_PE][7]);


				// /*** row 2 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][1],PE_out[5+i*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/9,/*SRU2*/OP_ROL,/*SRU2_IM*/19,/*LU3*/OP_NOP,/*OP_IM*/0); // Find new C and new G
				exe(OP, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][9],0, 0, PE_out[5+i*NUM_PE][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][2],PE_out[6+i*NUM_PE][2],0,0, /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][4], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_ROL,/*SRU2_IM*/7,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0], 0, PE_out[7+i*NUM_PE][1], PE_out[6+i*NUM_PE][7],/*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][4], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);


				// /*** row 3 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[12+i*NUM_PE],/*ALU in*/ 0, 0, PE_out[10+i*NUM_PE][2], 0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][2],PE_out[8+i*NUM_PE][8],PE_out[9+i*NUM_PE][2],PE_out[8+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);	// Wait C
				// printf("\n%08x\n", P0(0xc067debb));
				/*--*/OP = CUSTOM_OP(OP_P0);
				exe(OP, PE_out[13+i*NUM_PE],/*ALU in*/ PE_out[10+i*NUM_PE][2],0,0,0, /*BUFF 16to1 in*/ 0,PE_out[9+i*NUM_PE][8],PE_out[9+i*NUM_PE][1],PE_out[9+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0); 
				exe(OP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0, 0, 0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][5], PE_out[10+i*NUM_PE][6], PE_out[10+i*NUM_PE][7], PE_out[11+i*NUM_PE][4], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[15+i*NUM_PE],/*ALU in*/ PE_out[11+i*NUM_PE][1], 0, PE_out[11+i*NUM_PE][6], 0,/*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][5], PE_out[11+i*NUM_PE][6], PE_out[11+i*NUM_PE][7], 0,/*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]); // 0xd317e0c4 ^ 0x0000000a
			}
			else if(i<=65){
				/*** row 0 ***///
				// /**/OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/12,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[0+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][4],PE_out[14+(i-1)*NUM_PE][4],0,PE_out[14+(i-1)*NUM_PE][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][4],PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7]);
				/*--*/OP = CUSTOM_OP(OP_FF1);
				exe(OP, PE_out[1+i*NUM_PE],/*ALU in*/ PE_out[12+(i-1)*NUM_PE][4],PE_out[12+(i-1)*NUM_PE][5],PE_out[12+(i-1)*NUM_PE][6], 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][0], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[2+i*NUM_PE],/*ALU in*/ PE_out[13+(i-1)*NUM_PE][0],PE_out[13+(i-1)*NUM_PE][0],PE_out[13+(i-1)*NUM_PE][5],PE_out[13+(i-1)*NUM_PE][6], /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][8], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11], PE_out[14+(i-1)*NUM_PE][7]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR,/*OP_IM*/0);	// E^F^G
				exe(OP, PE_out[3+i*NUM_PE],/*ALU in*/ PE_out[15+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][4], 0, PE_out[14+(i-1)*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][2], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9],PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);

				// /*** row 1 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/K[i-1]);
				exe(OP, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][2],0, PE_out[1+i*NUM_PE][8], PE_out[0+i*NUM_PE][2], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][11],PE_out[1+i*NUM_PE][0],0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[1+i*NUM_PE][11], PE_out[2+i*NUM_PE][4], 0, /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR,/*OP_IM*/0);
				// printf("6164e387 ROTL 15 = %08x, 6164e387 ROTL 15 = %08x, XOR = %08x\n", ROTL(0x6164e387,15), ROTL(0x6164e387,23), ROTL(0x6164e387,15)^ ROTL(0x6164e387,23));
				exe(OP, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1], 0, PE_out[3+i*NUM_PE][1], PE_out[3+i*NUM_PE][1], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][8], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11], PE_out[3+i*NUM_PE][7]);


				// /*** row 2 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][1],PE_out[5+i*NUM_PE][2], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/9,/*SRU2*/OP_ROL,/*SRU2_IM*/19,/*LU3*/OP_NOP,/*OP_IM*/0); // Find new C and new G
				exe(OP, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][9],0, 0, PE_out[5+i*NUM_PE][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][2],PE_out[6+i*NUM_PE][2],0,0, /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][4], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_ROL,/*SRU2_IM*/7,/*LU3*/OP_XOR,/*OP_IM*/0);
				exe(OP, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0], 0, PE_out[7+i*NUM_PE][1], PE_out[6+i*NUM_PE][7],/*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][4], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);


				// /*** row 3 ***///
				// OP_Immidiate = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[12+i*NUM_PE],/*ALU in*/ 0, 0, PE_out[10+i*NUM_PE][2], 0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][2],PE_out[8+i*NUM_PE][8],PE_out[9+i*NUM_PE][2],PE_out[8+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);	// Wait C
				// printf("\n%08x\n", P0(0xc067debb));
				/*--*/OP = CUSTOM_OP(OP_P0);
				exe(OP, PE_out[13+i*NUM_PE],/*ALU in*/ PE_out[10+i*NUM_PE][2],0,0,0, /*BUFF 16to1 in*/ 0,PE_out[9+i*NUM_PE][8],PE_out[9+i*NUM_PE][1],PE_out[9+i*NUM_PE][10], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0); 
				exe(OP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0, 0, 0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][5], PE_out[10+i*NUM_PE][6], PE_out[10+i*NUM_PE][7], PE_out[11+i*NUM_PE][4], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP,/*OP_IM*/0);
				exe(OP, PE_out[15+i*NUM_PE],/*ALU in*/ PE_out[11+i*NUM_PE][1], 0, PE_out[11+i*NUM_PE][6], 0,/*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][5], PE_out[11+i*NUM_PE][6], PE_out[11+i*NUM_PE][7], 0,/*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]); // 0xd317e0c4 ^ 0x0000000a
			}

			
		}
		
	}
	#if defined(ARMSIL)
	for(k = 0; k < LOOP; k++){
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",k);
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		for (i = 0; i < NUM_PE; i++){
			printf ("PE_out[%d] =",i);
			for (j = 0; j < 12; j++){
				printf (" %08x",PE_out[i+k*NUM_PE][j]);
			}
			printf ("\n");
			if( ((i+1) % 4) == 0){
				printf ("-----------------------------------------------------------------------------------------------------------------------\n");
			}
		}
	}
	printf("\n Hash output =");
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[0][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[1][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[2][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[3][i][0]);
	}
	printf("\n");
	
	#elif defined(VIVADOSIL)	
		printf("Successfully write the configuration data to %s file\n",Config_File_Name);
	#endif
	
	
	return 0;
}
