/*
 *-----------------------------------------------------------------------------
 * Title         : Row Connection
 * Project       : U2CA
 *-----------------------------------------------------------------------------
 * File          : Row Connection_PE2.v
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

module Row_Connection_PE2
#(
  parameter                                   UNIT_NO    = 0
)
(
	input  wire                                 				CLK,
	input  wire                                 				RST,
	
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	input  wire 					              				start_in,
	
	///*** Context Memory ***///
	input  wire [`PE_NUM_BITS+`CTX_RC_ADDR_BITS-1:0]          	CTX_RC_addra_in,
	input  wire [`CTX_RC_BITS-1:0]              				CTX_RC_dina_in,
	input  wire 					              				CTX_RC_ena_in,
	input  wire 					              				CTX_RC_wea_in,
					
	input  wire 					              				CTX_incr_in,
	
	////==================== PE 0 ====================////
	
	///*** ALU Input ***///
	input  wire [`DWORD_BITS-1:0]                				PE0_S48_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_S48_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_S48_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_S48_3_in,
	///*** Buffer 16to1 Input ***///				
	input  wire [`DWORD_BITS-1:0]                				PE0_B16_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B16_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B16_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B16_3_in,
	///*** Buffer 8to1 Input ***///				
	input  wire [`DWORD_BITS-1:0]                				PE0_B8_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B8_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B8_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE0_B8_3_in,
	
	////==================== PE 1 ====================////
	
	///*** ALU Input ***///
	input  wire [`DWORD_BITS-1:0]                				PE1_S48_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_S48_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_S48_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_S48_3_in,
	///*** Buffer 16to1 Input ***///            				  
	input  wire [`DWORD_BITS-1:0]                				PE1_B16_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B16_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B16_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B16_3_in,
	///*** Buffer 8to1 Input ***///             				  
	input  wire [`DWORD_BITS-1:0]                				PE1_B8_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B8_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B8_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE1_B8_3_in,
	
	////==================== PE 2 ====================////
	
	///*** ALU Input ***///
	input  wire [`DWORD_BITS-1:0]                				PE2_S48_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_S48_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_S48_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_S48_3_in,
	///*** Buffer 16to1 Input ***///            				  
	input  wire [`DWORD_BITS-1:0]                				PE2_B16_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B16_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B16_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B16_3_in,
	///*** Buffer 8to1 Input ***///             				  
	input  wire [`DWORD_BITS-1:0]                				PE2_B8_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B8_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B8_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE2_B8_3_in,
	
	////==================== PE 3 ====================////
	
	///*** ALU Input ***///
	input  wire [`DWORD_BITS-1:0]                				PE3_S48_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_S48_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_S48_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_S48_3_in,
	///*** Buffer 16to1 Input ***///            				  
	input  wire [`DWORD_BITS-1:0]                				PE3_B16_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B16_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B16_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B16_3_in,
	///*** Buffer 8to1 Input ***///             				  
	input  wire [`DWORD_BITS-1:0]                				PE3_B8_0_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B8_1_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B8_2_in,
	input  wire [`DWORD_BITS-1:0]                				PE3_B8_3_in,
	
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------//  
	
	///*** ALU Output ***///
	output reg  [`DWORD_BITS-1:0]           	  				D48_0_out,
	output reg  [`DWORD_BITS-1:0]           	  				D48_1_out,
	output reg  [`DWORD_BITS-1:0]           	  				D48_2_out,
	output reg  [`DWORD_BITS-1:0]           	  				D48_3_out,
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
 
	/// ALU wire
	wire  [`DWORD_BITS-1:0]           	  						D48_0_wr;
	wire  [`DWORD_BITS-1:0]           	  						D48_1_wr;
	wire  [`DWORD_BITS-1:0]           	  						D48_2_wr;
	wire  [`DWORD_BITS-1:0]           	  						D48_3_wr;
							
	/// Buffer 16to1 wire						
	wire  [`DWORD_BITS-1:0]           	  						B16_0_wr;
	wire  [`DWORD_BITS-1:0]           	  						B16_1_wr;
	wire  [`DWORD_BITS-1:0]           	  						B16_2_wr;
	wire  [`DWORD_BITS-1:0]           	  						B16_3_wr;
	/// Buffer 8to1 wire						
	wire  [`DWORD_BITS-1:0]           	  						B8_0_wr;
	wire  [`DWORD_BITS-1:0]           	  						B8_1_wr;
	wire  [`DWORD_BITS-1:0]           	  						B8_2_wr;
	wire  [`DWORD_BITS-1:0]           	  						B8_3_wr;
						
	/// Context memory					
						
	reg  						          						CTX_enb_rg;
	reg  						          						CTX_web_rg;
	reg  [`CTX_RC_ADDR_BITS-1:0]          						CTX_addrb_rg;
	wire [`CTX_RC_BITS-1:0]          	  						CTX_doutb_wr;
							
	reg  [`CTX_RC_ADDR_BITS-1:0]	      						CTX_maxaddra_rg;
						
	wire [2:0]             			  							CTX_B8_0_wr;
	wire [2:0]             			  							CTX_B8_1_wr;
	wire [2:0]             			  							CTX_B8_2_wr;
	wire [2:0]             			  							CTX_B8_3_wr;
								
	wire [3:0]             			  							CTX_B16_0_wr;
	wire [3:0]             			  							CTX_B16_1_wr;
	wire [3:0]             			  							CTX_B16_2_wr;
	wire [3:0]             			  							CTX_B16_3_wr;
								
	wire [5:0]             			  							CTX_D48_0_wr;
	wire [5:0]             			  							CTX_D48_1_wr;
	wire [5:0]             			  							CTX_D48_2_wr;
	wire [5:0]             			  							CTX_D48_3_wr;

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
	
	localparam  												IDLE = 0;
	localparam  												EXEC = 1; 
	
	reg  [0:0]           	  			  						STATE_rg;
	wire								  						CTX_RC_ena_wr;
 
	assign  CTX_RC_ena_wr = (CTX_RC_addra_in[`PE_NUM_BITS +`CTX_RC_ADDR_BITS-1:`CTX_RC_ADDR_BITS] == UNIT_NO) ? CTX_RC_ena_in: 1'b0; 
 
	`ifdef REG_BRAM
	CTX_RAM # (.DWIDTH(`CTX_RC_BITS), .AWIDTH(`CTX_RC_ADDR_BITS))
	ctx_ram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_RC_ena_wr), // port A read enable
		.wea(CTX_RC_wea_in), // port A write enable
		.addra(CTX_RC_addra_in[`CTX_RC_ADDR_BITS-1:0]), // port A address
		.dina(CTX_RC_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_RC_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_doutb_wr) // port A data output
		);
	`endif

	`ifdef ZYNQ_BRAM
	CTX_BRAM ctx_ram (
		.clka(CLK), // clock
		///*** Port A***///
		.ena(CTX_RC_ena_wr), // port A read enable
		.wea(CTX_RC_wea_in), // port A write enable
		.addra(CTX_RC_addra_in[`CTX_RC_ADDR_BITS-1:0]), // port A address
		.dina(CTX_RC_dina_in), // port A data
		.douta(), // port A data output
		
		.clkb(CLK), // clock
		///*** Port B***///
		.enb(CTX_enb_rg), // port A read enable
		.web(CTX_web_rg), // port A write enable
		.addrb(CTX_addrb_rg[`CTX_RC_ADDR_BITS-1:0]), // port A address
		.dinb(), // port A data
		.doutb(CTX_doutb_wr) // port A data output
	);
	`endif
 
	assign CTX_B8_3_wr 	= CTX_doutb_wr[2:0];
	assign CTX_B8_2_wr 	= CTX_doutb_wr[5:3];
	assign CTX_B8_1_wr 	= CTX_doutb_wr[8:6];
	assign CTX_B8_0_wr 	= CTX_doutb_wr[11:9];
	
	assign CTX_B16_3_wr = CTX_doutb_wr[15:12];
	assign CTX_B16_2_wr = CTX_doutb_wr[19:16];
	assign CTX_B16_1_wr = CTX_doutb_wr[23:20];
	assign CTX_B16_0_wr = CTX_doutb_wr[27:24];
	
	assign CTX_D48_3_wr = CTX_doutb_wr[33:28];
	assign CTX_D48_2_wr = CTX_doutb_wr[39:34];
	assign CTX_D48_1_wr = CTX_doutb_wr[45:40];
	assign CTX_D48_0_wr = CTX_doutb_wr[51:46];
 
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
			CTX_addrb_rg  	<= `CTX_RC_ADDR_BITS'h0;
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
			case (STATE_rg)
				IDLE: begin
					CTX_addrb_rg  	<= `CTX_RC_ADDR_BITS'h0;
					finish_lg1_rg	<= 1'b0;
				end
				EXEC: begin
					CTX_addrb_rg  	<= CTX_addrb_rg + CTX_incr_in; 
					if(CTX_addrb_rg == CTX_maxaddra_rg) begin
					finish_lg1_rg	<= 1'b1;
					end
				end
				default: begin
					CTX_addrb_rg  	<= `CTX_RC_ADDR_BITS'h0;
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
			CTX_maxaddra_rg	<= `CTX_RC_ADDR_BITS'h0;
		end
		else begin
			if(CTX_RC_ena_in & CTX_RC_wea_in) begin
				CTX_maxaddra_rg	<= CTX_RC_addra_in[`CTX_RC_ADDR_BITS-1:0];
			end        
		end
	end
 
	MUX_48_1 mux_D48_0(
		.data0_in(PE0_S48_0_in),
		.data1_in(PE0_S48_1_in),
		.data2_in(PE0_S48_2_in),
		.data3_in(PE0_S48_3_in),
		.data4_in(PE0_B16_0_in),
		.data5_in(PE0_B16_1_in),
		.data6_in(PE0_B16_2_in),
		.data7_in(PE0_B16_3_in),
		.data8_in(PE0_B8_0_in),
		.data9_in(PE0_B8_1_in),
		.data10_in(PE0_B8_2_in),
		.data11_in(PE0_B8_3_in),
		.data12_in(PE1_S48_0_in),
		.data13_in(PE1_S48_1_in),
		.data14_in(PE1_S48_2_in),
		.data15_in(PE1_S48_3_in),
		.data16_in(PE1_B16_0_in),
		.data17_in(PE1_B16_1_in),
		.data18_in(PE1_B16_2_in),
		.data19_in(PE1_B16_3_in),
		.data20_in(PE1_B8_0_in),
		.data21_in(PE1_B8_1_in),
		.data22_in(PE1_B8_2_in),
		.data23_in(PE1_B8_3_in),
		.data24_in(PE2_S48_0_in),
		.data25_in(PE2_S48_1_in),
		.data26_in(PE2_S48_2_in),
		.data27_in(PE2_S48_3_in),
		.data28_in(PE2_B16_0_in),
		.data29_in(PE2_B16_1_in),
		.data30_in(PE2_B16_2_in),
		.data31_in(PE2_B16_3_in),
		.data32_in(PE2_B8_0_in),
		.data33_in(PE2_B8_1_in),
		.data34_in(PE2_B8_2_in),
		.data35_in(PE2_B8_3_in),
		.data36_in(PE3_S48_0_in),
		.data37_in(PE3_S48_1_in),
		.data38_in(PE3_S48_2_in),
		.data39_in(PE3_S48_3_in),
		.data40_in(PE3_B16_0_in),
		.data41_in(PE3_B16_1_in),
		.data42_in(PE3_B16_2_in),
		.data43_in(PE3_B16_3_in),
		.data44_in(PE3_B8_0_in),
		.data45_in(PE3_B8_1_in),
		.data46_in(PE3_B8_2_in),
		.data47_in(PE3_B8_3_in),   
		.sel_in(CTX_D48_0_wr),
		.mux_48_1_out(D48_0_wr)
	);

	MUX_48_1 mux_D48_1(
		.data0_in(PE0_S48_0_in),
		.data1_in(PE0_S48_1_in),
		.data2_in(PE0_S48_2_in),
		.data3_in(PE0_S48_3_in),
		.data4_in(PE0_B16_0_in),
		.data5_in(PE0_B16_1_in),
		.data6_in(PE0_B16_2_in),
		.data7_in(PE0_B16_3_in),
		.data8_in(PE0_B8_0_in),
		.data9_in(PE0_B8_1_in),
		.data10_in(PE0_B8_2_in),
		.data11_in(PE0_B8_3_in),
		.data12_in(PE1_S48_0_in),
		.data13_in(PE1_S48_1_in),
		.data14_in(PE1_S48_2_in),
		.data15_in(PE1_S48_3_in),
		.data16_in(PE1_B16_0_in),
		.data17_in(PE1_B16_1_in),
		.data18_in(PE1_B16_2_in),
		.data19_in(PE1_B16_3_in),
		.data20_in(PE1_B8_0_in),
		.data21_in(PE1_B8_1_in),
		.data22_in(PE1_B8_2_in),
		.data23_in(PE1_B8_3_in),
		.data24_in(PE2_S48_0_in),
		.data25_in(PE2_S48_1_in),
		.data26_in(PE2_S48_2_in),
		.data27_in(PE2_S48_3_in),
		.data28_in(PE2_B16_0_in),
		.data29_in(PE2_B16_1_in),
		.data30_in(PE2_B16_2_in),
		.data31_in(PE2_B16_3_in),
		.data32_in(PE2_B8_0_in),
		.data33_in(PE2_B8_1_in),
		.data34_in(PE2_B8_2_in),
		.data35_in(PE2_B8_3_in),
		.data36_in(PE3_S48_0_in),
		.data37_in(PE3_S48_1_in),
		.data38_in(PE3_S48_2_in),
		.data39_in(PE3_S48_3_in),
		.data40_in(PE3_B16_0_in),
		.data41_in(PE3_B16_1_in),
		.data42_in(PE3_B16_2_in),
		.data43_in(PE3_B16_3_in),
		.data44_in(PE3_B8_0_in),
		.data45_in(PE3_B8_1_in),
		.data46_in(PE3_B8_2_in),
		.data47_in(PE3_B8_3_in),   
		.sel_in(CTX_D48_1_wr),
		.mux_48_1_out(D48_1_wr)
	);

	MUX_48_1 mux_D48_2(
		.data0_in(PE0_S48_0_in),
		.data1_in(PE0_S48_1_in),
		.data2_in(PE0_S48_2_in),
		.data3_in(PE0_S48_3_in),
		.data4_in(PE0_B16_0_in),
		.data5_in(PE0_B16_1_in),
		.data6_in(PE0_B16_2_in),
		.data7_in(PE0_B16_3_in),
		.data8_in(PE0_B8_0_in),
		.data9_in(PE0_B8_1_in),
		.data10_in(PE0_B8_2_in),
		.data11_in(PE0_B8_3_in),
		.data12_in(PE1_S48_0_in),
		.data13_in(PE1_S48_1_in),
		.data14_in(PE1_S48_2_in),
		.data15_in(PE1_S48_3_in),
		.data16_in(PE1_B16_0_in),
		.data17_in(PE1_B16_1_in),
		.data18_in(PE1_B16_2_in),
		.data19_in(PE1_B16_3_in),
		.data20_in(PE1_B8_0_in),
		.data21_in(PE1_B8_1_in),
		.data22_in(PE1_B8_2_in),
		.data23_in(PE1_B8_3_in),
		.data24_in(PE2_S48_0_in),
		.data25_in(PE2_S48_1_in),
		.data26_in(PE2_S48_2_in),
		.data27_in(PE2_S48_3_in),
		.data28_in(PE2_B16_0_in),
		.data29_in(PE2_B16_1_in),
		.data30_in(PE2_B16_2_in),
		.data31_in(PE2_B16_3_in),
		.data32_in(PE2_B8_0_in),
		.data33_in(PE2_B8_1_in),
		.data34_in(PE2_B8_2_in),
		.data35_in(PE2_B8_3_in),
		.data36_in(PE3_S48_0_in),
		.data37_in(PE3_S48_1_in),
		.data38_in(PE3_S48_2_in),
		.data39_in(PE3_S48_3_in),
		.data40_in(PE3_B16_0_in),
		.data41_in(PE3_B16_1_in),
		.data42_in(PE3_B16_2_in),
		.data43_in(PE3_B16_3_in),
		.data44_in(PE3_B8_0_in),
		.data45_in(PE3_B8_1_in),
		.data46_in(PE3_B8_2_in),
		.data47_in(PE3_B8_3_in),   
		.sel_in(CTX_D48_2_wr),
		.mux_48_1_out(D48_2_wr)
	);

	MUX_48_1 mux_D48_3(
		.data0_in(PE0_S48_0_in),
		.data1_in(PE0_S48_1_in),
		.data2_in(PE0_S48_2_in),
		.data3_in(PE0_S48_3_in),
		.data4_in(PE0_B16_0_in),
		.data5_in(PE0_B16_1_in),
		.data6_in(PE0_B16_2_in),
		.data7_in(PE0_B16_3_in),
		.data8_in(PE0_B8_0_in),
		.data9_in(PE0_B8_1_in),
		.data10_in(PE0_B8_2_in),
		.data11_in(PE0_B8_3_in),
		.data12_in(PE1_S48_0_in),
		.data13_in(PE1_S48_1_in),
		.data14_in(PE1_S48_2_in),
		.data15_in(PE1_S48_3_in),
		.data16_in(PE1_B16_0_in),
		.data17_in(PE1_B16_1_in),
		.data18_in(PE1_B16_2_in),
		.data19_in(PE1_B16_3_in),
		.data20_in(PE1_B8_0_in),
		.data21_in(PE1_B8_1_in),
		.data22_in(PE1_B8_2_in),
		.data23_in(PE1_B8_3_in),
		.data24_in(PE2_S48_0_in),
		.data25_in(PE2_S48_1_in),
		.data26_in(PE2_S48_2_in),
		.data27_in(PE2_S48_3_in),
		.data28_in(PE2_B16_0_in),
		.data29_in(PE2_B16_1_in),
		.data30_in(PE2_B16_2_in),
		.data31_in(PE2_B16_3_in),
		.data32_in(PE2_B8_0_in),
		.data33_in(PE2_B8_1_in),
		.data34_in(PE2_B8_2_in),
		.data35_in(PE2_B8_3_in),
		.data36_in(PE3_S48_0_in),
		.data37_in(PE3_S48_1_in),
		.data38_in(PE3_S48_2_in),
		.data39_in(PE3_S48_3_in),
		.data40_in(PE3_B16_0_in),
		.data41_in(PE3_B16_1_in),
		.data42_in(PE3_B16_2_in),
		.data43_in(PE3_B16_3_in),
		.data44_in(PE3_B8_0_in),
		.data45_in(PE3_B8_1_in),
		.data46_in(PE3_B8_2_in),
		.data47_in(PE3_B8_3_in),   
		.sel_in(CTX_D48_3_wr),
		.mux_48_1_out(D48_3_wr)
	);

	MUX_16_1 mux_B16_0(
		.data0_in(PE2_S48_0_in),
		.data1_in(PE2_S48_1_in),
		.data2_in(PE2_S48_2_in),
		.data3_in(PE2_S48_3_in),
		.data4_in(PE2_B16_0_in),
		.data5_in(PE2_B16_1_in),
		.data6_in(PE2_B16_2_in),
		.data7_in(PE2_B16_3_in),
		.data8_in(PE2_B8_0_in),
		.data9_in(PE2_B8_1_in),
		.data10_in(PE2_B8_2_in),
		.data11_in(PE2_B8_3_in),
		.data12_in(PE3_S48_0_in),
		.data13_in(PE3_S48_1_in),
		.data14_in(PE3_S48_2_in),
		.data15_in(PE3_S48_3_in),
		.sel_in(CTX_B16_0_wr),
		.mux_16_1_out(B16_0_wr)
	); 
 
	MUX_16_1 mux_B16_1(
		.data0_in(PE2_S48_0_in),
		.data1_in(PE2_S48_1_in),
		.data2_in(PE2_S48_2_in),
		.data3_in(PE2_S48_3_in),
		.data4_in(PE2_B16_0_in),
		.data5_in(PE2_B16_1_in),
		.data6_in(PE2_B16_2_in),
		.data7_in(PE2_B16_3_in),
		.data8_in(PE2_B8_0_in),
		.data9_in(PE2_B8_1_in),
		.data10_in(PE2_B8_2_in),
		.data11_in(PE2_B8_3_in),
		.data12_in(PE3_S48_0_in),
		.data13_in(PE3_S48_1_in),
		.data14_in(PE3_S48_2_in),
		.data15_in(PE3_S48_3_in),
		.sel_in(CTX_B16_1_wr),
		.mux_16_1_out(B16_1_wr)
	); 

	MUX_16_1 mux_B16_2(
		.data0_in(PE2_S48_0_in),
		.data1_in(PE2_S48_1_in),
		.data2_in(PE2_S48_2_in),
		.data3_in(PE2_S48_3_in),
		.data4_in(PE2_B16_0_in),
		.data5_in(PE2_B16_1_in),
		.data6_in(PE2_B16_2_in),
		.data7_in(PE2_B16_3_in),
		.data8_in(PE2_B8_0_in),
		.data9_in(PE2_B8_1_in),
		.data10_in(PE2_B8_2_in),
		.data11_in(PE2_B8_3_in),
		.data12_in(PE3_S48_0_in),
		.data13_in(PE3_S48_1_in),
		.data14_in(PE3_S48_2_in),
		.data15_in(PE3_S48_3_in),
		.sel_in(CTX_B16_2_wr),
		.mux_16_1_out(B16_2_wr)
	); 

	MUX_16_1 mux_B16_3(
		.data0_in(PE2_S48_0_in),
		.data1_in(PE2_S48_1_in),
		.data2_in(PE2_S48_2_in),
		.data3_in(PE2_S48_3_in),
		.data4_in(PE2_B16_0_in),
		.data5_in(PE2_B16_1_in),
		.data6_in(PE2_B16_2_in),
		.data7_in(PE2_B16_3_in),
		.data8_in (PE2_B8_0_in),
		.data9_in (PE2_B8_1_in),
		.data10_in(PE2_B8_2_in),
		.data11_in(PE2_B8_3_in),
		.data12_in(PE3_S48_0_in),
		.data13_in(PE3_S48_1_in),
		.data14_in(PE3_S48_2_in),
		.data15_in(PE3_S48_3_in),
		.sel_in(CTX_B16_3_wr),
		.mux_16_1_out(B16_3_wr)
	); 
 
	MUX_8_1 mux_B8_0(
		.data0_in(PE2_B16_0_in),
		.data1_in(PE2_B16_1_in),
		.data2_in(PE2_B16_2_in),
		.data3_in(PE2_B16_3_in),
		.data4_in(PE2_B8_0_in),
		.data5_in(PE2_B8_1_in),
		.data6_in(PE2_B8_2_in),
		.data7_in(PE2_B8_3_in),
		.sel_in(CTX_B8_0_wr),
		.mux_8_1_out(B8_0_wr)
	); 

	MUX_8_1 mux_B8_1(
		.data0_in(PE2_B16_0_in),
		.data1_in(PE2_B16_1_in),
		.data2_in(PE2_B16_2_in),
		.data3_in(PE2_B16_3_in),
		.data4_in(PE2_B8_0_in),
		.data5_in(PE2_B8_1_in),
		.data6_in(PE2_B8_2_in),
		.data7_in(PE2_B8_3_in),
		.sel_in(CTX_B8_1_wr),
		.mux_8_1_out(B8_1_wr)
	); 

	MUX_8_1 mux_B8_2(
		.data0_in(PE2_B16_0_in),
		.data1_in(PE2_B16_1_in),
		.data2_in(PE2_B16_2_in),
		.data3_in(PE2_B16_3_in),
		.data4_in(PE2_B8_0_in),
		.data5_in(PE2_B8_1_in),
		.data6_in(PE2_B8_2_in),
		.data7_in(PE2_B8_3_in),
		.sel_in(CTX_B8_2_wr),
		.mux_8_1_out(B8_2_wr)
	);  
 
	MUX_8_1 mux_B8_3(
		.data0_in(PE2_B16_0_in),
		.data1_in(PE2_B16_1_in),
		.data2_in(PE2_B16_2_in),
		.data3_in(PE2_B16_3_in),
		.data4_in(PE2_B8_0_in),
		.data5_in(PE2_B8_1_in),
		.data6_in(PE2_B8_2_in),
		.data7_in(PE2_B8_3_in),
		.sel_in(CTX_B8_3_wr),
		.mux_8_1_out(B8_3_wr)
	);  

	always @(posedge CLK or negedge RST) begin
		if (~RST) begin
			D48_0_out <= `DWORD_BITS'h0;
			D48_1_out <= `DWORD_BITS'h0;
			D48_2_out <= `DWORD_BITS'h0;
			D48_3_out <= `DWORD_BITS'h0;
			
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
			if(finish_lg9_rg) begin
				D48_0_out <= `DWORD_BITS'h0;
				D48_1_out <= `DWORD_BITS'h0;
				D48_2_out <= `DWORD_BITS'h0;
				D48_3_out <= `DWORD_BITS'h0;
				
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
				D48_0_out <= D48_0_wr;
				D48_1_out <= D48_1_wr;
				D48_2_out <= D48_2_wr;
				D48_3_out <= D48_3_wr;
							
				B16_0_out <= B16_0_wr;
				B16_1_out <= B16_1_wr;
				B16_2_out <= B16_2_wr;
				B16_3_out <= B16_3_wr;
							
				B8_0_out  <= B8_0_wr;
				B8_1_out  <= B8_1_wr;
				B8_2_out  <= B8_2_wr;
				B8_3_out  <= B8_3_wr;
			end
		end
	end
 
endmodule