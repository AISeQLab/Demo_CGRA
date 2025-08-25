/*
 *-----------------------------------------------------------------------------
 * Title         : EXE_CUSTOM
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : EXE_CUSTOM.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2024.01.24
 *-----------------------------------------------------------------------------
 * Last modified : 2024.01.24
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2024.01.24 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"
module EXE_CUSTOM
(
	input  wire                                 CLK,
	input  wire                                 RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					            En_in,
	input  wire 					            Mode_in,
	input  wire 					            Finish_in,
	input  wire [`EXE_CT_CFG_BITS-1:0]          CFG_in,
	input  wire [`DWORD_BITS-1:0]              	S0_in,
	input  wire [`DWORD_BITS-1:0]              	S1_in,
	input  wire [`DWORD_BITS-1:0]              	S2_in,
	input  wire [`DWORD_BITS-1:0]              	S3_in,
	
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------// 
	output reg  [`DWORD_BITS-1:0]           	D0_out,
	output reg  [`DWORD_BITS-1:0]           	D1_out,
	output reg  [`DWORD_BITS-1:0]           	D2_out,
	output reg  [`DWORD_BITS-1:0]           	D3_out
);
 
	////////////////////////////////////
	///*** 32-bit MSB Declaration ***///
	////////////////////////////////////
	
	/// For EXE_CT_ISHF_ISUB
	wire  [`WORD_BITS-1:0]    					S0_MSB_wr, S1_MSB_wr, S2_MSB_wr, S3_MSB_wr;
	
	/// For EXE_CT_MIXCOL  	
	wire  [`WORD_BITS-1:0]    					D0_MSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_mixcol_wr;
	
	/// For EXE_CT_IMIXCOL 					
	wire  [`WORD_BITS-1:0]    					D0_MSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_inv_mixcol_wr;
	
	/// For EXE_CT_SUB_SHF	
	wire  [`WORD_BITS-1:0]    					D0_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_SBOX_wr;
	
	/// For EXE_CT_SUB_SHF	
	wire  [`WORD_BITS-1:0]    					D0_MSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_SHF_wr;
	
	/// For EXE_CT_ISHF_ISUB	
	wire  [`WORD_BITS-1:0]    					D0_MSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_ISHF_wr;
						
	wire  [`WORD_BITS-1:0]    					D0_MSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D1_MSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D2_MSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D3_MSB_ISBOX_wr; 
	
	/// For SBOX SM4	
	wire  [`WORD_BITS-1:0]    					D0_MSB_SBOX_SM4_wr;
	
	/// For EXE_CT_SBOX_8
	wire  [`WORD_BITS-1:0]    					T0_D0_MSB_SBOX_wr, T0_D1_MSB_SBOX_wr, T0_D2_MSB_SBOX_wr, T0_D3_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T1_D0_MSB_SBOX_wr, T1_D1_MSB_SBOX_wr, T1_D2_MSB_SBOX_wr, T1_D3_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T2_D0_MSB_SBOX_wr, T2_D1_MSB_SBOX_wr, T2_D2_MSB_SBOX_wr, T2_D3_MSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T3_D0_MSB_SBOX_wr, T3_D1_MSB_SBOX_wr, T3_D2_MSB_SBOX_wr, T3_D3_MSB_SBOX_wr;
	
	reg  [`WORD_BITS-1:0]     					D0_MSB_rg;
	reg  [`WORD_BITS-1:0]     					D1_MSB_rg;
	reg  [`WORD_BITS-1:0]     					D2_MSB_rg;
	reg  [`WORD_BITS-1:0]     					D3_MSB_rg;
	
	////////////////////////////////////
	///*** 32-bit LSB Declaration ***///
	////////////////////////////////////
	
	/// For EXE_CT_ISHF_ISUB
	wire  [`WORD_BITS-1:0]    					S0_LSB_wr, S1_LSB_wr, S2_LSB_wr, S3_LSB_wr;
	
	/// For EXE_CT_MIXCOL  					
	wire  [`WORD_BITS-1:0]    					D0_LSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_mixcol_wr;
	
	/// For EXE_CT_IMIXCOL 	
	wire  [`WORD_BITS-1:0]    					D0_LSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_inv_mixcol_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_inv_mixcol_wr;
	
	/// For EXE_CT_SUB_SHF	
	wire  [`WORD_BITS-1:0]    					D0_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_SBOX_wr;
	
	/// For EXE_CT_SUB_SHF					
	wire  [`WORD_BITS-1:0]    					D0_LSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_SHF_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_SHF_wr;
	
	/// For EXE_CT_ISHF_ISUB	
	wire  [`WORD_BITS-1:0]    					D0_LSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_ISHF_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_ISHF_wr;
						
	wire  [`WORD_BITS-1:0]    					D0_LSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D1_LSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D2_LSB_ISBOX_wr;
	wire  [`WORD_BITS-1:0]    					D3_LSB_ISBOX_wr; 
	
	/// For SBOX SM4	
	wire  [`WORD_BITS-1:0]    					D0_LSB_SBOX_SM4_wr;
	
	/// For EXE_CT_SBOX_8
	wire  [`WORD_BITS-1:0]    					T0_D0_LSB_SBOX_wr, T0_D1_LSB_SBOX_wr, T0_D2_LSB_SBOX_wr, T0_D3_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T1_D0_LSB_SBOX_wr, T1_D1_LSB_SBOX_wr, T1_D2_LSB_SBOX_wr, T1_D3_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T2_D0_LSB_SBOX_wr, T2_D1_LSB_SBOX_wr, T2_D2_LSB_SBOX_wr, T2_D3_LSB_SBOX_wr;
	wire  [`WORD_BITS-1:0]    					T3_D0_LSB_SBOX_wr, T3_D1_LSB_SBOX_wr, T3_D2_LSB_SBOX_wr, T3_D3_LSB_SBOX_wr;
						
	reg  [`WORD_BITS-1:0]     					D0_LSB_rg;
	reg  [`WORD_BITS-1:0]     					D1_LSB_rg;
	reg  [`WORD_BITS-1:0]     					D2_LSB_rg;
	reg  [`WORD_BITS-1:0]     					D3_LSB_rg;  

	////////////////////////////////////
	///*** 64-bit MSB Declaration ***///
	////////////////////////////////////

	reg  [`DWORD_BITS-1:0]     					D0_64_rg;
	reg  [`DWORD_BITS-1:0]     					D1_64_rg;
	reg  [`DWORD_BITS-1:0]     					D2_64_rg;
	reg  [`DWORD_BITS-1:0]     					D3_64_rg; 
						
	wire  [`DWORD_BITS-1:0]  					D0_wr;
	wire  [`DWORD_BITS-1:0]  					D1_wr;
	wire  [`DWORD_BITS-1:0]  					D2_wr;
	wire  [`DWORD_BITS-1:0]  					D3_wr;
	
	////////////////////////////////////
	///*** 32-bit MSB Calculation ***///
	////////////////////////////////////	
	assign S0_MSB_wr = S0_in[`DWORD_BITS-1:`WORD_BITS];
	assign S1_MSB_wr = S1_in[`DWORD_BITS-1:`WORD_BITS];
	assign S2_MSB_wr = S2_in[`DWORD_BITS-1:`WORD_BITS];
	assign S3_MSB_wr = S3_in[`DWORD_BITS-1:`WORD_BITS];
	
	/// For EXE_CT_SUB_SHF
	SBOX sbox00_MSB (.x_in(S0_MSB_wr[31:24]),.y_out(D0_MSB_SBOX_wr[31:24]));
	SBOX sbox01_MSB (.x_in(S0_MSB_wr[23:16]),.y_out(D0_MSB_SBOX_wr[23:16]));
	SBOX sbox02_MSB (.x_in(S0_MSB_wr[15:8]), .y_out(D0_MSB_SBOX_wr[15:8]));
	SBOX sbox03_MSB (.x_in(S0_MSB_wr[7:0]),  .y_out(D0_MSB_SBOX_wr[7:0]));
	
	SBOX sbox10_MSB (.x_in(S1_MSB_wr[31:24]),.y_out(D1_MSB_SBOX_wr[31:24]));
	SBOX sbox11_MSB (.x_in(S1_MSB_wr[23:16]),.y_out(D1_MSB_SBOX_wr[23:16]));
	SBOX sbox12_MSB (.x_in(S1_MSB_wr[15:8]), .y_out(D1_MSB_SBOX_wr[15:8]));
	SBOX sbox13_MSB (.x_in(S1_MSB_wr[7:0]),  .y_out(D1_MSB_SBOX_wr[7:0]));
	
	SBOX sbox20_MSB (.x_in(S2_MSB_wr[31:24]),.y_out(D2_MSB_SBOX_wr[31:24]));
	SBOX sbox21_MSB (.x_in(S2_MSB_wr[23:16]),.y_out(D2_MSB_SBOX_wr[23:16]));
	SBOX sbox22_MSB (.x_in(S2_MSB_wr[15:8]), .y_out(D2_MSB_SBOX_wr[15:8]));
	SBOX sbox23_MSB (.x_in(S2_MSB_wr[7:0]),  .y_out(D2_MSB_SBOX_wr[7:0]));
	
	SBOX sbox30_MSB (.x_in(S3_MSB_wr[31:24]),.y_out(D3_MSB_SBOX_wr[31:24]));
	SBOX sbox31_MSB (.x_in(S3_MSB_wr[23:16]),.y_out(D3_MSB_SBOX_wr[23:16]));
	SBOX sbox32_MSB (.x_in(S3_MSB_wr[15:8]), .y_out(D3_MSB_SBOX_wr[15:8]));
	SBOX sbox33_MSB (.x_in(S3_MSB_wr[7:0]),  .y_out(D3_MSB_SBOX_wr[7:0]));  
  
	assign D0_MSB_SHF_wr = {D0_MSB_SBOX_wr[31:24],D1_MSB_SBOX_wr[23:16],D2_MSB_SBOX_wr[15:8],D3_MSB_SBOX_wr[7:0]}; // SBOX for 32-bit S0_in + Shift Row 0
	assign D1_MSB_SHF_wr = {D1_MSB_SBOX_wr[31:24],D2_MSB_SBOX_wr[23:16],D3_MSB_SBOX_wr[15:8],D0_MSB_SBOX_wr[7:0]}; // SBOX for 32-bit S1_in + Shift Row 0
	assign D2_MSB_SHF_wr = {D2_MSB_SBOX_wr[31:24],D3_MSB_SBOX_wr[23:16],D0_MSB_SBOX_wr[15:8],D1_MSB_SBOX_wr[7:0]}; // SBOX for 32-bit S2_in + Shift Row 0
	assign D3_MSB_SHF_wr = {D3_MSB_SBOX_wr[31:24],D0_MSB_SBOX_wr[23:16],D1_MSB_SBOX_wr[15:8],D2_MSB_SBOX_wr[7:0]}; // SBOX for 32-bit S3_in + Shift Row 0
	
	/// For SBOX SM4
	SBOX_SM4 sbox00_sm4_MSB (.x_in(S0_MSB_wr[31:24]),.y_out(D0_MSB_SBOX_SM4_wr[31:24]));
	SBOX_SM4 sbox01_sm4_MSB (.x_in(S0_MSB_wr[23:16]),.y_out(D0_MSB_SBOX_SM4_wr[23:16]));
	SBOX_SM4 sbox02_sm4_MSB (.x_in(S0_MSB_wr[15:8]), .y_out(D0_MSB_SBOX_SM4_wr[15:8]));
	SBOX_SM4 sbox03_sm4_MSB (.x_in(S0_MSB_wr[7:0]),  .y_out(D0_MSB_SBOX_SM4_wr[7:0]));
	
	/// For EXE_CT_ISHF_ISUB
	assign D0_MSB_ISHF_wr = {S0_MSB_wr[31:24],S3_MSB_wr[23:16],S2_MSB_wr[15:8],S1_MSB_wr[7:0]}; // SBOX for 32-bit S0_in + Shift Row 0
	assign D1_MSB_ISHF_wr = {S1_MSB_wr[31:24],S0_MSB_wr[23:16],S3_MSB_wr[15:8],S2_MSB_wr[7:0]}; // SBOX for 32-bit S1_in + Shift Row 0
	assign D2_MSB_ISHF_wr = {S2_MSB_wr[31:24],S1_MSB_wr[23:16],S0_MSB_wr[15:8],S3_MSB_wr[7:0]}; // SBOX for 32-bit S2_in + Shift Row 0
	assign D3_MSB_ISHF_wr = {S3_MSB_wr[31:24],S2_MSB_wr[23:16],S1_MSB_wr[15:8],S0_MSB_wr[7:0]}; // SBOX for 32-bit S3_in + Shift Row 0
	
	INV_SBOX inv_sbox00_MSB (.x_in(D0_MSB_ISHF_wr[31:24]),.y_out(D0_MSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox01_MSB (.x_in(D0_MSB_ISHF_wr[23:16]),.y_out(D0_MSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox02_MSB (.x_in(D0_MSB_ISHF_wr[15:8]), .y_out(D0_MSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox03_MSB (.x_in(D0_MSB_ISHF_wr[7:0]),  .y_out(D0_MSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox10_MSB (.x_in(D1_MSB_ISHF_wr[31:24]),.y_out(D1_MSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox11_MSB (.x_in(D1_MSB_ISHF_wr[23:16]),.y_out(D1_MSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox12_MSB (.x_in(D1_MSB_ISHF_wr[15:8]), .y_out(D1_MSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox13_MSB (.x_in(D1_MSB_ISHF_wr[7:0]),  .y_out(D1_MSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox20_MSB (.x_in(D2_MSB_ISHF_wr[31:24]),.y_out(D2_MSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox21_MSB (.x_in(D2_MSB_ISHF_wr[23:16]),.y_out(D2_MSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox22_MSB (.x_in(D2_MSB_ISHF_wr[15:8]), .y_out(D2_MSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox23_MSB (.x_in(D2_MSB_ISHF_wr[7:0]),  .y_out(D2_MSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox30_MSB (.x_in(D3_MSB_ISHF_wr[31:24]),.y_out(D3_MSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox31_MSB (.x_in(D3_MSB_ISHF_wr[23:16]),.y_out(D3_MSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox32_MSB (.x_in(D3_MSB_ISHF_wr[15:8]), .y_out(D3_MSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox33_MSB (.x_in(D3_MSB_ISHF_wr[7:0]),  .y_out(D3_MSB_ISBOX_wr[7:0]));  
  
	/// For EXE_CT_MIXCOL  
	MIX_COLUMN mixcolumns_MSB
	( 
		.S0_in(S0_MSB_wr),
		.S1_in(S1_MSB_wr),
		.S2_in(S2_MSB_wr),
		.S3_in(S3_MSB_wr),
		.mtx_00_in(4'h2),
		.mtx_01_in(4'h3),
		.mtx_02_in(4'h1),
		.mtx_03_in(4'h1),
		.mtx_10_in(4'h1),
		.mtx_11_in(4'h2),
		.mtx_12_in(4'h3),
		.mtx_13_in(4'h1),
		.mtx_20_in(4'h1),
		.mtx_21_in(4'h1),
		.mtx_22_in(4'h2),
		.mtx_23_in(4'h3),
		.mtx_30_in(4'h3),
		.mtx_31_in(4'h1),
		.mtx_32_in(4'h1),
		.mtx_33_in(4'h2),
		.D0_out(D0_MSB_mixcol_wr),
		.D1_out(D1_MSB_mixcol_wr),
		.D2_out(D2_MSB_mixcol_wr),
		.D3_out(D3_MSB_mixcol_wr)
	);

	/// For EXE_CT_IMIXCOL  
	MIX_COLUMN inv_mixcolumns_MSB
	( 
		.S0_in(S0_MSB_wr),
		.S1_in(S1_MSB_wr),
		.S2_in(S2_MSB_wr),
		.S3_in(S3_MSB_wr),
		.mtx_00_in(4'he),
		.mtx_01_in(4'hb),
		.mtx_02_in(4'hd),
		.mtx_03_in(4'h9),
		.mtx_10_in(4'h9),
		.mtx_11_in(4'he),
		.mtx_12_in(4'hb),
		.mtx_13_in(4'hd),
		.mtx_20_in(4'hd),
		.mtx_21_in(4'h9),
		.mtx_22_in(4'he),
		.mtx_23_in(4'hb),
		.mtx_30_in(4'hb),
		.mtx_31_in(4'hd),
		.mtx_32_in(4'h9),
		.mtx_33_in(4'he),
		.D0_out(D0_MSB_inv_mixcol_wr),
		.D1_out(D1_MSB_inv_mixcol_wr),
		.D2_out(D2_MSB_inv_mixcol_wr),
		.D3_out(D3_MSB_inv_mixcol_wr)
	);
 
 	/// For EXE_CT_SBOX_8
	
	/*D0*/
	subcell sc00_0_MSB (.in(S0_MSB_wr[31:24]), .out(T0_D0_MSB_SBOX_wr[31:24]));
    subcell sc00_1_MSB (.in(T0_D0_MSB_SBOX_wr[31:24]), .out(T1_D0_MSB_SBOX_wr[31:24]));
    subcell sc00_2_MSB (.in(T1_D0_MSB_SBOX_wr[31:24]), .out(T2_D0_MSB_SBOX_wr[31:24]));
    subcell_last sc00_3_MSB (.in(T2_D0_MSB_SBOX_wr[31:24]), .out(T3_D0_MSB_SBOX_wr[31:24]));
	
	subcell sc01_0_MSB (.in(S0_MSB_wr[23:16]), .out(T0_D0_MSB_SBOX_wr[23:16]));
    subcell sc01_1_MSB (.in(T0_D0_MSB_SBOX_wr[23:16]), .out(T1_D0_MSB_SBOX_wr[23:16]));
    subcell sc01_2_MSB (.in(T1_D0_MSB_SBOX_wr[23:16]), .out(T2_D0_MSB_SBOX_wr[23:16]));
    subcell_last sc01_3_MSB (.in(T2_D0_MSB_SBOX_wr[23:16]), .out(T3_D0_MSB_SBOX_wr[23:16]));
	
	subcell sc02_0_MSB (.in(S0_MSB_wr[15:8]), .out(T0_D0_MSB_SBOX_wr[15:8]));
    subcell sc02_1_MSB (.in(T0_D0_MSB_SBOX_wr[15:8]), .out(T1_D0_MSB_SBOX_wr[15:8]));
    subcell sc02_2_MSB (.in(T1_D0_MSB_SBOX_wr[15:8]), .out(T2_D0_MSB_SBOX_wr[15:8]));
    subcell_last sc02_3_MSB (.in(T2_D0_MSB_SBOX_wr[15:8]), .out(T3_D0_MSB_SBOX_wr[15:8]));
	
	subcell sc03_0_MSB (.in(S0_MSB_wr[7:0]), .out(T0_D0_MSB_SBOX_wr[7:0]));
    subcell sc03_1_MSB (.in(T0_D0_MSB_SBOX_wr[7:0]), .out(T1_D0_MSB_SBOX_wr[7:0]));
    subcell sc03_2_MSB (.in(T1_D0_MSB_SBOX_wr[7:0]), .out(T2_D0_MSB_SBOX_wr[7:0]));
    subcell_last sc03_3_MSB (.in(T2_D0_MSB_SBOX_wr[7:0]), .out(T3_D0_MSB_SBOX_wr[7:0]));
	
	/*D1*/
	subcell sc10_0_MSB (.in(S1_MSB_wr[31:24]), .out(T0_D1_MSB_SBOX_wr[31:24]));
    subcell sc10_1_MSB (.in(T0_D1_MSB_SBOX_wr[31:24]), .out(T1_D1_MSB_SBOX_wr[31:24]));
    subcell sc10_2_MSB (.in(T1_D1_MSB_SBOX_wr[31:24]), .out(T2_D1_MSB_SBOX_wr[31:24]));
    subcell_last sc10_3_MSB (.in(T2_D1_MSB_SBOX_wr[31:24]), .out(T3_D1_MSB_SBOX_wr[31:24]));
	
	subcell sc11_0_MSB (.in(S1_MSB_wr[23:16]), .out(T0_D1_MSB_SBOX_wr[23:16]));
    subcell sc11_1_MSB (.in(T0_D1_MSB_SBOX_wr[23:16]), .out(T1_D1_MSB_SBOX_wr[23:16]));
    subcell sc11_2_MSB (.in(T1_D1_MSB_SBOX_wr[23:16]), .out(T2_D1_MSB_SBOX_wr[23:16]));
    subcell_last sc11_3_MSB (.in(T2_D1_MSB_SBOX_wr[23:16]), .out(T3_D1_MSB_SBOX_wr[23:16]));
	
	subcell sc12_0_MSB (.in(S1_MSB_wr[15:8]), .out(T0_D1_MSB_SBOX_wr[15:8]));
    subcell sc12_1_MSB (.in(T0_D1_MSB_SBOX_wr[15:8]), .out(T1_D1_MSB_SBOX_wr[15:8]));
    subcell sc12_2_MSB (.in(T1_D1_MSB_SBOX_wr[15:8]), .out(T2_D1_MSB_SBOX_wr[15:8]));
    subcell_last sc12_3_MSB (.in(T2_D1_MSB_SBOX_wr[15:8]), .out(T3_D1_MSB_SBOX_wr[15:8]));
	
	subcell sc13_0_MSB (.in(S1_MSB_wr[7:0]), .out(T0_D1_MSB_SBOX_wr[7:0]));
    subcell sc13_1_MSB (.in(T0_D1_MSB_SBOX_wr[7:0]), .out(T1_D1_MSB_SBOX_wr[7:0]));
    subcell sc13_2_MSB (.in(T1_D1_MSB_SBOX_wr[7:0]), .out(T2_D1_MSB_SBOX_wr[7:0]));
    subcell_last sc13_3_MSB (.in(T2_D1_MSB_SBOX_wr[7:0]), .out(T3_D1_MSB_SBOX_wr[7:0]));
	
	/*D2*/
	subcell sc20_0_MSB (.in(S2_MSB_wr[31:24]), .out(T0_D2_MSB_SBOX_wr[31:24]));
    subcell sc20_1_MSB (.in(T0_D2_MSB_SBOX_wr[31:24]), .out(T1_D2_MSB_SBOX_wr[31:24]));
    subcell sc20_2_MSB (.in(T1_D2_MSB_SBOX_wr[31:24]), .out(T2_D2_MSB_SBOX_wr[31:24]));
    subcell_last sc20_3_MSB (.in(T2_D2_MSB_SBOX_wr[31:24]), .out(T3_D2_MSB_SBOX_wr[31:24]));
	
	subcell sc21_0_MSB (.in(S2_MSB_wr[23:16]), .out(T0_D2_MSB_SBOX_wr[23:16]));
    subcell sc21_1_MSB (.in(T0_D2_MSB_SBOX_wr[23:16]), .out(T1_D2_MSB_SBOX_wr[23:16]));
    subcell sc21_2_MSB (.in(T1_D2_MSB_SBOX_wr[23:16]), .out(T2_D2_MSB_SBOX_wr[23:16]));
    subcell_last sc21_3_MSB (.in(T2_D2_MSB_SBOX_wr[23:16]), .out(T3_D2_MSB_SBOX_wr[23:16]));
	
	subcell sc22_0_MSB (.in(S2_MSB_wr[15:8]), .out(T0_D2_MSB_SBOX_wr[15:8]));
    subcell sc22_1_MSB (.in(T0_D2_MSB_SBOX_wr[15:8]), .out(T1_D2_MSB_SBOX_wr[15:8]));
    subcell sc22_2_MSB (.in(T1_D2_MSB_SBOX_wr[15:8]), .out(T2_D2_MSB_SBOX_wr[15:8]));
    subcell_last sc22_3_MSB (.in(T2_D2_MSB_SBOX_wr[15:8]), .out(T3_D2_MSB_SBOX_wr[15:8]));
	
	subcell sc23_0_MSB (.in(S2_MSB_wr[7:0]), .out(T0_D2_MSB_SBOX_wr[7:0]));
    subcell sc23_1_MSB (.in(T0_D2_MSB_SBOX_wr[7:0]), .out(T1_D2_MSB_SBOX_wr[7:0]));
    subcell sc23_2_MSB (.in(T1_D2_MSB_SBOX_wr[7:0]), .out(T2_D2_MSB_SBOX_wr[7:0]));
    subcell_last sc23_3_MSB (.in(T2_D2_MSB_SBOX_wr[7:0]), .out(T3_D2_MSB_SBOX_wr[7:0]));
	
	/*D3*/
	subcell sc30_0_MSB (.in(S3_MSB_wr[31:24]), .out(T0_D3_MSB_SBOX_wr[31:24]));
    subcell sc30_1_MSB (.in(T0_D3_MSB_SBOX_wr[31:24]), .out(T1_D3_MSB_SBOX_wr[31:24]));
    subcell sc30_2_MSB (.in(T1_D3_MSB_SBOX_wr[31:24]), .out(T2_D3_MSB_SBOX_wr[31:24]));
    subcell_last sc30_3_MSB (.in(T2_D3_MSB_SBOX_wr[31:24]), .out(T3_D3_MSB_SBOX_wr[31:24]));
	
	subcell sc31_0_MSB (.in(S3_MSB_wr[23:16]), .out(T0_D3_MSB_SBOX_wr[23:16]));
    subcell sc31_1_MSB (.in(T0_D3_MSB_SBOX_wr[23:16]), .out(T1_D3_MSB_SBOX_wr[23:16]));
    subcell sc31_2_MSB (.in(T1_D3_MSB_SBOX_wr[23:16]), .out(T2_D3_MSB_SBOX_wr[23:16]));
    subcell_last sc31_3_MSB (.in(T2_D3_MSB_SBOX_wr[23:16]), .out(T3_D3_MSB_SBOX_wr[23:16]));
	
	subcell sc32_0_MSB (.in(S3_MSB_wr[15:8]), .out(T0_D3_MSB_SBOX_wr[15:8]));
    subcell sc32_1_MSB (.in(T0_D3_MSB_SBOX_wr[15:8]), .out(T1_D3_MSB_SBOX_wr[15:8]));
    subcell sc32_2_MSB (.in(T1_D3_MSB_SBOX_wr[15:8]), .out(T2_D3_MSB_SBOX_wr[15:8]));
    subcell_last sc32_3_MSB (.in(T2_D3_MSB_SBOX_wr[15:8]), .out(T3_D3_MSB_SBOX_wr[15:8]));
	
	subcell sc33_0_MSB (.in(S3_MSB_wr[7:0]), .out(T0_D3_MSB_SBOX_wr[7:0]));
    subcell sc33_1_MSB (.in(T0_D3_MSB_SBOX_wr[7:0]), .out(T1_D3_MSB_SBOX_wr[7:0]));
    subcell sc33_2_MSB (.in(T1_D3_MSB_SBOX_wr[7:0]), .out(T2_D3_MSB_SBOX_wr[7:0]));
    subcell_last sc33_3_MSB (.in(T2_D3_MSB_SBOX_wr[7:0]), .out(T3_D3_MSB_SBOX_wr[7:0]));
	///////////////////////////////////////////////
	///*** 32-bit MSB Customized Calculation ***///
	///////////////////////////////////////////////
	
	always @(*) begin
		case (CFG_in)
		`EXE_CT_NOP: begin   ///*** No Operation ***///
			D0_MSB_rg = S0_MSB_wr; // Pass through S0_in
			D1_MSB_rg = S1_MSB_wr; // Pass through S1_in
			D2_MSB_rg = S2_MSB_wr; // Pass through S2_in
			D3_MSB_rg = S3_MSB_wr; // Pass through S3_in
		end
		`EXE_CT_GW3: begin   ///*** SubBytes using SBOX ***///
			D0_MSB_rg = {D0_MSB_SBOX_wr[23:0],D0_MSB_SBOX_wr[31:24]}; // SBOX for 32-bit S0_in
			D1_MSB_rg = S1_MSB_wr; // SBOX for 32-bit S1_in
			D2_MSB_rg = S2_MSB_wr; // SBOX for 32-bit S2_in
			D3_MSB_rg = S3_MSB_wr; // SBOX for 32-bit S3_in
		end
		`EXE_CT_SUB_SHF: begin   ///*** SubBytes using SBOX +Shift Row ***///		   
			D0_MSB_rg = D0_MSB_SHF_wr; // SBOX for 32-bit D0_SHF_wr
			D1_MSB_rg = D1_MSB_SHF_wr; // SBOX for 32-bit D1_SHF_wr
			D2_MSB_rg = D2_MSB_SHF_wr; // SBOX for 32-bit D2_SHF_wr
			D3_MSB_rg = D3_MSB_SHF_wr; // SBOX for 32-bit D3_SHF_wr
		end
		`EXE_CT_MIXCOL: begin   ///*** Mix Column ***///
			D0_MSB_rg = D0_MSB_mixcol_wr;
			D1_MSB_rg = D1_MSB_mixcol_wr;
			D2_MSB_rg = D2_MSB_mixcol_wr;
			D3_MSB_rg = D3_MSB_mixcol_wr;
		end
		`EXE_CT_ISHF_ISUB: begin   ///*** SubBytes using SBOX +Shift Row ***///		   
			D0_MSB_rg = D0_MSB_ISBOX_wr; // SBOX for 32-bit D0_SHF_wr
			D1_MSB_rg = D1_MSB_ISBOX_wr; // SBOX for 32-bit D1_SHF_wr
			D2_MSB_rg = D2_MSB_ISBOX_wr; // SBOX for 32-bit D2_SHF_wr
			D3_MSB_rg = D3_MSB_ISBOX_wr; // SBOX for 32-bit D3_SHF_wr
		end
		`EXE_CT_IMIXCOL: begin   ///*** Mix Column ***///
			D0_MSB_rg = D0_MSB_inv_mixcol_wr;
			D1_MSB_rg = D1_MSB_inv_mixcol_wr;
			D2_MSB_rg = D2_MSB_inv_mixcol_wr;
			D3_MSB_rg = D3_MSB_inv_mixcol_wr;
		end
		`EXE_CT_SUM01: begin   ///*** SUm function 0 1 for SHA-256***///
			D0_MSB_rg = {S0_MSB_wr[1:0],S0_MSB_wr[31:2]} ^ {S0_MSB_wr[12:0],S0_MSB_wr[31:13]} ^ {S0_MSB_wr[21:0],S0_MSB_wr[31:22]};
			D1_MSB_rg = {S1_MSB_wr[5:0],S1_MSB_wr[31:6]} ^ {S1_MSB_wr[10:0],S1_MSB_wr[31:11]} ^ {S1_MSB_wr[24:0],S1_MSB_wr[31:25]};
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_SIG01: begin   ///*** Sigma FUnction 0 1 for SHA-256 ***///
			D0_MSB_rg = {S0_MSB_wr[6:0],S0_MSB_wr[31:7]} ^ {S0_MSB_wr[17:0],S0_MSB_wr[31:18]} ^ (S0_MSB_wr >>  3);
			D1_MSB_rg = {S1_MSB_wr[16:0],S1_MSB_wr[31:17]} ^ {S1_MSB_wr[18:0],S1_MSB_wr[31:19]} ^ (S1_MSB_wr >>  10);
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_FF1: begin   ///*** Mix Column ***///
			D0_MSB_rg = ((S0_MSB_wr) & (S1_MSB_wr)) | ( (S0_MSB_wr) & (S2_MSB_wr)) | ( (S1_MSB_wr) & (S2_MSB_wr));
			D1_MSB_rg = S1_MSB_wr;
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_P0: begin   ///*** Mix Column ***///
			D0_MSB_rg = S0_MSB_wr ^ {S0_MSB_wr[22:0], S0_MSB_wr[31:23]} ^ {S0_MSB_wr[14:0], S0_MSB_wr[31:15]};
			D1_MSB_rg = S1_MSB_wr;
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_SBOX_SM4: begin   ///*** SBOX SM4 ***///
			D0_MSB_rg = D0_MSB_SBOX_SM4_wr;
			D1_MSB_rg = S1_MSB_wr;
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_4_RX_SM4: begin   ///*** RX SM4 ***///
			D0_MSB_rg = {S0_MSB_wr[29:0], S0_MSB_wr[31:30]} ^ {S0_MSB_wr[21:0], S0_MSB_wr[31:22]} ^  {S0_MSB_wr[13:0], S0_MSB_wr[31:14]} ^  {S0_MSB_wr[7:0], S0_MSB_wr[31:8]}; 
			D1_MSB_rg = S1_MSB_wr;
			D2_MSB_rg = S2_MSB_wr;
			D3_MSB_rg = S3_MSB_wr;
		end
		`EXE_CT_SBOX_8: begin   ///*** RX SM4 ***///
			D0_MSB_rg = T3_D0_MSB_SBOX_wr; 
			D1_MSB_rg = T3_D1_MSB_SBOX_wr;
			D2_MSB_rg = T3_D2_MSB_SBOX_wr;
			D3_MSB_rg = T3_D3_MSB_SBOX_wr;
		end
		////
		/// Define Other Function Here
		///
		default: begin
			D0_MSB_rg = S0_MSB_wr; // Pass through S0_in
			D1_MSB_rg = S1_MSB_wr; // Pass through S1_in
			D2_MSB_rg = S2_MSB_wr; // Pass through S2_in
			D3_MSB_rg = S3_MSB_wr; // Pass through S3_in
		end
		endcase
	end

	////////////////////////////////////
	///*** 32-bit LSB Calculation ***///
	////////////////////////////////////	
	assign S0_LSB_wr = S0_in[`WORD_BITS-1:0];
	assign S1_LSB_wr = S1_in[`WORD_BITS-1:0];
	assign S2_LSB_wr = S2_in[`WORD_BITS-1:0];
	assign S3_LSB_wr = S3_in[`WORD_BITS-1:0];
	
	/// For EXE_CT_SUB_SHF
	SBOX sbox00_LSB (.x_in(S0_LSB_wr[31:24]),.y_out(D0_LSB_SBOX_wr[31:24]));
	SBOX sbox01_LSB (.x_in(S0_LSB_wr[23:16]),.y_out(D0_LSB_SBOX_wr[23:16]));
	SBOX sbox02_LSB (.x_in(S0_LSB_wr[15:8]), .y_out(D0_LSB_SBOX_wr[15:8]));
	SBOX sbox03_LSB (.x_in(S0_LSB_wr[7:0]),  .y_out(D0_LSB_SBOX_wr[7:0]));
	
	SBOX sbox10_LSB (.x_in(S1_LSB_wr[31:24]),.y_out(D1_LSB_SBOX_wr[31:24]));
	SBOX sbox11_LSB (.x_in(S1_LSB_wr[23:16]),.y_out(D1_LSB_SBOX_wr[23:16]));
	SBOX sbox12_LSB (.x_in(S1_LSB_wr[15:8]), .y_out(D1_LSB_SBOX_wr[15:8]));
	SBOX sbox13_LSB (.x_in(S1_LSB_wr[7:0]),  .y_out(D1_LSB_SBOX_wr[7:0]));
	
	SBOX sbox20_LSB (.x_in(S2_LSB_wr[31:24]),.y_out(D2_LSB_SBOX_wr[31:24]));
	SBOX sbox21_LSB (.x_in(S2_LSB_wr[23:16]),.y_out(D2_LSB_SBOX_wr[23:16]));
	SBOX sbox22_LSB (.x_in(S2_LSB_wr[15:8]), .y_out(D2_LSB_SBOX_wr[15:8]));
	SBOX sbox23_LSB (.x_in(S2_LSB_wr[7:0]),  .y_out(D2_LSB_SBOX_wr[7:0]));
	
	SBOX sbox30_LSB (.x_in(S3_LSB_wr[31:24]),.y_out(D3_LSB_SBOX_wr[31:24]));
	SBOX sbox31_LSB (.x_in(S3_LSB_wr[23:16]),.y_out(D3_LSB_SBOX_wr[23:16]));
	SBOX sbox32_LSB (.x_in(S3_LSB_wr[15:8]), .y_out(D3_LSB_SBOX_wr[15:8]));
	SBOX sbox33_LSB (.x_in(S3_LSB_wr[7:0]),  .y_out(D3_LSB_SBOX_wr[7:0]));  
  
	/// For EXE_CT_SUB_SHF
	assign D0_LSB_SHF_wr = {D0_LSB_SBOX_wr[31:24],D1_LSB_SBOX_wr[23:16],D2_LSB_SBOX_wr[15:8],D3_LSB_SBOX_wr[7:0]}; // SBOX for 32-bit S0_in + Shift Row 0
	assign D1_LSB_SHF_wr = {D1_LSB_SBOX_wr[31:24],D2_LSB_SBOX_wr[23:16],D3_LSB_SBOX_wr[15:8],D0_LSB_SBOX_wr[7:0]}; // SBOX for 32-bit S1_in + Shift Row 0
	assign D2_LSB_SHF_wr = {D2_LSB_SBOX_wr[31:24],D3_LSB_SBOX_wr[23:16],D0_LSB_SBOX_wr[15:8],D1_LSB_SBOX_wr[7:0]}; // SBOX for 32-bit S2_in + Shift Row 0
	assign D3_LSB_SHF_wr = {D3_LSB_SBOX_wr[31:24],D0_LSB_SBOX_wr[23:16],D1_LSB_SBOX_wr[15:8],D2_LSB_SBOX_wr[7:0]}; // SBOX for 32-bit S3_in + Shift Row 0
	
	/// For SBOX SM4
	SBOX_SM4 sbox00_sm4_LSB (.x_in(S0_LSB_wr[31:24]),.y_out(D0_LSB_SBOX_SM4_wr[31:24]));
	SBOX_SM4 sbox01_sm4_LSB (.x_in(S0_LSB_wr[23:16]),.y_out(D0_LSB_SBOX_SM4_wr[23:16]));
	SBOX_SM4 sbox02_sm4_LSB (.x_in(S0_LSB_wr[15:8]), .y_out(D0_LSB_SBOX_SM4_wr[15:8]));
	SBOX_SM4 sbox03_sm4_LSB (.x_in(S0_LSB_wr[7:0]),  .y_out(D0_LSB_SBOX_SM4_wr[7:0]));
	
	/// For EXE_CT_ISHF_ISUB
	
	assign D0_LSB_ISHF_wr = {S0_LSB_wr[31:24],S3_LSB_wr[23:16],S2_LSB_wr[15:8],S1_LSB_wr[7:0]}; // SBOX for 32-bit S0_in + Shift Row 0
	assign D1_LSB_ISHF_wr = {S1_LSB_wr[31:24],S0_LSB_wr[23:16],S3_LSB_wr[15:8],S2_LSB_wr[7:0]}; // SBOX for 32-bit S1_in + Shift Row 0
	assign D2_LSB_ISHF_wr = {S2_LSB_wr[31:24],S1_LSB_wr[23:16],S0_LSB_wr[15:8],S3_LSB_wr[7:0]}; // SBOX for 32-bit S2_in + Shift Row 0
	assign D3_LSB_ISHF_wr = {S3_LSB_wr[31:24],S2_LSB_wr[23:16],S1_LSB_wr[15:8],S0_LSB_wr[7:0]}; // SBOX for 32-bit S3_in + Shift Row 0
	
	INV_SBOX inv_sbox00_LSB (.x_in(D0_LSB_ISHF_wr[31:24]),.y_out(D0_LSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox01_LSB (.x_in(D0_LSB_ISHF_wr[23:16]),.y_out(D0_LSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox02_LSB (.x_in(D0_LSB_ISHF_wr[15:8]), .y_out(D0_LSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox03_LSB (.x_in(D0_LSB_ISHF_wr[7:0]),  .y_out(D0_LSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox10_LSB (.x_in(D1_LSB_ISHF_wr[31:24]),.y_out(D1_LSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox11_LSB (.x_in(D1_LSB_ISHF_wr[23:16]),.y_out(D1_LSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox12_LSB (.x_in(D1_LSB_ISHF_wr[15:8]), .y_out(D1_LSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox13_LSB (.x_in(D1_LSB_ISHF_wr[7:0]),  .y_out(D1_LSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox20_LSB (.x_in(D2_LSB_ISHF_wr[31:24]),.y_out(D2_LSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox21_LSB (.x_in(D2_LSB_ISHF_wr[23:16]),.y_out(D2_LSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox22_LSB (.x_in(D2_LSB_ISHF_wr[15:8]), .y_out(D2_LSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox23_LSB (.x_in(D2_LSB_ISHF_wr[7:0]),  .y_out(D2_LSB_ISBOX_wr[7:0]));
															
	INV_SBOX inv_sbox30_LSB (.x_in(D3_LSB_ISHF_wr[31:24]),.y_out(D3_LSB_ISBOX_wr[31:24]));
	INV_SBOX inv_sbox31_LSB (.x_in(D3_LSB_ISHF_wr[23:16]),.y_out(D3_LSB_ISBOX_wr[23:16]));
	INV_SBOX inv_sbox32_LSB (.x_in(D3_LSB_ISHF_wr[15:8]), .y_out(D3_LSB_ISBOX_wr[15:8]));
	INV_SBOX inv_sbox33_LSB (.x_in(D3_LSB_ISHF_wr[7:0]),  .y_out(D3_LSB_ISBOX_wr[7:0]));  
  
	/// For EXE_CT_MIXCOL  
	MIX_COLUMN mixcolumns_LSB
	( 
		.S0_in(S0_LSB_wr),
		.S1_in(S1_LSB_wr),
		.S2_in(S2_LSB_wr),
		.S3_in(S3_LSB_wr),
		.mtx_00_in(4'h2),
		.mtx_01_in(4'h3),
		.mtx_02_in(4'h1),
		.mtx_03_in(4'h1),
		.mtx_10_in(4'h1),
		.mtx_11_in(4'h2),
		.mtx_12_in(4'h3),
		.mtx_13_in(4'h1),
		.mtx_20_in(4'h1),
		.mtx_21_in(4'h1),
		.mtx_22_in(4'h2),
		.mtx_23_in(4'h3),
		.mtx_30_in(4'h3),
		.mtx_31_in(4'h1),
		.mtx_32_in(4'h1),
		.mtx_33_in(4'h2),
		.D0_out(D0_LSB_mixcol_wr),
		.D1_out(D1_LSB_mixcol_wr),
		.D2_out(D2_LSB_mixcol_wr),
		.D3_out(D3_LSB_mixcol_wr)
	);

	/// For EXE_CT_IMIXCOL 
 
	MIX_COLUMN inv_mixcolumns_LSB
	( 
		.S0_in(S0_LSB_wr),
		.S1_in(S1_LSB_wr),
		.S2_in(S2_LSB_wr),
		.S3_in(S3_LSB_wr),
		.mtx_00_in(4'he),
		.mtx_01_in(4'hb),
		.mtx_02_in(4'hd),
		.mtx_03_in(4'h9),
		.mtx_10_in(4'h9),
		.mtx_11_in(4'he),
		.mtx_12_in(4'hb),
		.mtx_13_in(4'hd),
		.mtx_20_in(4'hd),
		.mtx_21_in(4'h9),
		.mtx_22_in(4'he),
		.mtx_23_in(4'hb),
		.mtx_30_in(4'hb),
		.mtx_31_in(4'hd),
		.mtx_32_in(4'h9),
		.mtx_33_in(4'he),
		.D0_out(D0_LSB_inv_mixcol_wr),
		.D1_out(D1_LSB_inv_mixcol_wr),
		.D2_out(D2_LSB_inv_mixcol_wr),
		.D3_out(D3_LSB_inv_mixcol_wr)
	);
	
 	/// For EXE_CT_SBOX_8
	
	/*D0*/
	subcell sc00_0_LSB (.in(S0_LSB_wr[31:24]), .out(T0_D0_LSB_SBOX_wr[31:24]));
    subcell sc00_1_LSB (.in(T0_D0_LSB_SBOX_wr[31:24]), .out(T1_D0_LSB_SBOX_wr[31:24]));
    subcell sc00_2_LSB (.in(T1_D0_LSB_SBOX_wr[31:24]), .out(T2_D0_LSB_SBOX_wr[31:24]));
    subcell_last sc00_3_LSB (.in(T2_D0_LSB_SBOX_wr[31:24]), .out(T3_D0_LSB_SBOX_wr[31:24]));
	
	subcell sc01_0_LSB (.in(S0_LSB_wr[23:16]), .out(T0_D0_LSB_SBOX_wr[23:16]));
    subcell sc01_1_LSB (.in(T0_D0_LSB_SBOX_wr[23:16]), .out(T1_D0_LSB_SBOX_wr[23:16]));
    subcell sc01_2_LSB (.in(T1_D0_LSB_SBOX_wr[23:16]), .out(T2_D0_LSB_SBOX_wr[23:16]));
    subcell_last sc01_3_LSB (.in(T2_D0_LSB_SBOX_wr[23:16]), .out(T3_D0_LSB_SBOX_wr[23:16]));
	
	subcell sc02_0_LSB (.in(S0_LSB_wr[15:8]), .out(T0_D0_LSB_SBOX_wr[15:8]));
    subcell sc02_1_LSB (.in(T0_D0_LSB_SBOX_wr[15:8]), .out(T1_D0_LSB_SBOX_wr[15:8]));
    subcell sc02_2_LSB (.in(T1_D0_LSB_SBOX_wr[15:8]), .out(T2_D0_LSB_SBOX_wr[15:8]));
    subcell_last sc02_3_LSB (.in(T2_D0_LSB_SBOX_wr[15:8]), .out(T3_D0_LSB_SBOX_wr[15:8]));
	
	subcell sc03_0_LSB (.in(S0_LSB_wr[7:0]), .out(T0_D0_LSB_SBOX_wr[7:0]));
    subcell sc03_1_LSB (.in(T0_D0_LSB_SBOX_wr[7:0]), .out(T1_D0_LSB_SBOX_wr[7:0]));
    subcell sc03_2_LSB (.in(T1_D0_LSB_SBOX_wr[7:0]), .out(T2_D0_LSB_SBOX_wr[7:0]));
    subcell_last sc03_3_LSB (.in(T2_D0_LSB_SBOX_wr[7:0]), .out(T3_D0_LSB_SBOX_wr[7:0]));
	
	/*D1*/
	subcell sc10_0_LSB (.in(S1_LSB_wr[31:24]), .out(T0_D1_LSB_SBOX_wr[31:24]));
    subcell sc10_1_LSB (.in(T0_D1_LSB_SBOX_wr[31:24]), .out(T1_D1_LSB_SBOX_wr[31:24]));
    subcell sc10_2_LSB (.in(T1_D1_LSB_SBOX_wr[31:24]), .out(T2_D1_LSB_SBOX_wr[31:24]));
    subcell_last sc10_3_LSB (.in(T2_D1_LSB_SBOX_wr[31:24]), .out(T3_D1_LSB_SBOX_wr[31:24]));
	
	subcell sc11_0_LSB (.in(S1_LSB_wr[23:16]), .out(T0_D1_LSB_SBOX_wr[23:16]));
    subcell sc11_1_LSB (.in(T0_D1_LSB_SBOX_wr[23:16]), .out(T1_D1_LSB_SBOX_wr[23:16]));
    subcell sc11_2_LSB (.in(T1_D1_LSB_SBOX_wr[23:16]), .out(T2_D1_LSB_SBOX_wr[23:16]));
    subcell_last sc11_3_LSB (.in(T2_D1_LSB_SBOX_wr[23:16]), .out(T3_D1_LSB_SBOX_wr[23:16]));
	
	subcell sc12_0_LSB (.in(S1_LSB_wr[15:8]), .out(T0_D1_LSB_SBOX_wr[15:8]));
    subcell sc12_1_LSB (.in(T0_D1_LSB_SBOX_wr[15:8]), .out(T1_D1_LSB_SBOX_wr[15:8]));
    subcell sc12_2_LSB (.in(T1_D1_LSB_SBOX_wr[15:8]), .out(T2_D1_LSB_SBOX_wr[15:8]));
    subcell_last sc12_3_LSB (.in(T2_D1_LSB_SBOX_wr[15:8]), .out(T3_D1_LSB_SBOX_wr[15:8]));
	
	subcell sc13_0_LSB (.in(S1_LSB_wr[7:0]), .out(T0_D1_LSB_SBOX_wr[7:0]));
    subcell sc13_1_LSB (.in(T0_D1_LSB_SBOX_wr[7:0]), .out(T1_D1_LSB_SBOX_wr[7:0]));
    subcell sc13_2_LSB (.in(T1_D1_LSB_SBOX_wr[7:0]), .out(T2_D1_LSB_SBOX_wr[7:0]));
    subcell_last sc13_3_LSB (.in(T2_D1_LSB_SBOX_wr[7:0]), .out(T3_D1_LSB_SBOX_wr[7:0]));
	
	/*D2*/
	subcell sc20_0_LSB (.in(S2_LSB_wr[31:24]), .out(T0_D2_LSB_SBOX_wr[31:24]));
    subcell sc20_1_LSB (.in(T0_D2_LSB_SBOX_wr[31:24]), .out(T1_D2_LSB_SBOX_wr[31:24]));
    subcell sc20_2_LSB (.in(T1_D2_LSB_SBOX_wr[31:24]), .out(T2_D2_LSB_SBOX_wr[31:24]));
    subcell_last sc20_3_LSB (.in(T2_D2_LSB_SBOX_wr[31:24]), .out(T3_D2_LSB_SBOX_wr[31:24]));
	
	subcell sc21_0_LSB (.in(S2_LSB_wr[23:16]), .out(T0_D2_LSB_SBOX_wr[23:16]));
    subcell sc21_1_LSB (.in(T0_D2_LSB_SBOX_wr[23:16]), .out(T1_D2_LSB_SBOX_wr[23:16]));
    subcell sc21_2_LSB (.in(T1_D2_LSB_SBOX_wr[23:16]), .out(T2_D2_LSB_SBOX_wr[23:16]));
    subcell_last sc21_3_LSB (.in(T2_D2_LSB_SBOX_wr[23:16]), .out(T3_D2_LSB_SBOX_wr[23:16]));
	
	subcell sc22_0_LSB (.in(S2_LSB_wr[15:8]), .out(T0_D2_LSB_SBOX_wr[15:8]));
    subcell sc22_1_LSB (.in(T0_D2_LSB_SBOX_wr[15:8]), .out(T1_D2_LSB_SBOX_wr[15:8]));
    subcell sc22_2_LSB (.in(T1_D2_LSB_SBOX_wr[15:8]), .out(T2_D2_LSB_SBOX_wr[15:8]));
    subcell_last sc22_3_LSB (.in(T2_D2_LSB_SBOX_wr[15:8]), .out(T3_D2_LSB_SBOX_wr[15:8]));
	
	subcell sc23_0_LSB (.in(S2_LSB_wr[7:0]), .out(T0_D2_LSB_SBOX_wr[7:0]));
    subcell sc23_1_LSB (.in(T0_D2_LSB_SBOX_wr[7:0]), .out(T1_D2_LSB_SBOX_wr[7:0]));
    subcell sc23_2_LSB (.in(T1_D2_LSB_SBOX_wr[7:0]), .out(T2_D2_LSB_SBOX_wr[7:0]));
    subcell_last sc23_3_LSB (.in(T2_D2_LSB_SBOX_wr[7:0]), .out(T3_D2_LSB_SBOX_wr[7:0]));
	
	/*D3*/
	subcell sc30_0_LSB (.in(S3_LSB_wr[31:24]), .out(T0_D3_LSB_SBOX_wr[31:24]));
    subcell sc30_1_LSB (.in(T0_D3_LSB_SBOX_wr[31:24]), .out(T1_D3_LSB_SBOX_wr[31:24]));
    subcell sc30_2_LSB (.in(T1_D3_LSB_SBOX_wr[31:24]), .out(T2_D3_LSB_SBOX_wr[31:24]));
    subcell_last sc30_3_LSB (.in(T2_D3_LSB_SBOX_wr[31:24]), .out(T3_D3_LSB_SBOX_wr[31:24]));
	
	subcell sc31_0_LSB (.in(S3_LSB_wr[23:16]), .out(T0_D3_LSB_SBOX_wr[23:16]));
    subcell sc31_1_LSB (.in(T0_D3_LSB_SBOX_wr[23:16]), .out(T1_D3_LSB_SBOX_wr[23:16]));
    subcell sc31_2_LSB (.in(T1_D3_LSB_SBOX_wr[23:16]), .out(T2_D3_LSB_SBOX_wr[23:16]));
    subcell_last sc31_3_LSB (.in(T2_D3_LSB_SBOX_wr[23:16]), .out(T3_D3_LSB_SBOX_wr[23:16]));
	
	subcell sc32_0_LSB (.in(S3_LSB_wr[15:8]), .out(T0_D3_LSB_SBOX_wr[15:8]));
    subcell sc32_1_LSB (.in(T0_D3_LSB_SBOX_wr[15:8]), .out(T1_D3_LSB_SBOX_wr[15:8]));
    subcell sc32_2_LSB (.in(T1_D3_LSB_SBOX_wr[15:8]), .out(T2_D3_LSB_SBOX_wr[15:8]));
    subcell_last sc32_3_LSB (.in(T2_D3_LSB_SBOX_wr[15:8]), .out(T3_D3_LSB_SBOX_wr[15:8]));
	
	subcell sc33_0_LSB (.in(S3_LSB_wr[7:0]), .out(T0_D3_LSB_SBOX_wr[7:0]));
    subcell sc33_1_LSB (.in(T0_D3_LSB_SBOX_wr[7:0]), .out(T1_D3_LSB_SBOX_wr[7:0]));
    subcell sc33_2_LSB (.in(T1_D3_LSB_SBOX_wr[7:0]), .out(T2_D3_LSB_SBOX_wr[7:0]));
    subcell_last sc33_3_LSB (.in(T2_D3_LSB_SBOX_wr[7:0]), .out(T3_D3_LSB_SBOX_wr[7:0]));
 
	///////////////////////////////////////////////
	///*** 32-bit LSB Customized Calculation ***///
	///////////////////////////////////////////////
	always @(*) begin
		case (CFG_in)
		`EXE_CT_NOP: begin   ///*** No Operation ***///
			D0_LSB_rg = S0_LSB_wr; // Pass through S0_in
			D1_LSB_rg = S1_LSB_wr; // Pass through S1_in
			D2_LSB_rg = S2_LSB_wr; // Pass through S2_in
			D3_LSB_rg = S3_LSB_wr; // Pass through S3_in
		end
		`EXE_CT_GW3: begin   ///*** SubBytes using SBOX ***///
			D0_LSB_rg = {D0_LSB_SBOX_wr[23:0],D0_LSB_SBOX_wr[31:24]}; // SBOX for 32-bit S0_in
			D1_LSB_rg = S1_LSB_wr; // SBOX for 32-bit S1_in
			D2_LSB_rg = S2_LSB_wr; // SBOX for 32-bit S2_in
			D3_LSB_rg = S3_LSB_wr; // SBOX for 32-bit S3_in
		end
		`EXE_CT_SUB_SHF: begin   ///*** SubBytes using SBOX +Shift Row ***///		   
			D0_LSB_rg = D0_LSB_SHF_wr; // SBOX for 32-bit D0_SHF_wr
			D1_LSB_rg = D1_LSB_SHF_wr; // SBOX for 32-bit D1_SHF_wr
			D2_LSB_rg = D2_LSB_SHF_wr; // SBOX for 32-bit D2_SHF_wr
			D3_LSB_rg = D3_LSB_SHF_wr; // SBOX for 32-bit D3_SHF_wr
		end
		`EXE_CT_MIXCOL: begin   ///*** Mix Column ***///
			D0_LSB_rg = D0_LSB_mixcol_wr;
			D1_LSB_rg = D1_LSB_mixcol_wr;
			D2_LSB_rg = D2_LSB_mixcol_wr;
			D3_LSB_rg = D3_LSB_mixcol_wr;
		end
		`EXE_CT_ISHF_ISUB: begin   ///*** SubBytes using SBOX +Shift Row ***///		   
			D0_LSB_rg = D0_LSB_ISBOX_wr; // SBOX for 32-bit D0_SHF_wr
			D1_LSB_rg = D1_LSB_ISBOX_wr; // SBOX for 32-bit D1_SHF_wr
			D2_LSB_rg = D2_LSB_ISBOX_wr; // SBOX for 32-bit D2_SHF_wr
			D3_LSB_rg = D3_LSB_ISBOX_wr; // SBOX for 32-bit D3_SHF_wr
		end
		`EXE_CT_IMIXCOL: begin   ///*** Mix Column ***///
			D0_LSB_rg = D0_LSB_inv_mixcol_wr;
			D1_LSB_rg = D1_LSB_inv_mixcol_wr;
			D2_LSB_rg = D2_LSB_inv_mixcol_wr;
			D3_LSB_rg = D3_LSB_inv_mixcol_wr;
		end
		`EXE_CT_SUM01: begin   ///*** SUm function 0 1 for SHA-256***///
			D0_LSB_rg = {S0_LSB_wr[1:0],S0_LSB_wr[31:2]} ^ {S0_LSB_wr[12:0],S0_LSB_wr[31:13]} ^ {S0_LSB_wr[21:0],S0_LSB_wr[31:22]};
			D1_LSB_rg = {S1_LSB_wr[5:0],S1_LSB_wr[31:6]} ^ {S1_LSB_wr[10:0],S1_LSB_wr[31:11]} ^ {S1_LSB_wr[24:0],S1_LSB_wr[31:25]};
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_SIG01: begin   ///*** Sigma FUnction 0 1 for SHA-256 ***///
			D0_LSB_rg = {S0_LSB_wr[6:0],S0_LSB_wr[31:7]} ^ {S0_LSB_wr[17:0],S0_LSB_wr[31:18]} ^ (S0_LSB_wr >>  3);
			D1_LSB_rg = {S1_LSB_wr[16:0],S1_LSB_wr[31:17]} ^ {S1_LSB_wr[18:0],S1_LSB_wr[31:19]} ^ (S1_LSB_wr >>  10);
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_FF1: begin   ///*** Mix Column ***///
			D0_LSB_rg = ((S0_LSB_wr) & (S1_LSB_wr)) | ( (S0_LSB_wr) & (S2_LSB_wr)) | ( (S1_LSB_wr) & (S2_LSB_wr));
			D1_LSB_rg = S1_LSB_wr;
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_P0: begin   ///*** Mix Column ***///
			D0_LSB_rg = S0_LSB_wr ^ {S0_LSB_wr[22:0], S0_LSB_wr[31:23]} ^ {S0_LSB_wr[14:0], S0_LSB_wr[31:15]};
			D1_LSB_rg = S1_LSB_wr;
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_SBOX_SM4: begin   ///*** SBOX SM4 ***///
			D0_LSB_rg = D0_LSB_SBOX_SM4_wr;
			D1_LSB_rg = S1_LSB_wr;
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_4_RX_SM4: begin   ///*** RX SM4 ***///
			D0_LSB_rg = {S0_LSB_wr[29:0], S0_LSB_wr[31:30]} ^ {S0_LSB_wr[21:0], S0_LSB_wr[31:22]} ^  {S0_LSB_wr[13:0], S0_LSB_wr[31:14]} ^  {S0_LSB_wr[7:0], S0_LSB_wr[31:8]}; 
			D1_LSB_rg = S1_LSB_wr;
			D2_LSB_rg = S2_LSB_wr;
			D3_LSB_rg = S3_LSB_wr;
		end
		`EXE_CT_SBOX_8: begin   ///*** RX SM4 ***///
			D0_LSB_rg = T3_D0_LSB_SBOX_wr; 
			D1_LSB_rg = T3_D1_LSB_SBOX_wr;
			D2_LSB_rg = T3_D2_LSB_SBOX_wr;
			D3_LSB_rg = T3_D3_LSB_SBOX_wr;
		end
		////
		/// Define Other Function Here
		///
		default: begin
			D0_LSB_rg = S0_LSB_wr; // Pass through S0_in
			D1_LSB_rg = S1_LSB_wr; // Pass through S1_in
			D2_LSB_rg = S2_LSB_wr; // Pass through S2_in
			D3_LSB_rg = S3_LSB_wr; // Pass through S3_in
		end
		endcase
	end
	
	/////////////////////////////////
	///*** 64-bit Calculation ***///
	////////////////////////////////	
	
	always @(*) begin
		case (CFG_in)
		`EXE_CT_64_NOP: begin   ///*** No Operation ***///
			D0_64_rg = S0_in; // Pass through S0_in
			D1_64_rg = S1_in; // Pass through S1_in
			D2_64_rg = S2_in; // Pass through S2_in
			D3_64_rg = S3_in; // Pass through S3_in
		end
		////
		/// Define Other Function Here
		///
		default: begin
			D0_64_rg = S0_in; // Pass through S0_in
			D1_64_rg = S1_in; // Pass through S1_in
			D2_64_rg = S2_in; // Pass through S2_in
			D3_64_rg = S3_in; // Pass through S3_in
		end
		endcase
	end
	
	assign D0_wr = (Mode_in == `MODE32)? {D0_MSB_rg,D0_LSB_rg}:D0_64_rg;	
	assign D1_wr = (Mode_in == `MODE32)? {D1_MSB_rg,D1_LSB_rg}:D1_64_rg;	
	assign D2_wr = (Mode_in == `MODE32)? {D2_MSB_rg,D2_LSB_rg}:D2_64_rg;	
	assign D3_wr = (Mode_in == `MODE32)? {D3_MSB_rg,D3_LSB_rg}:D3_64_rg;	
	
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			D0_out <= `DWORD_BITS'h0;
			D1_out <= `DWORD_BITS'h0;
			D2_out <= `DWORD_BITS'h0;
			D3_out <= `DWORD_BITS'h0;
		end
		else begin
			if(Finish_in) begin
				D0_out <= `DWORD_BITS'h0;
				D1_out <= `DWORD_BITS'h0;
				D2_out <= `DWORD_BITS'h0;
				D3_out <= `DWORD_BITS'h0;
			end
			else if(En_in) begin
				D0_out <= D0_wr; 
				D1_out <= D1_wr; 
				D2_out <= D2_wr; 
				D3_out <= D3_wr; 
			end
			else begin
				D0_out <= `DWORD_BITS'h0;
				D1_out <= `DWORD_BITS'h0;
				D2_out <= `DWORD_BITS'h0;
				D3_out <= `DWORD_BITS'h0;
			end
		end
	end
 
endmodule

module SBOX(
	input  wire [7:0] x_in,
	output reg  [7:0] y_out
);

	always @(x_in) begin
	case (x_in)
		8'h00 : y_out = 8'h63;
		8'h01 : y_out = 8'h7C;
		8'h02 : y_out = 8'h77;
		8'h03 : y_out = 8'h7B;
		8'h04 : y_out = 8'hF2;
		8'h05 : y_out = 8'h6B;
		8'h06 : y_out = 8'h6F;
		8'h07 : y_out = 8'hC5;
		8'h08 : y_out = 8'h30;
		8'h09 : y_out = 8'h01;
		8'h0A : y_out = 8'h67;
		8'h0B : y_out = 8'h2B;
		8'h0C : y_out = 8'hFE;
		8'h0D : y_out = 8'hD7;
		8'h0E : y_out = 8'hAB;
		8'h0F : y_out = 8'h76;
		8'h10 : y_out = 8'hCA;
		8'h11 : y_out = 8'h82;
		8'h12 : y_out = 8'hC9;
		8'h13 : y_out = 8'h7D;
		8'h14 : y_out = 8'hFA;
		8'h15 : y_out = 8'h59;
		8'h16 : y_out = 8'h47;
		8'h17 : y_out = 8'hF0;
		8'h18 : y_out = 8'hAD;
		8'h19 : y_out = 8'hD4;
		8'h1A : y_out = 8'hA2;
		8'h1B : y_out = 8'hAF;
		8'h1C : y_out = 8'h9C;
		8'h1D : y_out = 8'hA4;
		8'h1E : y_out = 8'h72;
		8'h1F : y_out = 8'hC0;
		8'h20 : y_out = 8'hB7;
		8'h21 : y_out = 8'hFD;
		8'h22 : y_out = 8'h93;
		8'h23 : y_out = 8'h26;
		8'h24 : y_out = 8'h36;
		8'h25 : y_out = 8'h3F;
		8'h26 : y_out = 8'hF7;
		8'h27 : y_out = 8'hCC;
		8'h28 : y_out = 8'h34;
		8'h29 : y_out = 8'hA5;
		8'h2A : y_out = 8'hE5;
		8'h2B : y_out = 8'hF1;
		8'h2C : y_out = 8'h71;
		8'h2D : y_out = 8'hD8;
		8'h2E : y_out = 8'h31;
		8'h2F : y_out = 8'h15;
		8'h30 : y_out = 8'h04;
		8'h31 : y_out = 8'hC7;
		8'h32 : y_out = 8'h23;
		8'h33 : y_out = 8'hC3;
		8'h34 : y_out = 8'h18;
		8'h35 : y_out = 8'h96;
		8'h36 : y_out = 8'h05;
		8'h37 : y_out = 8'h9A;
		8'h38 : y_out = 8'h07;
		8'h39 : y_out = 8'h12;
		8'h3A : y_out = 8'h80;
		8'h3B : y_out = 8'hE2;
		8'h3C : y_out = 8'hEB;
		8'h3D : y_out = 8'h27;
		8'h3E : y_out = 8'hB2;
		8'h3F : y_out = 8'h75;
		8'h40 : y_out = 8'h09;
		8'h41 : y_out = 8'h83;
		8'h42 : y_out = 8'h2C;
		8'h43 : y_out = 8'h1A;
		8'h44 : y_out = 8'h1B;
		8'h45 : y_out = 8'h6E;
		8'h46 : y_out = 8'h5A;
		8'h47 : y_out = 8'hA0;
		8'h48 : y_out = 8'h52;
		8'h49 : y_out = 8'h3B;
		8'h4A : y_out = 8'hD6;
		8'h4B : y_out = 8'hB3;
		8'h4C : y_out = 8'h29;
		8'h4D : y_out = 8'hE3;
		8'h4E : y_out = 8'h2F;
		8'h4F : y_out = 8'h84;
		8'h50 : y_out = 8'h53;
		8'h51 : y_out = 8'hD1;
		8'h52 : y_out = 8'h00;
		8'h53 : y_out = 8'hED;
		8'h54 : y_out = 8'h20;
		8'h55 : y_out = 8'hFC;
		8'h56 : y_out = 8'hB1;
		8'h57 : y_out = 8'h5B;
		8'h58 : y_out = 8'h6A;
		8'h59 : y_out = 8'hCB;
		8'h5A : y_out = 8'hBE;
		8'h5B : y_out = 8'h39;
		8'h5C : y_out = 8'h4A;
		8'h5D : y_out = 8'h4C;
		8'h5E : y_out = 8'h58;
		8'h5F : y_out = 8'hCF;
		8'h60 : y_out = 8'hD0;
		8'h61 : y_out = 8'hEF;
		8'h62 : y_out = 8'hAA;
		8'h63 : y_out = 8'hFB;
		8'h64 : y_out = 8'h43;
		8'h65 : y_out = 8'h4D;
		8'h66 : y_out = 8'h33;
		8'h67 : y_out = 8'h85;
		8'h68 : y_out = 8'h45;
		8'h69 : y_out = 8'hF9;
		8'h6A : y_out = 8'h02;
		8'h6B : y_out = 8'h7F;
		8'h6C : y_out = 8'h50;
		8'h6D : y_out = 8'h3C;
		8'h6E : y_out = 8'h9F;
		8'h6F : y_out = 8'hA8;
		8'h70 : y_out = 8'h51;
		8'h71 : y_out = 8'hA3;
		8'h72 : y_out = 8'h40;
		8'h73 : y_out = 8'h8F;
		8'h74 : y_out = 8'h92;
		8'h75 : y_out = 8'h9D;
		8'h76 : y_out = 8'h38;
		8'h77 : y_out = 8'hF5;
		8'h78 : y_out = 8'hBC;
		8'h79 : y_out = 8'hB6;
		8'h7A : y_out = 8'hDA;
		8'h7B : y_out = 8'h21;
		8'h7C : y_out = 8'h10;
		8'h7D : y_out = 8'hFF;
		8'h7E : y_out = 8'hF3;
		8'h7F : y_out = 8'hD2;
		8'h80 : y_out = 8'hCD;
		8'h81 : y_out = 8'h0C;
		8'h82 : y_out = 8'h13;
		8'h83 : y_out = 8'hEC;
		8'h84 : y_out = 8'h5F;
		8'h85 : y_out = 8'h97;
		8'h86 : y_out = 8'h44;
		8'h87 : y_out = 8'h17;
		8'h88 : y_out = 8'hC4;
		8'h89 : y_out = 8'hA7;
		8'h8A : y_out = 8'h7E;
		8'h8B : y_out = 8'h3D;
		8'h8C : y_out = 8'h64;
		8'h8D : y_out = 8'h5D;
		8'h8E : y_out = 8'h19;
		8'h8F : y_out = 8'h73;
		8'h90 : y_out = 8'h60;
		8'h91 : y_out = 8'h81;
		8'h92 : y_out = 8'h4F;
		8'h93 : y_out = 8'hDC;
		8'h94 : y_out = 8'h22;
		8'h95 : y_out = 8'h2A;
		8'h96 : y_out = 8'h90;
		8'h97 : y_out = 8'h88;
		8'h98 : y_out = 8'h46;
		8'h99 : y_out = 8'hEE;
		8'h9A : y_out = 8'hB8;
		8'h9B : y_out = 8'h14;
		8'h9C : y_out = 8'hDE;
		8'h9D : y_out = 8'h5E;
		8'h9E : y_out = 8'h0B;
		8'h9F : y_out = 8'hDB;
		8'hA0 : y_out = 8'hE0;
		8'hA1 : y_out = 8'h32;
		8'hA2 : y_out = 8'h3A;
		8'hA3 : y_out = 8'h0A;
		8'hA4 : y_out = 8'h49;
		8'hA5 : y_out = 8'h06;
		8'hA6 : y_out = 8'h24;
		8'hA7 : y_out = 8'h5C;
		8'hA8 : y_out = 8'hC2;
		8'hA9 : y_out = 8'hD3;
		8'hAA : y_out = 8'hAC;
		8'hAB : y_out = 8'h62;
		8'hAC : y_out = 8'h91;
		8'hAD : y_out = 8'h95;
		8'hAE : y_out = 8'hE4;
		8'hAF : y_out = 8'h79;
		8'hB0 : y_out = 8'hE7;
		8'hB1 : y_out = 8'hC8;
		8'hB2 : y_out = 8'h37;
		8'hB3 : y_out = 8'h6D;
		8'hB4 : y_out = 8'h8D;
		8'hB5 : y_out = 8'hD5;
		8'hB6 : y_out = 8'h4E;
		8'hB7 : y_out = 8'hA9;
		8'hB8 : y_out = 8'h6C;
		8'hB9 : y_out = 8'h56;
		8'hBA : y_out = 8'hF4;
		8'hBB : y_out = 8'hEA;
		8'hBC : y_out = 8'h65;
		8'hBD : y_out = 8'h7A;
		8'hBE : y_out = 8'hAE;
		8'hBF : y_out = 8'h08;
		8'hC0 : y_out = 8'hBA;
		8'hC1 : y_out = 8'h78;
		8'hC2 : y_out = 8'h25;
		8'hC3 : y_out = 8'h2E;
		8'hC4 : y_out = 8'h1C;
		8'hC5 : y_out = 8'hA6;
		8'hC6 : y_out = 8'hB4;
		8'hC7 : y_out = 8'hC6;
		8'hC8 : y_out = 8'hE8;
		8'hC9 : y_out = 8'hDD;
		8'hCA : y_out = 8'h74;
		8'hCB : y_out = 8'h1F;
		8'hCC : y_out = 8'h4B;
		8'hCD : y_out = 8'hBD;
		8'hCE : y_out = 8'h8B;
		8'hCF : y_out = 8'h8A;
		8'hD0 : y_out = 8'h70;
		8'hD1 : y_out = 8'h3E;
		8'hD2 : y_out = 8'hB5;
		8'hD3 : y_out = 8'h66;
		8'hD4 : y_out = 8'h48;
		8'hD5 : y_out = 8'h03;
		8'hD6 : y_out = 8'hF6;
		8'hD7 : y_out = 8'h0E;
		8'hD8 : y_out = 8'h61;
		8'hD9 : y_out = 8'h35;
		8'hDA : y_out = 8'h57;
		8'hDB : y_out = 8'hB9;
		8'hDC : y_out = 8'h86;
		8'hDD : y_out = 8'hC1;
		8'hDE : y_out = 8'h1D;
		8'hDF : y_out = 8'h9E;
		8'hE0 : y_out = 8'hE1;
		8'hE1 : y_out = 8'hF8;
		8'hE2 : y_out = 8'h98;
		8'hE3 : y_out = 8'h11;
		8'hE4 : y_out = 8'h69;
		8'hE5 : y_out = 8'hD9;
		8'hE6 : y_out = 8'h8E;
		8'hE7 : y_out = 8'h94;
		8'hE8 : y_out = 8'h9B;
		8'hE9 : y_out = 8'h1E;
		8'hEA : y_out = 8'h87;
		8'hEB : y_out = 8'hE9;
		8'hEC : y_out = 8'hCE;
		8'hED : y_out = 8'h55;
		8'hEE : y_out = 8'h28;
		8'hEF : y_out = 8'hDF;
		8'hF0 : y_out = 8'h8C;
		8'hF1 : y_out = 8'hA1;
		8'hF2 : y_out = 8'h89;
		8'hF3 : y_out = 8'h0D;
		8'hF4 : y_out = 8'hBF;
		8'hF5 : y_out = 8'hE6;
		8'hF6 : y_out = 8'h42;
		8'hF7 : y_out = 8'h68;
		8'hF8 : y_out = 8'h41;
		8'hF9 : y_out = 8'h99;
		8'hFA : y_out = 8'h2D;
		8'hFB : y_out = 8'h0F;
		8'hFC : y_out = 8'hB0;
		8'hFD : y_out = 8'h54;
		8'hFE : y_out = 8'hBB;
		8'hFF : y_out = 8'h16;
		default : y_out = 8'h63;
	endcase
	end

endmodule

module INV_SBOX(
	input  wire [7:0] x_in,
	output reg  [7:0] y_out
);
	always @(x_in) begin
	case (x_in)
		8'h00 : y_out = 8'h52;
		8'h01 : y_out = 8'h09;
		8'h02 : y_out = 8'h6a;
		8'h03 : y_out = 8'hd5;
		8'h04 : y_out = 8'h30;
		8'h05 : y_out = 8'h36;
		8'h06 : y_out = 8'ha5;
		8'h07 : y_out = 8'h38;
		8'h08 : y_out = 8'hbf;
		8'h09 : y_out = 8'h40;
		8'h0a : y_out = 8'ha3;
		8'h0b : y_out = 8'h9e;
		8'h0c : y_out = 8'h81;
		8'h0d : y_out = 8'hf3;
		8'h0e : y_out = 8'hd7;
		8'h0f : y_out = 8'hfb;
		8'h10 : y_out = 8'h7c;
		8'h11 : y_out = 8'he3;
		8'h12 : y_out = 8'h39;
		8'h13 : y_out = 8'h82;
		8'h14 : y_out = 8'h9b;
		8'h15 : y_out = 8'h2f;
		8'h16 : y_out = 8'hff;
		8'h17 : y_out = 8'h87;
		8'h18 : y_out = 8'h34;
		8'h19 : y_out = 8'h8e;
		8'h1a : y_out = 8'h43;
		8'h1b : y_out = 8'h44;
		8'h1c : y_out = 8'hc4;
		8'h1d : y_out = 8'hde;
		8'h1e : y_out = 8'he9;
		8'h1f : y_out = 8'hcb;
		8'h20 : y_out = 8'h54;
		8'h21 : y_out = 8'h7b;
		8'h22 : y_out = 8'h94;
		8'h23 : y_out = 8'h32;
		8'h24 : y_out = 8'ha6;
		8'h25 : y_out = 8'hc2;
		8'h26 : y_out = 8'h23;
		8'h27 : y_out = 8'h3d;
		8'h28 : y_out = 8'hee;
		8'h29 : y_out = 8'h4c;
		8'h2a : y_out = 8'h95;
		8'h2b : y_out = 8'h0b;
		8'h2c : y_out = 8'h42;
		8'h2d : y_out = 8'hfa;
		8'h2e : y_out = 8'hc3;
		8'h2f : y_out = 8'h4e;
		8'h30 : y_out = 8'h08;
		8'h31 : y_out = 8'h2e;
		8'h32 : y_out = 8'ha1;
		8'h33 : y_out = 8'h66;
		8'h34 : y_out = 8'h28;
		8'h35 : y_out = 8'hd9;
		8'h36 : y_out = 8'h24;
		8'h37 : y_out = 8'hb2;
		8'h38 : y_out = 8'h76;
		8'h39 : y_out = 8'h5b;
		8'h3a : y_out = 8'ha2;
		8'h3b : y_out = 8'h49;
		8'h3c : y_out = 8'h6d;
		8'h3d : y_out = 8'h8b;
		8'h3e : y_out = 8'hd1;
		8'h3f : y_out = 8'h25;
		8'h40 : y_out = 8'h72;
		8'h41 : y_out = 8'hf8;
		8'h42 : y_out = 8'hf6;
		8'h43 : y_out = 8'h64;
		8'h44 : y_out = 8'h86;
		8'h45 : y_out = 8'h68;
		8'h46 : y_out = 8'h98;
		8'h47 : y_out = 8'h16;
		8'h48 : y_out = 8'hd4;
		8'h49 : y_out = 8'ha4;
		8'h4a : y_out = 8'h5c;
		8'h4b : y_out = 8'hcc;
		8'h4c : y_out = 8'h5d;
		8'h4d : y_out = 8'h65;
		8'h4e : y_out = 8'hb6;
		8'h4f : y_out = 8'h92;
		8'h50 : y_out = 8'h6c;
		8'h51 : y_out = 8'h70;
		8'h52 : y_out = 8'h48;
		8'h53 : y_out = 8'h50;
		8'h54 : y_out = 8'hfd;
		8'h55 : y_out = 8'hed;
		8'h56 : y_out = 8'hb9;
		8'h57 : y_out = 8'hda;
		8'h58 : y_out = 8'h5e;
		8'h59 : y_out = 8'h15;
		8'h5a : y_out = 8'h46;
		8'h5b : y_out = 8'h57;
		8'h5c : y_out = 8'ha7;
		8'h5d : y_out = 8'h8d;
		8'h5e : y_out = 8'h9d;
		8'h5f : y_out = 8'h84;
		8'h60 : y_out = 8'h90;
		8'h61 : y_out = 8'hd8;
		8'h62 : y_out = 8'hab;
		8'h63 : y_out = 8'h00;
		8'h64 : y_out = 8'h8c;
		8'h65 : y_out = 8'hbc;
		8'h66 : y_out = 8'hd3;
		8'h67 : y_out = 8'h0a;
		8'h68 : y_out = 8'hf7;
		8'h69 : y_out = 8'he4;
		8'h6a : y_out = 8'h58;
		8'h6b : y_out = 8'h05;
		8'h6c : y_out = 8'hb8;
		8'h6d : y_out = 8'hb3;
		8'h6e : y_out = 8'h45;
		8'h6f : y_out = 8'h06;
		8'h70 : y_out = 8'hd0;
		8'h71 : y_out = 8'h2c;
		8'h72 : y_out = 8'h1e;
		8'h73 : y_out = 8'h8f;
		8'h74 : y_out = 8'hca;
		8'h75 : y_out = 8'h3f;
		8'h76 : y_out = 8'h0f;
		8'h77 : y_out = 8'h02;
		8'h78 : y_out = 8'hc1;
		8'h79 : y_out = 8'haf;
		8'h7a : y_out = 8'hbd;
		8'h7b : y_out = 8'h03;
		8'h7c : y_out = 8'h01;
		8'h7d : y_out = 8'h13;
		8'h7e : y_out = 8'h8a;
		8'h7f : y_out = 8'h6b;
		8'h80 : y_out = 8'h3a;
		8'h81 : y_out = 8'h91;
		8'h82 : y_out = 8'h11;
		8'h83 : y_out = 8'h41;
		8'h84 : y_out = 8'h4f;
		8'h85 : y_out = 8'h67;
		8'h86 : y_out = 8'hdc;
		8'h87 : y_out = 8'hea;
		8'h88 : y_out = 8'h97;
		8'h89 : y_out = 8'hf2;
		8'h8a : y_out = 8'hcf;
		8'h8b : y_out = 8'hce;
		8'h8c : y_out = 8'hf0;
		8'h8d : y_out = 8'hb4;
		8'h8e : y_out = 8'he6;
		8'h8f : y_out = 8'h73;
		8'h90 : y_out = 8'h96;
		8'h91 : y_out = 8'hac;
		8'h92 : y_out = 8'h74;
		8'h93 : y_out = 8'h22;
		8'h94 : y_out = 8'he7;
		8'h95 : y_out = 8'had;
		8'h96 : y_out = 8'h35;
		8'h97 : y_out = 8'h85;
		8'h98 : y_out = 8'he2;
		8'h99 : y_out = 8'hf9;
		8'h9a : y_out = 8'h37;
		8'h9b : y_out = 8'he8;
		8'h9c : y_out = 8'h1c;
		8'h9d : y_out = 8'h75;
		8'h9e : y_out = 8'hdf;
		8'h9f : y_out = 8'h6e;
		8'ha0 : y_out = 8'h47;
		8'ha1 : y_out = 8'hf1;
		8'ha2 : y_out = 8'h1a;
		8'ha3 : y_out = 8'h71;
		8'ha4 : y_out = 8'h1d;
		8'ha5 : y_out = 8'h29;
		8'ha6 : y_out = 8'hc5;
		8'ha7 : y_out = 8'h89;
		8'ha8 : y_out = 8'h6f;
		8'ha9 : y_out = 8'hb7;
		8'haa : y_out = 8'h62;
		8'hab : y_out = 8'h0e;
		8'hac : y_out = 8'haa;
		8'had : y_out = 8'h18;
		8'hae : y_out = 8'hbe;
		8'haf : y_out = 8'h1b;
		8'hb0 : y_out = 8'hfc;
		8'hb1 : y_out = 8'h56;
		8'hb2 : y_out = 8'h3e;
		8'hb3 : y_out = 8'h4b;
		8'hb4 : y_out = 8'hc6;
		8'hb5 : y_out = 8'hd2;
		8'hb6 : y_out = 8'h79;
		8'hb7 : y_out = 8'h20;
		8'hb8 : y_out = 8'h9a;
		8'hb9 : y_out = 8'hdb;
		8'hba : y_out = 8'hc0;
		8'hbb : y_out = 8'hfe;
		8'hbc : y_out = 8'h78;
		8'hbd : y_out = 8'hcd;
		8'hbe : y_out = 8'h5a;
		8'hbf : y_out = 8'hf4;
		8'hc0 : y_out = 8'h1f;
		8'hc1 : y_out = 8'hdd;
		8'hc2 : y_out = 8'ha8;
		8'hc3 : y_out = 8'h33;
		8'hc4 : y_out = 8'h88;
		8'hc5 : y_out = 8'h07;
		8'hc6 : y_out = 8'hc7;
		8'hc7 : y_out = 8'h31;
		8'hc8 : y_out = 8'hb1;
		8'hc9 : y_out = 8'h12;
		8'hca : y_out = 8'h10;
		8'hcb : y_out = 8'h59;
		8'hcc : y_out = 8'h27;
		8'hcd : y_out = 8'h80;
		8'hce : y_out = 8'hec;
		8'hcf : y_out = 8'h5f;
		8'hd0 : y_out = 8'h60;
		8'hd1 : y_out = 8'h51;
		8'hd2 : y_out = 8'h7f;
		8'hd3 : y_out = 8'ha9;
		8'hd4 : y_out = 8'h19;
		8'hd5 : y_out = 8'hb5;
		8'hd6 : y_out = 8'h4a;
		8'hd7 : y_out = 8'h0d;
		8'hd8 : y_out = 8'h2d;
		8'hd9 : y_out = 8'he5;
		8'hda : y_out = 8'h7a;
		8'hdb : y_out = 8'h9f;
		8'hdc : y_out = 8'h93;
		8'hdd : y_out = 8'hc9;
		8'hde : y_out = 8'h9c;
		8'hdf : y_out = 8'hef;
		8'he0 : y_out = 8'ha0;
		8'he1 : y_out = 8'he0;
		8'he2 : y_out = 8'h3b;
		8'he3 : y_out = 8'h4d;
		8'he4 : y_out = 8'hae;
		8'he5 : y_out = 8'h2a;
		8'he6 : y_out = 8'hf5;
		8'he7 : y_out = 8'hb0;
		8'he8 : y_out = 8'hc8;
		8'he9 : y_out = 8'heb;
		8'hea : y_out = 8'hbb;
		8'heb : y_out = 8'h3c;
		8'hec : y_out = 8'h83;
		8'hed : y_out = 8'h53;
		8'hee : y_out = 8'h99;
		8'hef : y_out = 8'h61;
		8'hf0 : y_out = 8'h17;
		8'hf1 : y_out = 8'h2b;
		8'hf2 : y_out = 8'h04;
		8'hf3 : y_out = 8'h7e;
		8'hf4 : y_out = 8'hba;
		8'hf5 : y_out = 8'h77;
		8'hf6 : y_out = 8'hd6;
		8'hf7 : y_out = 8'h26;
		8'hf8 : y_out = 8'he1;
		8'hf9 : y_out = 8'h69;
		8'hfa : y_out = 8'h14;
		8'hfb : y_out = 8'h63;
		8'hfc : y_out = 8'h55;
		8'hfd : y_out = 8'h21;
		8'hfe : y_out = 8'h0c;
		8'hff : y_out = 8'h7d;
		default : y_out = 8'h52;
	endcase
	end

endmodule


module MIX_COLUMN
( 
	input wire [`WORD_BITS-1:0]  	S0_in,
	input wire [`WORD_BITS-1:0]  	S1_in,
	input wire [`WORD_BITS-1:0]  	S2_in,
	input wire [`WORD_BITS-1:0] 	S3_in,
	input wire [3:0] 				mtx_00_in,
	input wire [3:0] 				mtx_01_in,
	input wire [3:0] 				mtx_02_in,
	input wire [3:0] 				mtx_03_in,
	input wire [3:0] 				mtx_10_in,
	input wire [3:0] 				mtx_11_in,
	input wire [3:0] 				mtx_12_in,
	input wire [3:0] 				mtx_13_in,
	input wire [3:0] 				mtx_20_in,
	input wire [3:0] 				mtx_21_in,
	input wire [3:0] 				mtx_22_in,
	input wire [3:0] 				mtx_23_in,
	input wire [3:0] 				mtx_30_in,
	input wire [3:0] 				mtx_31_in,
	input wire [3:0] 				mtx_32_in,
	input wire [3:0] 				mtx_33_in,
	output wire [`WORD_BITS-1:0]  D0_out,
	output wire [`WORD_BITS-1:0]  D1_out,
	output wire [`WORD_BITS-1:0]  D2_out,
	output wire [`WORD_BITS-1:0]  D3_out
  );

	wire [7:0] D0_gf_tmp00_wr, D0_gf_tmp01_wr, D0_gf_tmp02_wr, D0_gf_tmp03_wr;
	wire [7:0] D0_gf_tmp10_wr, D0_gf_tmp11_wr, D0_gf_tmp12_wr, D0_gf_tmp13_wr;
	wire [7:0] D0_gf_tmp20_wr, D0_gf_tmp21_wr, D0_gf_tmp22_wr, D0_gf_tmp23_wr;
	wire [7:0] D0_gf_tmp30_wr, D0_gf_tmp31_wr, D0_gf_tmp32_wr, D0_gf_tmp33_wr;
	
	wire [7:0] D1_gf_tmp00_wr, D1_gf_tmp01_wr, D1_gf_tmp02_wr, D1_gf_tmp03_wr;
	wire [7:0] D1_gf_tmp10_wr, D1_gf_tmp11_wr, D1_gf_tmp12_wr, D1_gf_tmp13_wr;
	wire [7:0] D1_gf_tmp20_wr, D1_gf_tmp21_wr, D1_gf_tmp22_wr, D1_gf_tmp23_wr;
	wire [7:0] D1_gf_tmp30_wr, D1_gf_tmp31_wr, D1_gf_tmp32_wr, D1_gf_tmp33_wr;
	
	wire [7:0] D2_gf_tmp00_wr, D2_gf_tmp01_wr, D2_gf_tmp02_wr, D2_gf_tmp03_wr;
	wire [7:0] D2_gf_tmp10_wr, D2_gf_tmp11_wr, D2_gf_tmp12_wr, D2_gf_tmp13_wr;
	wire [7:0] D2_gf_tmp20_wr, D2_gf_tmp21_wr, D2_gf_tmp22_wr, D2_gf_tmp23_wr;
	wire [7:0] D2_gf_tmp30_wr, D2_gf_tmp31_wr, D2_gf_tmp32_wr, D2_gf_tmp33_wr;
	
	wire [7:0] D3_gf_tmp00_wr, D3_gf_tmp01_wr, D3_gf_tmp02_wr, D3_gf_tmp03_wr;
	wire [7:0] D3_gf_tmp10_wr, D3_gf_tmp11_wr, D3_gf_tmp12_wr, D3_gf_tmp13_wr;
	wire [7:0] D3_gf_tmp20_wr, D3_gf_tmp21_wr, D3_gf_tmp22_wr, D3_gf_tmp23_wr;
	wire [7:0] D3_gf_tmp30_wr, D3_gf_tmp31_wr, D3_gf_tmp32_wr, D3_gf_tmp33_wr;
	
	///*** D0_out ***///
	GF_MUL d0_gf00 (.a(S0_in[31:24]),.b(mtx_00_in), .p(D0_gf_tmp00_wr));
	GF_MUL d0_gf01 (.a(S0_in[23:16]),.b(mtx_01_in), .p(D0_gf_tmp01_wr));
	GF_MUL d0_gf02 (.a(S0_in[15:8]) ,.b(mtx_02_in), .p(D0_gf_tmp02_wr));
	GF_MUL d0_gf03 (.a(S0_in[7:0])  ,.b(mtx_03_in), .p(D0_gf_tmp03_wr));
	
	GF_MUL d0_gf10 (.a(S0_in[31:24]),.b(mtx_10_in), .p(D0_gf_tmp10_wr));
	GF_MUL d0_gf11 (.a(S0_in[23:16]),.b(mtx_11_in), .p(D0_gf_tmp11_wr));
	GF_MUL d0_gf12 (.a(S0_in[15:8]) ,.b(mtx_12_in), .p(D0_gf_tmp12_wr));
	GF_MUL d0_gf13 (.a(S0_in[7:0])  ,.b(mtx_13_in), .p(D0_gf_tmp13_wr));
	
	GF_MUL d0_gf20 (.a(S0_in[31:24]),.b(mtx_20_in), .p(D0_gf_tmp20_wr));
	GF_MUL d0_gf21 (.a(S0_in[23:16]),.b(mtx_21_in), .p(D0_gf_tmp21_wr));
	GF_MUL d0_gf22 (.a(S0_in[15:8]) ,.b(mtx_22_in), .p(D0_gf_tmp22_wr));
	GF_MUL d0_gf23 (.a(S0_in[7:0])  ,.b(mtx_23_in), .p(D0_gf_tmp23_wr));
	
	GF_MUL d0_gf30 (.a(S0_in[31:24]),.b(mtx_30_in), .p(D0_gf_tmp30_wr));
	GF_MUL d0_gf31 (.a(S0_in[23:16]),.b(mtx_31_in), .p(D0_gf_tmp31_wr));
	GF_MUL d0_gf32 (.a(S0_in[15:8]) ,.b(mtx_32_in), .p(D0_gf_tmp32_wr));
	GF_MUL d0_gf33 (.a(S0_in[7:0])  ,.b(mtx_33_in), .p(D0_gf_tmp33_wr));  
	
	assign D0_out[31:24] = D0_gf_tmp00_wr ^ D0_gf_tmp01_wr ^ D0_gf_tmp02_wr ^ D0_gf_tmp03_wr;
	assign D0_out[23:16] = D0_gf_tmp10_wr ^ D0_gf_tmp11_wr ^ D0_gf_tmp12_wr ^ D0_gf_tmp13_wr;
	assign D0_out[15:8]  = D0_gf_tmp20_wr ^ D0_gf_tmp21_wr ^ D0_gf_tmp22_wr ^ D0_gf_tmp23_wr;
	assign D0_out[7:0]   = D0_gf_tmp30_wr ^ D0_gf_tmp31_wr ^ D0_gf_tmp32_wr ^ D0_gf_tmp33_wr;
	
	///*** D1_out ***///
	GF_MUL d1_gf00 (.a(S1_in[31:24]),.b(mtx_00_in), .p(D1_gf_tmp00_wr));
	GF_MUL d1_gf01 (.a(S1_in[23:16]),.b(mtx_01_in), .p(D1_gf_tmp01_wr));
	GF_MUL d1_gf02 (.a(S1_in[15:8]) ,.b(mtx_02_in), .p(D1_gf_tmp02_wr));
	GF_MUL d1_gf03 (.a(S1_in[7:0])  ,.b(mtx_03_in), .p(D1_gf_tmp03_wr));
														
	GF_MUL d1_gf10 (.a(S1_in[31:24]),.b(mtx_10_in), .p(D1_gf_tmp10_wr));
	GF_MUL d1_gf11 (.a(S1_in[23:16]),.b(mtx_11_in), .p(D1_gf_tmp11_wr));
	GF_MUL d1_gf12 (.a(S1_in[15:8]) ,.b(mtx_12_in), .p(D1_gf_tmp12_wr));
	GF_MUL d1_gf13 (.a(S1_in[7:0])  ,.b(mtx_13_in), .p(D1_gf_tmp13_wr));
														
	GF_MUL d1_gf20 (.a(S1_in[31:24]),.b(mtx_20_in), .p(D1_gf_tmp20_wr));
	GF_MUL d1_gf21 (.a(S1_in[23:16]),.b(mtx_21_in), .p(D1_gf_tmp21_wr));
	GF_MUL d1_gf22 (.a(S1_in[15:8]) ,.b(mtx_22_in), .p(D1_gf_tmp22_wr));
	GF_MUL d1_gf23 (.a(S1_in[7:0])  ,.b(mtx_23_in), .p(D1_gf_tmp23_wr));
														
	GF_MUL d1_gf30 (.a(S1_in[31:24]),.b(mtx_30_in), .p(D1_gf_tmp30_wr));
	GF_MUL d1_gf31 (.a(S1_in[23:16]),.b(mtx_31_in), .p(D1_gf_tmp31_wr));
	GF_MUL d1_gf32 (.a(S1_in[15:8]) ,.b(mtx_32_in), .p(D1_gf_tmp32_wr));
	GF_MUL d1_gf33 (.a(S1_in[7:0])  ,.b(mtx_33_in), .p(D1_gf_tmp33_wr));  
	
	assign D1_out[31:24] = D1_gf_tmp00_wr ^ D1_gf_tmp01_wr ^ D1_gf_tmp02_wr ^ D1_gf_tmp03_wr;
	assign D1_out[23:16] = D1_gf_tmp10_wr ^ D1_gf_tmp11_wr ^ D1_gf_tmp12_wr ^ D1_gf_tmp13_wr;
	assign D1_out[15:8]  = D1_gf_tmp20_wr ^ D1_gf_tmp21_wr ^ D1_gf_tmp22_wr ^ D1_gf_tmp23_wr;
	assign D1_out[7:0]   = D1_gf_tmp30_wr ^ D1_gf_tmp31_wr ^ D1_gf_tmp32_wr ^ D1_gf_tmp33_wr;  
	
	///*** D2_out ***///
	GF_MUL d2_gf00 (.a(S2_in[31:24]),.b(mtx_00_in), .p(D2_gf_tmp00_wr));
	GF_MUL d2_gf01 (.a(S2_in[23:16]),.b(mtx_01_in), .p(D2_gf_tmp01_wr));
	GF_MUL d2_gf02 (.a(S2_in[15:8]) ,.b(mtx_02_in), .p(D2_gf_tmp02_wr));
	GF_MUL d2_gf03 (.a(S2_in[7:0])  ,.b(mtx_03_in), .p(D2_gf_tmp03_wr));
														
	GF_MUL d2_gf10 (.a(S2_in[31:24]),.b(mtx_10_in), .p(D2_gf_tmp10_wr));
	GF_MUL d2_gf11 (.a(S2_in[23:16]),.b(mtx_11_in), .p(D2_gf_tmp11_wr));
	GF_MUL d2_gf12 (.a(S2_in[15:8]) ,.b(mtx_12_in), .p(D2_gf_tmp12_wr));
	GF_MUL d2_gf13 (.a(S2_in[7:0])  ,.b(mtx_13_in), .p(D2_gf_tmp13_wr));
														
	GF_MUL d2_gf20 (.a(S2_in[31:24]),.b(mtx_20_in), .p(D2_gf_tmp20_wr));
	GF_MUL d2_gf21 (.a(S2_in[23:16]),.b(mtx_21_in), .p(D2_gf_tmp21_wr));
	GF_MUL d2_gf22 (.a(S2_in[15:8]) ,.b(mtx_22_in), .p(D2_gf_tmp22_wr));
	GF_MUL d2_gf23 (.a(S2_in[7:0])  ,.b(mtx_23_in), .p(D2_gf_tmp23_wr));
														
	GF_MUL d2_gf30 (.a(S2_in[31:24]),.b(mtx_30_in), .p(D2_gf_tmp30_wr));
	GF_MUL d2_gf31 (.a(S2_in[23:16]),.b(mtx_31_in), .p(D2_gf_tmp31_wr));
	GF_MUL d2_gf32 (.a(S2_in[15:8]) ,.b(mtx_32_in), .p(D2_gf_tmp32_wr));
	GF_MUL d2_gf33 (.a(S2_in[7:0])  ,.b(mtx_33_in), .p(D2_gf_tmp33_wr));  
	
	assign D2_out[31:24] = D2_gf_tmp00_wr ^ D2_gf_tmp01_wr ^ D2_gf_tmp02_wr ^ D2_gf_tmp03_wr;
	assign D2_out[23:16] = D2_gf_tmp10_wr ^ D2_gf_tmp11_wr ^ D2_gf_tmp12_wr ^ D2_gf_tmp13_wr;
	assign D2_out[15:8]  = D2_gf_tmp20_wr ^ D2_gf_tmp21_wr ^ D2_gf_tmp22_wr ^ D2_gf_tmp23_wr;
	assign D2_out[7:0]   = D2_gf_tmp30_wr ^ D2_gf_tmp31_wr ^ D2_gf_tmp32_wr ^ D2_gf_tmp33_wr;  
	
	///*** D3_out ***///
	GF_MUL d3_gf00 (.a(S3_in[31:24]),.b(mtx_00_in), .p(D3_gf_tmp00_wr));
	GF_MUL d3_gf01 (.a(S3_in[23:16]),.b(mtx_01_in), .p(D3_gf_tmp01_wr));
	GF_MUL d3_gf02 (.a(S3_in[15:8]) ,.b(mtx_02_in), .p(D3_gf_tmp02_wr));
	GF_MUL d3_gf03 (.a(S3_in[7:0])  ,.b(mtx_03_in), .p(D3_gf_tmp03_wr));
														
	GF_MUL d3_gf10 (.a(S3_in[31:24]),.b(mtx_10_in), .p(D3_gf_tmp10_wr));
	GF_MUL d3_gf11 (.a(S3_in[23:16]),.b(mtx_11_in), .p(D3_gf_tmp11_wr));
	GF_MUL d3_gf12 (.a(S3_in[15:8]) ,.b(mtx_12_in), .p(D3_gf_tmp12_wr));
	GF_MUL d3_gf13 (.a(S3_in[7:0])  ,.b(mtx_13_in), .p(D3_gf_tmp13_wr));
														
	GF_MUL d3_gf20 (.a(S3_in[31:24]),.b(mtx_20_in), .p(D3_gf_tmp20_wr));
	GF_MUL d3_gf21 (.a(S3_in[23:16]),.b(mtx_21_in), .p(D3_gf_tmp21_wr));
	GF_MUL d3_gf22 (.a(S3_in[15:8]) ,.b(mtx_22_in), .p(D3_gf_tmp22_wr));
	GF_MUL d3_gf23 (.a(S3_in[7:0])  ,.b(mtx_23_in), .p(D3_gf_tmp23_wr));
														
	GF_MUL d3_gf30 (.a(S3_in[31:24]),.b(mtx_30_in), .p(D3_gf_tmp30_wr));
	GF_MUL d3_gf31 (.a(S3_in[23:16]),.b(mtx_31_in), .p(D3_gf_tmp31_wr));
	GF_MUL d3_gf32 (.a(S3_in[15:8]) ,.b(mtx_32_in), .p(D3_gf_tmp32_wr));
	GF_MUL d3_gf33 (.a(S3_in[7:0])  ,.b(mtx_33_in), .p(D3_gf_tmp33_wr));  
	
	assign D3_out[31:24] = D3_gf_tmp00_wr ^ D3_gf_tmp01_wr ^ D3_gf_tmp02_wr ^ D3_gf_tmp03_wr;
	assign D3_out[23:16] = D3_gf_tmp10_wr ^ D3_gf_tmp11_wr ^ D3_gf_tmp12_wr ^ D3_gf_tmp13_wr;
	assign D3_out[15:8]  = D3_gf_tmp20_wr ^ D3_gf_tmp21_wr ^ D3_gf_tmp22_wr ^ D3_gf_tmp23_wr;
	assign D3_out[7:0]   = D3_gf_tmp30_wr ^ D3_gf_tmp31_wr ^ D3_gf_tmp32_wr ^ D3_gf_tmp33_wr;  
  
endmodule

/*module GF_MUL
( 
	input wire [7:0] a,
	input wire [3:0] b,
	output reg [7:0] p
 );
  
	always @(a,b) begin
    case(b)
		4'b0000: p = 8'h00;
		4'b0001: p = a;
		4'b0010: p = (a << 1) ^ ((a & 8'h80) ? 8'h1B : 8'h00);
		4'b0011: p = ((a << 1) ^ ((a & 8'h80) ? 8'h1B : 8'h00)) ^ a;
		4'b0100: p = (a << 2) ^ ((a & 8'h80) ? (8'h1B << 1) : 8'h00);
		4'b0101: p = ((a << 2) ^ ((a & 8'h80) ? (8'h1B << 1) : 8'h00)) ^ a;
		4'b0110: p = ((a << 2) ^ ((a & 8'h80) ? (8'h1B << 1) : 8'h00)) ^ (a << 1);
		4'b0111: p = ((a << 2) ^ ((a & 8'h80) ? (8'h1B << 1) : 8'h00)) ^ (a << 1) ^ a;
		4'b1000: p = (a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00);
		4'b1001: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ a;
		4'b1010: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 1);
		4'b1011: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 1) ^ a;
		4'b1100: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 2);
		4'b1101: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 2) ^ a;
		4'b1110: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 2) ^ (a << 1);
		4'b1111: p = ((a << 3) ^ ((a & 8'h80) ? (8'h1B << 2) : 8'h00)) ^ (a << 2) ^ (a << 1) ^ a;
		default: p = 8'h00;
	endcase
	end
endmodule*/

module GF_MUL
( 
    input wire [7:0] a,
    input wire [3:0] b,
    output reg [7:0] p
);

    reg [7:0] temp_a;

    always @(a, b) begin
        temp_a = a;
        p = 0;

        // Check each bit of b and perform the operation
        if (b[0]) p = p ^ temp_a;
        temp_a = (temp_a << 1) ^ ((temp_a & 8'h80) ? 8'h1B : 8'h00);

        if (b[1]) p = p ^ temp_a;
        temp_a = (temp_a << 1) ^ ((temp_a & 8'h80) ? 8'h1B : 8'h00);

        if (b[2]) p = p ^ temp_a;
        temp_a = (temp_a << 1) ^ ((temp_a & 8'h80) ? 8'h1B : 8'h00);

        if (b[3]) p = p ^ temp_a;
        // Note: The following line is not necessary as it's the last iteration
        // temp_a = (temp_a << 1) ^ ((temp_a & 8'h80) ? 8'h1B : 8'h00);
    end

endmodule

module SBOX_SM4(
    input  wire [7:0] x_in,
    output reg  [7:0] y_out
);

    always @(x_in) begin
        case (x_in)
            // Row 0
            8'h00 : y_out = 8'hD6;   8'h01 : y_out = 8'h90;   8'h02 : y_out = 8'hE9;   8'h03 : y_out = 8'hFE;
            8'h04 : y_out = 8'hCC;   8'h05 : y_out = 8'hE1;   8'h06 : y_out = 8'h3D;   8'h07 : y_out = 8'hB7;
            8'h08 : y_out = 8'h16;   8'h09 : y_out = 8'hB6;   8'h0A : y_out = 8'h14;   8'h0B : y_out = 8'hC2;
            8'h0C : y_out = 8'h28;   8'h0D : y_out = 8'hFB;   8'h0E : y_out = 8'h2C;   8'h0F : y_out = 8'h05;

            // Row 1
            8'h10 : y_out = 8'h2B;   8'h11 : y_out = 8'h67;   8'h12 : y_out = 8'h9A;   8'h13 : y_out = 8'h76;
            8'h14 : y_out = 8'h2A;   8'h15 : y_out = 8'hBE;   8'h16 : y_out = 8'h04;   8'h17 : y_out = 8'hC3;
            8'h18 : y_out = 8'hAA;   8'h19 : y_out = 8'h44;   8'h1A : y_out = 8'h13;   8'h1B : y_out = 8'h26;
            8'h1C : y_out = 8'h49;   8'h1D : y_out = 8'h86;   8'h1E : y_out = 8'h06;   8'h1F : y_out = 8'h99;

            // Row 2
            8'h20 : y_out = 8'h9C;   8'h21 : y_out = 8'h42;   8'h22 : y_out = 8'h50;   8'h23 : y_out = 8'hF4;
            8'h24 : y_out = 8'h91;   8'h25 : y_out = 8'hEF;   8'h26 : y_out = 8'h98;   8'h27 : y_out = 8'h7A;
            8'h28 : y_out = 8'h33;   8'h29 : y_out = 8'h54;   8'h2A : y_out = 8'h0B;   8'h2B : y_out = 8'h43;
            8'h2C : y_out = 8'hED;   8'h2D : y_out = 8'hCF;   8'h2E : y_out = 8'hAC;   8'h2F : y_out = 8'h62;

            // Row 3
            8'h30 : y_out = 8'hE4;   8'h31 : y_out = 8'hB3;   8'h32 : y_out = 8'h1C;   8'h33 : y_out = 8'hA9;
            8'h34 : y_out = 8'hC9;   8'h35 : y_out = 8'h08;   8'h36 : y_out = 8'hE8;   8'h37 : y_out = 8'h95;
            8'h38 : y_out = 8'h80;   8'h39 : y_out = 8'hDF;   8'h3A : y_out = 8'h94;   8'h3B : y_out = 8'hFA;
            8'h3C : y_out = 8'h75;   8'h3D : y_out = 8'h8F;   8'h3E : y_out = 8'h3F;   8'h3F : y_out = 8'hA6;

            // Row 4
            8'h40 : y_out = 8'h47;   8'h41 : y_out = 8'h07;   8'h42 : y_out = 8'hA7;   8'h43 : y_out = 8'hFC;
            8'h44 : y_out = 8'hF3;   8'h45 : y_out = 8'h73;   8'h46 : y_out = 8'h17;   8'h47 : y_out = 8'hBA;
            8'h48 : y_out = 8'h83;   8'h49 : y_out = 8'h59;   8'h4A : y_out = 8'h3C;   8'h4B : y_out = 8'h19;
            8'h4C : y_out = 8'hE6;   8'h4D : y_out = 8'h85;   8'h4E : y_out = 8'h4F;   8'h4F : y_out = 8'hA8;

            // Row 5
            8'h50 : y_out = 8'h68;   8'h51 : y_out = 8'h6B;   8'h52 : y_out = 8'h81;   8'h53 : y_out = 8'hB2;
            8'h54 : y_out = 8'h71;   8'h55 : y_out = 8'h64;   8'h56 : y_out = 8'hDA;   8'h57 : y_out = 8'h8B;
            8'h58 : y_out = 8'hF8;   8'h59 : y_out = 8'hEB;   8'h5A : y_out = 8'h0F;   8'h5B : y_out = 8'h4B;
            8'h5C : y_out = 8'h70;   8'h5D : y_out = 8'h56;   8'h5E : y_out = 8'h9D;   8'h5F : y_out = 8'h35;

            // Row 6
            8'h60 : y_out = 8'h1E;   8'h61 : y_out = 8'h24;   8'h62 : y_out = 8'h0E;   8'h63 : y_out = 8'h5E;
            8'h64 : y_out = 8'h63;   8'h65 : y_out = 8'h58;   8'h66 : y_out = 8'hD1;   8'h67 : y_out = 8'hA2;
            8'h68 : y_out = 8'h25;   8'h69 : y_out = 8'h22;   8'h6A : y_out = 8'h7C;   8'h6B : y_out = 8'h3B;
            8'h6C : y_out = 8'h01;   8'h6D : y_out = 8'h21;   8'h6E : y_out = 8'h78;   8'h6F : y_out = 8'h87;

            // Row 7
            8'h70 : y_out = 8'hD4;   8'h71 : y_out = 8'h00;   8'h72 : y_out = 8'h46;   8'h73 : y_out = 8'h57;
            8'h74 : y_out = 8'h9F;   8'h75 : y_out = 8'hD3;   8'h76 : y_out = 8'h27;   8'h77 : y_out = 8'h52;
            8'h78 : y_out = 8'h4C;   8'h79 : y_out = 8'h36;   8'h7A : y_out = 8'h02;   8'h7B : y_out = 8'hE7;
            8'h7C : y_out = 8'hA0;   8'h7D : y_out = 8'hC4;   8'h7E : y_out = 8'hC8;   8'h7F : y_out = 8'h9E;
			
			// Row 8
            8'h80 : y_out = 8'hEA;   8'h81 : y_out = 8'hBF;   8'h82 : y_out = 8'h8A;   8'h83 : y_out = 8'hD2;
            8'h84 : y_out = 8'h40;   8'h85 : y_out = 8'hC7;   8'h86 : y_out = 8'h38;   8'h87 : y_out = 8'hB5;
            8'h88 : y_out = 8'hA3;   8'h89 : y_out = 8'hF7;   8'h8A : y_out = 8'hF2;   8'h8B : y_out = 8'hCE;
            8'h8C : y_out = 8'hF9;   8'h8D : y_out = 8'h61;   8'h8E : y_out = 8'h15;   8'h8F : y_out = 8'hA1;

            // Row 9
            8'h90 : y_out = 8'hE0;   8'h91 : y_out = 8'hAE;   8'h92 : y_out = 8'h5D;   8'h93 : y_out = 8'hA4;
            8'h94 : y_out = 8'h9B;   8'h95 : y_out = 8'h34;   8'h96 : y_out = 8'h1A;   8'h97 : y_out = 8'h55;
            8'h98 : y_out = 8'hAD;   8'h99 : y_out = 8'h93;   8'h9A : y_out = 8'h32;   8'h9B : y_out = 8'h30;
            8'h9C : y_out = 8'hF5;   8'h9D : y_out = 8'h8C;   8'h9E : y_out = 8'hB1;   8'h9F : y_out = 8'hE3;

            // Row 10
            8'hA0 : y_out = 8'h1D;   8'hA1 : y_out = 8'hF6;   8'hA2 : y_out = 8'hE2;   8'hA3 : y_out = 8'h2E;
            8'hA4 : y_out = 8'h82;   8'hA5 : y_out = 8'h66;   8'hA6 : y_out = 8'hCA;   8'hA7 : y_out = 8'h60;
            8'hA8 : y_out = 8'hC0;   8'hA9 : y_out = 8'h29;   8'hAA : y_out = 8'h23;   8'hAB : y_out = 8'hAB;
            8'hAC : y_out = 8'h0D;   8'hAD : y_out = 8'h53;   8'hAE : y_out = 8'h4E;   8'hAF : y_out = 8'h6F;

            // Row 11
            8'hB0 : y_out = 8'hD5;   8'hB1 : y_out = 8'hDB;   8'hB2 : y_out = 8'h37;   8'hB3 : y_out = 8'h45;
            8'hB4 : y_out = 8'hDE;   8'hB5 : y_out = 8'hFD;   8'hB6 : y_out = 8'h8E;   8'hB7 : y_out = 8'h2F;
            8'hB8 : y_out = 8'h03;   8'hB9 : y_out = 8'hFF;   8'hBA : y_out = 8'h6A;   8'hBB : y_out = 8'h72;
            8'hBC : y_out = 8'h6D;   8'hBD : y_out = 8'h6C;   8'hBE : y_out = 8'h5B;   8'hBF : y_out = 8'h51;

            // Row 12
            8'hC0 : y_out = 8'h8D;   8'hC1 : y_out = 8'h1B;   8'hC2 : y_out = 8'hAF;   8'hC3 : y_out = 8'h92;
            8'hC4 : y_out = 8'hBB;   8'hC5 : y_out = 8'hDD;   8'hC6 : y_out = 8'hBC;   8'hC7 : y_out = 8'h7F;
            8'hC8 : y_out = 8'h11;   8'hC9 : y_out = 8'hD9;   8'hCA : y_out = 8'h5C;   8'hCB : y_out = 8'h41;
            8'hCC : y_out = 8'h1F;   8'hCD : y_out = 8'h10;   8'hCE : y_out = 8'h5A;   8'hCF : y_out = 8'hD8;

            // Row 13
            8'hD0 : y_out = 8'h0A;   8'hD1 : y_out = 8'hC1;   8'hD2 : y_out = 8'h31;   8'hD3 : y_out = 8'h88;
            8'hD4 : y_out = 8'hA5;   8'hD5 : y_out = 8'hCD;   8'hD6 : y_out = 8'h7B;   8'hD7 : y_out = 8'hBD;
            8'hD8 : y_out = 8'h2D;   8'hD9 : y_out = 8'h74;   8'hDA : y_out = 8'hD0;   8'hDB : y_out = 8'h12;
            8'hDC : y_out = 8'hB8;   8'hDD : y_out = 8'hE5;   8'hDE : y_out = 8'hB4;   8'hDF : y_out = 8'hB0;

            // Row 14
            8'hE0 : y_out = 8'h89;   8'hE1 : y_out = 8'h69;   8'hE2 : y_out = 8'h97;   8'hE3 : y_out = 8'h4A;
            8'hE4 : y_out = 8'h0C;   8'hE5 : y_out = 8'h96;   8'hE6 : y_out = 8'h77;   8'hE7 : y_out = 8'h7E;
            8'hE8 : y_out = 8'h65;   8'hE9 : y_out = 8'hB9;   8'hEA : y_out = 8'hF1;   8'hEB : y_out = 8'h09;
            8'hEC : y_out = 8'hC5;   8'hED : y_out = 8'h6E;   8'hEE : y_out = 8'hC6;   8'hEF : y_out = 8'h84;

            // Row 15
            8'hF0 : y_out = 8'h18;   8'hF1 : y_out = 8'hF0;   8'hF2 : y_out = 8'h7D;   8'hF3 : y_out = 8'hEC;
            8'hF4 : y_out = 8'h3A;   8'hF5 : y_out = 8'hDC;   8'hF6 : y_out = 8'h4D;   8'hF7 : y_out = 8'h20;
            8'hF8 : y_out = 8'h79;   8'hF9 : y_out = 8'hEE;   8'hFA : y_out = 8'h5F;   8'hFB : y_out = 8'h3E;
            8'hFC : y_out = 8'hD7;   8'hFD : y_out = 8'hCB;   8'hFE : y_out = 8'h39;   8'hFF : y_out = 8'h48;
            default : y_out = 8'h00; // Default case
        endcase
    end

endmodule

module subcell(
    input wire [7:0] in,
    output wire [7:0] out
    ); 
    wire[7:0] tmp;
    assign tmp = {in[7], in[6], in[5], in[4] ^ ~(in[7]|in[6]), in[3], in[2], in[1], in[0]^~(in[3]|in[2])};
    assign out = {tmp[2], tmp[1], tmp[7], tmp[6], tmp[4], tmp[0], tmp[3], tmp[5]};
endmodule

module subcell_last(
    input wire [7:0] in,
    output wire [7:0] out
    ); 
    wire[7:0] tmp;
    assign out = {in[7], in[6], in[5], in[4] ^ ~(in[7]|in[6]), in[3], in[1], in[2], in[0]^~(in[3]|in[2])};
   
endmodule