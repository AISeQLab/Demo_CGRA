/*
 *-----------------------------------------------------------------------------
 * Title         : Processing Element
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : PE.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.03.03
 *-----------------------------------------------------------------------------
 * Last modified : 2023.12.08
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.03.03 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"

module PE
#(
	parameter                                   				UNIT_NO = 0,
	parameter									  				ROW_NO = 0,
	parameter									  				LR_NO = 0  //In mode 32, LR_NO is NOT used. In mode 64, LR_NO = 0 if write first two left PEs and LR_NO = 1 if write first two right PEs
)
(
	input  wire                                 				CLK,
	input  wire                                 				RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					              				start_in,
	input  wire 					              				Mode_in,
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
	
	input  wire 					              				CTX_incr_in,
					
	///*** Local Data Memory ***///		
	
	input  wire [`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:0]     	LDM_addra_in,
	input  wire 					              				LDM_ena_in,
	input  wire 					              				LDM_wea_in,
	//-MSB-//
	input  wire [`PE_AXI_DWIDTH_BITS-1:0]          				LDM_MSB_dina_in,
	output  wire [`PE_AXI_DWIDTH_BITS-1:0]         				LDM_MSB_douta_out,
	//-LSB-//
	input  wire [`PE_AXI_DWIDTH_BITS-1:0]          				LDM_LSB_dina_in,
	output  wire [`PE_AXI_DWIDTH_BITS-1:0]         				LDM_LSB_douta_out,	
	///*** ALU Input ***///				
	input  wire [`DWORD_BITS-1:0]                				S48_0_in,
	input  wire [`DWORD_BITS-1:0]                				S48_1_in,
	input  wire [`DWORD_BITS-1:0]                				S48_2_in,
	input  wire [`DWORD_BITS-1:0]                				S48_3_in,
	///*** Buffer 16to1 Input ***///				
	input  wire [`DWORD_BITS-1:0]                				B16_0_in,
	input  wire [`DWORD_BITS-1:0]                				B16_1_in,
	input  wire [`DWORD_BITS-1:0]                				B16_2_in,
	input  wire [`DWORD_BITS-1:0]                				B16_3_in,
	///*** Buffer 8to1 Input ***///				
	input  wire [`DWORD_BITS-1:0]                				B8_0_in,
	input  wire [`DWORD_BITS-1:0]                				B8_1_in,
	input  wire [`DWORD_BITS-1:0]                				B8_2_in,
	input  wire [`DWORD_BITS-1:0]                				B8_3_in,
	
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------//  
	
	///*** ALU Output ***///
	output wire  [`DWORD_BITS-1:0]           	  				D48_0_out,
	output wire  [`DWORD_BITS-1:0]           	  				D48_1_out,
	output wire  [`DWORD_BITS-1:0]           	  				D48_2_out,
	output wire  [`DWORD_BITS-1:0]           	  				D48_3_out,
	///*** Buffer 16to1 Output ***///
	output reg  [`DWORD_BITS-1:0]           	  				B16_0_out,
	output reg  [`DWORD_BITS-1:0]           	  				B16_1_out,
	output reg  [`DWORD_BITS-1:0]           	  				B16_2_out,
	output reg  [`DWORD_BITS-1:0]           	  				B16_3_out,
	///*** Buffer 8to1 Output ***///					        
	output reg  [`DWORD_BITS-1:0]           	  				B8_0_out,
	output reg  [`DWORD_BITS-1:0]           	  				B8_1_out,
	output reg  [`DWORD_BITS-1:0]           	  				B8_2_out,
	output reg  [`DWORD_BITS-1:0]           	  				B8_3_out
  
);
 
	/// Output ALU wire
	wire  [`DWORD_BITS-1:0]           	  						ALU_D0_wr;
	wire  [`DWORD_BITS-1:0]           	  						ALU_D1_wr;
	wire  [`DWORD_BITS-1:0]           	  						ALU_D2_wr;
	wire  [`DWORD_BITS-1:0]           	  						ALU_D3_wr;
							
	/// Output LSU wire						
	wire  [`DWORD_BITS-1:0]           	  						LSU_D0_wr;
	wire  [`DWORD_BITS-1:0]           	  						LSU_D1_wr;
	wire  [`DWORD_BITS-1:0]           	  						LSU_D2_wr;
	wire  [`DWORD_BITS-1:0]           	  						LSU_D3_wr;
							
	/// Context memory						
	wire								  						CTX_PE_ena_wr;
	wire								  						CTX_IM_ena_wr;
							
	reg  						          						CTX_enb_rg;
	reg  						          						CTX_web_rg;
	reg  [`CTX_PE_ADDR_BITS-1:0]          						CTX_addrb_rg;
	wire [`CTX_PE_BITS-1:0]          	  						CTX_PE_doutb_wr;
	wire [`CTX_IM_BITS-1:0]          	  						CTX_IM_doutb_wr;
							
	reg  [`CTX_PE_ADDR_BITS-1:0]	      						CTX_maxaddra_rg;
							
	wire [`LSU_CFG_BITS-1:0]              						LSU_CFG_wr;
	wire 						          						LSU_En_wr;
	wire [`ALU_CFG_BITS-1:0]              						ALU_CFG_wr;
	wire 						          						ALU_En_wr;
	reg 						          						LSU_En_rg;
	reg 						          						LSU_ALU_En_rg;
							
	/// LDM memory						
							
	wire 					              						LDM_MSB_ena_wr, LDM_LSB_ena_wr;
							
	/// FSM						
							
	reg  						          						finish_lg1_rg;
	reg  						          						finish_lg2_rg;
	reg  						          						finish_lg3_rg;
	reg  						          						finish_lg4_rg;
	reg  						          						finish_lg5_rg;
	reg  						          						finish_lg6_rg;
	reg  						          						finish_lg7_rg;
	reg  						          						finish_lg8_rg;
	reg  						          						finish_lg9_rg;
	
	localparam  IDLE = 0;
	localparam  EXEC = 1; 
	
	reg  [0:0]           	  			  STATE_rg;

	assign  CTX_PE_ena_wr = (CTX_PE_addra_in[`PE_NUM_BITS +`CTX_PE_ADDR_BITS-1:`CTX_PE_ADDR_BITS] == UNIT_NO) ? CTX_PE_ena_in: 1'b0;
	assign  CTX_IM_ena_wr = (CTX_IM_addra_in[`PE_NUM_BITS +`CTX_IM_ADDR_BITS-1:`CTX_IM_ADDR_BITS] == UNIT_NO) ? CTX_IM_ena_in: 1'b0;
	
	assign  LDM_MSB_ena_wr = ((Mode_in == `MODE32)&&(LDM_addra_in[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS+`LR_BITS] == ROW_NO)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == 0))     ? LDM_ena_in: 
							 ((Mode_in == `MODE64)&&(LDM_addra_in[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS+`LR_BITS] == ROW_NO)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == LR_NO)) ? LDM_ena_in:1'b0;
	assign  LDM_LSB_ena_wr = ((Mode_in == `MODE32)&&(LDM_addra_in[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS+`LR_BITS] == ROW_NO)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == 1))     ? LDM_ena_in: 
							 ((Mode_in == `MODE64)&&(LDM_addra_in[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS+`LR_BITS] == ROW_NO)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == LR_NO)) ? LDM_ena_in: 1'b0;
	
	`ifdef REG_BRAM
	CTX_RAM # (.DWIDTH(`CTX_PE_BITS), .AWIDTH(`CTX_PE_ADDR_BITS))
	ctx_pe_ram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_PE_ena_wr), // port A read enable
		.wea(CTX_PE_wea_in), // port A write enable
		.addra(CTX_PE_addra_in[`CTX_PE_ADDR_BITS-1:0]), // port A address
		.dina(CTX_PE_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_PE_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_PE_doutb_wr) // port A data output 
	);
	
	CTX_RAM # (.DWIDTH(`CTX_IM_BITS), .AWIDTH(`CTX_IM_ADDR_BITS))
	ctx_im_ram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_IM_ena_wr), // port A read enable
		.wea(CTX_IM_wea_in), // port A write enable
		.addra(CTX_IM_addra_in[`CTX_IM_ADDR_BITS-1:0]), // port A address
		.dina(CTX_IM_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_IM_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_IM_doutb_wr) // port A data output 
	);
	`endif

	`ifdef ZYNQ_BRAM
	CTX_PE_BRAM ctx_pe_bram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_PE_ena_wr), // port A read enable
		.wea(CTX_PE_wea_in), // port A write enable
		.addra(CTX_PE_addra_in[`CTX_PE_ADDR_BITS-1:0]), // port A address
		.dina(CTX_PE_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_PE_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_PE_doutb_wr) // port A data output
	);
	
	CTX_IM_BRAM ctx_im_bram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_IM_ena_wr), // port A read enable
		.wea(CTX_IM_wea_in), // port A write enable
		.addra(CTX_IM_addra_in[`CTX_IM_ADDR_BITS-1:0]), // port A address
		.dina(CTX_IM_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_IM_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_IM_doutb_wr) // port A data output
	);
	`endif
 
	assign ALU_En_wr  = ~CTX_PE_doutb_wr[`CTX_PE_BITS-1:`CTX_PE_BITS-1]&LSU_ALU_En_rg;
	
	assign ALU_CFG_wr = (ALU_En_wr)? CTX_PE_doutb_wr[`CTX_PE_BITS-2:0]: 31'h0;
	
	assign LSU_En_wr  = CTX_PE_doutb_wr[`CTX_PE_BITS-1:`CTX_PE_BITS-1]&LSU_ALU_En_rg;
	assign LSU_CFG_wr = (LSU_En_wr)? CTX_PE_doutb_wr[`CTX_PE_BITS-2:`CTX_PE_BITS -`LDM_ADDR_BITS-2]:11'd0;
	
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			STATE_rg  		<= IDLE;
		end
		else begin
			if((STATE_rg == IDLE)& start_in) begin
				STATE_rg		<= EXEC;
			end
			else if((STATE_rg == EXEC)& finish_lg7_rg) begin
				STATE_rg		<= IDLE;
			end
		end
	end
 
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			CTX_addrb_rg  	<= `CTX_PE_ADDR_BITS'h0;
			LSU_ALU_En_rg	<= 1'b0;
			LSU_En_rg		<= 1'b0;
			finish_lg1_rg	<= 1'b0;	
			finish_lg2_rg   <= 1'b0;
			finish_lg3_rg   <= 1'b0;
			finish_lg4_rg   <= 1'b0;  
			finish_lg5_rg   <= 1'b0;
			finish_lg6_rg   <= 1'b0;
			finish_lg7_rg   <= 1'b0;		
			finish_lg8_rg   <= 1'b0;
			finish_lg9_rg   <= 1'b0;
		end
		else begin
			finish_lg2_rg   <= finish_lg1_rg;
			finish_lg3_rg   <= finish_lg2_rg;
			finish_lg4_rg   <= finish_lg3_rg; 
			finish_lg5_rg   <= finish_lg4_rg;
			finish_lg6_rg   <= finish_lg5_rg;
			finish_lg7_rg   <= finish_lg6_rg;		
			finish_lg8_rg   <= finish_lg7_rg;
			finish_lg9_rg   <= finish_lg8_rg;
			LSU_En_rg		<= LSU_En_wr;
			case (STATE_rg)
			IDLE: begin
				CTX_addrb_rg  	<= `CTX_PE_ADDR_BITS'h0;
				LSU_ALU_En_rg	<= 1'b0;
				finish_lg1_rg	<= 1'b0;
			end
			EXEC: begin
				CTX_addrb_rg  	<= CTX_addrb_rg + CTX_incr_in; 			
				LSU_ALU_En_rg	<= 1'b1;
				if(CTX_addrb_rg == CTX_maxaddra_rg) begin
					finish_lg1_rg	<= 1'b1;
				end
			end
			default: begin
				CTX_addrb_rg  	<= `CTX_PE_ADDR_BITS'h0;
				LSU_ALU_En_rg	<= 1'b0;
				finish_lg1_rg	<= 1'b0;
			end
			endcase           
		end
	end
 
	always @(*) begin
		case (STATE_rg)
		IDLE: begin
			CTX_enb_rg  	= 1'b0;
			CTX_web_rg  	= 1'b0;
		end
		EXEC: begin
			CTX_enb_rg  	= 1'b1;
			CTX_web_rg  	= 1'b0;
		end
		default: begin
			CTX_enb_rg  	= 1'b0;
			CTX_web_rg  	= 1'b0;
		end
		endcase           
	end
	
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			CTX_maxaddra_rg	<= `CTX_PE_ADDR_BITS'h0;
		end
		else begin
			if(CTX_PE_ena_wr & CTX_PE_wea_in) begin
				CTX_maxaddra_rg	<= CTX_PE_addra_in[`CTX_PE_ADDR_BITS-1:0];
			end        
		end
	end
  
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			B16_0_out <= `DWORD_BITS'h0;
			B16_1_out <= `DWORD_BITS'h0;
			B16_2_out <= `DWORD_BITS'h0;
			B16_3_out <= `WORD_BITS'h0;
			
			B8_0_out  <= `DWORD_BITS'h0;
			B8_1_out  <= `DWORD_BITS'h0;
			B8_2_out  <= `DWORD_BITS'h0;
			B8_3_out  <= `DWORD_BITS'h0;
		end
		else begin 
			if(finish_lg9_rg) begin
				B16_0_out <= `DWORD_BITS'h0;
				B16_1_out <= `DWORD_BITS'h0;
				B16_2_out <= `DWORD_BITS'h0;
				B16_3_out <= `DWORD_BITS'h0;
				
				B8_0_out  <= `DWORD_BITS'h0;
				B8_1_out  <= `DWORD_BITS'h0;
				B8_2_out  <= `DWORD_BITS'h0;
				B8_3_out  <= `DWORD_BITS'h0;
			end
			else begin
				B16_0_out <= B16_0_in;
				B16_1_out <= B16_1_in;
				B16_2_out <= B16_2_in;
				B16_3_out <= B16_3_in;
				
				B8_0_out  <= B8_0_in;
				B8_1_out  <= B8_1_in;
				B8_2_out  <= B8_2_in;
				B8_3_out  <= B8_3_in;
			end
		end
	end
 
	LSU lsu(
		.CLK(CLK),
		.RST(RST),
		.En_in(LSU_En_wr),
		.Finish_in(finish_lg9_rg),
		.CFG_in(LSU_CFG_wr),
		///*** Local Data Memory ***///
		//-MSB-//
		.LDM_MSB_addra_in(LDM_addra_in[`LDM_ADDR_BITS-1:0]),
		.LDM_MSB_dina_in(LDM_MSB_dina_in),
		.LDM_MSB_ena_in(LDM_MSB_ena_wr),
		.LDM_MSB_wea_in(LDM_wea_in),
		.LDM_MSB_douta_out(LDM_MSB_douta_out),
		//-LSB-//
		.LDM_LSB_addra_in(LDM_addra_in[`LDM_ADDR_BITS-1:0]),
		.LDM_LSB_dina_in(LDM_LSB_dina_in),
		.LDM_LSB_ena_in(LDM_LSB_ena_wr),
		.LDM_LSB_wea_in(LDM_wea_in),
		.LDM_LSB_douta_out(LDM_LSB_douta_out),
		///*** LSU Input ***///
		.S48_0_in(S48_0_in),
		.S48_1_in(S48_1_in),
		.S48_2_in(S48_2_in),
		.S48_3_in(S48_3_in),
		
		///*** LSU Output ***///
		.D48_0_out(LSU_D0_wr),
		.D48_1_out(LSU_D1_wr),
		.D48_2_out(LSU_D2_wr),
		.D48_3_out(LSU_D3_wr)
	);
 
	ALU alu(
		.CLK(CLK),
		.RST(RST),
		.Mode_in(Mode_in),
		.En_in(ALU_En_wr),
		.Finish_in(finish_lg9_rg),
		.CFG_in(ALU_CFG_wr),
		.IM_in(CTX_IM_doutb_wr),
		.S0_in(S48_0_in),
		.S1_in(S48_1_in),
		.S2_in(S48_2_in),
		.S3_in(S48_3_in),
		.D0_out(ALU_D0_wr),
		.D1_out(ALU_D1_wr),
		.D2_out(ALU_D2_wr),
		.D3_out(ALU_D3_wr)
	);

	assign D48_0_out = (LSU_En_rg)? LSU_D0_wr : ALU_D0_wr;
	assign D48_1_out = (LSU_En_rg)? LSU_D1_wr : ALU_D1_wr;
	assign D48_2_out = (LSU_En_rg)? LSU_D2_wr : ALU_D2_wr;
	assign D48_3_out = (LSU_En_rg)? LSU_D3_wr : ALU_D3_wr;
 
endmodule

