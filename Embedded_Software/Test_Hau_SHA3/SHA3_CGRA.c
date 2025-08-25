#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

void Padding(uint8_t input[], int *in_len, int *absorb_times){
	*absorb_times = *in_len / 72;
    int add_last = *in_len % 72;
    if (72 - add_last == 1) {
        input[*in_len] = 0x86;
    }
    else {
        input[*in_len] = 0x06;
        for (int i = 1; i < (71 - add_last); i++){         
            input[*in_len + i] = 0x00;      
            // printf("%02x", input[*in_len + i]);
        }   
        input[*in_len + (71 - add_last)] = 0x80; 
        *in_len = *in_len + (72 - add_last);
    }

    *absorb_times += 1;
}

void assign_S_xor_R(){
	U32 OP;
	U64 OP_Immidiate;
	U64 x = 0;
	mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]);  // R[0]		R[1]
	mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]);	// R[2]		R[3]
	mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]);  // R[4] 	R[5]
	mop64(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); // R[6]		R[7]


	// /*** row 1 ***///
	OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
	exe64(OP, x, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[0][1],PE_out[0][4],PE_out[0][5], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X	S[1][0]		S[0][0] 	X		
	exe64(OP, x, PE_out[5],/*ALU in*/ PE_out[1][0],PE_out[1][1],PE_out[1][4],PE_out[1][5], /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // X	S[3][0]		S[2][0] 	X	
	exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[2][0],PE_out[2][1],PE_out[2][4],PE_out[2][5], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); // X	S[0][1]		S[4][0] 	X
	mop64(OP_LDW, 0, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][0],PE_out[3][1], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);  // 	R[8] // SAVE R[6]   R[7] TO BUFFER  [6] [7]
	

	/*** row 2 ***///
	exe64(OP, x, PE_out[8],/*ALU in*/ PE_out[7][6],PE_out[7][7],PE_out[4][6],PE_out[4][7],  /*BUFF 16to1 in*/PE_out[4][2],PE_out[4][1],PE_out[4][6],PE_out[4][7],/*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); // UPDATE S[1][0] S[0][0] // X	S[2][1]		S[1][1]		X
	mop64(OP_LDW, 0, PE_out[9],/*ALU in*/ PE_out[7][0],0,PE_out[5][7],0, /*BUFF 16to1 in*/ PE_out[5][2],PE_out[5][1],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // UPDATE S[3][0] S[2][0]
	mop64(OP_LDW, 0, PE_out[10], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][2],PE_out[6][1],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); // UPDATE S[0][1] S[4][0]
	exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[7][0],0,PE_out[7][4],0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); // X	X	S[3][1] 	X		

	/*** row 3 ***///
	exe64(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][2],PE_out[8][1], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); // UPDATE	S[2][1]		S[1][1]	
	mop64(OP_LDW, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][4], PE_out[9][5],PE_out[9][6], PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], PE_out[9][11]);
	mop64(OP_LDW, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4], PE_out[10][5],PE_out[10][6], PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
	mop64(OP_LDW, 0, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][2], PE_out[11][5], PE_out[11][6], PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]); //UPDATE S[3][1]
}


void round_ne(uint64_t t){
	U32 OP;
	U64 OP_Immidiate;
	U64 x = 0;
	for (int i = 1; i < 8; i++){
	 if (i == 1) { //THETA STEP
			/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	X	X	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	X	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	X	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	X	X	X	S[3][1]		X			X			X			X			X			X			X
		*/
	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);		
			exe64(OP, x, PE_out[0],/*ALU in*/ PE_out[12][4],PE_out[14][5],PE_out[12][8],PE_out[12][9], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //X		S[0][0^1^2^3]		X		X
			exe64(OP, x, PE_out[1],/*ALU in*/ PE_out[12][5],PE_out[12][6],PE_out[12][11],PE_out[13][8], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //X		S[1][0^1^2^3]		X		X
			exe64(OP, x, PE_out[2],/*ALU in*/ PE_out[13][4],PE_out[12][7],PE_out[13][10],PE_out[13][11], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); //X		S[2][0^1^2^3]		X		X
			exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[13][5],PE_out[15][4],PE_out[14][9],PE_out[14][6], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); //X		S[3][0^1^2^3]		X		X

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[4],/*ALU in*/ PE_out[2][1],PE_out[1][1],PE_out[2][8],PE_out[1][9], /*BUFF 16to1 in*/ PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X		C[1]	C[2]	X
			exe64(OP, x, PE_out[5],/*ALU in*/ PE_out[0][1],PE_out[3][1],PE_out[0][10],PE_out[2][7], /*BUFF 16to1 in*/ PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); //	X	C[3]	C[0]	X
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[2][4],PE_out[1][7],PE_out[2][10],PE_out[2][11], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); //X		S[4][0^1^2^3]		X		X
			exe64(OP, x, PE_out[7],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][4]);

			/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	X	X	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	X	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	X	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	X	X	X	S[3][1]		X			X			X			X			X			X			S[3][1]
		*/
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/1,/*LU3*/OP_XOR);
			exe64(OP, x, PE_out[8],/*ALU in*/ PE_out[5][2],PE_out[4][2],0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //	X	D[1]	X	X
			exe64(OP, x, PE_out[9],/*ALU in*/ PE_out[4][1],PE_out[5][1],0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // X		D[2]	X	X
			exe64(OP, x, PE_out[10],/*ALU in*/ PE_out[5][1],PE_out[5][2],0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); //X	D[4]	X	X
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/1,/*LU3*/OP_XOR);
			exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[6][1],PE_out[4][1],PE_out[5][6],0, /*BUFF 16to1 in*/ PE_out[4][1],PE_out[4][2],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); // X		D[0]	C[4]	X   // c[1] AND C[2] SAVE IN BUFFER 16 [4] [5]
		
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/1,/*LU3*/OP_XOR);
			exe64(OP, x, PE_out[12],/*ALU in*/ PE_out[11][5],PE_out[11][2],0,0, /*BUFF 16to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); // X		D[3]	X		X
			exe64(OP, x, PE_out[13],/*ALU in*/ 0, 0, 0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]);  
			exe64(OP, x, PE_out[14],/*ALU in*/ 0, 0, 0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[15],/*ALU in*/ PE_out[9][1], PE_out[10][1], 0,0, /*BUFF 16to1 in*/ PE_out[11][1],PE_out[8][1],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  //	X	D[4]	D[2]	X	// SAVE D[0] IN [4] AND D[1] IN [5]
                                                                                                                                    //D[0]                 D[1]
}

