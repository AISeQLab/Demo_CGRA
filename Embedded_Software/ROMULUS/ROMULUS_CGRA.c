#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

unsigned char* membase;
U32 *A_MSB, *A_LSB, *B_MSB, *B_LSB;	
/////////////////////////////////////ROMULUS-H USE SKINNY-128-384

	/*
	NOTE: 
		_ THERE ARE 4 TABLE: STATE - TWEAKEY0 - TWEAKEY1 - TWEAKEY2
		_ EACH TABLE HAS 16 8BIT-ELEMENTS. THUS, EACH OUTPUT STORES 4 ELEMENTS -> 1 TABLE IS STORED IN 4 OUTPUTS
		_ REFERENT FROM TABLE TO OUTPUT:
	****************************************************
		S0		S1		S2		S3		->  OUTPUT_0
		S4		S5		S6		S7		-> 	OUTPUT_1
		S8		S9		S10		S11		->	OUTPUT_2
		S12		S13		S14		S15		-> 	OUTPUT_3
	****************************************************

		
		MATRIX SAMPLE:
		PE[0] = X	X	X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]		
		PE[1] = X 	X	X	X		X		X		X		X			T0[0]		T0[1]		T0[2] 		T0[3]		
		PE[2] = X	X	X	X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]		
		PE[3] = X	X	X	X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]			

		 		
	*/

const unsigned char RC[40] = {
		0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F,
		0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E, 0x1D, 0x3A, 0x35, 0x2B,
		0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E,
		0x1C, 0x38, 0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A};

void tweakey2_add(){
	U64 OP;
/*		PE[0] = X	X	X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]		
		PE[1] = X 	X	X	X		X		X		X		X			T0[0]		T0[1]		T0[2] 		T0[3]		
		PE[2] = X	X	X	X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]		
		PE[3] = X	X	X	X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]		
		*/
	//-----------------------LOOP 1 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x20, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x80, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0x7f, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,/*SRU1_IM*/ 7,/*SRU2*/OP_SHR,/*SRU2_IM*/ 5,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[2][8],PE_out[0][1],PE_out[1][1],PE_out[2][8], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[2][9],PE_out[0][1],PE_out[1][1],PE_out[2][9], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[2][8],PE_out[2][1],PE_out[3][1],PE_out[2][8], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[2][9],PE_out[2][1],PE_out[3][1],PE_out[2][9], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[4][1],PE_out[7][7],PE_out[7][7],PE_out[6][1], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],PE_out[7][7],PE_out[7][7],PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

	//-----------------------LOOP 2 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x20, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x80, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][1],PE_out[15][1],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0x7f, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7],/*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,/*SRU1_IM*/ 7,/*SRU2*/OP_SHR,/*SRU2_IM*/ 5,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[2][4],PE_out[0][1],PE_out[1][1],PE_out[2][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[2][5],PE_out[0][1],PE_out[1][1],PE_out[2][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[2][4],PE_out[2][1],PE_out[3][1],PE_out[2][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[2][5],PE_out[2][1],PE_out[3][1],PE_out[2][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[4][1],0,0,PE_out[6][1], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],0,0,PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

	//-----------------------LOOP 3 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x20, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x80, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][1],PE_out[15][1],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0x7f, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7],/*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,/*SRU1_IM*/ 7,/*SRU2*/OP_SHR,/*SRU2_IM*/ 5,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[2][4],PE_out[0][1],PE_out[1][1],PE_out[2][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[2][5],PE_out[0][1],PE_out[1][1],PE_out[2][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[2][4],PE_out[2][1],PE_out[3][1],PE_out[2][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[2][5],PE_out[2][1],PE_out[3][1],PE_out[2][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[4][1],0,0,PE_out[6][1], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],0,0,PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
	//-----------------------LOOP 4 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x20, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x80, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][1],PE_out[15][1],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0x7f, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7],/*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,/*SRU1_IM*/ 7,/*SRU2*/OP_SHR,/*SRU2_IM*/ 5,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[2][4],PE_out[0][1],PE_out[1][1],PE_out[2][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[2][5],PE_out[0][1],PE_out[1][1],PE_out[2][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[2][4],PE_out[2][1],PE_out[3][1],PE_out[2][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[2][5],PE_out[2][1],PE_out[3][1],PE_out[2][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[4][1],0,0,PE_out[6][1], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],0,0,PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[10][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 


	//-----------------------LOOP 5 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][1],PE_out[15][1],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/  PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	exe32(OP, 0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
	
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);

}


