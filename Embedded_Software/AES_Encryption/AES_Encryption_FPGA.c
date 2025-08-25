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
	U32 OP;
	Init();
	//LOOP = 12;
	LOOP = 12;
	// }
	// uint32_t KEY[4] = {0x54686174, 0x73206D79, 0x204B756E, 0x67204675};
	//uint32_t Plain_Text[4] = {0x54776F20, 0x4F6E6520, 0x4E696E65, 0x2054776F};


	uint32_t KEY[] 			= { 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c };
	uint32_t Plain_Text[] 	= { 0x6bc1bee2, 0x2e409f96, 0xe93d7e11, 0x7393172a };

	uint32_t RCON[11] = {0x8d000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};
	
	LMM[0][0][0] = Plain_Text[0];
	LMM[0][1][0] = Plain_Text[1];

	LMM[2][0][0] = Plain_Text[0];
	LMM[2][1][0] = Plain_Text[1];
	
	LMM[1][0][0] = KEY[0];
	LMM[1][1][0] = KEY[1];

	LMM[3][0][0] = KEY[0];
	LMM[3][1][0] = KEY[1];
	
	LMM[4][0][0] = Plain_Text[2];
	LMM[4][1][0] = Plain_Text[3];

	LMM[6][0][0] = Plain_Text[2];
	LMM[6][1][0] = Plain_Text[3];
	
	LMM[5][0][0] = KEY[2];
	LMM[5][1][0] = KEY[3];

	LMM[7][0][0] = KEY[2];
	LMM[7][1][0] = KEY[3];

	
