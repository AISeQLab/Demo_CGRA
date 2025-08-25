
/* CGRA library                         */
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
#include "FPGA_Driver.c"		

/*******************************************************************************/
/*********************************** CGRA Interface ****************************/
/*******************************************************************************/

#if defined(MODE32)
	U32 LMM[NUM_PE][2][LMM_DEP];
	U32 PE_out[NUM_PE][NUM_PE_INOUT];
#elif defined(MODE64)	
	U64 LMM[NUM_PE][2][LMM_DEP];
	U64 PE_out[NUM_PE][NUM_PE_INOUT];
#endif

#define CTX_RC_File_Name "CTX_RC_File.txt"
#define CTX_PE_File_Name "CTX_PE_File.txt"
#define CTX_IM_File_Name "CTX_IM_File.txt"
#define LDM_File_Name "LDM_File.txt"
#define common_File_Name "common2.vh"

void Init(){
#if defined(ARMZYNQ)	
	for (int i = 0; i < NUM_PE/4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < NUM_PE_INOUT; k++){
				PE_out[i*4+j][k] = 64*i+NUM_PE_INOUT*j+k;
			}
		}
	}
	
	CGRA_info.PE_Counter = 0;
#elif defined(CHECKRULE)
	for (int i = 0; i < NUM_PE/4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < NUM_PE_INOUT; k++){
				PE_out[i*4+j][k] = 64*i+NUM_PE_INOUT*j+k+1;
			}
		}
	}
	CGRA_info.PE_Counter = 0;
	CGRA_info.Error_Counter = 0;
	CGRA_info.Warning_Counter = 0;
#elif defined(VIVADOSIL)
	for (int i = 0; i < NUM_PE/4; i++){
		for (int j = 0; j < 4; j++){
			for (int k = 0; k < NUM_PE_INOUT; k++){
				PE_out[i*4+j][k] = 64*i+NUM_PE_INOUT*j+k;
			}
		}
	}

	for (int i = 0; i < NUM_PE; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < LMM_DEP; k++)
				LMM[i][j][k] =0;
			
	CGRA_info.CTX_RC_File = fopen(CTX_RC_File_Name,"w");
	CGRA_info.CTX_PE_File = fopen(CTX_PE_File_Name,"w");
	CGRA_info.CTX_IM_File = fopen(CTX_IM_File_Name,"w");
	CGRA_info.LDM_File = fopen(LDM_File_Name,"w");
	CGRA_info.common_File = fopen(common_File_Name,"w");
	CGRA_info.PE_Counter = 0;
#elif defined(ARMSIL)
	for (int i = 0; i < NUM_PE; i++)
		for (int j = 0; j < NUM_PE_INOUT; j++)
			PE_out[i][j] = 0;
	
	for (int i = 0; i < NUM_PE; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < LMM_DEP; k++)
				LMM[i][j][k] =0;
			
	CGRA_info.PE_Counter = 0;
#endif
}

void Final(){
#if defined(ARMZYNQ)	
	

#elif defined(CHECKRULE)
	if(CGRA_info.Error_Counter == 0){
		printf("SUCCESS: There is no rule error!\n");
	}
	else {
		printf("WARINING: There is %d rule errors!\n",CGRA_info.Error_Counter);
	}
	if(CGRA_info.Warning_Counter == 0){
		printf("SUCCESS: There is no WARNING!\n");
	}
	else {
		printf("WARINING: There is %d WRANING!\n",CGRA_info.Warning_Counter);
	}

#elif defined(VIVADOSIL)
	fprintf(CGRA_info.common_File,"`define CTX_RC_DEPTH %d\n",CGRA_info.PE_Counter);	
	fprintf(CGRA_info.common_File,"`define CTX_PE_DEPTH %d\n",CGRA_info.PE_Counter);
	fprintf(CGRA_info.common_File,"`define CTX_IM_DEPTH %d\n",CGRA_info.PE_Counter);
#elif defined(ARMSIL)
	
#endif
}
#define ROTR(x, n) ((x >> n) | (x << (32 - n)))
#define SHL(x,n) (((x) & 0xFFFFFFFF) << n)
#define ROTL(x,n) (SHL((x),n) | ((x) >> (32 - n)))
#define P0(x) ((x) ^  ROTL((x),9) ^ ROTL((x),17))

#define ROTR64(a,b) (((a) >> (b)) | ((a) << (64-(b))))
///**For AES-like algorithms**///
const unsigned char sbox_8[256] = {
	0x65 , 0x4c , 0x6a , 0x42 , 0x4b , 0x63 , 0x43 , 0x6b , 0x55 , 0x75 , 0x5a , 0x7a , 0x53 , 0x73 , 
	0x5b , 0x7b , 0x35 , 0x8c , 0x3a , 0x81 , 0x89 , 0x33 , 0x80 , 0x3b , 0x95 , 0x25 , 0x98 , 0x2a , 
	0x90 , 0x23 , 0x99 , 0x2b , 0xe5 , 0xcc , 0xe8 , 0xc1 , 0xc9 , 0xe0 , 0xc0 , 0xe9 , 0xd5 , 0xf5 , 
	0xd8 , 0xf8 , 0xd0 , 0xf0 , 0xd9 , 0xf9 , 0xa5 , 0x1c , 0xa8 , 0x12 , 0x1b , 0xa0 , 0x13 , 0xa9 , 
	0x05 , 0xb5 , 0x0a , 0xb8 , 0x03 , 0xb0 , 0x0b , 0xb9 , 0x32 , 0x88 , 0x3c , 0x85 , 0x8d , 0x34 , 
	0x84 , 0x3d , 0x91 , 0x22 , 0x9c , 0x2c , 0x94 , 0x24 , 0x9d , 0x2d , 0x62 , 0x4a , 0x6c , 0x45 , 
	0x4d , 0x64 , 0x44 , 0x6d , 0x52 , 0x72 , 0x5c , 0x7c , 0x54 , 0x74 , 0x5d , 0x7d , 0xa1 , 0x1a , 
	0xac , 0x15 , 0x1d , 0xa4 , 0x14 , 0xad , 0x02 , 0xb1 , 0x0c , 0xbc , 0x04 , 0xb4 , 0x0d , 0xbd , 
	0xe1 , 0xc8 , 0xec , 0xc5 , 0xcd , 0xe4 , 0xc4 , 0xed , 0xd1 , 0xf1 , 0xdc , 0xfc , 0xd4 , 0xf4 , 
	0xdd , 0xfd , 0x36 , 0x8e , 0x38 , 0x82 , 0x8b , 0x30 , 0x83 , 0x39 , 0x96 , 0x26 , 0x9a , 0x28 , 
	0x93 , 0x20 , 0x9b , 0x29 , 0x66 , 0x4e , 0x68 , 0x41 , 0x49 , 0x60 , 0x40 , 0x69 , 0x56 , 0x76 , 
	0x58 , 0x78 , 0x50 , 0x70 , 0x59 , 0x79 , 0xa6 , 0x1e , 0xaa , 0x11 , 0x19 , 0xa3 , 0x10 , 0xab , 
	0x06 , 0xb6 , 0x08 , 0xba , 0x00 , 0xb3 , 0x09 , 0xbb , 0xe6 , 0xce , 0xea , 0xc2 , 0xcb , 0xe3 , 
	0xc3 , 0xeb , 0xd6 , 0xf6 , 0xda , 0xfa , 0xd3 , 0xf3 , 0xdb , 0xfb , 0x31 , 0x8a , 0x3e , 0x86 , 
	0x8f , 0x37 , 0x87 , 0x3f , 0x92 , 0x21 , 0x9e , 0x2e , 0x97 , 0x27 , 0x9f , 0x2f , 0x61 , 0x48 , 
	0x6e , 0x46 , 0x4f , 0x67 , 0x47 , 0x6f , 0x51 , 0x71 , 0x5e , 0x7e , 0x57 , 0x77 , 0x5f , 0x7f , 
	0xa2 , 0x18 , 0xae , 0x16 , 0x1f , 0xa7 , 0x17 , 0xaf , 0x01 , 0xb2 , 0x0e , 0xbe , 0x07 , 0xb7 , 
	0x0f , 0xbf , 0xe2 , 0xca , 0xee , 0xc6 , 0xcf , 0xe7 , 0xc7 , 0xef , 0xd2 , 0xf2 , 0xde , 0xfe , 
	0xd7 , 0xf7 , 0xdf , 0xff};
	
