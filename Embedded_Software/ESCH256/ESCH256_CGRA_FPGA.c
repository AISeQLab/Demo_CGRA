#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"



#define STATE_BYTES     48
#define RATE_BYTES      16
#define STEPS_SLIM       7
#define STEPS_BIG       11


#define ESCH_DIGEST_WORDS (ESCH_DIGEST_BYTES/4)

#define STATE_BRANS (STATE_BYTES/8) //6
#define STATE_WORDS (STATE_BYTES/4) //12
#define RATE_BRANS  (RATE_BYTES/8)
#define RATE_WORDS  (RATE_BYTES/4) //4
#define CAP_BYTES   (STATE_BYTES-RATE_BYTES)
#define CAP_BRANS   (CAP_BYTES/8)
#define CAP_WORDS   (CAP_BYTES/4)

#define CONST_M1 (((uint32_t) 1) << 24)

unsigned char* membase;
U32 *A_MSB, *A_LSB;	
static const uint32_t RCON[8] = {      \
  0xB7E15162, 0xBF715880, 0x38B4DA56, 0x324E7738, \
  0xBB1185EB, 0x4F7C7B57, 0xCFBFA1C8, 0xC2B3293D  \
	};
	/*
	NOTE: 
		THERE ARE 12 WORD-STATE
		STATE-BRANS = 6
		INITIAL 12 STATE = 0 
		
		MATRIX SAMPLE:
		PE[0] = X	X	X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]		
		PE[1] = X 	X	X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]		
		PE[2] = X	X	X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]		
		PE[3] = X	X	X	X		TMPX	TMPY	X		X			X			X			X			X			

		LMM[0][0] IN[0]
		LMM[1][0] IN[1]
		LMM[2][0] IN[2]
		LMM[3][0] IN[3]

		 		
	*/
