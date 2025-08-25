/*
 *-----------------------------------------------------------------------------
 * Title         : LSU
 * Project       : U2CP
 *-----------------------------------------------------------------------------
 * File          : LSU.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.02.21
 *-----------------------------------------------------------------------------
 * Last modified : 2023.02.21
 * Copyright (c) 2023 by NAIST This model is the confidential and
 * proprietary property of NAIST and the possession or use of this
 * file requires a written license from NAIST.
 *-----------------------------------------------------------------------------
 * Modification history :
 * 2023.02.21 : created
 *-----------------------------------------------------------------------------
 */
 
`timescale 1ns/1ns
`include "common.vh"

module LSU
(
  input  wire                                 CLK,
  input  wire                                 RST,
  
  //-----------------------------------------------------//
  //          			Input Signals                    // 
  //-----------------------------------------------------//
  input  wire 					              	En_in,
  input  wire 					              	Finish_in,
  input  wire [`LSU_CFG_BITS-1:0]             	CFG_in,
  ///*** Local Data Memory ***///
  //-MSB-//
  input  wire [`LDM_ADDR_BITS-1:0]            	LDM_MSB_addra_in,
  input  wire [`PE_AXI_DWIDTH_BITS-1:0]       	LDM_MSB_dina_in,
  input  wire 					              	LDM_MSB_ena_in,
  input  wire 					              	LDM_MSB_wea_in,
  output  wire [`PE_AXI_DWIDTH_BITS-1:0]      	LDM_MSB_douta_out,
  //-LSB-//
  input  wire [`LDM_ADDR_BITS-1:0]            	LDM_LSB_addra_in,
  input  wire [`PE_AXI_DWIDTH_BITS-1:0]       	LDM_LSB_dina_in,
  input  wire 					              	LDM_LSB_ena_in,
  input  wire 					              	LDM_LSB_wea_in,
  output  wire [`PE_AXI_DWIDTH_BITS-1:0]      	LDM_LSB_douta_out,
  ///*** LSU Input ***///	
  input  wire [`DWORD_BITS-1:0]              	S48_0_in,
  input  wire [`DWORD_BITS-1:0]              	S48_1_in,
  input  wire [`DWORD_BITS-1:0]              	S48_2_in,
  input  wire [`DWORD_BITS-1:0]              	S48_3_in,
  
  //-----------------------------------------------------//
  //          			Output Signals                   // 
  //-----------------------------------------------------//  
  
  ///*** LSU Output ***///
  output wire  [`DWORD_BITS-1:0]             	D48_0_out,
  output wire  [`DWORD_BITS-1:0]          	  	D48_1_out,
  output wire  [`DWORD_BITS-1:0]          	  	D48_2_out,
  output wire  [`DWORD_BITS-1:0]          	  	D48_3_out
  
);
 
 /// Port B register
 reg  						          			LDM_enb_rg;
 reg  						          			LDM_web_rg;
 reg  [`LDM_ADDR_BITS-1:0]            			LDM_addrb_rg;
 reg  [`AAG_BITS-1:0]            	  			LDW_addrb_offset_rg;
 reg  [`AAG_BITS-1:0]            	  			STW_addrb_offset_rg;
 wire [`PE_AXI_DWIDTH_BITS*2-1:0]       		LDM_doutb_wr;
 
 reg  [`DWORD_BITS-1:0]				  			D48_0_rg;
 reg  [`DWORD_BITS-1:0]				  			D48_1_rg;
 reg  [`DWORD_BITS-1:0]				  			D48_2_rg;
 reg  [`DWORD_BITS-1:0]				  			D48_3_rg;
 
 // CFGiguration signals
 wire [`LDM_ADDR_BITS-1:0]            			addrb_base_wr;
 wire [1:0]		  					  			OP_LSU_wr;
 reg [1:0]		  					  			OP_LSU_rg;
 reg 			  					  			En_rg;
 
assign OP_LSU_wr = CFG_in[`LSU_CFG_BITS-1:`LDM_ADDR_BITS];
assign addrb_base_wr = CFG_in[`LDM_ADDR_BITS-1:0];

`ifdef ZYNQ_BRAM
 LDM_BRAM LDM0 (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_ena_in), // port A read enable
  .wea(LDM_wea_in), // port A write enable
  .addra(LDM_addra_in), // port A address
  .dina(LDM_dina_in[`PE_AXI_DWIDTH_BITS-1:32]), // port A data
  .douta(LDM_douta_out[`PE_AXI_DWIDTH_BITS-1:32]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_0_in), // port A data
  .doutb(LDM_doutb_wr[`PE_AXI_DWIDTH_BITS-1:32]) // port A data output
  );
  
LDM_BRAM LDM1 (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_ena_in), // port A read enable
  .wea(LDM_wea_in), // port A write enable
  .addra(LDM_addra_in), // port A address
  .dina(LDM_dina_in[31:0]), // port A data
  .douta(LDM_douta_out[31:0]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_1_in), // port A data
  .doutb(LDM_doutb_wr[31:0]) // port A data output
  );
  
 `endif
 
`ifdef REG_BRAM
/// BRAM 32-bit x 1024

Dual_Port_LDM #
(.DWIDTH(`WORD_BITS), .AWIDTH(`LDM_ADDR_BITS))
 LDM0_MSB (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_MSB_ena_in), // port A read enable
  .wea(LDM_MSB_wea_in), // port A write enable
  .addra(LDM_MSB_addra_in), // port A address
  .dina(LDM_MSB_dina_in[`PE_AXI_DWIDTH_BITS-1:32]), // port A data
  .douta(LDM_MSB_douta_out[`PE_AXI_DWIDTH_BITS-1:32]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_0_in[63:32]), // port A data
  .doutb(LDM_doutb_wr[`PE_AXI_DWIDTH_BITS*2-1:`PE_AXI_DWIDTH_BITS*2-32]) // port A data output
  );

Dual_Port_LDM #
(.DWIDTH(`WORD_BITS), .AWIDTH(`LDM_ADDR_BITS))
 LDM0_LSB (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_LSB_ena_in), // port A read enable
  .wea(LDM_LSB_wea_in), // port A write enable
  .addra(LDM_LSB_addra_in), // port A address
  .dina(LDM_LSB_dina_in[`PE_AXI_DWIDTH_BITS-1:32]), // port A data
  .douta(LDM_LSB_douta_out[`PE_AXI_DWIDTH_BITS-1:32]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_0_in[31:0]), // port A data
  .doutb(LDM_doutb_wr[`PE_AXI_DWIDTH_BITS*2-32-1:`PE_AXI_DWIDTH_BITS*2-64]) // port A data output
  );
  
Dual_Port_LDM #
(.DWIDTH(`WORD_BITS), .AWIDTH(`LDM_ADDR_BITS))
 LDM1_MSB (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_MSB_ena_in), // port A read enable
  .wea(LDM_MSB_wea_in), // port A write enable
  .addra(LDM_MSB_addra_in), // port A address
  .dina(LDM_MSB_dina_in[31:0]), // port A data
  .douta(LDM_MSB_douta_out[31:0]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_1_in[63:32]), // port A data
  .doutb(LDM_doutb_wr[`PE_AXI_DWIDTH_BITS-1:32]) // port A data output
  );

Dual_Port_LDM #
(.DWIDTH(`WORD_BITS), .AWIDTH(`LDM_ADDR_BITS))
 LDM1_LSB (
  .clka(CLK), // clock
  ///*** Port A***///
  .ena(LDM_LSB_ena_in), // port A read enable
  .wea(LDM_LSB_wea_in), // port A write enable
  .addra(LDM_LSB_addra_in), // port A address
  .dina(LDM_LSB_dina_in[31:0]), // port A data
  .douta(LDM_LSB_douta_out[31:0]), // port A data output
  
  .clkb(CLK), // clock
  ///*** Port B***///
  .enb(LDM_enb_rg), // port A read enable
  .web(LDM_web_rg), // port A write enable
  .addrb(LDM_addrb_rg), // port A address
  .dinb(S48_1_in[31:0]), // port A data
  .doutb(LDM_doutb_wr[31:0]) // port A data output
  );

`endif
 
always @(posedge CLK or negedge RST) begin
    if (~RST) begin
	  LDW_addrb_offset_rg	<= `AAG_BITS'h0;
	  STW_addrb_offset_rg	<= `AAG_BITS'h0;	
	  OP_LSU_rg				<= 2'b0;
	  En_rg					<= 1'b0;
    end
    else begin
		OP_LSU_rg <= OP_LSU_wr;
		En_rg		<= En_in;
	  if(En_in) begin
		case (OP_LSU_wr)
		 `LSU_LDW: begin
			LDW_addrb_offset_rg	<= LDW_addrb_offset_rg + 1;
		 end
		 `LSU_STW: begin
			STW_addrb_offset_rg	<= STW_addrb_offset_rg + 1;
		 end
		default: begin
			LDW_addrb_offset_rg	<= `AAG_BITS'h0;
			STW_addrb_offset_rg	<= `AAG_BITS'h0;
        end
		endcase   
      end 
	else begin
		LDW_addrb_offset_rg	<= `AAG_BITS'h0;
		STW_addrb_offset_rg	<= `AAG_BITS'h0;
	end
    end
  end

	always @(*) begin
		if(En_in) begin
			case (OP_LSU_wr)
			`LSU_LDW: begin
				LDM_enb_rg  	= 1'b1;
				LDM_web_rg  	= 1'b0;
				LDM_addrb_rg    = addrb_base_wr + LDW_addrb_offset_rg; 
			end
			`LSU_STW: begin
				LDM_enb_rg  	= 1'b1;
				LDM_web_rg  	= 1'b1;
				LDM_addrb_rg  	= addrb_base_wr + STW_addrb_offset_rg; 
			end
			default: begin
				LDM_enb_rg  	= 1'b0;
				LDM_web_rg  	= 1'b0;
			end
			endcase   
		end   
		else begin 
			LDM_enb_rg  	= 1'b0;
			LDM_web_rg  	= 1'b0;
			LDM_addrb_rg  	= `LDM_ADDR_BITS'h0;  
		end
	end  
  
	assign D48_0_out  = (En_rg&(OP_LSU_rg == `LSU_LDW)) ? LDM_doutb_wr[`PE_AXI_DWIDTH_BITS*2-1:64] : D48_0_rg;
	assign D48_1_out  = (En_rg&(OP_LSU_rg == `LSU_LDW)) ? LDM_doutb_wr[63:0] : D48_1_rg;

  always @(posedge CLK or negedge RST) begin
    if (~RST) begin
      D48_0_rg <= `DWORD_BITS'h0;
	  D48_1_rg <= `DWORD_BITS'h0;
      D48_2_rg <= `DWORD_BITS'h0;
	  D48_3_rg <= `DWORD_BITS'h0;
    end
    else begin
		if(Finish_in) begin
			D48_0_rg <= `DWORD_BITS'h0;
			D48_1_rg <= `DWORD_BITS'h0;
			D48_2_rg <= `DWORD_BITS'h0;
			D48_3_rg <= `DWORD_BITS'h0;
		end
		else if(En_in) begin
			D48_0_rg <= S48_0_in;
			D48_1_rg <= S48_1_in;
			D48_2_rg <= S48_2_in; 
			D48_3_rg <= S48_3_in; 
		end
		else begin
			D48_0_rg <= `DWORD_BITS'h0;
			D48_1_rg <= `DWORD_BITS'h0;
			D48_2_rg <= `DWORD_BITS'h0;
			D48_3_rg <= `DWORD_BITS'h0;
	  end
    end
  end
  
  assign D48_2_out  = D48_2_rg;
  assign D48_3_out  = D48_3_rg;
  
endmodule

