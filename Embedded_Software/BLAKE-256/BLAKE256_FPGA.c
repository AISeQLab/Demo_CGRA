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
	U32 OP;
	Init();
	LOOP = 2 + (14*5) + 2;	
	U32 K[64];
	uint32_t H[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	// uint32_t W[16] = {0x61626380, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00000018};
	uint32_t W[16] = {0x6d736b64, 0x67626a61, 0x6b62646a, 0x6b616864, 0x6b6a6168, 0x6a6b6c64, 0x68616b80, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x000000d8};
	uint32_t T[2]	= {W[15], 0x0000000};
	
	static const uint32_t CS[16] = {
		0x243F6A88, 0x85A308D3,
		0x13198A2E, 0x03707344,
		0xA4093822, 0x299F31D0,
		0x082EFA98, 0xEC4E6C89,
		0x452821E6, 0x38D01377,
		0xBE5466CF, 0x34E90C6C,
		0xC0AC29B7, 0xC97C50DD,
		0x3F84D5B5, 0xB5470917
	};
	static const unsigned sigma[16][16] = {
		{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
		{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
		{ 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
		{  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
		{  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
		{  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 },
		{ 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 },
		{ 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 },
		{  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 },
		{ 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0 },
		{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
		{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
		{ 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
		{  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
		{  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
		{  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 }
	};

	static const uint32_t W_Idx[16] = {
		8, 9, 10, 11,
		8, 9, 10, 11,
		8, 9, 10, 11,
		8, 9, 10, 11
	};
	static const uint32_t Rw0_Idx[16] = {
		12, 12, 12, 12,
		13, 13, 13, 13,
		14, 14, 14, 14,
		15, 15, 15, 15
	};
	static const uint32_t Rw1_Idx[16] = {
		0, 0, 0, 0,
		1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3, 3, 3
	};
	static const uint32_t Rw2_Idx[16] = {
		4, 4, 4, 4,
		5, 5, 5, 5,
		6, 6, 6, 6,
		7, 7, 7, 7
	};
	static const uint32_t Rw3_Idx[16] = {
		8, 8, 8, 8,
		9, 9, 9, 9,
		10, 10, 10, 10,
		11, 11, 11, 11
	};
	 
	LMM[0][0][0]  = W[0];	
	LMM[0][1][0]  = W[1];	
	LMM[1][0][0]  = W[2];	
	LMM[1][1][0]  = W[3];	
	 
	LMM[2][0][0]  = W[4];	
	LMM[2][1][0]  = W[5];	
	LMM[3][0][0]  = W[6];	
	LMM[3][1][0]  = W[7];	
	 
	LMM[6][0][0]  = W[8];	
	LMM[6][1][0]  = W[9];	
	LMM[7][0][0]  = W[10];	
	LMM[7][1][0]  = W[11];	
 
	LMM[4][0][0]  = W[12];	
	LMM[4][1][0]  = W[13];	
	LMM[5][0][0]  = W[14];	
	LMM[5][1][0]  = W[15];	
	 
	LMM[8][0][0]  = H[0];
	LMM[8][1][0]  = H[1];
	LMM[9][0][0]  = H[2];
	LMM[9][1][0]  = H[3];

	LMM[10][0][0] = H[4];
	LMM[10][1][0] = H[5];
	LMM[11][0][0] = H[6];
	LMM[11][1][0] = H[7];

	LMM[14][0][0] = CS[0];
	LMM[14][1][0] = CS[1];
	LMM[15][0][0] = CS[2];
	LMM[15][1][0] = CS[3];

	LMM[12][0][0] = T[0] ^ CS[4];
	LMM[12][1][0] = T[0] ^ CS[5];
	LMM[13][0][0] = T[1] ^ CS[6];
	LMM[13][1][0] = T[1] ^ CS[7];

#if defined(ARMZYNQ)
	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	
	U32 *A;
	A = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if(j==0)
				A[i*8 +j] = LMM[j/2][j%2][0];
			else
				A[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	
	U32 *B;
	B = (U32*)(membase + ROW1_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			B[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}
	
	U32 *C;
	C = (U32*)(membase + ROW2_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			C[i*8 +j] = LMM[j/2+8][j%2][0];
		}
	}
	
	U32 *D;
	D = (U32*)(membase + ROW3_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			D[i*8 +j] = LMM[j/2+12][j%2][0];
		}
	}
	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[8][0][0],LMM[8][1][0],LMM[9][0][0],LMM[9][1][0],LMM[10][0][0],LMM[10][1][0],LMM[11][0][0],LMM[11][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[11][0][0],LMM[11][1][0],LMM[10][0][0],LMM[10][1][0],LMM[9][0][0],LMM[9][1][0],LMM[8][0][0],LMM[8][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;	
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",24);			
	#endif
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			/*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 1 ***///
			mop32(OP_LDW, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[5], /*ALU in*/ 0,0,PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[2][0], PE_out[2][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[6], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 2 ***///
			mop32(OP_LDW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5],PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][2], PE_out[5][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[10], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[11], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ PE_out[4][0], PE_out[4][1],0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			///*** row 3 ***///

			mop32(OP_LDW, 0, PE_out[12], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][0], PE_out[8][1], PE_out[9][0], PE_out[9][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5], PE_out[8][6], PE_out[8][7]);
			mop32(OP_LDW, 0, PE_out[13], /*ALU in*/ 0,0,PE_out[11][0], PE_out[11][1], /*BUFF 16to1 in*/ PE_out[10][0], PE_out[10][1],0,0, /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5], PE_out[9][6], PE_out[9][7]);
			mop32(OP_LDW, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			mop32(OP_LDW, 0, PE_out[15], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,PE_out[11][2],PE_out[11][3], /*BUFF 8to1 in*/ PE_out[11][4], PE_out[11][5], 0, 0);		
			
		}
		else if(i==1){
			exe32(OP_NOP, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
			exe32(OP_NOP, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][2], PE_out[13][3], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
			exe32(OP_NOP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][0], PE_out[14][1], PE_out[15][0], PE_out[15][1],  /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
			exe32(OP_NOP, 0, PE_out[3],/*ALU in*/ 0,0,PE_out[13][0],PE_out[13][1], /*BUFF 16to1 in*/ PE_out[12][0], PE_out[12][1], 0, 0, /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][6], PE_out[15][7]);
			///*** row 1 ***///
			exe32(OP_NOP, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
			exe32(OP_NOP, 0, PE_out[5], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][4], PE_out[1][5], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
			exe32(OP_NOP, 0, PE_out[6], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
			exe32(OP_NOP, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][0], PE_out[3][1], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);
			///*** row 2 ***///
			exe32(OP_NOP, 0, PE_out[8],  /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]); 
			exe32(OP_NOP, 0, PE_out[9],  /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
			exe32(OP_NOP, 0, PE_out[10], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
			exe32(OP_NOP, 0, PE_out[11], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);			
			///*** row 3 ***///

			exe32(OP_NOP, 0, PE_out[12], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
			exe32(OP_NOP, 0, PE_out[13], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
			exe32(OP_NOP, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
			exe32(OP_NOP, 0, PE_out[15], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);
		}
		else if(i==73){
					// printf("HERE!!!\n");
					///*** row 0 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
					exe32(OP, 0, PE_out[0], /*ALU in*/ PE_out[12][3],PE_out[12][1],PE_out[13][4],PE_out[15][4], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[1], /*ALU in*/ PE_out[13][3],PE_out[13][1],PE_out[13][5],PE_out[15][5], /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[2], /*ALU in*/ PE_out[14][3],PE_out[14][1],PE_out[13][6],PE_out[15][6], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[3], /*ALU in*/ PE_out[15][3],PE_out[15][1],PE_out[13][7],PE_out[14][7], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ 0,0,0,0);

					///*** row 1 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP, 0, PE_out[4], /*ALU in*/ PE_out[0][0],PE_out[0][1],PE_out[0][2],PE_out[0][3], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[5], /*ALU in*/ PE_out[1][0],PE_out[1][1],PE_out[1][2],PE_out[1][3], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[6], /*ALU in*/ PE_out[2][0],PE_out[2][1],PE_out[2][2],PE_out[2][3], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[7], /*ALU in*/ PE_out[3][0],PE_out[3][1],PE_out[3][2],PE_out[3][3], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);

					///*** row 2 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					mop32(OP_STW, 0, PE_out[8], /*ALU in*/ PE_out[4][1],PE_out[5][1],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					mop32(OP_STW, 0, PE_out[9], /*ALU in*/ PE_out[6][1],PE_out[7][1],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					mop32(OP_STW, 0, PE_out[10], /*ALU in*/ PE_out[4][3],PE_out[5][3],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					mop32(OP_STW, 0, PE_out[11], /*ALU in*/ PE_out[6][3],PE_out[7][3],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);

					/*** row 3 ***///
					// /*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					// exe32(OP, 0, PE_out[12], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					// exe32(OP, 0, PE_out[13], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					// exe32(OP, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
					// exe32(OP, 0, PE_out[15], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);

					
		}
		else
		{
			// printf("CS[sigma[k-2][1]] = %02x, %d\n", sigma[i-2][1], i-2);
		if((i-2)%5 == 0){
				if(i==2){
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP, CS[sigma[(i-2)/5][1]], PE_out[0],/*ALU in*/ 0,0,0,PE_out[12][8], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
					exe32(OP, CS[sigma[(i-2)/5][3]], PE_out[1],/*ALU in*/ 0,0,0,PE_out[12][10], /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
					exe32(OP, CS[sigma[(i-2)/5][5]], PE_out[2],/*ALU in*/ 0,0,0,PE_out[13][8], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6] , PE_out[14][7],  /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
					exe32(OP, CS[sigma[(i-2)/5][7]], PE_out[3],/*ALU in*/ 0,0,0,PE_out[13][10], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);

					///*** row 1 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
					exe32(OP, 0, PE_out[4], /*ALU in*/ PE_out[0][4],PE_out[1][4],PE_out[0][1],PE_out[3][4], /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
					exe32(OP, 0, PE_out[5], /*ALU in*/ PE_out[0][5],PE_out[1][5],PE_out[1][1],PE_out[3][5], /*BUFF 16to1 in*/ PE_out[1][4], PE_out[1][5], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
					exe32(OP, 0, PE_out[6], /*ALU in*/ PE_out[0][6],PE_out[1][6],PE_out[2][1],PE_out[3][6], /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
					exe32(OP, 0, PE_out[7], /*ALU in*/ PE_out[0][7],PE_out[1][7],PE_out[3][1],PE_out[3][7], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

					///*** row 2 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/16,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

					exe32(OP, 0, PE_out[8],  /*ALU in*/ PE_out[4][1],PE_out[6][2],PE_out[7][2],0, /*BUFF 16to1 in*/ PE_out[4][2], PE_out[5][2], 0, 0, /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]); 
					exe32(OP, 0, PE_out[9],  /*ALU in*/ PE_out[5][1],0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
					exe32(OP, 0, PE_out[10], /*ALU in*/ PE_out[6][1],0,0,0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
					exe32(OP, 0, PE_out[11], /*ALU in*/ PE_out[7][1],0,0,0, /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);			

					///*** row 3 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);	

					exe32(OP, 0, PE_out[12], /*ALU in*/ PE_out[8][2],PE_out[10][4],0,0, /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][3], PE_out[8][0], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
					exe32(OP, 0, PE_out[13], /*ALU in*/ PE_out[9][2],PE_out[10][5],0,0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
					exe32(OP, 0, PE_out[14], /*ALU in*/ PE_out[10][2],PE_out[10][6],0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
					exe32(OP, 0, PE_out[15], /*ALU in*/ PE_out[11][2],PE_out[10][7],PE_out[9][2],PE_out[10][2], /*BUFF 16to1 in*/ PE_out[8][2], 0, 0, PE_out[11][2], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);
				}
				else if(i<72)
				{
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP, CS[sigma[(i-2)/5][1]], PE_out[0],/*ALU in*/ 0,0,0,PE_out[12+sigma[(i-2)/5][0]/4][8+sigma[(i-2)/5][0]%4], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
					exe32(OP, CS[sigma[(i-2)/5][3]], PE_out[1],/*ALU in*/ PE_out[12][2],0,PE_out[15][2],PE_out[12+sigma[(i-2)/5][2]/4][8+sigma[(i-2)/5][2]%4], /*BUFF 16to1 in*/ 0, 0, PE_out[13][2], PE_out[14][2], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
					exe32(OP, CS[sigma[(i-2)/5][5]], PE_out[2],/*ALU in*/ 0,0,0,PE_out[12+sigma[(i-2)/5][4]/4][8+sigma[(i-2)/5][4]%4], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][3] , PE_out[14][1],  /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
					exe32(OP, CS[sigma[(i-2)/5][7]], PE_out[3],/*ALU in*/ 0,0,0,PE_out[12+sigma[(i-2)/5][6]/4][8+sigma[(i-2)/5][6]%4], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);

					///*** row 1 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
					exe32(OP, 0, PE_out[4], /*ALU in*/ PE_out[0][4],PE_out[1][0],PE_out[0][1],PE_out[3][4], /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
					exe32(OP, 0, PE_out[5], /*ALU in*/ PE_out[0][5],PE_out[1][2],PE_out[1][1],PE_out[3][5], /*BUFF 16to1 in*/ PE_out[1][0], PE_out[1][2], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
					exe32(OP, 0, PE_out[6], /*ALU in*/ PE_out[0][6],PE_out[1][6],PE_out[2][1],PE_out[3][6], /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
					exe32(OP, 0, PE_out[7], /*ALU in*/ PE_out[0][7],PE_out[1][7],PE_out[3][1],PE_out[3][7], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

					///*** row 2 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/16,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

					exe32(OP, 0, PE_out[8],  /*ALU in*/ PE_out[4][1],PE_out[6][2],PE_out[7][2],0, /*BUFF 16to1 in*/ PE_out[4][2], PE_out[5][2], 0, 0, /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]); 
					exe32(OP, 0, PE_out[9],  /*ALU in*/ PE_out[5][1],0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
					exe32(OP, 0, PE_out[10], /*ALU in*/ PE_out[6][1],0,0,0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
					exe32(OP, 0, PE_out[11], /*ALU in*/ PE_out[7][1],0,0,0, /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);			

					///*** row 3 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);	

					exe32(OP, 0, PE_out[12], /*ALU in*/ PE_out[8][2],PE_out[10][4],0,0, /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][3], PE_out[8][0], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
					exe32(OP, 0, PE_out[13], /*ALU in*/ PE_out[9][2],PE_out[10][5],0,0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
					exe32(OP, 0, PE_out[14], /*ALU in*/ PE_out[10][2],PE_out[10][6],0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
					exe32(OP, 0, PE_out[15], /*ALU in*/ PE_out[11][2],PE_out[10][7],PE_out[9][2],PE_out[10][2], /*BUFF 16to1 in*/ PE_out[8][2], 0, 0, PE_out[11][2], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);
				}
				else if(i==72){
					///*** row 0 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP_NOP, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[1],/*ALU in*/ 0,0,PE_out[15][2], PE_out[12][2], /*BUFF 16to1 in*/ 0, 0, PE_out[13][2], PE_out[14][2], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][3] , PE_out[14][1],  /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ 0,0,0,0);

					///*** row 1 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP_NOP, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[5], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][0], PE_out[1][1], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[6], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP_NOP, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ 0,0,0,0);

					///*** row 2 ***///
					mop32(OP_LDW, 0, PE_out[8] ,/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
					mop32(OP_LDW, 0, PE_out[9] ,/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
					mop32(OP_LDW, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
					mop32(OP_LDW, 0, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					///*** row 3 ***///
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
					exe32(OP, 0, PE_out[12], /*ALU in*/ PE_out[8][0],PE_out[10][0],PE_out[8][4],PE_out[10][4], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[13], /*ALU in*/ PE_out[8][1],PE_out[10][1],PE_out[8][5],PE_out[10][5], /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[14], /*ALU in*/ PE_out[9][0],PE_out[11][0],PE_out[8][6],PE_out[10][6], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ 0,0,0,0);
					exe32(OP, 0, PE_out[15], /*ALU in*/ PE_out[9][1],PE_out[11][1],PE_out[8][7],PE_out[10][7], /*BUFF 16to1 in*/ PE_out[11][4], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ 0,0,0,0);
				}
			}
			else if((i-2)%5 == 1){
				///*** row 0 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/12,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][2],0,PE_out[13][4],0, /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
				exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][2],0,PE_out[13][5],0, /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[14][2],PE_out[12][2],PE_out[13][6],PE_out[13][2], /*BUFF 16to1 in*/ 0, 0, PE_out[14][2] , PE_out[15][2],  /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[15][2],0,PE_out[13][7],0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][0], PE_out[15][1], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);

				///*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, CS[sigma[(i-2)/5][0]], PE_out[4], /*ALU in*/ 0,0,0,PE_out[0+sigma[(i-2)/5][1]/4][8+sigma[(i-2)/5][1]%4], /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
				exe32(OP, CS[sigma[(i-2)/5][2]], PE_out[5], /*ALU in*/ PE_out[0][2],0,PE_out[3][2],PE_out[0+sigma[(i-2)/5][3]/4][8+sigma[(i-2)/5][3]%4], /*BUFF 16to1 in*/ 0, PE_out[1][2], PE_out[2][2], 0, /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
				exe32(OP, CS[sigma[(i-2)/5][4]], PE_out[6], /*ALU in*/ 0,0,0,PE_out[0+sigma[(i-2)/5][5]/4][8+sigma[(i-2)/5][5]%4], /*BUFF 16to1 in*/ PE_out[2][3], PE_out[2][1], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP, CS[sigma[(i-2)/5][6]], PE_out[7], /*ALU in*/ 0,0,0,PE_out[0+sigma[(i-2)/5][7]/4][8+sigma[(i-2)/5][7]%4], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

				///*** row 2 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);

				exe32(OP, 0, PE_out[8],  /*ALU in*/ PE_out[4][4],PE_out[5][2],PE_out[4][1],PE_out[7][4], /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]); 
				exe32(OP, 0, PE_out[9],  /*ALU in*/ PE_out[4][5],PE_out[5][5],PE_out[5][1],PE_out[7][5], /*BUFF 16to1 in*/ PE_out[5][2], PE_out[5][5], PE_out[5][6], PE_out[5][0], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
				exe32(OP, 0, PE_out[10], /*ALU in*/ PE_out[4][6],PE_out[5][6],PE_out[6][1],PE_out[7][6], /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
				exe32(OP, 0, PE_out[11], /*ALU in*/ PE_out[4][7],PE_out[5][0],PE_out[7][1],PE_out[7][7], /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);			

				///*** row 3 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/8,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[12], /*ALU in*/ PE_out[8][1],PE_out[10][2],PE_out[11][2],0, /*BUFF 16to1 in*/ PE_out[8][2], PE_out[9][2], 0, 0, /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
				exe32(OP, 0, PE_out[13], /*ALU in*/ PE_out[9][1],0,0,0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
				exe32(OP, 0, PE_out[14], /*ALU in*/ PE_out[10][1],0,0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
				exe32(OP, 0, PE_out[15], /*ALU in*/ PE_out[11][1],0,0,0, /*BUFF 16to1 in*/ PE_out[11][4], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);
			}
			else if((i-2)%5 == 2){
				///*** row 0 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);	

				exe32(OP, 0, PE_out[0], /*ALU in*/ PE_out[12][2],PE_out[14][4],0,0, /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][3], PE_out[12][0], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
				exe32(OP, 0, PE_out[1], /*ALU in*/ PE_out[13][2],PE_out[14][5],0,0, /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				exe32(OP, 0, PE_out[2], /*ALU in*/ PE_out[14][2],PE_out[14][6],0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				exe32(OP, 0, PE_out[3], /*ALU in*/ PE_out[15][2],PE_out[14][7],PE_out[13][2],PE_out[14][2], /*BUFF 16to1 in*/ PE_out[12][2], 0, 0, PE_out[15][2], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);

				///*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],0,PE_out[1][4],0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
				exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][2],0,PE_out[1][5],0, /*BUFF 16to1 in*/ PE_out[1][4], PE_out[1][5], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
				exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][2],PE_out[0][2],PE_out[1][6],PE_out[1][2], /*BUFF 16to1 in*/ 0, 0, PE_out[2][2] , PE_out[3][2],  /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][2],0,PE_out[1][7],0, /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][0], PE_out[3][1], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

				///*** row 2 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, CS[sigma[(i-2)/5][9]], PE_out[8], /*ALU in*/ 0,0,0,PE_out[4+sigma[(i-2)/5][8]/4][8+sigma[(i-2)/5][8]%4], /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]);
				exe32(OP, CS[sigma[(i-2)/5][11]], PE_out[9], /*ALU in*/ PE_out[4][2],0,PE_out[7][2],PE_out[4+sigma[(i-2)/5][10]/4][8+sigma[(i-2)/5][10]%4], /*BUFF 16to1 in*/ 0, PE_out[5][2], PE_out[6][2], 0, /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
				exe32(OP, CS[sigma[(i-2)/5][13]], PE_out[10], /*ALU in*/ 0,0,0,PE_out[4+sigma[(i-2)/5][12]/4][8+sigma[(i-2)/5][12]%4], /*BUFF 16to1 in*/ PE_out[6][3], PE_out[6][1], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
				exe32(OP, CS[sigma[(i-2)/5][15]], PE_out[11], /*ALU in*/ 0,0,0,PE_out[4+sigma[(i-2)/5][14]/4][8+sigma[(i-2)/5][14]%4], /*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);

				///*** row 3 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[12], /*ALU in*/ PE_out[8][4],PE_out[9][5],PE_out[8][1],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
				exe32(OP, 0, PE_out[13], /*ALU in*/ PE_out[8][5],PE_out[9][6],PE_out[9][1],PE_out[11][4], /*BUFF 16to1 in*/ PE_out[9][2], PE_out[9][5], PE_out[9][6], PE_out[9][0], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
				exe32(OP, 0, PE_out[14], /*ALU in*/ PE_out[8][6],PE_out[9][0],PE_out[10][1],PE_out[11][5], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
				exe32(OP, 0, PE_out[15], /*ALU in*/ PE_out[8][7],PE_out[9][2],PE_out[11][1],PE_out[11][6], /*BUFF 16to1 in*/ PE_out[11][4], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);

			}
			else if((i-2)%5 == 3){
				///*** row 0 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/16,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[0],  /*ALU in*/ PE_out[12][1],PE_out[14][2],PE_out[15][2],0, /*BUFF 16to1 in*/ PE_out[12][2], PE_out[13][2], 0, 0, /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]); 
				exe32(OP, 0, PE_out[1],  /*ALU in*/ PE_out[13][1],0,0,0, /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				exe32(OP, 0, PE_out[2], /*ALU in*/ PE_out[14][1],0,0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				exe32(OP, 0, PE_out[3], /*ALU in*/ PE_out[15][1],0,0,0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);

				///*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);	

				exe32(OP, 0, PE_out[4], /*ALU in*/ PE_out[0][2],PE_out[2][6],0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][3], PE_out[0][0], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
				exe32(OP, 0, PE_out[5], /*ALU in*/ PE_out[1][2],PE_out[2][7],0,0, /*BUFF 16to1 in*/ PE_out[1][4], PE_out[1][5], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
				exe32(OP, 0, PE_out[6], /*ALU in*/ PE_out[2][2],PE_out[2][4],0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP, 0, PE_out[7], /*ALU in*/ PE_out[3][2],PE_out[2][5],PE_out[1][2],PE_out[2][2], /*BUFF 16to1 in*/ 0, 0, PE_out[3][2], PE_out[0][2], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);		

				///*** row 2 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/12,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][2],0,PE_out[5][5],0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]);
				exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][2],0,PE_out[5][6],0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
				exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][2],PE_out[4][2],PE_out[5][7],PE_out[5][2], /*BUFF 16to1 in*/ PE_out[6][2] , PE_out[7][2], 0,0,  /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
				exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][2],0,PE_out[5][4],0, /*BUFF 16to1 in*/ PE_out[7][0], PE_out[7][1], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);

				///*** row 3 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
				// printf("[%d][%d] -> CS[%d] = %08x\n ", 8+sigma[(i-2)/5][9]/4, 8+sigma[(i-2)/5][9]%4, sigma[(i-2)/5][8], CS[sigma[(i-2)/5][8]]);
				exe32(OP, CS[sigma[(i-2)/5][8]], PE_out[12], /*ALU in*/ 0,0,0,PE_out[8+sigma[(i-2)/5][9]/4][8+sigma[(i-2)/5][9]%4], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
				exe32(OP, CS[sigma[(i-2)/5][10]], PE_out[13], /*ALU in*/ PE_out[8][2],0,PE_out[11][2],PE_out[8+sigma[(i-2)/5][11]/4][8+sigma[(i-2)/5][11]%4], /*BUFF 16to1 in*/ 0, 0, PE_out[9][2], PE_out[10][2], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
				exe32(OP, CS[sigma[(i-2)/5][12]], PE_out[14], /*ALU in*/ 0,0,0,PE_out[8+sigma[(i-2)/5][13]/4][8+sigma[(i-2)/5][13]%4], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][3], PE_out[10][1], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
				exe32(OP, CS[sigma[(i-2)/5][14]], PE_out[15], /*ALU in*/ 0,0,0,PE_out[8+sigma[(i-2)/5][15]/4][8+sigma[(i-2)/5][15]%4], /*BUFF 16to1 in*/ PE_out[11][4], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);
			}
			else if((i-2)%5 == 4){

				///*** row 0 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[0], /*ALU in*/ PE_out[12][4],PE_out[13][2],PE_out[12][1],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], PE_out[12][11]);
				exe32(OP, 0, PE_out[1], /*ALU in*/ PE_out[12][5],PE_out[13][6],PE_out[13][1],PE_out[15][4], /*BUFF 16to1 in*/ PE_out[13][0], PE_out[13][2], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				exe32(OP, 0, PE_out[2], /*ALU in*/ PE_out[12][6],PE_out[13][7],PE_out[14][1],PE_out[15][5], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				exe32(OP, 0, PE_out[3], /*ALU in*/ PE_out[12][7],PE_out[13][0],PE_out[15][1],PE_out[15][6], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);
				
				///*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/8,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[4],  /*ALU in*/ PE_out[0][1],PE_out[2][2],PE_out[3][2],0, /*BUFF 16to1 in*/ PE_out[0][2], PE_out[1][2], 0, 0, /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]); 
				exe32(OP, 0, PE_out[5],  /*ALU in*/ PE_out[1][1],0,0,0, /*BUFF 16to1 in*/ PE_out[1][4], PE_out[1][5], PE_out[1][6], PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
				exe32(OP, 0, PE_out[6], /*ALU in*/ PE_out[2][1],0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP, 0, PE_out[7], /*ALU in*/ PE_out[3][1],0,0,0, /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

				///*** row 2 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);	

				exe32(OP, 0, PE_out[8], /*ALU in*/ PE_out[4][2],PE_out[6][6],0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][3], PE_out[4][0], /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]);
				exe32(OP, 0, PE_out[9], /*ALU in*/ PE_out[5][2],PE_out[6][7],0,0, /*BUFF 16to1 in*/ PE_out[5][4], PE_out[5][5], PE_out[5][6], PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
				exe32(OP, 0, PE_out[10], /*ALU in*/ PE_out[6][2],PE_out[6][4],0,0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
				exe32(OP, 0, PE_out[11], /*ALU in*/ PE_out[7][2],PE_out[6][5],PE_out[5][2],PE_out[6][2], /*BUFF 16to1 in*/ 0, 0, PE_out[7][2], PE_out[4][2], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);

				///*** row 3 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);

				exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][2],0,PE_out[9][5],0, /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);
				exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][2],0,PE_out[9][6],0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5], PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
				exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][2],PE_out[8][2],PE_out[9][7],PE_out[9][2], /*BUFF 16to1 in*/ PE_out[10][2] , PE_out[11][2], 0,0,  /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
				exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][2],0,PE_out[9][4],0, /*BUFF 16to1 in*/ PE_out[11][0], PE_out[11][1], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);	
				//printf("END OF LOOP!!!\n\n");	
			}
			// /*** row 0 ***///
			// 	// /**/OP_Immidiate = K[i-1];
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,CS[sigma[k-2][1]],0,PE_out[12][8], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9], PE_out[12][10], PE_out[12][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,CS[sigma[k-2][1]],0,PE_out[12][8], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9], PE_out[12][10], PE_out[12][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,CS[sigma[k-2][1]],0,PE_out[12][8], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9], PE_out[12][10], PE_out[12][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,CS[sigma[k-2][1]],0,PE_out[12][8], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][6], PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9], PE_out[12][10], PE_out[12][11]);

			// 	// /*** row 1 ***///
			// 	// OP_Immidiate = K[i-1];
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			// 	exe32(OP, K[i-1], PE_out[4],/*ALU in*/ PE_out[0][2],0, PE_out[1][8], PE_out[0][2], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], PE_out[0][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			// 	exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][1],PE_out[0][11],PE_out[1][1],0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], PE_out[1][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][1],PE_out[1][11], PE_out[2][4], 0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/15,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			// 	// printf("6164e387 ROTL 15 = %08x, 6164e387 ROTL 15 = %08x, XOR = %08x\n", ROTL(0x6164e387,15), ROTL(0x6164e387,23), ROTL(0x6164e387,15)^ ROTL(0x6164e387,23));
			// 	exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][1], 0, PE_out[3][1], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][10], PE_out[3][11]);


			// 	// /*** row 2 ***///
			// 	// OP_Immidiate = K[i-1];
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][1],PE_out[5][2], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4][8], PE_out[4][9], PE_out[4][10], PE_out[4][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/9,/*SRU2*/OP_ROL,/*SRU2_IM*/19,/*LU3*/OP_NOP); // Find new C and new G
			// 	exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[4][9],0, 0, PE_out[5][9], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5][8], PE_out[5][9], PE_out[5][10], PE_out[5][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[4][2],PE_out[6][2],PE_out[7][4],0, /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8], PE_out[6][9], PE_out[6][10], PE_out[6][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_ROL,/*SRU2_IM*/7,/*LU3*/OP_XOR);
			// 	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0], 0, PE_out[7][1], PE_out[6][7],/*BUFF 16to1 in*/ PE_out[7][4], PE_out[7][5], PE_out[7][6], PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8], PE_out[7][9], PE_out[7][10], PE_out[7][11]);


			// 	// /*** row 3 ***///
			// 	// OP_Immidiate = K[i-1];
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[12],/*ALU in*/ 0, 0, PE_out[10][2], 0, /*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][8],PE_out[9][2],PE_out[8][10], /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], PE_out[8][11]);	// Wait C
			// 	// printf("\n%08x\n", P0(0xc067debb));
			// 	/*--*/OP = CUSTOM_OP(OP_P0);
			// 	exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[10][2],0,0,0, /*BUFF 16to1 in*/ 0,PE_out[9][8],PE_out[9][1],PE_out[9][10], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP); 
			// 	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0, 0, 0, /*BUFF 16to1 in*/ PE_out[10][5], PE_out[10][6], PE_out[10][7], PE_out[10][0], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
			// 	/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			// 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1], 0, PE_out[11][6], 0,/*BUFF 16to1 in*/ PE_out[11][5], PE_out[11][6], PE_out[11][7], 0,/*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]); // 0xd317e0c4 ^ 0x0000000a

		}
	}
		
	
	#if defined(ARMZYNQ)
	
	dma_write(ROW0_LSB_BASE_PHYS,8);
	dma_write(ROW1_LSB_BASE_PHYS,8);
	dma_write(ROW2_LSB_BASE_PHYS,8);
	dma_write(ROW3_LSB_BASE_PHYS,8);
	
	*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    while(1){
		if(*(CGRA_info.ctx_pe_mmap+(FINISH_BASE_IP)))
			break;
	}
	//sleep(1);
	//printf("Done!\n");
	
	dma_read(ROW2_LSB_BASE_PHYS,8);
	
		for (i=0; i<8; i++) {
		printf("Output %d = ",i);
		for (j=0; j<8; j++) {
			printf("%08x ",C[i*8+j]);
		}
		printf("\n");
		}

	#endif
	
	#if defined(ARMSIL)
		printf("\n Hash output =");
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[8][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[9][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[10][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[11][i][0]);
		}
		printf("\n");
	#endif
	Final();
	
	return 0;
}
