#include <stdio.h>
#include <stdint.h> // we use 32-bit words

// rotate x to left by n bits, the bits that go over the left edge reappear on the right
#define R(x,n) (((x) << (n)) | ((x) >> (32-(n))))

// addition wraps modulo 2^32
// the choice of 7,9,13,18 "doesn't seem very importa/*BUFF 8to1 in*/ 0,0,0,0nt" (spec)
#define quarter(a,b,c,d) do {\
	b ^= R(d+a, 7);\
	c ^= R(a+b, 9);\
	d ^= R(b+c, 13);\
	a ^= R(c+d, 18);\
} while (0)

void salsa20_words(uint32_t *out, uint32_t in[16]) {
	uint32_t x[4][4];
	int i;
	#ifdef DEBUG
		printf("========================================================\n");
		printf("\n");
	#endif
	int y[] = 
	{	0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
		0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
		0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c,
		0x00000001, 0x00000000, 0x4a000000, 0x00000000};

	for (i=0; i<16; ++i) {
		x[i/4][i%4] = y[i];
		#ifdef DEBUG
			printf("x[%d] = %08x\n", i, x[i/4][i%4]);
		#endif
	}
	for (i=0; i<10; ++i) { // 10 double rounds = 20 rounds
		// column round: quarter round on each column; start at ith element and wrap
		quarter(y[0], y[4], y[8], y[12]);
		quarter(y[5], y[9], y[13], y[1]);
		quarter(y[10], y[14], y[2], y[6]);
		quarter(y[15], y[3], y[7], y[11]);
    #ifdef DEBUG
        printf("========================================================\n");
        printf("To the First Round %d:\n", i);
        printf("x[0] = %08x, x[4] = %08x, x[8 ] = %08x, x[12] = %08x\n", x[0][0], x[0][1], x[0][2], x[0][3]);
        printf("x[1] = %08x, x[5] = %08x, x[9 ] = %08x, x[13] = %08x\n", x[1][0], x[1][1], x[1][2], x[1][3]);
        printf("x[2] = %08x, x[6] = %08x, x[10] = %08x, x[14] = %08x\n", x[2][0], x[2][1], x[2][2], x[2][3]);
        printf("x[3] = %08x, x[7] = %08x, x[11] = %08x, x[15] = %08x\n", x[3][0], x[3][1], x[3][2], x[3][3]);
        printf("\n");
    #endif
		// row round: quarter round on each row; start at ith element and wrap around
		quarter(y[0], y[1], y[2], y[3]);
		quarter(y[5], y[6], y[7], y[4]);
		quarter(y[10], y[11], y[8], y[9]);
		quarter(y[15], y[12], y[13], y[14]);
    #ifdef DEBUG
        printf("To the Second Round %d:\n", i);
        printf("x[0] = %08x, x[4] = %08x, x[8 ] = %08x, x[12] = %08x\n", x[0][0], x[0][1], x[0][2], x[0][3]);
        printf("x[1] = %08x, x[5] = %08x, x[9 ] = %08x, x[13] = %08x\n", x[1][0], x[1][1], x[1][2], x[1][3]);
        printf("x[2] = %08x, x[6] = %08x, x[10] = %08x, x[14] = %08x\n", x[2][0], x[2][1], x[2][2], x[2][3]);
        printf("x[3] = %08x, x[7] = %08x, x[11] = %08x, x[15] = %08x\n", x[3][0], x[3][1], x[3][2], x[3][3]);
        printf("\n");
    #endif
	}
	for (i=0; i<16; ++i) out[i] = x[i/4][i%4] + in[i];
}

// inputting a key, message nonce, keystream index and constants to that transormation
void salsa20_block(uint8_t *out, uint8_t key[32], uint64_t nonce, uint64_t index) {
	static const char c[16] = "expand 32-byte k"; // arbitrary constant
	#define LE(p) ( (p)[0] | ((p)[1]<<8) | ((p)[2]<<16) | ((p)[3]<<24) )
	uint32_t in[16] = {LE(c),            LE(key),    LE(key+4),        LE(key+8),
	                   LE(key+12),       LE(c+4),    nonce&0xffffffff, nonce>>32,
	                   index&0xffffffff, index>>32,  LE(c+8),          LE(key+16),
	                   LE(key+20),       LE(key+24), LE(key+28),       LE(c+12)};
	uint32_t wordout[16];
	salsa20_words(wordout, in);
	int i;
	for (i=0; i<64; ++i) out[i] = 0xff & (wordout[i/4] >> (8*(i%4)));
}

// enc/dec: xor a message with transformations of key, a per-message nonce and block index
void salsa20(uint8_t *message, uint64_t mlen, uint8_t key[32], uint64_t nonce) {
	int i;
	uint8_t block[64];
	for (i=0; i<mlen; i++) {
		if (i%64 == 0) salsa20_block(block, key, nonce, i/64);
		message[i] ^= block[i%64];
	}
}

//Set 2, vector# 0:
//                         key = 00000000000000000000000000000000
//                               00000000000000000000000000000000
//                          IV = 0000000000000000
//               stream[0..63] = 9A97F65B9B4C721B960A672145FCA8D4
//                               E32E67F9111EA979CE9C4826806AEEE6
//                               3DE9C0DA2BD7F91EBCB2639BF989C625
//                               1B29BF38D39A9BDCE7C55F4B2AC12A39

int  main () {
	uint8_t key[32] = {0};
	uint64_t nonce = 0;
	uint8_t msg[64] = {0};
	
	salsa20(msg, sizeof(msg), key, nonce);
	int i; for (i=0; i<sizeof(msg); ++i) printf("%02X", msg[i]); printf("\n");
}