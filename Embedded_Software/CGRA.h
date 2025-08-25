typedef uint8_t       U8;
typedef uint16_t      U16;
typedef uint32_t      U32;
typedef uint64_t      U64;
  
void  mop32(U8 OP, U32 base_addr, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3);
void  exe32(U32 OP, U32 OP_IM, U32 *PE_out, /*ALU in*/ U32 s48_0, U32 s48_1, U32 s48_2, U32 s48_3, /*BUFF 16to1 in*/ U32 b16_0, U32 b16_1, U32 b16_2, U32 b16_3, /*BUFF 8to1 in*/ U32 b8_0, U32 b8_1, U32 b8_2, U32 b8_3);
void  mop64(U8 OP, U32 base_addr, U64 *PE_out, /*ALU in*/ U64 s48_0, U64 s48_1, U64 s48_2, U64 s48_3, /*BUFF 16to1 in*/ U64 b16_0, U64 b16_1, U64 b16_2, U64 b16_3, /*BUFF 8to1 in*/ U64 b8_0, U64 b8_1, U64 b8_2, U64 b8_3);
void  exe64(U32 OP, U64 OP_IM, U64 *PE_out, /*ALU in*/ U64 s48_0, U64 s48_1, U64 s48_2, U64 s48_3, /*BUFF 16to1 in*/ U64 b16_0, U64 b16_1, U64 b16_2, U64 b16_3, /*BUFF 8to1 in*/ U64 b8_0, U64 b8_1, U64 b8_2, U64 b8_3);
U32 BASIC_OP(/*AU*/U8 OP_AU,/*LU1*/U8 OP_LU1,/*LU2*/U8 OP_LU2,/*SRU1*/U8 OP_SRU1,/*SRU1_IM*/U8 SRU1_IM,/*SRU2*/U8 OP_SRU2,/*SRU2_IM*/U8 SRU2_IM,/*LU3*/U8 OP_LU3);
U32 CUSTOM_OP(U8 OP_CUSTOM);


////***mop()***////
#define NUM_PE          16
#define NUM_LMM         16
#define LMM_DEP         1024
#define NUM_PE_INOUT    12
//OP Memory
#define OP_LDW          0
#define OP_STW          1

////***exe()***////

//OP_AU

#define OP_NOP          0
#define OP_ADD2         1
#define OP_ADD3         2
#define OP_SUB2         3

//OP_LU1/2/3

#define OP_NOP          0
#define OP_XOR          1
#define OP_OR           2
#define OP_AND          3
#define OP_NOT          4
#define OP_NOT_XOR      5
#define OP_NOT_OR       6
#define OP_NOT_AND      7

//OP_SRU1/2

#define OP_SHL          0
#define OP_SHR          1
#define OP_ROL          2
#define OP_ROR          3

//Customized OP

#define OP_NOP          0
#define OP_GW3      	1
#define OP_SUB_SHF    	2
#define OP_MIXCOL       3
#define OP_ISHF_ISUB  	4
#define OP_IMIXCOL      5
#define OP_SUM01      	6
#define OP_SIG01      	7
#define OP_FF1          8
#define OP_P0           9
#define OP_SBOX_SM4     10
#define OP_4_RX_SM4     11
#define OP_SBOX_8		12
/* User define*/