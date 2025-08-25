#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>
#include <endian.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

#define N 4



int main(){
	int k,i,j;
	U32 OP;
	Init();

	U32 K[64];
	uint32_t KEY[8] = {0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f, 0x10111213, 0x14151617, 0x18191a1b, 0x1c1d1e1f};
	uint32_t PLAIN_TEXT[] = {0x4c616469, 0x65732061, 0x6e642047, 0x656e746c, 0x656d656e, 0x206f6620, 0x74686520, 0x636c6173, 0x73206f66, 0x20273939, 0x3a204966, 0x20492063, 0x6f756c64, 0x206f6666, 0x65722079, 0x6f75206f};
	uint32_t NONCE[3] = {0x00000000, 0x4a000000, 0x00000000};
	uint32_t COUNTER = 0x00000001;

	uint32_t KEY_LE[8];
	for (int i = 0; i < 8; i++) {
		KEY_LE[i] = __builtin_bswap32(KEY[i]);
	}
	uint32_t STATE[] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, KEY_LE[0], KEY_LE[1], KEY_LE[2], KEY_LE[3], KEY_LE[4], KEY_LE[5], KEY_LE[6], KEY_LE[7], COUNTER, NONCE[0], NONCE[1], NONCE[2]};
	// //printf("State:\n");
	// for (i = 0; i < 16; i++) {
		// //printf("%08x ", STATE[i]);
		// if((i+1)%4 == 0){
			// //printf("\n");
		// }
	// }
		// Initial PLAIN_TEXT to Local Memory
	// LMM[0][0][0] = STATE[0] ;
	// LMM[0][1][0] = STATE[4] ;
    // LMM[4][0][0] = STATE[8] ;
	// LMM[4][1][0] = STATE[12] ;

	// LMM[1][0][0] = STATE[5] ;
	// LMM[1][1][0] = STATE[9] ;
    // LMM[5][0][0] = STATE[13] ;
	// LMM[5][1][0] = STATE[1] ;

 	// LMM[2][0][0] = STATE[10] ;
	// LMM[2][1][0] = STATE[14] ;
    // LMM[6][0][0] = STATE[2];
	// LMM[6][1][0] = STATE[6];

 	// LMM[3][0][0] = STATE[15];
	// LMM[3][1][0] = STATE[3];
	// LMM[7][0][0] = STATE[7];
	// LMM[7][1][0] = STATE[11];

	// LMM[0][0][0] = 0x1111222211112222;
	// LMM[0][1][0] = 0x3333444433334444;
	// LMM[1][0][0] = 0x5555666655556666;
	// LMM[1][1][0] = 0x7777888877778888;
	LMM[0][0][0] = 0x1111111111111111;
	LMM[0][1][0] = 0x2222222222222222;
	LMM[1][0][0] = 0x3333333333333333;
	LMM[1][1][0] = 0x4444444444444444;

	// LMM[0][0][0] = 0x1111111122222222;
	// LMM[0][1][0] = 0x3333333344444444;
	// LMM[1][0][0] = 0x5555555566666666;
	// LMM[1][1][0] = 0x7777777788888888;
	
	LMM[2][0][0] = 0x1111111111111111;
	LMM[2][1][0] = 0x2222222222222222;
	LMM[3][0][0] = 0x3333333333333333;
	LMM[3][1][0] = 0x4444444444444444;
	
    LMM[4][0][0] = 0x5566778811112222;
	LMM[4][1][0] = 0x99aabbcc33334444;
    LMM[5][0][0] = 0x0123012355556666;
	LMM[5][1][0] = 0x4567456777778888;
    LMM[6][0][0] = 0x89ab89ab9999aaaa;
	LMM[6][1][0] = 0xcdefcdefbbbbcccc;
	LMM[7][0][0] = 0x12345678ddddeeee;
	LMM[7][1][0] = 0x9abcdef011223344;

	// Initial STATE to Local Memory

	#if defined(ARMZYNQ)

	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	////printf("membase: %llx\n", (U64)membase);
	
	U32 *A;
	A = (U32*)(membase + ROW0_BASE_PHYS);
	
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			if(j==0)
				A[i*8 +j] = LMM[j/2][j%2][0];
			else
				A[i*8 +j] = LMM[j/2][j%2][0];
		}
	}
	
	U32 *B;
	B = (U32*)(membase + ROW1_BASE_PHYS);
	for (i=0; i<8; i++) {
		for (j=0; j<8; j++) {
			B[i*8 +j] = LMM[j/2+4][j%2][0];
		}
	}


	#elif defined(VIVADOSIL)
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%016lx%016lx%016lx%016lx\n",ROW0_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%016lx%016lx%016lx%016lx\n",ROW0_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%016lx%016lx%016lx%016lx\n",ROW1_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%016lx%016lx%016lx%016lx\n",ROW1_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		CGRA_info.LDM_Offset = 0;

		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",32);			
	#endif
	
	// #elif defined(VIVADOSIL)
		// CGRA_info.LDM_Offset = 0;
		
		// for (i = 0; i < 8; i ++){	
			// //fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			// fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			// CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		// }
		
		// CGRA_info.LDM_Offset = 0;
		// for (i = 0; i < 8; i ++){	
			// //fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[0][0][0],LMM[0][1][0],LMM[1][0][0],LMM[1][1][0],LMM[2][0][0],LMM[2][1][0],LMM[3][0][0],LMM[3][1][0]);
			// fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			// CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		// }
		// CGRA_info.LDM_Offset = 0;
		
		// for (i = 0; i < 8; i ++){	
			// //fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			// fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_MSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			// CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		// }
		
		// CGRA_info.LDM_Offset = 0;
		// for (i = 0; i < 8; i ++){	
			// //fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			// fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_LSB_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			// CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		// }
		// CGRA_info.LDM_Offset = 0;

		// fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",32);			
	// #endif



	int PE_out_idx = 0;	// PE output index
	int PE_in_idx = 0;	// PE input index
	int loop_idx = 0; 	// loop management
	int auto_PE_idx = 0;
    int CGRA_loop_idx = 0;
    int Salsa_loop_idx = 0;
	// int NUM_LOOP = 8 + 4*8 + 2;
	int NUM_LOOP = 2 + (5*20)/4;

	for(i = 0; i < NUM_LOOP; i++){
		if(i == 0){
			///*** 1st loop ***///
			///*** 1st row ***///

			PE_in_idx = (12+PE_out_idx)%16;
			for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				// //printf("PE_out_idx = %d, PE_in_idx = %d\n", PE_out_idx, PE_in_idx);

				mop32(OP_LDW, 0, PE_out[PE_out_idx], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0,0,0,0); // Cause sengmentation fault ? find out why

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
				
			}

			

			///*** 2nd row ***///

			for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				
				mop32(OP_LDW, 0, PE_out[PE_out_idx], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][0], PE_out[PE_in_idx][1], 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}

			///*** 3rd row ***///

			for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				
				mop32(OP_LDW, 0, PE_out[PE_out_idx], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4], PE_out[PE_in_idx][5], PE_out[PE_in_idx][0], PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}

			///*** 4th row ***///

			for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				
				mop32(OP_LDW, 0, PE_out[PE_out_idx], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4], PE_out[PE_in_idx][5], PE_out[PE_in_idx][6], PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}
		}
		else if (i<26)
		{
            for(CGRA_loop_idx = 0; CGRA_loop_idx < 4; CGRA_loop_idx++)		
            {    if(loop_idx == 0 || loop_idx == 5){
                    /*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/7,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        ///*** PE0 - row 1 ***///
                        exe32(OP, 0, PE_out[PE_out_idx], \
                        /*ALU in*/ \
                        PE_out[PE_in_idx][4-4*(i!=1)],\
                        PE_out[PE_in_idx][7-4*(i!=1)],\
                        0,\
                        PE_out[PE_in_idx][5-4*(i!=1)],\
                        /*BUFF 16to1 in*/ \
                        PE_out[PE_in_idx][4-4*(i!=1)],\
                        PE_out[PE_in_idx][5-4*(i!=1)],\
                        PE_out[PE_in_idx][6-4*(i!=1)],\
                        PE_out[PE_in_idx][7-4*(i!=1)],\
                        /*BUFF 8to1 in*/ 0,0,0,0); 

                        PE_out_idx = (PE_out_idx+1)%16;
                        PE_in_idx = (12+PE_out_idx)%16;
                    }

                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d\n", loop_idx);
                }
                else if(loop_idx == 1 || loop_idx == 6){
                    /*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/9,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        exe32(OP, 0, PE_out[PE_out_idx], \
                        /*ALU in*/ \
                        PE_out[PE_in_idx][1],\
                        PE_out[PE_in_idx][4],\
                        0,\
                        PE_out[PE_in_idx][6],\
                        /*BUFF 16to1 in*/ \
                        PE_out[PE_in_idx][4],\
                        PE_out[PE_in_idx][1],\
                        PE_out[PE_in_idx][6],\
                        PE_out[PE_in_idx][7],\
                        /*BUFF 8to1 in*/ 0,0,0,0); 

                        PE_out_idx = (PE_out_idx+1)%16;
                        PE_in_idx = (12+PE_out_idx)%16;
                    }

                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d\n", loop_idx);
                }
                else if(loop_idx == 2 || loop_idx == 7){
                    /*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/13,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        exe32(OP, 0, PE_out[PE_out_idx], \
                        /*ALU in*/ \
                        PE_out[PE_in_idx][1],\
                        PE_out[PE_in_idx][5],\
                        0,\
                        PE_out[PE_in_idx][7],\
                        /*BUFF 16to1 in*/ \
                        PE_out[PE_in_idx][4],\
                        PE_out[PE_in_idx][5],\
                        PE_out[PE_in_idx][1],\
                        PE_out[PE_in_idx][7],\
                        /*BUFF 8to1 in*/ 0,0,0,0); 

                        PE_out_idx = (PE_out_idx+1)%16;
                        PE_in_idx = (12+PE_out_idx)%16;
                    }
                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d\n", loop_idx);
                }
                else if(loop_idx == 3 || loop_idx == 8){
                    /*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_ROL,/*SRU1_IM*/18,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_XOR);

                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        exe32(OP, 0, PE_out[PE_out_idx], \
                        /*ALU in*/ \
                        PE_out[PE_in_idx][1],\
                        PE_out[PE_in_idx][6],\
                        0,\
                        PE_out[PE_in_idx][4],\
                        /*BUFF 16to1 in*/ \
                        PE_out[PE_in_idx][4],\
                        PE_out[PE_in_idx][5],\
                        PE_out[PE_in_idx][6],\
                        PE_out[PE_in_idx][1],\
                        /*BUFF 8to1 in*/ 0,0,0,0); 

                        PE_out_idx = (PE_out_idx+1)%16;
                        PE_in_idx = (12+PE_out_idx)%16;
                    }
                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d\n", loop_idx);
                }
                else if(loop_idx == 4){
                    /*--*/OP = OP_NOP;
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        if(auto_PE_idx == 0){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+2][6],\
                            PE_out[PE_in_idx-auto_PE_idx+3][5],\
                            PE_out[PE_in_idx-auto_PE_idx+0][1],\
                            PE_out[PE_in_idx-auto_PE_idx+1][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 1){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+3][6],\
                            PE_out[PE_in_idx-auto_PE_idx+0][5],\
                            PE_out[PE_in_idx-auto_PE_idx+1][1],\
                            PE_out[PE_in_idx-auto_PE_idx+2][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 2){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+0][6],\
                            PE_out[PE_in_idx-auto_PE_idx+1][5],\
                            PE_out[PE_in_idx-auto_PE_idx+2][1],\
                            PE_out[PE_in_idx-auto_PE_idx+3][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 3){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+1][6],\
                            PE_out[PE_in_idx-auto_PE_idx+2][5],\
                            PE_out[PE_in_idx-auto_PE_idx+3][1],\
                            PE_out[PE_in_idx-auto_PE_idx+0][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                    }
                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d\n", loop_idx);
                }
                else if(loop_idx == 9 && i < 25){
                    /*--*/OP = OP_NOP;
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        if(auto_PE_idx == 0){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+2][6],\
                            PE_out[PE_in_idx-auto_PE_idx+3][5],\
                            PE_out[PE_in_idx-auto_PE_idx+0][1],\
                            PE_out[PE_in_idx-auto_PE_idx+1][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 1){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+3][6],\
                            PE_out[PE_in_idx-auto_PE_idx+0][5],\
                            PE_out[PE_in_idx-auto_PE_idx+1][1],\
                            PE_out[PE_in_idx-auto_PE_idx+2][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 2){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+0][6],\
                            PE_out[PE_in_idx-auto_PE_idx+1][5],\
                            PE_out[PE_in_idx-auto_PE_idx+2][1],\
                            PE_out[PE_in_idx-auto_PE_idx+3][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 3){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx-auto_PE_idx+1][6],\
                            PE_out[PE_in_idx-auto_PE_idx+2][5],\
                            PE_out[PE_in_idx-auto_PE_idx+3][1],\
                            PE_out[PE_in_idx-auto_PE_idx+0][7],\
                            /*BUFF 16to1 in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                    }
                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d. END OF LOOP %d HERE!!!!\n", loop_idx, Salsa_loop_idx);
                    Salsa_loop_idx++;
                }
                else if(loop_idx == 9 && i == 25){
                    /*--*/OP = OP_NOP;
                    for(auto_PE_idx = 0; auto_PE_idx<4; auto_PE_idx++){
                        if(auto_PE_idx == 0){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            /*BUFF 16to1 in*/ \
                            0,\
                            0,\
                            0,\
                            0,\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 1){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            PE_out[PE_in_idx][1],\
                            /*BUFF 16to1 in*/ \
                            0,\
                            0,\
                            0,\
                            0,\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 2){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            PE_out[PE_in_idx][7],\
                            /*BUFF 16to1 in*/ \
                            0,\
                            0,\
                            0,\
                            0,\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                        else if(auto_PE_idx == 3){
                            exe32(OP, 0, PE_out[PE_out_idx], \
                            /*ALU in*/ \
                            PE_out[PE_in_idx][7],\
                            PE_out[PE_in_idx][1],\
                            PE_out[PE_in_idx][5],\
                            PE_out[PE_in_idx][6],\
                            /*BUFF 16to1 in*/ \
                            0,\
                            0,\
                            0,\
                            0,\
                            /*BUFF 8to1 in*/ 0,0,0,0);  

                            PE_out_idx = (PE_out_idx+1)%16;
                            PE_in_idx = (12+PE_out_idx)%16;
                        }
                    }
                    loop_idx = (loop_idx + 1)%10;
                    //printf("loop_idx = %d. END OF LOOP %d HERE!!!!\n", loop_idx, Salsa_loop_idx);
                    Salsa_loop_idx++;
                }
            }
		}
        else{
            for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				// //printf("PE_out_idx = %d, PE_in_idx = %d\n", PE_out_idx, PE_in_idx);

				mop32(OP_STW, 0, PE_out[PE_out_idx], \
                /*ALU in*/ \
                PE_out[PE_in_idx][0],\
                PE_out[PE_in_idx][1],\
                PE_out[PE_in_idx][2],\
                PE_out[PE_in_idx][3],\
                /*BUFF 16to1 in*/ \
                0,0,0,0, \
                /*BUFF 8to1 in*/ \
                0,0,0,0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}

            for(auto_PE_idx = 0; auto_PE_idx < 4; auto_PE_idx++){
				// //printf("PE_out_idx = %d, PE_in_idx = %d\n", PE_out_idx, PE_in_idx);

				mop32(OP_STW, 0, PE_out[PE_out_idx], \
                /*ALU in*/ \
                PE_out[PE_in_idx][2],\
                PE_out[PE_in_idx][3],\
                0,\
                0,\
                /*BUFF 16to1 in*/ \
                0,0,0,0, \
                /*BUFF 8to1 in*/ \
                0,0,0,0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}

            for(auto_PE_idx = 0; auto_PE_idx < 8; auto_PE_idx++){
				printf("PE_out_idx = %d, PE_in_idx = %d\n", PE_out_idx, PE_in_idx);

				exe32(OP_NOP, 0, PE_out[PE_out_idx], \
                /*ALU in*/ \
                0,\
                0,\
                0,\
                0,\
                /*BUFF 16to1 in*/ \
                0,0,0,0, \
                /*BUFF 8to1 in*/ \
                0,0,0,0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+PE_out_idx)%16;
			}
        }

	}
	
	#if defined(ARMZYNQ)
	
	dma_write(ROW0_BASE_PHYS,8);
	dma_write(ROW1_BASE_PHYS,8);
	
	*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    while(1){
		if(*(CGRA_info.ctx_pe_mmap+(FINISH_BASE_IP>>2)))
			break;
	}
	//sleep(1);
	////printf("Done!\n");
	
	dma_read(ROW0_BASE_PHYS,8);
	dma_read(ROW1_BASE_PHYS,8);
		for (i=0; i<8; i++) {
			printf("Output[%d] = ",i);
			printf("%08X ",A[i*8+0]);
			printf("%08X ",A[i*8+1]);
			printf("%08X ",B[i*8+0]);
			printf("%08X ",B[i*8+1]);
			
			printf("%08X ",A[i*8+2]);
			printf("%08X ",A[i*8+3]);
			printf("%08X ",B[i*8+2]);
			printf("%08X ",B[i*8+3]);
			
			printf("%08X ",A[i*8+4]);
			printf("%08X ",A[i*8+5]);
			printf("%08X ",B[i*8+4]);
			printf("%08X ",B[i*8+5]);
			
			printf("%08X ",A[i*8+6]);
			printf("%08X ",A[i*8+7]);
			printf("%08X ",B[i*8+6]);
			printf("%08X ",B[i*8+7]);
			printf("\n");
		}
	#endif
	
	#if defined(ARMSIL)
		printf("\n Output =");
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[0][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[4][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[1][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[5][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[2][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[6][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[3][i][0]);
        }
        for(i = 0; i < 2; i++){
            printf(" %08X", LMM[7][i][0]);
        }

		printf("\n");
	#endif
	Final();
	
	return 0;
}