static const U8 SBOX[256] = {
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

static const U8 INV_SBOX[256] = {
	//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
	0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
	0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
	0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
	0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
	0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
	0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
	0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
	0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
	0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
	0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
	0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
	0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
	0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
	0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
	0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
	0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

static const unsigned char SboxTableSM4[16][16] = 
{
{0xd6,0x90,0xe9,0xfe,0xcc,0xe1,0x3d,0xb7,0x16,0xb6,0x14,0xc2,0x28,0xfb,0x2c,0x05},
{0x2b,0x67,0x9a,0x76,0x2a,0xbe,0x04,0xc3,0xaa,0x44,0x13,0x26,0x49,0x86,0x06,0x99},
{0x9c,0x42,0x50,0xf4,0x91,0xef,0x98,0x7a,0x33,0x54,0x0b,0x43,0xed,0xcf,0xac,0x62},
{0xe4,0xb3,0x1c,0xa9,0xc9,0x08,0xe8,0x95,0x80,0xdf,0x94,0xfa,0x75,0x8f,0x3f,0xa6},
{0x47,0x07,0xa7,0xfc,0xf3,0x73,0x17,0xba,0x83,0x59,0x3c,0x19,0xe6,0x85,0x4f,0xa8},
{0x68,0x6b,0x81,0xb2,0x71,0x64,0xda,0x8b,0xf8,0xeb,0x0f,0x4b,0x70,0x56,0x9d,0x35},
{0x1e,0x24,0x0e,0x5e,0x63,0x58,0xd1,0xa2,0x25,0x22,0x7c,0x3b,0x01,0x21,0x78,0x87},
{0xd4,0x00,0x46,0x57,0x9f,0xd3,0x27,0x52,0x4c,0x36,0x02,0xe7,0xa0,0xc4,0xc8,0x9e},
{0xea,0xbf,0x8a,0xd2,0x40,0xc7,0x38,0xb5,0xa3,0xf7,0xf2,0xce,0xf9,0x61,0x15,0xa1},
{0xe0,0xae,0x5d,0xa4,0x9b,0x34,0x1a,0x55,0xad,0x93,0x32,0x30,0xf5,0x8c,0xb1,0xe3},
{0x1d,0xf6,0xe2,0x2e,0x82,0x66,0xca,0x60,0xc0,0x29,0x23,0xab,0x0d,0x53,0x4e,0x6f},
{0xd5,0xdb,0x37,0x45,0xde,0xfd,0x8e,0x2f,0x03,0xff,0x6a,0x72,0x6d,0x6c,0x5b,0x51},
{0x8d,0x1b,0xaf,0x92,0xbb,0xdd,0xbc,0x7f,0x11,0xd9,0x5c,0x41,0x1f,0x10,0x5a,0xd8},
{0x0a,0xc1,0x31,0x88,0xa5,0xcd,0x7b,0xbd,0x2d,0x74,0xd0,0x12,0xb8,0xe5,0xb4,0xb0},
{0x89,0x69,0x97,0x4a,0x0c,0x96,0x77,0x7e,0x65,0xb9,0xf1,0x09,0xc5,0x6e,0xc6,0x84},
{0x18,0xf0,0x7d,0xec,0x3a,0xdc,0x4d,0x20,0x79,0xee,0x5f,0x3e,0xd7,0xcb,0x39,0x48}
};
U8 gf_mul(U8 a, U8 b) {
	U8 p = 0;
	U8 hi_bit_set;
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
		
U32 *mixColumns(U32 state[4]) {

	U8 state0[4], state1[4], state2[4], state3[4];
	U8 out0[4], out1[4], out2[4], out3[4];
	U32 *out;
	out = (U32*)calloc(4, sizeof(U32));
	
	state0[0] = (state[0] >> 24) &0xFF;
	state0[1] = (state[0] >> 16) &0xFF;
	state0[2] = (state[0] >> 8)  &0xFF;
	state0[3] = (state[0] >> 0)  &0xFF;
	
	state1[0] = (state[1] >> 24) &0xFF;
	state1[1] = (state[1] >> 16) &0xFF;
	state1[2] = (state[1] >> 8)  &0xFF;
	state1[3] = (state[1] >> 0)  &0xFF;
	
	state2[0] = (state[2] >> 24) &0xFF;
	state2[1] = (state[2] >> 16) &0xFF;
	state2[2] = (state[2] >> 8)  &0xFF;
	state2[3] = (state[2] >> 0)  &0xFF;
	
	state3[0] = (state[3] >> 24) &0xFF;
	state3[1] = (state[3] >> 16) &0xFF;
	state3[2] = (state[3] >> 8)  &0xFF;
	state3[3] = (state[3] >> 0)  &0xFF;
	
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

U32 *INV_MixColumns(U32 state[4]) {

	U8 state0[4], state1[4], state2[4], state3[4];
	U8 out0[4], out1[4], out2[4], out3[4];
	U32 *out;
	out = (U32*)calloc(4, sizeof(U32));
	
	int i;
	
	state0[0] = (state[0] >> 24) &0xFF;
	state0[1] = (state[0] >> 16) &0xFF;
	state0[2] = (state[0] >> 8)  &0xFF;
	state0[3] = (state[0] >> 0)  &0xFF;
	
	state1[0] = (state[1] >> 24) &0xFF;
	state1[1] = (state[1] >> 16) &0xFF;
	state1[2] = (state[1] >> 8)  &0xFF;
	state1[3] = (state[1] >> 0)  &0xFF;
	
	state2[0] = (state[2] >> 24) &0xFF;
	state2[1] = (state[2] >> 16) &0xFF;
	state2[2] = (state[2] >> 8)  &0xFF;
	state2[3] = (state[2] >> 0)  &0xFF;
	
	state3[0] = (state[3] >> 24) &0xFF;
	state3[1] = (state[3] >> 16) &0xFF;
	state3[2] = (state[3] >> 8)  &0xFF;
	state3[3] = (state[3] >> 0)  &0xFF;
	
	out0[0] = gf_mul(state0[0],0x0e)^gf_mul(state0[1],0x0b)^gf_mul(state0[2],0x0d)^gf_mul(state0[3],0x09);
	out1[0] = gf_mul(state1[0],0x0e)^gf_mul(state1[1],0x0b)^gf_mul(state1[2],0x0d)^gf_mul(state1[3],0x09);
	out2[0] = gf_mul(state2[0],0x0e)^gf_mul(state2[1],0x0b)^gf_mul(state2[2],0x0d)^gf_mul(state2[3],0x09);
	out3[0] = gf_mul(state3[0],0x0e)^gf_mul(state3[1],0x0b)^gf_mul(state3[2],0x0d)^gf_mul(state3[3],0x09);
	
	out0[1] = gf_mul(state0[0],0x09)^gf_mul(state0[1],0x0e)^gf_mul(state0[2],0x0b)^gf_mul(state0[3],0x0d);
	out1[1] = gf_mul(state1[0],0x09)^gf_mul(state1[1],0x0e)^gf_mul(state1[2],0x0b)^gf_mul(state1[3],0x0d);
	out2[1] = gf_mul(state2[0],0x09)^gf_mul(state2[1],0x0e)^gf_mul(state2[2],0x0b)^gf_mul(state2[3],0x0d);
	out3[1] = gf_mul(state3[0],0x09)^gf_mul(state3[1],0x0e)^gf_mul(state3[2],0x0b)^gf_mul(state3[3],0x0d);
	
	out0[2] = gf_mul(state0[0],0x0d)^gf_mul(state0[1],0x09)^gf_mul(state0[2],0x0e)^gf_mul(state0[3],0x0b);
	out1[2] = gf_mul(state1[0],0x0d)^gf_mul(state1[1],0x09)^gf_mul(state1[2],0x0e)^gf_mul(state1[3],0x0b);
	out2[2] = gf_mul(state2[0],0x0d)^gf_mul(state2[1],0x09)^gf_mul(state2[2],0x0e)^gf_mul(state2[3],0x0b);
	out3[2] = gf_mul(state3[0],0x0d)^gf_mul(state3[1],0x09)^gf_mul(state3[2],0x0e)^gf_mul(state3[3],0x0b);
	
	out0[3] = gf_mul(state0[0],0x0b)^gf_mul(state0[1],0x0d)^gf_mul(state0[2],0x09)^gf_mul(state0[3],0x0e);
	out1[3] = gf_mul(state1[0],0x0b)^gf_mul(state1[1],0x0d)^gf_mul(state1[2],0x09)^gf_mul(state1[3],0x0e);
	out2[3] = gf_mul(state2[0],0x0b)^gf_mul(state2[1],0x0d)^gf_mul(state2[2],0x09)^gf_mul(state2[3],0x0e);
	out3[3] = gf_mul(state3[0],0x0b)^gf_mul(state3[1],0x0d)^gf_mul(state3[2],0x09)^gf_mul(state3[3],0x0e);
	
	out[0] = (out0[0] << 24) | (out0[1] << 16) | (out0[2] << 8) | (out0[3] <<0);
	out[1] = (out1[0] << 24) | (out1[1] << 16) | (out1[2] << 8) | (out1[3] <<0);
	out[2] = (out2[0] << 24) | (out2[1] << 16) | (out2[2] << 8) | (out2[3] <<0);
	out[3] = (out3[0] << 24) | (out3[1] << 16) | (out3[2] << 8) | (out3[3] <<0);

	return out;
}

U32 subcell(U32 x){
    U32 v0 = 0;
    U32 v1 = 0;
    U32 v2 = 0;
    U32 v3 = 0;
    v0 = (sbox_8[(x&0xff000000)>>24] ^ v0) << 24; 
    v1 = (sbox_8[(x&0xff0000)>>16] ^ v1) << 16; 
    v2 = (sbox_8[(x&0xff00)>>8] ^ v2) << 8; 
    v3 = v3 ^ sbox_8[(x&0xff)] ; 
    return v0 = v0 ^ v1 ^ v2 ^ v3;
}
  
void  /*__attribute__((always_inline))*/
mop32(U8 OP, U32 base_addr, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3)
{
#if defined(ARMZYNQ)
	U64 OP_RC = 0;
	
	if((CGRA_info.ctx_rc_offset%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	*(CGRA_info.ctx_rc_mmap+CGRA_info.ctx_rc_offset) = OP_RC;
	CGRA_info.ctx_rc_offset = CGRA_info.ctx_rc_offset + 1;
	
	U32 OP_LSU;
	OP_LSU = (1<<31)|((OP&0x1)<<30)|(((base_addr)&0x3FF)<<20);
	
	*(CGRA_info.ctx_pe_mmap+CGRA_info.ctx_pe_offset) = OP_LSU;
	CGRA_info.ctx_pe_offset = CGRA_info.ctx_pe_offset + 1;
	
	*(CGRA_info.ctx_im_mmap+CGRA_info.ctx_im_offset) = 0;
	CGRA_info.ctx_im_offset = CGRA_info.ctx_im_offset + 1;

#elif defined(CHECKRULE)
	///*** Check Selection Rule ***///
	if((CGRA_info.PE_Counter%NUM_PE) < 4){
		if((s48_0!=0)&((s48_0 <192)|(s48_0 >240))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <192)|(s48_1 >240))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <192)|(s48_2 >240))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <192)|(s48_3 >240))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <192)|(b16_0 >240))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <192)|(b16_1 >240))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <192)|(b16_2 >240))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <192)|(b16_3 >240))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <192)|(b8_0 >240))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <192)|(b8_1 >240))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <192)|(b8_2 >240))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <192)|(b8_3 >240))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 4)&((CGRA_info.PE_Counter%NUM_PE) < 8)){
		if((s48_0!=0)&((s48_0 <1)|(s48_0 >48))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <1)|(s48_1 >48))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <1)|(s48_2 >48))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <1)|(s48_3 >48))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <1)|(b16_0 >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <1)|(b16_1 >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <1)|(b16_2 >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <1)|(b16_3 >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <1)|(b8_0 >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <1)|(b8_1 >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <1)|(b8_2 >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <1)|(b8_3 >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 8)&((CGRA_info.PE_Counter%NUM_PE) < 12)){
		if((s48_0!=0)&((s48_0 <65)|(s48_0 >112))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <65)|(s48_1 >112))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <65)|(s48_2 >112))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <65)|(s48_3 >112))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <65)|(b16_0 >112))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <65)|(b16_1 >112))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <65)|(b16_2 >112))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <65)|(b16_3 >112))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <65)|(b8_0 >112))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <65)|(b8_1 >112))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <65)|(b8_2 >112))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <65)|(b8_3 >112))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 12)&((CGRA_info.PE_Counter%NUM_PE) < 16)){
		if((s48_0!=0)&((s48_0 <129)|(s48_0 >176))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <129)|(s48_1 >176))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <129)|(s48_2 >176))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <129)|(s48_3 >176))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <129)|(b16_0 >176))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <129)|(b16_1 >176))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <129)|(b16_2 >176))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <129)|(b16_3 >176))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <129)|(b8_0 >176))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <129)|(b8_1 >176))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <129)|(b8_2 >176))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <129)|(b8_3 >176))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}	
	///*** Check Buffer Rule ***///
	if((CGRA_info.PE_Counter%4) == 0){
		if((b16_0!=0)&(((b16_0&0x3F) <1)|((b16_0&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <1)|((b16_1&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <1)|((b16_2&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <1)|((b16_3&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <5)|((b8_0&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <5)|((b8_1&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <5)|((b8_2&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <5)|((b8_3&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		if((b16_0!=0)&(((b16_0&0x3F) <13)|((b16_0&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <13)|((b16_1&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <13)|((b16_2&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <13)|((b16_3&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <17)|((b8_0&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <17)|((b8_1&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <17)|((b8_2&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <17)|((b8_3&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		if((b16_0!=0)&(((b16_0&0x3F) <25)|((b16_0&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <25)|((b16_1&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <25)|((b16_2&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <25)|((b16_3&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <29)|((b8_0&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <29)|((b8_1&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <29)|((b8_2&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <29)|((b8_3&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 3){
		if((b16_0!=0)&((((b16_0&0x3F) <37)&((b16_0&0x3F)>4))|((b16_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((((b16_1&0x3F) <37)&((b16_1&0x3F)>4))|((b16_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((((b16_2&0x3F) <37)&((b16_2&0x3F)>4))|((b16_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&((((b16_3&0x3F) <37)&((b16_3&0x3F)>4))|((b16_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <41)|((b8_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <41)|((b8_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <41)|((b8_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <41)|((b8_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
#elif defined(VIVADOSIL)
	U64 OP_RC = 0;

	if((CGRA_info.PE_Counter%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	    
	U32 OP_LSU;
	OP_LSU = (1<<31)|((OP&0x1)<<30)|(((base_addr)&0x3FF)<<20);

	fprintf(CGRA_info.CTX_RC_File,"%08x_%016lx\n",(U32)(CTX_RC_BASE_IP+CGRA_info.PE_Counter),OP_RC);	
	fprintf(CGRA_info.CTX_PE_File,"%08x_%08x\n",(U32)(CTX_PE_BASE_IP+CGRA_info.PE_Counter),OP_LSU);
	fprintf(CGRA_info.CTX_IM_File,"%08x_%016lx\n",(U32)(CTX_IM_BASE_IP+CGRA_info.PE_Counter),(U64)0);
    CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;		
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
	
	PE_out[2] = s48_2;
	PE_out[3] = s48_3;
  
	switch (OP) {
		case OP_LDW:
			PE_out[0] = LMM[CGRA_info.PE_Counter%NUM_PE][0][base_addr]; // load word
			PE_out[1] = LMM[CGRA_info.PE_Counter%NUM_PE][1][base_addr]; // load word
			break;
		case OP_STW:
			LMM[CGRA_info.PE_Counter%NUM_PE][0][base_addr] = s48_0; // input_0
			LMM[CGRA_info.PE_Counter%NUM_PE][1][base_addr] = s48_1; // input_0
			PE_out[0] = s48_0; // load word
			PE_out[1] = s48_1; // load word
			break;
		default:
			printf("Operation of OP is NOT supported!\n");
			break;
	}
	if((CGRA_info.PE_Counter%NUM_PE)==0){
		printf ("------------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",CGRA_info.PE_Counter/NUM_PE);
		printf ("------------------------------------------------------------------------------------------------------------------------\n");
	}
	if(CGRA_info.PE_Counter%NUM_PE<10)
		printf ("PE_out[%d]  =",CGRA_info.PE_Counter%NUM_PE);
	else
		printf ("PE_out[%d] =",CGRA_info.PE_Counter%NUM_PE);
	for (int j = 0; j < NUM_PE_INOUT; j++){
		// if((j < 4)&(PE_out[j] == 0)){
			// printf (" xxxxxxxx");
		// }else if(((j >= 4) & (j < 8))&(PE_out[j] == 0)){	
			// printf (" xxxxxxxx");
		// }else if(((j >= 8) & (j < 12))&(PE_out[j] == 0)){
			// printf (" xxxxxxxx");
		// }else{
			printf (" %08x",PE_out[j]);
		// }
	}
	printf ("\n");
	if((((CGRA_info.PE_Counter%NUM_PE)+1) % 4) == 0){
		printf ("------------------------------------------------------------------------------------------------------------------------\n");
	}
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
#else
	printf("Please select the Platform: -DCHECKRULE or -DARMSIL or -DVIVADOSIL or -DARMZYNQ!\n");
	exit(0);
 #endif
}

void  /*__attribute__((always_inline))*/
mop64(U8 OP, U32 base_addr, U64 *PE_out, /*ALU in*/ U64 s48_0, U64 s48_1, U64 s48_2, U64 s48_3, /*BUFF 16to1 in*/ U64 b16_0, U64 b16_1, U64 b16_2, U64 b16_3, /*BUFF 8to1 in*/ U64 b8_0, U64 b8_1, U64 b8_2, U64 b8_3)
{
   #if defined(ARMZYNQ)
	U64 OP_RC = 0;
	
	if((CGRA_info.ctx_rc_offset%4) == 0){
	 OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
	}
	else if((CGRA_info.ctx_rc_offset%4) == 1){
	 OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
	}
	else if((CGRA_info.ctx_rc_offset%4) == 2){
	 OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
	}
	else if((CGRA_info.ctx_rc_offset%4) == 3){
	 OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|((((b16_0|36)-36)&0xF)<<24)|((((b16_1|36)-36)&0xF)<<20)|((((b16_2|36)-36)&0xF)<<16)|((((b16_3|36)-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	*(CGRA_info.ctx_rc_mmap+CGRA_info.ctx_rc_offset) = OP_RC;
	CGRA_info.ctx_rc_offset = CGRA_info.ctx_rc_offset + 1;
	
	U32 OP_LSU;
	OP_LSU = (1<<31)|((OP&0x1)<<30)|(((base_addr)&0x3FF)<<20);
	
	*(CGRA_info.ctx_pe_mmap+CGRA_info.ctx_pe_offset) = OP_LSU;
	CGRA_info.ctx_pe_offset = CGRA_info.ctx_pe_offset + 1;
	
	*(CGRA_info.ctx_im_mmap+CGRA_info.ctx_im_offset) = 0;
	CGRA_info.ctx_im_offset = CGRA_info.ctx_im_offset + 1;
	
	

#elif defined(CHECKRULE)
	///*** Check Selection Rule ***///
	if((CGRA_info.PE_Counter%NUM_PE) < 4){
		if((s48_0!=0)&((s48_0 <192)|(s48_0 >240))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <192)|(s48_1 >240))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <192)|(s48_2 >240))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <192)|(s48_3 >240))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <192)|(b16_0 >240))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <192)|(b16_1 >240))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <192)|(b16_2 >240))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <192)|(b16_3 >240))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <192)|(b8_0 >240))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <192)|(b8_1 >240))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <192)|(b8_2 >240))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <192)|(b8_3 >240))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 4)&((CGRA_info.PE_Counter%NUM_PE) < 8)){
		if((s48_0!=0)&((s48_0 <1)|(s48_0 >48))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <1)|(s48_1 >48))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <1)|(s48_2 >48))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <1)|(s48_3 >48))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <1)|(b16_0 >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <1)|(b16_1 >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <1)|(b16_2 >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <1)|(b16_3 >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <1)|(b8_0 >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <1)|(b8_1 >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <1)|(b8_2 >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <1)|(b8_3 >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 8)&((CGRA_info.PE_Counter%NUM_PE) < 12)){
		if((s48_0!=0)&((s48_0 <65)|(s48_0 >112))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <65)|(s48_1 >112))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <65)|(s48_2 >112))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <65)|(s48_3 >112))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <65)|(b16_0 >112))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <65)|(b16_1 >112))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <65)|(b16_2 >112))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <65)|(b16_3 >112))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <65)|(b8_0 >112))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <65)|(b8_1 >112))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <65)|(b8_2 >112))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <65)|(b8_3 >112))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 12)&((CGRA_info.PE_Counter%NUM_PE) < 16)){
		if((s48_0!=0)&((s48_0 <129)|(s48_0 >176))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <129)|(s48_1 >176))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <129)|(s48_2 >176))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <129)|(s48_3 >176))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <129)|(b16_0 >176))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <129)|(b16_1 >176))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <129)|(b16_2 >176))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <129)|(b16_3 >176))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <129)|(b8_0 >176))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <129)|(b8_1 >176))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <129)|(b8_2 >176))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <129)|(b8_3 >176))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}	
	///*** Check Buffer Rule ***///
	if((CGRA_info.PE_Counter%4) == 0){
		if((b16_0!=0)&(((b16_0&0x3F) <1)|((b16_0&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <1)|((b16_1&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <1)|((b16_2&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <1)|((b16_3&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0 <5&0x3F))|((b8_0&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <5)|((b8_1&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <5)|((b8_2&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <5)|((b8_3&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		if((b16_0!=0)&(((b16_0&0x3F) <13)|((b16_0&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <13)|((b16_1&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <13)|((b16_2&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <13)|((b16_3&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <17)|((b8_0&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <17)|((b8_1&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <17)|((b8_2&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <17)|((b8_3&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		if((b16_0!=0)&(((b16_0&0x3F) <25)|((b16_0&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <25)|((b16_1&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <25)|((b16_2&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <25)|((b16_3&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <29)|((b8_0&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <29)|((b8_1&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <29)|((b8_2&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <29)|((b8_3&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 3){
		if((b16_0!=0)&((((b16_0&0x3F) <37)&((b16_0&0x3F)>4))|((b16_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((((b16_1&0x3F) <37)&((b16_1&0x3F)>4))|((b16_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((((b16_2&0x3F) <37)&((b16_2&0x3F)>4))|((b16_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&((((b16_3&0x3F) <37)&((b16_3&0x3F)>4))|((b16_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <41)|((b8_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <41)|((b8_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <41)|((b8_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <41)|((b8_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	  
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;	

#elif defined(VIVADOSIL)
	U64 OP_RC = 0;

	if((CGRA_info.PE_Counter%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	    
	U32 OP_LSU;
	OP_LSU = (1<<31)|((OP&0x1)<<30)|(((base_addr)&0x3FF)<<20);

	fprintf(CGRA_info.CTX_RC_File,"%08x_%016lx\n",(U32)(CTX_RC_BASE_IP+CGRA_info.PE_Counter),OP_RC);	
	fprintf(CGRA_info.CTX_PE_File,"%08x_%08x\n",(U32)(CTX_PE_BASE_IP+CGRA_info.PE_Counter),OP_LSU);
	fprintf(CGRA_info.CTX_IM_File,"%08x_%016lx\n",(U32)(CTX_IM_BASE_IP+CGRA_info.PE_Counter),(U64)0);
    CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
	
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
	
	PE_out[2] = s48_2;
	PE_out[3] = s48_3;
  
	switch (OP) {
		case OP_LDW:
			PE_out[0] = LMM[CGRA_info.PE_Counter%NUM_PE][0][base_addr]; // load word
			PE_out[1] = LMM[CGRA_info.PE_Counter%NUM_PE][1][base_addr]; // load word
			break;
		case OP_STW:
			LMM[CGRA_info.PE_Counter%NUM_PE][0][base_addr] = s48_0; // input_0
			LMM[CGRA_info.PE_Counter%NUM_PE][1][base_addr] = s48_1; // input_0
			PE_out[0] = s48_0; // load word
			PE_out[1] = s48_1; // load word
			break;
		default:
			printf("Operation of OP is NOT supported!\n");
			break;
	}
  
	if((CGRA_info.PE_Counter%NUM_PE)==0){
		printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		printf ("|   LOOP %d \n",CGRA_info.PE_Counter/NUM_PE);
		printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	}
	if(CGRA_info.PE_Counter%NUM_PE<10)
		printf ("PE_out[%d]  =",CGRA_info.PE_Counter%NUM_PE);
	else
		printf ("PE_out[%d] =",CGRA_info.PE_Counter%NUM_PE);
	
	for (int j = 0; j < NUM_PE_INOUT; j++){
		// if((j < 4)&(PE_out[j] == 0)){
			// printf (" xxxxxxxxxxxxxxxx");
		// }else if(((j >= 4) & (j < 8))&(PE_out[j] == 0)){	
			// printf (" xxxxxxxxxxxxxxxx");
		// }else if(((j >= 8) & (j < 12))&(PE_out[j] == 0)){
			// printf (" xxxxxxxxxxxxxxxx");
		// }else{
			printf (" %016lx",PE_out[j]);
		// }
	}
	printf ("\n");
	if((((CGRA_info.PE_Counter%NUM_PE)+1) % 4) == 0){
		printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
	}
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
#else
	printf("Please select the Platform: -DCHECKRULE or -DARMSIL or -DVIVADOSIL or -DARMZYNQ!\n");
	exit(0);
#endif
}

U32 BASIC_OP(/*AU*/U8 OP_AU,/*LU1*/U8 OP_LU1,/*LU2*/U8 OP_LU2,/*SRU1*/U8 OP_SRU1,/*SRU1_IM*/U8 SRU1_IM,/*SRU2*/U8 OP_SRU2,/*SRU2_IM*/U8 SRU2_IM,/*LU3*/U8 OP_LU3)
{
	U32 OP_ALU;
	U8 OP_AU_t, OP_LU1_t, OP_LU2_t, OP_LU3_t, OP_SRU1_t, OP_SRU2_t;
    U8 SRU1_IM_t, SRU2_IM_t;
	
	OP_AU_t   = OP_AU   & 0x3;
	OP_LU1_t  = OP_LU1  & 0x7;
	OP_LU2_t  = OP_LU2  & 0x7;
	OP_SRU1_t = OP_SRU1 & 0x3;
	SRU1_IM_t = SRU1_IM & 0x3F;
	OP_SRU2_t = OP_SRU2 & 0x3;
	SRU2_IM_t = SRU2_IM & 0x3F;
	OP_LU3_t  = OP_LU3  & 0x3;

    //printf("\n OP2_LU1 = %d, OP2_LU2 = %d, OP2_SRU1 = %d, SRU1_IM = %d, OP2_SRU2 = %d, SRU2_IM = %d, OP2_LU3 = %d\n",OP_LU1,OP_LU2,OP_SRU1,SRU1_IM,OP_SRU2,SRU2_IM,OP_LU3);
	OP_ALU = (0<<31)|(OP_AU_t<<29)|(OP_LU1_t<<26)|(OP_LU2_t<<23)|(OP_SRU1_t<<21)|(SRU1_IM_t<<15)|(OP_SRU2_t<<13)|(SRU2_IM_t<<7)|(OP_LU3_t<<5)|0x0;
	return OP_ALU;
}

U32 CUSTOM_OP(U8 OP_CUSTOM)
{
	U32 OP_ALU;
  
	OP_ALU = (0<<31)|(0<<6)|(OP_CUSTOM&0x1F);
	return OP_ALU;
}
	
void  /*__attribute__((always_inline))*/
exe32(U32 OP, U32 OP_IM, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3)
{
#if defined(ARMZYNQ)
	U64 OP_RC = 0;
	U64 OP_IM_64b = 0;
	
	if((CGRA_info.ctx_rc_offset%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	*(CGRA_info.ctx_rc_mmap+CGRA_info.ctx_rc_offset) = OP_RC;
	CGRA_info.ctx_rc_offset = CGRA_info.ctx_rc_offset + 1;
	
	
	*(CGRA_info.ctx_pe_mmap+CGRA_info.ctx_pe_offset) = OP;
	CGRA_info.ctx_pe_offset = CGRA_info.ctx_pe_offset + 1;

	OP_IM_64b = ((U64)OP_IM<<32)|OP_IM;
	*(CGRA_info.ctx_im_mmap+CGRA_info.ctx_im_offset) = OP_IM_64b;
	CGRA_info.ctx_im_offset = CGRA_info.ctx_im_offset + 1;
	
#elif defined(CHECKRULE)
	///*** Check Selection Rule ***///
	if((CGRA_info.PE_Counter%NUM_PE) < 4){
		if((s48_0!=0)&((s48_0 <192)|(s48_0 >240))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <192)|(s48_1 >240))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <192)|(s48_2 >240))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <192)|(s48_3 >240))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <192)|(b16_0 >240))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <192)|(b16_1 >240))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <192)|(b16_2 >240))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <192)|(b16_3 >240))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <192)|(b8_0 >240))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <192)|(b8_1 >240))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <192)|(b8_2 >240))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <192)|(b8_3 >240))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 4)&((CGRA_info.PE_Counter%NUM_PE) < 8)){
		if((s48_0!=0)&((s48_0 <1)|(s48_0 >48))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <1)|(s48_1 >48))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <1)|(s48_2 >48))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <1)|(s48_3 >48))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <1)|(b16_0 >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <1)|(b16_1 >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <1)|(b16_2 >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <1)|(b16_3 >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <1)|(b8_0 >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <1)|(b8_1 >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <1)|(b8_2 >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <1)|(b8_3 >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 8)&((CGRA_info.PE_Counter%NUM_PE) < 12)){
		if((s48_0!=0)&((s48_0 <65)|(s48_0 >112))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <65)|(s48_1 >112))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <65)|(s48_2 >112))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <65)|(s48_3 >112))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <65)|(b16_0 >112))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <65)|(b16_1 >112))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <65)|(b16_2 >112))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <65)|(b16_3 >112))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <65)|(b8_0 >112))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <65)|(b8_1 >112))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <65)|(b8_2 >112))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <65)|(b8_3 >112))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 12)&((CGRA_info.PE_Counter%NUM_PE) < 16)){
		if((s48_0!=0)&((s48_0 <129)|(s48_0 >176))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <129)|(s48_1 >176))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <129)|(s48_2 >176))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <129)|(s48_3 >176))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <129)|(b16_0 >176))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <129)|(b16_1 >176))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <129)|(b16_2 >176))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <129)|(b16_3 >176))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <129)|(b8_0 >176))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <129)|(b8_1 >176))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <129)|(b8_2 >176))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <129)|(b8_3 >176))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}	
	///*** Check Buffer Rule ***///
	if((CGRA_info.PE_Counter%4) == 0){
		if((b16_0!=0)&(((b16_0&0x3F) <1)|((b16_0&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <1)|((b16_1&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <1)|((b16_2&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <1)|((b16_3&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0 <5&0x3F))|((b8_0&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <5)|((b8_1&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <5)|((b8_2&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <5)|((b8_3&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		if((b16_0!=0)&(((b16_0&0x3F) <13)|((b16_0&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <13)|((b16_1&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <13)|((b16_2&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <13)|((b16_3&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <17)|((b8_0&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <17)|((b8_1&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <17)|((b8_2&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <17)|((b8_3&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		if((b16_0!=0)&(((b16_0&0x3F) <25)|((b16_0&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <25)|((b16_1&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <25)|((b16_2&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <25)|((b16_3&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <29)|((b8_0&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <29)|((b8_1&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <29)|((b8_2&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <29)|((b8_3&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 3){
		if((b16_0!=0)&((((b16_0&0x3F) <37)&((b16_0&0x3F)>4))|((b16_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((((b16_1&0x3F) <37)&((b16_1&0x3F)>4))|((b16_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((((b16_2&0x3F) <37)&((b16_2&0x3F)>4))|((b16_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&((((b16_3&0x3F) <37)&((b16_3&0x3F)>4))|((b16_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <41)|((b8_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <41)|((b8_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <41)|((b8_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <41)|((b8_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	

	U8 OP_AU, OP_LU1, OP_LU2, OP_LU3, OP_SRU1, OP_SRU2;
	U8 SRU1_IM, SRU2_IM;
	U8 OP_CUSTOM;
 
	/// AU, LU1/2/3, RSU1/2/3 Configuration Decoder
 
	OP_AU  	= (OP >> 29) & 0x3;
	OP_LU1 	= (OP >> 26) & 0x7;
	OP_LU2 	= (OP >> 23) & 0x7;
	OP_SRU1 = (OP >> 21) & 0x3;
	SRU1_IM = (OP >> 15) & 0x3F;
	OP_SRU2 = (OP >> 13) & 0x3;
	SRU2_IM = (OP >> 7)  & 0x3F;
	OP_LU3 	= (OP >> 5)  & 0x3;
	OP_CUSTOM = (U8)OP   & 0x1F;

	if((OP_AU==OP_ADD3)&&(OP_CUSTOM==0)&&(OP_IM!=0)&&((s48_0 == 0)||(s48_2 == 0))){
		printf("ERROR: In loop %d, the AU in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU==OP_ADD3)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_0 == 0)||(s48_1 == 0)||(s48_2 == 0))){
		printf("ERROR: In loop %d, the AU 1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU!=OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM!=0)&&(s48_0 == 0)){
		printf("ERROR: In loop %d, the AU 2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU!=OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_0 == 0)||(s48_1 == 0))){
		printf("ERROR: In loop %d, the AU 3 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} 	
	
	if((OP_LU1!= OP_NOP)&&(OP_CUSTOM==0)&&((s48_0 == 0)||(s48_2 == 0))){	
		printf("ERROR: In loop %d, the LU1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	
	if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_3 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	else if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_1 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_1 == 0)||(s48_3 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU1== OP_NOP)&&(SRU1_IM!=0)&&(s48_0 == 0)){	
		printf("ERROR: In loop %d, the SRU1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU2==OP_NOP)&&(OP_CUSTOM==0)&&(SRU2_IM!=0)&&(s48_3 == 0)){	
		printf("ERROR: In loop %d, the SRU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU1== OP_NOP)&&(SRU1_IM==0)&&(s48_0 == 0)&&(OP_LU2==OP_NOP)&&(OP_CUSTOM==0)&&(SRU2_IM!=0)&&(s48_3 == 0)&&(OP_LU3!= OP_NOP)){	
		printf("ERROR: In loop %d, the LU3 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
#elif defined(VIVADOSIL)
	U64 OP_RC = 0;

	if((CGRA_info.PE_Counter%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	fprintf(CGRA_info.CTX_RC_File,"%08x_%016lx\n",(U32)(CTX_RC_BASE_IP+CGRA_info.PE_Counter),OP_RC);	
	fprintf(CGRA_info.CTX_PE_File,"%08x_%08x\n",(U32)(CTX_PE_BASE_IP+CGRA_info.PE_Counter),OP);
	
	U64 OP_IM_64b;
	OP_IM_64b = ((U64)OP_IM<<32)|OP_IM;
	fprintf(CGRA_info.CTX_IM_File,"%08x_%016lx\n",(U32)(CTX_IM_BASE_IP+CGRA_info.PE_Counter),OP_IM_64b);
    CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
	
#elif defined(ARMSIL)

	U32 s48_1_t;  
	U32 t;
	U32 temp[4];
	U32 s48_in[4];
	U32 *MIXCOL_out;
	MIXCOL_out = (U32*)calloc(4, sizeof(U32));
	uint8_t addr[4];
	unsigned char *pTable;
	
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
	U32 *BASIC_EX1_td, *BASIC_EX2_td;
	U32 *CUSTOM_EX_tmp, *CUSTOM_EX_td;
	
	BASIC_EX1_td = (U32*)calloc(4, sizeof(U32));
	BASIC_EX2_td = (U32*)calloc(4, sizeof(U32));
	
	CUSTOM_EX_tmp = (U32*)calloc(4, sizeof(U32));
	CUSTOM_EX_td  = (U32*)calloc(4, sizeof(U32));
	
	U8 OP_AU, OP_LU1, OP_LU2, OP_LU3, OP_SRU1, OP_SRU2;
	U8 SRU1_IM, SRU2_IM;
	U8 OP_CUSTOM;
	
	U32 LU1_td, LU2_td, LU3_td, SRU1_td, SRU2_td;
 
	/// AU, LU1/2/3, RSU1/2/3 Configuration Decoder
 
	OP_AU  	= (OP >> 29) & 0x3;
	OP_LU1 	= (OP >> 26) & 0x7;
	OP_LU2 	= (OP >> 23) & 0x7;
	OP_SRU1 = (OP >> 21) & 0x3;
	SRU1_IM = (OP >> 15) & 0x3F;
	OP_SRU2 = (OP >> 13) & 0x3;
	SRU2_IM = (OP >> 7)  & 0x3F;
	OP_LU3 	= (OP >> 5)  & 0x3;
	OP_CUSTOM = (U8)OP   & 0x1F;
	
	if(OP_IM == 0) 
		s48_1_t = s48_1;
	else
		s48_1_t = OP_IM;
  
	// printf("\n OP2_LU1 = %d, OP2_LU2 = %d, OP2_SRU1 = %d, SRU1_IM = %d, OP2_SRU2 = %d, SRU2_IM = %d, OP2_LU3 = %d\n",OP_LU1,OP_LU2,OP_SRU1,SRU1_IM,OP_SRU2,SRU2_IM,OP_LU3);

	///*** EXE1 ***///
  		
	switch (OP_AU) {
		case OP_NOP:
			BASIC_EX1_td[0] = s48_0; // pass through input_0
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_ADD2:
			BASIC_EX1_td[0] = s48_0 + s48_1_t; // input_0 + input_1
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_ADD3:
			BASIC_EX1_td[0] = s48_0 + s48_1_t + s48_2; // input_0 + input_1 + input_2
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_SUB2:
			BASIC_EX1_td[0] = s48_0 - s48_1_t; // input_0 + input_1 + input_2
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;	  
		default:
			printf("Operation of OP1 is NOT supported!\n");
			break;
	}
  
   ///*** EXE2 ***///
	
	switch (OP_LU1) {
		case OP_NOP:
			LU1_td = BASIC_EX1_td[0]; // pass through BASIC_EX1_td[0]
			break;
		case OP_XOR:
			LU1_td = BASIC_EX1_td[0] ^ BASIC_EX1_td[2]; // BASIC_EX1_td[0] XOR BASIC_EX1_td[1]
			break;
		case OP_OR:
			LU1_td = BASIC_EX1_td[0] | BASIC_EX1_td[2]; // BASIC_EX1_td[0] OR BASIC_EX1_td[1]
			break;
		case OP_AND:
			LU1_td = BASIC_EX1_td[0] & BASIC_EX1_td[2]; // BASIC_EX1_td[0] AND BASIC_EX1_td[1]
			break;
		case OP_NOT:
			LU1_td = ~BASIC_EX1_td[0]; // NOT BASIC_EX1_td[0]
			break;
		case OP_NOT_XOR:
			LU1_td = ~BASIC_EX1_td[0] ^ BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] XOR BASIC_EX1_td[1]
			break;
		case OP_NOT_OR:
			LU1_td = ~BASIC_EX1_td[0] | BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] OR BASIC_EX1_td[1]
			break;
		case OP_NOT_AND:
			LU1_td = ~BASIC_EX1_td[0] & BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] AND BASIC_EX1_td[1]
			break;
		default:
			printf("Operation of LU1 is NOT supported!\n");
			break;
	}
	  
	   // LU2
	switch (OP_LU2) {
		case OP_NOP:
			LU2_td = BASIC_EX1_td[3]; // pass through BASIC_EX1_td[3]
			break;
		case OP_XOR:
			LU2_td = BASIC_EX1_td[1] ^ BASIC_EX1_td[3]; // BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_OR:
			LU2_td = BASIC_EX1_td[1] | BASIC_EX1_td[3]; // BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_AND:
			LU2_td = BASIC_EX1_td[1] & BASIC_EX1_td[3]; // BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		case OP_NOT:
			LU2_td = ~BASIC_EX1_td[1]; // NOT BASIC_EX1_td[2]
			break;
		case OP_NOT_XOR:
			LU2_td = ~BASIC_EX1_td[1] ^ BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_NOT_OR:
			LU2_td = ~BASIC_EX1_td[1] | BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_NOT_AND:
			LU2_td = ~BASIC_EX1_td[1] & BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	 
	  // SRU1
	switch (OP_SRU1) {
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
	switch (OP_SRU2) {
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
	  
	  // LU3
	switch (OP_LU3) {
		case OP_NOP:
			LU3_td = SRU2_td; // pass through BASIC_EX1_td[2]
			break;
		case OP_XOR:
			LU3_td = SRU1_td ^ SRU2_td; // BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_OR:
			LU3_td = SRU1_td | SRU2_td; // BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_AND:
			LU3_td = SRU1_td & SRU2_td; // BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	
    ////////*** Custom Function Unit ***/////////////	
	U8 s48_0_0, s48_0_1, s48_0_2, s48_0_3; // seperate input_0 into 4 bytes
	s48_0_0 = (s48_0 >> 24) & 0x000000FF; 
	s48_0_1 = (s48_0 >> 16) & 0x000000FF; 
	s48_0_2 = (s48_0 >> 8) & 0x000000FF; 
	s48_0_3 = (s48_0 >> 0) & 0x000000FF; 

	U8 s48_1_0, s48_1_1, s48_1_2, s48_1_3; // seperate input_1 into 4 bytes
	s48_1_0 = (s48_1 >> 24) & 0x000000FF; 
	s48_1_1 = (s48_1 >> 16) & 0x000000FF; 
	s48_1_2 = (s48_1 >> 8) & 0x000000FF; 
	s48_1_3 = (s48_1 >> 0) & 0x000000FF; 

	U8 s48_2_0, s48_2_1, s48_2_2, s48_2_3; // seperate input_2 into 4 bytes
	s48_2_0 = (s48_2 >> 24) & 0x000000FF; 
	s48_2_1 = (s48_2 >> 16) & 0x000000FF; 
	s48_2_2 = (s48_2 >> 8) & 0x000000FF; 
	s48_2_3 = (s48_2 >> 0) & 0x000000FF; 

	U8 s48_3_0, s48_3_1, s48_3_2, s48_3_3; // seperate input_3 into 4 bytes
	s48_3_0 = (s48_3 >> 24) & 0x000000FF; 
	s48_3_1 = (s48_3 >> 16) & 0x000000FF; 
	s48_3_2 = (s48_3 >> 8) & 0x000000FF; 
	s48_3_3 = (s48_3 >> 0) & 0x000000FF; 
	
	switch (OP_CUSTOM) {
		case OP_NOP:
			CUSTOM_EX_td[0] = s48_0; // pass through input_0
			CUSTOM_EX_td[1] = s48_1; // pass through input_1
			CUSTOM_EX_td[2] = s48_2; // pass through input_2
			CUSTOM_EX_td[3] = s48_3; // pass through input_3
			break;
	    case OP_GW3:
			CUSTOM_EX_td[0] = (SBOX[s48_0_1] << 24)| (SBOX[s48_0_2] << 16) | (SBOX[s48_0_3] << 8) | (SBOX[s48_0_0] << 0); // SBOX(input_0)
			CUSTOM_EX_td[1] =  s48_in[1]; // SBOX(input_1)
			CUSTOM_EX_td[2] =  s48_in[2]; // SBOX(input_2)
			CUSTOM_EX_td[3] =  s48_in[3]; // SBOX(input_2)
			break;
		case OP_SUB_SHF:
			CUSTOM_EX_tmp[0] = (SBOX[s48_0_0] << 24)| (SBOX[s48_0_1] << 16) | (SBOX[s48_0_2] << 8) | (SBOX[s48_0_3] << 0); // SBOX(input_0)
			CUSTOM_EX_tmp[1] = (SBOX[s48_1_0] << 24)| (SBOX[s48_1_1] << 16) | (SBOX[s48_1_2] << 8) | (SBOX[s48_1_3] << 0); // SBOX(input_1)
			CUSTOM_EX_tmp[2] = (SBOX[s48_2_0] << 24)| (SBOX[s48_2_1] << 16) | (SBOX[s48_2_2] << 8) | (SBOX[s48_2_3] << 0); // SBOX(input_2)
			CUSTOM_EX_tmp[3] = (SBOX[s48_3_0] << 24)| (SBOX[s48_3_1] << 16) | (SBOX[s48_3_2] << 8) | (SBOX[s48_3_3] << 0); // SBOX(input_2)
			
			CUSTOM_EX_td[0] = (CUSTOM_EX_tmp[0]& 0xFF000000)|(CUSTOM_EX_tmp[1]& 0xFF0000)|(CUSTOM_EX_tmp[2]& 0xFF00)|(CUSTOM_EX_tmp[3] & 0xFF);
			CUSTOM_EX_td[1] = (CUSTOM_EX_tmp[1]& 0xFF000000)|(CUSTOM_EX_tmp[2]& 0xFF0000)|(CUSTOM_EX_tmp[3]& 0xFF00)|(CUSTOM_EX_tmp[0] & 0xFF);
			CUSTOM_EX_td[2] = (CUSTOM_EX_tmp[2]& 0xFF000000)|(CUSTOM_EX_tmp[3]& 0xFF0000)|(CUSTOM_EX_tmp[0]& 0xFF00)|(CUSTOM_EX_tmp[1] & 0xFF);
			CUSTOM_EX_td[3] = (CUSTOM_EX_tmp[3]& 0xFF000000)|(CUSTOM_EX_tmp[0]& 0xFF0000)|(CUSTOM_EX_tmp[1]& 0xFF00)|(CUSTOM_EX_tmp[2] & 0xFF);
			break;
		case OP_MIXCOL:
			MIXCOL_out = mixColumns(s48_in);
			CUSTOM_EX_td[0] = MIXCOL_out[0];
			CUSTOM_EX_td[1] = MIXCOL_out[1];
			CUSTOM_EX_td[2] = MIXCOL_out[2];
			CUSTOM_EX_td[3] = MIXCOL_out[3];
			break;
		case OP_ISHF_ISUB:
			CUSTOM_EX_tmp[0] = (s48_in[0] & 0xFF000000) | (s48_in[3] & 0xFF0000) | (s48_in[2] & 0xFF00) | (s48_in[1] & 0xFF);
			CUSTOM_EX_tmp[1] = (s48_in[1] & 0xFF000000) | (s48_in[0] & 0xFF0000) | (s48_in[3] & 0xFF00) | (s48_in[2] & 0xFF);
			CUSTOM_EX_tmp[2] = (s48_in[2] & 0xFF000000) | (s48_in[1] & 0xFF0000) | (s48_in[0] & 0xFF00) | (s48_in[3] & 0xFF);
			CUSTOM_EX_tmp[3] = (s48_in[3] & 0xFF000000) | (s48_in[2] & 0xFF0000) | (s48_in[1] & 0xFF00) | (s48_in[0] & 0xFF);
			
			U8 CUSTOM_EX_0_0, CUSTOM_EX_0_1, CUSTOM_EX_0_2, CUSTOM_EX_0_3; // seperate input_0 into 4 bytes
			CUSTOM_EX_0_0 = (CUSTOM_EX_tmp[0] >> 24) & 0x000000FF; 
			CUSTOM_EX_0_1 = (CUSTOM_EX_tmp[0] >> 16) & 0x000000FF; 
			CUSTOM_EX_0_2 = (CUSTOM_EX_tmp[0] >> 8) & 0x000000FF; 
			CUSTOM_EX_0_3 = (CUSTOM_EX_tmp[0] >> 0) & 0x000000FF; 
			
			U8 CUSTOM_EX_1_0, CUSTOM_EX_1_1, CUSTOM_EX_1_2, CUSTOM_EX_1_3; // seperate input_1 into 4 bytes
			CUSTOM_EX_1_0 = (CUSTOM_EX_tmp[1] >> 24) & 0x000000FF; 
			CUSTOM_EX_1_1 = (CUSTOM_EX_tmp[1] >> 16) & 0x000000FF; 
			CUSTOM_EX_1_2 = (CUSTOM_EX_tmp[1] >> 8) & 0x000000FF; 
			CUSTOM_EX_1_3 = (CUSTOM_EX_tmp[1] >> 0) & 0x000000FF; 
			
			U8 CUSTOM_EX_2_0, CUSTOM_EX_2_1, CUSTOM_EX_2_2, CUSTOM_EX_2_3; // seperate input_2 into 4 bytes
			CUSTOM_EX_2_0 = (CUSTOM_EX_tmp[2] >> 24) & 0x000000FF; 
			CUSTOM_EX_2_1 = (CUSTOM_EX_tmp[2] >> 16) & 0x000000FF; 
			CUSTOM_EX_2_2 = (CUSTOM_EX_tmp[2] >> 8) & 0x000000FF; 
			CUSTOM_EX_2_3 = (CUSTOM_EX_tmp[2] >> 0) & 0x000000FF; 
			
			U8 CUSTOM_EX_3_0, CUSTOM_EX_3_1, CUSTOM_EX_3_2, CUSTOM_EX_3_3; // seperate input_3 into 4 bytes
			CUSTOM_EX_3_0 = (CUSTOM_EX_tmp[3] >> 24) & 0x000000FF; 
			CUSTOM_EX_3_1 = (CUSTOM_EX_tmp[3] >> 16) & 0x000000FF; 
			CUSTOM_EX_3_2 = (CUSTOM_EX_tmp[3] >> 8) & 0x000000FF; 
			CUSTOM_EX_3_3 = (CUSTOM_EX_tmp[3] >> 0) & 0x000000FF;  
			
			CUSTOM_EX_td[0] = (INV_SBOX[CUSTOM_EX_0_0] << 24)| (INV_SBOX[CUSTOM_EX_0_1] << 16) | (INV_SBOX[CUSTOM_EX_0_2] << 8) | (INV_SBOX[CUSTOM_EX_0_3] << 0); // SBOX(input_0)
			CUSTOM_EX_td[1] = (INV_SBOX[CUSTOM_EX_1_0] << 24)| (INV_SBOX[CUSTOM_EX_1_1] << 16) | (INV_SBOX[CUSTOM_EX_1_2] << 8) | (INV_SBOX[CUSTOM_EX_1_3] << 0); // SBOX(input_1)
			CUSTOM_EX_td[2] = (INV_SBOX[CUSTOM_EX_2_0] << 24)| (INV_SBOX[CUSTOM_EX_2_1] << 16) | (INV_SBOX[CUSTOM_EX_2_2] << 8) | (INV_SBOX[CUSTOM_EX_2_3] << 0); // SBOX(input_2)
			CUSTOM_EX_td[3] = (INV_SBOX[CUSTOM_EX_3_0] << 24)| (INV_SBOX[CUSTOM_EX_3_1] << 16) | (INV_SBOX[CUSTOM_EX_3_2] << 8) | (INV_SBOX[CUSTOM_EX_3_3] << 0); // SBOX(input_2)	
			break;
		case OP_IMIXCOL:
			MIXCOL_out = INV_MixColumns(s48_in);
			CUSTOM_EX_td[0] = MIXCOL_out[0];
			CUSTOM_EX_td[1] = MIXCOL_out[1];
			CUSTOM_EX_td[2] = MIXCOL_out[2];
			CUSTOM_EX_td[3] = MIXCOL_out[3];
			break;
		case OP_SUM01:
			CUSTOM_EX_td[0] = ROTR(s48_0, 2) ^ ROTR(s48_0, 13) ^ ROTR(s48_0, 22);
			CUSTOM_EX_td[1] = ROTR(s48_1, 6) ^ ROTR(s48_1, 11) ^ ROTR(s48_1, 25);
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_SIG01:
			CUSTOM_EX_td[0] = ROTR(s48_0, 7) ^ ROTR(s48_0, 18) ^ (s48_0 >> 3);
			CUSTOM_EX_td[1] = ROTR(s48_1, 17) ^ ROTR(s48_1, 19) ^ (s48_1 >> 10);
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_FF1:
			CUSTOM_EX_td[0] = ((s48_0) & (s48_1)) | ( (s48_0) & (s48_2)) | ( (s48_1) & (s48_2));
			CUSTOM_EX_td[1] = s48_1;
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_P0:
			CUSTOM_EX_td[0] = P0(s48_0);
			CUSTOM_EX_td[1] = s48_1;
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_SBOX_SM4:
			pTable = (unsigned char *)SboxTableSM4;
			addr[0] = pTable[(s48_0 & 0xff000000)>>24];
			addr[1] = pTable[(s48_0 & 0x00ff0000)>>16];
			addr[2] = pTable[(s48_0 & 0x0000ff00)>>8] ;
			addr[3] = pTable[(s48_0 & 0x000000ff)] 	;
		  	CUSTOM_EX_td[0] = (addr[0]<<24)|(addr[1]<<16)|(addr[2]<<8)|(addr[3]);
			CUSTOM_EX_td[1] = s48_1;
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_4_RX_SM4:
			CUSTOM_EX_td[0] = (ROTL(s48_0, 2))^(ROTL(s48_0, 10))^(ROTL(s48_0, 18))^(ROTL(s48_0, 24));
			CUSTOM_EX_td[1] = s48_1;
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_SBOX_8:
			CUSTOM_EX_td[0] = subcell(s48_0);
			CUSTOM_EX_td[1] = subcell(s48_1); 
			CUSTOM_EX_td[2] = subcell(s48_2); 
			CUSTOM_EX_td[3] = subcell(s48_3); 
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	  
	if(PE_out){		  
		BASIC_EX2_td[0] = BASIC_EX1_td[2];
		BASIC_EX2_td[1] = LU3_td;
		BASIC_EX2_td[2] = SRU1_td;	
		BASIC_EX2_td[3] = BASIC_EX1_td[1];
		if(OP_CUSTOM == 0){
			PE_out[0] = BASIC_EX2_td[0];
			PE_out[1] = BASIC_EX2_td[1];
			PE_out[2] = BASIC_EX2_td[2];
			PE_out[3] = BASIC_EX2_td[3];
		}
		else {
			PE_out[0] = CUSTOM_EX_td[0];
			PE_out[1] = CUSTOM_EX_td[1];
			PE_out[2] = CUSTOM_EX_td[2];
			PE_out[3] = CUSTOM_EX_td[3];
		}	
		if((CGRA_info.PE_Counter%NUM_PE)==0){
			printf ("------------------------------------------------------------------------------------------------------------------------\n");
			printf ("|   LOOP %d \n",CGRA_info.PE_Counter/NUM_PE);
			printf ("------------------------------------------------------------------------------------------------------------------------\n");
		}
		if(CGRA_info.PE_Counter%NUM_PE<10)
			printf ("PE_out[%d]  =",CGRA_info.PE_Counter%NUM_PE);
		else
			printf ("PE_out[%d] =",CGRA_info.PE_Counter%NUM_PE);
		for (int j = 0; j < NUM_PE_INOUT; j++){
			// if((j < 4)&(PE_out[j] == 0)){
				// printf (" xxxxxxxx");
			// }else if(((j >= 4) & (j < 8))&(PE_out[j] == 0)){	
				// printf (" xxxxxxxx");
			// }else if(((j >= 8) & (j < 12))&(PE_out[j] == 0)){
				// printf (" xxxxxxxx");
			// }else{
				printf (" %08x",PE_out[j]);
			// }
		}
		printf ("\n");
		if((((CGRA_info.PE_Counter%NUM_PE)+1) % 4) == 0){
			printf ("------------------------------------------------------------------------------------------------------------------------\n");
		}
		CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
		}
		else {
		printf("There no ALU output!\n");
		exit(0);
	}

#else
	printf("Please select the Platform: -ARMZYNQ or -ARMSIL!\n");
	exit(0);
 #endif
}

void  /*__attribute__((always_inline))*/
exe64(U32 OP, U64 OP_IM, U64 *PE_out, /*ALU in*/ U64 s48_0, U64 s48_1, U64 s48_2, U64 s48_3, /*BUFF 16to1 in*/ U64 b16_0, U64 b16_1, U64 b16_2, U64 b16_3, /*BUFF 8to1 in*/ U64 b8_0, U64 b8_1, U64 b8_2, U64 b8_3)
{
#if defined(ARMZYNQ)
	U64 OP_RC = 0;

	if((CGRA_info.ctx_rc_offset%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.ctx_rc_offset%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	
	*(CGRA_info.ctx_rc_mmap+CGRA_info.ctx_rc_offset) = OP_RC;
	CGRA_info.ctx_rc_offset = CGRA_info.ctx_rc_offset + 1;
	
	*(CGRA_info.ctx_pe_mmap+CGRA_info.ctx_pe_offset) = OP;
	CGRA_info.ctx_pe_offset = CGRA_info.ctx_pe_offset + 1;
	
	*(CGRA_info.ctx_im_mmap+CGRA_info.ctx_im_offset) = OP_IM;
	CGRA_info.ctx_im_offset = CGRA_info.ctx_im_offset + 1;
#elif defined(CHECKRULE)
	///*** Check Selection Rule ***///
	if((CGRA_info.PE_Counter%NUM_PE) < 4){
		if((s48_0!=0)&((s48_0 <192)|(s48_0 >240))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <192)|(s48_1 >240))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <192)|(s48_2 >240))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <192)|(s48_3 >240))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <192)|(b16_0 >240))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <192)|(b16_1 >240))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <192)|(b16_2 >240))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <192)|(b16_3 >240))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <192)|(b8_0 >240))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <192)|(b8_1 >240))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <192)|(b8_2 >240))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <192)|(b8_3 >240))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 4)&((CGRA_info.PE_Counter%NUM_PE) < 8)){
		if((s48_0!=0)&((s48_0 <1)|(s48_0 >48))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <1)|(s48_1 >48))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <1)|(s48_2 >48))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <1)|(s48_3 >48))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <1)|(b16_0 >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <1)|(b16_1 >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <1)|(b16_2 >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <1)|(b16_3 >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <1)|(b8_0 >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <1)|(b8_1 >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <1)|(b8_2 >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <1)|(b8_3 >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 8)&((CGRA_info.PE_Counter%NUM_PE) < 12)){
		if((s48_0!=0)&((s48_0 <65)|(s48_0 >112))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <65)|(s48_1 >112))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <65)|(s48_2 >112))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <65)|(s48_3 >112))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <65)|(b16_0 >112))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <65)|(b16_1 >112))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <65)|(b16_2 >112))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <65)|(b16_3 >112))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <65)|(b8_0 >112))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <65)|(b8_1 >112))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <65)|(b8_2 >112))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <65)|(b8_3 >112))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}
	else if(((CGRA_info.PE_Counter%NUM_PE) >= 12)&((CGRA_info.PE_Counter%NUM_PE) < 16)){
		if((s48_0!=0)&((s48_0 <129)|(s48_0 >176))){
			printf("ERROR: In loop %d, the s48_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_1!=0)&((s48_1 <129)|(s48_1 >176))){
			printf("ERROR: In loop %d, the s48_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((s48_2!=0)&((s48_2 <129)|(s48_2 >176))){
			printf("ERROR: In loop %d, the s48_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((s48_3!=0)&((s48_3 <129)|(s48_3 >176))){
			printf("ERROR: In loop %d, the s48_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_0!=0)&((b16_0 <129)|(b16_0 >176))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((b16_1 <129)|(b16_1 >176))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((b16_2 <129)|(b16_2 >176))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b16_3!=0)&((b16_3 <129)|(b16_3 >176))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_0!=0)&((b8_0 <129)|(b8_0 >176))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_1!=0)&((b8_1 <129)|(b8_1 >176))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_2!=0)&((b8_2 <129)|(b8_2 >176))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    }
		if((b8_3!=0)&((b8_3 <129)|(b8_3 >176))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is outside the scope of the previous row!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	    } 
	}	
	///*** Check Buffer Rule ***///
	if((CGRA_info.PE_Counter%4) == 0){
		if((b16_0!=0)&(((b16_0&0x3F) <1)|((b16_0&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <1)|((b16_1&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <1)|((b16_2&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <1)|((b16_3&0x3F) >16))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0 <5&0x3F))|((b8_0&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <5)|((b8_1&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <5)|((b8_2&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <5)|((b8_3&0x3F) >12))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		if((b16_0!=0)&(((b16_0&0x3F) <13)|((b16_0&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <13)|((b16_1&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <13)|((b16_2&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <13)|((b16_3&0x3F) >28))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <17)|((b8_0&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <17)|((b8_1&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <17)|((b8_2&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <17)|((b8_3&0x3F) >25))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		if((b16_0!=0)&(((b16_0&0x3F) <25)|((b16_0&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&(((b16_1&0x3F) <25)|((b16_1&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&(((b16_2&0x3F) <25)|((b16_2&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&(((b16_3&0x3F) <25)|((b16_3&0x3F) >40))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <29)|((b8_0&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <29)|((b8_1&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <29)|((b8_2&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <29)|((b8_3&0x3F) >36))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	else if((CGRA_info.PE_Counter%4) == 3){
		if((b16_0!=0)&((((b16_0&0x3F) <37)&((b16_0&0x3F)>4))|((b16_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_1!=0)&((((b16_1&0x3F) <37)&((b16_1&0x3F)>4))|((b16_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_2!=0)&((((b16_2&0x3F) <37)&((b16_2&0x3F)>4))|((b16_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b16_3!=0)&((((b16_3&0x3F) <37)&((b16_3&0x3F)>4))|((b16_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b16_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_0!=0)&(((b8_0&0x3F) <41)|((b8_0&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_0 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_1!=0)&(((b8_1&0x3F) <41)|((b8_1&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_1 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_2!=0)&(((b8_2&0x3F) <41)|((b8_2&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_2 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
		if((b8_3!=0)&(((b8_3&0x3F) <41)|((b8_3&0x3F) >48))){
			printf("ERROR: In loop %d, the b8_3 in PE[%d] received input that is out of scope!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
			CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
		}
	}
	
	U8 OP_AU, OP_LU1, OP_LU2, OP_LU3, OP_SRU1, OP_SRU2;
	U8 SRU1_IM, SRU2_IM;
	U8 OP_CUSTOM;
 
	/// AU, LU1/2/3, RSU1/2/3 Configuration Decoder
 
	OP_AU  	= (OP >> 29) & 0x3;
	OP_LU1 	= (OP >> 26) & 0x7;
	OP_LU2 	= (OP >> 23) & 0x7;
	OP_SRU1 = (OP >> 21) & 0x3;
	SRU1_IM = (OP >> 15) & 0x3F;
	OP_SRU2 = (OP >> 13) & 0x3;
	SRU2_IM = (OP >> 7)  & 0x3F;
	OP_LU3 	= (OP >> 5)  & 0x3;
	OP_CUSTOM = (U8)OP   & 0x1F;

	if((OP_AU==OP_ADD3)&&(OP_CUSTOM==0)&&(OP_IM!=0)&&((s48_0 == 0)||(s48_2 == 0))){
		printf("ERROR: In loop %d, the AU in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU==OP_ADD3)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_0 == 0)||(s48_1 == 0)||(s48_2 == 0))){
		printf("ERROR: In loop %d, the AU 1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU!=OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM!=0)&&(s48_0 == 0)){
		printf("ERROR: In loop %d, the AU 2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_AU!=OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_0 == 0)||(s48_1 == 0))){
		printf("ERROR: In loop %d, the AU 3 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} 	
	
	if((OP_LU1!= OP_NOP)&&(OP_CUSTOM==0)&&((s48_0 == 0)||(s48_2 == 0))){	
		printf("ERROR: In loop %d, the LU1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	
	if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_3 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	else if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_1 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	} else if((OP_LU2!= OP_NOP)&&(OP_CUSTOM==0)&&(OP_IM==0)&&((s48_1 == 0)||(s48_3 == 0))){
		printf("ERROR: In loop %d, the LU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU1== OP_NOP)&&(SRU1_IM!=0)&&(s48_0 == 0)){	
		printf("ERROR: In loop %d, the SRU1 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU2==OP_NOP)&&(OP_CUSTOM==0)&&(SRU2_IM!=0)&&(s48_3 == 0)){	
		printf("ERROR: In loop %d, the SRU2 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}

	if((OP_LU1== OP_NOP)&&(SRU1_IM==0)&&(s48_0 == 0)&&(OP_LU2==OP_NOP)&&(OP_CUSTOM==0)&&(SRU2_IM!=0)&&(s48_3 == 0)&&(OP_LU3!= OP_NOP)){	
		printf("ERROR: In loop %d, the LU3 in PE[%d] used the arbitrary value for computation!\n", (CGRA_info.PE_Counter/NUM_PE),(CGRA_info.PE_Counter%NUM_PE));
		CGRA_info.Error_Counter = CGRA_info.Error_Counter + 1;
	}
	  
	CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
#elif defined(VIVADOSIL)
	U64 OP_RC = 0;

	if((CGRA_info.PE_Counter%4) == 0){  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0)&0xF)<<24)|(((b16_1)&0xF)<<20)|(((b16_2)&0xF)<<16)|(((b16_3)&0xF)<<12)|(((b8_0-4)&0x7)<<9)|(((b8_1-4)&0x7)<<6)|(((b8_2-4)&0x7)<<3)|(((b8_3-4)&0x7)<<0);
		//printf("PE 0|4|8|12 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0)&0xF),((b16_1)&0xF),((b16_2)&0xF),((b16_3)&0xF),((b8_0-4)&0x7),((b8_1-4)&0x7),((b8_2-4)&0x7),((b8_3-4)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 1){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-12)&0xF)<<24)|(((b16_1-12)&0xF)<<20)|(((b16_2-12)&0xF)<<16)|(((b16_3-12)&0xF)<<12)|(((b8_0-16)&0x7)<<9)|(((b8_1-16)&0x7)<<6)|(((b8_2-16)&0x7)<<3)|(((b8_3-16)&0x7)<<0);
		//printf("PE 1|5|9|13 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-12)&0xF),((b16_1-12)&0xF),((b16_2-12)&0xF),((b16_3-12)&0xF),((b8_0-16)&0x7),((b8_1-16)&0x7),((b8_2-16)&0x7),((b8_3-16)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 2){
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0-24)&0xF)<<24)|(((b16_1-24)&0xF)<<20)|(((b16_2-24)&0xF)<<16)|(((b16_3-24)&0xF)<<12)|(((b8_0-28)&0x7)<<9)|(((b8_1-28)&0x7)<<6)|(((b8_2-28)&0x7)<<3)|(((b8_3-28)&0x7)<<0);
		//printf("PE 2|6|10|14 = /*ALU*/ %d, %d, %d, %d, /*BUFF 16*/ %d, %d, %d, %d, /*BUFF 8*/ %d, %d, %d, %d\n",((U64)s48_0&0x3F),((U64)s48_1&0x3F),((U64)s48_2&0x3F),((U64)s48_3&0x3F),((b16_0-24)&0xF),((b16_1-24)&0xF),((b16_2-24)&0xF),((b16_3-24)&0xF),((b8_0-28)&0x7),((b8_1-28)&0x7),((b8_2-28)&0x7),((b8_3-28)&0x7));
	}
	else if((CGRA_info.PE_Counter%4) == 3){
	    
		U64 /*BUFF 8to1 in*/ b16_0_t, b16_1_t, b16_2_t, b16_3_t;
		if(b16_0 < 4){
			b16_0_t = b16_0 + 48;
		}
		else {
			b16_0_t = b16_0;
		}
	  
		if(b16_1 < 4){
			b16_1_t = b16_1 + 48;
		}
		else {
			b16_1_t = b16_1;
		}
	  
		if(b16_2 < 4){
			b16_2_t = b16_2 + 48;
		}
		else {
			b16_2_t = b16_2;
		}
		if(b16_3 < 4){
			b16_3_t = b16_3 + 48;
		}
		else {
			b16_3_t = b16_3;
		}
	  
		OP_RC = (((U64)s48_0&0x3F)<<46)|(((U64)s48_1&0x3F)<<40)|(((U64)s48_2&0x3F)<<34)|(((U64)s48_3&0x3F)<<28)|(((b16_0_t-36)&0xF)<<24)|(((b16_1_t-36)&0xF)<<20)|(((b16_2_t-36)&0xF)<<16)|(((b16_3_t-36)&0xF)<<12)|(((b8_0-40)&0x7)<<9)|(((b8_1-40)&0x7)<<6)|(((b8_2-40)&0x7)<<3)|(((b8_3-40)&0x7)<<0);
	}
	
	fprintf(CGRA_info.CTX_RC_File,"%08x_%016lx\n",(U32)(CTX_RC_BASE_IP+CGRA_info.PE_Counter),OP_RC);	
	fprintf(CGRA_info.CTX_PE_File,"%08x_%08x\n",(U32)(CTX_PE_BASE_IP+CGRA_info.PE_Counter),OP);
	fprintf(CGRA_info.CTX_IM_File,"%08x_%016lx\n",(U32)(CTX_IM_BASE_IP+CGRA_info.PE_Counter),OP_IM);
    CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;	

#elif defined(ARMSIL)

	U64 s48_1_t;  
	U64 t;
	U64 temp[4];
	U64 s48_in[4];
	
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
	PE_out[10] = b8_2;
	PE_out[11] = b8_3;
	
	///**** ALU Architecture ****///
	U64 *BASIC_EX1_td, *BASIC_EX2_td;
	U64 *CUSTOM_EX_tmp, *CUSTOM_EX_td;
	
	BASIC_EX1_td = (U64*)calloc(4, sizeof(U64));
	BASIC_EX2_td = (U64*)calloc(4, sizeof(U64));
	
	CUSTOM_EX_tmp = (U64*)calloc(4, sizeof(U64));
	CUSTOM_EX_td  = (U64*)calloc(4, sizeof(U64));
	
	U8 OP_AU, OP_LU1, OP_LU2, OP_LU3, OP_SRU1, OP_SRU2;
	U8 SRU1_IM, SRU2_IM;
	U8 OP_CUSTOM;
	
	U64 LU1_td, LU2_td, LU3_td, SRU1_td, SRU2_td;
	// AU, LU1/2/3, RSU1/2/3 Configuration Decoder
	
	OP_AU  	= (OP >> 29) & 0x3;
	OP_LU1 	= (OP >> 26) & 0x7;
	OP_LU2 	= (OP >> 23) & 0x7;
	OP_SRU1 = (OP >> 21) & 0x3;
	SRU1_IM = (OP >> 15) & 0x3F;
	OP_SRU2 = (OP >> 13) & 0x3;
	SRU2_IM = (OP >> 7)  & 0x3F;
	OP_LU3 	= (OP >> 5)  & 0x3;
	OP_CUSTOM = (U8)OP   & 0x1F;
	
	if(OP_IM == 0) 
		s48_1_t = s48_1;
	else
		s48_1_t = OP_IM;
	
	//printf("\n OP2_LU1 = %d, OP2_LU2 = %d, OP2_SRU1 = %d, SRU1_IM = %d, OP2_SRU2 = %d, SRU2_IM = %d, OP2_LU3 = %d\n",OP_LU1,OP_LU2,OP_SRU1,SRU1_IM,OP_SRU2,SRU2_IM,OP_LU3);
	
	///*** EXE1 ***///
			
	switch (OP_AU) {
		case OP_NOP:
			BASIC_EX1_td[0] = s48_0; // pass through input_0
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_ADD2:
			BASIC_EX1_td[0] = s48_0 + s48_1_t; // input_0 + input_1
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_ADD3:
			BASIC_EX1_td[0] = s48_0 + s48_1_t + s48_2; // input_0 + input_1 + input_2
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;
		case OP_SUB2:
			BASIC_EX1_td[0] = s48_0 - s48_1_t; // input_0 + input_1 + input_2
			BASIC_EX1_td[1] = s48_1_t; // pass through input_1
			BASIC_EX1_td[2] = s48_2; // pass through input_2
			BASIC_EX1_td[3] = s48_3; // pass through input_3
			break;	  
		default:
			printf("Operation of OP1 is NOT supported!\n");
			break;
	}
  
   ///*** EXE2 ***///
	
	// LU1
	switch (OP_LU1) {
		case OP_NOP:
			LU1_td = BASIC_EX1_td[0]; // pass through BASIC_EX1_td[0]
			break;
		case OP_XOR:
			LU1_td = BASIC_EX1_td[0] ^ BASIC_EX1_td[2]; // BASIC_EX1_td[0] XOR BASIC_EX1_td[1]
			break;
		case OP_OR:
			LU1_td = BASIC_EX1_td[0] | BASIC_EX1_td[2]; // BASIC_EX1_td[0] OR BASIC_EX1_td[1]
			break;
		case OP_AND:
			LU1_td = BASIC_EX1_td[0] & BASIC_EX1_td[2]; // BASIC_EX1_td[0] AND BASIC_EX1_td[1]
			break;
		case OP_NOT:
			LU1_td = ~BASIC_EX1_td[0]; // NOT BASIC_EX1_td[0]
			break;
		case OP_NOT_XOR:
			LU1_td = ~BASIC_EX1_td[0] ^ BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] XOR BASIC_EX1_td[1]
			break;
		case OP_NOT_OR:
			LU1_td = ~BASIC_EX1_td[0] | BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] OR BASIC_EX1_td[1]
			break;
		case OP_NOT_AND:
			LU1_td = ~BASIC_EX1_td[0] & BASIC_EX1_td[2]; // NOT BASIC_EX1_td[0] AND BASIC_EX1_td[1]
			break;
		default:
			printf("Operation of LU1 is NOT supported!\n");
			break;
	}
	  
	// LU2
	switch (OP_LU2) {
		case OP_NOP:
			LU2_td = BASIC_EX1_td[3]; // pass through BASIC_EX1_td[3]
			break;
		case OP_XOR:
			LU2_td = BASIC_EX1_td[1] ^ BASIC_EX1_td[3]; // BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_OR:
			LU2_td = BASIC_EX1_td[1] | BASIC_EX1_td[3]; // BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_AND:
			LU2_td = BASIC_EX1_td[1] & BASIC_EX1_td[3]; // BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		case OP_NOT:
			LU2_td = ~BASIC_EX1_td[1]; // NOT BASIC_EX1_td[2]
			break;
		case OP_NOT_XOR:
			LU2_td = ~BASIC_EX1_td[1] ^ BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_NOT_OR:
			LU2_td = ~BASIC_EX1_td[1] | BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_NOT_AND:
			LU2_td = ~BASIC_EX1_td[1] & BASIC_EX1_td[3]; // NOT BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	 
	// SRU1
	switch (OP_SRU1) {
		case (OP_NOP | OP_SHL):
			SRU1_td = LU1_td << SRU1_IM; // Shift left with SRU1_IM bit
			break;
		case OP_SHR:
			SRU1_td = LU1_td >> SRU1_IM; // Shift right with SRU1_IM bit
			break;
		case OP_ROL:
			SRU1_td = (LU1_td << SRU1_IM) | (LU1_td >> (64 - SRU1_IM)); // Rotate left with SRU1_IM bit
			break;
		case OP_ROR:
			SRU1_td = (LU1_td >> SRU1_IM) | (LU1_td << (64 - SRU1_IM)); // Rotate left with SRU1_IM bit
			break;
		default:
			printf("Operation of SRU1 is NOT supported!\n");
			break;
	} 
	  
	// SRU2
	switch (OP_SRU2) {
		case (OP_NOP | OP_SHL):
			SRU2_td = LU2_td << SRU2_IM; // Shift left with SRU2_IM bit
			break;
		case OP_SHR:
			SRU2_td = LU2_td >> SRU2_IM; // Shift right with SRU1_IM bit
			break;
		case OP_ROL:
			SRU2_td = (LU2_td << SRU2_IM) | (LU2_td >> (64 - SRU2_IM)); // Rotate left with SRU2_IM bit
			break;
		case OP_ROR:
			SRU2_td = (LU2_td >> SRU2_IM) | (LU2_td << (64 - SRU2_IM)); // Rotate left with SRU2_IM bit
			break;
		default:
			printf("Operation of SRU2 is NOT supported!\n");
			break;
	} 
	  
	// LU3
	switch (OP_LU3) {
		case OP_NOP:
			LU3_td = SRU2_td; // pass through BASIC_EX1_td[2]
			break;
		case OP_XOR:
			LU3_td = SRU1_td ^ SRU2_td; // BASIC_EX1_td[2] XOR BASIC_EX1_td[3]
			break;
		case OP_OR:
			LU3_td = SRU1_td | SRU2_td; // BASIC_EX1_td[2] OR BASIC_EX1_td[3]
			break;
		case OP_AND:
			LU3_td = SRU1_td & SRU2_td; // BASIC_EX1_td[2] AND BASIC_EX1_td[3]
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	
    ////////*** Custom Function Unit ***/////////////	

	switch (OP_CUSTOM) {
		case OP_NOP:
			CUSTOM_EX_td[0] = s48_0; // pass through input_0
			CUSTOM_EX_td[1] = s48_1; // pass through input_1
			CUSTOM_EX_td[2] = s48_2; // pass through input_2
			CUSTOM_EX_td[3] = s48_3; // pass through input_3
			break;
		case OP_SUM01:
			CUSTOM_EX_td[0] = ROTR64(s48_0, 28) ^ ROTR64(s48_0, 34) ^ ROTR64(s48_0, 39);
			CUSTOM_EX_td[1] = ROTR64(s48_1, 14) ^ ROTR64(s48_1, 18) ^ ROTR64(s48_1, 41);
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		case OP_SIG01:
			CUSTOM_EX_td[0] = ROTR64(s48_0, 1) ^ ROTR64(s48_0, 8) ^ (s48_0 >> 7);
			CUSTOM_EX_td[1] = ROTR64(s48_1, 19) ^ ROTR64(s48_1, 61) ^ (s48_1 >> 6);
			CUSTOM_EX_td[2] = s48_2;
			CUSTOM_EX_td[3] = s48_3;
			break;
		default:
			printf("Operation of LU2 is NOT supported!\n");
			break;
	}
	  
	if(PE_out){		  
		BASIC_EX2_td[0] = BASIC_EX1_td[2];
		BASIC_EX2_td[1] = LU3_td;
		BASIC_EX2_td[2] = SRU1_td;	
		BASIC_EX2_td[3] = BASIC_EX1_td[1];
		if(OP_CUSTOM == 0){
			PE_out[0] = BASIC_EX2_td[0];
			PE_out[1] = BASIC_EX2_td[1];
			PE_out[2] = BASIC_EX2_td[2];
			PE_out[3] = BASIC_EX2_td[3];
		}
		else {
			PE_out[0] = CUSTOM_EX_td[0];
			PE_out[1] = CUSTOM_EX_td[1];
			PE_out[2] = CUSTOM_EX_td[2];
			PE_out[3] = CUSTOM_EX_td[3];
		}	
		if((CGRA_info.PE_Counter%NUM_PE)==0){
			printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			printf ("|   LOOP %d \n",CGRA_info.PE_Counter/NUM_PE);
			printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		}
		if(CGRA_info.PE_Counter%NUM_PE<10)
			printf ("PE_out[%d]  =",CGRA_info.PE_Counter%NUM_PE);
		else
			printf ("PE_out[%d] =",CGRA_info.PE_Counter%NUM_PE);
		for (int j = 0; j < NUM_PE_INOUT; j++){
			// if((j < 4)&(PE_out[j] == 0)){
				// printf (" xxxxxxxxxxxxxxxx");
			// }else if(((j >= 4) & (j < 8))&(PE_out[j] == 0)){	
				// printf (" xxxxxxxxxxxxxxxx");
			// }else if(((j >= 8) & (j < 12))&(PE_out[j] == 0)){
				// printf (" xxxxxxxxxxxxxxxx");
			// }else{
				printf (" %016lx",PE_out[j]);
			// }
		}
		printf ("\n");
		if((((CGRA_info.PE_Counter%NUM_PE)+1) % 4) == 0){
			printf ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
		}
		CGRA_info.PE_Counter = CGRA_info.PE_Counter + 1;
	}
	else {
		printf("There no ALU output!\n");
		exit(0);
	}

#else
	printf("Please select the Platform: -DCHECKRULE or -DARMSIL or -DVIVADOSIL or -DARMZYNQ!\n");
	exit(0);
 #endif
}