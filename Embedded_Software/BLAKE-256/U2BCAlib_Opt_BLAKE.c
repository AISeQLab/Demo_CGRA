
/* U2BCA library                        */
/*     Copyright (C) 2023- by NAIST     */
/*       Primary writer: Pham Hoai Luan */
/*            pham.luan@is.naist.jp     */

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <linux/ioctl.h>


/*******************************************************************************/
/********************************** U2BCA Interface ****************************/
/*******************************************************************************/

///**For AES-like algorithms**///

static const uint8_t SBOX[256] = {
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

uint8_t gf_mul(uint8_t a, uint8_t b) {
  uint8_t p = 0;
  uint8_t hi_bit_set;
  int i;

  for (i = 0; i < 8; i++) {
    if (b & 1) {
      p ^= a;
    }
    hi_bit_set = (a & 0x80);
    a <<= 1;
    if (hi_bit_set) {
      a ^= 0x1b;
    }
    b >>= 1;
  }

  return p;
}
		
uint32_t *mixColumns(uint32_t state[4]) {

  uint8_t state0[4], state1[4], state2[4], state3[4];
  uint8_t out0[4], out1[4], out2[4], out3[4];
  uint32_t *out;
   out = (uint32_t*)calloc(4, sizeof(uint32_t));
   
  int i;
  
  state0[0] = (state[0] >> 24) &0xFF;
  state0[1] = (state[0] >> 16) &0xFF;
  state0[2] = (state[0] >> 8) &0xFF;
  state0[3] = (state[0] >> 0) &0xFF;
  
  state1[0] = (state[1] >> 24) &0xFF;
  state1[1] = (state[1] >> 16) &0xFF;
  state1[2] = (state[1] >> 8) &0xFF;
  state1[3] = (state[1] >> 0) &0xFF;
  
  state2[0] = (state[2] >> 24) &0xFF;
  state2[1] = (state[2] >> 16) &0xFF;
  state2[2] = (state[2] >> 8) &0xFF;
  state2[3] = (state[2] >> 0) &0xFF;
  
  state3[0] = (state[3] >> 24) &0xFF;
  state3[1] = (state[3] >> 16) &0xFF;
  state3[2] = (state[3] >> 8) &0xFF;
  state3[3] = (state[3] >> 0) &0xFF;
  
  out0[0] = gf_mul(state0[0],2)^gf_mul(state0[1],3)^state0[2]^state0[3];
  out1[0] = gf_mul(state1[0],2)^gf_mul(state1[1],3)^state1[2]^state1[3];
  out2[0] = gf_mul(state2[0],2)^gf_mul(state2[1],3)^state2[2]^state2[3];
  out3[0] = gf_mul(state3[0],2)^gf_mul(state3[1],3)^state3[2]^state3[3];
  
  out0[1] = state0[0]^gf_mul(state0[1],2)^gf_mul(state0[2],3)^state0[3];
  out1[1] = state1[0]^gf_mul(state1[1],2)^gf_mul(state1[2],3)^state1[3];
  out2[1] = state2[0]^gf_mul(state2[1],2)^gf_mul(state2[2],3)^state2[3];
  out3[1] = state3[0]^gf_mul(state3[1],2)^gf_mul(state3[2],3)^state3[3];

  out0[2] = state0[0]^state0[1]^gf_mul(state0[2],2)^gf_mul(state0[3],3);
  out1[2] = state1[0]^state1[1]^gf_mul(state1[2],2)^gf_mul(state1[3],3);
  out2[2] = state2[0]^state2[1]^gf_mul(state2[2],2)^gf_mul(state2[3],3);
  out3[2] = state3[0]^state3[1]^gf_mul(state3[2],2)^gf_mul(state3[3],3);
  
  out0[3] = state0[2]^state0[1]^gf_mul(state0[3],2)^gf_mul(state0[0],3);
  out1[3] = state1[2]^state1[1]^gf_mul(state1[3],2)^gf_mul(state1[0],3);
  out2[3] = state2[2]^state2[1]^gf_mul(state2[3],2)^gf_mul(state2[0],3);
  out3[3] = state3[2]^state3[1]^gf_mul(state3[3],2)^gf_mul(state3[0],3);
	
  out[0] = (out0[0] << 24) | (out0[1] << 16) | (out0[2] << 8) | (out0[3] <<0);
  out[1] = (out1[0] << 24) | (out1[1] << 16) | (out1[2] << 8) | (out1[3] <<0);
  out[2] = (out2[0] << 24) | (out2[1] << 16) | (out2[2] << 8) | (out2[3] <<0);
  out[3] = (out3[0] << 24) | (out3[1] << 16) | (out3[2] << 8) | (out3[3] <<0);
  
  return out;
}

  
void  /*__attribute__((always_inline))*/
mop(U8 OP, U64 base, U64 off_set, U32 *LMM, U32 *PE_out, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3)
{
  #if defined(ARMZYNQ)
  
  #elif defined(ARMSIL)
  ///  Buffer 16to1 output mapping
  PE_out[4] = b16_0;
  PE_out[5] = b16_1;
  PE_out[6] = b16_2;
  PE_out[7] = b16_3;
  
  ///  Buffer 8to1 output mapping
  PE_out[8]  = b8_0;
  PE_out[9]  = b8_1;
  PE_out[10]  = b8_2;
  PE_out[11]  = b8_3;
  
  switch (OP) {
	  case OP_LDW:
		PE_out[0] = LMM[0]; // load word
		PE_out[1] = LMM[1]; // load word
		PE_out[2] = LMM[2]; // load word
		PE_out[3] = LMM[3]; // load word
		break;
	  case OP_STW:
		LMM[0] = PE_out[0]; // input_0
		LMM[1] = PE_out[1]; // input_0
		LMM[2] = PE_out[2]; // input_0
		LMM[3] = PE_out[3]; // input_0
		break;
	  default:
		printf("Operation of OP is NOT supported!\n");
		break;
  }
  #else
	printf("Please select the Platform: -ARMZYNQ or -ARMSIL!\n");
	exit(0);
 #endif
}

uint64_t OP2_CAT(U8 OP2_LU1,U8 OP2_LU2,U8 OP2_SRU1, U8 SRU1_IM, U8 OP2_SRU2,U8 SRU2_IM, U8 OP2_SRU3, U8 SRU3_IM, U8 OP2_LU3, U8 KEEP_OP1){

	uint64_t OP2;
	uint8_t OP2_LU1_t, OP2_LU2_t, OP2_LU3_t, OP2_SRU1_t, OP2_SRU2_t, OP2_SRU3_t;
    uint8_t SRU1_IM_t, SRU2_IM_t, SRU3_IM_t;
	
	OP2_LU1_t = OP2_LU1 & 0x7;
	OP2_LU2_t = OP2_LU2 & 0x7;
	OP2_SRU1_t = OP2_SRU1 & 0x3;
	SRU1_IM_t = SRU1_IM & 0x1F;
	OP2_SRU2_t = OP2_SRU2 & 0x3;
	SRU2_IM_t = SRU2_IM & 0x1F;
	OP2_SRU3_t = OP2_SRU3 & 0x3;
	SRU3_IM_t = SRU3_IM & 0x3F;
	OP2_LU3_t = OP2_LU3 & 0x7;
  
	OP2 = (KEEP_OP1<<31)|(OP2_LU1_t<<28)|(OP2_LU2_t<<25)|(OP2_SRU1_t<<23)|(SRU1_IM_t<<18)|(OP2_SRU2_t<<16)|(SRU2_IM_t<<11)|(OP2_SRU3_t<<9)|(SRU3_IM_t<<3)|(OP2_SRU3_t<<9)|(OP2_LU3_t<<0);
	//printf ("OP2 = %08x\n",OP2);
	return (OP2 & 0xFFFFFFFF);
}
	
void  /*__attribute__((always_inline))*/
exe(U8 OP1,U64 OP2, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3)
{
#if defined(ARMZYNQ)
	uint16_t config;
	uint8_t OP2_LU1, OP2_LU2, OP2_LU3, OP2_SRU1, OP2_SRU2, OP2_SRU3;
    uint8_t SRU1_IM, SRU2_IM, SRU3_IM;
	
	// config = ((op <<8)| (in1 <<6) | (in2 <<4) | (buf1 <<2)| (buf2 <<0)) & 0x00000FFF;
	// (*prt ) = config;
	// printf("config = %03x\n",config);
	
#elif defined(ARMSIL)
  uint32_t t;
  uint32_t temp[4];
  uint32_t s48_in[4];
  uint32_t *MIXCOL_out;
  MIXCOL_out = (uint32_t*)calloc(4, sizeof(uint32_t));

  s48_in[0] = s48_0;
  s48_in[1] = s48_1;
  s48_in[2] = s48_2;
  s48_in[3] = s48_3;
  
  ///  Buffer 16to1 output mapping
  PE_out[4] = b16_0;
  PE_out[5] = b16_1;
  PE_out[6] = b16_2;
  PE_out[7] = b16_3;
  
  ///  Buffer 8to1 output mapping
  PE_out[8]  = b8_0;
  PE_out[9]  = b8_1;
  PE_out[10]  = b8_2;
  PE_out[11]  = b8_3;
  
  ///**** ALU Architecture ****///
  uint32_t *ALU1_td, *ALU2_td;
  
  ALU1_td = (uint32_t*)calloc(4, sizeof(uint32_t));
  ALU2_td = (uint32_t*)calloc(4, sizeof(uint32_t));
  
  ///*** ALU1 ***///
  
    uint8_t s48_0_0, s48_0_1, s48_0_2, s48_0_3; // seperate input_0 into 4 bytes
	s48_0_0 = (s48_0 >> 24) & 0x000000FF; 
	s48_0_1 = (s48_0 >> 16) & 0x000000FF; 
	s48_0_2 = (s48_0 >> 8) & 0x000000FF; 
	s48_0_3 = (s48_0 >> 0) & 0x000000FF; 

	uint8_t s48_1_0, s48_1_1, s48_1_2, s48_1_3; // seperate input_1 into 4 bytes
	s48_1_0 = (s48_1 >> 24) & 0x000000FF; 
	s48_1_1 = (s48_1 >> 16) & 0x000000FF; 
	s48_1_2 = (s48_1 >> 8) & 0x000000FF; 
	s48_1_3 = (s48_1 >> 0) & 0x000000FF; 

	uint8_t s48_2_0, s48_2_1, s48_2_2, s48_2_3; // seperate input_2 into 4 bytes
	s48_2_0 = (s48_2 >> 24) & 0x000000FF; 
	s48_2_1 = (s48_2 >> 16) & 0x000000FF; 
	s48_2_2 = (s48_2 >> 8) & 0x000000FF; 
	s48_2_3 = (s48_2 >> 0) & 0x000000FF; 

	uint8_t s48_3_0, s48_3_1, s48_3_2, s48_3_3; // seperate input_3 into 4 bytes
	s48_3_0 = (s48_3 >> 24) & 0x000000FF; 
	s48_3_1 = (s48_3 >> 16) & 0x000000FF; 
	s48_3_2 = (s48_3 >> 8) & 0x000000FF; 
	s48_3_3 = (s48_3 >> 0) & 0x000000FF; 
		
  switch (OP1) {
	  case OP_NOP:
		ALU1_td[0] = s48_0; // pass through input_0
		ALU1_td[1] = s48_1; // pass through input_1
		ALU1_td[2] = s48_2; // pass through input_2
		ALU1_td[3] = s48_3; // pass through input_3
		break;
	  case OP_ADD2:
		ALU1_td[0] = s48_0 + s48_1; // input_0 + input_1
		ALU1_td[1] = s48_1; // pass through input_1
		ALU1_td[2] = s48_2; // pass through input_2
		ALU1_td[3] = s48_3; // pass through input_3
		break;
	  case OP_ADD3:
		ALU1_td[0] = s48_0 + s48_1 + s48_2; // input_0 + input_1 + input_2
		ALU1_td[1] = s48_1; // pass through input_1
		ALU1_td[2] = s48_2; // pass through input_2
		ALU1_td[3] = s48_3; // pass through input_3
		break;
	  case OP_SUB2:
		ALU1_td[0] = s48_0 - s48_1; // input_0 + input_1 + input_2
		ALU1_td[1] = s48_1; // pass through input_1
		ALU1_td[2] = s48_2; // pass through input_2
		ALU1_td[3] = s48_3; // pass through input_3
		break;
	  case OP_SUB3:
		ALU1_td[0] = s48_0 - s48_1 - s48_2; // input_0 + input_1 + input_2
		ALU1_td[1] = s48_1; // pass through input_1
		ALU1_td[2] = s48_2; // pass through input_2
		ALU1_td[3] = s48_3; // pass through input_3
		break;
	  case OP_SUBBYTE: 
		ALU1_td[0] = (SBOX[s48_0_0] << 24)| (SBOX[s48_0_1] << 16) | (SBOX[s48_0_2] << 8) | (SBOX[s48_0_3] << 0); // SBOX(input_0)
		ALU1_td[1] = (SBOX[s48_1_0] << 24)| (SBOX[s48_1_1] << 16) | (SBOX[s48_1_2] << 8) | (SBOX[s48_1_3] << 0); // SBOX(input_1)
		ALU1_td[2] = (SBOX[s48_2_0] << 24)| (SBOX[s48_2_1] << 16) | (SBOX[s48_2_2] << 8) | (SBOX[s48_2_3] << 0); // SBOX(input_2)
		ALU1_td[3] = (SBOX[s48_3_0] << 24)| (SBOX[s48_3_1] << 16) | (SBOX[s48_3_2] << 8) | (SBOX[s48_3_3] << 0); // SBOX(input_2)
		// printf("\n ALU1_td[0] = %08x\n",ALU1_td[0]);
		// printf("\n ALU1_td[1] = %08x\n",ALU1_td[1]);
		// printf("\n ALU1_td[2] = %08x\n",ALU1_td[2]);
		// printf("\n ALU1_td[3] = %08x\n",ALU1_td[3]);
		break;
	  case OP_MIXCOL: 
		MIXCOL_out = mixColumns(s48_in);
		ALU1_td[0] = MIXCOL_out[0];
		ALU1_td[1] = MIXCOL_out[1];
		ALU1_td[2] = MIXCOL_out[2];
		ALU1_td[3] = MIXCOL_out[3];
		break;
	  default:
		printf("Operation of OP1 is NOT supported!\n");
		break;
  }
  

   ///*** ALU2 ***///
   uint8_t OP2_LU1, OP2_LU2, OP2_LU3, OP2_SRU1, OP2_SRU2, OP2_SRU3;
   uint8_t SRU1_IM, SRU2_IM, SRU3_IM;
   uint8_t KEEP_OP1;
   
   uint32_t LU1_td, LU2_td, LU3_td, SRU1_td, SRU2_td, SRU3_td;
  
  KEEP_OP1 = (OP2 >> 31 ) & 0x1;
  OP2_LU1 = (OP2 >> 28) & 0x7;
  OP2_LU2 = (OP2 >> 25) & 0x7;
  OP2_SRU1 = (OP2 >> 23) & 0x3;
  SRU1_IM = (OP2 >> 18) & 0x1F;
  OP2_SRU2 = (OP2 >> 16) & 0x3;
  SRU2_IM = (OP2 >> 11) & 0x1F;
  OP2_SRU3 = (OP2 >> 9) & 0x3;
  OP2_LU3 = (OP2 >> 0) & 0x7;
  //printf("\nOP2 = %08x\n",OP2);
  if(OP2 == OP_SHROW){
		
		ALU2_td[0] = (ALU1_td[0]& 0xFF000000)|(ALU1_td[1]& 0xFF0000)|(ALU1_td[2]& 0xFF00)|(ALU1_td[3] & 0xFF);
		ALU2_td[1] = (ALU1_td[1]& 0xFF000000)|(ALU1_td[2]& 0xFF0000)|(ALU1_td[3]& 0xFF00)|(ALU1_td[0] & 0xFF);
		ALU2_td[2] = (ALU1_td[2]& 0xFF000000)|(ALU1_td[3]& 0xFF0000)|(ALU1_td[0]& 0xFF00)|(ALU1_td[1] & 0xFF);
		ALU2_td[3] = (ALU1_td[3]& 0xFF000000)|(ALU1_td[0]& 0xFF0000)|(ALU1_td[1]& 0xFF00)|(ALU1_td[2] & 0xFF);
		
		PE_out[0] = ALU2_td[0];
		PE_out[1] = ALU2_td[1];
		PE_out[2] = ALU2_td[2];
		PE_out[3] = ALU2_td[3];
  }
  else{
	if(OP2 != 0){
	  ALU1_td[1] = s48_1; // Mix Column multiplies fixed matrix against current State Matrix
	  ALU1_td[2] = s48_2; // Mix Column multiplies fixed matrix against current State Matrix
	  ALU1_td[3] = s48_3; // Mix Column multiplies fixed matrix against current State Matrix
	}
	  if(OP2 == 0)
		SRU3_IM = 32;
	  else
		SRU3_IM = (OP2 >> 3) & 0x3F;  
	   
	  //printf("\n OP2 = %08x\n",OP2);
	  //printf("\n OP2_LU1 = %d, OP2_LU2 = %d, OP2_SRU1 = %d, SRU1_IM = %d, OP2_SRU2 = %d, SRU2_IM = %d, OP2_SRU3 = %d, SRU3_IM = %d, OP2_LU3 = %d\n",OP2_LU1,OP2_LU2,OP2_SRU1,SRU1_IM,OP2_SRU2,SRU2_IM,OP2_SRU3,SRU3_IM,OP2_LU3);
	  // LU1	
	  switch (OP2_LU1) {
		  case OP_NOP:
			LU1_td = ALU1_td[0]; // pass through ALU1_td[0]
			break;
		  case OP_XOR:
			LU1_td = ALU1_td[0] ^ ALU1_td[3]; // ALU1_td[0] XOR ALU1_td[1]
			break;
		  case OP_OR:
			LU1_td = ALU1_td[0] | ALU1_td[3]; // ALU1_td[0] OR ALU1_td[1]
			break;
		  case OP_AND:
			LU1_td = ALU1_td[0] & ALU1_td[3]; // ALU1_td[0] AND ALU1_td[1]
			break;
		  case OP_NOT:
			LU1_td = ~ALU1_td[0]; // NOT ALU1_td[0]
			break;
		  case OP_NOT_XOR:
			LU1_td = ~ALU1_td[0] ^ ALU1_td[3]; // NOT ALU1_td[0] XOR ALU1_td[1]
			break;
		  case OP_NOT_OR:
			LU1_td = ~ALU1_td[0] | ALU1_td[3]; // NOT ALU1_td[0] OR ALU1_td[1]
			break;
		  case OP_NOT_AND:
			LU1_td = ~ALU1_td[0] & ALU1_td[3]; // NOT ALU1_td[0] AND ALU1_td[1]
			break;
		  default:
			printf("Operation of LU1 is NOT supported!\n");
			break;
	  }
	  
	   // LU2
	  switch (OP2_LU2) {
		  case OP_NOP:
			LU2_td = ALU1_td[1]; // pass through ALU1_td[2]
			break;
		  case OP_XOR:
			LU2_td = ALU1_td[1] ^ ALU1_td[2]; // ALU1_td[2] XOR ALU1_td[3]
			break;
		  case OP_OR:
			LU2_td = ALU1_td[1] | ALU1_td[2]; // ALU1_td[2] OR ALU1_td[3]
			break;
		  case OP_AND:
			LU2_td = ALU1_td[1] & ALU1_td[2]; // ALU1_td[2] AND ALU1_td[3]
			break;
		  case OP_NOT:
			LU2_td = ~ALU1_td[1]; // NOT ALU1_td[2]
			break;
		  case OP_NOT_XOR:
			LU2_td = ~ALU1_td[1] ^ ALU1_td[2]; // NOT ALU1_td[2] XOR ALU1_td[3]
			break;
		  case OP_NOT_OR:
			LU2_td = ~ALU1_td[1] | ALU1_td[2]; // NOT ALU1_td[2] OR ALU1_td[3]
			break;
		  case OP_NOT_AND:
			LU2_td = ~ALU1_td[1] & ALU1_td[2]; // NOT ALU1_td[2] AND ALU1_td[3]
			break;
		  default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	  }
	 
	  // SRU1
	  switch (OP2_SRU1) {
		  case (OP_NOP | OP_SHL):
			SRU1_td = LU1_td << SRU1_IM; // Shift left with SRU1_IM bit
			break;
		  case OP_SHR:
			SRU1_td = LU1_td >> SRU1_IM; // Shift right with SRU1_IM bit
			break;
		  case OP_ROL:
			SRU1_td = (LU1_td << SRU1_IM) | (LU1_td >> (32 - SRU1_IM)); // Rotate left with SRU1_IM bit
			break;
		  case OP_ROR:
			SRU1_td = (LU1_td >> SRU1_IM) | (LU1_td << (32 - SRU1_IM)); // Rotate left with SRU1_IM bit
			break;
		  default:
			printf("Operation of SRU1 is NOT supported!\n");
			break;
	  } 
	  
	  // SRU2
	  switch (OP2_SRU2) {
		  case (OP_NOP | OP_SHL):
			SRU2_td = LU2_td << SRU2_IM; // Shift left with SRU2_IM bit
			break;
		  case OP_SHR:
			SRU2_td = LU2_td >> SRU2_IM; // Shift right with SRU1_IM bit
			break;
		  case OP_ROL:
			SRU2_td = (LU2_td << SRU2_IM) | (LU2_td >> (32 - SRU2_IM)); // Rotate left with SRU2_IM bit
			break;
		  case OP_ROR:
			SRU2_td = (LU2_td >> SRU2_IM) | (LU2_td << (32 - SRU2_IM)); // Rotate left with SRU2_IM bit
			break;
		  default:
			printf("Operation of SRU2 is NOT supported!\n");
			break;
	  } 
	  
	  // SRU3
	  switch (OP2_SRU3) {
		  case (OP_NOP | OP_SHL):
			if(SRU3_IM == 32)
				SRU3_td = 0;
			else 
				SRU3_td = ALU1_td[3] << SRU3_IM; // Shift left with SRU2_IM bit
			break;
		  case OP_SHR:
			SRU3_td = ALU1_td[3] >> SRU3_IM; // Shift right with SRU1_IM bit
			break;
		  case OP_ROL:
			SRU3_td = (ALU1_td[3] << SRU3_IM) | (ALU1_td[3] >> (32 - SRU3_IM)); // Rotate left with SRU2_IM bit
			break;
		  case OP_ROR:
			SRU3_td = (ALU1_td[3] >> SRU3_IM) | (ALU1_td[3] << (32 - SRU3_IM)); // Rotate left with SRU2_IM bit
			break;
		  default:
			printf("Operation of SRU3 is NOT supported!\n");
			break;
	  } 
	  
	  // LU3
	  switch (OP2_LU3) {
		  case OP_NOP:
			LU3_td = SRU2_td; // pass through ALU1_td[2]
			break;
		  case OP_XOR:
			LU3_td = SRU1_td ^ SRU2_td; // ALU1_td[2] XOR ALU1_td[3]
			break;
		  case OP_OR:
			LU3_td = SRU1_td | SRU2_td; // ALU1_td[2] OR ALU1_td[3]
			break;
		  case OP_AND:
			LU3_td = SRU1_td & SRU2_td; // ALU1_td[2] AND ALU1_td[3]
			break;
		  case OP_NOT:
			LU3_td = ~SRU1_td; // NOT ALU1_td[2]
			break;
		  case OP_NOT_XOR:
			LU3_td = ~SRU1_td ^ SRU2_td; // NOT ALU1_td[2] XOR ALU1_td[3]
			break;
		  case OP_NOT_OR:
			LU3_td = ~SRU1_td | SRU2_td; // NOT ALU1_td[2] OR ALU1_td[3]
			break;
		  case OP_NOT_AND:
			LU3_td = ~SRU1_td & SRU2_td; // NOT ALU1_td[2] AND ALU1_td[3]
			break;
		  default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	  }
	  // if(OP2 == 0x3e000101){
		  // printf(" ALU2_td[1] = %08x\n",SRU3_td);
		  // printf("\n SRU3_IM = %08x\n",SRU3_IM);
	  // }
	  
	  if(PE_out){
		ALU2_td[0] = SRU1_td;
		ALU2_td[1] = LU3_td ^ SRU3_td;
		if(KEEP_OP1)
			ALU2_td[2] = ALU1_td[0];	
		else
			ALU2_td[2] = ALU1_td[2];
		ALU2_td[3] = ALU1_td[3];
		
		PE_out[0] = ALU2_td[0];
		PE_out[1] = ALU2_td[1];
		PE_out[2] = ALU2_td[2];
		PE_out[3] = ALU2_td[3];
	  }
	  else {
		  printf("There no ALU output!\n");
		  exit(0);
	  }
  }

#else
	printf("Please select the Platform: -ARMZYNQ or -ARMSIL!\n");
	exit(0);
 #endif
}
