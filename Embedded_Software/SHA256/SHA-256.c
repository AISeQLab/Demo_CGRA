#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

#define N 4

int main(){
	int k,i,j, LOOP;
	Init();
	
	LOOP = 67;
	U32 OP;
	
	U32 OP_IM;

	uint32_t H[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	uint32_t W[16] = {0x61626380,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018};
	//uint32_t W[16] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf, 0x10};
	//uint32_t W[16] = {0x71776572,0x74797569,0x6f706173,0x64666768,0x6a6b6c7a,0x78637662,0x6e6d3132,0x33343536,0x37383971,0x77657274,0x79637662,0x6e800000,0x00000000,0x00000000,0x00000000,0x00000168};
	static const uint32_t K[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};

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

	fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",24);		
	#endif
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			///*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 1 ***///
			mop32(OP_LDW, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[5], /*ALU in*/ 0,0,PE_out[2][0], PE_out[2][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[6], /*ALU in*/ 0,0,PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 2 ***///
			mop32(OP_LDW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5],PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2], PE_out[5][3], PE_out[6][2], PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[10], /*ALU in*/ 0,0,PE_out[4][0], PE_out[4][1], /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[11], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			///*** row 3 ***///

			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,0,PE_out[8][1], /*BUFF 16to1 in*/ 0,0,0,PE_out[8][4], /*BUFF 8to1 in*/ PE_out[8][5], PE_out[8][6], PE_out[8][7], 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][4], /*BUFF 8to1 in*/ PE_out[9][5], PE_out[9][6], PE_out[9][7], 0);
			exe32(OP_NOP, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][2], PE_out[10][3], PE_out[11][2], PE_out[11][3], /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			exe32(OP_NOP, 0, PE_out[15], /*ALU in*/ PE_out[9][0], PE_out[9][1],PE_out[10][0],PE_out[10][1], /*BUFF 16to1 in*/ 0, 0, PE_out[11][0], PE_out[11][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);		
			}
		else if(i == 65){
			// /*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0], /*ALU in*/ 0,0,PE_out[12][2],PE_out[12][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1], /*ALU in*/ 0,0,PE_out[12][9],PE_out[12][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,PE_out[13][7],PE_out[13][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,PE_out[13][9],PE_out[13][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[0][2],PE_out[0][1],PE_out[0][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][0],PE_out[1][2],PE_out[1][1],PE_out[1][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][0],PE_out[2][2],PE_out[2][1],PE_out[2][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][0],PE_out[3][2],PE_out[3][1],PE_out[3][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],PE_out[4][1], 0, PE_out[4][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][0],PE_out[5][1], 0, PE_out[5][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],PE_out[6][1], 0, PE_out[6][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0],PE_out[7][1], 0, PE_out[7][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, PE_out[8][1], PE_out[8][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, PE_out[9][1], PE_out[9][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0,PE_out[10][1],PE_out[10][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0,PE_out[11][1],PE_out[11][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i == 66){
			// /*** row 0 ***///
			mop32(OP_STW, 0, PE_out[0], /*ALU in*/ PE_out[12][0],PE_out[12][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[1], /*ALU in*/ PE_out[13][0],PE_out[13][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[2], /*ALU in*/ PE_out[14][0],PE_out[14][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[3], /*ALU in*/ PE_out[15][0],PE_out[15][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			exe32(OP_NOP, 0, PE_out[4],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[5],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[6],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe32(OP_NOP, 0, PE_out[8],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[9],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[10],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else{
			///*** row 0 ***///
			if( i == 1) {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][4],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][7],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][11], PE_out[14][10], PE_out[14][9], PE_out[14][8], /*BUFF 8to1 in*/ PE_out[14][7], PE_out[14][6], PE_out[14][5], PE_out[14][4]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][5],PE_out[15][6], PE_out[15][0], PE_out[15][1], /*BUFF 16to1 in*/ PE_out[12][2], PE_out[12][1], PE_out[15][2], PE_out[15][3], /*BUFF 8to1 in*/ 0, 0, PE_out[15][6], PE_out[15][7]);

				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][5],PE_out[2][11], 0, 0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][4], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][11], PE_out[3][10], PE_out[3][3], PE_out[3][2], /*BUFF 8to1 in*/ PE_out[3][7], PE_out[3][6], PE_out[3][5], PE_out[3][4]);
			}
			else {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][11],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][2], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][2],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][10],PE_out[15][5], 0, 0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);
			
				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][10],PE_out[2][11], 0, PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][11], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

			}
			/*** row 2 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][2],PE_out[4][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4][7], PE_out[4][8], PE_out[4][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[4][2],PE_out[4][10], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5][7], PE_out[5][8], PE_out[5][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[5][1],PE_out[4][8],0,PE_out[4][9], /*BUFF 16to1 in*/ PE_out[7][0],PE_out[6][4],PE_out[6][5],PE_out[6][6], /*BUFF 8to1 in*/ PE_out[6][7], PE_out[6][8], PE_out[6][9], PE_out[6][10]);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][2],PE_out[7][1],PE_out[6][2],0, /*BUFF 16to1 in*/ 0,PE_out[7][4],PE_out[7][5],PE_out[7][6], /*BUFF 8to1 in*/ PE_out[7][7], PE_out[7][8], PE_out[7][9], PE_out[7][10]);
			/*** row 3 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][2],PE_out[10][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][2], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[11][2],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);		
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
	Final();
	
	return 0;
}
