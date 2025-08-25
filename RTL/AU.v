/*
 *-----------------------------------------------------------------------------
 * Title         : AU
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : AU.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2024.01.15
 *-----------------------------------------------------------------------------
 * Last modified : 2024.01.15
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.03.02 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"
module AU
(
	input  wire                                 CLK,
	input  wire                                 RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					            Mode_in,
	input  wire [`EXE1_CFG_BITS-1:0]            CFG_in,
	input  wire [`DWORD_BITS-1:0]              	S0_in,
	input  wire [`DWORD_BITS-1:0]              	S1_in,
	input  wire [`DWORD_BITS-1:0]              	S2_in,
	input  wire [`DWORD_BITS-1:0]              	S3_in,
	
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------// 
	output wire  [`DWORD_BITS-1:0]           	D0_out,
	output wire  [`DWORD_BITS-1:0]           	D1_out,
	output wire  [`DWORD_BITS-1:0]           	D2_out,
	output wire  [`DWORD_BITS-1:0]           	D3_out
);
  
	wire [`DWORD_BITS-1:0]              		D0_64_wr;
	wire [`WORD_BITS-1:0]             	 		D0_MSB_wr;
	wire [`WORD_BITS-1:0]             	 		D0_LSB_wr;
	
	assign D0_64_wr	= 	(CFG_in == `EXE1_ADD2) ? S0_in + S1_in:
						(CFG_in == `EXE1_ADD3) ? S0_in + S1_in + S2_in:
						(CFG_in == `EXE1_SUB2) ? S0_in - S1_in: S0_in;
					
	assign D0_MSB_wr = 	(CFG_in == `EXE1_ADD2) ? S0_in[`DWORD_BITS-1:`WORD_BITS] + S1_in[`DWORD_BITS-1:`WORD_BITS]:
						(CFG_in == `EXE1_ADD3) ? S0_in[`DWORD_BITS-1:`WORD_BITS] + S1_in[`DWORD_BITS-1:`WORD_BITS] + S2_in[`DWORD_BITS-1:`WORD_BITS]:
						(CFG_in == `EXE1_SUB2) ? S0_in[`DWORD_BITS-1:`WORD_BITS] - S1_in[`DWORD_BITS-1:`WORD_BITS]: S0_in[`DWORD_BITS-1:`WORD_BITS];					

	assign D0_LSB_wr = 	(CFG_in == `EXE1_ADD2) ? S0_in[`WORD_BITS-1:0] + S1_in[`WORD_BITS-1:0]:
						(CFG_in == `EXE1_ADD3) ? S0_in[`WORD_BITS-1:0] + S1_in[`WORD_BITS-1:0] + S2_in[`WORD_BITS-1:0]:
						(CFG_in == `EXE1_SUB2) ? S0_in[`WORD_BITS-1:0] - S1_in[`WORD_BITS-1:0]: S0_in[`WORD_BITS-1:0];	
					
	assign D0_out = 	(Mode_in == `MODE32) ? {D0_MSB_wr,D0_LSB_wr}:D0_64_wr;
					
	assign D1_out = S1_in;
	assign D2_out = S2_in;
	assign D3_out = S3_in;
 
endmodule