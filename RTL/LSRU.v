/*
 *-----------------------------------------------------------------------------
 * Title         : LSRU
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : LSRU.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2024.01.14
 *-----------------------------------------------------------------------------
 * Last modified : 2024.01.14
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
module LSRU
(
	input  wire                                 CLK,
	input  wire                                 RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					            En_in,
	input  wire 					            Mode_in,
	input  wire 					            Finish_in,
	input  wire [`EXE2_CFG_BITS-1:0]            CFG_in,
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
  
	reg  [`DWORD_BITS-1:0]    					LU1_64_rg;
	reg  [`DWORD_BITS-1:0]    					LU2_64_rg;
	reg  [`DWORD_BITS-1:0]    					LU3_64_rg;
	reg  [`DWORD_BITS-1:0]    					SRU1_64_rg;
	reg  [`DWORD_BITS-1:0]    					SRU2_64_rg;
	
	reg  [`WORD_BITS-1:0]    					SRU1_MSB_rg;
	reg  [`WORD_BITS-1:0]    					SRU2_MSB_rg;

	reg  [`WORD_BITS-1:0]    					SRU1_LSB_rg;
	reg  [`WORD_BITS-1:0]    					SRU2_LSB_rg;

	wire [`DWORD_BITS-1:0]    					SRU1_wr;
	wire [`DWORD_BITS-1:0]    					SRU2_wr;
	
	wire [2:0]									OP2_LU1_wr;
	wire [2:0]									OP2_LU2_wr;
	wire [1:0]									OP2_LU3_wr;
						
	wire [1:0]									OP2_SRU1_wr;
	wire [1:0]									OP2_SRU2_wr;
						
	wire [4:0]									SRU1_32_IM_wr;
	wire [4:0]									SRU2_32_IM_wr;
					
	wire [5:0]									SRU1_64_IM_wr;
	wire [5:0]									SRU2_64_IM_wr;
	
	///*** Configuration Decoder ***///
  
	assign OP2_LU3_wr	= CFG_in[1:0];
	
	assign SRU2_32_IM_wr = CFG_in[6:2]; /// for 32-bit SRU
	assign SRU2_64_IM_wr = CFG_in[7:2]; /// for 64-bit SRU
	assign OP2_SRU2_wr 	= CFG_in[9:8];
	
	assign SRU1_32_IM_wr = CFG_in[14:10]; /// for 32-bit SRU
	assign SRU1_64_IM_wr = CFG_in[15:10]; /// for 64-bit SRU
	assign OP2_SRU1_wr 	= CFG_in[17:16];
	
	assign OP2_LU2_wr 	= CFG_in[20:18];
	assign OP2_LU1_wr 	= CFG_in[23:21];
  
	////////////////////////////////////
	///*** 32/64-bit Logic Unit 1 ***///
	////////////////////////////////////
  
	always @(*) begin
		case (OP2_LU1_wr)
			`EXE2_NOP: begin   ///*** No Operation ***///
				LU1_64_rg = S0_in; // Pass through S0_in
			end
			`EXE2_XOR: begin   ///*** A XOR B Operation ***///
				LU1_64_rg = S0_in ^ S2_in; // S0_in XOR S2_in
			end
			`EXE2_OR: begin   ///*** A OR B Operation ***///
				LU1_64_rg = S0_in | S2_in; // S0_in OR S2_in
			end
			`EXE2_AND: begin   ///*** A AND B Operation ***///
				LU1_64_rg = S0_in & S2_in; // S0_in AND S2_in
			end
			`EXE2_NOT: begin   ///*** NOT A Operation ***///
				LU1_64_rg = ~S0_in; // NOT S0_in
			end
			`EXE2_NOT_XOR: begin  ///*** NOT A XOR B Operation ***///
				LU1_64_rg = ~S0_in ^ S2_in; // NOT S0_in XOR S2_in
			end
			`EXE2_NOT_OR: begin   ///*** NOT A OR B Operation ***///
				LU1_64_rg = ~S0_in | S2_in; // NOT S0_in OR S2_in
			end
			`EXE2_NOT_AND: begin   ///*** NOT A AND B Operation ***///
				LU1_64_rg = ~S0_in & S2_in; // NOT S0_in AND S2_in
			end
			default: begin
				LU1_64_rg = S0_in; // Pass through S0_in
			end
		endcase
	end
  
	////////////////////////////////////
	///*** 32/64-bit Logic Unit 2 ***///
	////////////////////////////////////
  
	always @(*) begin
		case (OP2_LU2_wr)
			`EXE2_NOP: begin   ///*** No Operation ***///
				LU2_64_rg = S3_in; // Pass through S3_in
			end
			`EXE2_XOR: begin   ///*** A XOR B Operation ***///
				LU2_64_rg = S1_in ^ S3_in; // S1_in XOR S3_in
			end
			`EXE2_OR: begin   ///*** A OR B Operation ***///
				LU2_64_rg = S1_in | S3_in; // S1_in OR S3_in
			end
			`EXE2_AND: begin   ///*** A AND B Operation ***///
				LU2_64_rg = S1_in & S3_in; // S1_in AND S3_in
			end
			`EXE2_NOT: begin   ///*** NOT A Operation ***///
				LU2_64_rg = ~S1_in; // NOT S1_in
			end
			`EXE2_NOT_XOR: begin  ///*** NOT A XOR B Operation ***///
				LU2_64_rg = ~S1_in ^ S3_in; // NOT S1_in XOR S3_in
			end
			`EXE2_NOT_OR: begin   ///*** NOT A OR B Operation ***///
				LU2_64_rg = ~S1_in | S3_in; // NOT S1_in OR S3_in
			end
			`EXE2_NOT_AND: begin   ///*** NOT A AND B Operation ***///
				LU2_64_rg = ~S1_in & S3_in; // NOT S1_in AND S3_in
			end
			default: begin
				LU2_64_rg = S3_in; // Pass through S3_in
			end
		endcase
	end
 
	////////////////////////////////////////
	///*** 64-bit Shift Rorate Unit 1 ***///
	////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU1_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU1_64_rg = LU1_64_rg << SRU1_64_IM_wr; // Shift Left LU2_64_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU1_64_rg = LU1_64_rg >> SRU1_64_IM_wr; // Shift Right LU2_64_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU1_64_rg =  (LU1_64_rg << SRU1_64_IM_wr) | (LU1_64_rg >> (64-SRU1_64_IM_wr)); // Rotate Left LU2_64_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU1_64_rg =  (LU1_64_rg >> SRU1_64_IM_wr) | (LU1_64_rg << (64-SRU1_64_IM_wr)); // Rotate Left LU2_64_rg
			end
			default: begin
				SRU1_64_rg = LU1_64_rg << SRU1_64_IM_wr; // Shift Left LU2_64_rg
			end
		endcase
	end
	
	////////////////////////////////////////////
	///*** 32-bit MSB Shift Rorate Unit 1 ***///
	////////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU1_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU1_MSB_rg = LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU1_32_IM_wr; // Shift Left LU1_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU1_MSB_rg = LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] >> SRU1_32_IM_wr; // Shift Right LU1_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU1_MSB_rg =  (LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU1_32_IM_wr) | (LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] >> (32-SRU1_32_IM_wr)); // Rotate Left LU1_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU1_MSB_rg =  (LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] >> SRU1_32_IM_wr) | (LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] << (32-SRU1_32_IM_wr)); // Rotate Left LU1_rg
			end
			default: begin
				SRU1_MSB_rg = LU1_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU1_32_IM_wr; // Shift Left LU1_rg
			end
		endcase
	end
 
 	////////////////////////////////////////////
	///*** 32-bit LSB Shift Rorate Unit 1 ***///
	////////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU1_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU1_LSB_rg = LU1_64_rg[`WORD_BITS-1:0] << SRU1_32_IM_wr; // Shift Left LU1_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU1_LSB_rg = LU1_64_rg[`WORD_BITS-1:0] >> SRU1_32_IM_wr; // Shift Right LU1_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU1_LSB_rg =  (LU1_64_rg[`WORD_BITS-1:0] << SRU1_32_IM_wr) | (LU1_64_rg[`WORD_BITS-1:0] >> (32-SRU1_32_IM_wr)); // Rotate Left LU1_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU1_LSB_rg =  (LU1_64_rg[`WORD_BITS-1:0] >> SRU1_32_IM_wr) | (LU1_64_rg[`WORD_BITS-1:0] << (32-SRU1_32_IM_wr)); // Rotate Left LU1_rg
			end
			default: begin
				SRU1_LSB_rg = LU1_64_rg[`WORD_BITS-1:0] << SRU1_32_IM_wr; // Shift Left LU1_rg
			end
		endcase
	end
	
	////////////////////////////////////////////
	///*** 64-bit MSB Shift Rorate Unit 2 ***///
	////////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU2_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU2_64_rg = LU2_64_rg << SRU2_64_IM_wr; // Shift Left LU2_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU2_64_rg = LU2_64_rg >> SRU2_64_IM_wr; // Shift Right LU2_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU2_64_rg =  (LU2_64_rg << SRU2_64_IM_wr) | (LU2_64_rg >> (64-SRU2_64_IM_wr)); // Rotate Left LU2_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU2_64_rg =  (LU2_64_rg >> SRU2_64_IM_wr) | (LU2_64_rg << (64-SRU2_64_IM_wr)); // Rotate Left LU2_rg
			end
			default: begin
				SRU2_64_rg = LU2_64_rg << SRU2_64_IM_wr; // Shift Left LU2_rg
			end
		endcase
	end
  

	////////////////////////////////////////////
	///*** 32-bit MSB Shift Rorate Unit 2 ***///
	////////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU2_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU2_MSB_rg = LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU2_32_IM_wr; // Shift Left LU1_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU2_MSB_rg = LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] >> SRU2_32_IM_wr; // Shift Right LU1_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU2_MSB_rg =  (LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU2_32_IM_wr) | (LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] >> (32-SRU2_32_IM_wr)); // Rotate Left LU1_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU2_MSB_rg =  (LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] >> SRU2_32_IM_wr) | (LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] << (32-SRU2_32_IM_wr)); // Rotate Left LU1_rg
			end
			default: begin
				SRU2_MSB_rg = LU2_64_rg[`DWORD_BITS-1:`WORD_BITS] << SRU2_32_IM_wr; // Shift Left LU1_rg
			end
		endcase
	end

 	////////////////////////////////////////////
	///*** 32-bit LSB Shift Rorate Unit 2 ***///
	////////////////////////////////////////////
  
	always @(*) begin
		case (OP2_SRU2_wr)
			`EXE2_SHL: begin   ///*** Shift Left /NO Operation ***///
				SRU2_LSB_rg = LU2_64_rg[`WORD_BITS-1:0] << SRU2_32_IM_wr; // Shift Left LU1_rg
			end
			`EXE2_SHR: begin   ///*** Shift Right Operation ***///
				SRU2_LSB_rg = LU2_64_rg[`WORD_BITS-1:0] >> SRU2_32_IM_wr; // Shift Right LU1_rg
			end
			`EXE2_ROL: begin   ///*** Rotate Left Operation ***///
				SRU2_LSB_rg =  (LU2_64_rg[`WORD_BITS-1:0] << SRU2_32_IM_wr) | (LU2_64_rg[`WORD_BITS-1:0] >> (32-SRU2_32_IM_wr)); // Rotate Left LU1_rg
			end
			`EXE2_ROR: begin   ///*** Rotate Right Operation ***///
				SRU2_LSB_rg =  (LU2_64_rg[`WORD_BITS-1:0] >> SRU2_32_IM_wr) | (LU2_64_rg[`WORD_BITS-1:0] << (32-SRU2_32_IM_wr)); // Rotate Left LU1_rg
			end
			default: begin
				SRU2_LSB_rg = LU2_64_rg[`WORD_BITS-1:0] << SRU2_32_IM_wr; // Shift Left LU1_rg
			end
		endcase
	end
	
	
	//////////////////////////
	///*** Logic Unit 3 ***///
	//////////////////////////
  
	assign SRU1_wr = (Mode_in == `MODE32) ? {SRU1_MSB_rg,SRU1_LSB_rg} : SRU1_64_rg;
	assign SRU2_wr = (Mode_in == `MODE32) ? {SRU2_MSB_rg,SRU2_LSB_rg} : SRU2_64_rg;
	
	always @(*) begin
		case (OP2_LU3_wr)
			`sLU_NOP: begin   ///*** No Operation ***///
				LU3_64_rg = SRU2_wr; // Pass through LU2_rg
			end
			`sLU_XOR: begin   ///*** A XOR B Operation ***///
				LU3_64_rg = SRU1_wr ^ SRU2_wr; // LU1_rg XOR LU2_rg
			end
			`sLU_OR: begin   ///*** A OR B Operation ***///
				LU3_64_rg = SRU1_wr | SRU2_wr; // LU1_rg OR LU2_rg
			end
			`sLU_AND: begin   ///*** A AND B Operation ***///
				LU3_64_rg = SRU1_wr & SRU2_wr; // LU1_rg AND LU2_rg
			end
			default: begin
				LU3_64_rg = SRU2_wr; // Pass through LU2_rg
			end
		endcase
	end 
  
  
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
				D0_out <= S2_in; 
				D1_out <= LU3_64_rg; 
				D2_out <= SRU1_wr; 
				D3_out <= S1_in; 
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