else if (i == 2) {
			/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	D[3]  X	    X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	X	  X		X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	X	  X		X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X  D[4]  D[2]	X	D[0]		D[1]			X			X			X			X			X		S[3][1]
		*/

	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);		
			exe64(OP, x, PE_out[0],/*ALU in*/ PE_out[15][4],PE_out[15][5],PE_out[12][4],PE_out[12][5], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //	X	S[1][0]		S[0][0]		X
			exe64(OP, x, PE_out[1],/*ALU in*/ PE_out[15][2],PE_out[12][1],PE_out[13][4],PE_out[13][5], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // x		S[3][0]		S[2][0]		x
			exe64(OP, x, PE_out[2],/*ALU in*/ PE_out[15][1],PE_out[15][4],PE_out[14][4],PE_out[14][5], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // x		S[0][1]		S[4][0]		x
			exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[12][1],PE_out[12][1],PE_out[15][11],PE_out[14][6], /*BUFF 16to1 in*/ PE_out[15][1],PE_out[15][2],PE_out[12][1],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][4],PE_out[15][5],PE_out[15][11]);// x		S[3][3]		S[3][1]		x
	                                                                                                                                                                                              //D[4]                     D[2]                       D[3]                                                                                                 // D[0]                    //D[1]             
 					/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	S10   S00   X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	S30	  S20	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	S01	  S40	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X   S33   S31	X	D[4]		D[2]		D[3]			X			X		D[0]		D[1]		S[3][1]
		*/
                                                
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_XOR,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[4],/*ALU in*/ PE_out[3][10],PE_out[3][5],PE_out[0][6],PE_out[0][7], /*BUFF 16to1 in*/ PE_out[0][2],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X	S[2][1]		S[1][1]		X
			exe64(OP, x, PE_out[5],/*ALU in*/ PE_out[3][4],PE_out[3][4],PE_out[1][6],PE_out[1][7], /*BUFF 16to1 in*/ PE_out[1][2],PE_out[1][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // X	S[4][1]		S[4][4]		X
			exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[3][5],PE_out[3][6],PE_out[2][8],PE_out[2][7], /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][1],PE_out[3][1],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); //	X	S[3][4]		S[2][4]		X
			exe64(OP, x, PE_out[7],/*ALU in*/ PE_out[3][6],PE_out[3][4],PE_out[2][9],PE_out[2][10], /*BUFF 16to1 in*/ PE_out[3][2],0,PE_out[3][4],PE_out[3][5], /*BUFF 8to1 in*/ PE_out[3][6],PE_out[3][9],PE_out[3][10],PE_out[3][11]); // X	S[4][2]		S[3][2]		X

 					/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	S21   S11   X	S00		S10		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	S41	  S44	X	S20		S30		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	S34	  S24	X 	S40		S01		S33			S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X   S42   S32	X	S31				D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/

			// OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_ROL,/*SRU2_IM*/1,/*LU3*/OP_XOR);
			// exe64(OP, x, PE_out[8],/*ALU in*/ PE_out[7][9],PE_out[7][9],PE_out[4][8],PE_out[4][9], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][2],PE_out[4][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //X		S[0][3] 	S[0][2]		X
			// exe64(OP, x, PE_out[9],/*ALU in*/ PE_out[7][10],PE_out[7][10],PE_out[5][8],PE_out[5][9], /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][2],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // X		S[1][4]		S[1][3]		X
			// exe64(OP, x, PE_out[10],/*ALU in*/ PE_out[7][4],0,PE_out[6][11],0, /*BUFF 16to1 in*/ PE_out[6][2],PE_out[7][2],PE_out[7][1],PE_out[6][1], /*BUFF 8to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][11]); //X	X	S[4][3]		X
			// exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[7][9],PE_out[7][10],PE_out[4][10],PE_out[4][11], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); // X	S[1][2]		S[0][4]		X
		
			exe64(OP, x, PE_out[8],/*ALU in*/PE_out[7][9],PE_out[7][9],PE_out[4][8],PE_out[4][9],  /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][2],PE_out[4][1], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //X		S[0][3] 	S[0][2]		X
			exe64(OP, x, PE_out[9],/*ALU in*/PE_out[7][9],PE_out[7][10],PE_out[4][10],PE_out[4][11],  /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][2],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // X		S[1][2]		S[0][4]		X
			exe64(OP, x, PE_out[10],/*ALU in*/PE_out[7][10],PE_out[7][10],PE_out[5][8],PE_out[5][9], /*BUFF 16to1 in*/ PE_out[6][2],PE_out[7][2],PE_out[7][1],PE_out[6][1], /*BUFF 8to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][11]); //X	S[1][4]		S[1][3]		X
			exe64(OP, x, PE_out[11],/*ALU in*/PE_out[7][6],0,PE_out[6][11],0,  /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); // X		X		S[4][3]		X

		
		/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	S03   S02   X	S00		S10		S11		S21		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	S12	  S04	X	S20		S30		S44 	S41		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	S14	  S13	X 	S24		S32		S42		S34		S40			S01			S33			S[4][3]
		PE[3] = X   X     S43	X	S31		X		D[4]	D[2]	D[3]		D[0]		D[1]		S[3][1]
		*/
			exe64(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][2],PE_out[8][1],PE_out[9][2],PE_out[9][1], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); //
			exe64(OP, x, PE_out[13],/*ALU in*/ PE_out[9][10], PE_out[9][11], PE_out[11][7],PE_out[11][7], /*BUFF 16to1 in*/ PE_out[10][2],PE_out[10][1],PE_out[9][10],PE_out[9][11], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]);  // X		S[2][3]		S[2][2]		X
			exe64(OP, x, PE_out[14],/*ALU in*/ 0, 0, 0,0, /*BUFF 16to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[11][2], /*BUFF 8to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7]); // 
			exe64(OP, x, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  //A
		/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	X     X     X	S02		S03		S04			S12			S00			S10			S11			S21
		PE[1] = X 	S23	  S22	X	S13		S14		S[2][2] 	S[2][3]		S20			S30			S44			S41
		PE[2] = X	X	  X		X 	S40		S01		S33			S43			S24			S32			S42			S34			
		PE[3] = X   X     X		X	S31		X		D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/
	//-> DONE THETA STEP
}
else if (i == 3) {
		/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	X     X     X	S02		S03		S04			S12			S00			S10			S11			S21
		PE[1] = X 	S23	  S22	X	S13		S14		S[2][2] 	S[2][3]		S20			S30			S44			S41
		PE[2] = X	X	  X		X 	S40		S01		S33			S43			S24			S32			S42			S34			
		PE[3] = X   X     X		X	S31		X		D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/

		//mop64(OP_STW, 0, 0, LMM[0], PE_out[0],/*ALU in*/ 1,2,3,4, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);  // 
		//mop64(OP_LDW, 0, 0, LMM[0], PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);  // 
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/3,/*SRU2*/OP_ROL,/*SRU2_IM*/41,/*LU3*/OP_NOP);		
			exe64(OP, x, PE_out[0],/*ALU in*/ PE_out[12][4],PE_out[12][5],0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //	X	B[3][4]		B[2][1]		X
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/45,/*SRU2*/OP_ROL,/*SRU2_IM*/2,/*LU3*/OP_NOP);	
			exe64(OP, x, PE_out[1],/*ALU in*/ PE_out[13][4],PE_out[13][5],0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][2],PE_out[13][1], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); // 	x	B[4][4]		B[3][1]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/27,/*SRU2*/OP_ROL,/*SRU2_IM*/36,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[2],/*ALU in*/ PE_out[14][4],PE_out[14][5],0,0, /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); // 	x	B[1][3]		B[0][3]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/21,/*SRU2*/OP_ROL,/*SRU2_IM*/8,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[14][6],PE_out[14][7],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]);// 	x	B[3][2]		B[3][0]		x
		/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	B[3][4]		B[2][1]		X		S02		S03		S04			S12			S00			S10			S11			S21
		PE[1] = x	B[4][4]		B[3][1]		x		S13		S14		S22 		S23			S20			S30			S44			S41
		PE[2] = x	B[1][3]		B[0][3]		x 		S40		S01		S33			S43			S24			S32			S42			S34			
		PE[3] = x	B[3][2]		B[3][0]		x		S31		X		D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/	                                                                                                                                                                                                  
			

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/18,/*SRU2*/OP_ROL,/*SRU2_IM*/10,/*LU3*/OP_NOP);		
			exe64(OP, x, PE_out[4],/*ALU in*/ PE_out[0][6],PE_out[0][7],0,0, /*BUFF 16to1 in*/ PE_out[0][2],PE_out[0][1],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); //	X	B[2][3]		B[4][2]		X
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/43,/*SRU2*/OP_ROL,/*SRU2_IM*/15,/*LU3*/OP_NOP);	
			exe64(OP, x, PE_out[5],/*ALU in*/ PE_out[1][6],PE_out[1][7],0,0, /*BUFF 16to1 in*/ PE_out[1][2],PE_out[1][1],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); // x		B[3][3]		B[2][0]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/61,/*SRU2*/OP_ROL,/*SRU2_IM*/25,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[2][8],PE_out[2][9],0,0, /*BUFF 16to1 in*/ PE_out[2][2],PE_out[2][1],PE_out[3][2],PE_out[3][1], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); // x		B[2][2]		B[4][1]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/39,/*SRU2*/OP_ROL,/*SRU2_IM*/56,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[7],/*ALU in*/ PE_out[2][10],PE_out[2][11],0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]);// x		B[4][3]		B[2][4]		x

		/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	B[2][3]     B[4][2]     X		B21		B34		S04			S12			S00			S10			S11			S21
		PE[1] = X 	B[3][3]	  	B[2][0]		X		B31		B44		S22 		S23			S20			S30			S44			S41
		PE[2] = X	B[2][2]		B[4][1]	  	X 		B03		B13		B30			B32			S24			S32			S42			S34			
		PE[3] = X   B[4][3]     B[2][4]		X		S31		X		D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/	

			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/0,/*SRU2*/OP_ROL,/*SRU2_IM*/1,/*LU3*/OP_NOP);		
			exe64(OP, x, PE_out[8],/*ALU in*/ PE_out[4][8],PE_out[4][9],0,0, /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][1],PE_out[4][2], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //	X	B[0][2]		B[0][0]		X
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/62,/*SRU2*/OP_ROL,/*SRU2_IM*/28,/*LU3*/OP_NOP);	
			exe64(OP, x, PE_out[9],/*ALU in*/ PE_out[5][8],PE_out[5][9],0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][1],PE_out[5][2], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // x		B[3][1]		B[0][4]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/14,/*SRU2*/OP_ROL,/*SRU2_IM*/20,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[10],/*ALU in*/ PE_out[5][10],PE_out[5][11],0,0, /*BUFF 16to1 in*/ PE_out[6][1],PE_out[6][2],PE_out[7][1],PE_out[7][2], /*BUFF 8to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7]); // x		B[1][1]		B[4][0]		x
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/55,/*SRU2*/OP_ROL,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[7][4],0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]);// x		X		B[1][4]		x

					/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	B[0][2]     B[0][0]     X		B21		B34		B23			B42			S00			S10			S11			S21
		PE[1] = X 	B[3][1]	  	B[0][4]		X		B31		B44		B33 		B20			S20			S30			S44			S41
		PE[2] = X	B[1][1]		B[4][0]	  	X 		B22		B41		B43			B24			B03			B13			B30			B32					
		PE[3] = X      X	    B[1][4]		X		S31		X		D[4]		D[2]		D[3]		D[0]		D[1]		S[3][1]
		*/	
			OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/44,/*SRU2*/OP_ROL,/*SRU2_IM*/6,/*LU3*/OP_NOP);
			exe64(OP, x, PE_out[12],/*ALU in*/ PE_out[8][10],PE_out[8][11],0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][2],PE_out[8][10],PE_out[8][11], /*BUFF 8to1 in*/ PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7]); 
			exe64(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][2],PE_out[10][1],PE_out[10][2], /*BUFF 8to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7]); 
			exe64(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); 
			exe64(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][2],0,0,0, /*BUFF 8to1 in*/ 0,0,0,0);
							/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	B[1][2]     B[1][0]     X		B02		B00		S11			S21		B21		B34		B23			B42			
		PE[1] = X 		X	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	X	   		X		X		B14		0		0			0		0		0		0			0
		*/	

		//-> DONE RHO & PI

}
else if (i == 4) {

						/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X	B[1][2]     B[1][0]     X		B02		B00		S11			S21		B21		B34		B23			B42			
		PE[1] = X 		X	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	X	   		X		X		B14		0		0			0		0		0		0			0


		->AFTER MUST BE:
		PE[0] = X	X	X	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	X	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	X	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	X	X	X	S[3][1]		X			X			X			X			X			X			X

		*/	
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
	//	exe64(OP, x, PE_out[0],/*ALU in*/ B,~,0,0x0609, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); 
		exe64(OP, x, PE_out[0],/*ALU in*/ PE_out[12][1],PE_out[14][4],0,PE_out[14][11], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][1],PE_out[12][2], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //X		S[1][2]		X		X
		exe64(OP, x, PE_out[1],/*ALU in*/ PE_out[13][5],PE_out[15][4],0,PE_out[14][7], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //X		S[0][4]		X		X
		exe64(OP, x, PE_out[2],/*ALU in*/ PE_out[14][8],PE_out[14][9],0,PE_out[12][10], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); //X		S[0][3]		X		X
		exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[12][4],PE_out[12][1],0,PE_out[14][4], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); //X		S[0][2]		X		X

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		S12     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		S04	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		S03			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S02	   		X		X		B14		0		0			0		0		0		0			0
		*/

		mop64(OP_STW, 0, PE_out[4],/*ALU in*/ PE_out[0][1],PE_out[1][1],0,0, /*BUFF 16to1 in*/  PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); //LMM[4] =  S12		S04
		mop64(OP_STW, 0, PE_out[5],/*ALU in*/ PE_out[2][1],PE_out[3][1],0,0, /*BUFF 16to1 in*/  PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); //LMM[5] =  S03		S02
		exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[0][8],PE_out[1][8],0,PE_out[2][5], /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); //X		S[2][1]		X		X
		exe64(OP, x, PE_out[7],/*ALU in*/ PE_out[1][6],PE_out[0][8],0,PE_out[1][8], /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]); //X		S[1][1]		X		X
		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		x	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		x	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		S21			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S11	   		X		X		B14		0		0			0		0		0		0			0
		*/

		mop64(OP_STW, 0, PE_out[8],/*ALU in*/ PE_out[6][1],PE_out[7][1],0,0, /*BUFF 16to1 in*/  PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //LMM[8] =  S21		S11
		exe64(OP, x, PE_out[9],/*ALU in*/ PE_out[4][7],PE_out[5][11],0,PE_out[6][10], /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); //X		S[1][0]		X		X
		exe64(OP, x, PE_out[10],/*ALU in*/ PE_out[4][5],PE_out[4][7],0,PE_out[5][11], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); //X		S[0][0]		X		X
		exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[4][10],PE_out[5][10],0,PE_out[6][6], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); //X		S[2][3]		X		X

		mop64(OP_STW, 0, PE_out[12],/*ALU in*/ PE_out[9][1],PE_out[10][1],PE_out[11][1],0, /*BUFF 16to1 in*/  PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); //LMM[12] =  S10		S00	//	X		X		S[2][3]		X
		exe64(OP, x, PE_out[13],/*ALU in*/ PE_out[10][4],PE_out[10][11],0,PE_out[8][11], /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); //X		S[2][2]		X		X
		exe64(OP, x, PE_out[14],/*ALU in*/ PE_out[7][4],PE_out[6][7],0,PE_out[8][9], /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); //X		S[1][4]		X		X
		exe64(OP, x, PE_out[15],/*ALU in*/ PE_out[10][9],PE_out[8][10],0,PE_out[9][10], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); //X		S[1][3]		X		X
}
else if (i == 5) {

						/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		x	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		x	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		S21			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S11	   		X		X		B14		0		0			0		0		0		0			0


		->AFTER MUST BE:
		PE[0] = X	X	S23	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	S22	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	S14	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	S13	X	X	S[3][1]		X			X			X			X			X			X			X

		*/	
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
	//	exe64(OP, x, PE_out[0],/*ALU in*/ B,~,0,0x0609, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); 
		mop64(OP_STW, 0, PE_out[0],/*ALU in*/ PE_out[12][2],PE_out[13][1],0,0, /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //LMM[0] = S23	S22
		mop64(OP_STW, 0, PE_out[1],/*ALU in*/ PE_out[14][1],PE_out[15][1],0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //LMM[1] = S14	S13
		exe64(OP, x, PE_out[2],/*ALU in*/ PE_out[14][5],PE_out[13][4],0,PE_out[13][6], /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); //X		S[4][1]		X		X		
		exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[13][9],PE_out[13][5],0,PE_out[15][4], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); //X		S[4][4]		X		X

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		X     		X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		X	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		S41			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S44	   		X		X		B14		0		0			0		0		0		0			0

		NO LMM[4][5][8][12]
		*/

		exe64(OP, x, PE_out[4],/*ALU in*/ PE_out[2][10],PE_out[1][7],0,PE_out[0][5],/*BUFF 16to1 in*/  PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]); // X		S[3][0]		X		X
		exe64(OP, x, PE_out[5],/*ALU in*/ PE_out[1][11],PE_out[2][10],0,PE_out[1][7], /*BUFF 16to1 in*/  PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); //X		S[2][0]		X		X
		mop64(OP_STW, 0, PE_out[6],/*ALU in*/ PE_out[2][1],PE_out[3][1],0,0, /*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); //	LMM[6] = S41	S44
		exe64(OP, x, PE_out[7],/*ALU in*/ PE_out[2][6],PE_out[2][8],0,PE_out[2][9], /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]); //X		S[4][3]		X		X
		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		S30	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		S20	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S43	   		X		X		B14		0		0			0		0		0		0			0
		*/

		exe64(OP, x, PE_out[8],/*ALU in*/ PE_out[5][10],PE_out[6][6],0,PE_out[6][8], /*BUFF 16to1 in*/  PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //X		S[3][3]		X		X
		mop64(OP_STW, 0, PE_out[9],/*ALU in*/ PE_out[4][1],PE_out[5][1],PE_out[7][1],0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // LMM[9] = S30	S20 //	X	X	S43		X
		exe64(OP, x, PE_out[10],/*ALU in*/ PE_out[5][4],PE_out[5][6],0,PE_out[4][8], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][7], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); //X		S[0][1]		X		X
		exe64(OP, x, PE_out[11],/*ALU in*/ PE_out[5][7],PE_out[4][5],0,PE_out[4][7], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); //X		S[4][0]		X		X


		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		S33	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		X	  		S43		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		S01			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S40	   		X		X		B14		0		0			0		0		0		0			0
		

			->AFTER MUST BE:
		PE[0] = X	X	X	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	X	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	X	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	X	X	X	S[3][1]		X			X			X			X			X			X			X

		*/	

		exe64(OP, x, PE_out[12],/*ALU in*/ PE_out[9][8],PE_out[10][5],0,PE_out[9][4], /*BUFF 16to1 in*/  PE_out[8][4],PE_out[8][5],PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); //	X		S[3][1]		X		X
		mop64(OP_STW, 0, PE_out[13],/*ALU in*/ PE_out[8][1],PE_out[9][2],0,0, /*BUFF 16to1 in*/ PE_out[9][4],PE_out[9][5],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); // LMM[13] = S33	S43
		mop64(OP_STW, 0, PE_out[14],/*ALU in*/ PE_out[10][1],PE_out[11][1],0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); //	LMM[14] = S01	S40
		exe64(OP, x, PE_out[15],/*ALU in*/ PE_out[8][11],PE_out[8][4],0,PE_out[8][6], /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); //X		S[4][2]		X		X

		//USED LMM[4][5][8][12][0][1][6][9][13][14]
}

