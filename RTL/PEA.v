/*
 *-----------------------------------------------------------------------------
 * Title         : PEA
 * Project       : U2CP
 *-----------------------------------------------------------------------------
 * File          : PEA.v
 * Author        : Pham Hoai Luan
 *                <pham.luan@is.naist.jp>
 * Created       : 2023.03.03
 *-----------------------------------------------------------------------------
 * Last modified : 2023.03.03
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
module	PEA (
	input	wire								  				CLK,
	input	wire								  				RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire [7:0]				              				start_in,
	input  wire						              				Mode_in,
	
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
	
	///*** Context Increment ***///
	input  wire [7:0]				              				CTX_incr_in,
	
	///*** Local Data Memory ***///	
	input  wire [`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:0]     	LDM_addra_in,
	input  wire [`AXI_DWIDTH_BITS-1:0]          				LDM_dina_in,
	input  wire 					              				LDM_ena_in,
	input  wire 					              				LDM_wea_in,
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------//  
	output  wire [`AXI_DWIDTH_BITS-1:0]         				LDM_douta_out
 );

	///*** Interconnection wire ***///
	wire [`DWORD_BITS-1:0]       								PE_D48_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_D48_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_D48_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_D48_3_wr[0:`PE_NUM-1];
									
	wire [`DWORD_BITS-1:0]       								PE_B16_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B16_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B16_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B16_3_wr[0:`PE_NUM-1];
													
	wire [`DWORD_BITS-1:0]       								PE_B8_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B8_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B8_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								PE_B8_3_wr[0:`PE_NUM-1];
	///*** PE wire ***///								
	wire [`DWORD_BITS-1:0]       								HI_D48_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_D48_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_D48_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_D48_3_wr[0:`PE_NUM-1];
																
	wire [`DWORD_BITS-1:0]       								HI_B16_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B16_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B16_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B16_3_wr[0:`PE_NUM-1];
																
	wire [`DWORD_BITS-1:0]       								HI_B8_0_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B8_1_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B8_2_wr[0:`PE_NUM-1];
	wire [`DWORD_BITS-1:0]       								HI_B8_3_wr[0:`PE_NUM-1];
	
	///*** PE reg ***///
		
	wire [`AXI_DWIDTH_BITS-1:0]                					LDM_MSB_douta_rw0_wr, LDM_MSB_douta_rw1_wr, LDM_MSB_douta_rw2_wr, LDM_MSB_douta_rw3_wr;
	wire [`AXI_DWIDTH_BITS-1:0]                					LDM_LSB_douta_rw0_wr, LDM_LSB_douta_rw1_wr, LDM_LSB_douta_rw2_wr, LDM_LSB_douta_rw3_wr;
	wire [`AXI_DWIDTH_BITS-1:0]          						LDM_MSB_dina_wr, LDM_LSB_dina_wr;
	reg [`AXI_DWIDTH_BITS-1:0]                					LDM_mode32_douta_tmp_reg, LDM_mode64_douta_tmp_reg;
	reg [`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:0]         		LDM_addra_reg;
 
	assign  LDM_MSB_dina_wr = ((Mode_in == `MODE32)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == 0))? LDM_dina_in: 
							  (Mode_in == `MODE64)? {LDM_dina_in[255:224],LDM_dina_in[191:160],LDM_dina_in[127:96],LDM_dina_in[63:32],LDM_dina_in[255:224],LDM_dina_in[191:160],LDM_dina_in[127:96],LDM_dina_in[63:32]}: 256'h0;
	
	assign  LDM_LSB_dina_wr = ((Mode_in == `MODE32)&&(LDM_addra_in[`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS] == 1))? LDM_dina_in: 
							  (Mode_in == `MODE64)? {LDM_dina_in[223:192],LDM_dina_in[159:128],LDM_dina_in[95:64],LDM_dina_in[31:0],LDM_dina_in[223:192],LDM_dina_in[159:128],LDM_dina_in[95:64],LDM_dina_in[31:0]}: 256'h0;
							  
	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			LDM_addra_reg  		<= 0;
		end
		else begin
			LDM_addra_reg 		<= LDM_addra_in;
		end
	end
	
	assign LDM_douta_out	= (Mode_in == `MODE32)? LDM_mode32_douta_tmp_reg: LDM_mode64_douta_tmp_reg;
	
	always @* begin
		case (LDM_addra_reg[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS])
			3'b000: begin   
				LDM_mode32_douta_tmp_reg = LDM_MSB_douta_rw0_wr; 
			end
			3'b001: begin   
				LDM_mode32_douta_tmp_reg = LDM_LSB_douta_rw0_wr; 
			end
			3'b010: begin   
				LDM_mode32_douta_tmp_reg = LDM_MSB_douta_rw1_wr; 
			end
			3'b011: begin   
				LDM_mode32_douta_tmp_reg = LDM_LSB_douta_rw1_wr; 
			end
			3'b100: begin   
				LDM_mode32_douta_tmp_reg = LDM_MSB_douta_rw2_wr; 
			end
			3'b101: begin   
				LDM_mode32_douta_tmp_reg = LDM_LSB_douta_rw2_wr; 
			end
			3'b110: begin   
				LDM_mode32_douta_tmp_reg = LDM_MSB_douta_rw3_wr; 
			end
			3'b111: begin   
				LDM_mode32_douta_tmp_reg = LDM_LSB_douta_rw3_wr; 
			end
			default: begin
				LDM_mode32_douta_tmp_reg = LDM_MSB_douta_rw0_wr;
			end
		endcase
	end
 
	always @* begin
		case (LDM_addra_reg[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:`LDM_ADDR_BITS])
			3'b000: begin   
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw0_wr[127:96],LDM_LSB_douta_rw0_wr[127:96],LDM_MSB_douta_rw0_wr[95:64],LDM_LSB_douta_rw0_wr[95:64],LDM_MSB_douta_rw0_wr[63:32],LDM_LSB_douta_rw0_wr[63:32],LDM_MSB_douta_rw0_wr[31:0],LDM_LSB_douta_rw0_wr[31:0]}; 
			end
			3'b001: begin   
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw0_wr[255:224],LDM_LSB_douta_rw0_wr[255:224],LDM_MSB_douta_rw0_wr[223:192],LDM_LSB_douta_rw0_wr[223:192],LDM_MSB_douta_rw0_wr[191:160],LDM_LSB_douta_rw0_wr[191:160],LDM_MSB_douta_rw0_wr[159:128],LDM_LSB_douta_rw0_wr[159:128]}; 
			end
			3'b010: begin   
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw1_wr[127:96],LDM_LSB_douta_rw1_wr[127:96],LDM_MSB_douta_rw1_wr[95:64],LDM_LSB_douta_rw1_wr[95:64],LDM_MSB_douta_rw1_wr[63:32],LDM_LSB_douta_rw1_wr[63:32],LDM_MSB_douta_rw1_wr[31:0],LDM_LSB_douta_rw1_wr[31:0]}; 
			end                            
			3'b011: begin                  
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw1_wr[255:224],LDM_LSB_douta_rw1_wr[255:224],LDM_MSB_douta_rw1_wr[223:192],LDM_LSB_douta_rw1_wr[223:192],LDM_MSB_douta_rw1_wr[191:160],LDM_LSB_douta_rw1_wr[191:160],LDM_MSB_douta_rw1_wr[159:128],LDM_LSB_douta_rw1_wr[159:128]};
			end
			3'b100: begin   
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw2_wr[127:96],LDM_LSB_douta_rw2_wr[127:96],LDM_MSB_douta_rw2_wr[95:64],LDM_LSB_douta_rw2_wr[95:64],LDM_MSB_douta_rw2_wr[63:32],LDM_LSB_douta_rw2_wr[63:32],LDM_MSB_douta_rw2_wr[31:0],LDM_LSB_douta_rw2_wr[31:0]}; 
			end                            
			3'b101: begin                  
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw2_wr[255:224],LDM_LSB_douta_rw2_wr[255:224],LDM_MSB_douta_rw2_wr[223:192],LDM_LSB_douta_rw2_wr[223:192],LDM_MSB_douta_rw2_wr[191:160],LDM_LSB_douta_rw2_wr[191:160],LDM_MSB_douta_rw2_wr[159:128],LDM_LSB_douta_rw2_wr[159:128]};
			end
			3'b110: begin   
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw3_wr[127:96],LDM_LSB_douta_rw3_wr[127:96],LDM_MSB_douta_rw3_wr[95:64],LDM_LSB_douta_rw3_wr[95:64],LDM_MSB_douta_rw3_wr[63:32],LDM_LSB_douta_rw3_wr[63:32],LDM_MSB_douta_rw3_wr[31:0],LDM_LSB_douta_rw3_wr[31:0]}; 
			end                            
			3'b111: begin                  
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw3_wr[255:224],LDM_LSB_douta_rw3_wr[255:224],LDM_MSB_douta_rw3_wr[223:192],LDM_LSB_douta_rw3_wr[223:192],LDM_MSB_douta_rw3_wr[191:160],LDM_LSB_douta_rw3_wr[191:160],LDM_MSB_douta_rw3_wr[159:128],LDM_LSB_douta_rw3_wr[159:128]};
			end
			default: begin
				LDM_mode64_douta_tmp_reg = {LDM_MSB_douta_rw0_wr[127:96],LDM_LSB_douta_rw0_wr[127:96],LDM_MSB_douta_rw0_wr[95:64],LDM_LSB_douta_rw0_wr[95:64],LDM_MSB_douta_rw0_wr[63:32],LDM_LSB_douta_rw0_wr[63:32],LDM_MSB_douta_rw0_wr[31:0],LDM_LSB_douta_rw0_wr[31:0]};
			end
		endcase
	end

//ila_pea D0(
//.clk(CLK),

//.probe0(start_in[0:0]), // 1 bits
//.probe1(CTX_incr_in), // 8 bits
//.probe2(PE_D48_0_wr[0]), // 64 bits
//.probe3(PE_D48_0_wr[1]), // 64 bits
//.probe4(PE_D48_0_wr[2]), // 64 bits
//.probe5(PE_D48_0_wr[3]), // 64 bits
//.probe6(PE_D48_0_wr[4]), // 64 bits
//.probe7(PE_D48_0_wr[5]), // 64 bits
//.probe8(PE_D48_0_wr[6]), // 64 bits
//.probe9(PE_D48_0_wr[7]), // 64 bits
//.probe10(PE_D48_0_wr[8]), // 64 bits
//.probe11(PE_D48_0_wr[9]), // 64 bits
//.probe12(PE_D48_0_wr[10]), // 64 bits
//.probe13(PE_D48_0_wr[11]), // 64 bits
//.probe14(PE_D48_0_wr[12]), // 64 bits
//.probe15(PE_D48_0_wr[13]), // 64 bits
//.probe16(PE_D48_0_wr[14]), // 64 bits
//.probe17(PE_D48_0_wr[15]), // 64 bits
//.probe18(PE_B16_0_wr[0]), // 64 bits
//.probe19(PE_B16_0_wr[1]), // 64 bits
//.probe20(PE_B16_0_wr[2]), // 64 bits
//.probe21(PE_B16_0_wr[3]), // 64 bits
//.probe22(PE_B16_0_wr[4]), // 64 bits
//.probe23(PE_B16_0_wr[5]), // 64 bits
//.probe24(PE_B16_0_wr[6]), // 64 bits
//.probe25(PE_B16_0_wr[7]), // 64 bits
//.probe26(PE_B16_0_wr[8]), // 64 bits
//.probe27(PE_B16_0_wr[9]), // 64 bits
//.probe28(PE_B16_0_wr[10]), // 64 bits
//.probe29(PE_B16_0_wr[11]), // 64 bits
//.probe30(PE_B16_0_wr[12]), // 64 bits
//.probe31(PE_B16_0_wr[13]), // 64 bits
//.probe32(PE_B16_0_wr[14]), // 64 bits
//.probe33(PE_B16_0_wr[15]), // 64 bits
//.probe34(PE_B8_0_wr[0]), // 64 bits
//.probe35(PE_B8_0_wr[1]), // 64 bits
//.probe36(PE_B8_0_wr[2]), // 64 bits
//.probe37(PE_B8_0_wr[3]), // 64 bits
//.probe38(PE_B8_0_wr[4]), // 64 bits
//.probe39(PE_B8_0_wr[5]), // 64 bits
//.probe40(PE_B8_0_wr[6]), // 64 bits
//.probe41(PE_B8_0_wr[7]), // 64 bits
//.probe42(PE_B8_0_wr[8]), // 64 bits
//.probe43(PE_B8_0_wr[9]), // 64 bits
//.probe44(PE_B8_0_wr[10]), // 64 bits
//.probe45(PE_B8_0_wr[11]), // 64 bits
//.probe46(PE_B8_0_wr[12]), // 64 bits
//.probe47(PE_B8_0_wr[13]), // 64 bits
//.probe48(PE_B8_0_wr[14]), // 64 bits
//.probe49(PE_B8_0_wr[15]) // 64 bits
//);

//ila_pea D1(
//.clk(CLK),

//.probe0(start_in[0:0]), // 1 bits
//.probe1(CTX_incr_in), // 8 bits
//.probe2(PE_D48_1_wr[0]), // 64 bits
//.probe3(PE_D48_1_wr[1]), // 64 bits
//.probe4(PE_D48_1_wr[2]), // 64 bits
//.probe5(PE_D48_1_wr[3]), // 64 bits
//.probe6(PE_D48_1_wr[4]), // 64 bits
//.probe7(PE_D48_1_wr[5]), // 64 bits
//.probe8(PE_D48_1_wr[6]), // 64 bits
//.probe9(PE_D48_1_wr[7]), // 64 bits
//.probe10(PE_D48_1_wr[8]), // 64 bits
//.probe11(PE_D48_1_wr[9]), // 64 bits
//.probe12(PE_D48_1_wr[10]), // 64 bits
//.probe13(PE_D48_1_wr[11]), // 64 bits
//.probe14(PE_D48_1_wr[12]), // 64 bits
//.probe15(PE_D48_1_wr[13]), // 64 bits
//.probe16(PE_D48_1_wr[14]), // 64 bits
//.probe17(PE_D48_1_wr[15]), // 64 bits
//.probe18(PE_B16_1_wr[0]), // 64 bits
//.probe19(PE_B16_1_wr[1]), // 64 bits
//.probe20(PE_B16_1_wr[2]), // 64 bits
//.probe21(PE_B16_1_wr[3]), // 64 bits
//.probe22(PE_B16_1_wr[4]), // 64 bits
//.probe23(PE_B16_1_wr[5]), // 64 bits
//.probe24(PE_B16_1_wr[6]), // 64 bits
//.probe25(PE_B16_1_wr[7]), // 64 bits
//.probe26(PE_B16_1_wr[8]), // 64 bits
//.probe27(PE_B16_1_wr[9]), // 64 bits
//.probe28(PE_B16_1_wr[10]), // 64 bits
//.probe29(PE_B16_1_wr[11]), // 64 bits
//.probe30(PE_B16_1_wr[12]), // 64 bits
//.probe31(PE_B16_1_wr[13]), // 64 bits
//.probe32(PE_B16_1_wr[14]), // 64 bits
//.probe33(PE_B16_1_wr[15]), // 64 bits
//.probe34(PE_B8_1_wr[0]), // 64 bits
//.probe35(PE_B8_1_wr[1]), // 64 bits
//.probe36(PE_B8_1_wr[2]), // 64 bits
//.probe37(PE_B8_1_wr[3]), // 64 bits
//.probe38(PE_B8_1_wr[4]), // 64 bits
//.probe39(PE_B8_1_wr[5]), // 64 bits
//.probe40(PE_B8_1_wr[6]), // 64 bits
//.probe41(PE_B8_1_wr[7]), // 64 bits
//.probe42(PE_B8_1_wr[8]), // 64 bits
//.probe43(PE_B8_1_wr[9]), // 64 bits
//.probe44(PE_B8_1_wr[10]), // 64 bits
//.probe45(PE_B8_1_wr[11]), // 64 bits
//.probe46(PE_B8_1_wr[12]), // 64 bits
//.probe47(PE_B8_1_wr[13]), // 64 bits
//.probe48(PE_B8_1_wr[14]), // 64 bits
//.probe49(PE_B8_1_wr[15]) // 64 bits
//);

//ila_pea D2(
//.clk(CLK),

//.probe0(start_in[0:0]), // 1 bits
//.probe1(CTX_incr_in), // 8 bits
//.probe2(PE_D48_2_wr[0]), // 64 bits
//.probe3(PE_D48_2_wr[1]), // 64 bits
//.probe4(PE_D48_2_wr[2]), // 64 bits
//.probe5(PE_D48_2_wr[3]), // 64 bits
//.probe6(PE_D48_2_wr[4]), // 64 bits
//.probe7(PE_D48_2_wr[5]), // 64 bits
//.probe8(PE_D48_2_wr[6]), // 64 bits
//.probe9(PE_D48_2_wr[7]), // 64 bits
//.probe10(PE_D48_2_wr[8]), // 64 bits
//.probe11(PE_D48_2_wr[9]), // 64 bits
//.probe12(PE_D48_2_wr[10]), // 64 bits
//.probe13(PE_D48_2_wr[11]), // 64 bits
//.probe14(PE_D48_2_wr[12]), // 64 bits
//.probe15(PE_D48_2_wr[13]), // 64 bits
//.probe16(PE_D48_2_wr[14]), // 64 bits
//.probe17(PE_D48_2_wr[15]), // 64 bits
//.probe18(PE_B16_2_wr[0]), // 64 bits
//.probe19(PE_B16_2_wr[1]), // 64 bits
//.probe20(PE_B16_2_wr[2]), // 64 bits
//.probe21(PE_B16_2_wr[3]), // 64 bits
//.probe22(PE_B16_2_wr[4]), // 64 bits
//.probe23(PE_B16_2_wr[5]), // 64 bits
//.probe24(PE_B16_2_wr[6]), // 64 bits
//.probe25(PE_B16_2_wr[7]), // 64 bits
//.probe26(PE_B16_2_wr[8]), // 64 bits
//.probe27(PE_B16_2_wr[9]), // 64 bits
//.probe28(PE_B16_2_wr[10]), // 64 bits
//.probe29(PE_B16_2_wr[11]), // 64 bits
//.probe30(PE_B16_2_wr[12]), // 64 bits
//.probe31(PE_B16_2_wr[13]), // 64 bits
//.probe32(PE_B16_2_wr[14]), // 64 bits
//.probe33(PE_B16_2_wr[15]), // 64 bits
//.probe34(PE_B8_2_wr[0]), // 64 bits
//.probe35(PE_B8_2_wr[1]), // 64 bits
//.probe36(PE_B8_2_wr[2]), // 64 bits
//.probe37(PE_B8_2_wr[3]), // 64 bits
//.probe38(PE_B8_2_wr[4]), // 64 bits
//.probe39(PE_B8_2_wr[5]), // 64 bits
//.probe40(PE_B8_2_wr[6]), // 64 bits
//.probe41(PE_B8_2_wr[7]), // 64 bits
//.probe42(PE_B8_2_wr[8]), // 64 bits
//.probe43(PE_B8_2_wr[9]), // 64 bits
//.probe44(PE_B8_2_wr[10]), // 64 bits
//.probe45(PE_B8_2_wr[11]), // 64 bits
//.probe46(PE_B8_2_wr[12]), // 64 bits
//.probe47(PE_B8_2_wr[13]), // 64 bits
//.probe48(PE_B8_2_wr[14]), // 64 bits
//.probe49(PE_B8_2_wr[15]) // 64 bits
//);

// ila_pea D3(
// .clk(CLK),

// .probe0(start_in[0:0]), // 1 bits
// .probe1(CTX_incr_in), // 8 bits
// .probe2(PE_D48_3_wr[0]), // 64 bits
// .probe3(PE_D48_3_wr[1]), // 64 bits
// .probe4(PE_D48_3_wr[2]), // 64 bits
// .probe5(PE_D48_3_wr[3]), // 64 bits
// .probe6(PE_D48_3_wr[4]), // 64 bits
// .probe7(PE_D48_3_wr[5]), // 64 bits
// .probe8(PE_D48_3_wr[6]), // 64 bits
// .probe9(PE_D48_3_wr[7]), // 64 bits
// .probe10(PE_D48_3_wr[8]), // 64 bits
// .probe11(PE_D48_3_wr[9]), // 64 bits
// .probe12(PE_D48_3_wr[10]), // 64 bits
// .probe13(PE_D48_3_wr[11]), // 64 bits
// .probe14(PE_D48_3_wr[12]), // 64 bits
// .probe15(PE_D48_3_wr[13]), // 64 bits
// .probe16(PE_D48_3_wr[14]), // 64 bits
// .probe17(PE_D48_3_wr[15]), // 64 bits
// .probe18(PE_B16_3_wr[0]), // 64 bits
// .probe19(PE_B16_3_wr[1]), // 64 bits
// .probe20(PE_B16_3_wr[2]), // 64 bits
// .probe21(PE_B16_3_wr[3]), // 64 bits
// .probe22(PE_B16_3_wr[4]), // 64 bits
// .probe23(PE_B16_3_wr[5]), // 64 bits
// .probe24(PE_B16_3_wr[6]), // 64 bits
// .probe25(PE_B16_3_wr[7]), // 64 bits
// .probe26(PE_B16_3_wr[8]), // 64 bits
// .probe27(PE_B16_3_wr[9]), // 64 bits
// .probe28(PE_B16_3_wr[10]), // 64 bits
// .probe29(PE_B16_3_wr[11]), // 64 bits
// .probe30(PE_B16_3_wr[12]), // 64 bits
// .probe31(PE_B16_3_wr[13]), // 64 bits
// .probe32(PE_B16_3_wr[14]), // 64 bits
// .probe33(PE_B16_3_wr[15]), // 64 bits
// .probe34(PE_B8_3_wr[0]), // 64 bits
// .probe35(PE_B8_3_wr[1]), // 64 bits
// .probe36(PE_B8_3_wr[2]), // 64 bits
// .probe37(PE_B8_3_wr[3]), // 64 bits
// .probe38(PE_B8_3_wr[4]), // 64 bits
// .probe39(PE_B8_3_wr[5]), // 64 bits
// .probe40(PE_B8_3_wr[6]), // 64 bits
// .probe41(PE_B8_3_wr[7]), // 64 bits
// .probe42(PE_B8_3_wr[8]), // 64 bits
// .probe43(PE_B8_3_wr[9]), // 64 bits
// .probe44(PE_B8_3_wr[10]), // 64 bits
// .probe45(PE_B8_3_wr[11]), // 64 bits
// .probe46(PE_B8_3_wr[12]), // 64 bits
// .probe47(PE_B8_3_wr[13]), // 64 bits
// .probe48(PE_B8_3_wr[14]), // 64 bits
// .probe49(PE_B8_3_wr[15]) // 64 bits
// );

// ila_pea HI0(
// .clk(CLK),

// .probe0(start_in[0:0]), // 1 bits
// .probe1(CTX_incr_in), // 8 bits
// .probe2(HI_D48_0_wr[0]), // 64 bits
// .probe3(HI_D48_0_wr[1]), // 64 bits
// .probe4(HI_D48_0_wr[2]), // 64 bits
// .probe5(HI_D48_0_wr[3]), // 64 bits
// .probe6(HI_D48_0_wr[4]), // 64 bits
// .probe7(HI_D48_0_wr[5]), // 64 bits
// .probe8(HI_D48_0_wr[6]), // 64 bits
// .probe9(HI_D48_0_wr[7]), // 64 bits
// .probe10(HI_D48_0_wr[8]), // 64 bits
// .probe11(HI_D48_0_wr[9]), // 64 bits
// .probe12(HI_D48_0_wr[10]), // 64 bits
// .probe13(HI_D48_0_wr[11]), // 64 bits
// .probe14(HI_D48_0_wr[12]), // 64 bits
// .probe15(HI_D48_0_wr[13]), // 64 bits
// .probe16(HI_D48_0_wr[14]), // 64 bits
// .probe17(HI_D48_0_wr[15]), // 64 bits
// .probe18(HI_B16_0_wr[0]), // 64 bits
// .probe19(HI_B16_0_wr[1]), // 64 bits
// .probe20(HI_B16_0_wr[2]), // 64 bits
// .probe21(HI_B16_0_wr[3]), // 64 bits
// .probe22(HI_B16_0_wr[4]), // 64 bits
// .probe23(HI_B16_0_wr[5]), // 64 bits
// .probe24(HI_B16_0_wr[6]), // 64 bits
// .probe25(HI_B16_0_wr[7]), // 64 bits
// .probe26(HI_B16_0_wr[8]), // 64 bits
// .probe27(HI_B16_0_wr[9]), // 64 bits
// .probe28(HI_B16_0_wr[10]), // 64 bits
// .probe29(HI_B16_0_wr[11]), // 64 bits
// .probe30(HI_B16_0_wr[12]), // 64 bits
// .probe31(HI_B16_0_wr[13]), // 64 bits
// .probe32(HI_B16_0_wr[14]), // 64 bits
// .probe33(HI_B16_0_wr[15]), // 64 bits
// .probe34(HI_B8_0_wr[0]), // 64 bits
// .probe35(HI_B8_0_wr[1]), // 64 bits
// .probe36(HI_B8_0_wr[2]), // 64 bits
// .probe37(HI_B8_0_wr[3]), // 64 bits
// .probe38(HI_B8_0_wr[4]), // 64 bits
// .probe39(HI_B8_0_wr[5]), // 64 bits
// .probe40(HI_B8_0_wr[6]), // 64 bits
// .probe41(HI_B8_0_wr[7]), // 64 bits
// .probe42(HI_B8_0_wr[8]), // 64 bits
// .probe43(HI_B8_0_wr[9]), // 64 bits
// .probe44(HI_B8_0_wr[10]), // 64 bits
// .probe45(HI_B8_0_wr[11]), // 64 bits
// .probe46(HI_B8_0_wr[12]), // 64 bits
// .probe47(HI_B8_0_wr[13]), // 64 bits
// .probe48(HI_B8_0_wr[14]), // 64 bits
// .probe49(HI_B8_0_wr[15]) // 64 bits
// );

// ila_pea HI1(
// .clk(CLK),

// .probe0(start_in[0:0]), // 1 bits
// .probe1(CTX_incr_in), // 8 bits
// .probe2(HI_D48_1_wr[0]), // 64 bits
// .probe3(HI_D48_1_wr[1]), // 64 bits
// .probe4(HI_D48_1_wr[2]), // 64 bits
// .probe5(HI_D48_1_wr[3]), // 64 bits
// .probe6(HI_D48_1_wr[4]), // 64 bits
// .probe7(HI_D48_1_wr[5]), // 64 bits
// .probe8(HI_D48_1_wr[6]), // 64 bits
// .probe9(HI_D48_1_wr[7]), // 64 bits
// .probe10(HI_D48_1_wr[8]), // 64 bits
// .probe11(HI_D48_1_wr[9]), // 64 bits
// .probe12(HI_D48_1_wr[10]), // 64 bits
// .probe13(HI_D48_1_wr[11]), // 64 bits
// .probe14(HI_D48_1_wr[12]), // 64 bits
// .probe15(HI_D48_1_wr[13]), // 64 bits
// .probe16(HI_D48_1_wr[14]), // 64 bits
// .probe17(HI_D48_1_wr[15]), // 64 bits
// .probe18(HI_B16_1_wr[0]), // 64 bits
// .probe19(HI_B16_1_wr[1]), // 64 bits
// .probe20(HI_B16_1_wr[2]), // 64 bits
// .probe21(HI_B16_1_wr[3]), // 64 bits
// .probe22(HI_B16_1_wr[4]), // 64 bits
// .probe23(HI_B16_1_wr[5]), // 64 bits
// .probe24(HI_B16_1_wr[6]), // 64 bits
// .probe25(HI_B16_1_wr[7]), // 64 bits
// .probe26(HI_B16_1_wr[8]), // 64 bits
// .probe27(HI_B16_1_wr[9]), // 64 bits
// .probe28(HI_B16_1_wr[10]), // 64 bits
// .probe29(HI_B16_1_wr[11]), // 64 bits
// .probe30(HI_B16_1_wr[12]), // 64 bits
// .probe31(HI_B16_1_wr[13]), // 64 bits
// .probe32(HI_B16_1_wr[14]), // 64 bits
// .probe33(HI_B16_1_wr[15]), // 64 bits
// .probe34(HI_B8_1_wr[0]), // 64 bits
// .probe35(HI_B8_1_wr[1]), // 64 bits
// .probe36(HI_B8_1_wr[2]), // 64 bits
// .probe37(HI_B8_1_wr[3]), // 64 bits
// .probe38(HI_B8_1_wr[4]), // 64 bits
// .probe39(HI_B8_1_wr[5]), // 64 bits
// .probe40(HI_B8_1_wr[6]), // 64 bits
// .probe41(HI_B8_1_wr[7]), // 64 bits
// .probe42(HI_B8_1_wr[8]), // 64 bits
// .probe43(HI_B8_1_wr[9]), // 64 bits
// .probe44(HI_B8_1_wr[10]), // 64 bits
// .probe45(HI_B8_1_wr[11]), // 64 bits
// .probe46(HI_B8_1_wr[12]), // 64 bits
// .probe47(HI_B8_1_wr[13]), // 64 bits
// .probe48(HI_B8_1_wr[14]), // 64 bits
// .probe49(HI_B8_1_wr[15]) // 64 bits
// );

// ila_pea HI2(
// .clk(CLK),

// .probe0(start_in[0:0]), // 1 bits
// .probe1(CTX_incr_in), // 8 bits
// .probe2(HI_D48_2_wr[0]), // 64 bits
// .probe3(HI_D48_2_wr[1]), // 64 bits
// .probe4(HI_D48_2_wr[2]), // 64 bits
// .probe5(HI_D48_2_wr[3]), // 64 bits
// .probe6(HI_D48_2_wr[4]), // 64 bits
// .probe7(HI_D48_2_wr[5]), // 64 bits
// .probe8(HI_D48_2_wr[6]), // 64 bits
// .probe9(HI_D48_2_wr[7]), // 64 bits
// .probe10(HI_D48_2_wr[8]), // 64 bits
// .probe11(HI_D48_2_wr[9]), // 64 bits
// .probe12(HI_D48_2_wr[10]), // 64 bits
// .probe13(HI_D48_2_wr[11]), // 64 bits
// .probe14(HI_D48_2_wr[12]), // 64 bits
// .probe15(HI_D48_2_wr[13]), // 64 bits
// .probe16(HI_D48_2_wr[14]), // 64 bits
// .probe17(HI_D48_2_wr[15]), // 64 bits
// .probe18(HI_B16_2_wr[0]), // 64 bits
// .probe19(HI_B16_2_wr[1]), // 64 bits
// .probe20(HI_B16_2_wr[2]), // 64 bits
// .probe21(HI_B16_2_wr[3]), // 64 bits
// .probe22(HI_B16_2_wr[4]), // 64 bits
// .probe23(HI_B16_2_wr[5]), // 64 bits
// .probe24(HI_B16_2_wr[6]), // 64 bits
// .probe25(HI_B16_2_wr[7]), // 64 bits
// .probe26(HI_B16_2_wr[8]), // 64 bits
// .probe27(HI_B16_2_wr[9]), // 64 bits
// .probe28(HI_B16_2_wr[10]), // 64 bits
// .probe29(HI_B16_2_wr[11]), // 64 bits
// .probe30(HI_B16_2_wr[12]), // 64 bits
// .probe31(HI_B16_2_wr[13]), // 64 bits
// .probe32(HI_B16_2_wr[14]), // 64 bits
// .probe33(HI_B16_2_wr[15]), // 64 bits
// .probe34(HI_B8_2_wr[0]), // 64 bits
// .probe35(HI_B8_2_wr[1]), // 64 bits
// .probe36(HI_B8_2_wr[2]), // 64 bits
// .probe37(HI_B8_2_wr[3]), // 64 bits
// .probe38(HI_B8_2_wr[4]), // 64 bits
// .probe39(HI_B8_2_wr[5]), // 64 bits
// .probe40(HI_B8_2_wr[6]), // 64 bits
// .probe41(HI_B8_2_wr[7]), // 64 bits
// .probe42(HI_B8_2_wr[8]), // 64 bits
// .probe43(HI_B8_2_wr[9]), // 64 bits
// .probe44(HI_B8_2_wr[10]), // 64 bits
// .probe45(HI_B8_2_wr[11]), // 64 bits
// .probe46(HI_B8_2_wr[12]), // 64 bits
// .probe47(HI_B8_2_wr[13]), // 64 bits
// .probe48(HI_B8_2_wr[14]), // 64 bits
// .probe49(HI_B8_2_wr[15]) // 64 bits
// );

// ila_pea HI3(
// .clk(CLK),

// .probe0(start_in[0:0]), // 1 bits
// .probe1(CTX_incr_in), // 8 bits
// .probe2(HI_D48_3_wr[0]), // 64 bits
// .probe3(HI_D48_3_wr[1]), // 64 bits
// .probe4(HI_D48_3_wr[2]), // 64 bits
// .probe5(HI_D48_3_wr[3]), // 64 bits
// .probe6(HI_D48_3_wr[4]), // 64 bits
// .probe7(HI_D48_3_wr[5]), // 64 bits
// .probe8(HI_D48_3_wr[6]), // 64 bits
// .probe9(HI_D48_3_wr[7]), // 64 bits
// .probe10(HI_D48_3_wr[8]), // 64 bits
// .probe11(HI_D48_3_wr[9]), // 64 bits
// .probe12(HI_D48_3_wr[10]), // 64 bits
// .probe13(HI_D48_3_wr[11]), // 64 bits
// .probe14(HI_D48_3_wr[12]), // 64 bits
// .probe15(HI_D48_3_wr[13]), // 64 bits
// .probe16(HI_D48_3_wr[14]), // 64 bits
// .probe17(HI_D48_3_wr[15]), // 64 bits
// .probe18(HI_B16_3_wr[0]), // 64 bits
// .probe19(HI_B16_3_wr[1]), // 64 bits
// .probe20(HI_B16_3_wr[2]), // 64 bits
// .probe21(HI_B16_3_wr[3]), // 64 bits
// .probe22(HI_B16_3_wr[4]), // 64 bits
// .probe23(HI_B16_3_wr[5]), // 64 bits
// .probe24(HI_B16_3_wr[6]), // 64 bits
// .probe25(HI_B16_3_wr[7]), // 64 bits
// .probe26(HI_B16_3_wr[8]), // 64 bits
// .probe27(HI_B16_3_wr[9]), // 64 bits
// .probe28(HI_B16_3_wr[10]), // 64 bits
// .probe29(HI_B16_3_wr[11]), // 64 bits
// .probe30(HI_B16_3_wr[12]), // 64 bits
// .probe31(HI_B16_3_wr[13]), // 64 bits
// .probe32(HI_B16_3_wr[14]), // 64 bits
// .probe33(HI_B16_3_wr[15]), // 64 bits
// .probe34(HI_B8_3_wr[0]), // 64 bits
// .probe35(HI_B8_3_wr[1]), // 64 bits
// .probe36(HI_B8_3_wr[2]), // 64 bits
// .probe37(HI_B8_3_wr[3]), // 64 bits
// .probe38(HI_B8_3_wr[4]), // 64 bits
// .probe39(HI_B8_3_wr[5]), // 64 bits
// .probe40(HI_B8_3_wr[6]), // 64 bits
// .probe41(HI_B8_3_wr[7]), // 64 bits
// .probe42(HI_B8_3_wr[8]), // 64 bits
// .probe43(HI_B8_3_wr[9]), // 64 bits
// .probe44(HI_B8_3_wr[10]), // 64 bits
// .probe45(HI_B8_3_wr[11]), // 64 bits
// .probe46(HI_B8_3_wr[12]), // 64 bits
// .probe47(HI_B8_3_wr[13]), // 64 bits
// .probe48(HI_B8_3_wr[14]), // 64 bits
// .probe49(HI_B8_3_wr[15]) // 64 bits
// );
	
  //-----------------------------------------------------//
  //          			Row 0                   		 // 
  //-----------------------------------------------------//

Row_Connection_PE0
#(
  .UNIT_NO(0)
)
 RC0_0(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[0:0]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[0]),
  .PE0_S48_0_in(PE_D48_0_wr[12]),
  .PE0_S48_1_in(PE_D48_1_wr[12]),
  .PE0_S48_2_in(PE_D48_2_wr[12]),
  .PE0_S48_3_in(PE_D48_3_wr[12]),
  .PE0_B16_0_in(PE_B16_0_wr[12]),
  .PE0_B16_1_in(PE_B16_1_wr[12]),
  .PE0_B16_2_in(PE_B16_2_wr[12]),
  .PE0_B16_3_in(PE_B16_3_wr[12]),
  .PE0_B8_0_in(PE_B8_0_wr[12]),
  .PE0_B8_1_in(PE_B8_1_wr[12]),
  .PE0_B8_2_in(PE_B8_2_wr[12]),
  .PE0_B8_3_in(PE_B8_3_wr[12]),
  .PE1_S48_0_in(PE_D48_0_wr[13]),
  .PE1_S48_1_in(PE_D48_1_wr[13]),
  .PE1_S48_2_in(PE_D48_2_wr[13]),
  .PE1_S48_3_in(PE_D48_3_wr[13]),
  .PE1_B16_0_in(PE_B16_0_wr[13]),
  .PE1_B16_1_in(PE_B16_1_wr[13]),
  .PE1_B16_2_in(PE_B16_2_wr[13]),
  .PE1_B16_3_in(PE_B16_3_wr[13]),
  .PE1_B8_0_in(PE_B8_0_wr[13]),
  .PE1_B8_1_in(PE_B8_1_wr[13]),
  .PE1_B8_2_in(PE_B8_2_wr[13]),
  .PE1_B8_3_in(PE_B8_3_wr[13]),
  .PE2_S48_0_in(PE_D48_0_wr[14]),
  .PE2_S48_1_in(PE_D48_1_wr[14]),
  .PE2_S48_2_in(PE_D48_2_wr[14]),
  .PE2_S48_3_in(PE_D48_3_wr[14]),
  .PE2_B16_0_in(PE_B16_0_wr[14]),
  .PE2_B16_1_in(PE_B16_1_wr[14]),
  .PE2_B16_2_in(PE_B16_2_wr[14]),
  .PE2_B16_3_in(PE_B16_3_wr[14]),
  .PE2_B8_0_in(PE_B8_0_wr[14]),
  .PE2_B8_1_in(PE_B8_1_wr[14]),
  .PE2_B8_2_in(PE_B8_2_wr[14]),
  .PE2_B8_3_in(PE_B8_3_wr[14]),
  .PE3_S48_0_in(PE_D48_0_wr[15]),
  .PE3_S48_1_in(PE_D48_1_wr[15]),
  .PE3_S48_2_in(PE_D48_2_wr[15]),
  .PE3_S48_3_in(PE_D48_3_wr[15]),
  .PE3_B16_0_in(PE_B16_0_wr[15]),
  .PE3_B16_1_in(PE_B16_1_wr[15]),
  .PE3_B16_2_in(PE_B16_2_wr[15]),
  .PE3_B16_3_in(PE_B16_3_wr[15]),
  .PE3_B8_0_in(PE_B8_0_wr[15]),
  .PE3_B8_1_in(PE_B8_1_wr[15]),
  .PE3_B8_2_in(PE_B8_2_wr[15]),
  .PE3_B8_3_in(PE_B8_3_wr[15]),
  .D48_0_out(HI_D48_0_wr[0]),
  .D48_1_out(HI_D48_1_wr[0]),
  .D48_2_out(HI_D48_2_wr[0]),
  .D48_3_out(HI_D48_3_wr[0]),
  .B16_0_out(HI_B16_0_wr[0]),
  .B16_1_out(HI_B16_1_wr[0]),
  .B16_2_out(HI_B16_2_wr[0]),
  .B16_3_out(HI_B16_3_wr[0]),
  .B8_0_out(HI_B8_0_wr[0]),
  .B8_1_out(HI_B8_1_wr[0]),
  .B8_2_out(HI_B8_2_wr[0]),
  .B8_3_out(HI_B8_3_wr[0])
);

Row_Connection_PE1
#(
  .UNIT_NO(1)
)
 RC0_1(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[0:0]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[0]),
  .PE0_S48_0_in(PE_D48_0_wr[12]),
  .PE0_S48_1_in(PE_D48_1_wr[12]),
  .PE0_S48_2_in(PE_D48_2_wr[12]),
  .PE0_S48_3_in(PE_D48_3_wr[12]),
  .PE0_B16_0_in(PE_B16_0_wr[12]),
  .PE0_B16_1_in(PE_B16_1_wr[12]),
  .PE0_B16_2_in(PE_B16_2_wr[12]),
  .PE0_B16_3_in(PE_B16_3_wr[12]),
  .PE0_B8_0_in(PE_B8_0_wr[12]),
  .PE0_B8_1_in(PE_B8_1_wr[12]),
  .PE0_B8_2_in(PE_B8_2_wr[12]),
  .PE0_B8_3_in(PE_B8_3_wr[12]),
  .PE1_S48_0_in(PE_D48_0_wr[13]),
  .PE1_S48_1_in(PE_D48_1_wr[13]),
  .PE1_S48_2_in(PE_D48_2_wr[13]),
  .PE1_S48_3_in(PE_D48_3_wr[13]),
  .PE1_B16_0_in(PE_B16_0_wr[13]),
  .PE1_B16_1_in(PE_B16_1_wr[13]),
  .PE1_B16_2_in(PE_B16_2_wr[13]),
  .PE1_B16_3_in(PE_B16_3_wr[13]),
  .PE1_B8_0_in(PE_B8_0_wr[13]),
  .PE1_B8_1_in(PE_B8_1_wr[13]),
  .PE1_B8_2_in(PE_B8_2_wr[13]),
  .PE1_B8_3_in(PE_B8_3_wr[13]),
  .PE2_S48_0_in(PE_D48_0_wr[14]),
  .PE2_S48_1_in(PE_D48_1_wr[14]),
  .PE2_S48_2_in(PE_D48_2_wr[14]),
  .PE2_S48_3_in(PE_D48_3_wr[14]),
  .PE2_B16_0_in(PE_B16_0_wr[14]),
  .PE2_B16_1_in(PE_B16_1_wr[14]),
  .PE2_B16_2_in(PE_B16_2_wr[14]),
  .PE2_B16_3_in(PE_B16_3_wr[14]),
  .PE2_B8_0_in(PE_B8_0_wr[14]),
  .PE2_B8_1_in(PE_B8_1_wr[14]),
  .PE2_B8_2_in(PE_B8_2_wr[14]),
  .PE2_B8_3_in(PE_B8_3_wr[14]),
  .PE3_S48_0_in(PE_D48_0_wr[15]),
  .PE3_S48_1_in(PE_D48_1_wr[15]),
  .PE3_S48_2_in(PE_D48_2_wr[15]),
  .PE3_S48_3_in(PE_D48_3_wr[15]),
  .PE3_B16_0_in(PE_B16_0_wr[15]),
  .PE3_B16_1_in(PE_B16_1_wr[15]),
  .PE3_B16_2_in(PE_B16_2_wr[15]),
  .PE3_B16_3_in(PE_B16_3_wr[15]),
  .PE3_B8_0_in(PE_B8_0_wr[15]),
  .PE3_B8_1_in(PE_B8_1_wr[15]),
  .PE3_B8_2_in(PE_B8_2_wr[15]),
  .PE3_B8_3_in(PE_B8_3_wr[15]),
  .D48_0_out(HI_D48_0_wr[1]),
  .D48_1_out(HI_D48_1_wr[1]),
  .D48_2_out(HI_D48_2_wr[1]),
  .D48_3_out(HI_D48_3_wr[1]),
  .B16_0_out(HI_B16_0_wr[1]),
  .B16_1_out(HI_B16_1_wr[1]),
  .B16_2_out(HI_B16_2_wr[1]),
  .B16_3_out(HI_B16_3_wr[1]),
  .B8_0_out(HI_B8_0_wr[1]),
  .B8_1_out(HI_B8_1_wr[1]),
  .B8_2_out(HI_B8_2_wr[1]),
  .B8_3_out(HI_B8_3_wr[1])
);	

Row_Connection_PE2
#(
  .UNIT_NO(2)
)
 RC0_2(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[0:0]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[0]),
  .PE0_S48_0_in(PE_D48_0_wr[12]),
  .PE0_S48_1_in(PE_D48_1_wr[12]),
  .PE0_S48_2_in(PE_D48_2_wr[12]),
  .PE0_S48_3_in(PE_D48_3_wr[12]),
  .PE0_B16_0_in(PE_B16_0_wr[12]),
  .PE0_B16_1_in(PE_B16_1_wr[12]),
  .PE0_B16_2_in(PE_B16_2_wr[12]),
  .PE0_B16_3_in(PE_B16_3_wr[12]),
  .PE0_B8_0_in(PE_B8_0_wr[12]),
  .PE0_B8_1_in(PE_B8_1_wr[12]),
  .PE0_B8_2_in(PE_B8_2_wr[12]),
  .PE0_B8_3_in(PE_B8_3_wr[12]),
  .PE1_S48_0_in(PE_D48_0_wr[13]),
  .PE1_S48_1_in(PE_D48_1_wr[13]),
  .PE1_S48_2_in(PE_D48_2_wr[13]),
  .PE1_S48_3_in(PE_D48_3_wr[13]),
  .PE1_B16_0_in(PE_B16_0_wr[13]),
  .PE1_B16_1_in(PE_B16_1_wr[13]),
  .PE1_B16_2_in(PE_B16_2_wr[13]),
  .PE1_B16_3_in(PE_B16_3_wr[13]),
  .PE1_B8_0_in(PE_B8_0_wr[13]),
  .PE1_B8_1_in(PE_B8_1_wr[13]),
  .PE1_B8_2_in(PE_B8_2_wr[13]),
  .PE1_B8_3_in(PE_B8_3_wr[13]),
  .PE2_S48_0_in(PE_D48_0_wr[14]),
  .PE2_S48_1_in(PE_D48_1_wr[14]),
  .PE2_S48_2_in(PE_D48_2_wr[14]),
  .PE2_S48_3_in(PE_D48_3_wr[14]),
  .PE2_B16_0_in(PE_B16_0_wr[14]),
  .PE2_B16_1_in(PE_B16_1_wr[14]),
  .PE2_B16_2_in(PE_B16_2_wr[14]),
  .PE2_B16_3_in(PE_B16_3_wr[14]),
  .PE2_B8_0_in(PE_B8_0_wr[14]),
  .PE2_B8_1_in(PE_B8_1_wr[14]),
  .PE2_B8_2_in(PE_B8_2_wr[14]),
  .PE2_B8_3_in(PE_B8_3_wr[14]),
  .PE3_S48_0_in(PE_D48_0_wr[15]),
  .PE3_S48_1_in(PE_D48_1_wr[15]),
  .PE3_S48_2_in(PE_D48_2_wr[15]),
  .PE3_S48_3_in(PE_D48_3_wr[15]),
  .PE3_B16_0_in(PE_B16_0_wr[15]),
  .PE3_B16_1_in(PE_B16_1_wr[15]),
  .PE3_B16_2_in(PE_B16_2_wr[15]),
  .PE3_B16_3_in(PE_B16_3_wr[15]),
  .PE3_B8_0_in(PE_B8_0_wr[15]),
  .PE3_B8_1_in(PE_B8_1_wr[15]),
  .PE3_B8_2_in(PE_B8_2_wr[15]),
  .PE3_B8_3_in(PE_B8_3_wr[15]),
  .D48_0_out(HI_D48_0_wr[2]),
  .D48_1_out(HI_D48_1_wr[2]),
  .D48_2_out(HI_D48_2_wr[2]),
  .D48_3_out(HI_D48_3_wr[2]),
  .B16_0_out(HI_B16_0_wr[2]),
  .B16_1_out(HI_B16_1_wr[2]),
  .B16_2_out(HI_B16_2_wr[2]),
  .B16_3_out(HI_B16_3_wr[2]),
  .B8_0_out(HI_B8_0_wr[2]),
  .B8_1_out(HI_B8_1_wr[2]),
  .B8_2_out(HI_B8_2_wr[2]),
  .B8_3_out(HI_B8_3_wr[2])
);	

Row_Connection_PE3
#(
  .UNIT_NO(3)
)
 RC0_3(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[0:0]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[0]),
  .PE0_S48_0_in(PE_D48_0_wr[12]),
  .PE0_S48_1_in(PE_D48_1_wr[12]),
  .PE0_S48_2_in(PE_D48_2_wr[12]),
  .PE0_S48_3_in(PE_D48_3_wr[12]),
  .PE0_B16_0_in(PE_B16_0_wr[12]),
  .PE0_B16_1_in(PE_B16_1_wr[12]),
  .PE0_B16_2_in(PE_B16_2_wr[12]),
  .PE0_B16_3_in(PE_B16_3_wr[12]),
  .PE0_B8_0_in(PE_B8_0_wr[12]),
  .PE0_B8_1_in(PE_B8_1_wr[12]),
  .PE0_B8_2_in(PE_B8_2_wr[12]),
  .PE0_B8_3_in(PE_B8_3_wr[12]),
  .PE1_S48_0_in(PE_D48_0_wr[13]),
  .PE1_S48_1_in(PE_D48_1_wr[13]),
  .PE1_S48_2_in(PE_D48_2_wr[13]),
  .PE1_S48_3_in(PE_D48_3_wr[13]),
  .PE1_B16_0_in(PE_B16_0_wr[13]),
  .PE1_B16_1_in(PE_B16_1_wr[13]),
  .PE1_B16_2_in(PE_B16_2_wr[13]),
  .PE1_B16_3_in(PE_B16_3_wr[13]),
  .PE1_B8_0_in(PE_B8_0_wr[13]),
  .PE1_B8_1_in(PE_B8_1_wr[13]),
  .PE1_B8_2_in(PE_B8_2_wr[13]),
  .PE1_B8_3_in(PE_B8_3_wr[13]),
  .PE2_S48_0_in(PE_D48_0_wr[14]),
  .PE2_S48_1_in(PE_D48_1_wr[14]),
  .PE2_S48_2_in(PE_D48_2_wr[14]),
  .PE2_S48_3_in(PE_D48_3_wr[14]),
  .PE2_B16_0_in(PE_B16_0_wr[14]),
  .PE2_B16_1_in(PE_B16_1_wr[14]),
  .PE2_B16_2_in(PE_B16_2_wr[14]),
  .PE2_B16_3_in(PE_B16_3_wr[14]),
  .PE2_B8_0_in(PE_B8_0_wr[14]),
  .PE2_B8_1_in(PE_B8_1_wr[14]),
  .PE2_B8_2_in(PE_B8_2_wr[14]),
  .PE2_B8_3_in(PE_B8_3_wr[14]),
  .PE3_S48_0_in(PE_D48_0_wr[15]),
  .PE3_S48_1_in(PE_D48_1_wr[15]),
  .PE3_S48_2_in(PE_D48_2_wr[15]),
  .PE3_S48_3_in(PE_D48_3_wr[15]),
  .PE3_B16_0_in(PE_B16_0_wr[15]),
  .PE3_B16_1_in(PE_B16_1_wr[15]),
  .PE3_B16_2_in(PE_B16_2_wr[15]),
  .PE3_B16_3_in(PE_B16_3_wr[15]),
  .PE3_B8_0_in(PE_B8_0_wr[15]),
  .PE3_B8_1_in(PE_B8_1_wr[15]),
  .PE3_B8_2_in(PE_B8_2_wr[15]),
  .PE3_B8_3_in(PE_B8_3_wr[15]),
  .D48_0_out(HI_D48_0_wr[3]),
  .D48_1_out(HI_D48_1_wr[3]),
  .D48_2_out(HI_D48_2_wr[3]),
  .D48_3_out(HI_D48_3_wr[3]),
  .B16_0_out(HI_B16_0_wr[3]),
  .B16_1_out(HI_B16_1_wr[3]),
  .B16_2_out(HI_B16_2_wr[3]),
  .B16_3_out(HI_B16_3_wr[3]),
  .B8_0_out(HI_B8_0_wr[3]),
  .B8_1_out(HI_B8_1_wr[3]),
  .B8_2_out(HI_B8_2_wr[3]),
  .B8_3_out(HI_B8_3_wr[3])
);	

PE
#(
  .UNIT_NO(0),
  .ROW_NO(0),
  .LR_NO(0)
)
 PE0 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[1]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[1]),	
	//-MSB-//  
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[63:0]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw0_wr[63:0]),		
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[63:0]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw0_wr[63:0]),  
  .S48_0_in(HI_D48_0_wr[0]),
  .S48_1_in(HI_D48_1_wr[0]),
  .S48_2_in(HI_D48_2_wr[0]),
  .S48_3_in(HI_D48_3_wr[0]),
  .B16_0_in(HI_B16_0_wr[0]),
  .B16_1_in(HI_B16_1_wr[0]),
  .B16_2_in(HI_B16_2_wr[0]),
  .B16_3_in(HI_B16_3_wr[0]),
  .B8_0_in(HI_B8_0_wr[0]),
  .B8_1_in(HI_B8_1_wr[0]),
  .B8_2_in(HI_B8_2_wr[0]),
  .B8_3_in(HI_B8_3_wr[0]),
  .D48_0_out(PE_D48_0_wr[0]),
  .D48_1_out(PE_D48_1_wr[0]),
  .D48_2_out(PE_D48_2_wr[0]),
  .D48_3_out(PE_D48_3_wr[0]),
  .B16_0_out(PE_B16_0_wr[0]),
  .B16_1_out(PE_B16_1_wr[0]),
  .B16_2_out(PE_B16_2_wr[0]),
  .B16_3_out(PE_B16_3_wr[0]),
  .B8_0_out(PE_B8_0_wr[0]),
  .B8_1_out(PE_B8_1_wr[0]),
  .B8_2_out(PE_B8_2_wr[0]),
  .B8_3_out(PE_B8_3_wr[0])
);

PE
#(
  .UNIT_NO(1),
  .ROW_NO(0),
  .LR_NO(0)
)
 PE1 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[1]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[1]),	
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[127:64]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw0_wr[127:64]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[127:64]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw0_wr[127:64]),	
  .S48_0_in(HI_D48_0_wr[1]),
  .S48_1_in(HI_D48_1_wr[1]),
  .S48_2_in(HI_D48_2_wr[1]),
  .S48_3_in(HI_D48_3_wr[1]),
  .B16_0_in(HI_B16_0_wr[1]),
  .B16_1_in(HI_B16_1_wr[1]),
  .B16_2_in(HI_B16_2_wr[1]),
  .B16_3_in(HI_B16_3_wr[1]),
  .B8_0_in(HI_B8_0_wr[1]),
  .B8_1_in(HI_B8_1_wr[1]),
  .B8_2_in(HI_B8_2_wr[1]),
  .B8_3_in(HI_B8_3_wr[1]),
  .D48_0_out(PE_D48_0_wr[1]),
  .D48_1_out(PE_D48_1_wr[1]),
  .D48_2_out(PE_D48_2_wr[1]),
  .D48_3_out(PE_D48_3_wr[1]),
  .B16_0_out(PE_B16_0_wr[1]),
  .B16_1_out(PE_B16_1_wr[1]),
  .B16_2_out(PE_B16_2_wr[1]),
  .B16_3_out(PE_B16_3_wr[1]),
  .B8_0_out(PE_B8_0_wr[1]),
  .B8_1_out(PE_B8_1_wr[1]),
  .B8_2_out(PE_B8_2_wr[1]),
  .B8_3_out(PE_B8_3_wr[1])
);

PE
#(
  .UNIT_NO(2),
  .ROW_NO(0),
  .LR_NO(1)
)
 PE2 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[1]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[1]),	
	//-MSB-//   
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[191:128]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw0_wr[191:128]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[191:128]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw0_wr[191:128]),		
  .S48_0_in(HI_D48_0_wr[2]),
  .S48_1_in(HI_D48_1_wr[2]),
  .S48_2_in(HI_D48_2_wr[2]),
  .S48_3_in(HI_D48_3_wr[2]),
  .B16_0_in(HI_B16_0_wr[2]),
  .B16_1_in(HI_B16_1_wr[2]),
  .B16_2_in(HI_B16_2_wr[2]),
  .B16_3_in(HI_B16_3_wr[2]),
  .B8_0_in(HI_B8_0_wr[2]),
  .B8_1_in(HI_B8_1_wr[2]),
  .B8_2_in(HI_B8_2_wr[2]),
  .B8_3_in(HI_B8_3_wr[2]),
  .D48_0_out(PE_D48_0_wr[2]),
  .D48_1_out(PE_D48_1_wr[2]),
  .D48_2_out(PE_D48_2_wr[2]),
  .D48_3_out(PE_D48_3_wr[2]),
  .B16_0_out(PE_B16_0_wr[2]),
  .B16_1_out(PE_B16_1_wr[2]),
  .B16_2_out(PE_B16_2_wr[2]),
  .B16_3_out(PE_B16_3_wr[2]),
  .B8_0_out(PE_B8_0_wr[2]),
  .B8_1_out(PE_B8_1_wr[2]),
  .B8_2_out(PE_B8_2_wr[2]),
  .B8_3_out(PE_B8_3_wr[2])
);

PE
#(
  .UNIT_NO(3),
  .ROW_NO(0),
  .LR_NO(1)
)
 PE3 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[1]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[1]),	
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[255:192]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw0_wr[255:192]),
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[255:192]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw0_wr[255:192]),	
  .S48_0_in(HI_D48_0_wr[3]),
  .S48_1_in(HI_D48_1_wr[3]),
  .S48_2_in(HI_D48_2_wr[3]),
  .S48_3_in(HI_D48_3_wr[3]),
  .B16_0_in(HI_B16_0_wr[3]),
  .B16_1_in(HI_B16_1_wr[3]),
  .B16_2_in(HI_B16_2_wr[3]),
  .B16_3_in(HI_B16_3_wr[3]),
  .B8_0_in(HI_B8_0_wr[3]),
  .B8_1_in(HI_B8_1_wr[3]),
  .B8_2_in(HI_B8_2_wr[3]),
  .B8_3_in(HI_B8_3_wr[3]),
  .D48_0_out(PE_D48_0_wr[3]),
  .D48_1_out(PE_D48_1_wr[3]),
  .D48_2_out(PE_D48_2_wr[3]),
  .D48_3_out(PE_D48_3_wr[3]),
  .B16_0_out(PE_B16_0_wr[3]),
  .B16_1_out(PE_B16_1_wr[3]),
  .B16_2_out(PE_B16_2_wr[3]),
  .B16_3_out(PE_B16_3_wr[3]),
  .B8_0_out(PE_B8_0_wr[3]),
  .B8_1_out(PE_B8_1_wr[3]),
  .B8_2_out(PE_B8_2_wr[3]),
  .B8_3_out(PE_B8_3_wr[3])
);

  //-----------------------------------------------------//
  //          			Row 1	                  		 // 
  //-----------------------------------------------------//
 
Row_Connection_PE0
#(
  .UNIT_NO(4)
)
 RC1_0(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[2]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[2]),
  .PE0_S48_0_in(PE_D48_0_wr[0]),
  .PE0_S48_1_in(PE_D48_1_wr[0]),
  .PE0_S48_2_in(PE_D48_2_wr[0]),
  .PE0_S48_3_in(PE_D48_3_wr[0]),
  .PE0_B16_0_in(PE_B16_0_wr[0]),
  .PE0_B16_1_in(PE_B16_1_wr[0]),
  .PE0_B16_2_in(PE_B16_2_wr[0]),
  .PE0_B16_3_in(PE_B16_3_wr[0]),
  .PE0_B8_0_in(PE_B8_0_wr[0]),
  .PE0_B8_1_in(PE_B8_1_wr[0]),
  .PE0_B8_2_in(PE_B8_2_wr[0]),
  .PE0_B8_3_in(PE_B8_3_wr[0]),
  .PE1_S48_0_in(PE_D48_0_wr[1]),
  .PE1_S48_1_in(PE_D48_1_wr[1]),
  .PE1_S48_2_in(PE_D48_2_wr[1]),
  .PE1_S48_3_in(PE_D48_3_wr[1]),
  .PE1_B16_0_in(PE_B16_0_wr[1]),
  .PE1_B16_1_in(PE_B16_1_wr[1]),
  .PE1_B16_2_in(PE_B16_2_wr[1]),
  .PE1_B16_3_in(PE_B16_3_wr[1]),
  .PE1_B8_0_in(PE_B8_0_wr[1]),
  .PE1_B8_1_in(PE_B8_1_wr[1]),
  .PE1_B8_2_in(PE_B8_2_wr[1]),
  .PE1_B8_3_in(PE_B8_3_wr[1]),
  .PE2_S48_0_in(PE_D48_0_wr[2]),
  .PE2_S48_1_in(PE_D48_1_wr[2]),
  .PE2_S48_2_in(PE_D48_2_wr[2]),
  .PE2_S48_3_in(PE_D48_3_wr[2]),
  .PE2_B16_0_in(PE_B16_0_wr[2]),
  .PE2_B16_1_in(PE_B16_1_wr[2]),
  .PE2_B16_2_in(PE_B16_2_wr[2]),
  .PE2_B16_3_in(PE_B16_3_wr[2]),
  .PE2_B8_0_in(PE_B8_0_wr[2]),
  .PE2_B8_1_in(PE_B8_1_wr[2]),
  .PE2_B8_2_in(PE_B8_2_wr[2]),
  .PE2_B8_3_in(PE_B8_3_wr[2]),
  .PE3_S48_0_in(PE_D48_0_wr[3]),
  .PE3_S48_1_in(PE_D48_1_wr[3]),
  .PE3_S48_2_in(PE_D48_2_wr[3]),
  .PE3_S48_3_in(PE_D48_3_wr[3]),
  .PE3_B16_0_in(PE_B16_0_wr[3]),
  .PE3_B16_1_in(PE_B16_1_wr[3]),
  .PE3_B16_2_in(PE_B16_2_wr[3]),
  .PE3_B16_3_in(PE_B16_3_wr[3]),
  .PE3_B8_0_in(PE_B8_0_wr[3]),
  .PE3_B8_1_in(PE_B8_1_wr[3]),
  .PE3_B8_2_in(PE_B8_2_wr[3]),
  .PE3_B8_3_in(PE_B8_3_wr[3]),
  .D48_0_out(HI_D48_0_wr[4]),
  .D48_1_out(HI_D48_1_wr[4]),
  .D48_2_out(HI_D48_2_wr[4]),
  .D48_3_out(HI_D48_3_wr[4]),
  .B16_0_out(HI_B16_0_wr[4]),
  .B16_1_out(HI_B16_1_wr[4]),
  .B16_2_out(HI_B16_2_wr[4]),
  .B16_3_out(HI_B16_3_wr[4]),
  .B8_0_out(HI_B8_0_wr[4]),
  .B8_1_out(HI_B8_1_wr[4]),
  .B8_2_out(HI_B8_2_wr[4]),
  .B8_3_out(HI_B8_3_wr[4])
);

Row_Connection_PE1
#(
  .UNIT_NO(5)
)
 RC1_1(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[2]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[2]),
  .PE0_S48_0_in(PE_D48_0_wr[0]),
  .PE0_S48_1_in(PE_D48_1_wr[0]),
  .PE0_S48_2_in(PE_D48_2_wr[0]),
  .PE0_S48_3_in(PE_D48_3_wr[0]),
  .PE0_B16_0_in(PE_B16_0_wr[0]),
  .PE0_B16_1_in(PE_B16_1_wr[0]),
  .PE0_B16_2_in(PE_B16_2_wr[0]),
  .PE0_B16_3_in(PE_B16_3_wr[0]),
  .PE0_B8_0_in(PE_B8_0_wr[0]),
  .PE0_B8_1_in(PE_B8_1_wr[0]),
  .PE0_B8_2_in(PE_B8_2_wr[0]),
  .PE0_B8_3_in(PE_B8_3_wr[0]),
  .PE1_S48_0_in(PE_D48_0_wr[1]),
  .PE1_S48_1_in(PE_D48_1_wr[1]),
  .PE1_S48_2_in(PE_D48_2_wr[1]),
  .PE1_S48_3_in(PE_D48_3_wr[1]),
  .PE1_B16_0_in(PE_B16_0_wr[1]),
  .PE1_B16_1_in(PE_B16_1_wr[1]),
  .PE1_B16_2_in(PE_B16_2_wr[1]),
  .PE1_B16_3_in(PE_B16_3_wr[1]),
  .PE1_B8_0_in(PE_B8_0_wr[1]),
  .PE1_B8_1_in(PE_B8_1_wr[1]),
  .PE1_B8_2_in(PE_B8_2_wr[1]),
  .PE1_B8_3_in(PE_B8_3_wr[1]),
  .PE2_S48_0_in(PE_D48_0_wr[2]),
  .PE2_S48_1_in(PE_D48_1_wr[2]),
  .PE2_S48_2_in(PE_D48_2_wr[2]),
  .PE2_S48_3_in(PE_D48_3_wr[2]),
  .PE2_B16_0_in(PE_B16_0_wr[2]),
  .PE2_B16_1_in(PE_B16_1_wr[2]),
  .PE2_B16_2_in(PE_B16_2_wr[2]),
  .PE2_B16_3_in(PE_B16_3_wr[2]),
  .PE2_B8_0_in(PE_B8_0_wr[2]),
  .PE2_B8_1_in(PE_B8_1_wr[2]),
  .PE2_B8_2_in(PE_B8_2_wr[2]),
  .PE2_B8_3_in(PE_B8_3_wr[2]),
  .PE3_S48_0_in(PE_D48_0_wr[3]),
  .PE3_S48_1_in(PE_D48_1_wr[3]),
  .PE3_S48_2_in(PE_D48_2_wr[3]),
  .PE3_S48_3_in(PE_D48_3_wr[3]),
  .PE3_B16_0_in(PE_B16_0_wr[3]),
  .PE3_B16_1_in(PE_B16_1_wr[3]),
  .PE3_B16_2_in(PE_B16_2_wr[3]),
  .PE3_B16_3_in(PE_B16_3_wr[3]),
  .PE3_B8_0_in(PE_B8_0_wr[3]),
  .PE3_B8_1_in(PE_B8_1_wr[3]),
  .PE3_B8_2_in(PE_B8_2_wr[3]),
  .PE3_B8_3_in(PE_B8_3_wr[3]),
  .D48_0_out(HI_D48_0_wr[5]),
  .D48_1_out(HI_D48_1_wr[5]),
  .D48_2_out(HI_D48_2_wr[5]),
  .D48_3_out(HI_D48_3_wr[5]),
  .B16_0_out(HI_B16_0_wr[5]),
  .B16_1_out(HI_B16_1_wr[5]),
  .B16_2_out(HI_B16_2_wr[5]),
  .B16_3_out(HI_B16_3_wr[5]),
  .B8_0_out(HI_B8_0_wr[5]),
  .B8_1_out(HI_B8_1_wr[5]),
  .B8_2_out(HI_B8_2_wr[5]),
  .B8_3_out(HI_B8_3_wr[5])
);

Row_Connection_PE2
#(
  .UNIT_NO(6)
)
 RC1_2(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[2]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[2]),
  .PE0_S48_0_in(PE_D48_0_wr[0]),
  .PE0_S48_1_in(PE_D48_1_wr[0]),
  .PE0_S48_2_in(PE_D48_2_wr[0]),
  .PE0_S48_3_in(PE_D48_3_wr[0]),
  .PE0_B16_0_in(PE_B16_0_wr[0]),
  .PE0_B16_1_in(PE_B16_1_wr[0]),
  .PE0_B16_2_in(PE_B16_2_wr[0]),
  .PE0_B16_3_in(PE_B16_3_wr[0]),
  .PE0_B8_0_in(PE_B8_0_wr[0]),
  .PE0_B8_1_in(PE_B8_1_wr[0]),
  .PE0_B8_2_in(PE_B8_2_wr[0]),
  .PE0_B8_3_in(PE_B8_3_wr[0]),
  .PE1_S48_0_in(PE_D48_0_wr[1]),
  .PE1_S48_1_in(PE_D48_1_wr[1]),
  .PE1_S48_2_in(PE_D48_2_wr[1]),
  .PE1_S48_3_in(PE_D48_3_wr[1]),
  .PE1_B16_0_in(PE_B16_0_wr[1]),
  .PE1_B16_1_in(PE_B16_1_wr[1]),
  .PE1_B16_2_in(PE_B16_2_wr[1]),
  .PE1_B16_3_in(PE_B16_3_wr[1]),
  .PE1_B8_0_in(PE_B8_0_wr[1]),
  .PE1_B8_1_in(PE_B8_1_wr[1]),
  .PE1_B8_2_in(PE_B8_2_wr[1]),
  .PE1_B8_3_in(PE_B8_3_wr[1]),
  .PE2_S48_0_in(PE_D48_0_wr[2]),
  .PE2_S48_1_in(PE_D48_1_wr[2]),
  .PE2_S48_2_in(PE_D48_2_wr[2]),
  .PE2_S48_3_in(PE_D48_3_wr[2]),
  .PE2_B16_0_in(PE_B16_0_wr[2]),
  .PE2_B16_1_in(PE_B16_1_wr[2]),
  .PE2_B16_2_in(PE_B16_2_wr[2]),
  .PE2_B16_3_in(PE_B16_3_wr[2]),
  .PE2_B8_0_in(PE_B8_0_wr[2]),
  .PE2_B8_1_in(PE_B8_1_wr[2]),
  .PE2_B8_2_in(PE_B8_2_wr[2]),
  .PE2_B8_3_in(PE_B8_3_wr[2]),
  .PE3_S48_0_in(PE_D48_0_wr[3]),
  .PE3_S48_1_in(PE_D48_1_wr[3]),
  .PE3_S48_2_in(PE_D48_2_wr[3]),
  .PE3_S48_3_in(PE_D48_3_wr[3]),
  .PE3_B16_0_in(PE_B16_0_wr[3]),
  .PE3_B16_1_in(PE_B16_1_wr[3]),
  .PE3_B16_2_in(PE_B16_2_wr[3]),
  .PE3_B16_3_in(PE_B16_3_wr[3]),
  .PE3_B8_0_in(PE_B8_0_wr[3]),
  .PE3_B8_1_in(PE_B8_1_wr[3]),
  .PE3_B8_2_in(PE_B8_2_wr[3]),
  .PE3_B8_3_in(PE_B8_3_wr[3]),
  .D48_0_out(HI_D48_0_wr[6]),
  .D48_1_out(HI_D48_1_wr[6]),
  .D48_2_out(HI_D48_2_wr[6]),
  .D48_3_out(HI_D48_3_wr[6]),
  .B16_0_out(HI_B16_0_wr[6]),
  .B16_1_out(HI_B16_1_wr[6]),
  .B16_2_out(HI_B16_2_wr[6]),
  .B16_3_out(HI_B16_3_wr[6]),
  .B8_0_out(HI_B8_0_wr[6]),
  .B8_1_out(HI_B8_1_wr[6]),
  .B8_2_out(HI_B8_2_wr[6]),
  .B8_3_out(HI_B8_3_wr[6])
);
 
Row_Connection_PE3
#(
  .UNIT_NO(7)
)
 RC1_3(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[2]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[2]),
  .PE0_S48_0_in(PE_D48_0_wr[0]),
  .PE0_S48_1_in(PE_D48_1_wr[0]),
  .PE0_S48_2_in(PE_D48_2_wr[0]),
  .PE0_S48_3_in(PE_D48_3_wr[0]),
  .PE0_B16_0_in(PE_B16_0_wr[0]),
  .PE0_B16_1_in(PE_B16_1_wr[0]),
  .PE0_B16_2_in(PE_B16_2_wr[0]),
  .PE0_B16_3_in(PE_B16_3_wr[0]),
  .PE0_B8_0_in(PE_B8_0_wr[0]),
  .PE0_B8_1_in(PE_B8_1_wr[0]),
  .PE0_B8_2_in(PE_B8_2_wr[0]),
  .PE0_B8_3_in(PE_B8_3_wr[0]),
  .PE1_S48_0_in(PE_D48_0_wr[1]),
  .PE1_S48_1_in(PE_D48_1_wr[1]),
  .PE1_S48_2_in(PE_D48_2_wr[1]),
  .PE1_S48_3_in(PE_D48_3_wr[1]),
  .PE1_B16_0_in(PE_B16_0_wr[1]),
  .PE1_B16_1_in(PE_B16_1_wr[1]),
  .PE1_B16_2_in(PE_B16_2_wr[1]),
  .PE1_B16_3_in(PE_B16_3_wr[1]),
  .PE1_B8_0_in(PE_B8_0_wr[1]),
  .PE1_B8_1_in(PE_B8_1_wr[1]),
  .PE1_B8_2_in(PE_B8_2_wr[1]),
  .PE1_B8_3_in(PE_B8_3_wr[1]),
  .PE2_S48_0_in(PE_D48_0_wr[2]),
  .PE2_S48_1_in(PE_D48_1_wr[2]),
  .PE2_S48_2_in(PE_D48_2_wr[2]),
  .PE2_S48_3_in(PE_D48_3_wr[2]),
  .PE2_B16_0_in(PE_B16_0_wr[2]),
  .PE2_B16_1_in(PE_B16_1_wr[2]),
  .PE2_B16_2_in(PE_B16_2_wr[2]),
  .PE2_B16_3_in(PE_B16_3_wr[2]),
  .PE2_B8_0_in(PE_B8_0_wr[2]),
  .PE2_B8_1_in(PE_B8_1_wr[2]),
  .PE2_B8_2_in(PE_B8_2_wr[2]),
  .PE2_B8_3_in(PE_B8_3_wr[2]),
  .PE3_S48_0_in(PE_D48_0_wr[3]),
  .PE3_S48_1_in(PE_D48_1_wr[3]),
  .PE3_S48_2_in(PE_D48_2_wr[3]),
  .PE3_S48_3_in(PE_D48_3_wr[3]),
  .PE3_B16_0_in(PE_B16_0_wr[3]),
  .PE3_B16_1_in(PE_B16_1_wr[3]),
  .PE3_B16_2_in(PE_B16_2_wr[3]),
  .PE3_B16_3_in(PE_B16_3_wr[3]),
  .PE3_B8_0_in(PE_B8_0_wr[3]),
  .PE3_B8_1_in(PE_B8_1_wr[3]),
  .PE3_B8_2_in(PE_B8_2_wr[3]),
  .PE3_B8_3_in(PE_B8_3_wr[3]),
  .D48_0_out(HI_D48_0_wr[7]),
  .D48_1_out(HI_D48_1_wr[7]),
  .D48_2_out(HI_D48_2_wr[7]),
  .D48_3_out(HI_D48_3_wr[7]),
  .B16_0_out(HI_B16_0_wr[7]),
  .B16_1_out(HI_B16_1_wr[7]),
  .B16_2_out(HI_B16_2_wr[7]),
  .B16_3_out(HI_B16_3_wr[7]),
  .B8_0_out(HI_B8_0_wr[7]),
  .B8_1_out(HI_B8_1_wr[7]),
  .B8_2_out(HI_B8_2_wr[7]),
  .B8_3_out(HI_B8_3_wr[7])
);

PE
#(
  .UNIT_NO(4),
  .ROW_NO(1),
  .LR_NO(0)
)
 PE4 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[3]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[3]),				
	//-MSB-//  
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[63:0]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw1_wr[63:0]),		
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[63:0]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw1_wr[63:0]),  			
  .S48_0_in(HI_D48_0_wr[4]),
  .S48_1_in(HI_D48_1_wr[4]),
  .S48_2_in(HI_D48_2_wr[4]),
  .S48_3_in(HI_D48_3_wr[4]),
  .B16_0_in(HI_B16_0_wr[4]),
  .B16_1_in(HI_B16_1_wr[4]),
  .B16_2_in(HI_B16_2_wr[4]),
  .B16_3_in(HI_B16_3_wr[4]),
  .B8_0_in(HI_B8_0_wr[4]),
  .B8_1_in(HI_B8_1_wr[4]),
  .B8_2_in(HI_B8_2_wr[4]),
  .B8_3_in(HI_B8_3_wr[4]),
  .D48_0_out(PE_D48_0_wr[4]),
  .D48_1_out(PE_D48_1_wr[4]),
  .D48_2_out(PE_D48_2_wr[4]),
  .D48_3_out(PE_D48_3_wr[4]),
  .B16_0_out(PE_B16_0_wr[4]),
  .B16_1_out(PE_B16_1_wr[4]),
  .B16_2_out(PE_B16_2_wr[4]),
  .B16_3_out(PE_B16_3_wr[4]),
  .B8_0_out(PE_B8_0_wr[4]),
  .B8_1_out(PE_B8_1_wr[4]),
  .B8_2_out(PE_B8_2_wr[4]),
  .B8_3_out(PE_B8_3_wr[4])
);

PE
#(
  .UNIT_NO(5),
  .ROW_NO(1),
  .LR_NO(0)
)
 PE5 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[3]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[3]),				
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[127:64]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw1_wr[127:64]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[127:64]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw1_wr[127:64]),			
  .S48_0_in(HI_D48_0_wr[5]),
  .S48_1_in(HI_D48_1_wr[5]),
  .S48_2_in(HI_D48_2_wr[5]),
  .S48_3_in(HI_D48_3_wr[5]),
  .B16_0_in(HI_B16_0_wr[5]),
  .B16_1_in(HI_B16_1_wr[5]),
  .B16_2_in(HI_B16_2_wr[5]),
  .B16_3_in(HI_B16_3_wr[5]),
  .B8_0_in(HI_B8_0_wr[5]),
  .B8_1_in(HI_B8_1_wr[5]),
  .B8_2_in(HI_B8_2_wr[5]),
  .B8_3_in(HI_B8_3_wr[5]),
  .D48_0_out(PE_D48_0_wr[5]),
  .D48_1_out(PE_D48_1_wr[5]),
  .D48_2_out(PE_D48_2_wr[5]),
  .D48_3_out(PE_D48_3_wr[5]),
  .B16_0_out(PE_B16_0_wr[5]),
  .B16_1_out(PE_B16_1_wr[5]),
  .B16_2_out(PE_B16_2_wr[5]),
  .B16_3_out(PE_B16_3_wr[5]),
  .B8_0_out(PE_B8_0_wr[5]),
  .B8_1_out(PE_B8_1_wr[5]),
  .B8_2_out(PE_B8_2_wr[5]),
  .B8_3_out(PE_B8_3_wr[5])
);

PE
#(
  .UNIT_NO(6),
  .ROW_NO(1),
  .LR_NO(1)
)
 PE6 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[3]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[3]),				
	//-MSB-//   
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[191:128]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw1_wr[191:128]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[191:128]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw1_wr[191:128]),				
  .S48_0_in(HI_D48_0_wr[6]),
  .S48_1_in(HI_D48_1_wr[6]),
  .S48_2_in(HI_D48_2_wr[6]),
  .S48_3_in(HI_D48_3_wr[6]),
  .B16_0_in(HI_B16_0_wr[6]),
  .B16_1_in(HI_B16_1_wr[6]),
  .B16_2_in(HI_B16_2_wr[6]),
  .B16_3_in(HI_B16_3_wr[6]),
  .B8_0_in(HI_B8_0_wr[6]),
  .B8_1_in(HI_B8_1_wr[6]),
  .B8_2_in(HI_B8_2_wr[6]),
  .B8_3_in(HI_B8_3_wr[6]),
  .D48_0_out(PE_D48_0_wr[6]),
  .D48_1_out(PE_D48_1_wr[6]),
  .D48_2_out(PE_D48_2_wr[6]),
  .D48_3_out(PE_D48_3_wr[6]),
  .B16_0_out(PE_B16_0_wr[6]),
  .B16_1_out(PE_B16_1_wr[6]),
  .B16_2_out(PE_B16_2_wr[6]),
  .B16_3_out(PE_B16_3_wr[6]),
  .B8_0_out(PE_B8_0_wr[6]),
  .B8_1_out(PE_B8_1_wr[6]),
  .B8_2_out(PE_B8_2_wr[6]),
  .B8_3_out(PE_B8_3_wr[6])
);

PE
#(
  .UNIT_NO(7),
  .ROW_NO(1),
  .LR_NO(1)
)
 PE7 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[3]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[3]),					
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[255:192]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw1_wr[255:192]),
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[255:192]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw1_wr[255:192]),			
  .S48_0_in(HI_D48_0_wr[7]),
  .S48_1_in(HI_D48_1_wr[7]),
  .S48_2_in(HI_D48_2_wr[7]),
  .S48_3_in(HI_D48_3_wr[7]),
  .B16_0_in(HI_B16_0_wr[7]),
  .B16_1_in(HI_B16_1_wr[7]),
  .B16_2_in(HI_B16_2_wr[7]),
  .B16_3_in(HI_B16_3_wr[7]),
  .B8_0_in(HI_B8_0_wr[7]),
  .B8_1_in(HI_B8_1_wr[7]),
  .B8_2_in(HI_B8_2_wr[7]),
  .B8_3_in(HI_B8_3_wr[7]),
  .D48_0_out(PE_D48_0_wr[7]),
  .D48_1_out(PE_D48_1_wr[7]),
  .D48_2_out(PE_D48_2_wr[7]),
  .D48_3_out(PE_D48_3_wr[7]),
  .B16_0_out(PE_B16_0_wr[7]),
  .B16_1_out(PE_B16_1_wr[7]),
  .B16_2_out(PE_B16_2_wr[7]),
  .B16_3_out(PE_B16_3_wr[7]),
  .B8_0_out(PE_B8_0_wr[7]),
  .B8_1_out(PE_B8_1_wr[7]),
  .B8_2_out(PE_B8_2_wr[7]),
  .B8_3_out(PE_B8_3_wr[7])
);

  //-----------------------------------------------------//
  //          			Row 2	                  		 // 
  //-----------------------------------------------------//   
Row_Connection_PE0
#(
  .UNIT_NO(8)
)
 RC2_0(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[4]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[4]),
  .PE0_S48_0_in(PE_D48_0_wr[4]),
  .PE0_S48_1_in(PE_D48_1_wr[4]),
  .PE0_S48_2_in(PE_D48_2_wr[4]),
  .PE0_S48_3_in(PE_D48_3_wr[4]),
  .PE0_B16_0_in(PE_B16_0_wr[4]),
  .PE0_B16_1_in(PE_B16_1_wr[4]),
  .PE0_B16_2_in(PE_B16_2_wr[4]),
  .PE0_B16_3_in(PE_B16_3_wr[4]),
  .PE0_B8_0_in(PE_B8_0_wr[4]),
  .PE0_B8_1_in(PE_B8_1_wr[4]),
  .PE0_B8_2_in(PE_B8_2_wr[4]),
  .PE0_B8_3_in(PE_B8_3_wr[4]),
  .PE1_S48_0_in(PE_D48_0_wr[5]),
  .PE1_S48_1_in(PE_D48_1_wr[5]),
  .PE1_S48_2_in(PE_D48_2_wr[5]),
  .PE1_S48_3_in(PE_D48_3_wr[5]),
  .PE1_B16_0_in(PE_B16_0_wr[5]),
  .PE1_B16_1_in(PE_B16_1_wr[5]),
  .PE1_B16_2_in(PE_B16_2_wr[5]),
  .PE1_B16_3_in(PE_B16_3_wr[5]),
  .PE1_B8_0_in(PE_B8_0_wr[5]),
  .PE1_B8_1_in(PE_B8_1_wr[5]),
  .PE1_B8_2_in(PE_B8_2_wr[5]),
  .PE1_B8_3_in(PE_B8_3_wr[5]),
  .PE2_S48_0_in(PE_D48_0_wr[6]),
  .PE2_S48_1_in(PE_D48_1_wr[6]),
  .PE2_S48_2_in(PE_D48_2_wr[6]),
  .PE2_S48_3_in(PE_D48_3_wr[6]),
  .PE2_B16_0_in(PE_B16_0_wr[6]),
  .PE2_B16_1_in(PE_B16_1_wr[6]),
  .PE2_B16_2_in(PE_B16_2_wr[6]),
  .PE2_B16_3_in(PE_B16_3_wr[6]),
  .PE2_B8_0_in(PE_B8_0_wr[6]),
  .PE2_B8_1_in(PE_B8_1_wr[6]),
  .PE2_B8_2_in(PE_B8_2_wr[6]),
  .PE2_B8_3_in(PE_B8_3_wr[6]),
  .PE3_S48_0_in(PE_D48_0_wr[7]),
  .PE3_S48_1_in(PE_D48_1_wr[7]),
  .PE3_S48_2_in(PE_D48_2_wr[7]),
  .PE3_S48_3_in(PE_D48_3_wr[7]),
  .PE3_B16_0_in(PE_B16_0_wr[7]),
  .PE3_B16_1_in(PE_B16_1_wr[7]),
  .PE3_B16_2_in(PE_B16_2_wr[7]),
  .PE3_B16_3_in(PE_B16_3_wr[7]),
  .PE3_B8_0_in(PE_B8_0_wr[7]),
  .PE3_B8_1_in(PE_B8_1_wr[7]),
  .PE3_B8_2_in(PE_B8_2_wr[7]),
  .PE3_B8_3_in(PE_B8_3_wr[7]),
  .D48_0_out(HI_D48_0_wr[8]),
  .D48_1_out(HI_D48_1_wr[8]),
  .D48_2_out(HI_D48_2_wr[8]),
  .D48_3_out(HI_D48_3_wr[8]),
  .B16_0_out(HI_B16_0_wr[8]),
  .B16_1_out(HI_B16_1_wr[8]),
  .B16_2_out(HI_B16_2_wr[8]),
  .B16_3_out(HI_B16_3_wr[8]),
  .B8_0_out(HI_B8_0_wr[8]),
  .B8_1_out(HI_B8_1_wr[8]),
  .B8_2_out(HI_B8_2_wr[8]),
  .B8_3_out(HI_B8_3_wr[8])
);

Row_Connection_PE1
#(
  .UNIT_NO(9)
)
 RC2_1(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[4]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[4]),
  .PE0_S48_0_in(PE_D48_0_wr[4]),
  .PE0_S48_1_in(PE_D48_1_wr[4]),
  .PE0_S48_2_in(PE_D48_2_wr[4]),
  .PE0_S48_3_in(PE_D48_3_wr[4]),
  .PE0_B16_0_in(PE_B16_0_wr[4]),
  .PE0_B16_1_in(PE_B16_1_wr[4]),
  .PE0_B16_2_in(PE_B16_2_wr[4]),
  .PE0_B16_3_in(PE_B16_3_wr[4]),
  .PE0_B8_0_in(PE_B8_0_wr[4]),
  .PE0_B8_1_in(PE_B8_1_wr[4]),
  .PE0_B8_2_in(PE_B8_2_wr[4]),
  .PE0_B8_3_in(PE_B8_3_wr[4]),
  .PE1_S48_0_in(PE_D48_0_wr[5]),
  .PE1_S48_1_in(PE_D48_1_wr[5]),
  .PE1_S48_2_in(PE_D48_2_wr[5]),
  .PE1_S48_3_in(PE_D48_3_wr[5]),
  .PE1_B16_0_in(PE_B16_0_wr[5]),
  .PE1_B16_1_in(PE_B16_1_wr[5]),
  .PE1_B16_2_in(PE_B16_2_wr[5]),
  .PE1_B16_3_in(PE_B16_3_wr[5]),
  .PE1_B8_0_in(PE_B8_0_wr[5]),
  .PE1_B8_1_in(PE_B8_1_wr[5]),
  .PE1_B8_2_in(PE_B8_2_wr[5]),
  .PE1_B8_3_in(PE_B8_3_wr[5]),
  .PE2_S48_0_in(PE_D48_0_wr[6]),
  .PE2_S48_1_in(PE_D48_1_wr[6]),
  .PE2_S48_2_in(PE_D48_2_wr[6]),
  .PE2_S48_3_in(PE_D48_3_wr[6]),
  .PE2_B16_0_in(PE_B16_0_wr[6]),
  .PE2_B16_1_in(PE_B16_1_wr[6]),
  .PE2_B16_2_in(PE_B16_2_wr[6]),
  .PE2_B16_3_in(PE_B16_3_wr[6]),
  .PE2_B8_0_in(PE_B8_0_wr[6]),
  .PE2_B8_1_in(PE_B8_1_wr[6]),
  .PE2_B8_2_in(PE_B8_2_wr[6]),
  .PE2_B8_3_in(PE_B8_3_wr[6]),
  .PE3_S48_0_in(PE_D48_0_wr[7]),
  .PE3_S48_1_in(PE_D48_1_wr[7]),
  .PE3_S48_2_in(PE_D48_2_wr[7]),
  .PE3_S48_3_in(PE_D48_3_wr[7]),
  .PE3_B16_0_in(PE_B16_0_wr[7]),
  .PE3_B16_1_in(PE_B16_1_wr[7]),
  .PE3_B16_2_in(PE_B16_2_wr[7]),
  .PE3_B16_3_in(PE_B16_3_wr[7]),
  .PE3_B8_0_in(PE_B8_0_wr[7]),
  .PE3_B8_1_in(PE_B8_1_wr[7]),
  .PE3_B8_2_in(PE_B8_2_wr[7]),
  .PE3_B8_3_in(PE_B8_3_wr[7]),
  .D48_0_out(HI_D48_0_wr[9]),
  .D48_1_out(HI_D48_1_wr[9]),
  .D48_2_out(HI_D48_2_wr[9]),
  .D48_3_out(HI_D48_3_wr[9]),
  .B16_0_out(HI_B16_0_wr[9]),
  .B16_1_out(HI_B16_1_wr[9]),
  .B16_2_out(HI_B16_2_wr[9]),
  .B16_3_out(HI_B16_3_wr[9]),
  .B8_0_out(HI_B8_0_wr[9]),
  .B8_1_out(HI_B8_1_wr[9]),
  .B8_2_out(HI_B8_2_wr[9]),
  .B8_3_out(HI_B8_3_wr[9])
);

Row_Connection_PE2
#(
  .UNIT_NO(10)
)
 RC2_2(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[4]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[4]),
  .PE0_S48_0_in(PE_D48_0_wr[4]),
  .PE0_S48_1_in(PE_D48_1_wr[4]),
  .PE0_S48_2_in(PE_D48_2_wr[4]),
  .PE0_S48_3_in(PE_D48_3_wr[4]),
  .PE0_B16_0_in(PE_B16_0_wr[4]),
  .PE0_B16_1_in(PE_B16_1_wr[4]),
  .PE0_B16_2_in(PE_B16_2_wr[4]),
  .PE0_B16_3_in(PE_B16_3_wr[4]),
  .PE0_B8_0_in(PE_B8_0_wr[4]),
  .PE0_B8_1_in(PE_B8_1_wr[4]),
  .PE0_B8_2_in(PE_B8_2_wr[4]),
  .PE0_B8_3_in(PE_B8_3_wr[4]),
  .PE1_S48_0_in(PE_D48_0_wr[5]),
  .PE1_S48_1_in(PE_D48_1_wr[5]),
  .PE1_S48_2_in(PE_D48_2_wr[5]),
  .PE1_S48_3_in(PE_D48_3_wr[5]),
  .PE1_B16_0_in(PE_B16_0_wr[5]),
  .PE1_B16_1_in(PE_B16_1_wr[5]),
  .PE1_B16_2_in(PE_B16_2_wr[5]),
  .PE1_B16_3_in(PE_B16_3_wr[5]),
  .PE1_B8_0_in(PE_B8_0_wr[5]),
  .PE1_B8_1_in(PE_B8_1_wr[5]),
  .PE1_B8_2_in(PE_B8_2_wr[5]),
  .PE1_B8_3_in(PE_B8_3_wr[5]),
  .PE2_S48_0_in(PE_D48_0_wr[6]),
  .PE2_S48_1_in(PE_D48_1_wr[6]),
  .PE2_S48_2_in(PE_D48_2_wr[6]),
  .PE2_S48_3_in(PE_D48_3_wr[6]),
  .PE2_B16_0_in(PE_B16_0_wr[6]),
  .PE2_B16_1_in(PE_B16_1_wr[6]),
  .PE2_B16_2_in(PE_B16_2_wr[6]),
  .PE2_B16_3_in(PE_B16_3_wr[6]),
  .PE2_B8_0_in(PE_B8_0_wr[6]),
  .PE2_B8_1_in(PE_B8_1_wr[6]),
  .PE2_B8_2_in(PE_B8_2_wr[6]),
  .PE2_B8_3_in(PE_B8_3_wr[6]),
  .PE3_S48_0_in(PE_D48_0_wr[7]),
  .PE3_S48_1_in(PE_D48_1_wr[7]),
  .PE3_S48_2_in(PE_D48_2_wr[7]),
  .PE3_S48_3_in(PE_D48_3_wr[7]),
  .PE3_B16_0_in(PE_B16_0_wr[7]),
  .PE3_B16_1_in(PE_B16_1_wr[7]),
  .PE3_B16_2_in(PE_B16_2_wr[7]),
  .PE3_B16_3_in(PE_B16_3_wr[7]),
  .PE3_B8_0_in(PE_B8_0_wr[7]),
  .PE3_B8_1_in(PE_B8_1_wr[7]),
  .PE3_B8_2_in(PE_B8_2_wr[7]),
  .PE3_B8_3_in(PE_B8_3_wr[7]),
  .D48_0_out(HI_D48_0_wr[10]),
  .D48_1_out(HI_D48_1_wr[10]),
  .D48_2_out(HI_D48_2_wr[10]),
  .D48_3_out(HI_D48_3_wr[10]),
  .B16_0_out(HI_B16_0_wr[10]),
  .B16_1_out(HI_B16_1_wr[10]),
  .B16_2_out(HI_B16_2_wr[10]),
  .B16_3_out(HI_B16_3_wr[10]),
  .B8_0_out(HI_B8_0_wr[10]),
  .B8_1_out(HI_B8_1_wr[10]),
  .B8_2_out(HI_B8_2_wr[10]),
  .B8_3_out(HI_B8_3_wr[10])
);

Row_Connection_PE3
#(
  .UNIT_NO(11)
)
 RC2_3(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[4]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[4]),
  .PE0_S48_0_in(PE_D48_0_wr[4]),
  .PE0_S48_1_in(PE_D48_1_wr[4]),
  .PE0_S48_2_in(PE_D48_2_wr[4]),
  .PE0_S48_3_in(PE_D48_3_wr[4]),
  .PE0_B16_0_in(PE_B16_0_wr[4]),
  .PE0_B16_1_in(PE_B16_1_wr[4]),
  .PE0_B16_2_in(PE_B16_2_wr[4]),
  .PE0_B16_3_in(PE_B16_3_wr[4]),
  .PE0_B8_0_in(PE_B8_0_wr[4]),
  .PE0_B8_1_in(PE_B8_1_wr[4]),
  .PE0_B8_2_in(PE_B8_2_wr[4]),
  .PE0_B8_3_in(PE_B8_3_wr[4]),
  .PE1_S48_0_in(PE_D48_0_wr[5]),
  .PE1_S48_1_in(PE_D48_1_wr[5]),
  .PE1_S48_2_in(PE_D48_2_wr[5]),
  .PE1_S48_3_in(PE_D48_3_wr[5]),
  .PE1_B16_0_in(PE_B16_0_wr[5]),
  .PE1_B16_1_in(PE_B16_1_wr[5]),
  .PE1_B16_2_in(PE_B16_2_wr[5]),
  .PE1_B16_3_in(PE_B16_3_wr[5]),
  .PE1_B8_0_in(PE_B8_0_wr[5]),
  .PE1_B8_1_in(PE_B8_1_wr[5]),
  .PE1_B8_2_in(PE_B8_2_wr[5]),
  .PE1_B8_3_in(PE_B8_3_wr[5]),
  .PE2_S48_0_in(PE_D48_0_wr[6]),
  .PE2_S48_1_in(PE_D48_1_wr[6]),
  .PE2_S48_2_in(PE_D48_2_wr[6]),
  .PE2_S48_3_in(PE_D48_3_wr[6]),
  .PE2_B16_0_in(PE_B16_0_wr[6]),
  .PE2_B16_1_in(PE_B16_1_wr[6]),
  .PE2_B16_2_in(PE_B16_2_wr[6]),
  .PE2_B16_3_in(PE_B16_3_wr[6]),
  .PE2_B8_0_in(PE_B8_0_wr[6]),
  .PE2_B8_1_in(PE_B8_1_wr[6]),
  .PE2_B8_2_in(PE_B8_2_wr[6]),
  .PE2_B8_3_in(PE_B8_3_wr[6]),
  .PE3_S48_0_in(PE_D48_0_wr[7]),
  .PE3_S48_1_in(PE_D48_1_wr[7]),
  .PE3_S48_2_in(PE_D48_2_wr[7]),
  .PE3_S48_3_in(PE_D48_3_wr[7]),
  .PE3_B16_0_in(PE_B16_0_wr[7]),
  .PE3_B16_1_in(PE_B16_1_wr[7]),
  .PE3_B16_2_in(PE_B16_2_wr[7]),
  .PE3_B16_3_in(PE_B16_3_wr[7]),
  .PE3_B8_0_in(PE_B8_0_wr[7]),
  .PE3_B8_1_in(PE_B8_1_wr[7]),
  .PE3_B8_2_in(PE_B8_2_wr[7]),
  .PE3_B8_3_in(PE_B8_3_wr[7]),
  .D48_0_out(HI_D48_0_wr[11]),
  .D48_1_out(HI_D48_1_wr[11]),
  .D48_2_out(HI_D48_2_wr[11]),
  .D48_3_out(HI_D48_3_wr[11]),
  .B16_0_out(HI_B16_0_wr[11]),
  .B16_1_out(HI_B16_1_wr[11]),
  .B16_2_out(HI_B16_2_wr[11]),
  .B16_3_out(HI_B16_3_wr[11]),
  .B8_0_out(HI_B8_0_wr[11]),
  .B8_1_out(HI_B8_1_wr[11]),
  .B8_2_out(HI_B8_2_wr[11]),
  .B8_3_out(HI_B8_3_wr[11])
);

PE
#(
  .UNIT_NO(8),
  .ROW_NO(2),
  .LR_NO(0)
)
 PE8 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[5]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[5]),				
	//-MSB-//  
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[63:0]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw2_wr[63:0]),		
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[63:0]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw2_wr[63:0]),  			
  .S48_0_in(HI_D48_0_wr[8]),
  .S48_1_in(HI_D48_1_wr[8]),
  .S48_2_in(HI_D48_2_wr[8]),
  .S48_3_in(HI_D48_3_wr[8]),
  .B16_0_in(HI_B16_0_wr[8]),
  .B16_1_in(HI_B16_1_wr[8]),
  .B16_2_in(HI_B16_2_wr[8]),
  .B16_3_in(HI_B16_3_wr[8]),
  .B8_0_in(HI_B8_0_wr[8]),
  .B8_1_in(HI_B8_1_wr[8]),
  .B8_2_in(HI_B8_2_wr[8]),
  .B8_3_in(HI_B8_3_wr[8]),
  .D48_0_out(PE_D48_0_wr[8]),
  .D48_1_out(PE_D48_1_wr[8]),
  .D48_2_out(PE_D48_2_wr[8]),
  .D48_3_out(PE_D48_3_wr[8]),
  .B16_0_out(PE_B16_0_wr[8]),
  .B16_1_out(PE_B16_1_wr[8]),
  .B16_2_out(PE_B16_2_wr[8]),
  .B16_3_out(PE_B16_3_wr[8]),
  .B8_0_out(PE_B8_0_wr[8]),
  .B8_1_out(PE_B8_1_wr[8]),
  .B8_2_out(PE_B8_2_wr[8]),
  .B8_3_out(PE_B8_3_wr[8])
);

PE
#(
  .UNIT_NO(9),
  .ROW_NO(2),
  .LR_NO(0)
)
 PE9 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[5]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[5]),				
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[127:64]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw2_wr[127:64]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[127:64]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw2_wr[127:64]),			
  .S48_0_in(HI_D48_0_wr[9]),
  .S48_1_in(HI_D48_1_wr[9]),
  .S48_2_in(HI_D48_2_wr[9]),
  .S48_3_in(HI_D48_3_wr[9]),
  .B16_0_in(HI_B16_0_wr[9]),
  .B16_1_in(HI_B16_1_wr[9]),
  .B16_2_in(HI_B16_2_wr[9]),
  .B16_3_in(HI_B16_3_wr[9]),
  .B8_0_in(HI_B8_0_wr[9]),
  .B8_1_in(HI_B8_1_wr[9]),
  .B8_2_in(HI_B8_2_wr[9]),
  .B8_3_in(HI_B8_3_wr[9]),
  .D48_0_out(PE_D48_0_wr[9]),
  .D48_1_out(PE_D48_1_wr[9]),
  .D48_2_out(PE_D48_2_wr[9]),
  .D48_3_out(PE_D48_3_wr[9]),
  .B16_0_out(PE_B16_0_wr[9]),
  .B16_1_out(PE_B16_1_wr[9]),
  .B16_2_out(PE_B16_2_wr[9]),
  .B16_3_out(PE_B16_3_wr[9]),
  .B8_0_out(PE_B8_0_wr[9]),
  .B8_1_out(PE_B8_1_wr[9]),
  .B8_2_out(PE_B8_2_wr[9]),
  .B8_3_out(PE_B8_3_wr[9])
);

PE
#(
  .UNIT_NO(10),
  .ROW_NO(2),
  .LR_NO(1)
)
 PE10 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[5]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[5]),				
	//-MSB-//   
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[191:128]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw2_wr[191:128]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[191:128]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw2_wr[191:128]),					
  .S48_0_in(HI_D48_0_wr[10]),
  .S48_1_in(HI_D48_1_wr[10]),
  .S48_2_in(HI_D48_2_wr[10]),
  .S48_3_in(HI_D48_3_wr[10]),
  .B16_0_in(HI_B16_0_wr[10]),
  .B16_1_in(HI_B16_1_wr[10]),
  .B16_2_in(HI_B16_2_wr[10]),
  .B16_3_in(HI_B16_3_wr[10]),
  .B8_0_in(HI_B8_0_wr[10]),
  .B8_1_in(HI_B8_1_wr[10]),
  .B8_2_in(HI_B8_2_wr[10]),
  .B8_3_in(HI_B8_3_wr[10]),
  .D48_0_out(PE_D48_0_wr[10]),
  .D48_1_out(PE_D48_1_wr[10]),
  .D48_2_out(PE_D48_2_wr[10]),
  .D48_3_out(PE_D48_3_wr[10]),
  .B16_0_out(PE_B16_0_wr[10]),
  .B16_1_out(PE_B16_1_wr[10]),
  .B16_2_out(PE_B16_2_wr[10]),
  .B16_3_out(PE_B16_3_wr[10]),
  .B8_0_out(PE_B8_0_wr[10]),
  .B8_1_out(PE_B8_1_wr[10]),
  .B8_2_out(PE_B8_2_wr[10]),
  .B8_3_out(PE_B8_3_wr[10])
);

PE
#(
  .UNIT_NO(11),
  .ROW_NO(2),
  .LR_NO(1)
)
 PE11 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[5]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[5]),				
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[255:192]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw2_wr[255:192]),
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[255:192]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw2_wr[255:192]),			
  .S48_0_in(HI_D48_0_wr[11]),
  .S48_1_in(HI_D48_1_wr[11]),
  .S48_2_in(HI_D48_2_wr[11]),
  .S48_3_in(HI_D48_3_wr[11]),
  .B16_0_in(HI_B16_0_wr[11]),
  .B16_1_in(HI_B16_1_wr[11]),
  .B16_2_in(HI_B16_2_wr[11]),
  .B16_3_in(HI_B16_3_wr[11]),
  .B8_0_in(HI_B8_0_wr[11]),
  .B8_1_in(HI_B8_1_wr[11]),
  .B8_2_in(HI_B8_2_wr[11]),
  .B8_3_in(HI_B8_3_wr[11]),
  .D48_0_out(PE_D48_0_wr[11]),
  .D48_1_out(PE_D48_1_wr[11]),
  .D48_2_out(PE_D48_2_wr[11]),
  .D48_3_out(PE_D48_3_wr[11]),
  .B16_0_out(PE_B16_0_wr[11]),
  .B16_1_out(PE_B16_1_wr[11]),
  .B16_2_out(PE_B16_2_wr[11]),
  .B16_3_out(PE_B16_3_wr[11]),
  .B8_0_out(PE_B8_0_wr[11]),
  .B8_1_out(PE_B8_1_wr[11]),
  .B8_2_out(PE_B8_2_wr[11]),
  .B8_3_out(PE_B8_3_wr[11])
);

  //-----------------------------------------------------//
  //          			Row 2	                  		 // 
  //-----------------------------------------------------//  

Row_Connection_PE0
#(
  .UNIT_NO(12)
)
 RC3_0(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[6]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[6]),
  .PE0_S48_0_in(PE_D48_0_wr[8]),
  .PE0_S48_1_in(PE_D48_1_wr[8]),
  .PE0_S48_2_in(PE_D48_2_wr[8]),
  .PE0_S48_3_in(PE_D48_3_wr[8]),
  .PE0_B16_0_in(PE_B16_0_wr[8]),
  .PE0_B16_1_in(PE_B16_1_wr[8]),
  .PE0_B16_2_in(PE_B16_2_wr[8]),
  .PE0_B16_3_in(PE_B16_3_wr[8]),
  .PE0_B8_0_in(PE_B8_0_wr[8]),
  .PE0_B8_1_in(PE_B8_1_wr[8]),
  .PE0_B8_2_in(PE_B8_2_wr[8]),
  .PE0_B8_3_in(PE_B8_3_wr[8]),
  .PE1_S48_0_in(PE_D48_0_wr[9]),
  .PE1_S48_1_in(PE_D48_1_wr[9]),
  .PE1_S48_2_in(PE_D48_2_wr[9]),
  .PE1_S48_3_in(PE_D48_3_wr[9]),
  .PE1_B16_0_in(PE_B16_0_wr[9]),
  .PE1_B16_1_in(PE_B16_1_wr[9]),
  .PE1_B16_2_in(PE_B16_2_wr[9]),
  .PE1_B16_3_in(PE_B16_3_wr[9]),
  .PE1_B8_0_in(PE_B8_0_wr[9]),
  .PE1_B8_1_in(PE_B8_1_wr[9]),
  .PE1_B8_2_in(PE_B8_2_wr[9]),
  .PE1_B8_3_in(PE_B8_3_wr[9]),
  .PE2_S48_0_in(PE_D48_0_wr[10]),
  .PE2_S48_1_in(PE_D48_1_wr[10]),
  .PE2_S48_2_in(PE_D48_2_wr[10]),
  .PE2_S48_3_in(PE_D48_3_wr[10]),
  .PE2_B16_0_in(PE_B16_0_wr[10]),
  .PE2_B16_1_in(PE_B16_1_wr[10]),
  .PE2_B16_2_in(PE_B16_2_wr[10]),
  .PE2_B16_3_in(PE_B16_3_wr[10]),
  .PE2_B8_0_in(PE_B8_0_wr[10]),
  .PE2_B8_1_in(PE_B8_1_wr[10]),
  .PE2_B8_2_in(PE_B8_2_wr[10]),
  .PE2_B8_3_in(PE_B8_3_wr[10]),
  .PE3_S48_0_in(PE_D48_0_wr[11]),
  .PE3_S48_1_in(PE_D48_1_wr[11]),
  .PE3_S48_2_in(PE_D48_2_wr[11]),
  .PE3_S48_3_in(PE_D48_3_wr[11]),
  .PE3_B16_0_in(PE_B16_0_wr[11]),
  .PE3_B16_1_in(PE_B16_1_wr[11]),
  .PE3_B16_2_in(PE_B16_2_wr[11]),
  .PE3_B16_3_in(PE_B16_3_wr[11]),
  .PE3_B8_0_in(PE_B8_0_wr[11]),
  .PE3_B8_1_in(PE_B8_1_wr[11]),
  .PE3_B8_2_in(PE_B8_2_wr[11]),
  .PE3_B8_3_in(PE_B8_3_wr[11]),
  .D48_0_out(HI_D48_0_wr[12]),
  .D48_1_out(HI_D48_1_wr[12]),
  .D48_2_out(HI_D48_2_wr[12]),
  .D48_3_out(HI_D48_3_wr[12]),
  .B16_0_out(HI_B16_0_wr[12]),
  .B16_1_out(HI_B16_1_wr[12]),
  .B16_2_out(HI_B16_2_wr[12]),
  .B16_3_out(HI_B16_3_wr[12]),
  .B8_0_out(HI_B8_0_wr[12]),
  .B8_1_out(HI_B8_1_wr[12]),
  .B8_2_out(HI_B8_2_wr[12]),
  .B8_3_out(HI_B8_3_wr[12])
);

Row_Connection_PE1
#(
  .UNIT_NO(13)
)
 RC3_1(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[6]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[6]),
  .PE0_S48_0_in(PE_D48_0_wr[8]),
  .PE0_S48_1_in(PE_D48_1_wr[8]),
  .PE0_S48_2_in(PE_D48_2_wr[8]),
  .PE0_S48_3_in(PE_D48_3_wr[8]),
  .PE0_B16_0_in(PE_B16_0_wr[8]),
  .PE0_B16_1_in(PE_B16_1_wr[8]),
  .PE0_B16_2_in(PE_B16_2_wr[8]),
  .PE0_B16_3_in(PE_B16_3_wr[8]),
  .PE0_B8_0_in(PE_B8_0_wr[8]),
  .PE0_B8_1_in(PE_B8_1_wr[8]),
  .PE0_B8_2_in(PE_B8_2_wr[8]),
  .PE0_B8_3_in(PE_B8_3_wr[8]),
  .PE1_S48_0_in(PE_D48_0_wr[9]),
  .PE1_S48_1_in(PE_D48_1_wr[9]),
  .PE1_S48_2_in(PE_D48_2_wr[9]),
  .PE1_S48_3_in(PE_D48_3_wr[9]),
  .PE1_B16_0_in(PE_B16_0_wr[9]),
  .PE1_B16_1_in(PE_B16_1_wr[9]),
  .PE1_B16_2_in(PE_B16_2_wr[9]),
  .PE1_B16_3_in(PE_B16_3_wr[9]),
  .PE1_B8_0_in(PE_B8_0_wr[9]),
  .PE1_B8_1_in(PE_B8_1_wr[9]),
  .PE1_B8_2_in(PE_B8_2_wr[9]),
  .PE1_B8_3_in(PE_B8_3_wr[9]),
  .PE2_S48_0_in(PE_D48_0_wr[10]),
  .PE2_S48_1_in(PE_D48_1_wr[10]),
  .PE2_S48_2_in(PE_D48_2_wr[10]),
  .PE2_S48_3_in(PE_D48_3_wr[10]),
  .PE2_B16_0_in(PE_B16_0_wr[10]),
  .PE2_B16_1_in(PE_B16_1_wr[10]),
  .PE2_B16_2_in(PE_B16_2_wr[10]),
  .PE2_B16_3_in(PE_B16_3_wr[10]),
  .PE2_B8_0_in(PE_B8_0_wr[10]),
  .PE2_B8_1_in(PE_B8_1_wr[10]),
  .PE2_B8_2_in(PE_B8_2_wr[10]),
  .PE2_B8_3_in(PE_B8_3_wr[10]),
  .PE3_S48_0_in(PE_D48_0_wr[11]),
  .PE3_S48_1_in(PE_D48_1_wr[11]),
  .PE3_S48_2_in(PE_D48_2_wr[11]),
  .PE3_S48_3_in(PE_D48_3_wr[11]),
  .PE3_B16_0_in(PE_B16_0_wr[11]),
  .PE3_B16_1_in(PE_B16_1_wr[11]),
  .PE3_B16_2_in(PE_B16_2_wr[11]),
  .PE3_B16_3_in(PE_B16_3_wr[11]),
  .PE3_B8_0_in(PE_B8_0_wr[11]),
  .PE3_B8_1_in(PE_B8_1_wr[11]),
  .PE3_B8_2_in(PE_B8_2_wr[11]),
  .PE3_B8_3_in(PE_B8_3_wr[11]),
  .D48_0_out(HI_D48_0_wr[13]),
  .D48_1_out(HI_D48_1_wr[13]),
  .D48_2_out(HI_D48_2_wr[13]),
  .D48_3_out(HI_D48_3_wr[13]),
  .B16_0_out(HI_B16_0_wr[13]),
  .B16_1_out(HI_B16_1_wr[13]),
  .B16_2_out(HI_B16_2_wr[13]),
  .B16_3_out(HI_B16_3_wr[13]),
  .B8_0_out(HI_B8_0_wr[13]),
  .B8_1_out(HI_B8_1_wr[13]),
  .B8_2_out(HI_B8_2_wr[13]),
  .B8_3_out(HI_B8_3_wr[13])
);

Row_Connection_PE2
#(
  .UNIT_NO(14)
)
 RC3_2(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[6]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[6]),
  .PE0_S48_0_in(PE_D48_0_wr[8]),
  .PE0_S48_1_in(PE_D48_1_wr[8]),
  .PE0_S48_2_in(PE_D48_2_wr[8]),
  .PE0_S48_3_in(PE_D48_3_wr[8]),
  .PE0_B16_0_in(PE_B16_0_wr[8]),
  .PE0_B16_1_in(PE_B16_1_wr[8]),
  .PE0_B16_2_in(PE_B16_2_wr[8]),
  .PE0_B16_3_in(PE_B16_3_wr[8]),
  .PE0_B8_0_in(PE_B8_0_wr[8]),
  .PE0_B8_1_in(PE_B8_1_wr[8]),
  .PE0_B8_2_in(PE_B8_2_wr[8]),
  .PE0_B8_3_in(PE_B8_3_wr[8]),
  .PE1_S48_0_in(PE_D48_0_wr[9]),
  .PE1_S48_1_in(PE_D48_1_wr[9]),
  .PE1_S48_2_in(PE_D48_2_wr[9]),
  .PE1_S48_3_in(PE_D48_3_wr[9]),
  .PE1_B16_0_in(PE_B16_0_wr[9]),
  .PE1_B16_1_in(PE_B16_1_wr[9]),
  .PE1_B16_2_in(PE_B16_2_wr[9]),
  .PE1_B16_3_in(PE_B16_3_wr[9]),
  .PE1_B8_0_in(PE_B8_0_wr[9]),
  .PE1_B8_1_in(PE_B8_1_wr[9]),
  .PE1_B8_2_in(PE_B8_2_wr[9]),
  .PE1_B8_3_in(PE_B8_3_wr[9]),
  .PE2_S48_0_in(PE_D48_0_wr[10]),
  .PE2_S48_1_in(PE_D48_1_wr[10]),
  .PE2_S48_2_in(PE_D48_2_wr[10]),
  .PE2_S48_3_in(PE_D48_3_wr[10]),
  .PE2_B16_0_in(PE_B16_0_wr[10]),
  .PE2_B16_1_in(PE_B16_1_wr[10]),
  .PE2_B16_2_in(PE_B16_2_wr[10]),
  .PE2_B16_3_in(PE_B16_3_wr[10]),
  .PE2_B8_0_in(PE_B8_0_wr[10]),
  .PE2_B8_1_in(PE_B8_1_wr[10]),
  .PE2_B8_2_in(PE_B8_2_wr[10]),
  .PE2_B8_3_in(PE_B8_3_wr[10]),
  .PE3_S48_0_in(PE_D48_0_wr[11]),
  .PE3_S48_1_in(PE_D48_1_wr[11]),
  .PE3_S48_2_in(PE_D48_2_wr[11]),
  .PE3_S48_3_in(PE_D48_3_wr[11]),
  .PE3_B16_0_in(PE_B16_0_wr[11]),
  .PE3_B16_1_in(PE_B16_1_wr[11]),
  .PE3_B16_2_in(PE_B16_2_wr[11]),
  .PE3_B16_3_in(PE_B16_3_wr[11]),
  .PE3_B8_0_in(PE_B8_0_wr[11]),
  .PE3_B8_1_in(PE_B8_1_wr[11]),
  .PE3_B8_2_in(PE_B8_2_wr[11]),
  .PE3_B8_3_in(PE_B8_3_wr[11]),
  .D48_0_out(HI_D48_0_wr[14]),
  .D48_1_out(HI_D48_1_wr[14]),
  .D48_2_out(HI_D48_2_wr[14]),
  .D48_3_out(HI_D48_3_wr[14]),
  .B16_0_out(HI_B16_0_wr[14]),
  .B16_1_out(HI_B16_1_wr[14]),
  .B16_2_out(HI_B16_2_wr[14]),
  .B16_3_out(HI_B16_3_wr[14]),
  .B8_0_out(HI_B8_0_wr[14]),
  .B8_1_out(HI_B8_1_wr[14]),
  .B8_2_out(HI_B8_2_wr[14]),
  .B8_3_out(HI_B8_3_wr[14])
);

Row_Connection_PE3
#(
  .UNIT_NO(15)
)
 RC3_3(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[6]),
  .CTX_RC_addra_in(CTX_RC_addra_in),
  .CTX_RC_dina_in(CTX_RC_dina_in),
  .CTX_RC_ena_in(CTX_RC_ena_in),
  .CTX_RC_wea_in(CTX_RC_wea_in),
  .CTX_incr_in(CTX_incr_in[6]),
  .PE0_S48_0_in(PE_D48_0_wr[8]),
  .PE0_S48_1_in(PE_D48_1_wr[8]),
  .PE0_S48_2_in(PE_D48_2_wr[8]),
  .PE0_S48_3_in(PE_D48_3_wr[8]),
  .PE0_B16_0_in(PE_B16_0_wr[8]),
  .PE0_B16_1_in(PE_B16_1_wr[8]),
  .PE0_B16_2_in(PE_B16_2_wr[8]),
  .PE0_B16_3_in(PE_B16_3_wr[8]),
  .PE0_B8_0_in(PE_B8_0_wr[8]),
  .PE0_B8_1_in(PE_B8_1_wr[8]),
  .PE0_B8_2_in(PE_B8_2_wr[8]),
  .PE0_B8_3_in(PE_B8_3_wr[8]),
  .PE1_S48_0_in(PE_D48_0_wr[9]),
  .PE1_S48_1_in(PE_D48_1_wr[9]),
  .PE1_S48_2_in(PE_D48_2_wr[9]),
  .PE1_S48_3_in(PE_D48_3_wr[9]),
  .PE1_B16_0_in(PE_B16_0_wr[9]),
  .PE1_B16_1_in(PE_B16_1_wr[9]),
  .PE1_B16_2_in(PE_B16_2_wr[9]),
  .PE1_B16_3_in(PE_B16_3_wr[9]),
  .PE1_B8_0_in(PE_B8_0_wr[9]),
  .PE1_B8_1_in(PE_B8_1_wr[9]),
  .PE1_B8_2_in(PE_B8_2_wr[9]),
  .PE1_B8_3_in(PE_B8_3_wr[9]),
  .PE2_S48_0_in(PE_D48_0_wr[10]),
  .PE2_S48_1_in(PE_D48_1_wr[10]),
  .PE2_S48_2_in(PE_D48_2_wr[10]),
  .PE2_S48_3_in(PE_D48_3_wr[10]),
  .PE2_B16_0_in(PE_B16_0_wr[10]),
  .PE2_B16_1_in(PE_B16_1_wr[10]),
  .PE2_B16_2_in(PE_B16_2_wr[10]),
  .PE2_B16_3_in(PE_B16_3_wr[10]),
  .PE2_B8_0_in(PE_B8_0_wr[10]),
  .PE2_B8_1_in(PE_B8_1_wr[10]),
  .PE2_B8_2_in(PE_B8_2_wr[10]),
  .PE2_B8_3_in(PE_B8_3_wr[10]),
  .PE3_S48_0_in(PE_D48_0_wr[11]),
  .PE3_S48_1_in(PE_D48_1_wr[11]),
  .PE3_S48_2_in(PE_D48_2_wr[11]),
  .PE3_S48_3_in(PE_D48_3_wr[11]),
  .PE3_B16_0_in(PE_B16_0_wr[11]),
  .PE3_B16_1_in(PE_B16_1_wr[11]),
  .PE3_B16_2_in(PE_B16_2_wr[11]),
  .PE3_B16_3_in(PE_B16_3_wr[11]),
  .PE3_B8_0_in(PE_B8_0_wr[11]),
  .PE3_B8_1_in(PE_B8_1_wr[11]),
  .PE3_B8_2_in(PE_B8_2_wr[11]),
  .PE3_B8_3_in(PE_B8_3_wr[11]),
  .D48_0_out(HI_D48_0_wr[15]),
  .D48_1_out(HI_D48_1_wr[15]),
  .D48_2_out(HI_D48_2_wr[15]),
  .D48_3_out(HI_D48_3_wr[15]),
  .B16_0_out(HI_B16_0_wr[15]),
  .B16_1_out(HI_B16_1_wr[15]),
  .B16_2_out(HI_B16_2_wr[15]),
  .B16_3_out(HI_B16_3_wr[15]),
  .B8_0_out(HI_B8_0_wr[15]),
  .B8_1_out(HI_B8_1_wr[15]),
  .B8_2_out(HI_B8_2_wr[15]),
  .B8_3_out(HI_B8_3_wr[15])
);

PE
#(
  .UNIT_NO(12),
  .ROW_NO(3),
  .LR_NO(0)
)
 PE12 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[7]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[7]),				
	//-MSB-//  
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[63:0]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw3_wr[63:0]),		
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[63:0]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw3_wr[63:0]),  			
  .S48_0_in(HI_D48_0_wr[12]),
  .S48_1_in(HI_D48_1_wr[12]),
  .S48_2_in(HI_D48_2_wr[12]),
  .S48_3_in(HI_D48_3_wr[12]),
  .B16_0_in(HI_B16_0_wr[12]),
  .B16_1_in(HI_B16_1_wr[12]),
  .B16_2_in(HI_B16_2_wr[12]),
  .B16_3_in(HI_B16_3_wr[12]),
  .B8_0_in(HI_B8_0_wr[12]),
  .B8_1_in(HI_B8_1_wr[12]),
  .B8_2_in(HI_B8_2_wr[12]),
  .B8_3_in(HI_B8_3_wr[12]),
  .D48_0_out(PE_D48_0_wr[12]),
  .D48_1_out(PE_D48_1_wr[12]),
  .D48_2_out(PE_D48_2_wr[12]),
  .D48_3_out(PE_D48_3_wr[12]),
  .B16_0_out(PE_B16_0_wr[12]),
  .B16_1_out(PE_B16_1_wr[12]),
  .B16_2_out(PE_B16_2_wr[12]),
  .B16_3_out(PE_B16_3_wr[12]),
  .B8_0_out(PE_B8_0_wr[12]),
  .B8_1_out(PE_B8_1_wr[12]),
  .B8_2_out(PE_B8_2_wr[12]),
  .B8_3_out(PE_B8_3_wr[12])
);


PE
#(
  .UNIT_NO(13),
  .ROW_NO(3),
  .LR_NO(0)
)
 PE13 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[7]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),	
  .CTX_incr_in(CTX_incr_in[7]),					
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[127:64]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw3_wr[127:64]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[127:64]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw3_wr[127:64]),				
  .S48_0_in(HI_D48_0_wr[13]),
  .S48_1_in(HI_D48_1_wr[13]),
  .S48_2_in(HI_D48_2_wr[13]),
  .S48_3_in(HI_D48_3_wr[13]),
  .B16_0_in(HI_B16_0_wr[13]),
  .B16_1_in(HI_B16_1_wr[13]),
  .B16_2_in(HI_B16_2_wr[13]),
  .B16_3_in(HI_B16_3_wr[13]),
  .B8_0_in(HI_B8_0_wr[13]),
  .B8_1_in(HI_B8_1_wr[13]),
  .B8_2_in(HI_B8_2_wr[13]),
  .B8_3_in(HI_B8_3_wr[13]),
  .D48_0_out(PE_D48_0_wr[13]),
  .D48_1_out(PE_D48_1_wr[13]),
  .D48_2_out(PE_D48_2_wr[13]),
  .D48_3_out(PE_D48_3_wr[13]),
  .B16_0_out(PE_B16_0_wr[13]),
  .B16_1_out(PE_B16_1_wr[13]),
  .B16_2_out(PE_B16_2_wr[13]),
  .B16_3_out(PE_B16_3_wr[13]),
  .B8_0_out(PE_B8_0_wr[13]),
  .B8_1_out(PE_B8_1_wr[13]),
  .B8_2_out(PE_B8_2_wr[13]),
  .B8_3_out(PE_B8_3_wr[13])
);

PE
#(
  .UNIT_NO(14),
  .ROW_NO(3),
  .LR_NO(1)
)
 PE14 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[7]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[7]),				
	//-MSB-//   
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[191:128]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw3_wr[191:128]),	
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[191:128]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw3_wr[191:128]),				
  .S48_0_in(HI_D48_0_wr[14]),
  .S48_1_in(HI_D48_1_wr[14]),
  .S48_2_in(HI_D48_2_wr[14]),
  .S48_3_in(HI_D48_3_wr[14]),
  .B16_0_in(HI_B16_0_wr[14]),
  .B16_1_in(HI_B16_1_wr[14]),
  .B16_2_in(HI_B16_2_wr[14]),
  .B16_3_in(HI_B16_3_wr[14]),
  .B8_0_in(HI_B8_0_wr[14]),
  .B8_1_in(HI_B8_1_wr[14]),
  .B8_2_in(HI_B8_2_wr[14]),
  .B8_3_in(HI_B8_3_wr[14]),
  .D48_0_out(PE_D48_0_wr[14]),
  .D48_1_out(PE_D48_1_wr[14]),
  .D48_2_out(PE_D48_2_wr[14]),
  .D48_3_out(PE_D48_3_wr[14]),
  .B16_0_out(PE_B16_0_wr[14]),
  .B16_1_out(PE_B16_1_wr[14]),
  .B16_2_out(PE_B16_2_wr[14]),
  .B16_3_out(PE_B16_3_wr[14]),
  .B8_0_out(PE_B8_0_wr[14]),
  .B8_1_out(PE_B8_1_wr[14]),
  .B8_2_out(PE_B8_2_wr[14]),
  .B8_3_out(PE_B8_3_wr[14])
);

PE
#(
  .UNIT_NO(15),
  .ROW_NO(3),
  .LR_NO(1)
)
 PE15 (
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in[7]),
  .Mode_in(Mode_in),
  .CTX_PE_addra_in(CTX_PE_addra_in),
  .CTX_PE_dina_in(CTX_PE_dina_in),
  .CTX_PE_ena_in(CTX_PE_ena_in),
  .CTX_PE_wea_in(CTX_PE_wea_in),
  .CTX_IM_addra_in(CTX_IM_addra_in),
  .CTX_IM_dina_in(CTX_IM_dina_in),
  .CTX_IM_ena_in(CTX_IM_ena_in),
  .CTX_IM_wea_in(CTX_IM_wea_in),
  .CTX_incr_in(CTX_incr_in[7]),				
	//-MSB-//    
  .LDM_addra_in(LDM_addra_in),
  .LDM_MSB_dina_in(LDM_MSB_dina_wr[255:192]),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_MSB_douta_out(LDM_MSB_douta_rw3_wr[255:192]),
	//-LSB-//
  .LDM_LSB_dina_in(LDM_LSB_dina_wr[255:192]),
  .LDM_LSB_douta_out(LDM_LSB_douta_rw3_wr[255:192]),				
  .S48_0_in(HI_D48_0_wr[15]),
  .S48_1_in(HI_D48_1_wr[15]),
  .S48_2_in(HI_D48_2_wr[15]),
  .S48_3_in(HI_D48_3_wr[15]),
  .B16_0_in(HI_B16_0_wr[15]),
  .B16_1_in(HI_B16_1_wr[15]),
  .B16_2_in(HI_B16_2_wr[15]),
  .B16_3_in(HI_B16_3_wr[15]),
  .B8_0_in(HI_B8_0_wr[15]),
  .B8_1_in(HI_B8_1_wr[15]),
  .B8_2_in(HI_B8_2_wr[15]),
  .B8_3_in(HI_B8_3_wr[15]),
  .D48_0_out(PE_D48_0_wr[15]),
  .D48_1_out(PE_D48_1_wr[15]),
  .D48_2_out(PE_D48_2_wr[15]),
  .D48_3_out(PE_D48_3_wr[15]),
  .B16_0_out(PE_B16_0_wr[15]),
  .B16_1_out(PE_B16_1_wr[15]),
  .B16_2_out(PE_B16_2_wr[15]),
  .B16_3_out(PE_B16_3_wr[15]),
  .B8_0_out(PE_B8_0_wr[15]),
  .B8_1_out(PE_B8_1_wr[15]),
  .B8_2_out(PE_B8_2_wr[15]),
  .B8_3_out(PE_B8_3_wr[15])
);

endmodule