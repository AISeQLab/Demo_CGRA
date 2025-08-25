#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

#include "../CGRA.h"
#include "../CGRAlib.c"

#define N 4
////// FOR CPU COMPUTATION/////////

typedef unsigned int  WORD;            

typedef struct {
	WORD data[16];
	WORD datalen;
	unsigned long long bitlen;
	WORD state[8];
} SHA256_CTX;

/****************************** MACROS ******************************/
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

/**************************** VARIABLES *****************************/
static const WORD k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static const WORD H[8] = { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};	
	
/*********************** FUNCTION DEFINITIONS ***********************/
void sha256_transform_1(SHA256_CTX *ctx, const WORD data[])
{
	WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

	//Update W
	for (i = 0; i < 16; ++i){
		m[i] = data[i];
	}
	for ( ; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
		t2 = EP0(a) + MAJ(a,b,c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void sha256_init_1(SHA256_CTX *ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_update_1(SHA256_CTX *ctx, const WORD data[], WORD hash[])
{
	WORD i;

	for (i = 0; i < 16; ++i) {
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
	}
	sha256_transform_1(ctx, ctx ->data);
		
		hash[0] = ctx->state[0];
		hash[1] = ctx->state[1];
		hash[2] = ctx->state[2];
		hash[3] = ctx->state[3];
		hash[4] = ctx->state[4];
		hash[5] = ctx->state[5];
		hash[6] = ctx->state[6];
		hash[7] = ctx->state[7];
}
///////////////////////////////////////////////////////

int main(){

	unsigned char* membase;
	if (cgra_open() == 0)
		exit(1);
	
	cgra.dma_ctrl = CGRA_info.dma_mmap;
	membase = (unsigned char*) CGRA_info.ddr_mmap;
		/////CPU COMPUTATION/////////
	
	FILE *file1 = fopen("datatest_input.txt", "r");  // Open file in read mode
    if (file1 == NULL) {
        perror("ERROR: Unable to open datatest_input.txt file!");
        return 1;
    }
	
	FILE *file2 = fopen("Output_CPU.txt", "w");  // Open file in read mode
    if (file2 == NULL) {
        perror("Unable to create Output_CPU.txt file");
        return 1;
    }

	FILE *file3 = fopen("datatest_input.txt", "r");  // Open file in read mode
    if (file3 == NULL) {
        perror("ERROR: Unable to open datatest_input.txt file!");
        return 1;
    }
	
	FILE *file4 = fopen("Output_CGRA.txt", "w");  // Open file in read mode
    if (file4 == NULL) {
        perror("Unable to create Output_CGRA.txt file");
        return 1;
    }	
	SHA256_CTX ctx;
	uint32_t Data[16];
	uint32_t Output[8];
	uint32_t value;
	int i = 0;
	int j = 0;
	printf("*****************************************************************************\n");
	printf("First 16 Outputs from CPU Calculation\n");
	while (fscanf(file1, "%8x", &value) == 1) {  

		Data[j] = value;
		i++;
		j++;
		if((i%16 ==0)&&(i!=0)){
			sha256_init_1(&ctx);
			sha256_update_1(&ctx, Data, Output);
			for (int k=0; k<8; k++) {
				fprintf(file2,"%08x", Output[k]);
			}
			fprintf(file2,"\n");
			if((i/16)<17){
				printf("Output[%d] = ", (i/16)-1);
				for (int k=0; k<8; k++) {
					printf("%08x", Output[k]);
				}
				printf("\n");
			}
			j = 0;
		}
    }
	fclose(file1);
	fclose(file2);
	printf("*****************************************************************************\n");
	printf("Write successfully %d outputs from CPU calculation to Output_CPU.txt file!\n",i/16);
	printf("*****************************************************************************\n");
	printf("\n");
	/////////////////////////////
	int k, LOOP;
	Init();
	
	LOOP = 67;
	U32 OP;
	
	U32 OP_IM;

	uint32_t H[8] = {0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	
	uint32_t W[8][16] = {
		{0x61626380,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626480,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626580,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626680,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626780,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626880,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626980,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018},
		{0x61626a80,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0, 0x00000018}
	};

	//uint32_t W[16] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf, 0x10};
	//uint32_t W[16] = {0x71776572,0x74797569,0x6f706173,0x64666768,0x6a6b6c7a,0x78637662,0x6e6d3132,0x33343536,0x37383971,0x77657274,0x79637662,0x6e800000,0x00000000,0x00000000,0x00000000,0x00000168};
	static const uint32_t K[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
	};

	LMM[0][0][0] = H[0];
	LMM[0][1][0] = H[1];
	
	LMM[1][0][0] = H[2];
	LMM[1][1][0] = H[3];

	LMM[2][0][0] = H[4];
	LMM[2][1][0] = H[5];
	
	LMM[3][0][0] = H[6];
	LMM[3][1][0] = H[7];

	for(i=0;i<8;i++){
		LMM[4][0][i] = W[i][0];
		LMM[4][1][i] = W[i][1];
		
		LMM[5][0][i] = W[i][2];
		LMM[5][1][i] = W[i][3];

		LMM[6][0][i] = W[i][4];
		LMM[6][1][i] = W[i][5];
		
		LMM[7][0][i] = W[i][6];
		LMM[7][1][i] = W[i][7];	
		
		LMM[8][0][i] = W[i][8];
		LMM[8][1][i] = W[i][9];
		
		LMM[9][0][i] = W[i][10];
		LMM[9][1][i] = W[i][11];

		LMM[10][0][i] = W[i][12];
		LMM[10][1][i] = W[i][13];
		
		LMM[11][0][i] = W[i][14];
		LMM[11][1][i] = W[i][15];
	}
	#if defined(ARMZYNQ)
		U32 *A_MSB, *A_LSB;
		A_MSB = (U32*)(membase + ROW0_MSB_BASE_PHYS);
		A_LSB = (U32*)(membase + ROW0_LSB_BASE_PHYS);
		U32 *B_MSB, *B_LSB;
		B_MSB = (U32*)(membase + ROW1_MSB_BASE_PHYS);
		B_LSB = (U32*)(membase + ROW1_LSB_BASE_PHYS);
		U32 *C_MSB, *C_LSB;
		C_MSB = (U32*)(membase + ROW2_MSB_BASE_PHYS);
		C_LSB = (U32*)(membase + ROW2_LSB_BASE_PHYS);
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

		CGRA_info.LDM_Offset = 0;
		
		for (i = 0; i < 8; i ++){	
			//fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[8][0][0],LMM[8][1][0],LMM[9][0][0],LMM[9][1][0],LMM[10][0][0],LMM[10][1][0],LMM[11][0][0],LMM[11][1][0]);
			fprintf(CGRA_info.LDM_File,"%08x_%08x%08x%08x%08x%08x%08x%08x%08x\n",ROW2_BASE_PHYS+CGRA_info.LDM_Offset,LMM[11][0][0],LMM[11][1][0],LMM[10][0][0],LMM[10][1][0],LMM[9][0][0],LMM[9][1][0],LMM[8][0][0],LMM[8][1][0]);
			CGRA_info.LDM_Offset = CGRA_info.LDM_Offset+32;
		}
		
		CGRA_info.LDM_Offset = 0;	

	fprintf(CGRA_info.common_File,"`define LDM_DEPTH %d\n",24);		
	#endif
	for (i = 0; i < LOOP; i++){
		if( i == 0){
			///*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 1 ***///
			mop32(OP_LDW, 0, PE_out[4], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[0][0], PE_out[0][1], PE_out[1][0], PE_out[1][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[5], /*ALU in*/ 0,0,PE_out[2][0], PE_out[2][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[6], /*ALU in*/ 0,0,PE_out[3][0], PE_out[3][1], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[7], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			///*** row 2 ***///
			mop32(OP_LDW, 0, PE_out[8],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[4][4], PE_out[4][5],PE_out[4][6], PE_out[4][7], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[9],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[5][2], PE_out[5][3], PE_out[6][2], PE_out[6][3], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[10], /*ALU in*/ 0,0,PE_out[4][0], PE_out[4][1], /*BUFF 16to1 in*/ PE_out[6][0], PE_out[6][1], PE_out[7][0], PE_out[7][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[11], /*ALU in*/ 0,0,PE_out[5][0], PE_out[5][1], /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ 0, 0, 0, 0);			///*** row 3 ***///
			///*** row 3 ***///

			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ PE_out[8][0],0,0,PE_out[8][1], /*BUFF 16to1 in*/ 0,0,0,PE_out[8][4], /*BUFF 8to1 in*/ PE_out[8][5], PE_out[8][6], PE_out[8][7], 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][4], /*BUFF 8to1 in*/ PE_out[9][5], PE_out[9][6], PE_out[9][7], 0);
			exe32(OP_NOP, 0, PE_out[14], /*ALU in*/ 0,0,0,0, /*BUFF 16to1 in*/ PE_out[10][2], PE_out[10][3], PE_out[11][2], PE_out[11][3], /*BUFF 8to1 in*/ PE_out[10][4], PE_out[10][5], PE_out[10][6], PE_out[10][7]);
			exe32(OP_NOP, 0, PE_out[15], /*ALU in*/ PE_out[9][0], PE_out[9][1],PE_out[10][0],PE_out[10][1], /*BUFF 16to1 in*/ 0, 0, PE_out[11][0], PE_out[11][1], /*BUFF 8to1 in*/ 0, 0, 0, 0);		
			}
		else if(i == 65){
			// /*** row 0 ***///
			mop32(OP_LDW, 0, PE_out[0], /*ALU in*/ 0,0,PE_out[12][2],PE_out[12][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[1], /*ALU in*/ 0,0,PE_out[12][9],PE_out[12][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[2],/*ALU in*/ 0,0,PE_out[13][7],PE_out[13][8], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_LDW, 0, PE_out[3],/*ALU in*/ 0,0,PE_out[13][9],PE_out[13][10], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 1 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][0],PE_out[0][2],PE_out[0][1],PE_out[0][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[1][0],PE_out[1][2],PE_out[1][1],PE_out[1][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[2][0],PE_out[2][2],PE_out[2][1],PE_out[2][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[7],/*ALU in*/ PE_out[3][0],PE_out[3][2],PE_out[3][1],PE_out[3][3], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][0],PE_out[4][1], 0, PE_out[4][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[5][0],PE_out[5][1], 0, PE_out[5][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[6][0],PE_out[6][1], 0, PE_out[6][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][0],PE_out[7][1], 0, PE_out[7][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, PE_out[8][1], PE_out[8][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, PE_out[9][1], PE_out[9][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0,PE_out[10][1],PE_out[10][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0,PE_out[11][1],PE_out[11][2], /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else if(i == 66){
			mop32(OP_STW, 0, PE_out[0], /*ALU in*/ PE_out[12][0],PE_out[12][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[1], /*ALU in*/ PE_out[13][0],PE_out[13][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[2], /*ALU in*/ PE_out[14][0],PE_out[14][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			mop32(OP_STW, 0, PE_out[3], /*ALU in*/ PE_out[15][0],PE_out[15][1], 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			
			// /*** row 1 ***///
			exe32(OP_NOP, 0, PE_out[4],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[5],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[6],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 2 ***///
			exe32(OP_NOP, 0, PE_out[8],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[9],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[10],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[11],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			// /*** row 3 ***///
			exe32(OP_NOP, 0, PE_out[12],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0, 0, 0, 0, /*BUFF 8to1 in*/ 0, 0, 0, 0);
		}
		else{
			///*** row 0 ***///
			if( i == 1) {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][4],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][7], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][7],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][11], PE_out[14][10], PE_out[14][9], PE_out[14][8], /*BUFF 8to1 in*/ PE_out[14][7], PE_out[14][6], PE_out[14][5], PE_out[14][4]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][5],PE_out[15][6], PE_out[15][0], PE_out[15][1], /*BUFF 16to1 in*/ PE_out[12][2], PE_out[12][1], PE_out[15][2], PE_out[15][3], /*BUFF 8to1 in*/ 0, 0, PE_out[15][6], PE_out[15][7]);

				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][5],PE_out[2][11], 0, 0, /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][4], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][11], PE_out[3][10], PE_out[3][3], PE_out[3][2], /*BUFF 8to1 in*/ PE_out[3][7], PE_out[3][6], PE_out[3][5], PE_out[3][4]);
			}
			else {
				/*** row 0 ***///
				/**/OP_IM = K[i-1];
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, OP_IM, PE_out[0],/*ALU in*/ PE_out[13][10],0,PE_out[14][11],0, /*BUFF 16to1 in*/ 0,0,0,PE_out[12][2], /*BUFF 8to1 in*/ PE_out[12][8], PE_out[12][9], PE_out[12][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_NOT_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[1],/*ALU in*/ PE_out[13][7],PE_out[13][7],PE_out[13][8],PE_out[13][9], /*BUFF 16to1 in*/ 0,0,0,PE_out[13][7], /*BUFF 8to1 in*/ PE_out[13][8], PE_out[13][9], PE_out[13][10], PE_out[13][11]);
				/*--*/OP = CUSTOM_OP(OP_SUM01);
				exe32(OP, 0, PE_out[2],/*ALU in*/ PE_out[12][2],PE_out[13][7],0,0, /*BUFF 16to1 in*/ PE_out[14][4], PE_out[14][5], PE_out[14][6], PE_out[14][7], /*BUFF 8to1 in*/ PE_out[14][8], PE_out[14][9], PE_out[14][10], PE_out[14][11]);
				/*--*/OP = CUSTOM_OP(OP_SIG01);
				exe32(OP, 0, PE_out[3],/*ALU in*/ PE_out[14][10],PE_out[15][5], 0, 0, /*BUFF 16to1 in*/ PE_out[15][4], PE_out[15][5], PE_out[15][6], PE_out[15][7], /*BUFF 8to1 in*/ PE_out[15][8], PE_out[15][9], PE_out[15][10], PE_out[15][11]);
			
				/*** row 1 ***///
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[4],/*ALU in*/ PE_out[0][2],PE_out[1][1], PE_out[2][1], PE_out[2][0], /*BUFF 16to1 in*/ PE_out[0][2],0,0,PE_out[0][7], /*BUFF 8to1 in*/ PE_out[0][8], PE_out[0][9], PE_out[0][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_AND,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
				exe32(OP, 0, PE_out[5],/*ALU in*/ PE_out[0][7],PE_out[0][7],PE_out[0][8],PE_out[0][9], /*BUFF 16to1 in*/ PE_out[1][2],0,0,PE_out[1][7], /*BUFF 8to1 in*/ PE_out[1][8], PE_out[1][9], PE_out[1][10], 0);
				/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
				exe32(OP, 0, PE_out[6],/*ALU in*/ PE_out[3][10],PE_out[2][11], 0, PE_out[3][11], /*BUFF 16to1 in*/ PE_out[2][4], PE_out[2][5], PE_out[2][6], PE_out[2][7], /*BUFF 8to1 in*/ PE_out[2][8], PE_out[2][9], PE_out[2][10], PE_out[2][11]);
				exe32(OP_NOP, 0, PE_out[7],/*ALU in*/ PE_out[3][0], 0, PE_out[3][11], PE_out[3][1], /*BUFF 16to1 in*/ PE_out[3][4], PE_out[3][5], PE_out[3][6], PE_out[3][7], /*BUFF 8to1 in*/ PE_out[3][8], PE_out[3][9], PE_out[3][10], PE_out[3][11]);

			}
			/*** row 2 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[8],/*ALU in*/ PE_out[4][2],PE_out[4][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[4][7], PE_out[4][8], PE_out[4][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[9],/*ALU in*/ PE_out[4][2],PE_out[4][10], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[5][7], PE_out[5][8], PE_out[5][9], 0);
			/*--*/OP = BASIC_OP(/*AU*/OP_NOP,/*LU1*/OP_NOP,/*LU2*/OP_AND,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_XOR);
			exe32(OP, 0, PE_out[10],/*ALU in*/ PE_out[5][1],PE_out[4][8],0,PE_out[4][9], /*BUFF 16to1 in*/ PE_out[7][0],PE_out[6][4],PE_out[6][5],PE_out[6][6], /*BUFF 8to1 in*/ PE_out[6][7], PE_out[6][8], PE_out[6][9], PE_out[6][10]);
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD3,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[11],/*ALU in*/ PE_out[7][2],PE_out[7][1],PE_out[6][2],0, /*BUFF 16to1 in*/ 0,PE_out[7][4],PE_out[7][5],PE_out[7][6], /*BUFF 8to1 in*/ PE_out[7][7], PE_out[7][8], PE_out[7][9], PE_out[7][10]);
			/*** row 3 ***///
			/*--*/OP = BASIC_OP(/*AU*/OP_ADD2,/*LU1*/OP_NOP,/*LU2*/OP_NOP,/*SRU1*/OP_NOP,/*SRU1_IM*/OP_NOP,/*SRU2*/OP_NOP,/*SRU2_IM*/OP_NOP,/*LU3*/OP_NOP);
			exe32(OP, 0, PE_out[12],/*ALU in*/ PE_out[8][2],PE_out[10][1], 0, 0, /*BUFF 16to1 in*/ 0,0,0,0, /*BUFF 8to1 in*/ PE_out[8][8], PE_out[8][9], PE_out[8][10], 0);
			exe32(OP_NOP, 0, PE_out[13],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ 0,0,0,PE_out[9][2], /*BUFF 8to1 in*/ PE_out[9][8], PE_out[9][9], PE_out[9][10], 0);
			exe32(OP_NOP, 0, PE_out[14],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[10][4],PE_out[10][5],PE_out[10][6],PE_out[10][7], /*BUFF 8to1 in*/ PE_out[10][8], PE_out[10][9], PE_out[10][10], PE_out[10][11]);
			exe32(OP_NOP, 0, PE_out[15],/*ALU in*/ 0, 0, 0, 0, /*BUFF 16to1 in*/ PE_out[11][2],PE_out[11][5],PE_out[11][6],PE_out[11][7], /*BUFF 8to1 in*/ PE_out[11][8], PE_out[11][9], PE_out[11][10], PE_out[11][11]);		
			}
		
	}
	#if defined(ARMZYNQ)
	uint32_t Data2[32*8];
	int working_session = 0;
	i = 0;
	j = 0;
	
	printf("*****************************************************************************\n");
	printf("First 16 Outputs from CGRA Calculation\n");
	
	while (fscanf(file3, "%8x", &value) == 1) {  

		Data2[j] = value;
		i++;
		j++;
		
		if(((i%(32*8)) == 0)&&(i!=0)){
			///******Write W******///
			// printf("npe = %d\n",npe);
			for (int m=0; m<8; m++) {
				for (int n=0; n<8; n++) {
					A_MSB[m*8 +n] = H[n];
				}
			}
			for (int m=0; m<8; m++) {
				for (int n=0; n<8; n++) {
					A_LSB[m*8 +n] = H[n];
				}
			}
			
			for (int m=0; m<8; m++) {
				for (int n=0; n<16; n++) {
					if(n<8){
						B_MSB[m*8 +n] = Data2[m*16+n];
					}
					else {
						C_MSB[m*8 +n-8] = Data2[m*16+n];
					}
					
					
				}
			}

			for (int m=0; m<8; m++) {
				for (int n=0; n<16; n++) {
					if(n<8){
						B_LSB[m*8 +n] = Data2[m*16+128+n];
					}
					else {
						C_LSB[m*8 +n-8] = Data2[m*16+128+n];
					}
					
					
				}
			}
		*(CGRA_info.ctx_pe_mmap+MODE_BASE_IP) = 0; // Mode = 0 for 32-bit computations and Mode = 1 for 64-bit computations
		dma_write_U32(ROW0_MSB_BASE_PHYS,8);
		dma_write_U32(ROW1_MSB_BASE_PHYS,8);
		dma_write_U32(ROW2_MSB_BASE_PHYS,8);
		dma_write_U32(ROW0_LSB_BASE_PHYS,8);
		dma_write_U32(ROW1_LSB_BASE_PHYS,8);
		dma_write_U32(ROW2_LSB_BASE_PHYS,8);
		*(CGRA_info.ctx_pe_mmap+START_BASE_IP) = 1;
	
    	while(1){
			if(*(CGRA_info.ctx_pe_mmap+FINISH_BASE_IP) == 1)
				break;
		}

		dma_read_U32(ROW0_MSB_BASE_PHYS,8);
		dma_read_U32(ROW0_LSB_BASE_PHYS,8);
		if(working_session==0){
			for (int m=0; m<8; m++) {
			printf("Output MSB[%d] = ",m);
			for (int n=0; n<8; n++) {
				printf("%08x",A_MSB[m*8+n]);
			}
			printf("\n");
			}
			printf("\n");
			for (int m=0; m<8; m++) {
			printf("Output LSB[%d] = ",m);
			for (int n=0; n<8; n++) {
				printf("%08x",A_LSB[m*8+n]);
			}
			printf("\n");
			}
		printf("\n");	
		}

			for (int m=0; m<8; m++) {
				for (int n=0; n<8; n++) {
					fprintf(file4,"%08x",A_MSB[m*8+n]);
				}
			fprintf(file4,"\n");
			}
			
			for (int m=0; m<8; m++) {
				for (int n=0; n<8; n++) {
					fprintf(file4,"%08x",A_LSB[m*8+n]);
				}
			fprintf(file4,"\n");
			}
			j = 0;
			working_session++;
		}
		
	}
	printf("*****************************************************************************\n");
	printf("Write successfully %d outputs from CGRA calculation to Output_CGRA.txt file!\n",i/16);
	printf("*****************************************************************************\n");
	printf("\n");
	#endif
	Final();
	
	return 0;
}