void exe_add_msg_blk(){
	U32 OP;
	U64 x = 0;
	mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); // IN[0]		IN[1]		X		X
	mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // IN[2]		IN[3]		X		X
	mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	/*
	PE[0] = IN[0]		X		X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]	
	PE[1] = IN[1] 		X		X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]	
	PE[2] = IN[2]		X		X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]	
	PE[3] = IN[3]		X		X	X		X		X		X		X			X			X			X			X		
	*/

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_SHL,/*SRU2_IM*/ 16,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[2][0],PE_out[2][0],PE_out[0][0], /*BUFF 16to1 in*/ PE_out[0][0],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X		0.5*TMPX		X		X
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[1][0],PE_out[3][0],PE_out[3][0],PE_out[1][0], /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // X		0.5*TMPY		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);		
	/*
	PE[0] = X		0.5*TMPX	X	X		IN[0]		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[1] = X 		0.5*TMPY	X	X		IN[1]		X		X		X			S[4]		S[5]		S[6] 		S[7]
	PE[2] = X		X			X	X 		IN[2]		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[3] = X		X			X	X		IN[3]		X		X		X			X			X			X			X	
	*/

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/0,0,0,PE_out[4][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  // X		TMPX		X		X
	exe32(OP, x, PE_out[9],/*ALU in*/0,0,0,PE_out[5][1],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  // X		TMPY		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, x, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 

		/*
	PE[0] = X		TMPX		X	X		IN[0]		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[1] = X 		TMPY		X	X		IN[1]		X		X		X			S[4]		S[5]		S[6] 		S[7]
	PE[2] = X		X			X	X 		IN[2]		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[3] = X		X			X	X		IN[3]		X		X		X			X			X			X			X	
	*/
  /*
  s[0] = s[0] ^ in32[0] ^ tmpy;
  s[1] = s[1] ^ in32[1] ^ tmpx;
  s[2] = s[2] ^ in32[2] ^ tmpy;
  s[3] = s[3] ^ in32[3] ^ tmpx;
  s[4] = s[4]  ^ tmpy;
  s[5] = s[5]  ^ tmpx;
  
  */
 	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
 	exe32(OP, x, PE_out[12],/*ALU in*/ PE_out[9][1],PE_out[8][8],PE_out[8][4],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); // X		S0		X		X
 	exe32(OP, x, PE_out[13],/*ALU in*/ PE_out[8][1],PE_out[8][9],PE_out[9][4],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]);  // X		S1		X		X
 	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[9][8],PE_out[11][11],PE_out[9][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); // X		S4		X		X
 	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[9][9],PE_out[11][11],PE_out[8][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  // X		S5		X		X

		/*
	PE[12] = X		S0			X	X		TMPX		X		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = X 		S1			X	X		TMPY		X		X		X			S[4]		S[5]		S[6] 		S[7]
	PE[14] = X		S4			X	X 		IN[2]		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		S5			X	X		IN[3]		X		X		X			X			X			X			X	
	*/




	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[12][10],PE_out[15][11],PE_out[13][4],PE_out[14][4], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[13][1],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //X		S2		X		X
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[12][11],PE_out[15][11],PE_out[12][4],PE_out[15][4], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //X		S3		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[14][1],0,0,PE_out[15][1], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		S5		S4		X
	exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 
			/*
	PE[12] = X		S2			X	X		S0		S1		X		X			S[0]		S[1]		S[2]		S[3]
	PE[13] = X 		S3			X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]
	PE[14] = X		S5			S4	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x			X	X		X		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][1],PE_out[1][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][10],PE_out[0][11]); 
	exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][1],PE_out[2][2],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
	exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	
			/*
	PE[12] = X		X			X	X		S2		S3		X		X			S0			S1			S[2]		S[3]
	PE[13] = X 		X			X	S5		S4		X		X		X			S[4]		S[5]		S[6] 		S[7]
	PE[14] = X		X			X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x			X	X		X		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][4],PE_out[4][5]); 
	exe32(OP, x, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][5],PE_out[5][4],PE_out[5][10],PE_out[5][11]); 
	exe32(OP, x, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);
	exe32(OP, x, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);	

	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);
	exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	
			/*
	PE[12] = X		X			X	X		x		x		X		X			S0			S1			S2			S3
	PE[13] = X 		X			X	x		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		X			X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x			X	X		X		X		X		X			X			X			X			X	
	*/
}

void arxbox(){
	U32 OP;
	U64 x = 0;
	U32 rc0 = RCON[0];
	U32 rc2 = RCON[1];
	U32 rc4 = RCON[2];
	U32 rc6 = RCON[3];
	U32 rc8 = RCON[4];
	U32 rc10 = RCON[5];
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/31,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[12][9],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X	ROT(state[1], 31)		x		x
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[12][11],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // X	ROT(state[3], 31)		x		x
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[13][9],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		ROT(state[5], 31)		x		x
	exe32(OP, x, PE_out[3],/*ALU in*/ PE_out[13][11],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X	ROT(state[7], 31)		x		x
			/*
	PE[12] = X		s131		X		X		x		x		X		X			S0			S1			S2			S3
	PE[13] = X 		s331		X		x		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		s531		X		X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		s731		X		X		X		X		X		X			X			X			X			X	
	*/


	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[4],/*ALU in*/ PE_out[0][8],PE_out[0][1],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X	 x		s0		x
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[0][10],PE_out[1][1],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // X	 x		s2		x
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[1][8],PE_out[2][1],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);// X	 x		s4		x
	exe32(OP, x, PE_out[7],/*ALU in*/ PE_out[1][10],PE_out[3][1],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 x		s6		x
			/*
	PE[12] = X		x		s0		X		x		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2		x		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4		X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6		X		X		X		X		X			X			X			X			X	
	*/


	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/24,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	PE_out[4][2],PE_out[4][2],0,PE_out[4][9], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); // X		x		s124		s0
	exe32(OP, x, PE_out[9],/*ALU in*/ 	PE_out[5][2],PE_out[5][2],0,PE_out[4][11], /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		x		s324 		s2
	exe32(OP, x, PE_out[10],/*ALU in*/ 	PE_out[6][2],PE_out[6][2],0,PE_out[5][9], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		x		s524		s4
	exe32(OP, x, PE_out[11],/*ALU in*/ 	PE_out[7][2],PE_out[7][2],0,PE_out[5][11], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		x		s724		s6	
			/*
	PE[12] = X		s124		x		s0		x		x		X		X			S0			S1			S2			S3
	PE[13] = X 		s324		x		s2		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		s524 		x		s4		X		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		s724		x		s6		X		X		X		X			X			X			X			X	
	*/
	// OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROR,/*SRU2_IM*/ 17,/*LU3*/OP_NOP);
	// exe32(OP, x, PE_out[12],/*ALU in*/ PE_out[8][3],PE_out[8][1], rc0,0,  /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		s117	  s0rc		x
	// exe32(OP, x, PE_out[13],/*ALU in*/ PE_out[9][3],PE_out[9][1], rc2,0,  /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		// x		s317	  s2rc		x
	// exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][3],PE_out[10][1],rc4,0, /*BUFF 16to1 in*/PE_out[10][1],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		s517	  s4rc		x
	// exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[11][3],PE_out[11][1],rc6,0, /*BUFF 16to1 in*/PE_out[11][1],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		s717	  s6rc		x
	
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/17,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, rc0, PE_out[12],/*ALU in*/ PE_out[8][1], PE_out[11][11],PE_out[11][11],PE_out[8][3] , /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		s117	  s0rc		x
	exe32(OP, rc2, PE_out[13],/*ALU in*/ PE_out[9][1], PE_out[11][11],PE_out[11][11],PE_out[9][3] , /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		// x		s317	  s2rc		x
	exe32(OP, rc4, PE_out[14],/*ALU in*/ PE_out[10][1],PE_out[11][11],PE_out[11][11],PE_out[10][3],/*BUFF 16to1 in*/PE_out[10][1],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		s517	  s4rc		x
	exe32(OP, rc6, PE_out[15],/*ALU in*/ PE_out[11][1],PE_out[11][11],PE_out[11][11],PE_out[11][3],/*BUFF 16to1 in*/PE_out[11][1],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		s717	  s6rc		x
	/*
	PE[12] = X		s117		s0rc		x		s124		x		X		X			S0			S1			S2			S3
	PE[13] = X 		s317		s2rc		x		s324		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		s517		s4rc 		x		s524		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		s717		s6rc		x		s724		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[12][1],PE_out[12][2],0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X	x		s0		x
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[13][1],PE_out[13][2],0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // X		x		s2		x
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[14][1],PE_out[14][2],0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		x		s4		x
	exe32(OP, x, PE_out[3],/*ALU in*/ PE_out[15][1],PE_out[15][2],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X		x		s6		x		
	/*
	PE[12] = X		x		s0		x		s124		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2		x		s324		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4 		x		s524		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6		x		s724		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/17,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[4],/*ALU in*/ PE_out[0][2],0,0,PE_out[0][4], /*BUFF 16to1 in*/ PE_out[0][2],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	// X	 x		s1		x
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[1][2],0,0,PE_out[1][4], /*BUFF 16to1 in*/ PE_out[1][2],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	// X	 x		s3		x
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[2][2],0,0,PE_out[2][4], /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 x		s5		x
	exe32(OP, x, PE_out[7],/*ALU in*/ PE_out[3][2],0,0,PE_out[3][4], /*BUFF 16to1 in*/ PE_out[3][2],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 x		s7		x
	/*
	PE[12] = X		s1		x		x		s0		x		X		X			S0			S1			S2			S3
	PE[13] = X 		s3		x		x		s2		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		s5 		x		x		s4		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		s7		x		x		s6		X		X		X			X			X			X			X	
	*/


	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, rc0, PE_out[8],/*ALU in*/ 	PE_out[4][4],PE_out[7][11],PE_out[7][11],PE_out[7][11], /*BUFF 16to1 in*/ PE_out[4][1],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); // X		s0rc		x		x		
	exe32(OP, rc2, PE_out[9],/*ALU in*/ 	PE_out[5][4],PE_out[7][11],PE_out[7][11],PE_out[7][11], /*BUFF 16to1 in*/ PE_out[5][1],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		s2rc 		x		x		
	exe32(OP, rc4, PE_out[10],/*ALU in*/ 	PE_out[6][4],PE_out[7][11],PE_out[7][11],PE_out[7][11], /*BUFF 16to1 in*/ PE_out[6][1],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		s4rc		x		x		
	exe32(OP, rc6, PE_out[11],/*ALU in*/ 	PE_out[7][4],PE_out[7][11],PE_out[7][11],PE_out[7][11], /*BUFF 16to1 in*/ PE_out[7][1],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		s6rc		x		x			
	/*
	PE[12] = X		x		s0rc		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2rc		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4rc 		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6rc		x		s7		X		X		X			X			X			X			X	
	*/

	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[12],/*ALU in*/ PE_out[8][1], PE_out[8][4] , 0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		x	  s0    	x
	exe32(OP, x, PE_out[13],/*ALU in*/ PE_out[9][1], PE_out[9][4] , 0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		// x		x	  s2    	x
	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][1],PE_out[10][4],0, 0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		x	  s4    	x
	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[11][1],PE_out[11][4],0, 0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		x	  s6    	x
	/*
	PE[12] = X		x		s0		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4 		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6		x		s7		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/31,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[12][2],0,0,PE_out[12][4], /*BUFF 16to1 in*/ PE_out[12][2],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X	s1		x	x		
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[13][2],0,0,PE_out[13][4], /*BUFF 16to1 in*/ PE_out[13][2],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // X		s3		x	x		
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[14][2],0,0,PE_out[14][4], /*BUFF 16to1 in*/ PE_out[14][2],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		s5		x	x		
	exe32(OP, x, PE_out[3],/*ALU in*/ PE_out[15][2],0,0,PE_out[15][4], /*BUFF 16to1 in*/ PE_out[15][2],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X		s7		x	x				
	/*
	PE[12] = X		s1		x		x		s0		x		X		X			S0			S1			S2			S3
	PE[13] = X 		s3		x		x		s2		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		s5 		x		x		s4		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		s7		x		x		s6		X		X		X			X			X			X			X	
	*/

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/24,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, rc0, PE_out[4],/*ALU in*/ PE_out[0][1],PE_out[3][11],0,PE_out[0][4], /*BUFF 16to1 in*/ PE_out[0][1],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	// X	 s124		s0rc		x
	exe32(OP, rc2, PE_out[5],/*ALU in*/ PE_out[1][1],PE_out[3][11],0,PE_out[1][4], /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	// X	 s324		s2rc		x
	exe32(OP, rc4, PE_out[6],/*ALU in*/ PE_out[2][1],PE_out[3][11],0,PE_out[2][4], /*BUFF 16to1 in*/ PE_out[2][1],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 s524		s4rc		x
	exe32(OP, rc6, PE_out[7],/*ALU in*/ PE_out[3][1],PE_out[3][11],0,PE_out[3][4], /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 s724		s6rc		x

	/*
	PE[12] = X	 s124		s0rc		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X	 s324		s2rc		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 s524		s4rc		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 s724		s6rc		x		s7		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	PE_out[4][2],PE_out[4][1],0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); // X		x		s0		x		
	exe32(OP, x, PE_out[9],/*ALU in*/ 	PE_out[5][2],PE_out[5][1],0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		x		s2 		x		
	exe32(OP, x, PE_out[10],/*ALU in*/ 	PE_out[6][2],PE_out[6][1],0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		x		s4		x		
	exe32(OP, x, PE_out[11],/*ALU in*/ 	PE_out[7][2],PE_out[7][1],0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		x		s6		x			
	/*
	PE[12] = X	 x		s0		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X	 x		s2		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 x		s4		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 x		s6		x		s7		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/16,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[12],/*ALU in*/ PE_out[8][2], 0, 0,PE_out[8][4] ,/*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		s1    	x	  x
	exe32(OP, x, PE_out[13],/*ALU in*/ PE_out[9][2], 0, 0,PE_out[9][4] ,/*BUFF 16to1 in*/ PE_out[9][2],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		// x		s3    	x	  x
	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][2],0,0, PE_out[10][4], /*BUFF 16to1 in*/PE_out[10][2],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		s5    	x	  x
	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[11][2],0,0, PE_out[11][4], /*BUFF 16to1 in*/PE_out[11][2],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		s7    	x	  x
	/*
	PE[12] = X	 s1		x		x		s0		x		X		X			S0			S1			S2			S3
	PE[13] = X	 s3		x		x		s2		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 s5		x		x		s4		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 s7		x		x		s6		X		X		X			X			X			X			X	
	*/
	///LOOP 3------------------------------------------------
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, rc0, PE_out[0],/*ALU in*/ PE_out[12][4],0,PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[13][1],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X		s0		x	x		
	exe32(OP, rc2, PE_out[1],/*ALU in*/ PE_out[13][4],0,PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[14][1],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // X		s2		x	x		
	exe32(OP, rc4, PE_out[2],/*ALU in*/ PE_out[14][4],0,PE_out[15][11],PE_out[15][11],/*BUFF 16to1 in*/ PE_out[15][1],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		s4		x	x		
	exe32(OP, rc6, PE_out[3],/*ALU in*/ PE_out[15][4],0,PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[15][1],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X		s6		x	x				

	/*
	PE[12] = X	 s0		x		x		s1		s3		X		X			S0			S1			S2			S3
	PE[13] = X	 s2		x		x		s5		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 s4		x		x		s7		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 s6		x		x		s7		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/31,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[4],/*ALU in*/ PE_out[2][9],0,0,PE_out[3][11], /*BUFF 16to1 in*/ PE_out[0][1],PE_out[1][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][4],PE_out[0][10],PE_out[0][5]); 	//	x	s931	x	x
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[3][1],0,0,PE_out[3][4], /*BUFF 16to1 in*/ PE_out[1][4],PE_out[2][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][4],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/31,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[2][11],0,0,PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 ss1131		x		x
	exe32(OP, x, PE_out[7],/*ALU in*/ PE_out[3][11],PE_out[3][11],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	
	/*
	PE[12] = X	 s931		x		x		s0		s2		X		X			S0			S[1]			S2			S[3]
	PE[13] = X	 s7			s6		x		s5		s4		X		X			S4			S[5]			S[6] 		S[7]
	PE[14] = X	 s1131		x		x		s7		X		X		X			S8			S9				S10			S11
	PE[15] = X	 x			x		x		s7		X		X		X			X			X				X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][4],PE_out[4][9],PE_out[4][5],PE_out[4][11]); // X		
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2],PE_out[5][1],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][5],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[10],/*ALU in*/ 	PE_out[6][8],PE_out[4][1],0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		
	exe32(OP, x, PE_out[11],/*ALU in*/ 	PE_out[6][10],PE_out[6][1],0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		
		/*
	PE[12] = X	 x		x			x		s0		s2		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 x		x			x		s[6]	s[7]	X		X			S[4]			S[5]			S6 			S7
	PE[14] = X	 x		s8			x		s7		X		X		X			S8				S9				S10			S11
	PE[15] = X	 x		s10			x		s7		X		X		X			X				X				X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][4],PE_out[9][5]); 		// x	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/24,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);	
	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][2], PE_out[10][2],0, PE_out[10][9], /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		
	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[11][2], PE_out[11][2],0, PE_out[10][11], /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		
	/*
	PE[12] = X	 x			x			x		x		x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 x			x			x		x		x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 s924		x			s8		x		X		X		X			S8				S9				S10			S11
	PE[15] = X	 s1124		x			s10		x		X		X		X			X				X				X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
																																	  
	exe32(OP, x, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);
	exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]);
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/17,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, rc8, PE_out[2],/*ALU in*/ PE_out[14][1],PE_out[15][11],PE_out[15][11], PE_out[14][3],/*BUFF 16to1 in*/ PE_out[14][1],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); 
	exe32(OP, rc10, PE_out[3],/*ALU in*/ PE_out[15][1],PE_out[15][11],PE_out[15][11],PE_out[15][3],/*BUFF 16to1 in*/ PE_out[15][1],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 	
	/*
	PE[12] = X	 x			x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 x			x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 s917		s8rc		x		s924		X		X		X			S8				S9				S10			S11
	PE[15] = X	 s1117		s10rc		x		s1124		X		X		X			X				X				X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[4],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, x, PE_out[5],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[2][1],PE_out[2][2],0,0, /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 x		s8		x
	exe32(OP, x, PE_out[7],/*ALU in*/ PE_out[3][1],PE_out[3][2],0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 x		s10		x
	/*
	PE[12] = X	 x			x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 x			x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 x			s8			x		s924		X		X		X			S8				S9				S10			S11
	PE[15] = X	 x			s10			x		s1124		X		X		X			X				X				X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); 	
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/17,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[10],/*ALU in*/ 	PE_out[6][2],0,0,PE_out[6][4], /*BUFF 16to1 in*/ PE_out[6][2],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		s1		x		x		
	exe32(OP, x, PE_out[11],/*ALU in*/ 	PE_out[7][2],0,0,PE_out[7][4], /*BUFF 16to1 in*/ PE_out[7][2],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		s3		x		x			

	/*
	PE[12] = X	 	x			x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 	x			x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 	s9			x			x		s8			X		X		X			S8				S9				S10			S11
	PE[15] = X	 	s11			x			x		s10			X		X		X			X				X				X			X	
	*/
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[12],/*ALU in*/0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, x, PE_out[13],/*ALU in*/0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);		
	exe32(OP, rc8, PE_out[14],/*ALU in*/ PE_out[10][4],PE_out[11][11],PE_out[11][11],PE_out[11][11], /*BUFF 16to1 in*/PE_out[10][1],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		s8rc	  x    	x
	exe32(OP, rc10, PE_out[15],/*ALU in*/ PE_out[11][4],PE_out[11][11],PE_out[11][11],PE_out[11][11], /*BUFF 16to1 in*/PE_out[11][1],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		s10rc	  x    	x

	/*
	PE[12] = X	 	x		x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 	x		x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 	x		s8rc		x		s9			X		X		X			S8				S9				S10			S11
	PE[15] = X	 	x		s10rc		x		s11			X		X		X			X				X				X			X	
	*/

	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[0],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][2],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 
	exe32(OP, x, PE_out[1],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][2],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[14][1],PE_out[14][4],0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		x	s8		x		
	exe32(OP, x, PE_out[3],/*ALU in*/ PE_out[15][1],PE_out[15][4],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X		x	s10		x				

	/*
	PE[12] = X	 	x		x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 	x		x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 	x		s8			x		s9			X		X		X			S8				S9				S10			S11
	PE[15] = X	 	x		s10			x		s11			X		X		X			X				X				X			X	
	*/

	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][1],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 
	exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROR,/*SRU1_IM*/31,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[2][2],0,0,PE_out[2][4], /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 s9		x		x
	exe32(OP, x, PE_out[7],/*ALU in*/ PE_out[3][2],0,0,PE_out[3][4], /*BUFF 16to1 in*/ PE_out[3][2],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 s11	x		x
	/*
	PE[12] = X	 	x			x		x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 	x			x		x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 	s9			x		x		s8			X		X		X			S8				S9				S10			S11
	PE[15] = X	 	s11			x		x		s10			X		X		X			X				X				X			X	
	*/
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	 0,0,0,0,/*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);	
	exe32(OP, x, PE_out[9],/*ALU in*/ 	 0,0,0,0,/*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_ROR,/*SRU1_IM*/24,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);	
	exe32(OP, rc8, PE_out[10],/*ALU in*/ 	PE_out[6][1],0,0, PE_out[6][4],/*BUFF 16to1 in*/ PE_out[6][1],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		s924		s8rc		x		
	exe32(OP, rc10, PE_out[11],/*ALU in*/ 	PE_out[7][1],0,0,PE_out[7][4], /*BUFF 16to1 in*/ PE_out[7][1],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		s1124		s10rc		x			
	/*
	PE[12] = X	 	x			x			x		x			x		X		X			S[0]			S[1]			S[2]		S[3]
	PE[13] = X	 	x			x			x		x			x		X		X			[4]				S[5]			S[6] 		S[7]
	PE[14] = X	 	s924		s8rc		x		s9			X		X		X			S8				S9				S10			S11
	PE[15] = X	 	s1124		s10rc		x		s11			X		X		X			X				X				X			X	
	*/
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][2],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][2],PE_out[10][1],0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		x	s8    	x	  
	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[11][2],PE_out[11][1],0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		x	s10    	x	  

	/*
	PE[12] = X	 x		x		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X	 x		x		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 x		s8		x		s9		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 x		s10		x		s11		X		X		X			X			X			X			X	
	*/
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[0],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][3],PE_out[13][1],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP, x, PE_out[1],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][3],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/16,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[14][2],0,0,PE_out[14][4], /*BUFF 16to1 in*/ PE_out[14][2],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X		s9		x	x		
	exe32(OP, x, PE_out[3],/*ALU in*/ PE_out[15][2],0,0,PE_out[15][4], /*BUFF 16to1 in*/ PE_out[15][2],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // X		s11		x	x				
	/*
	PE[12] = X	 x		x		x		x		x		X		X			S0			S1			S2			S3
	PE[13] = X	 x		x		x		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 s9		x		x		s8		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 s11	x		x		s10		X		X		X			X			X			X			X	
	*/
	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][1],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, rc8, PE_out[6],/*ALU in*/ PE_out[2][4],0,PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][1],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 s8		x		x
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	//exe32(OP, x, PE_out[7],/*ALU in*/ rc10,0,PE_out[3][4],PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 s10		x		x
	exe32(OP, rc10, PE_out[7],/*ALU in*/ PE_out[3][11],0,PE_out[3][1],PE_out[3][4], /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 s10		x		x
	   /*
	PE[12] = X	 x		x		x		x		x		X		X			S0			S1			S2			S3
	PE[13] = X	 x		x		x		x		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X	 s8		x		x		s9		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X	 10		x		s11		s11		X		X		X			X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); 
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); 
	exe32(OP, x, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][1],PE_out[6][4],PE_out[7][1],PE_out[7][0], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);
	exe32(OP, x, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);

	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		
	exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/0,0,0,0, /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);

}


void linear(){
	U32 OP;
	U64 x = 0;

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[12][10],PE_out[15][11],PE_out[13][8], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //X		S024		X		X
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[12][9],PE_out[12][11],PE_out[13][9],PE_out[15][11], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //X		S135		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0,/*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); 
	exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);
		/*
	PE[12] = X	s024	x		X		X	X	X	X		S[0]		S[1]		S[2]		S[3]
	PE[13] = X 	s135	x		X		X	X	X	X		S[4]		S[5]		S[6] 		S[7]
	PE[14] = X	x		x		X 		X	X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[15] = X	x		x		X		X	X	X	X		X			X			X			X	
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_SHL,/*SRU2_IM*/ 16,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[4],/*ALU in*/  PE_out[0][1], PE_out[0][1], PE_out[3][11], PE_out[3][11], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X		0.5*TMPX		X		X
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[1][1],PE_out[1][1],PE_out[3][11], PE_out[3][11], /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][2],PE_out[2][1],PE_out[2][2], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]);// X		0.5*TMPY		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][2],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	
		/*
	PE[12] = X	mpx	x		X		X	X	X	X		S[0]		S[1]		S[2]		S[3]
	PE[13] = X 	mpy	x		X		X	X	X	X		S[4]		S[5]		S[6] 		S[7]
	PE[14] = X	x	x		X 		X	X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[15] = X	x	x		X		X	X	X	X		X			X			X			X	
	*/

	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/ 16,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[8],/*ALU in*/PE_out[7][11],0,0,PE_out[4][1],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  // X		TMPX		X		X
	exe32(OP, x, PE_out[9],/*ALU in*/PE_out[7][11],0,0,PE_out[5][1],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  // X		TMPY		X		X
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP, x, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
		/*
	PE[8] = X	tmpx	x		X		X	X	X	X		S[0]		S[1]		S[2]		S[3]
	PE[9] = X 	tmpy	x		X		X	X	X	X		S[4]		S[5]		S[6] 		S[7]
	PE[10] = X	x		x		X 		X	X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[11] = X	x		x		X		X	X	X	X		X			X			X			X	
	*/
	// state[0] = state[8] ^ state[2] ^ tmpy
	// state[8] = state[2]
	// state[1] = state[9] ^ state[3] ^ tmpx
	// state[9] = state[3]
	// state[2] = state[10] ^ state[4] ^ tmpy
	// state[10] = state[4]
	// state[3] = state[11] ^ state[5] ^ tmpx
	// state[11] = state[5]
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[12],/*ALU in*/ PE_out[10][8],PE_out[8][10],PE_out[9][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); // X		S0		X		X
 	exe32(OP, x, PE_out[13],/*ALU in*/ PE_out[10][9],PE_out[8][11],PE_out[8][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]);  // X		S1		X		X
 	exe32(OP, x, PE_out[14],/*ALU in*/ PE_out[10][10],PE_out[9][8],PE_out[9][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); // X		S2		X		X
 	exe32(OP, x, PE_out[15],/*ALU in*/ PE_out[10][11],PE_out[9][9],PE_out[8][1],PE_out[11][11], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  // X		S3		X		X

		/*
	PE[12] = 	X	S0	x	X		tmpx	X	X	X		S[0]		S[1]		S8			S9
	PE[13] = 	X 	S1	x	X		tmpy	X	X	X		S10			S11			S[6] 		S[7]
	PE[14] = 	X	S2	x	X 		X		X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[15] = 	X	S3	x	X		X		X	X	X		X			X			X			X	
	*/	
	/*
	s4 = s6 ^ x0 ^tmpy
	s6 = x0
	s5 = s7 ^ y0 ^ tmpx
	s7 = y0
	*/
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[14][1],0,0,PE_out[15][1], /*BUFF 16to1 in*/ PE_out[12][1],PE_out[13][1],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // x	s3	s2	x
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
	exe32(OP, x, PE_out[1],/*ALU in*/ PE_out[13][10],PE_out[12][8],PE_out[13][4],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // X	s4		x		x
	exe32(OP, x, PE_out[2],/*ALU in*/ PE_out[13][11],PE_out[12][9],PE_out[12][4],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // X	s5		x		x
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 
		/*
	PE[0] = 	X	s3	s2	X		s0	s1	X	X		S6			S7			S8			S9
	PE[1] = 	X 	S4	x	X		x	X	X	X		S10			S11			S[6] 		S[7]
	PE[2] = 	X	S5	x	X 		X	X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[3] = 	X	x	x	X		X	X	X	X		X			X			X			X	
	*/	
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[4],/*ALU in*/ PE_out[3][11],PE_out[3][11],PE_out[3][11],PE_out[3][11], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][2],PE_out[0][1], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]);
	exe32(OP, x, PE_out[5],/*ALU in*/ PE_out[0][8],0,0,PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][1],PE_out[2][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // X	 s7		s6		x
	exe32(OP, x, PE_out[6],/*ALU in*/ PE_out[0][10],PE_out[1][9],PE_out[1][8],PE_out[0][11], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);// s10	 s9		s8		s11
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	
		/*
	PE[0] = 	X	x	x	X		s0	s1	s2	s3		S6			S7			S8			S9
	PE[1] = 	X 	s7	s6	X		s4	s5	X	X		S10			S11			S[6] 		S[7]
	PE[2] = 	s10	s9	s8	s11 	X	X	X	X		S[8]		S[9]		S[10]		S[11]
	PE[3] = 	X	x	x	X		X	X	X	X		X			X			X			X	
	*/	

	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); 
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][2],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); 
	exe32(OP, x, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][2],PE_out[6][1],PE_out[6][0],PE_out[6][3], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);
	exe32(OP, x, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);
			/*
	PE[0] = 	X	x	x	X		s0	s1	s2	s3		S6			S7			S8			S9
	PE[1] = 	X 	x	x	X		s4	s5	s6	s7		S10			S11			S[6] 		S[7]
	PE[2] = 	x	x	x	x 		s8	s9	s10	s11		S[8]		S[9]		S[10]		S[11]
	PE[3] = 	X	x	x	X		X	X	X	X		X			X			X			X	
	*/	

	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 		
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 		
	exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]);
	exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7]);
	/*
	PE[0] = x	X		X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]	
	PE[1] = x	X		X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]	
	PE[2] = x	X		X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]	
	PE[3] = x	X		X	X		X		X		X		X			X			X			X			X		
	*/


}

void sparkle(int step){
	uint32_t OP, tmp;
	U64 x = 0;
	for (int i = 0; i<step; i++){
		// printf("sparkle");
		tmp = RCON[i%8];
		// printf("tmp = %08x\n",tmp);
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
		// exe32(OP, x, PE_out[0],/*ALU in*/  PE_out[12][9], PE_out[12][11],tmp,i, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X	s3		s1		x
		// exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 
		// exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); 
		// exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 
		
		// exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][1],PE_out[0][2],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 
		// exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
		// exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
		// exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);		
		exe32(OP, tmp, PE_out[0],/*ALU in*/  PE_out[12][9],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // X	s1		0		x
		exe32(OP, i, PE_out[1],/*ALU in*/ PE_out[12][11],PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // x	s3		x		x
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
		exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); 
		exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 
		
		exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][1],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 
		exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][0],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
		exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
		exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);		

		exe32(OP, x, PE_out[8],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][5],PE_out[4][10],PE_out[4][4]);  
		exe32(OP, x, PE_out[9],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
		exe32(OP, x, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
		exe32(OP, x, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
		
		exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); // X		S0		X		X
 		exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]);  // X		S1		X		X
 		exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); // X		S4		X		X
 		exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  // X		S5		X		X

		arxbox();
		linear();
	}

}
void const_to_s5(uint32_t c){
	U32 OP;
	U64 x = 0;
		/*
	PE[12] = x	X		X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]	
	PE[13] = x	X		X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]	
	PE[14] = x	X		X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]	
	PE[15] = x	X		X	X		X		X		X		X			X			X			X			X		
	*/
	// printf("c = %08x\n",c);
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);
		exe32(OP, x, PE_out[0],/*ALU in*/ PE_out[15][11],PE_out[15][11],PE_out[15][11],PE_out[15][11],  /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  
		exe32(OP, (c<<24), PE_out[1],/*ALU in*/ PE_out[13][9], PE_out[15][11],PE_out[15][11],PE_out[15][11], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // x		s5		x		x
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);
		exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); 
		exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); 
		
		exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][1],PE_out[0][2],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 
		exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][1],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
		exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][0],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
		exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][0],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);		

		exe32(OP, x, PE_out[8],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][5],PE_out[4][10],PE_out[4][4]);  
		exe32(OP, x, PE_out[9],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][4],PE_out[5][10],PE_out[5][11]);  
		exe32(OP, x, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
		exe32(OP, x, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
		
		exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 		exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 		exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 		exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 


}
void add_msg_blk(const uint8_t *in, FILE *fs){
	uint32_t buffer[RATE_WORDS];
  	uint32_t *in32;
 	uint32_t tmpx = 0, tmpy = 0;
 	int i;
  
 	in32 = (uint32_t *) in;
	LMM[0][0][0] = in32[0];
	LMM[1][0][0] = in32[1];	
	LMM[2][0][0] = in32[2];	
	LMM[3][0][0] = in32[3];	
	// LMM[0][0][0] = 1234;
	// LMM[1][0][0] = 1234;	
	// LMM[2][0][0] = 1234;	
	// LMM[3][0][0] = 1234;	
	#if defined(ARMZYNQ)
	A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				//A[i*8 +j] = LMM[j/2][j%2][0]+i; //for different inputs
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	
	A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				//A[i*8 +j] = LMM[j/2][j%2][0]+i; //for different inputs
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (int i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
			
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",8);															
	#endif

	fprintf(fs,"%08x\n",in32[0]);
	fprintf(fs,"%08x\n",in32[1]);
	fprintf(fs,"%08x\n",in32[2]);
	fprintf(fs,"%08x\n",in32[3]);
	// main_string[0] = in32[0];
	// main_string[1] = in32[1];
	// main_string[2] = in32[2];
	// main_string[3] = in32[3];
	exe_add_msg_blk();
}	

void add_msg_blk_last(const uint8_t *in, size_t inlen, FILE *fs){
	uint32_t buffer[RATE_WORDS];
  	uint8_t *bufptr;
  	memcpy(buffer, in, inlen);
  	if (inlen < RATE_BYTES) {  // padding
  	  bufptr = ((uint8_t *) buffer) + inlen;
  	  memset(bufptr, 0, (RATE_BYTES - inlen));
  	  *bufptr = 0x80;
  }
  	LMM[0][0][0] = buffer[0];
	LMM[1][0][0] = buffer[1];	
	LMM[2][0][0] = buffer[2];	
	LMM[3][0][0] = buffer[3];	

	#if defined(ARMZYNQ)

	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				//A[i*8 +j] = LMM[j/2][j%2][0]+i; //for different inputs
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_MSB[i*8 +j] = LMM[j/2][j%2][0];
			
			//printf("A[%d] = %08x\n",j,A[i*8 +j]);
		}
	}
	
	for (int i=0; i<8; i++) {
		for (int j=0; j<8; j++) {
			if(j==0)
				//A[i*8 +j] = LMM[j/2][j%2][0]+i; //for different inputs
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
			else
				A_LSB[i*8 +j] = LMM[j/2][j%2][0];
			
			//printf("A[%d] = %08x\n",j,A[i*8 +j]);
		}
	}
	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (int i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;		
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",8);														  
	#endif

	fprintf(fs,"%08x\n",buffer[0]);
	fprintf(fs,"%08x\n",buffer[1]);
	fprintf(fs,"%08x\n",buffer[2]);
	fprintf(fs,"%08x\n",buffer[3]);
	
	exe_add_msg_blk();
}
// void EndianSwap(uint32_t &x){
// 	x = (x >> 56/2) |
// 		((x << 40/2) & 0x00FF000000000000) |
// 		((x << 24/2) & 0x0000FF0000000000) |
// 		((x << 8/2)  & 0x000000FF00000000) |
// 		((x >> 8/2)  & 0x00000000FF000000) |
// 		((x >> 24/2) & 0x0000000000FF0000) |
// 		((x >> 40/2) & 0x000000000000FF00) |
// 		(x << 56/2);
// }
void store1(){
		/*
	PE[12] = x	X		X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]	
	PE[13] = x	X		X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]	
	PE[14] = x	X		X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]	
	PE[15] = x	X		X	X		X		X		X		X			X			X			X			X		
	*/
	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_XOR);

	mop32(OP_STW, 0, PE_out[0],/*ALU in*/ PE_out[12][8],PE_out[12][9],0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	mop32(OP_STW, 0, PE_out[1],/*ALU in*/ PE_out[12][10],PE_out[12][11],0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	exe32(OP,0, PE_out[2],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	exe32(OP,0, PE_out[3],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);


	exe32(OP,0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP,0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/	  PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP,0, PE_out[6],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/    PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP,0, PE_out[7],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/    PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	
	exe32(OP,0, PE_out[8],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP,0, PE_out[9],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP,0, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP,0, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	
	exe32(OP,0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP,0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP,0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP,0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
}


void store2(){
/*
	PE[12] = x	X		X	X		X		X		X		X			S[0]		S[1]		S[2]		S[3]	
	PE[13] = x	X		X	X		X		X		X		X			S[4]		S[5]		S[6] 		S[7]	
	PE[14] = x	X		X	X 		X		X		X		X			S[8]		S[9]		S[10]		S[11]	
	PE[15] = x	X		X	X		X		X		X		X			X			X			X			X		
	*/
	U64 OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/ OP_NOP,/*LU3*/OP_NOP);

	exe32(OP,0, PE_out[0],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); 	
	exe32(OP,0, PE_out[1],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); 	
	mop32(OP_STW,  0, PE_out[2],/*ALU in*/  PE_out[12][8],PE_out[12][9],0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);
	mop32(OP_STW,  0, PE_out[3],/*ALU in*/  PE_out[12][10],PE_out[12][11],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);

	exe32(OP,  0, PE_out[4],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/   PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	
	exe32(OP,  0, PE_out[5],/*ALU in*/  0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	
	exe32(OP,  0, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);
	exe32(OP,  0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/   PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);

	

	exe32(OP,0, PE_out[8],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]);  
	exe32(OP,0, PE_out[9],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]);  
	exe32(OP,0, PE_out[10],/*ALU in*/0,0,0,0, /*BUFF 16to1 in*/ 	PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
	exe32(OP,0, PE_out[11],/*ALU in*/0,0,0,0,  /*BUFF 16to1 in*/ 	PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 
	
	exe32(OP,0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 
 	exe32(OP,0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 
 	exe32(OP,0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
 	exe32(OP,0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 
}

void create_input(const uint8_t *in, int inlen){
	FILE *fs;
	fs = fopen("main_string.txt","w");
	uint32_t hash[8];
//	memset(hash, 0, 32);
	while (inlen > 16) {
    // addition of a message block to the state
    add_msg_blk(in, fs);
	
    // execute SPARKLE with slim number of steps
    sparkle(7);
    inlen -= 16;
    in += 16;
  	}
//	state[STATE_BRANS-1] ^= ((inlen < RATE_BYTES) ? CONST_M1 : CONST_M2);
	if (inlen < 16) const_to_s5(1);
	else const_to_s5(2);

  	 add_msg_blk_last(in, inlen, fs);
  	// // execute SPARKLE with big number of steps
  	 sparkle(11);
	 hash[0] = PE_out[12][8];
	 hash[1] = PE_out[12][9];
	 hash[2] = PE_out[12][10];
	 hash[3] = PE_out[12][11];
	 store1();
	 //hash += 4;
	 sparkle(7);
	 // hash[4] = PE_out[12][8];
	 // hash[5] = PE_out[12][9];
	 // hash[6] = PE_out[12][10];
	 // hash[7] = PE_out[12][11];
	 store2();
	 // printf("HASH VALUE: \n");
	 // for (int i = 0; i<8; i++){
		// EndianSwap(hash[i]);
		// printf("%08x\n", hash[i]);
	 // }
	 // printf("\n");


	// for (int ldm = 0; ldm <4; ldm++ ){
		// printf("%08x	%08x	\n",LMM[ldm][0][0], LMM[ldm][1][0]);
	// }
	//printf("STORE LDM SUCCEESS! FROM LMM[0] -> LMM[3]\n");
	fclose(fs);

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
	exe32(OP, x, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x7f37fb80,0xbba211f6,0xbae335e6,0xe5211611); 
	exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x2da96eb2,0x332f0211,0x08cf9556,0x0f814c25); 
	exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0x6eb22da9,0x0211332f,0xb8ca01f8,0xe33e8248);
	exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);	

	// exe32(OP, x, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); 
	// exe32(OP, x, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0x1000000,0,0); 
	// exe32(OP, x, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
	// exe32(OP, x, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);	

	exe32(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][2],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); 	// X	 x		s1		x
	exe32(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[1][2],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 	// X	 x		s3		x
	exe32(OP, x, PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]);	// X	 x		s5		x
	exe32(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][2],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);	// X	 x		s7		x
	/*
	PE[12] = X		x		s1		x		s0		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s3		x		s2		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s5 		x		s4		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s7		x		s6		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[8],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][2],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); // X		s0rc		x		x		
	exe32(OP, x, PE_out[9],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		s2rc 		x		x		
	exe32(OP, x, PE_out[10],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][2],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]);// x		s4rc		x		x		
	exe32(OP, x, PE_out[11],/*ALU in*/ 	0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][2],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		s6rc		x		x			
	/*
	PE[12] = X		x		s0rc		x		s1		x		X		X			S0			S1			S2			S3
	PE[13] = X 		x		s2rc		x		s3		X		X		X			S4			S5			S[6] 		S[7]
	PE[14] = X		x		s4rc 		x		s5		X		X		X			S[8]		S[9]		S[10]		S[11]
	PE[15] = X		x		s6rc		x		s7		X		X		X			X			X			X			X	
	*/

	exe32(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); 		// x		x	  s0    	x
	exe32(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0,/*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); 		// x		x	  s2    	x
	exe32(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]);// x		x	  s4    	x
	exe32(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);// x		x	  s6    	x
}

void show_result(){

	printf("state[%1x] = %8lx\n", 0, PE_out[12][8]);
	printf("state[%1x] = %8lx\n", 1, PE_out[12][9]);
	printf("state[%1x] = %8lx\n", 2, PE_out[12][10]);
	printf("state[%1x] = %8lx\n", 3, PE_out[12][11]);
	printf("state[%1x] = %8lx\n", 4, PE_out[13][8]);
	printf("state[%1x] = %8lx\n", 5, PE_out[13][9]);
	printf("state[%1x] = %8lx\n", 6, PE_out[13][10]);
	printf("state[%1x] = %8lx\n", 7, PE_out[13][11]);
	printf("state[%1x] = %8lx\n", 8, PE_out[14][8]);
	printf("state[%1x] = %8lx\n", 9, PE_out[14][9]);
	printf("state[%1x] = %8lx\n", 10, PE_out[14][10]);
	printf("state[%1x] = %8lx\n", 11, PE_out[14][11]);
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
	
	//create_input(msg, 3);
	//init_test();
	//exe_add_msg_blk();
	//arxbox();
	#if defined(ARMZYNQ)
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
	A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
	// dma_read(ROW0_BASE_PHYS,8);
	
	// for (int i=0; i<8; i++) {
		// printf("Pre-Output %d = ",i);
		// for (int j=0; j<8; j++) {
			// printf("%08x ",A[i*8+j]);
		// }
		// printf("\n");
		// }
	#endif
	
	create_input(msg, 3);
	//linear();
	//sparkle(1);
	//store1();
	#if defined(ARMZYNQ)
	// for (int i=0; i<8; i++) {
		// for (int j=0; j<8; j++) {		
			// printf("A[%d] = %08x\n",j,A[i*8 +j]);
		// }
	// }
	*(CGRA_info.ctx_pe_mmap+MODE_BASE_IP) = 0;
	dma_write_U32(ROW0_MSB_BASE_PHYS,8);
	dma_write_U32(ROW0_LSB_BASE_PHYS,8);
	
	// dma_read(ROW0_BASE_PHYS,8);
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
		if(*(CGRA_info.ctx_pe_mmap+(FINISH_BASE_IP>>2)))
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
	return 0;
}



/******************************************************************************
                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/
/*  TEST ARXBOX
#include <iostream>

using namespace std;
#define ROT(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define ELL(x) (ROT(((x) ^ ((x) << 16)), 16))
typedef unsigned char UChar;
typedef unsigned long long int ULLInt;
// Round constants
static const uint32_t RCON[8] = {      \
  0xB7E15162, 0xBF715880, 0x38B4DA56, 0x324E7738, \
  0xBB1185EB, 0x4F7C7B57, 0xCFBFA1C8, 0xC2B3293D  \
};

#define STATE_BYTES     48
#define RATE_BYTES      16
#define STEPS_SLIM       7
#define STEPS_BIG       11


#define ESCH_DIGEST_WORDS (ESCH_DIGEST_BYTES/4)

#define STATE_BRANS (STATE_BYTES/8) //6
#define STATE_WORDS (STATE_BYTES/4) //12
#define RATE_BRANS  (RATE_BYTES/8)
#define RATE_WORDS  (RATE_BYTES/4) //4
#define CAP_BYTES   (STATE_BYTES-RATE_BYTES)
#define CAP_BRANS   (CAP_BYTES/8)
#define CAP_WORDS   (CAP_BYTES/4)
int main()
{
    uint32_t state[12], rc;
    state[0] =	0x7f37fb80;
	state[1] =  0xbba211f6;
	state[2] =  0xbae335e6;
	state[3] =  0xe5211611;
	state[4] =  0x2da96eb2;
	state[5] =  0x332f0211;
	state[6] =  0x08cf9556;
	state[7] =  0x0f814c25;
    for(int j = 0; j < 12; j += 2) {
      rc = RCON[j>>1];
      state[j] += ROT(state[j+1], 31);
    //  cout<<hex<<ROT(state[j+1], 31)<<"     ";
      state[j+1] ^= ROT(state[j], 24);
       state[j] ^= rc;
       state[j] += ROT(state[j+1], 17);
       state[j+1] ^= ROT(state[j], 17);
       state[j] ^= rc;
       state[j] += state[j+1];
       state[j+1] ^= ROT(state[j], 31);
       state[j] ^= rc;
       state[j] += ROT(state[j+1], 24);
       state[j+1] ^= ROT(state[j], 16);
       state[j] ^= rc;
    }
    
    for (int i = 0; i<12; i++){
        cout<<hex<<"state["<<i<<"] = "<<state[i]<<endl;
    }

    return 0;
}
*/



/*   TEST LINEAR
#include <iostream>

using namespace std;
#define ROT(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define ELL(x) (ROT(((x) ^ ((x) << 16)), 16))
typedef unsigned char UChar;
typedef unsigned long long int ULLInt;
// Round constants
static const uint32_t RCON[8] = {      \
  0xB7E15162, 0xBF715880, 0x38B4DA56, 0x324E7738, \
  0xBB1185EB, 0x4F7C7B57, 0xCFBFA1C8, 0xC2B3293D  \
};

#define STATE_BYTES     48
#define RATE_BYTES      16
#define STEPS_SLIM       7
#define STEPS_BIG       11


#define ESCH_DIGEST_WORDS (ESCH_DIGEST_BYTES/4)

#define STATE_BRANS (STATE_BYTES/8) //6
#define STATE_WORDS (STATE_BYTES/4) //12
#define RATE_BRANS  (RATE_BYTES/8)
#define RATE_WORDS  (RATE_BYTES/4) //4
#define CAP_BYTES   (STATE_BYTES-RATE_BYTES)
#define CAP_BRANS   (CAP_BYTES/8)
#define CAP_WORDS   (CAP_BYTES/4)

int main()
{
    
    uint32_t state[12], rc;
    state[0] =	0x7f37fb80;
	state[1] =  0xbba211f6;
	state[2] =  0xbae335e6;
	state[3] =  0xe5211611;
	state[4] =  0x2da96eb2;
	state[5] =  0x332f0211;
	state[6] =  0x08cf9556;
	state[7] =  0x0f814c25;
    int brans = 6;
    int j;
    uint32_t x0, y0;
    uint32_t tmpx = x0 = state[0];
    uint32_t tmpy = y0 = state[1];
    for(j = 2; j < brans; j += 2) {
      tmpx ^= state[j];
      tmpy ^= state[j+1];
    }
    tmpx = ELL(tmpx);
    tmpy = ELL(tmpy);
    for (j = 2; j < brans; j += 2) {
      state[j-2] = state[j+brans] ^ state[j] ^ tmpy;
      state[j+brans] = state[j];
      state[j-1] = state[j+brans+1] ^ state[j+1] ^ tmpx;
      state[j+brans+1] = state[j+1];
    }
    state[brans-2] = state[brans] ^ x0 ^ tmpy;
    state[brans] = x0;
    state[brans-1] = state[brans+1] ^ y0 ^ tmpx;
    state[brans+1] = y0;
    
    for (int i = 0; i<12; i++){
        cout<<hex<<"state["<<i<<"] = "<<state[i]<<endl;
    }

    return 0;
}*/







/*#include <iostream>   TEST SPARKLE

using namespace std;
#define ROT(x, n) (((x) >> (n)) | ((x) << (32-(n))))
#define ELL(x) (ROT(((x) ^ ((x) << 16)), 16))
typedef unsigned char UChar;
typedef unsigned long long int ULLInt;
// Round constants
static const uint32_t RCON[8] = {      \
  0xB7E15162, 0xBF715880, 0x38B4DA56, 0x324E7738, \
  0xBB1185EB, 0x4F7C7B57, 0xCFBFA1C8, 0xC2B3293D  \
};

#define STATE_BYTES     48
#define RATE_BYTES      16
#define STEPS_SLIM       7
#define STEPS_BIG       11


#define ESCH_DIGEST_WORDS (ESCH_DIGEST_BYTES/4)

#define STATE_BRANS (STATE_BYTES/8) //6
#define STATE_WORDS (STATE_BYTES/4) //12
#define RATE_BRANS  (RATE_BYTES/8)
#define RATE_WORDS  (RATE_BYTES/4) //4
#define CAP_BYTES   (STATE_BYTES-RATE_BYTES)
#define CAP_BRANS   (CAP_BYTES/8)
#define CAP_WORDS   (CAP_BYTES/4)

int main()
{
    
    uint32_t state[12] = {0};
    state[0] =	0x7f37fb80;
	state[1] =  0xbba211f6;
	state[2] =  0xbae335e6;
	state[3] =  0xe5211611;
	state[4] =  0x2da96eb2;
	state[5] =  0x332f0211;
	state[6] =  0x08cf9556;
	state[7] =  0x0f814c25;
	state[8] = 0x6eb22da9;
    state[9] = 0x0211332f;
    state[10] = 0xb8ca01f8;
    state[11] = 0xe33e8248;
	
    int brans = 6;
   int i, j;  // Step and branch counter
   uint32_t rc, tmpx, tmpy, x0, y0;
  
  for(i = 0; i < 7; i ++) {
    // Add round constant
    state[1] ^= RCON[i%8];
    state[3] ^= i;
    // ARXBOX layer
    for(j = 0; j < 2*brans; j += 2) {
      rc = RCON[j>>1];
      state[j] += ROT(state[j+1], 31);
      state[j+1] ^= ROT(state[j], 24);
      state[j] ^= rc;
      state[j] += ROT(state[j+1], 17);
      state[j+1] ^= ROT(state[j], 17);
      state[j] ^= rc;
      state[j] += state[j+1];
      state[j+1] ^= ROT(state[j], 31);
      state[j] ^= rc;
      state[j] += ROT(state[j+1], 24);
      state[j+1] ^= ROT(state[j], 16);
      state[j] ^= rc;
    }
    // Linear layer
    tmpx = x0 = state[0];
    tmpy = y0 = state[1];
    for(j = 2; j < brans; j += 2) {
      tmpx ^= state[j];
      tmpy ^= state[j+1];
    }
    tmpx = ELL(tmpx);
    tmpy = ELL(tmpy);
    for (j = 2; j < brans; j += 2) {
      state[j-2] = state[j+brans] ^ state[j] ^ tmpy;
      state[j+brans] = state[j];
      state[j-1] = state[j+brans+1] ^ state[j+1] ^ tmpx;
      state[j+brans+1] = state[j+1];
    }
    state[brans-2] = state[brans] ^ x0 ^ tmpy;
    state[brans] = x0;
    state[brans-1] = state[brans+1] ^ y0 ^ tmpx;
    state[brans+1] = y0;
  }
  for (int i = 0; i<12; i++){
        cout<<hex<<"state["<<i<<"] = "<<state[i]<<endl;
    }

    return 0;
}*/