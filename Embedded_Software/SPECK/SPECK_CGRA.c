#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>
#include <byteswap.h>
#include "../CGRA.h"
#include "../CGRAlib.c"
#define u64 unsigned long long
///////////////////////////////////// 	SIPHASH - CGRA 	/////////////////////////////////////////////////////////////
#define ROR64(x,r) (((x)>>(r))|((x)<<(64-(r))))
#define ROL64(x,r) (((x)<<(r))|((x)>>(64-(r))))
#define RII(x,y,k) (y^=x, y=ROR64(y,3), x^=k, x-=y, x=ROL64(x,8))

void DOUBLE_ROUND(uint64_t v0, uint64_t v1, uint64_t v2, uint64_t v3){
	LMM[0][0][0] = v0;
	LMM[0][1][0] = v1;
	
	LMM[1][0][0] = v2;
	LMM[1][1][0] = v3;
	U64 OP;
	mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); // a		b		X		X
	mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // c		d		X		X
	mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
//HALF_ROUND_0 (V0, V1, V2, V3, 13, 16)
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 13,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[4],/*ALU in*/  PE_out[0][0],PE_out[0][1],0,PE_out[0][1], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	//	x	b	x	x
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 16,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[5],/*ALU in*/  PE_out[1][0],PE_out[1][1],0,PE_out[1][1], /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	//x		d	x	x
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/ 32,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[6],/*ALU in*/  PE_out[0][0],PE_out[0][1],0,0, /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);//	x	x	a	x
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[7],/*ALU in*/  PE_out[1][0],PE_out[1][1],0,0, /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]); //	x	x	c	x
/*		MATRIX SAMPLE:
		PE[0] = X	v1	X	X		X		X		X		X			X		X		X		X		
		PE[1] = X 	v3	X	X		X		X		X		X			X		X		X		X		
		PE[2] = X	X	v0	X 		X		X		X		X			X		X		X		X		
		PE[3] = X	X	v2	X		X		X		X		X			X		X		X		X	
		*/