void tweakey3_add(){
	U64 OP;
	//-----------------------LOOP 1 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x40, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x1, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0xff, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 7,/*SRU2*/OP_SHL,/*SRU2_IM*/ 1,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][8],PE_out[0][1],PE_out[1][1],PE_out[3][8], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][9],PE_out[0][1],PE_out[1][1],PE_out[3][9], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,  /*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[3][8],PE_out[2][1],PE_out[3][1],PE_out[3][8], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[3][9],PE_out[2][1],PE_out[3][1],PE_out[3][9], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][1],PE_out[7][7],PE_out[7][7],PE_out[6][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],PE_out[7][7],PE_out[7][7],PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

	//-----------------------LOOP 2 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x40, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x1, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0xff, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[15][1],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 7,/*SRU2*/OP_SHL,/*SRU2_IM*/ 1,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][4],PE_out[0][1],PE_out[1][1],PE_out[3][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][5],PE_out[0][1],PE_out[1][1],PE_out[3][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,  /*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[3][4],PE_out[2][1],PE_out[3][1],PE_out[3][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[3][5],PE_out[2][1],PE_out[3][1],PE_out[3][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][1],PE_out[7][7],PE_out[7][7],PE_out[6][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],PE_out[7][7],PE_out[7][7],PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

	//-----------------------LOOP 3 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x40, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x1, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0xff, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[15][1],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 7,/*SRU2*/OP_SHL,/*SRU2_IM*/ 1,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][4],PE_out[0][1],PE_out[1][1],PE_out[3][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][5],PE_out[0][1],PE_out[1][1],PE_out[3][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,  /*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[3][4],PE_out[2][1],PE_out[3][1],PE_out[3][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[3][5],PE_out[2][1],PE_out[3][1],PE_out[3][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][1],PE_out[7][7],PE_out[7][7],PE_out[6][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],PE_out[7][7],PE_out[7][7],PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);

	//-----------------------LOOP 4 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0x40, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x1, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0xffffff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0xff, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[15][1],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 7,/*SRU2*/OP_SHL,/*SRU2_IM*/ 1,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][4],PE_out[0][1],PE_out[1][1],PE_out[3][4], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][5],PE_out[0][1],PE_out[1][1],PE_out[3][5], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHR,  /*SRU1_IM*/ 1,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[3][4],PE_out[2][1],PE_out[3][1],PE_out[3][4], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[3][5],PE_out[2][1],PE_out[3][1],PE_out[3][5], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_OR);
	exe32(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][1],PE_out[7][7],PE_out[7][7],PE_out[6][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[5][1],PE_out[7][7],PE_out[7][7],PE_out[7][1],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ PE_out[11][1],0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 


	//-----------------------LOOP 5 -------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/  PE_out[12][1],PE_out[15][1],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	exe32(OP, 0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][10],PE_out[3][11]);
	
	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);
}