#if defined(ARMZYNQ)
	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	
	U32 *A_MSB, *A_LSB;
	A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
	A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if(j==0)
				A_MSB[i*8 +j] = LMM[j/2][j%2][0]+i;
			else
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}

	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if(j==0)
				A_LSB[i*8 +j] = LMM[j/2][j%2][0]+i+8;
			else
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	
	U32 *B_MSB, *B_LSB;
	B_MSB = (U32*)(membase + ROW1_MSB_BASE_PHYS);
	B_LSB = (U32*)(membase + ROW1_LSB_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			B_MSB[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}

	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			B_LSB[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}	
	
	#elif defined(VIVADOSIL)
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
		
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",32);			
	#endif
	//ROM[8][0] = K[0];
	
	for (i = 0; i < LOOP; i++){
		if( i == 0){

			///*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 1 ***///
			mop32(OP_LDW, 0, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0],PE_out[0][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 2 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8 ],/*ALU in*/ PE_out[4][4],PE_out[4][5],PE_out[5][4],PE_out[5][5], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5], PE_out[4][0],PE_out[4][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
	
			exe32(OP_NOP, 0, PE_out[9 ],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5], PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][4],PE_out[6][5],PE_out[7][4],PE_out[7][5], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5], PE_out[6][0],PE_out[6][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			OP = CUSTOM_OP(OP_GW3);
			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5], PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 3 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][6],PE_out[8][7],PE_out[9][6],PE_out[9][7], /*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][1], PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			/*--*/OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][7],0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5], PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][6],PE_out[10][7],PE_out[11][6],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][2],PE_out[10][1], PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			/*--*/ OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[9][7],0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5], PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i < 10){
			// /*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/8,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			
			///*** row 0 ***///
			/*--*/OP = CUSTOM_OP(OP_SUB_SHF);
			exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][4],PE_out[12][5],PE_out[12][2], PE_out[12][1], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][2], PE_out[12][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, RCON[i], PE_out[1],/*ALU in*/ PE_out[13][4],0,0,PE_out[13][0], /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			/*--*/OP = CUSTOM_OP(OP_SUB_SHF);
			exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[14][4],PE_out[14][5],PE_out[14][2],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, RCON[i], PE_out[3],/*ALU in*/ PE_out[15][4],0,0,PE_out[15][0], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 1 ***///
			/*--*/OP = CUSTOM_OP(OP_MIXCOL);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[0][1],PE_out[0][2],PE_out[0][3], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5], PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][1],0,PE_out[1][5],PE_out[1][6], /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5], PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = CUSTOM_OP(OP_MIXCOL);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][0],PE_out[2][1],PE_out[2][2],PE_out[2][3], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5], PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][1],0,PE_out[3][5],PE_out[3][6], /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][5], PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 2 ***///
			
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8 ],/*ALU in*/ PE_out[4][0],PE_out[4][1],PE_out[5][4],PE_out[5][2], /*BUFF 16to1 in*/ PE_out[4][0],PE_out[4][1], PE_out[4][2],PE_out[4][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP	, 0, PE_out[9],/*ALU in*/ PE_out[5][1],0,PE_out[5][7],0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][2], PE_out[5][1],PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],PE_out[6][1],PE_out[7][4],PE_out[7][2], /*BUFF 16to1 in*/ PE_out[6][0],PE_out[6][1], PE_out[6][2],PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP	, 0, PE_out[11],/*ALU in*/ PE_out[7][1],0,PE_out[7][7],0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][2], PE_out[7][1],PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 3 ***///
			
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][6],PE_out[8][7],PE_out[9][6],PE_out[9][2], /*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][1], PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			/*--*/OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][2],0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5], PE_out[9][6],PE_out[9][2], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][6],PE_out[10][7],PE_out[11][6],PE_out[11][2], /*BUFF 16to1 in*/ PE_out[10][2],PE_out[10][1], PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][2],0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5], PE_out[11][6],PE_out[11][2], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if (i==10){
			// /*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/8,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			
			///*** row 0 ***///
			/*--*/OP = CUSTOM_OP(OP_SUB_SHF);
			exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][4],PE_out[12][5],PE_out[12][2], PE_out[12][1], /*BUFF 16to1 in*/ PE_out[12][4], PE_out[12][5], PE_out[12][2], PE_out[12][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, RCON[i], PE_out[1],/*ALU in*/ PE_out[13][4],0,0,PE_out[13][0], /*BUFF 16to1 in*/ PE_out[13][4], PE_out[13][5], PE_out[13][6], PE_out[13][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			/*--*/OP = CUSTOM_OP(OP_SUB_SHF);
			exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[14][4],PE_out[14][5],PE_out[14][2],PE_out[14][1], /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, RCON[i], PE_out[3],/*ALU in*/ PE_out[15][4],0,0,PE_out[15][0], /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 1 ***///

			exe32(OP_NOP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[0][3],PE_out[0][0],PE_out[0][1], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5], PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][1],0,PE_out[1][5],PE_out[1][6], /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5], PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			

			exe32(OP_NOP, 0, PE_out[6],/*ALU in*/ PE_out[2][2],PE_out[2][3],PE_out[2][0],PE_out[2][1], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5], PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][1],0,PE_out[3][5],PE_out[3][6], /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][5], PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 2 ***///
			
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8 ],/*ALU in*/ PE_out[4][0],PE_out[4][1],PE_out[5][4],PE_out[5][2], /*BUFF 16to1 in*/ PE_out[4][0],PE_out[4][1], PE_out[4][2],PE_out[4][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP	, 0, PE_out[9],/*ALU in*/ PE_out[5][1],0,PE_out[5][7],0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][2], PE_out[5][1],PE_out[5][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],PE_out[6][1],PE_out[7][4],PE_out[7][2], /*BUFF 16to1 in*/ PE_out[6][0],PE_out[6][1], PE_out[6][2],PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP	, 0, PE_out[11],/*ALU in*/ PE_out[7][1],0,PE_out[7][7],0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][2], PE_out[7][1],PE_out[7][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 3 ***///
			
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][6],PE_out[8][7],PE_out[9][6],PE_out[9][2], /*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][1], PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			/*--*/OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[9][2],0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5], PE_out[9][6],PE_out[9][2], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][6],PE_out[10][7],PE_out[11][6],PE_out[11][2], /*BUFF 16to1 in*/ PE_out[10][2],PE_out[10][1], PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			/*--*/OP = CUSTOM_OP(OP_GW3);
			exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][2],0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5], PE_out[11][6],PE_out[11][2], /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else {
			///*** row 0 ***///

			mop32(OP_STW, 0, PE_out[0],/*ALU in*/ PE_out[12][4],PE_out[12][5],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[1],/*ALU in*/ PE_out[12][2],PE_out[12][1],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[2],/*ALU in*/ PE_out[14][4],PE_out[14][5],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[3],/*ALU in*/ PE_out[14][2],PE_out[14][1],0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			///*** row 1 ***///
			exe32(OP_NOP, 0, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 2 ***///
			exe32(OP_NOP, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

			///*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

		}
	
	}
	
	#if defined(ARMZYNQ)
	
	*(CGRA_info.ctx_pe_mmap+MODE_BASE_IP) = 0;
	
	dma_write_U32(ROW0_MSB_BASE_PHYS,8);
	dma_write_U32(ROW1_MSB_BASE_PHYS,8);
	dma_write_U32(ROW0_LSB_BASE_PHYS,8);
	dma_write_U32(ROW1_LSB_BASE_PHYS,8);
	
	*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    while(1){
		if(*(CGRA_info.ctx_pe_mmap+FINISH_BASE_IP))
			break;
	}
	//sleep(1);
	//printf("Done!\n");
	
	dma_read_U32(ROW0_MSB_BASE_PHYS,8);
	dma_read_U32(ROW0_LSB_BASE_PHYS,8);
		for (i=0; i<8; i++) {
		printf("OutputMSB [%d] = ",i);
		for (j=0; j<8; j++) {
			printf("%08x ",A_MSB[i*8+j]);
		}
		printf("\n");
		}
		printf("\n");
		for (i=0; i<8; i++) {
		printf("Output LSB [%d] = ",i);
		for (j=0; j<8; j++) {
			printf("%08x ",A_LSB[i*8+j]);
		}
		printf("\n");
		}
	#endif
	
	#if defined(ARMSIL)
		printf("\n Hash output 0 =");
		for(i = 0; i < 2; i++){
			printf(" %08x", LMM[0][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[1][i][0]);
		}
		printf("\n Hash output 1 =");
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