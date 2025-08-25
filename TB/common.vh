/*
 *-----------------------------------------------------------------------------
 * Title         : U2CA
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : common.vh
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.03.07
 *-----------------------------------------------------------------------------
 * Last modified : 2023.12.01
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.03.07 : created
 *-----------------------------------------------------------------------------
 */
`define START_BASE_PHYS	 40'h0400000000
`define MODE_BASE_PHYS	 40'h0400000020
`define FINISH_BASE_PHYS 40'h0400000000
`define CTX_PE_BASE_IP	 16'h0401
`define CTX_RC_BASE_IP	 16'h0402
`define CTX_IM_BASE_IP	 16'h0403
`define LMM_BASE_PHYS	 12'h048

`define PE_NUM		       16
`define RW_NUM		       4
`define RW_NUM_BITS		   2
`define LR_BITS		   	   1 //In mode 32, LR_BITS is NOT used. In mode 64, the value of LR_BITS = 0 if write first two left PEs and the value of LR_BITS = 1 if write first two right PEs

`define PEHI_SEL_BITS      1
`define PE_NUM_BITS	       4

`define HI_SELECT	       1'd0
`define PE_SELECT	       1'd1

`define MODE32	       	   1'd0
`define MODE64	       	   1'd1

///////////////////////////////////////////////
/// 	Processing Element Array (PEA) 	   ////
///////////////////////////////////////////////

	///***------ Row Connection (RC) ------***////
		`define CTX_RC_ADDR_BITS   10
		`define CTX_RC_BITS		   52
	///***--------------------------------***////

	///***----- Immediate Value (IM)-----***////
		`define CTX_IM_ADDR_BITS   10
		`define CTX_IM_BITS		   64	
	///***--------------------------------***////		
	
	///***---- Processing Element (PE)----***////
		`define CTX_PE_ADDR_BITS   10
		`define CTX_PE_BITS		   32
		`define WORD_BITS		   32
		`define DWORD_BITS		   64
		`define AXI_DWIDTH_BITS	   256
		`define PE_AXI_DWIDTH_BITS 64
		///--------- Load Store Unit (LSU) ---------////
			`define REG_BRAM
			//`define ZYNQ_BRAM
			`define LDM_ADDR_BITS      10
			`define AAG_BITS      	   8
			`define LSU_CFG_BITS       (1+`LDM_ADDR_BITS)
			`define LSU_LDW            1'd0
			`define LSU_STW            1'd1

		///------ Arithmetic Logic Unit (ALU)------////

			//-------- EXE1 --------//
			`define ADDRA_BITS     	   6
			//-------- EXE2 --------//
			`define EXE1_CFG_BITS     2
			
			// define Operation for EXE1
			`define EXE1_NOP             `EXE1_CFG_BITS'd0
			`define EXE1_ADD2            `EXE1_CFG_BITS'd1
			`define EXE1_ADD3            `EXE1_CFG_BITS'd2
			`define EXE1_SUB2            `EXE1_CFG_BITS'd3
			
			/*---- EXE2 ----*/
			
			`define EXE2_LSR          1'd0
			`define EXE2_SHROW        1'd1
			
			// define Operation for Logic Unit 1/2/3
			
			`define EXE2_CFG_BITS     24
			
			`define EXE2_NOP          3'd0
			`define EXE2_XOR          3'd1
			`define EXE2_OR           3'd2
			`define EXE2_AND          3'd3
			`define EXE2_NOT          3'd4
			`define EXE2_NOT_XOR      3'd5
			`define EXE2_NOT_OR       3'd6
			`define EXE2_NOT_AND      3'd7
			
			`define sLU_NOP          2'd0
			`define sLU_XOR          2'd1
			`define sLU_OR           2'd2
			`define sLU_AND          2'd3
			
			// define Operation for Shift Rorate Unit 1/2
			
			`define EXE2_SHL          2'd0
			`define EXE2_SHR          2'd1
			`define EXE2_ROL          2'd2
			`define EXE2_ROR          2'd3
			
			/*---- EXE_32-bit CUSTOM ----*/
			`define EXE_CT_CFG_BITS   5 
			
			`define EXE_CT_NOP        `EXE_CT_CFG_BITS'd0
			`define EXE_CT_GW3        `EXE_CT_CFG_BITS'd1
			`define EXE_CT_SUB_SHF    `EXE_CT_CFG_BITS'd2
			`define EXE_CT_MIXCOL     `EXE_CT_CFG_BITS'd3
			`define EXE_CT_ISHF_ISUB  `EXE_CT_CFG_BITS'd4
			`define EXE_CT_IMIXCOL    `EXE_CT_CFG_BITS'd5
			`define EXE_CT_SUM01      `EXE_CT_CFG_BITS'd6
			`define EXE_CT_SIG01      `EXE_CT_CFG_BITS'd7
			`define EXE_CT_FF1        `EXE_CT_CFG_BITS'd8
			`define EXE_CT_P0         `EXE_CT_CFG_BITS'd9
			`define EXE_CT_SBOX_SM4   `EXE_CT_CFG_BITS'd10
			`define EXE_CT_4_RX_SM4	  `EXE_CT_CFG_BITS'd11
			
			/*---- EXE_32-bit CUSTOM ----*/
			`define EXE_CT_64_NOP     `EXE_CT_CFG_BITS'd0
			
			//*** RESERVED ***//

// define Operation for custom unit
`define EXE_IMM_BITS   32
`define ALU_CFG_BITS  	 `EXE1_CFG_BITS + `EXE2_CFG_BITS + `EXE_CT_CFG_BITS