else if (i == 6) {



						/*
		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		S31	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		x	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S42	   		X		X		B14		0		0			0		0		0		0			0


		->AFTER MUST BE:
		PE[0] = X	X	S23	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	S22	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	S14	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	S13	X	X	S[3][1]		X			X			X			X			X			X			X

		*/	
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
	//	exe64(OP, x, PE_out[0],/*ALU in*/ B,~,0,0x0609, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); 
		exe64(OP, x, PE_out[0],/*ALU in*/ PE_out[14][11],PE_out[12][11],0,PE_out[12][4], /*BUFF 16to1 in*/ PE_out[12][4],PE_out[12][5],PE_out[12][1],PE_out[12][2], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //X		S[3][2]		X		X
		exe64(OP, x, PE_out[1],/*ALU in*/ PE_out[14][7],PE_out[12][9],0,PE_out[13][9], /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //X		S[2][4]		X		X
		mop64(OP_STW, 0, PE_out[2],/*ALU in*/ PE_out[12][1],PE_out[15][1],0,0,  /*BUFF 16to1 in*/ PE_out[14][4],PE_out[14][5],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); //	LMM[2] = S[3][1]	S[4][2]
		exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[12][9],PE_out[13][9],0,PE_out[13][5], /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); //X		S[3][4]		X		X

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		S32	     	X     	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		S24	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			X	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	S34	   		X		X		B14		0		0			0		0		0		0			0
		*/
		
		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
		exe64(OP, x, PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8],PE_out[0][9],PE_out[0][10],PE_out[0][11]);  //	X		X		S34		X
		exe64(OP, x, PE_out[5],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[1][4],PE_out[1][5],PE_out[1][6],PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8],PE_out[1][9],PE_out[1][10],PE_out[1][11]); 
		exe64(OP, x, PE_out[6],/*ALU in*/ PE_out[3][1],0,0,0,/*BUFF 16to1 in*/ PE_out[2][4],PE_out[2][5],PE_out[2][6],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8],PE_out[2][9],PE_out[2][10],PE_out[2][11]); 
		mop64(OP_STW, 0, PE_out[7],/*ALU in*/ PE_out[0][1],PE_out[1][1],0,0, /*BUFF 16to1 in*/ PE_out[3][4],PE_out[3][5],PE_out[3][6],PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8],PE_out[3][9],PE_out[3][10],PE_out[3][11]); //LMM[7] = S[3][2]	S[2][4]
				// -> DONE CHI
		
			// printf("LMM[4]	=  S12		S04 = %16lx		%16lx\n", LMM[4][0][0], LMM[4][1][0] );
			// printf("LMM[5]	=  S03		S02 = %16lx		%16lx\n", LMM[5][0][0], LMM[5][1][0] );
			// printf("LMM[8]	=  S21		S11 = %16lx		%16lx\n", LMM[8][0][0], LMM[8][1][0] );
			// printf("LMM[12]   =  S10		S00 = %16lx		%16lx\n", LMM[12][0][0], LMM[12][1][0] );
			// printf("LMM[0] 	=  S23	    S22 = %16lx		%16lx\n", LMM[0][0][0], LMM[0][1][0] );
			// printf("LMM[1] 	=  S14	    S13 = %16lx		%16lx\n", LMM[1][0][0], LMM[1][1][0] );
			// printf("LMM[6] 	=  S41	    S44 = %16lx		%16lx\n", LMM[6][0][0], LMM[6][1][0] );
			// printf("LMM[9] 	=  S30	    S20 = %16lx		%16lx\n", LMM[9][0][0], LMM[9][1][0] );
			// printf("LMM[13]   =  S33	    S43 = %16lx		%16lx\n", LMM[13][0][0], LMM[13][1][0] );
			// printf("LMM[14]   =  S01	    S40 = %16lx		%16lx\n", LMM[14][0][0], LMM[14][1][0] );
			// printf("LMM[2]	=  S31	    S42 = %16lx		%16lx\n", LMM[2][0][0], LMM[2][1][0] );
			// printf("LMM[7]	=  S32	    S24 = %16lx		%16lx\n", LMM[7][0][0], LMM[7][1][0] );

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = X		X	     	x    	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = X 		X	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			S34	  	X 		B22		B41		B43			B24		B03		B13		B30			B32					
		PE[3] = X      	X	   		X		X		B14		0		0			0		0		0		0			0

				->AFTER MUST BE:
		PE[0] = X	X	x	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	x	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	x	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	x	X	X	S[3][1]		X			X			X			X			X			X			X
		*/

		mop64(OP_LDW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7], /*BUFF 8to1 in*/ PE_out[4][8],PE_out[4][9],PE_out[4][10],PE_out[4][11]); //LMM[8] =  S21		S11
		mop64(OP_LDW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7], /*BUFF 8to1 in*/ PE_out[5][8],PE_out[5][9],PE_out[5][10],PE_out[5][11]); // LMM[9] = S30	S20 
		exe64(OP, x, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5],PE_out[6][6],PE_out[6][2], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][10],PE_out[6][11]); 
		exe64(OP, x, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = S21		S11	     	x    	X		B02		B00		B12			B10		B21		B34		B23			B42			
		PE[1] = S30 	S20	  		X		X		B01		B04		B11			B40		B31		B44		B33 		B20	
		PE[2] = X		X			x	  	X 		B22		B41		B43			S34		B03		B13		B30			B32					
		PE[3] = X      	X	   		X		X		B14		0		0			0		0		0		0			0
		*/

		mop64(OP_LDW, 0, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[8][4],PE_out[8][5],PE_out[8][1],PE_out[8][0], /*BUFF 8to1 in*/ PE_out[8][8],PE_out[8][9],PE_out[8][10],PE_out[8][11]); //LMM[12] =  S10		S00	
		mop64(OP_LDW, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[9][1],PE_out[9][0],PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][10],PE_out[9][11]); // LMM[13] = S33	S43
		mop64(OP_LDW, 0, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][10],PE_out[10][11]); //	LMM[14] = S01	S40
		exe64(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]); 

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = S10		S00	     	x    	X		B02		B00		S11			S21		B21		B34		B23			B42			
		PE[1] = S33 	S43	  		X		X		S20		S30		B11			B40		B31		B44		B33 		B20	
		PE[2] = S01		S40			x	  	X 		B22		B41		B43			S34		B03		B13		B30			B32					
		PE[3] = X      	X	   		X		X		B14		0		0			0		0		0		0			0
		*/	
}
else if (i==7){
		mop64(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[12][1],PE_out[12][0],PE_out[12][6],PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8],PE_out[12][9],PE_out[12][10],PE_out[12][11]); //LMM[0] = S23	S22
		mop64(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[13][4],PE_out[13][5],PE_out[13][6],PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8],PE_out[13][9],PE_out[13][10],PE_out[13][11]); //LMM[1] = S14	S13
		mop64(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0,  /*BUFF 16to1 in*/ PE_out[14][1],PE_out[14][0],PE_out[14][6],PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8],PE_out[14][9],PE_out[14][10],PE_out[14][11]); //	LMM[2] = S[3][1]	S[4][2]
		exe64(OP, x, PE_out[3],/*ALU in*/ PE_out[13][0],PE_out[13][1],0,0, /*BUFF 16to1 in*/ PE_out[15][4],PE_out[15][5],PE_out[15][6],PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8],PE_out[15][9],PE_out[15][10],PE_out[15][11]); //X		S43		S33		X

		/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = S23		S22	     	x    	X		S00		S10		S11			S21		B21		B34		B23			B42			
		PE[1] = S14 	S13	  		X		X		S20		S30		B11			B40		B31		B44		B33 		B20	
		PE[2] = S31		S42			x	  	X 		S40		S01		B43			S34		B03		B13		B30			B32					
		PE[3] = X      	S43	   		S33		X		B14		0		0			0		0		0		0			0

			->AFTER MUST BE:
		PE[0] = X	X	x	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	x	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	x	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	x	X	X	S[3][1]		X			X			X			X			X			X			X
		*/
		mop64(OP_LDW, 0,PE_out[4],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/  PE_out[0][4],PE_out[0][5],PE_out[0][6],PE_out[0][7], /*BUFF 8to1 in*/ 0,0,0,0); //LMM[4] =  S12		S04
		mop64(OP_LDW, 0,PE_out[5],/*ALU in*/ 0,0,PE_out[0][0],PE_out[0][1], /*BUFF 16to1 in*/  PE_out[1][4],PE_out[1][5],PE_out[1][1],PE_out[1][0], /*BUFF 8to1 in*/ 0,0,0,0); //LMM[5] =  S03		S02    S23
		mop64(OP_LDW, 0,PE_out[6],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[3][1],PE_out[3][2],PE_out[2][1],PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][4],PE_out[2][5],0,PE_out[2][7]); //	LMM[6] = S41	S44
		mop64(OP_LDW, 0,PE_out[7],/*ALU in*/ 0,0,PE_out[2][0],0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); //LMM[7] = S[3][2]	S[2][4]		S31

				/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = S12		S04	     	x    	X		S00		S10		S11			S21		0		0		0			0			
		PE[1] = S03 	S02	  		S23		S22		S20		S30		S13			S14		0		0		0 			0	
		PE[2] = S41		S44			x	  	X 		S43		S33		S42			S34		S40		S01		0			S34					
		PE[3] = S32     S24	   		S31		X		0		0		0			0		0		0		0			0 */

		OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);

		exe64(OP, t, PE_out[8],/*ALU in*/ 0,0,0,PE_out[4][4], /*BUFF 16to1 in*/  PE_out[4][0],PE_out[4][1],PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ PE_out[4][4],PE_out[4][5],PE_out[4][6],PE_out[4][7]); // X		S[0][0] 	X		X
		exe64(OP, x, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2],PE_out[6][0],PE_out[6][1],PE_out[5][3], /*BUFF 8to1 in*/ PE_out[5][4],PE_out[5][5],PE_out[5][6],PE_out[5][7]); 
		exe64(OP, x, PE_out[10],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][0],PE_out[7][1],PE_out[6][6],PE_out[6][5], /*BUFF 8to1 in*/ PE_out[6][8],PE_out[6][9],PE_out[6][4],PE_out[6][11]); 
		exe64(OP, x, PE_out[11],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[7][2],PE_out[7][5],PE_out[7][6],PE_out[7][7], /*BUFF 8to1 in*/ PE_out[7][8],PE_out[7][9],PE_out[7][10],PE_out[7][11]); 

						/*		-> AFTER EACH ROUND OF SHA3, LAST ROW OF PES MUST BE RETURN AS THIS SAMPLE
		PE[0] = 0		S00	     	x    	X		S12		S04		S03			S02		S00		S10		S11			S21				
		PE[1] = 0 		0	  		0		X		S23		S41		S44			S22		S20		S30		S13			S14			
		PE[2] = 0		0			x	  	X 		S32		S24		S42			S33		S40		S01		S43			S34					
		PE[3] = 0       0	   		X		X		S31		0		0			0		0		0		0			0 */

		exe64(OP, x, PE_out[12],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][9],PE_out[8][10],PE_out[8][11], /*BUFF 8to1 in*/ PE_out[8][7],PE_out[8][6],PE_out[8][5],PE_out[8][4]); 
		exe64(OP, x, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[9][8],PE_out[9][9],PE_out[9][6],PE_out[9][5], /*BUFF 8to1 in*/ PE_out[9][10],PE_out[9][11],PE_out[9][7],PE_out[9][4]); 		
		exe64(OP, x, PE_out[14],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][8],PE_out[10][9],PE_out[10][7],PE_out[10][11], /*BUFF 8to1 in*/ PE_out[10][5],PE_out[10][4],PE_out[10][6],PE_out[10][10]); 
		exe64(OP, x, PE_out[15],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[11][4],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8],PE_out[11][9],PE_out[11][10],PE_out[11][11]);  

		/*DONE CHI AND RETURN
		PE[0] = X	X	x	X	S[0][0]		S[1][0]		S[1][1]		S[2][1]		S[0][2]		S[0][3]		S[0][4]		S[1][2]
		PE[1] = X 	x	X	X	S[2][0]		S[3][0]		S[4][4] 	S[4][1]		S[1][3]		S[1][4]		S[2][2]		S[2][3]
		PE[2] = X	x	X	X 	S[4][0]		S[0][1]		S[3][3]		S[3][4]		S[2][4]		S[3][2]		S[4][2]		S[4][3]
		PE[3] = X	x	X	X	S[3][1]		X			X			X			X			X			X			X*/
}

			
	}
	
}

