/*
 *-----------------------------------------------------------------------------
 * Title         : ALU
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : ALU.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.02.20
 *-----------------------------------------------------------------------------
 * Last modified : 2023.12.08
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.02.20 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"

module ALU
(
	input  wire                                 CLK,
	input  wire                                 RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					            En_in,
	input  wire 					            Mode_in,
	input  wire 					            Finish_in,
	input  wire [`ALU_CFG_BITS-1:0]             CFG_in,
	input  wire [`CTX_IM_BITS-1:0]             	IM_in,
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

	///*** EXE1 and EXE2 signals ***///
	wire [`DWORD_BITS-1:0]      				D0_AU_wr, D1_AU_wr, D2_AU_wr, D3_AU_wr;
	wire [`DWORD_BITS-1:0]      				D0_LSRU_wr, D1_LSRU_wr, D2_LSRU_wr, D3_LSRU_wr;
	wire [`DWORD_BITS-1:0]      				S1_AU_wr;
	
	///*** EXE CUSTOM signals ***///
	
	wire [`DWORD_BITS-1:0]      				D0_EXE_CT_wr, D1_EXE_CT_wr, D2_EXE_CT_wr, D3_EXE_CT_wr;
	
	///*** CFGiguration of EXE1, EXE2, and EXE_CUSTOM signals ***///
	
	wire [`EXE1_CFG_BITS-1:0] 					CFG_AU_wr;
	wire [`EXE2_CFG_BITS-1:0] 					CFG_LSRU_wr;
	wire [`EXE_CT_CFG_BITS-1:0] 				CFG_EXE_CT_wr;
	reg  [`EXE_CT_CFG_BITS-1:0] 				CFG_EXE_CT_rg;
	
	///*** CFGiguration Decoder ***///
	
	assign CFG_AU_wr    	= CFG_in[`ALU_CFG_BITS-1:`ALU_CFG_BITS -`EXE1_CFG_BITS];
	assign CFG_LSRU_wr    	= CFG_in[`ALU_CFG_BITS-`EXE1_CFG_BITS-1:`ALU_CFG_BITS -`EXE1_CFG_BITS -`EXE2_CFG_BITS];
	assign CFG_EXE_CT_wr  	= CFG_in[`ALU_CFG_BITS -`EXE1_CFG_BITS -`EXE2_CFG_BITS-1:`ALU_CFG_BITS -`EXE1_CFG_BITS -`EXE2_CFG_BITS - `EXE_CT_CFG_BITS];

	assign S1_AU_wr = (IM_in == 0) ? S1_in: IM_in;

	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			CFG_EXE_CT_rg <= `EXE_CT_CFG_BITS'h0;
		end
		else begin
			CFG_EXE_CT_rg <= CFG_EXE_CT_wr;
		end
	end
  
	AU au
	(
		.CLK(CLK),
		.RST(RST),
		.Mode_in(Mode_in),
		.CFG_in(CFG_AU_wr),
		.S0_in(S0_in),
		.S1_in(S1_AU_wr),
		.S2_in(S2_in),
		.S3_in(S3_in),
		.D0_out(D0_AU_wr),
		.D1_out(D1_AU_wr),
		.D2_out(D2_AU_wr),
		.D3_out(D3_AU_wr)
	);


	LSRU lsru
	(
		.CLK(CLK),
		.RST(RST),
		.Mode_in(Mode_in),
		.En_in(En_in),
		.Finish_in(Finish_in),
		.CFG_in(CFG_LSRU_wr),
		.S0_in(D0_AU_wr),
		.S1_in(D1_AU_wr),
		.S2_in(D2_AU_wr),
		.S3_in(D3_AU_wr),
		.D0_out(D0_LSRU_wr),
		.D1_out(D1_LSRU_wr),
		.D2_out(D2_LSRU_wr),
		.D3_out(D3_LSRU_wr)
	);

	EXE_CUSTOM exe_custom
	(
		.CLK(CLK),
		.RST(RST),
		.Mode_in(Mode_in),
		.En_in(En_in),
		.Finish_in(Finish_in),
		.CFG_in(CFG_EXE_CT_wr),
		.S0_in(S0_in),
		.S1_in(S1_in),
		.S2_in(S2_in),
		.S3_in(S3_in),
		.D0_out(D0_EXE_CT_wr),
		.D1_out(D1_EXE_CT_wr),
		.D2_out(D2_EXE_CT_wr),
		.D3_out(D3_EXE_CT_wr)
	);

	assign D0_out = (CFG_EXE_CT_rg == 0) ? D0_LSRU_wr: D0_EXE_CT_wr;
	assign D1_out = (CFG_EXE_CT_rg == 0) ? D1_LSRU_wr: D1_EXE_CT_wr;
	assign D2_out = (CFG_EXE_CT_rg == 0) ? D2_LSRU_wr: D2_EXE_CT_wr;
	assign D3_out = (CFG_EXE_CT_rg == 0) ? D3_LSRU_wr: D3_EXE_CT_wr;
 
endmodule

