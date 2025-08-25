#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"
// #include "../FPGA_Driver.c"

#define N 		 4
#define Nk 4 // The number of 32 bit words in a key.
#define Nb 4 // The number of columns comprising a state in AES. This is fixed.
#define Nr 10 // The number of rounds in AES Cipher.

// S-box transformation table
static const uint8_t sbox[256] = {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

uint8_t getSBoxValue(uint8_t num)
{
    return sbox[num];
}

// Rcon is Round Constant used for the Key Expansion [1st col is 2^(r-1) in GF(2^8)] [ยง5.2]
static const uint8_t Rcon[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion32bit(uint32_t* RoundKey, const uint32_t* Key)
{
    unsigned i, j;
    uint32_t temp;

    // The first round key is the key itself.
    for(i = 0; i < Nk; ++i)
    {
        RoundKey[i] = Key[i];
    }

    // All other round keys are found from the previous round keys.
    for(; (i < (Nb * (Nr + 1))); ++i)
    {
        temp = RoundKey[i-1];
        if (i % Nk == 0)
        {
            // This function rotates the 4 bytes in a word to the left once.
            // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

            // Function RotWord()
            {
                const uint8_t u8temp = (temp >> 24) & 0xff;
                temp <<= 8;
                temp |= u8temp;
            }

            // SubWord() is a function that takes a four-byte input word and 
            // applies the S-box to each of the four bytes to produce an output word.

            // Function Subword()
            {
                for(j = 0; j < 4; ++j)
                {
                    uint8_t u8temp = (temp >> (24 - j * 8)) & 0xff;
                    u8temp = getSBoxValue(u8temp);
                    temp &= ~(0xff << (24 - j * 8));
                    temp |= u8temp << (24 - j * 8);
                }
            }

            temp ^= Rcon[i/Nk]<<24;
        }
        else if (Nk > 6 && i % Nk == 4)
        {
            // Function Subword()
            {
                for(j = 0; j < 4; ++j)
                {
                    uint8_t u8temp = (temp >> (24 - j * 8)) & 0xff;
                    u8temp = getSBoxValue(u8temp);
                    temp &= ~(0xff << (24 - j * 8));
                    temp |= u8temp << (24 - j * 8);
                }
            }
        }
        RoundKey[i] = RoundKey[i - Nk] ^ temp;
		printf("RoundKey[%d] = %x\n", i, RoundKey[i]);
    }
}

int main(){

	int k,i,j, LOOP;
	U32 OP;
	Init();
	//LOOP = 12;
	
	// }
	// uint32_t KEY[4] = {0x54686174, 0x73206D79, 0x204B756E, 0x67204675};
	//uint32_t Plain_Text[4] = {0x54776F20, 0x4F6E6520, 0x4E696E65, 0x2054776F};


	uint32_t KEY[] 			= { 0x2b7e1516, 0x28aed2a6, 0xabf71588, 0x09cf4f3c };
	uint32_t Plain_Text[] 	= { 0x6bc1bee2, 0x2e409f96, 0xe93d7e11, 0x7393172a };
	uint32_t Cipher_Text[] 	= { 0x3ad77bb4, 0x0d7a3660, 0xa89ecaf3, 0x2466ef97 };

	uint32_t RCON[11] = {0x8d000000, 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

	// Create an array to hold the expanded key.
    uint32_t RoundKey[Nb * (Nr + 1)];

	KeyExpansion32bit(RoundKey, KEY);
	
	LMM[0][0][0] = Cipher_Text[0];
	LMM[0][1][0] = Cipher_Text[1];

	LMM[2][0][0] = Cipher_Text[0];
	LMM[2][1][0] = Cipher_Text[1];
	
	LMM[1][0][0] = Cipher_Text[0];
	LMM[1][1][0] = Cipher_Text[1];

	LMM[3][0][0] = Cipher_Text[0];
	LMM[3][1][0] = Cipher_Text[1];
	
	LMM[4][0][0] = Cipher_Text[2];
	LMM[4][1][0] = Cipher_Text[3];

	LMM[6][0][0] = Cipher_Text[2];
	LMM[6][1][0] = Cipher_Text[3];
	
	LMM[5][0][0] = Cipher_Text[2];
	LMM[5][1][0] = Cipher_Text[3];

	LMM[7][0][0] = Cipher_Text[2];
	LMM[7][1][0] = Cipher_Text[3];

	int loop_mng = 0;
	int row_count = 0;
	int PE_out_idx = 0;
	int PE_in_idx = 0;
	int key_idx = 44;
	int row_idx = 0;
	//ROM[8][0] = K[0];
	
#if defined(ARMZYNQ)
	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
	
	printf("membase: %llx\n", (U64)membase);
	
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
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW0_BASE_PHYS+CGRA_info.LDM_Offset,LMM[3][0][0],LMM[3][1][0],LMM[2][0][0],LMM[2][1][0],LMM[1][0][0],LMM[1][1][0],LMM[0][0][0],LMM[0][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[4][0][0],LMM[4][1][0],LMM[5][0][0],LMM[5][1][0],LMM[6][0][0],LMM[6][1][0],LMM[7][0][0],LMM[7][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW1_BASE_PHYS+CGRA_info.LDM_Offset,LMM[7][0][0],LMM[7][1][0],LMM[6][0][0],LMM[6][1][0],LMM[5][0][0],LMM[5][1][0],LMM[4][0][0],LMM[4][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}

		
		fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",16);			
	#endif
	
	LOOP = 17;
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
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[8],/*ALU in*/ 0,0,0,PE_out[4][4], /*BUFF 16to1 in*/ PE_out[4][4],PE_out[4][5], PE_out[4][0],PE_out[4][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
	
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[9],/*ALU in*/ 0,0,0,PE_out[5][4], /*BUFF 16to1 in*/ PE_out[5][4],PE_out[5][5], PE_out[5][0],PE_out[5][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[10],/*ALU in*/ 0,0,0,PE_out[6][4], /*BUFF 16to1 in*/ PE_out[6][4],PE_out[6][5], PE_out[6][0],PE_out[6][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[11],/*ALU in*/ 0,0,0,PE_out[7][4], /*BUFF 16to1 in*/ PE_out[7][4],PE_out[7][5], PE_out[7][0],PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			loop_mng = (loop_mng+1)%6;

			///*** row 3 ***///		
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[12],/*ALU in*/ 0,0,0,PE_out[8][5], /*BUFF 16to1 in*/ PE_out[8][1],PE_out[8][5], PE_out[8][6],PE_out[8][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
	
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[13],/*ALU in*/ 0,0,0,PE_out[9][5], /*BUFF 16to1 in*/ PE_out[9][1],PE_out[9][5], PE_out[9][6],PE_out[9][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[14],/*ALU in*/ 0,0,0,PE_out[10][5], /*BUFF 16to1 in*/ PE_out[10][1],PE_out[10][5], PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[15],/*ALU in*/ 0,0,0,PE_out[11][5], /*BUFF 16to1 in*/ PE_out[11][1],PE_out[11][5], PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			loop_mng = (loop_mng+1)%6;

		}
		else if(i==1){
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			///*** row 0 ***///		------------------------------------------------------------
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			key_idx = key_idx - 4*(loop_mng==3);
			loop_mng = (loop_mng+1)%6;


			///*** row 1 ***///		------------------------------------------------------------
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
	
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			key_idx = key_idx - 4*(loop_mng==3);
			loop_mng = (loop_mng+1)%6;
						
			///*** row 2 ***///		------------------------------------------------------------
			/*--*/OP = CUSTOM_OP(OP_ISHF_ISUB);
			exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
			exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
	
			key_idx = key_idx - 4*(loop_mng==3);
			loop_mng = 0;

			///*** row 3 ***///		------------------------------------------------------------
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			
			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
	
			key_idx = key_idx - 4*(loop_mng==3);
			loop_mng = (loop_mng+1)%6;


		}
		else if(i < 16){ 
			for (row_idx = 0; row_idx < 4; row_idx++){
				///*** row 0 ***///		------------------------------------------------------------
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
				
				if(loop_mng == 0){
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
				
					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
					key_idx = key_idx - 4*(loop_mng==3);
					loop_mng = (loop_mng+1)%6;
				}
				else if(loop_mng < 4){
					/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
					key_idx = key_idx - 4*(loop_mng==3);
					loop_mng = (loop_mng+1)%6;

					
				}																									
				else if(loop_mng==4){
					
					/*--*/OP = CUSTOM_OP(OP_IMIXCOL);
					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
					
					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
					key_idx = key_idx - 4*(loop_mng==3);
					loop_mng = (loop_mng+1)%6;
					
				}
				else{
					/*--*/OP = CUSTOM_OP(OP_ISHF_ISUB);
					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
					
					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

					exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

					PE_out_idx = (PE_out_idx+1)%16;
					PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
					key_idx = key_idx - 4*(loop_mng==3);
					loop_mng = (loop_mng+1)%6;
					
					printf("End of round %d\n",(44 - key_idx)/4);
				}
			}
		}
		else if(i==16){

			///*** row 0 ***///		------------------------------------------------------------
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
			if(loop_mng == 0){
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
			
				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][loop_mng],PE_out[PE_in_idx][loop_mng+1], PE_out[PE_in_idx][loop_mng+2],PE_out[PE_in_idx][loop_mng+3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
		
				key_idx = key_idx - 4*(loop_mng==3);
				loop_mng = (loop_mng+1)%6;
			}
			else if(loop_mng < 4){
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_XOR,/*SRU1*/OP_NOP,/*SRU1_IM*/0,/*SRU2*/OP_NOP,/*SRU2_IM*/0,/*LU3*/OP_NOP);
				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
		
				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, RoundKey[key_idx-4+loop_mng], PE_out[PE_out_idx],/*ALU in*/ 0,0,0,PE_out[PE_in_idx][4+loop_mng], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4-((loop_mng==1) ? 2+loop_mng:0)],PE_out[PE_in_idx][5-((loop_mng==2) ? 2+loop_mng:0)], PE_out[PE_in_idx][6-((loop_mng==3) ? 2+loop_mng:0)],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
				key_idx = key_idx - 4*(loop_mng==3);
				loop_mng = (loop_mng+1)%6;

				
			}
			else if(loop_mng==4){
				
				/*--*/OP = CUSTOM_OP(OP_IMIXCOL);
				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
				
				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
		
				key_idx = key_idx - 4*(loop_mng==3);
				loop_mng = (loop_mng+1)%6;
				
			}
			else{
				/*--*/OP = CUSTOM_OP(OP_ISHF_ISUB);
				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
				
				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

				exe32(OP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1],PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 16to1 in*/ PE_out[PE_in_idx][0],PE_out[PE_in_idx][1], PE_out[PE_in_idx][2],PE_out[PE_in_idx][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);

				PE_out_idx = (PE_out_idx+1)%16;
				PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
		
				key_idx = key_idx - 4*(loop_mng==3);
				loop_mng = (loop_mng+1)%6;
				
				printf("End of round %d\n",(44 - key_idx)/4);
			}
			///*** row 1 ***///
			exe32(OP_NOP, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
			exe32(OP_NOP, 0, PE_out[PE_out_idx],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP_NOP, 0, PE_out[PE_out_idx],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			exe32(OP_NOP, 0, PE_out[PE_out_idx],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
	
			///*** row 2 ***///
			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			///*** row 3 ***///
			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][6],PE_out[PE_in_idx][7],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
			
			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][6],PE_out[PE_in_idx][7],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][6],PE_out[PE_in_idx][7],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;

			mop32(OP_STW, 0, PE_out[PE_out_idx],/*ALU in*/ PE_out[PE_in_idx][6],PE_out[PE_in_idx][7],0,0, /*BUFF 16to1 in*/ PE_out[PE_in_idx][4],PE_out[PE_in_idx][5], PE_out[PE_in_idx][6],PE_out[PE_in_idx][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);

			PE_out_idx = (PE_out_idx+1)%16;
			PE_in_idx = (12+(4*row_count + PE_out_idx))%16;
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
	
		U32 *C;
	C = (U32*)(membase + ROW2_BASE_PHYS);
	dma_read(ROW2_BASE_PHYS,8);
	
	for (i=0; i<8; i++) {
		printf("Output %d = ",i);
		for (j=0; j<8; j++) {
			printf("%08x ",C[i*8+j]);
		}
		printf("\n");
		}
		
	U32 *D;
	D = (U32*)(membase + ROW3_BASE_PHYS);
	dma_read(ROW3_BASE_PHYS,8);
	
		for (i=0; i<8; i++) {
		printf("Output %d = ",i);
		for (j=0; j<8; j++) {
			printf("%08x ",D[i*8+j]);
		}
		printf("\n");
		}

	#endif
	
	#if defined(ARMSIL)
		printf("\n Hash output 0 =");
		for(i = 0; i < 2; i++){
			printf(" %08x", LMM[8][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[12][i][0]);
		}
		printf("\n Hash output 1 =");
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[9][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[13][i][0]);
		}
		printf("\n Hash output 2 =");
		for(i = 0; i < 2; i++){
			printf(" %08x", LMM[10][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[14][i][0]);
		}
		printf("\n Hash output 3 =");
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[11][i][0]);
		}
		for(i = 0; i < 2; i++){
			printf(" %08x",LMM[15][i][0]);
		}
		printf("\n");
	#endif
	Final();
	return 0;
}