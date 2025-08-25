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
	
	LOOP = 83;
	U64 OP;
	
	U64 OP_IM;

	U64 H[8] = {0x6a09e667f3bcc908,0xbb67ae8584caa73b,0x3c6ef372fe94f82b,0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179};
	
	U64 W[16] = {0x6162638000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x18};
	//uint32_t W[16] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf, 0x10};
	//uint32_t W[16] = {0x71776572,0x74797569,0x6f706173,0x64666768,0x6a6b6c7a,0x78637662,0x6e6d3132,0x33343536,0x37383971,0x77657274,0x79637662,0x6e800000,0x00000000,0x00000000,0x00000000,0x00000168};
	static const U64 K[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
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
	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	
	U64 *A_MSB,  *A_LSB;
	A_MSB = (U64*)(membase + ROW0_MSB_BASE_PHYS);
	A_LSB = (U64*)(membase + ROW0_LSB_BASE_PHYS);
	
	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			if(j==0)
				A_MSB[i*4 +j] = LMM[j/2][j%2][0];
			else
				A_MSB[i*4 +j] = LMM[j/2][j%2][0];
		}
	}

	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			if(j==0)
				A_LSB[i*4 +j] = LMM[j/2+2][j%2][0];
			else
				A_LSB[i*4 +j] = LMM[j/2+2][j%2][0];
		}
	}
	
	U64 *B_MSB, *B_LSB;
	B_MSB = (U64*)(membase + ROW1_MSB_BASE_PHYS);
	B_LSB = (U64*)(membase + ROW1_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			B_MSB[i*4 +j] = LMM[j/2+4][j%2][0];
		}
	}

	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			B_LSB[i*4 +j] = LMM[j/2+6][j%2][0];
		}
	}
	
	U64 *C_MSB, *C_LSB;
	C_MSB = (U64*)(membase + ROW2_MSB_BASE_PHYS);
	C_LSB = (U64*)(membase + ROW2_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			C_MSB[i*4 +j] = LMM[j/2+8][j%2][0];
		}
	}

	for (i=0; i<8; i++) {
		for (j=0; j<4; j++) {
			C_LSB[i*4 +j] = LMM[j/2+10][j%2][0];
		}
	}
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
			mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 1 ***///
			mop64(OP_LDW, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[5], /*ALU in*/ 0,0,PE_out[2][0], PE_out[2][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[6], /*ALU in*/ 0,0,PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 2 ***///
			mop64(OP_LDW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5],PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2], PE_out[5][3], PE_out[6][2], PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[10], /*ALU in*/ 0,0,PE_out[4][0], PE_out[4][1], /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[11], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			///*** row 3 ***///

			exe64(OP_NOP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,0,PE_out[8][1], /*BUFF 16to1 in*/ 0,0,0,PE_out[8][4], /*BUFF 8to1 in*/ PE_out[8][5], PE_out[8][6], PE_out[8][7], 0);
			exe64(OP_NOP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][4], /*BUFF 8to1 in*/ PE_out[9][5], PE_out[9][6], PE_out[9][7], 0);
			exe64(OP_NOP, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][2], PE_out[10][3], PE_out[11][2], PE_out[11][3], /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			exe64(OP_NOP, 0, PE_out[15], /*ALU in*/ PE_out[9][0], PE_out[9][1],PE_out[10][0],PE_out[10][1], /*BUFF 16to1 in*/ 0, 0, PE_out[11][0], PE_out[11][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);		
			}
		else if(i == 81){
			// /*** row 0 ***///
			mop64(OP_LDW, 0, PE_out[0], /*ALU in*/ 0,0,PE_out[12][2],PE_out[12][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[1], /*ALU in*/ 0,0,PE_out[12][9],PE_out[12][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,PE_out[13][7],PE_out[13][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,PE_out[13][9],PE_out[13][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[0][2],PE_out[0][1],PE_out[0][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][0],PE_out[1][2],PE_out[1][1],PE_out[1][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][0],PE_out[2][2],PE_out[2][1],PE_out[2][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][0],PE_out[3][2],PE_out[3][1],PE_out[3][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe64(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],PE_out[4][1], 0, PE_out[4][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][0],PE_out[5][1], 0, PE_out[5][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],PE_out[6][1], 0, PE_out[6][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0],PE_out[7][1], 0, PE_out[7][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe64(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, PE_out[8][1], PE_out[8][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, PE_out[9][1], PE_out[9][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0,PE_out[10][1],PE_out[10][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0,PE_out[11][1],PE_out[11][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i == 82){
			// /*** row 0 ***///
			mop64(OP_STW, 0, PE_out[0], /*ALU in*/ PE_out[12][0],PE_out[12][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_STW, 0, PE_out[1], /*ALU in*/ PE_out[13][0],PE_out[13][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_STW, 0, PE_out[2], /*ALU in*/ PE_out[14][0],PE_out[14][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop64(OP_STW, 0, PE_out[3], /*ALU in*/ PE_out[15][0],PE_out[15][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			exe64(OP_NOP, 0, PE_out[4],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[5],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[6],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[7],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe64(OP_NOP, 0, PE_out[8],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[9],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[10],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[11],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe64(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe64(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else{
			///*** row 0 ***///
			if( i == 1) {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][4],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe64(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe64(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][7],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][11], PE_out[14][10], PE_out[14][9], PE_out[14][8], /*BUFF 8to1 in*/ PE_out[14][7], PE_out[14][6], PE_out[14][5], PE_out[14][4]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe64(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][5],PE_out[15][6], PE_out[15][0], PE_out[15][1], /*BUFF 16to1 in*/ PE_out[12][2], PE_out[12][1], PE_out[15][2], PE_out[15][3], /*BUFF 8to1 in*/ 0, 0, PE_out[15][6], PE_out[15][7]);

				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe64(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][5],PE_out[2][11], 0, 0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe64(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][4], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][11], PE_out[3][10], PE_out[3][3], PE_out[3][2], /*BUFF 8to1 in*/ PE_out[3][7], PE_out[3][6], PE_out[3][5], PE_out[3][4]);
			}
			else {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][11],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][2], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe64(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe64(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][2],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe64(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][10],PE_out[15][5], 0, 0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);
			
				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe64(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe64(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][10],PE_out[2][11], 0, PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe64(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][11], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

			}
			/*** row 2 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][2],PE_out[4][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4][7], PE_out[4][8], PE_out[4][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, 0, PE_out[9],/*ALU in*/ PE_out[4][2],PE_out[4][10], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5][7], PE_out[5][8], PE_out[5][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			exe64(OP, 0, PE_out[10],/*ALU in*/ PE_out[5][1],PE_out[4][8],0,PE_out[4][9], /*BUFF 16to1 in*/ PE_out[7][0],PE_out[6][4],PE_out[6][5],PE_out[6][6], /*BUFF 8to1 in*/ PE_out[6][7], PE_out[6][8], PE_out[6][9], PE_out[6][10]);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][2],PE_out[7][1],PE_out[6][2],0, /*BUFF 16to1 in*/ 0,PE_out[7][4],PE_out[7][5],PE_out[7][6], /*BUFF 8to1 in*/ PE_out[7][7], PE_out[7][8], PE_out[7][9], PE_out[7][10]);
			/*** row 3 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][2],PE_out[10][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], 0);
			exe64(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][2], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], 0);
			exe64(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
			exe64(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[11][2],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);		
			}
		
	}
	#if defined(ARMZYNQ)
	*(CGRA_info.ctx_pe_mmap+MODE_BASE_IP) = 1;
	dma_write_U64(ROW0_MSB_BASE_PHYS,8);
	dma_write_U64(ROW0_LSB_BASE_PHYS,8);
	dma_write_U64(ROW1_MSB_BASE_PHYS,8);
	dma_write_U64(ROW1_LSB_BASE_PHYS,8);	
	dma_write_U64(ROW2_MSB_BASE_PHYS,8);
	dma_write_U64(ROW2_LSB_BASE_PHYS,8);	
	
	*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    while(1){
		if(*(CGRA_info.ctx_pe_mmap+(FINISH_BASE_IP)))
			break;
	}

	dma_read_U64(ROW0_MSB_BASE_PHYS,8);
	dma_read_U64(ROW0_LSB_BASE_PHYS,8);

		
	printf("\n");
	for (i=0; i<8; i++) {
		printf("Output [%d] = ",i);
		for (j=0; j<8; j++) {
			if(j<4)
				printf("%016lx ",A_MSB[i*4+j]);
			else
				printf("%016lx ",A_LSB[i*4+j-4]);
		}
		printf("\n");
	}
	
	#elif defined(ARMSIL)
	printf("\n Hash output =");
	for(i = 0; i < 2; i++){
		printf(" %016lx",LMM[0][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %016lx",LMM[1][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %016lx",LMM[2][i][0]);
	}
	for(i = 0; i < 2; i++){
		printf(" %016lx",LMM[3][i][0]);
	}
	printf("\n");
	#endif
	Final();
	
	return 0;
}