void Permutation(){
	/*		
		PE[0] = X	X	X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]		
		PE[1] = X 	X	X	X		X		X		X		X			T0[0]		T0[1]		T0[2] 		T0[3]		
		PE[2] = X	X	X	X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]		
		PE[3] = X	X	X	X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]		
		*/
	U64 OP;
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0xff000000, PE_out[0],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0x00ff0000, PE_out[1],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0x0000ff00, PE_out[2],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0x000000ff, PE_out[3],/*ALU in*/ 0,0,0,PE_out[15][7], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	//Solve T0
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[1][10],PE_out[0][1],PE_out[1][1],PE_out[1][10], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[1][11],PE_out[1][1],PE_out[3][1],PE_out[1][11], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[1][10],PE_out[3][1],PE_out[2][1],PE_out[1][10], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[1][11],PE_out[0][1],PE_out[2][1],PE_out[1][11], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[8],/*ALU in*/  PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  PE_out[4][1],PE_out[4][2],PE_out[5][1],PE_out[5][2],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][1],PE_out[6][2],PE_out[7][1],PE_out[7][2], /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		T0[0]	X		X		X		X		X			T0[2]		T0[3]		T02			T03
	PE[14] = x	X		T0[1]	X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]
	PE[15] = x	X		X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/ 

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0xff000000, PE_out[12],/*ALU in*/ 0,0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0x00ff0000, PE_out[13],/*ALU in*/ 0,0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[9][1],PE_out[10][1],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][8],PE_out[9][9]); 
 	exe32(OP, 0x0000ff00, PE_out[14],/*ALU in*/ 0,0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0x000000ff, PE_out[15],/*ALU in*/ 0,0,0,PE_out[11][7], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		X		X		T0[0]	T0[1]	X		X			T0[2]		T0[3]		T0[2]		T0[3]
	PE[14] = x	X		X		X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]
	PE[15] = x	X		X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/ 

	// Solve T1
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[0],/*ALU in*/  PE_out[14][10],PE_out[12][1],PE_out[13][1],PE_out[14][10], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[1],/*ALU in*/  PE_out[14][11],PE_out[13][1],PE_out[15][1],PE_out[14][11], /*BUFF 16to1 in*/	PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][10],PE_out[13][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[2],/*ALU in*/  PE_out[14][10],PE_out[15][1],PE_out[14][1],PE_out[14][10], /*BUFF 16to1 in*/  PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[3],/*ALU in*/  PE_out[14][11],PE_out[12][1],PE_out[14][1],PE_out[14][11], /*BUFF 16to1 in*/  PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		X		X		X		X		X		X			T0[0]		T0[1]		T0[2]		T0[3]
	PE[14] = x	X		X		X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]
	PE[15] = x	X		X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/ 
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][7],PE_out[3][7],PE_out[3][7],PE_out[3][7], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][7],PE_out[3][7],PE_out[3][7],PE_out[3][7], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  PE_out[0][1],PE_out[0][2],PE_out[1][1],PE_out[1][2], /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  PE_out[2][1],PE_out[2][2],PE_out[3][1],PE_out[3][2], /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		X		X		X		X		X		X			T0[0]		T0[1]		T0[2]		T0[3]
	PE[14] = x	T1[0]	X		X 		X		X		X		X			T1[2]		T1[3]		T12 		T13
	PE[15] = x	T1[1]	X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/ 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0xff000000, PE_out[8],/*ALU in*/  0,0,0,PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0x00ff0000, PE_out[9],/*ALU in*/  0,0,0,PE_out[7][7],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0x0000ff00, PE_out[10],/*ALU in*/ 0,0,0,PE_out[7][7], /*BUFF 16to1 in*/ 	PE_out[6][1],PE_out[7][1],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][8],PE_out[6][9]); 
	exe32(OP, 0x000000ff, PE_out[11],/*ALU in*/ 0,0,0,PE_out[7][7],  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		X		X		X		X		X		X			T0[0]		T0[1]		T0[2]		T0[3]
	PE[14] = x	x		X		X 		T1[0]	T1[1]	X		X			T1[2]		T1[3]		T1[2]		T1[3]
	PE[15] = x	x		X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/ 
	// Solve t2
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP,0, PE_out[12],/*ALU in*/ PE_out[11][10],PE_out[8][1],PE_out[9][1],PE_out[11][10], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP,0, PE_out[13],/*ALU in*/ PE_out[11][11],PE_out[9][1],PE_out[11][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 16,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP,0, PE_out[14],/*ALU in*/ PE_out[11][10],PE_out[11][1],PE_out[10][1],PE_out[11][10], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][10],PE_out[10][11]); 
 	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_SHL,/*SRU1_IM*/ 8,/*SRU2*/OP_SHR,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP,0, PE_out[15],/*ALU in*/ PE_out[11][11],PE_out[8][1],PE_out[10][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

		/*
	PE[12] = x	X		X		X		X		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = x	X		X		X		X		X		X		X			T0[0]		T0[1]		T0[2]		T0[3]
	PE[14] = x	x		X		X 		x		x		X		X			T1[0]		T1[1]		T1[2]		T1[3]
	PE[15] = x	x		X		X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[14][1],PE_out[14][2],PE_out[15][1],PE_out[15][2], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[12][1],PE_out[12][2],PE_out[13][1],PE_out[13][2], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][1],PE_out[0][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][8],PE_out[3][9]);

	exe32(OP, 0, PE_out[8],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, 0, PE_out[11],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][10],PE_out[7][11]); 

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][7],PE_out[8][7],PE_out[8][7],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][7],PE_out[9][7],PE_out[9][7],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][7],PE_out[10][7],PE_out[10][7],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][7],PE_out[11][7],PE_out[11][7],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
}

void apply_subtk(){
	U64 OP;
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[13][8],PE_out[14][8],PE_out[15][8], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[12][9],PE_out[13][9],PE_out[14][9],PE_out[15][9], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[0][10],PE_out[1][10],PE_out[2][10],PE_out[3][10], /*BUFF 16to1 in*/  	PE_out[0][1],PE_out[1][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[0][11],PE_out[1][11],PE_out[2][11],PE_out[3][11], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	exe32(OP, 0, PE_out[8],/*ALU in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][1],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); 		
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
}


void AddKey(){
	apply_subtk();
	Permutation();
	tweakey2_add();
	tweakey3_add();
}

void init_test(){
	LMM[0][0][0] = 0x80020100;		//A[0][0]: a66be1dc7f37fb80        0X123;     
	LMM[1][0][0] = 0;		//A[0][1]: 13931921bba211f6        0X456;
	LMM[2][0][0] = 0;		//A[0][2]: bff00febbae335e6        0X789;
	LMM[3][0][0] = 0;		//A[0][3]: 201f4a6fe5211611        0X10a;
	// LMM[2][0][0] = 0x2da96eb2;		//A[0][4]: 737be8ae2da96eb2        0X11b;
	// LMM[2][1][0] = 0x332f0211;		//A[1][0]: 800f0c5d332f0211        0X412;
	// LMM[3][0][0] = 0x08cf9556;		//A[1][1]: b8ca01f808cf9556        0X309;
	// LMM[3][1][0] = 0x0f814c25;		//A[1][2]: a09a3e970f814c25        0X221;
	// LMM[7][0][0] = 0xe3eef0e2;		//A[1][3]: e33e8248e3eef0e2        0X690;
	// state[0] =	0x7f37fb80;
	// state[1] =  0xbba211f6;
	// state[2] =  0xbae335e6;
	// state[3] =  0xe5211611;
	// state[4] =  0x2da96eb2;
	// state[5] =  0x332f0211;
	// state[6] =  0x08cf9556;
	// state[7] =  0x0f814c25;
	// state[8] = 0x2da96eb2;
	// state[9] = 0x332f0211;
	// state[10] = 0xb8ca01f8;
	// state[11] = 0xe33e8248;

	U32 OP;
	U64 x = 0;
	exe32(OP, x, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0xbba2118f,0x7f37fb8e,0xbae335e6,0xe5211611); 
	exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x2da96eb2,0x332f0211,0x08cf9556,0x0f814c25); 
	exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x6eb22da9,0x0211332f,0xb8ca01f8,0xe33e8248);
	exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x3c126a1e,0xba690c12,0x8816afea,0x648a36cb);	

	// exe32(OP, x, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); 
	// exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0x1000000,0,0); 
	// exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
	// exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);	

	exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][2],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][2],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][2],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	
	/*
	PE[12] = X		x		s1		x		s0		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s3		x		s2		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s5 		x		s4		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s7		x		s6		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][2],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);	
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, x, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][2],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, x, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][2],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);		
	/*
	PE[12] = X		x		s0rc		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2rc		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4rc 		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6rc		x		s7		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);
}


void AddConstants(int r){
	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_SHL,/*SRU2_IM*/ 24,/*LU3*/OP_XOR);
	exe32(OP, (RC[r] & 0xf), PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, ((RC[r]>>4) & 0x3), PE_out[1],/*ALU in*/ PE_out[12][9],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[12][1],PE_out[12][2],PE_out[13][1],PE_out[13][2], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);


	exe32(OP, 0x2, PE_out[4],/*ALU in*/  PE_out[0][10],0,0,PE_out[3][7], /*BUFF 16to1 in*/  	PE_out[0][1],PE_out[1][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][1],PE_out[0][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);


	exe32(OP, 0, PE_out[8],/*ALU in*/ 	PE_out[7][7],PE_out[7][7],PE_out[7][7],PE_out[7][7], /*BUFF 16to1 in*/ PE_out[4][1],PE_out[4][5],PE_out[4][1],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][10],PE_out[4][11]); 		
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][4],PE_out[8][11]); 		
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][7],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
}