void Get_Result(){
	printf("A[%d][%d] = %16lx\n", 0, 0, PE_out[12][4]);
	printf("A[%d][%d] = %16lx\n", 0, 1, PE_out[14][5]);
	printf("A[%d][%d] = %16lx\n", 0, 2, PE_out[12][8]);
	printf("A[%d][%d] = %16lx\n", 0, 3, PE_out[12][9]);
	printf("A[%d][%d] = %16lx\n", 0, 4, PE_out[12][10]);
	printf("A[%d][%d] = %16lx\n", 1, 0, PE_out[12][5]);
	printf("A[%d][%d] = %16lx\n", 1, 1, PE_out[12][6]);
	printf("A[%d][%d] = %16lx\n", 1, 2, PE_out[12][11]);
	printf("A[%d][%d] = %16lx\n", 1, 3, PE_out[13][8]);
	printf("A[%d][%d] = %16lx\n", 1, 4, PE_out[13][9]);
	printf("A[%d][%d] = %16lx\n", 2, 0, PE_out[13][4]);
	printf("A[%d][%d] = %16lx\n", 2, 1, PE_out[12][7]);
	printf("A[%d][%d] = %16lx\n", 2, 2, PE_out[13][10]);
	printf("A[%d][%d] = %16lx\n", 2, 3, PE_out[13][11]);
	printf("A[%d][%d] = %16lx\n", 2, 4, PE_out[14][8]);
	printf("A[%d][%d] = %16lx\n", 3, 0, PE_out[13][5]);
	printf("A[%d][%d] = %16lx\n", 3, 1, PE_out[15][4]);
	printf("A[%d][%d] = %16lx\n", 3, 2, PE_out[14][9]);
	printf("A[%d][%d] = %16lx\n", 3, 3, PE_out[14][6]);
	printf("A[%d][%d] = %16lx\n", 3, 4, PE_out[14][7]);
	printf("A[%d][%d] = %16lx\n", 4, 0, PE_out[14][4]);
	printf("A[%d][%d] = %16lx\n", 4, 1, PE_out[13][7]);
	printf("A[%d][%d] = %16lx\n", 4, 2, PE_out[14][10]);
	printf("A[%d][%d] = %16lx\n", 4, 3, PE_out[14][11]);
	printf("A[%d][%d] = %16lx\n", 4, 4, PE_out[13][6]);

}
void SHA3_512(U64 RC[24]){
	assign_S_xor_R();
	for (int i = 0; i < 24; i++){
	round_ne(RC[i]);
	}
	Get_Result();
}

