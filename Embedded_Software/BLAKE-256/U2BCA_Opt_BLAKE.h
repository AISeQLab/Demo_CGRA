typedef uint8_t      U8;
typedef uint16_t      U16;
typedef uint32_t      U32;
typedef uint64_t      U64;

void  /*__attribute__((always_inline))*/exe(U8 OP1,U64 OP2, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3);
void  /*__attribute__((always_inline))*/mop(U8 OP, U64 base, U64 off_set, U32 *LMM, U32 *PE_out, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3);
uint64_t OP2_CAT(U8 OP2_LU1,U8 OP2_LU2,U8 OP2_SRU1, U8 SRU1_IM, U8 OP2_SRU2,U8 SRU2_IM, U8 OP2_SRU3, U8 SRU3_IM, U8 OP2_LU3, U8 KEEP_OP1);
////***mop()***////
#define NUM_PE          16

//OP
#define OP_LDW          0
#define OP_STW          1

////***exe()***////

//OP1

#define OP_NOP          0
#define OP_ADD2         1
#define OP_ADD3         2
#define OP_SUB2         3
#define OP_SUB3         4
#define OP_SUBBYTE      5
#define OP_MIXCOL       6

//OP2_LU1/2/3

#define OP_NOP          0
#define OP_XOR          1
#define OP_OR           2
#define OP_AND          3
#define OP_NOT          4
#define OP_NOT_XOR      5
#define OP_NOT_OR       6
#define OP_NOT_AND      7
#define OP_SHROW       0x100000000

//SRU1/2

#define OP_SHL          0
#define OP_SHR          1
#define OP_ROL          2
#define OP_ROR          3

//KEEP output of OP1
#define PASS        0
#define KEEP        1