void ShiftRows(){
	printf("----------------SHIFT ROW-------------:\n");
	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 24,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[0],/*ALU in*/ 0,0,0,PE_out[12][9], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[1],/*ALU in*/ 0,0,0,PE_out[12][10], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 8,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][7],PE_out[3][7],PE_out[3][7],PE_out[0][11], /*BUFF 16to1 in*/  	PE_out[0][1],PE_out[1][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][1],PE_out[0][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);


	exe32(OP, 0, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][1],PE_out[4][5],PE_out[4][1],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][4],PE_out[4][5],PE_out[4][11]); 		
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][7],PE_out[8][7],PE_out[8][7],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][4]); 		
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][7],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
}

void MixColumn(){
	printf("----------------MIX COLUMN-------------:\n");
	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][9],PE_out[12][8],PE_out[12][10],PE_out[12][10], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[12][11],PE_out[12][8],PE_out[15][7],PE_out[12][10], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[15][7],PE_out[15][7],PE_out[15][7],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
/*		PE[0] = X	3	2	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]		
		PE[1] = X 	0	X	X		X		X		X		X			T0[0]		T0[1]		T0[2] 		T0[3]		
		PE[2] = X	X	X	X 		X		X		X		X			T1[0]		T1[1]		T1[2]		T1[3]		
		PE[3] = X	X	X	X		X		X		X		X			T2[0]		T2[1]		T2[2]		T2[3]		
		*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[4],/*ALU in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 16to1 in*/  	PE_out[1][1],PE_out[0][8],PE_out[0][2],PE_out[0][1], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);


	exe32(OP, 0, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][1],PE_out[4][1],PE_out[4][1],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7]); 		
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
}