//HALF_ROUND_1 (V2, V1, V0, V3, 17, 21)
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 17,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[8],/*ALU in*/  PE_out[7][2],PE_out[4][1],0,PE_out[4][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 21,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[9],/*ALU in*/  PE_out[6][2],PE_out[5][1],0,PE_out[5][1],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/ 32,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[10],/*ALU in*/ PE_out[7][2],PE_out[4][1],0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[11],/*ALU in*/ PE_out[6][2],PE_out[5][1],0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

/*		MATRIX SAMPLE:
		PE[8] = X	v1	X	X		X		X		X		X			X		X		X		X		
		PE[9] = X 	v3	X	X		X		X		X		X			X		X		X		X		
		PE[10] = X	X	v2	X 		X		X		X		X			X		X		X		X		
		PE[11] = X	X	v0	X		X		X		X		X			X		X		X		X	
*/
//HALF_ROUND_2 (V0, V1, V2, V3, 13, 16)
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 13,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[12],/*ALU in*/ PE_out[11][2],PE_out[8][1],0,PE_out[8][1], /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 16,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[13],/*ALU in*/ PE_out[10][2],PE_out[9][1],0,PE_out[9][1], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/ 32,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
 	exe64(OP, 0, PE_out[14],/*ALU in*/ PE_out[11][2],PE_out[8][1],0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[15],/*ALU in*/ PE_out[10][2],PE_out[9][1],0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
/*		MATRIX SAMPLE:
		PE[12] = X	v1	X	X		X		X		X		X			X		X		X		X		
		PE[13] = X 	v3	X	X		X		X		X		X			X		X		X		X		
		PE[14] = X	X	v0	X 		X		X		X		X			X		X		X		X		
		PE[15] = X	X	v2	X		X		X		X		X			X		X		X		X	
*/
//HALF_ROUND_3 (V2, V1, V0, V3, 17, 21)
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 17,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[0],/*ALU in*/ PE_out[15][2],PE_out[12][1],0,PE_out[12][1], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 21,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[1],/*ALU in*/ PE_out[14][2],PE_out[13][1],0,PE_out[13][1], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/ 32,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[2],/*ALU in*/ PE_out[15][2],PE_out[12][1],0,0, /*BUFF 16to1 in*/ PE_out[14][1],PE_out[15][1],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][2],PE_out[13][1],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
/*		MATRIX SAMPLE:
		PE[12] = X	v1	X	X		X		X		X		X			X		X		X		X		
		PE[13] = X 	v3	X	X		X		X		X		X			X		X		X		X		
		PE[14] = X	X	v2	X 		X		X		X		X			X		X		X		X		
		PE[15] = X	X	v0	X		X		X		X		X			X		X		X		X	
*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	mop64(OP_STW, 0, PE_out[4],/*ALU in*/ PE_out[3][2],PE_out[0][1],0,0, /*BUFF 16to1 in*/ PE_out[0][0],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 
	mop64(OP_STW, 0, PE_out[5],/*ALU in*/ PE_out[2][2],PE_out[1][1],0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
	exe64(OP, 0, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][1],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe64(OP, 0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][1],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	exe64(OP, 0, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][0],PE_out[4][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);	
	exe64(OP, 0, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);					
	exe64(OP, 0, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][0],PE_out[6][1], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);	
	exe64(OP, 0, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);		

	exe64(OP, 0, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 				
	exe64(OP, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[9][1],PE_out[9][1],PE_out[9][1],PE_out[9][1], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 				
	exe64(OP, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[10][1],PE_out[10][1],PE_out[10][1],PE_out[10][1], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	exe64(OP, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[11][1],PE_out[11][1],PE_out[11][1],PE_out[11][1], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);
}



int R(uint64_t *B, uint64_t *A, uint64_t K)
{

  LMM[0][0][0] = *B;
  LMM[0][1][0] = K;

	U64 OP;
	mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 
	mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 
	mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/ 8,/*SRU2*/OP_ROL,/*SRU2_IM*/ 3,/*LU3*/OP_NOP);
	exe64(OP, *A, PE_out[4],/*ALU in*/  PE_out[0][0],0,PE_out[0][1],PE_out[15][7], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	//	x	y	xx	oy
	exe64(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	//x		d	x	x
	exe64(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);//	x	x	a	x
	exe64(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]); //	x	x	c	x

	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/ OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe64(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][2],PE_out[4][3],PE_out[4][0],PE_out[4][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  // x		y		xx		x
	exe64(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe64(OP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe64(OP, 0, PE_out[11],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	mop64(OP_STW, 0, PE_out[12],/*ALU in*/ PE_out[8][2],PE_out[8][1],0,0,/*BUFF 16to1 in*/    PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 				
	mop64(OP_STW, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 				
	mop64(OP_STW, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	mop64(OP_STW, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);

	*B = LMM[12][0][0] ;
	*A = LMM[12][1][0] ;

	// printf("B: %016lx", *B);
	// printf("\nA: %016lx", *A);
}

int RI(uint64_t *Y, uint64_t *X, uint64_t K)
{
	//RI(x,y,k) (y^=x, y=ROR64(y,3), x^=k, x-=y, x=ROL64(x,8))
  LMM[0][0][0] = *Y;
  LMM[0][1][0] = *X;
	U64 OP;
	mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 
	mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 
	mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/ 3,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, K, PE_out[4],/*ALU in*/  PE_out[0][0],0,PE_out[0][1],PE_out[0][0], /*BUFF 16to1 in*/  	PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	//	x	xx	y	x
	exe64(OP, 0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe64(OP, 0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe64(OP, 0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/  PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	OP = BASIC_OP(/*AU*/OP_SUB2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/ 8,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, 0, PE_out[8],/*ALU in*/  PE_out[4][1],PE_out[4][2],0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); 
	exe64(OP, 0, PE_out[9],/*ALU in*/  0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe64(OP, 0, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe64(OP, 0, PE_out[11],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 	

	mop64(OP_STW, 0, PE_out[12],/*ALU in*/ PE_out[8][2],PE_out[8][3],0,0,/*BUFF 16to1 in*/    PE_out[8][1],PE_out[8][1],PE_out[8][1],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 				
	mop64(OP_STW, 0, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/    PE_out[9][1],PE_out[9][1],PE_out[9][1],PE_out[9][1], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 				
	mop64(OP_STW, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[10][1],PE_out[10][1],PE_out[10][1],PE_out[10][1], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	mop64(OP_STW, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[11][1],PE_out[11][1],PE_out[11][1],PE_out[11][1], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);

	*Y = LMM[12][0][0] ;
	*X = LMM[12][1][0] ;
}


int Encrypt(u64 *u,u64 *v,u64 key[])
{
  u64 i,x=*u,y=*v;
  u64 *xx = &x;
  u64 *yy = &y;

  for(i=0;i<32;i++) R(xx,yy,key[i]);

  *u=x; *v=y;
	printf("U: %016lx", *u);
	printf("\nV: %016lx", *v);
  return 0;
}

int Decrypt(u64 *u,u64 *v,u64 *key)
{
  int i;
  u64 x=*u,y=*v;
  u64 *xx = &x;
  u64 *yy = &y;

  for(i=31;i>=0;i--) RI(xx,yy,key[i]);

  *u=x; *v=y;
  printf("%16lx\n", *u);
  printf("%16lx\n", *v);
  return 0;
}


int ExpandKey(u64 K[],u64 key[])
{
  u64 i,B=K[1],A=K[0];
  u64 *BB = &B;
  u64 *AA = &A;
  for(i=0;i<32;i++){
    key[i]=A; 
	R(BB,AA,i);
  }

  return 0;
}
int crypto_stream_speck128128ctr_ref(
  unsigned char *out, 
  unsigned long long outlen, 
  const unsigned char *n, 
  const unsigned char *k
)
{
  u64 i,nonce[2],K[2],key[32],x,y,t;
  unsigned char *block= (unsigned char *)malloc(16);   

  if (!outlen) {free(block); return 0;}

  nonce[0]=((u64*)n)[0];
  nonce[1]=((u64*)n)[1];

  for(i=0;i<2;i++) K[i]=((u64 *)k)[i];

  ExpandKey(K,key);

  t=0;
  while(outlen>=16){
    x=nonce[1]; y=nonce[0]; nonce[0]++;
    Encrypt(&x,&y,key);                          
    ((u64 *)out)[1+t]=x; 
    ((u64 *)out)[0+t]=y; 
    t+=2;                                      
    outlen-=16;                                  
  }

  if (outlen>0){
    x=nonce[1]; y=nonce[0];
    Encrypt(&x,&y,key);
    ((u64 *)block)[1]=x; ((u64 *)block)[0]=y;
    for(i=0;i<outlen;i++) out[i+8*t]=block[i];
  }

  free(block);

  return 0;
}


void show_result(){
	printf("RESULT:\n");
	printf("%16lx\n",  PE_out[3][2]);
	printf("%16lx\n",  PE_out[0][1]);
	printf("%16lx\n",  PE_out[2][2]);
	printf("%16lx\n",  PE_out[1][1]);


}

int main(){

	Init();
	unsigned char output[16]; 
	unsigned char *out = output;
	uint64_t outlen = 16;
	unsigned char data[16];
	unsigned char *n = data;
	unsigned char key[16];

  	u64 kk[32];
	//intit 
	printf("Data: \n");
	for (int i = 0; i<16; i++){
		data[i] = i*8-5;
		printf("%02x", data[i]);
		key[i] = i;
	}
  	unsigned char *k = key;
	int c = crypto_stream_speck128128ctr_ref(out, outlen, n, k);

  	printf("\n Output:\n");
  	for (int  i = 0; i<16; i++) printf("%02x", out[i]);
	printf("\n");
  	Final();
	return 0;
}

// int main(){

// 	Init();
// 	u64 a,b,k[32],K[2],key[32];
//     b = 0xa7860dc951291788;
//     a = 0x4c8a26e2fad6912a;
// 	K[0] = 0xf93ba182d5141cc3;
// 	K[1] = 0x744005b4ceb6b93b;
//     for (int i = 0; i <32; i++) k[i] = i;
// 	u64 *u = &a;
// 	u64 *v = &b;
// 	//Encrypt(u, v, k);
// 	Decrypt(u,v, (u64*) k);
// 	// ExpandKey(K, key);
// 	// for (int i = 0; i <32; i++) printf("key[%0d]: %16lx\n", i, key[i]);
// 	printf("\n");

// 	return 0;
// }


