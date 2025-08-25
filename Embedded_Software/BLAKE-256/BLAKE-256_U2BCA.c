#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "./U2BCA_Opt_BLAKE.h"
#include "./U2BCAlib_Opt_BLAKE.c"

#define N 4

int main(){
	 int k,i,j, LOOP;

	// LOOP = 16;
	LOOP = 2;

	uint64_t OP2;
	uint32_t LMM[3][4][4];
	//uint32_t ROM[16][(LOOP*4)*4];
	uint32_t *PE_out[(LOOP*4)*16];
	uint32_t **ROM;
	//uint32_t **PE_out;
	
	//PE_out =  (uint32_t **) calloc(LOOP*4*16,sizeof(uint32_t));
	for (i = 0; i < LOOP*4*16; i ++){
		PE_out[i] = (uint32_t *)calloc(12,sizeof(uint32_t));
	}
	
	ROM = (uint32_t **) calloc(16,sizeof(uint32_t));
	for (i = 0; i < 16; i ++){
		ROM[i] = (uint32_t *) calloc(LOOP*4*16,sizeof(uint32_t));
	}
	
	
	#if defined(ARMZYNQ)
	
	#endif
	
	uint32_t H[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	//uint32_t W[16] = {0x61626380, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001, 0x00000000, 0x00000018};
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
	
	LMM[0][0][0] = H[0];
	LMM[0][0][1] = H[1];
	LMM[0][0][2] = H[2];
	LMM[0][0][3] = H[3];

	LMM[0][1][0] = H[4];
	LMM[0][1][1] = H[5];
	LMM[0][1][2] = H[6];
	LMM[0][1][3] = H[7];
	
	LMM[0][2][0] = CS[0];
	LMM[0][2][1] = CS[1];
	LMM[0][2][2] = CS[2];
	LMM[0][2][3] = CS[3];

	LMM[0][3][0] = T[0] ^ CS[4];
	LMM[0][3][1] = T[0] ^ CS[5];
	LMM[0][3][2] = T[1] ^ CS[6];
	LMM[0][3][3] = T[1] ^ CS[7];
	
	LMM[1][0][0] = W[0];
	LMM[1][0][1] = W[1];
	LMM[1][0][2] = W[2];
	LMM[1][0][3] = W[3];

	LMM[1][1][0] = W[4];
	LMM[1][1][1] = W[5];
	LMM[1][1][2] = W[6];
	LMM[1][1][3] = W[7];

	LMM[1][2][0] = W[8];
	LMM[1][2][1] = W[9];
	LMM[1][2][2] = W[10];
	LMM[1][2][3] = W[11];

	LMM[1][3][0] = W[12];
	LMM[1][3][1] = W[13];
	LMM[1][3][2] = W[14];
	LMM[1][3][3] = W[15];

	
	
for (k = 0; k < LOOP; k++){
	
	if( k == 0){
		//-----------------------------------------------------------------------------------------------------------------------//
		//  INITIAL STEP																										 //
		//-----------------------------------------------------------------------------------------------------------------------//
		i = k;
		///*** row 0 ***///
		mop(OP_LDW, 0, 0, LMM[0][0], PE_out[0+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[0][1], PE_out[1+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[0][2], PE_out[2+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[0][3], PE_out[3+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		///*** row 1 ***///
		mop(OP_LDW, 0, 0, LMM[1][0], PE_out[4+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][0], PE_out[0+i*NUM_PE][1], PE_out[0+i*NUM_PE][2], PE_out[0+i*NUM_PE][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[1][1], PE_out[5+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][0], PE_out[1+i*NUM_PE][1], PE_out[1+i*NUM_PE][2], PE_out[1+i*NUM_PE][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[1][2], PE_out[6+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][0], PE_out[2+i*NUM_PE][1], PE_out[2+i*NUM_PE][2], PE_out[2+i*NUM_PE][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[1][3], PE_out[7+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][0], PE_out[3+i*NUM_PE][1], PE_out[3+i*NUM_PE][2], PE_out[3+i*NUM_PE][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		///*** row 2 ***///
		exe(OP_NOP,OP_NOP, PE_out[8+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][0], PE_out[4+i*NUM_PE][1], PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][3], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][4], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[9+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][0], PE_out[5+i*NUM_PE][1], PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][3], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][4], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[10+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][0], PE_out[6+i*NUM_PE][1], PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][3], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][4], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[11+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][0], PE_out[7+i*NUM_PE][1], PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][3], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][4], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][7]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][5], PE_out[8+i*NUM_PE][6], PE_out[8+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][5], PE_out[9+i*NUM_PE][6], PE_out[9+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][5], PE_out[10+i*NUM_PE][6], PE_out[10+i*NUM_PE][7]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][5], PE_out[11+i*NUM_PE][6], PE_out[11+i*NUM_PE][7]);
	
	}
	else if( k == LOOP-1){
		//-----------------------------------------------------------------------------------------------------------------------//
		//  FINALIZATION STEP																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		i = (k-1)*4+1;
		///*** row 0 ***///
		mop(OP_LDW, 0, 0, LMM[0][0], PE_out[0+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][0], PE_out[12+(i-1)*NUM_PE][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_LDW, 0, 0, LMM[0][1], PE_out[1+i*NUM_PE], /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][0], PE_out[13+(i-1)*NUM_PE][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		exe(OP_NOP,OP_NOP, PE_out[2+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][0], PE_out[14+(i-1)*NUM_PE][1],0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		exe(OP_NOP,OP_NOP, PE_out[3+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][0], PE_out[15+(i-1)*NUM_PE][1],0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][0],PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][5],PE_out[0+i*NUM_PE][4], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		exe(OP_NOP,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][2],PE_out[0+i*NUM_PE][3],PE_out[1+i*NUM_PE][5],PE_out[1+i*NUM_PE][4], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		exe(OP_NOP,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][0],PE_out[1+i*NUM_PE][1],PE_out[2+i*NUM_PE][5],PE_out[2+i*NUM_PE][4], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		exe(OP_NOP,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][2],PE_out[1+i*NUM_PE][3],PE_out[3+i*NUM_PE][5],PE_out[3+i*NUM_PE][4], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
		///*** row 1 ***///
		mop(OP_STW, 0, 0, LMM[2][0], PE_out[4+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_STW, 0, 0, LMM[2][1], PE_out[5+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_STW, 0, 0, LMM[2][2], PE_out[6+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		mop(OP_STW, 0, 0, LMM[2][3], PE_out[7+i*NUM_PE], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
	}
	else if(k == LOOP - 2){
		i = (k-1)*4+1;
		j = 0;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j+1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 2;
		ROM[1][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 4;
		ROM[2][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
		j = 6;
		ROM[3][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,16,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,12,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[6+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[7+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[4+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[5+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
		
		//-----------------------------------------------------------------------------------------------------------------------//
		//  2/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1){
			i = k+1;
		}
		else{
			i = (k-1)*4+2;
			printf(" i = %d\n",i);
		}
		
		j = 1;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j-1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 3;
		ROM[1][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 5;
		ROM[2][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
		j = 7;
		ROM[3][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,8,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,7,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[4+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[5+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[6+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[7+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
		
		//-----------------------------------------------------------------------------------------------------------------------//
		//  3/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1)
			i = k+2;
		else
			i = (k-1)*4+3;
		
		j = 8;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j+1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 10;
		ROM[1][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 12;
		ROM[2][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
		j = 14;
		ROM[3][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,16,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,12,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[6+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[7+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[4+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[5+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);

		//-----------------------------------------------------------------------------------------------------------------------//
		//  4/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1)
			i = k+3;
		else
			i = (k-1)*4+4;
		
		j = 9;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j-1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 11;
		ROM[1][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 13;
		ROM[2][i-1] = CS[sigma[k-1][j-1]];
		
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);	
		j = 15;
		ROM[3][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,8,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,7,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[4+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[5+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[6+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[7+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[12+i*NUM_PE],/*ALU in*/ PE_out[8+i*NUM_PE][4],PE_out[9+i*NUM_PE][4],PE_out[11+i*NUM_PE][2],PE_out[10+i*NUM_PE][2], /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP2, PE_out[13+i*NUM_PE],/*ALU in*/ PE_out[10+i*NUM_PE][4],PE_out[11+i*NUM_PE][4],PE_out[9+i*NUM_PE][2],PE_out[8+i*NUM_PE][2], /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP2, PE_out[14+i*NUM_PE],/*ALU in*/ PE_out[11+i*NUM_PE][0],PE_out[8+i*NUM_PE][0],PE_out[10+i*NUM_PE][7],PE_out[9+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP2, PE_out[15+i*NUM_PE],/*ALU in*/ PE_out[9+i*NUM_PE][0],PE_out[10+i*NUM_PE][0],PE_out[8+i*NUM_PE][7],PE_out[11+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
	
	}
	else{
		//-----------------------------------------------------------------------------------------------------------------------//
		//  1/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1){
			i = k;
			j = 0;
			///*** row 0 ***///
			ROM[0][i-1] = CS[sigma[k-1][j+1]];
			printf("k=%d, PE_out[%d][%d] \n", k, Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE, W_Idx[sigma[k-1][j]]);
			/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
			exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
			j = 2;
			ROM[1][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
			j = 4;
			ROM[2][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
			j = 6;
			ROM[3][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
			///*** row 1 ***///
			/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,16,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
			exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[1+i*NUM_PE][4],PE_out[3+i*NUM_PE][4], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[0+i*NUM_PE][5],PE_out[1+i*NUM_PE][5],PE_out[3+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[0+i*NUM_PE][6],PE_out[1+i*NUM_PE][6],PE_out[3+i*NUM_PE][6], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[0+i*NUM_PE][7],PE_out[1+i*NUM_PE][7],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
			///*** row 2 ***///
			/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,12,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
			exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[6+i*NUM_PE][4],0,PE_out[5+i*NUM_PE][4], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[6+i*NUM_PE][5],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[6+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][6], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[6+i*NUM_PE][7],0,PE_out[5+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
			///*** row 3 ***///
			exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
	
		}
		else{
			i = (k-1)*4+1;
			j = 0;
			///*** row 0 ***///
			ROM[0][i-1] = CS[sigma[k-1][j+1]];
			/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
			exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
			j = 2;
			ROM[1][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
			j = 4;
			ROM[2][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
			j = 6;
			ROM[3][i-1] = CS[sigma[k-1][j+1]];
			exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
			///*** row 1 ***///
			/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,16,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
			exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
			exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
			///*** row 2 ***///
			/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,12,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
			exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[6+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[7+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[4+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
			exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[5+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
			///*** row 3 ***///
			exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
			exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
		}
		//-----------------------------------------------------------------------------------------------------------------------//
		//  2/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1){
			i = k+1;
		}
		else{
			i = (k-1)*4+2;
			printf(" i = %d\n",i);
		}
		
		j = 1;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j-1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 3;
		ROM[1][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 5;
		ROM[2][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
		j = 7;
		ROM[3][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,8,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,7,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[4+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[5+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[6+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[7+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
		
		//-----------------------------------------------------------------------------------------------------------------------//
		//  3/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1)
			i = k+2;
		else
			i = (k-1)*4+3;
		
		j = 8;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j+1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 10;
		ROM[1][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 12;
		ROM[2][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);
		j = 14;
		ROM[3][i-1] = CS[sigma[k-1][j+1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,16,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,12,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[6+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[7+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[4+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[5+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);

		//-----------------------------------------------------------------------------------------------------------------------//
		//  4/2 G-function   																									 //
		//-----------------------------------------------------------------------------------------------------------------------//
		if (k == 1)
			i = k+3;
		else
			i = (k-1)*4+4;
		
		j = 9;
		///*** row 0 ***///
		ROM[0][i-1] = CS[sigma[k-1][j-1]];
		/**/ OP2 = OP2_CAT(OP_NOP,OP_XOR,OP_NOP,0,OP_NOP,0,OP_NOP,32,OP_NOP,PASS);
		exe(OP_NOP,OP2, PE_out[0+i*NUM_PE],/*ALU in*/ 0,ROM[0][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[12+(i-1)*NUM_PE][4], PE_out[12+(i-1)*NUM_PE][5], PE_out[12+(i-1)*NUM_PE][6], PE_out[12+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[12+(i-1)*NUM_PE][8], PE_out[12+(i-1)*NUM_PE][9], PE_out[12+(i-1)*NUM_PE][10], PE_out[12+(i-1)*NUM_PE][11]);
		j = 11;
		ROM[1][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[1+i*NUM_PE],/*ALU in*/ 0,ROM[1][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[13+(i-1)*NUM_PE][4], PE_out[13+(i-1)*NUM_PE][5], PE_out[13+(i-1)*NUM_PE][6], PE_out[13+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[13+(i-1)*NUM_PE][8], PE_out[13+(i-1)*NUM_PE][9], PE_out[13+(i-1)*NUM_PE][10], PE_out[13+(i-1)*NUM_PE][11]);
		j = 13;
		ROM[2][i-1] = CS[sigma[k-1][j-1]];
		
		exe(OP_NOP,OP2, PE_out[2+i*NUM_PE],/*ALU in*/ 0,ROM[2][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[14+(i-1)*NUM_PE][4], PE_out[14+(i-1)*NUM_PE][5], PE_out[14+(i-1)*NUM_PE][6], PE_out[14+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[14+(i-1)*NUM_PE][8], PE_out[14+(i-1)*NUM_PE][9], PE_out[14+(i-1)*NUM_PE][10], PE_out[14+(i-1)*NUM_PE][11]);	
		j = 15;
		ROM[3][i-1] = CS[sigma[k-1][j-1]];
		exe(OP_NOP,OP2, PE_out[3+i*NUM_PE],/*ALU in*/ 0,ROM[3][i-1],PE_out[Rw0_Idx[sigma[k-1][j]]+(i-1)*NUM_PE][W_Idx[sigma[k-1][j]]],0, /*BUFF 16to1 in*/ PE_out[15+(i-1)*NUM_PE][4], PE_out[15+(i-1)*NUM_PE][5], PE_out[15+(i-1)*NUM_PE][6], PE_out[15+(i-1)*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[15+(i-1)*NUM_PE][8], PE_out[15+(i-1)*NUM_PE][9], PE_out[15+(i-1)*NUM_PE][10], PE_out[15+(i-1)*NUM_PE][11]);
		///*** row 1 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,8,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD3,OP2, PE_out[4+i*NUM_PE],/*ALU in*/ PE_out[0+i*NUM_PE][1],PE_out[0+i*NUM_PE][4],PE_out[0+i*NUM_PE][5],PE_out[0+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[0+i*NUM_PE][4], PE_out[0+i*NUM_PE][5], PE_out[0+i*NUM_PE][6], PE_out[0+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[0+i*NUM_PE][8], PE_out[0+i*NUM_PE][9], PE_out[0+i*NUM_PE][10], PE_out[0+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[5+i*NUM_PE],/*ALU in*/ PE_out[1+i*NUM_PE][1],PE_out[1+i*NUM_PE][4],PE_out[1+i*NUM_PE][5],PE_out[1+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[1+i*NUM_PE][4], PE_out[1+i*NUM_PE][5], PE_out[1+i*NUM_PE][6], PE_out[1+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[1+i*NUM_PE][8], PE_out[1+i*NUM_PE][9], PE_out[1+i*NUM_PE][10], PE_out[1+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[6+i*NUM_PE],/*ALU in*/ PE_out[2+i*NUM_PE][1],PE_out[2+i*NUM_PE][4],PE_out[2+i*NUM_PE][5],PE_out[2+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[2+i*NUM_PE][4], PE_out[2+i*NUM_PE][5], PE_out[2+i*NUM_PE][6], PE_out[2+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[2+i*NUM_PE][8], PE_out[2+i*NUM_PE][9], PE_out[2+i*NUM_PE][10], PE_out[2+i*NUM_PE][11]);
		exe(OP_ADD3,OP2, PE_out[7+i*NUM_PE],/*ALU in*/ PE_out[3+i*NUM_PE][1],PE_out[3+i*NUM_PE][4],PE_out[3+i*NUM_PE][5],PE_out[3+i*NUM_PE][7], /*BUFF 16to1 in*/ PE_out[3+i*NUM_PE][4], PE_out[3+i*NUM_PE][5], PE_out[3+i*NUM_PE][6], PE_out[3+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[3+i*NUM_PE][8], PE_out[3+i*NUM_PE][9], PE_out[3+i*NUM_PE][10], PE_out[3+i*NUM_PE][11]);
		///*** row 2 ***///
		/**/ OP2 = OP2_CAT(OP_XOR,OP_NOP,OP_ROR,7,OP_NOP,0,OP_NOP,32,OP_NOP,KEEP);
		exe(OP_ADD2,OP2, PE_out[8+i*NUM_PE],/*ALU in*/ PE_out[4+i*NUM_PE][0], PE_out[4+i*NUM_PE][6],0,PE_out[4+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[4+i*NUM_PE][2], PE_out[4+i*NUM_PE][5], PE_out[4+i*NUM_PE][6], PE_out[4+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[4+i*NUM_PE][8], PE_out[4+i*NUM_PE][9], PE_out[4+i*NUM_PE][10], PE_out[4+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[9+i*NUM_PE],/*ALU in*/ PE_out[5+i*NUM_PE][0], PE_out[5+i*NUM_PE][6],0,PE_out[5+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[5+i*NUM_PE][2], PE_out[5+i*NUM_PE][5], PE_out[5+i*NUM_PE][6], PE_out[5+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[5+i*NUM_PE][8], PE_out[5+i*NUM_PE][9], PE_out[5+i*NUM_PE][10], PE_out[5+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[10+i*NUM_PE],/*ALU in*/ PE_out[6+i*NUM_PE][0],PE_out[6+i*NUM_PE][6],0,PE_out[6+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[6+i*NUM_PE][2], PE_out[6+i*NUM_PE][5], PE_out[6+i*NUM_PE][6], PE_out[6+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[6+i*NUM_PE][8], PE_out[6+i*NUM_PE][9], PE_out[6+i*NUM_PE][10], PE_out[6+i*NUM_PE][11]);
		exe(OP_ADD2,OP2, PE_out[11+i*NUM_PE],/*ALU in*/ PE_out[7+i*NUM_PE][0],PE_out[7+i*NUM_PE][6],0,PE_out[7+i*NUM_PE][5], /*BUFF 16to1 in*/ PE_out[7+i*NUM_PE][2], PE_out[7+i*NUM_PE][5], PE_out[7+i*NUM_PE][6], PE_out[7+i*NUM_PE][0], /*BUFF 8to1 in*/ PE_out[7+i*NUM_PE][8], PE_out[7+i*NUM_PE][9], PE_out[7+i*NUM_PE][10], PE_out[7+i*NUM_PE][11]);
		///*** row 3 ***///
		exe(OP_NOP,OP_NOP, PE_out[12+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8+i*NUM_PE][4], PE_out[8+i*NUM_PE][0], PE_out[8+i*NUM_PE][2], PE_out[8+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[8+i*NUM_PE][8], PE_out[8+i*NUM_PE][9], PE_out[8+i*NUM_PE][10], PE_out[8+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[13+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9+i*NUM_PE][4], PE_out[9+i*NUM_PE][0], PE_out[9+i*NUM_PE][2], PE_out[9+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[9+i*NUM_PE][8], PE_out[9+i*NUM_PE][9], PE_out[9+i*NUM_PE][10], PE_out[9+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[14+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10+i*NUM_PE][4], PE_out[10+i*NUM_PE][0], PE_out[10+i*NUM_PE][2], PE_out[10+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[10+i*NUM_PE][8], PE_out[10+i*NUM_PE][9], PE_out[10+i*NUM_PE][10], PE_out[10+i*NUM_PE][11]);
		exe(OP_NOP,OP_NOP, PE_out[15+i*NUM_PE],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11+i*NUM_PE][4], PE_out[11+i*NUM_PE][0], PE_out[11+i*NUM_PE][2], PE_out[11+i*NUM_PE][7], /*BUFF 8to1 in*/ PE_out[11+i*NUM_PE][8], PE_out[11+i*NUM_PE][9], PE_out[11+i*NUM_PE][10], PE_out[11+i*NUM_PE][11]);
	
	}

}
	#if defined(ARMSIL)
	for(k = 0; k < (LOOP*4)- 3 - 3; k++){
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",k);
		printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		for (i = 0; i < NUM_PE; i++){
			printf ("PE_out[%d] =",i+k*NUM_PE);
			for (j = 0; j < 12; j++){
				printf (" %08x",PE_out[i+k*NUM_PE][j]);
			}
			printf ("\n");
			if( ((i+1) % 4) == 0){
				printf ("-----------------------------------------------------------------------------------------------------------------------\n");
			}
		}
	}
	printf("\n Hash output =",k);
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[2][0][i]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[2][1][i]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[2][2][i]);
	}
	for(i = 0; i < 2; i++){
		printf(" %08x",LMM[2][3][i]);
	}
	printf("\n");
	/* for(k = 0; k < LOOP; k++){
		// printf ("\n-----------------------------------------------------------------------------------------------------------------------\n");
		// printf ("|   LOOP %d \n",k-2);
		// printf ("-----------------------------------------------------------------------------------------------------------------------\n");
		printf ("\nW %d =",k);
		printf (" %08x\n",PE_out[10+k*NUM_PE][0]);
		for (i = 0; i < 8; i++){
				printf (" %08x",PE_out[14+k*NUM_PE][i+4]);
			}
		for (i = 0; i < 8; i++){
				printf (" %08x",PE_out[15+k*NUM_PE][i+4]);
			} 
		} */
	#endif
	
	
	return 0;
}