int main(){
	
	Init();
	
	int k,i,j, LOOP;
	LOOP = 25;


	int max_input_length = 4096; // Maximum length of the input
    int block_size = 72; // Block size for padding
    uint8_t input[max_input_length + block_size]; // Input buffer
    memset(input, 0, sizeof(input)); // Initialize input buffer with zeros

   
	LMM[0][0][0] = 0xa66be1dc7f37fb80;		//A[0][0]: a66be1dc7f37fb80        0X123;     
	LMM[0][1][0] = 0x13931921bba211f6;		//A[0][1]: 13931921bba211f6        0X456;
	LMM[1][0][0] = 0xbff00febbae335e6;		//A[0][2]: bff00febbae335e6        0X789;
	LMM[1][1][0] = 0x201f4a6fe5211611;		//A[0][3]: 201f4a6fe5211611        0X10a;
	LMM[2][0][0] = 0x737be8ae2da96eb2;		//A[0][4]: 737be8ae2da96eb2        0X11b;
	LMM[2][1][0] = 0x800f0c5d332f0211;		//A[1][0]: 800f0c5d332f0211        0X412;
	LMM[3][0][0] = 0xb8ca01f808cf9556;		//A[1][1]: b8ca01f808cf9556        0X309;
	LMM[3][1][0] = 0xa09a3e970f814c25;		//A[1][2]: a09a3e970f814c25        0X221;
	LMM[7][0][0] = 0xe33e8248e3eef0e2;		//A[1][3]: e33e8248e3eef0e2        0X690;
	
	
	U64 RC[24] = {0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,0x000000000000808B,0x0000000080000001,0x8000000080008081,0x8000000000008009,0x000000000000008A,0x0000000000000088,0x0000000080008009,
	0x000000008000000A,0x000000008000808B,0x800000000000008B,0x8000000000008089,0x8000000000008003,0x8000000000008002,0x8000000000000080,0x000000000000800A,0x800000008000000A,0x8000000080008081,0x8000000000008080,0x0000000080000001,0x8000000080008008};
	SHA3_512(RC);
	LMM[0][0][0] = 0xa66be1dc7f37fb80;		//A[0][0]: a66be1dc7f37fb80        0X123;     
	LMM[0][1][0] = 0x13931921bba211f6;		//A[0][1]: 13931921bba211f6        0X456;
	LMM[1][0][0] = 0xbff00febbae335e6;		//A[0][2]: bff00febbae335e6        0X789;
	LMM[1][1][0] = 0x201f4a6fe5211611;		//A[0][3]: 201f4a6fe5211611        0X10a;
	LMM[2][0][0] = 0x737be8ae2da96eb2;		//A[0][4]: 737be8ae2da96eb2        0X11b;
	LMM[2][1][0] = 0x800f0c5d332f0211;		//A[1][0]: 800f0c5d332f0211        0X412;
	LMM[3][0][0] = 0xb8ca01f808cf9556;		//A[1][1]: b8ca01f808cf9556        0X309;
	LMM[3][1][0] = 0xa09a3e970f814c25;		//A[1][2]: a09a3e970f814c25        0X221;
	LMM[7][0][0] = 0xe33e8248e3eef0e2;		//A[1][3]: e33e8248e3eef0e2        0X690;
	SHA3_512(RC);
	Final();

	return 0;
}
