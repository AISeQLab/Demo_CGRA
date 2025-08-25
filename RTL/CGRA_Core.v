/*
 *-----------------------------------------------------------------------------
 * Title         : U2CP
 * Project       : U2CP
 *-----------------------------------------------------------------------------
 * File          : U2CP.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.03.07
 *-----------------------------------------------------------------------------
 * Last modified : 2023.03.07
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.03.07 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"

module CGRA_Core
(
	input	wire								  				CLK,
	input	wire								  				RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                    // 
	//-----------------------------------------------------//
	input  wire 					              				start_in,
	input  wire 					              				Mode_in,
	
	///*** Context RC Memory ***///
	input  wire [`PE_NUM_BITS+`CTX_RC_ADDR_BITS-1:0]   			CTX_RC_addra_in,
	input  wire [`CTX_RC_BITS-1:0]              				CTX_RC_dina_in,
	input  wire 					              				CTX_RC_ena_in,
	input  wire 					              				CTX_RC_wea_in,
	
	///*** Context PE Memory ***///
	input  wire [`PE_NUM_BITS+`CTX_PE_ADDR_BITS-1:0]          	CTX_PE_addra_in,
	input  wire [`CTX_PE_BITS-1:0]              				CTX_PE_dina_in,
	input  wire 					              				CTX_PE_ena_in,
	input  wire 					              				CTX_PE_wea_in,
	
	///*** Context IM Memory ***///
	input  wire [`PE_NUM_BITS+`CTX_IM_ADDR_BITS-1:0]          	CTX_IM_addra_in,
	input  wire [`CTX_IM_BITS-1:0]              				CTX_IM_dina_in,
	input  wire 					              				CTX_IM_ena_in,
	input  wire 					              				CTX_IM_wea_in,
	
	///*** Local Data Memory ***///				
	input  wire [`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:0]     	LDM_addra_in,
	input  wire [`AXI_DWIDTH_BITS-1:0]          				LDM_dina_in,
	input  wire 					              				LDM_ena_in,
	input  wire 					              				LDM_wea_in,
	
	//-----------------------------------------------------//
	//          			Output Signals                   // 
	//-----------------------------------------------------//  
	output  wire 						         				complete_out,
	output  wire [`AXI_DWIDTH_BITS-1:0]         				LDM_douta_out
 );

	//*** PEA Controller ***///
	wire								  		CTX_ena_wr;
	reg  [`CTX_RC_ADDR_BITS-1:0]	          	CTX_maxaddra_rg;
	
	reg [`CTX_RC_ADDR_BITS-1:0]   				LOOP_rg;
	
	reg [7:0]				              		start_rg;
	reg [3:0]				              		waiting_rg;
	reg [7:0]				              		CTX_incr_rg;
	wire 					              		start_wr;
	
	reg 					              		complete_rg;

	localparam  								IDLE 	= 0;
	localparam  								START 	= 1; 
	localparam  								EXEC 	= 2;
	
	reg  [1:0]           	  			  		STATE_rg;
	
	assign complete_out = complete_rg;
	assign start_wr		= (STATE_rg == START) ? 1'b1:1'b0;
	
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			STATE_rg  		<= IDLE;
		end
		else begin
			if((STATE_rg == IDLE)& start_in) begin
				STATE_rg		<= START;
			end
			else if(STATE_rg == START) begin
				STATE_rg		<= EXEC;
			end
			else if((STATE_rg == EXEC)& complete_out) begin
				STATE_rg		<= IDLE;
			end
		end
	end
	
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			start_rg 	<= 8'b0;
			waiting_rg	<= 4'b0;
			LOOP_rg <= `CTX_RC_ADDR_BITS'b0;
			complete_rg <= 1'b0;
		end
		else begin 
			start_rg[0] <= start_wr;
			start_rg[1] <= start_rg[0];
			start_rg[2] <= start_rg[1];
			start_rg[3] <= start_rg[2];
			start_rg[4] <= start_rg[3];
			start_rg[5] <= start_rg[4];
			start_rg[6] <= start_rg[5];
			start_rg[7] <= start_rg[6];		  
			if((LOOP_rg > CTX_maxaddra_rg)&&(waiting_rg == 4'd9)) begin
				LOOP_rg 	<= `CTX_RC_ADDR_BITS'b0;
				complete_rg <= 1'b1;
			end
			else begin
				LOOP_rg <= LOOP_rg + (start_rg[7]|CTX_incr_rg[7]);
				complete_rg <= complete_rg&~start_wr;
				if(LOOP_rg > CTX_maxaddra_rg) begin
					waiting_rg <= waiting_rg +1'b1;
				end
				else begin
					waiting_rg <= 4'b0;
				end
			end
		end
	end
	 
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
		  CTX_incr_rg 	<= 8'b0;
		end
		else begin 
			if(LOOP_rg == 0) begin
				CTX_incr_rg[0] <= start_rg[7];
				CTX_incr_rg[1] <= CTX_incr_rg[0];
				CTX_incr_rg[2] <= CTX_incr_rg[1];
				CTX_incr_rg[3] <= CTX_incr_rg[2];
				CTX_incr_rg[4] <= CTX_incr_rg[3];
				CTX_incr_rg[5] <= CTX_incr_rg[4];
				CTX_incr_rg[6] <= CTX_incr_rg[5];
				CTX_incr_rg[7] <= CTX_incr_rg[6];
			end
			else if(LOOP_rg == CTX_maxaddra_rg) begin
				CTX_incr_rg[0] <= 1'b0;
				CTX_incr_rg[1] <= CTX_incr_rg[0];
				CTX_incr_rg[2] <= CTX_incr_rg[1];
				CTX_incr_rg[3] <= CTX_incr_rg[2];
				CTX_incr_rg[4] <= CTX_incr_rg[3];
				CTX_incr_rg[5] <= CTX_incr_rg[4];
				CTX_incr_rg[6] <= CTX_incr_rg[5];
				CTX_incr_rg[7] <= CTX_incr_rg[6];
			end
			else if(LOOP_rg < CTX_maxaddra_rg) begin
				CTX_incr_rg[0] <= CTX_incr_rg[7];
				CTX_incr_rg[1] <= CTX_incr_rg[0];
				CTX_incr_rg[2] <= CTX_incr_rg[1];
				CTX_incr_rg[3] <= CTX_incr_rg[2];
				CTX_incr_rg[4] <= CTX_incr_rg[3];
				CTX_incr_rg[5] <= CTX_incr_rg[4];
				CTX_incr_rg[6] <= CTX_incr_rg[5];
				CTX_incr_rg[7] <= CTX_incr_rg[6];
			end
			else begin
				CTX_incr_rg <= 8'b0;
			end
		end
	end

	assign  CTX_ena_wr = CTX_RC_ena_in&CTX_RC_wea_in;


	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			CTX_maxaddra_rg	<= `CTX_RC_ADDR_BITS'h0;
		end
		else begin
			if(CTX_ena_wr) begin
				CTX_maxaddra_rg	<= CTX_RC_addra_in[`CTX_RC_ADDR_BITS-1:0];
			end        
		end
	end
	  
	PEA pea(
		.CLK(CLK),
		.RST(RST),
		.start_in(start_rg),
		.Mode_in(Mode_in),
		.CTX_RC_addra_in(CTX_RC_addra_in),
		.CTX_RC_dina_in(CTX_RC_dina_in),
		.CTX_RC_ena_in(CTX_RC_ena_in),
		.CTX_RC_wea_in(CTX_RC_wea_in),		
		.CTX_PE_addra_in(CTX_PE_addra_in),
		.CTX_PE_dina_in(CTX_PE_dina_in),
		.CTX_PE_ena_in(CTX_PE_ena_in),
		.CTX_PE_wea_in(CTX_PE_wea_in),	  
		.CTX_IM_addra_in(CTX_IM_addra_in),
		.CTX_IM_dina_in(CTX_IM_dina_in),
		.CTX_IM_ena_in(CTX_IM_ena_in),
		.CTX_IM_wea_in(CTX_IM_wea_in),	
		.CTX_incr_in(CTX_incr_rg),
		.LDM_addra_in(LDM_addra_in),
		.LDM_dina_in(LDM_dina_in),
		.LDM_ena_in(LDM_ena_in),
		.LDM_wea_in(LDM_wea_in),
		.LDM_douta_out(LDM_douta_out)
	);
 
endmodule

