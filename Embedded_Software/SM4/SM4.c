#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"
// #include "../FPGA_Driver.c"

#define N 		 4

int main(){

	 int k,i,j, LOOP;
	Init();
	// LOOP = 3;
	LOOP = 35;
	U32 OP;
	
	#if defined(ARMZYNQ)

	#endif
	#if defined(ARMSIL)

	#endif
	// }
	uint32_t KEY[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
	static const unsigned int FK[4] = {0xa3b1bac6,0x56aa3350,0x677d9197,0xb27022dc};
	// uint32_t Plain_Text[4] = {0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210};
	uint32_t Plain_Text[4] = {(0x8d10f00f+7), 0x603ac9bf, 0xd840c3fa, 0xc36d752e};
	uint32_t Plain_Text2[4] = {0x657c31f0, 0xaea4dfea, 0x3e888371, 0xc34e5c89};

	// 657c31f0aea4dfea3e888371c34e5c89
	// 8d10f00f603ac9bfd840c3fac36d752e	
	// 0123456789abcdeffedcba9876543210
	uint32_t RCON[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

	/* fixed parameter */
static const unsigned int CK[32] =
{
0x00070e15,0x1c232a31,0x383f464d,0x545b6269,
0x70777e85,0x8c939aa1,0xa8afb6bd,0xc4cbd2d9,
0xe0e7eef5,0xfc030a11,0x181f262d,0x343b4249,
0x50575e65,0x6c737a81,0x888f969d,0xa4abb2b9,
0xc0c7ced5,0xdce3eaf1,0xf8ff060d,0x141b2229,
0x30373e45,0x4c535a61,0x686f767d,0x848b9299,
0xa0a7aeb5,0xbcc3cad1,0xd8dfe6ed,0xf4fb0209,
0x10171e25,0x2c333a41,0x484f565d,0x646b7279
};
	
	LMM[0][0][0] = KEY[0] ^ FK[0];
	LMM[0][1][0] = KEY[1] ^ FK[1];

	LMM[2][0][0] = KEY[0] ^ FK[0];
	LMM[2][1][0] = KEY[1] ^ FK[1];
	
	LMM[1][0][0] = KEY[2] ^ FK[2];
	LMM[1][1][0] = KEY[3] ^ FK[3];

	LMM[3][0][0] = KEY[2] ^ FK[2];
	LMM[3][1][0] = KEY[3] ^ FK[3];
	
	LMM[4][0][0] = Plain_Text[0];
	LMM[4][1][0] = Plain_Text[1];

	LMM[6][0][0] = Plain_Text2[0];
	LMM[6][1][0] = Plain_Text2[1];
	
	LMM[5][0][0] = Plain_Text[2];
	LMM[5][1][0] = Plain_Text[3];

	LMM[7][0][0] = Plain_Text2[2];
	LMM[7][1][0] = Plain_Text2[3];

	//ROM[8][0] = K[0];
	#if defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;		
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",32);			
	#endif
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			///*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);


			///*** row 1 ***///
			mop32(OP_LDW, 0, PE_out[4], /*ALU in*/ 0,0, PE_out[1][0], PE_out[1][1], /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[5], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			
			
			mop32(OP_LDW, 0, PE_out[6], /*ALU in*/ 0,0, PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[2][0], PE_out[2][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);	

			///*** row 2 ***///
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][2], PE_out[4][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[4][0], PE_out[4][1],0,0, /*BUFF 16to1 in*/ 0,0,PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][2],PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ PE_out[6][0],PE_out[6][1],0,0, /*BUFF 16to1 in*/ 0, 0, PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][2],PE_out[9][3],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][2], PE_out[11][3], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i == 1){
			// /*** row 0 ***///
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[0],/*ALU in*/ PE_out[12][5],0,PE_out[12][6],PE_out[12][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[2],/*ALU in*/ PE_out[14][5],0,PE_out[14][6],PE_out[14][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6], PE_out[15][7],/*BUFF 8to1 in*/ 0, 0, 0, 0);			// /*** row 1 ***///
			// /*** row 1 ***///
			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][1],0,0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[5],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][1],0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			// /*** row 2 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],0,PE_out[4][0],PE_out[4][0], /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[9],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],0,PE_out[6][0],PE_out[6][0], /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			// /*** row 3 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,PE_out[8][1],PE_out[8][4], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][0],0,PE_out[10][1],PE_out[10][4], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
			
		else if (i==2){
			// /*** row 0 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[0],/*ALU in*/ PE_out[12][6],0,PE_out[12][7],PE_out[12][1], /*BUFF 16to1 in*/ PE_out[12][5],PE_out[12][6],PE_out[12][7],PE_out[12][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][5],PE_out[13][6],PE_out[13][7],PE_out[12][1], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[2],/*ALU in*/ PE_out[14][6],0,PE_out[14][7],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[14][5],PE_out[14][6],PE_out[14][7],PE_out[14][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			// printf("Debug here!!!\n");
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[15][5],PE_out[15][6],PE_out[15][7],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6], PE_out[15][7],/*BUFF 8to1 in*/ 0, 0, 0, 0);			// /*** row 1 ***///


			// /*** row 1 ***///
			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][1],0,0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][1], 0, 0, 0, /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][1],0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][1], 0, 0, 0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			// /*** row 2 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],0,PE_out[4][0],PE_out[4][0], /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			OP = CUSTOM_OP(OP_4_RX_SM4);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][0], PE_out[5][0], 0, 0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],0,PE_out[6][0],PE_out[6][0], /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_4_RX_SM4);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0], PE_out[7][0], 0, 0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			// /*** row 3 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,PE_out[8][1],PE_out[8][4], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][4], 0, PE_out[9][0], PE_out[9][1], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][0],0,PE_out[10][1],PE_out[10][4], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[15],/*ALU in*/  PE_out[11][4], 0, PE_out[11][0], PE_out[11][1], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

		}
		else if(i<=33)
		{
						// /*** row 0 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[0],/*ALU in*/ PE_out[12][6],0,PE_out[12][7],PE_out[12][1], /*BUFF 16to1 in*/ PE_out[12][5],PE_out[12][6],PE_out[12][7],PE_out[12][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][6],PE_out[13][7],PE_out[13][1],PE_out[12][1], /*BUFF 16to1 in*/ PE_out[13][5],PE_out[13][6],PE_out[13][7],PE_out[13][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, CK[i-1], PE_out[2],/*ALU in*/ PE_out[14][6],0,PE_out[14][7],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[14][5],PE_out[14][6],PE_out[14][7],PE_out[14][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[15][6],PE_out[15][7],PE_out[15][1],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[15][5],PE_out[15][6],PE_out[15][7], PE_out[15][1],/*BUFF 8to1 in*/ 0, 0, 0, 0);			// /*** row 1 ***///


			// /*** row 1 ***///
			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][1],0,0,0, /*BUFF 16to1 in*/ PE_out[0][4], PE_out[0][5], PE_out[0][6], PE_out[0][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][1], 0, 0, 0, /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][1],0,0,0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_SBOX_SM4);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][1], 0, 0, 0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			// /*** row 2 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],0,PE_out[4][0],PE_out[4][0], /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5], PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			OP = CUSTOM_OP(OP_4_RX_SM4);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][0], PE_out[5][0], 0, 0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP, /*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_ROL,/*SRU2_IM*/23,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],0,PE_out[6][0],PE_out[6][0], /*BUFF 16to1 in*/ PE_out[6][4], PE_out[6][5], PE_out[6][6], PE_out[6][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_4_RX_SM4);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0], PE_out[7][0], 0, 0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			// /*** row 3 ***///

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,PE_out[8][1],PE_out[8][4], /*BUFF 16to1 in*/ PE_out[8][4], PE_out[8][5], PE_out[8][6], PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][4], 0, PE_out[9][0], PE_out[9][1], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP, /*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][0],0,PE_out[10][1],PE_out[10][4], /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP, /*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP, /*SRU2*/OP_NOP, /*SRU2_IM*/OP_NOP, /*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[15],/*ALU in*/  PE_out[11][4], 0, PE_out[11][0], PE_out[11][1], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

		}
		else{
			mop32(OP_STW, 0, PE_out[0], /*ALU in*/ PE_out[13][1],PE_out[13][7], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[1], /*ALU in*/ PE_out[13][6],PE_out[13][5], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[2], /*ALU in*/ PE_out[15][1],PE_out[15][7], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[3], /*ALU in*/ PE_out[15][6],PE_out[15][5], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		
	}
	
	#if defined(ARMSIL)
		printf("\n Cirplertext =");
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[0][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[1][i][0]);
		}
		printf("\tOutput 2: ");
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[2][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[3][i][0]);
		}
		printf("\n");
	#endif
	Final();
	
	return 0;
}