void SubCell8(){
	U64 OP = CUSTOM_OP(OP_SBOX_8);
	exe32(OP, 0, PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	exe32(OP, 0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  	PE_out[0][0],PE_out[0][1],PE_out[0][2],PE_out[0][3], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	exe32(OP, 0, PE_out[8],/*ALU in*/ 	PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7]); 		
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);	
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/   PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/   PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
}

void get_data(){
	LMM[0][0][0] = 0xbba2118f;
	LMM[0][1][0] = 0x7f37fb8e;

	LMM[1][0][0] = 0x2da96eb2;
	LMM[1][1][0] = 0x332f0211;

	LMM[2][0][0] = 0x6eb22da9;
	LMM[2][1][0] = 0x0211332f;

	LMM[3][0][0] = 0x3c126a1e;
	LMM[3][1][0] = 0xba690c12;

	LMM[4][0][0] = 0xbae335e6;
	LMM[4][1][0] = 0xe5211611;

	LMM[5][0][0] = 0x08cf9556;
	LMM[5][1][0] = 0x0f814c25;

	LMM[6][0][0] = 0xb8ca01f8;
	LMM[6][1][0] = 0xe33e8248;

	LMM[7][0][0] = 0x8816afea;
	LMM[7][1][0] = 0x648a36cb;
 	
	#if defined(ARMZYNQ)

	A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
	
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}

	A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	
	B_MSB = (U32*)(membase + ROW1_MSB_BASE_PHYS);
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			B_MSB[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}

	B_LSB = (U32*)(membase + ROW1_LSB_BASE_PHYS);
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			B_LSB[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}	
	#endif
	#if defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		int i;
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
		
//0x3c126a1e,0xba690c12,0x8816afea,0x648a36cb);	
	mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); // IN[0]		IN[1]		X		X
	mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // IN[2]		IN[3]		X		X
	mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	mop32(OP_LDW, 0, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // IN[0]		IN[1]		X		X
	mop32(OP_LDW, 0, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // IN[2]		IN[3]		X		X
	mop32(OP_LDW, 0, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][1],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	mop32(OP_LDW, 0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_SHL,/*SRU1_IM*/ 32,/*SRU2*/OP_SHL,/*SRU2_IM*/ 32,/*LU3*/OP_XOR);
	exe32(OP, 0, PE_out[8],/*ALU in*/ 	PE_out[4][4],PE_out[4][4],PE_out[4][4],PE_out[4][4], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][0],PE_out[4][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);	
	exe32(OP, 0, PE_out[9],/*ALU in*/ 	PE_out[4][4],PE_out[4][4],PE_out[4][4],PE_out[4][4], /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);					
	exe32(OP, 0, PE_out[10],/*ALU in*/ 	PE_out[4][4],PE_out[4][4],PE_out[4][4],PE_out[4][4], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][0],PE_out[6][1], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe32(OP, 0, PE_out[11],/*ALU in*/ 	PE_out[4][4],PE_out[4][4],PE_out[4][4],PE_out[4][4], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);		

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 				
	exe32(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[9][1],PE_out[9][1],PE_out[9][1],PE_out[9][1], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 				
	exe32(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[10][1],PE_out[10][1],PE_out[10][1],PE_out[10][1], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	exe32(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[11][1],PE_out[11][1],PE_out[11][1],PE_out[11][1], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);
}

void store_data(){
	mop32(OP_STW, 0, PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[12][9],0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); // IN[0]		IN[1]
	mop32(OP_STW, 0, PE_out[1],/*ALU in*/ PE_out[13][8],PE_out[13][9],0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // IN[2]		IN[3]
	mop32(OP_STW, 0, PE_out[2],/*ALU in*/ PE_out[14][8],PE_out[14][9],0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop32(OP_STW, 0, PE_out[3],/*ALU in*/ PE_out[15][8],PE_out[15][9],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
	
	mop32(OP_STW, 0, PE_out[4],/*ALU in*/ PE_out[0][10],PE_out[0][11],0,0, /*BUFF 16to1 in*/ PE_out[0][0],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // IN[0]		IN[1]		X
	mop32(OP_STW, 0, PE_out[5],/*ALU in*/ PE_out[1][10],PE_out[1][11],0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // IN[2]		IN[3]		X
	mop32(OP_STW, 0, PE_out[6],/*ALU in*/ PE_out[2][10],PE_out[2][11],0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][1],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	mop32(OP_STW, 0, PE_out[7],/*ALU in*/ PE_out[3][10],PE_out[3][11],0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);
	
	mop32(OP_STW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][0],PE_out[4][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);	
	mop32(OP_STW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);					
	mop32(OP_STW, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[6][4],PE_out[6][5],PE_out[6][0],PE_out[6][1], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	mop32(OP_STW, 0, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[7][4],PE_out[7][5],PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);		
	
	mop32(OP_STW, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][0],PE_out[8][0],PE_out[8][0],PE_out[8][0], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 				
	mop32(OP_STW, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][0],PE_out[9][0],PE_out[9][0],PE_out[9][0], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 				
	mop32(OP_STW, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][0],PE_out[10][0],PE_out[10][0],PE_out[10][0], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	mop32(OP_STW, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][0],PE_out[11][0],PE_out[11][0],PE_out[11][0], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);
}
void Skinny(int r){

	 SubCell8();
	 AddConstants(r);
	 AddKey();
	 ShiftRows();
	 MixColumn();
}

void show_result(){
	printf("RESULT:\n");
	printf("%08x\n",  PE_out[12][8]);
	printf("%08x\n",  PE_out[12][9]);
	printf("%08x\n",  PE_out[12][10]);
	printf("%08x\n",  PE_out[12][11]);
	printf("%08x\n",  PE_out[13][8]);
	printf("%08x\n",  PE_out[13][9]);
	printf("%08x\n",  PE_out[13][10]);
	printf("%08x\n",  PE_out[13][11]);
	printf("%08x\n",  PE_out[14][8]);
	printf("%08x\n",  PE_out[14][9]);
	printf("%08x\n",  PE_out[14][10]);
	printf("%08x\n",  PE_out[14][11]);
	printf("%08x\n",  PE_out[15][8]);
	printf("%08x\n",  PE_out[15][9]);
	printf("%08x\n",  PE_out[15][10]);
	printf("%08x\n",  PE_out[15][11]);
}
int main(){

	int max_input_length = 4096; // Maximum length of the input
    int block_size = 72; // Block size for padding
    uint8_t input[max_input_length + block_size]; // Input buffer
    memset(input, 0, sizeof(input)); // Initialize input buffer with zeros
	uint8_t msg[1024], digest[32];
	msg[0] = (uint8_t) 0x0;
  	msg[1] = (uint8_t) 0x1;
  	msg[2] = (uint8_t) 0x2;
	

	Init();
	//init_test();
	#if defined(ARMZYNQ)
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
	A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	
	B_MSB = (U32*)(membase + ROW1_MSB_BASE_PHYS);
	B_LSB = (U32*)(membase + ROW1_LSB_BASE_PHYS);
	// dma_read_U32(ROW0_BASE_PHYS,8);
	
	// for (int i=0; i<8; i++) {
		// printf("Pre-Output %d = ",i);
		// for (int j=0; j<8; j++) {
			// printf("%08x ",A[i*8+j]);
		// }
		// printf("\n");
		// }
	#endif



	get_data();

	for (int i = 0; i<40 ; i++){
        Skinny(i);
    }
	store_data();
	//show_result();
#if defined(ARMZYNQ)
	// for (int i=0; i<8; i++) {
		// for (int j=0; j<8; j++) {		
			// printf("A[%d] = %08x\n",j,A[i*8 +j]);
		// }
	// }
	*(CGRA_info.ctx_pe_mmap+MODE_BASE_IP) = 0;
	dma_write_U32(ROW0_MSB_BASE_PHYS,8);
	dma_write_U32(ROW0_LSB_BASE_PHYS,8);
	dma_write_U32(ROW1_MSB_BASE_PHYS,8);
	dma_write_U32(ROW1_LSB_BASE_PHYS,8);	
	// dma_read_U32(ROW0_BASE_PHYS,8);
	// printf("\n");
	// for (int i=0; i<8; i++) {
		// printf("Post-Output %d = ",i);
		// for (int j=0; j<8; j++) {
			// printf("%08x ",A[i*8+j]);
		// }
		// printf("\n");
		// }
		
	*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    while(1){
		if(*(CGRA_info.ctx_pe_mmap+(FINISH_BASE_IP)))
			break;
	}

	//B = (U32*)(membase + ROW0_BASE_PHYS);
	dma_read_U32(ROW0_MSB_BASE_PHYS,8);
	dma_read_U32(ROW0_LSB_BASE_PHYS,8);
	printf("\n");
		for (int i=0; i<8; i++) {
		printf("Output MSB %d = ",i);
		for (int j=0; j<8; j++) {
			printf("%08x ",A_MSB[i*8+j]);
		}
		printf("\n");
		}

	printf("\n");
		for (int i=0; i<8; i++) {
		printf("Output LSB %d = ",i);
		for (int j=0; j<8; j++) {
			printf("%08x ",A_LSB[i*8+j]);
		}
		printf("\n");
		}
	#endif
	Final();

	// printf("%08x\n",  LMM[0][0][0] );
	// printf("%08x\n",  LMM[0][1][0] );
	// printf("%08x\n",  LMM[1][0][0] );
	// printf("%08x\n",  LMM[1][1][0] );
	// printf("%08x\n",  LMM[2][0][0] );
	// printf("%08x\n",  LMM[2][1][0] );
	// printf("%08x\n",  LMM[3][0][0] );
	// printf("%08x\n",  LMM[3][1][0] );
	// printf("%08x\n",  LMM[4][0][0] );
	// printf("%08x\n",  LMM[4][1][0] );
	// printf("%08x\n",  LMM[5][0][0] );
	// printf("%08x\n",  LMM[5][1][0] );
	// printf("%08x\n",  LMM[6][0][0] );
	// printf("%08x\n",  LMM[6][1][0] );
	// printf("%08x\n",  LMM[7][0][0] );
	// printf("%08x\n",  LMM[7][1][0] );
	return 0;
}



