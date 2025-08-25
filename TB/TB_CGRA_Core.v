`timescale 1ns / 1ps
//-------------------------------------------------------------------------------------------------//
//  File name	: TB_PEA.v							                      			     		   //
//  Project		: SoC Simulation															       //
//  Author		: Pham Hoai Luan                                                                   //
//  Description	: Test bench file 			    		                                           //
//  Referents	: none.																		       //
//-------------------------------------------------------------------------------------------------//
`include "common.vh"
`include "/home/hoailuan/Research/2024/CGRA_64bit/C_Simulator/SHA3/common2.vh"
module TB_CGRA_Core();
	reg								  					CLK;
	reg								  					RST;
    
	//-----------------------------------------------------//
	//          			Input Signals                  // 
	//-----------------------------------------------------//
	reg 					              				start_in;
	reg													Mode_in;
	
	///*** Context RC Memory ***///
	wire [`PE_NUM_BITS+`CTX_RC_ADDR_BITS-1:0]   		CTX_RC_addra_in;
	reg [31:0]   										CTX_RC_addra_rg;
	wire [`CTX_RC_BITS-1:0]              				CTX_RC_dina_in;
	reg [63:0]              							CTX_RC_dina_rg;
	reg 					              				CTX_RC_ena_in;
	reg 					              				CTX_RC_wea_in;
	
	///*** Context PE Memory ***///
	wire [`PE_NUM_BITS+`CTX_PE_ADDR_BITS-1:0]          	CTX_PE_addra_in;
	reg [31:0]   										CTX_PE_addra_rg;
	reg [`CTX_PE_BITS-1:0]              				CTX_PE_dina_in;
	reg 					              				CTX_PE_ena_in;
	reg 					              				CTX_PE_wea_in;
	
	///*** Context IM Memory ***///
	wire [`PE_NUM_BITS+`CTX_IM_ADDR_BITS-1:0]          	CTX_IM_addra_in;
	reg [31:0]   										CTX_IM_addra_rg;
	reg [`CTX_IM_BITS-1:0]              				CTX_IM_dina_in;
	reg 					              				CTX_IM_ena_in;
	reg 					              				CTX_IM_wea_in;
			
			
	///*** Local Data Memory ***///				
	reg [31:0]          								LDM_addra_rg;
	wire [`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS-1:0]     LDM_addra_in;
	reg [`AXI_DWIDTH_BITS-1:0]          				LDM_dina_in;
	reg 					              				LDM_ena_in;
	reg 					              				LDM_wea_in;
    
	//-----------------------------------------------------//
	//          			Output Signals                 // 
	//-----------------------------------------------------//  
	wire [`AXI_DWIDTH_BITS-1:0]         				LDM_douta_out;
	wire 						         				complete_out;
	
	reg     [95:0] 	CTX_RC [0:`CTX_RC_DEPTH-1];
	reg     [63:0] 	CTX_PE [0:`CTX_PE_DEPTH-1];
	reg     [95:0] 	CTX_IM [0:`CTX_IM_DEPTH-1];
	reg     [287:0] LDM [0:`LDM_DEPTH-1];
	
	integer         i, w;
	  
	CGRA_Core cgra_core(
		.CLK(CLK),
		.RST(RST),
		.start_in(start_in),
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
		.LDM_addra_in(LDM_addra_in),
		.LDM_dina_in(LDM_dina_in),
		.LDM_ena_in(LDM_ena_in),
		.LDM_wea_in(LDM_wea_in),
		.LDM_douta_out(LDM_douta_out),
		.complete_out(complete_out)
	);
	 
	assign LDM_addra_in = LDM_addra_rg[`RW_NUM_BITS+`LR_BITS+`LDM_ADDR_BITS+5-1:5];
	assign CTX_RC_addra_in = {CTX_RC_addra_rg[`PE_NUM_BITS-1:0],CTX_RC_addra_rg[`PE_NUM_BITS+`CTX_RC_ADDR_BITS-1:`PE_NUM_BITS]}; 
	assign CTX_PE_addra_in = {CTX_PE_addra_rg[`PE_NUM_BITS-1:0],CTX_PE_addra_rg[`PE_NUM_BITS+`CTX_PE_ADDR_BITS-1:`PE_NUM_BITS]};   
	assign CTX_IM_addra_in = {CTX_IM_addra_rg[`PE_NUM_BITS-1:0],CTX_IM_addra_rg[`PE_NUM_BITS+`CTX_IM_ADDR_BITS-1:`PE_NUM_BITS]}; 
  	assign CTX_RC_dina_in = CTX_RC_dina_rg[`CTX_RC_BITS-1:0]; 
		initial begin
			CLK = 1'b0;
			forever #10 CLK = ~CLK;
		end

		initial begin
			RST = 1'b0;
			start_in = 1'b0;
			Mode_in	= 1'b1;
			CTX_RC_ena_in = 1'b0;
			CTX_RC_wea_in = 1'b0;
			CTX_PE_ena_in = 1'b0;
			CTX_PE_wea_in = 1'b0;
			CTX_IM_ena_in = 1'b0;
			CTX_IM_wea_in = 1'b0;
			LDM_ena_in = 1'b0;
			LDM_wea_in = 1'b0;
			CTX_RC_addra_rg = 32'h0;
			CTX_PE_addra_rg = 32'h0;
			CTX_IM_addra_rg = 32'h0;
			LDM_addra_rg = 32'h0;
			CTX_RC_dina_rg = 0;
			CTX_PE_dina_in = 0;
			CTX_IM_dina_in = 0;
			LDM_dina_in = 128'h0;
			#40 RST = 1'b1;
			 #100;
			$readmemh("/home/hoailuan/Research/2024/CGRA_64bit/C_Simulator/SHA3/CTX_RC_File.txt", CTX_RC);
			$readmemh("/home/hoailuan/Research/2024/CGRA_64bit/C_Simulator/SHA3/CTX_PE_File.txt", CTX_PE);
			$readmemh("/home/hoailuan/Research/2024/CGRA_64bit/C_Simulator/SHA3/CTX_IM_File.txt", CTX_IM);
			$readmemh("/home/hoailuan/Research/2024/CGRA_64bit/C_Simulator/SHA3/LDM_File.txt", LDM);
			for (i = 0; i < `CTX_RC_DEPTH; i = i + 1) begin
				{CTX_RC_addra_rg, CTX_RC_dina_rg} = CTX_RC[i];
				CTX_RC_ena_in = 1'b1;
				CTX_RC_wea_in = 1'b1;
				#20;
			end
			CTX_RC_ena_in = 1'b0;
			CTX_RC_wea_in = 1'b0;
			#20;
			
			for (i = 0; i < `CTX_PE_DEPTH; i = i + 1) begin
				{CTX_PE_addra_rg, CTX_PE_dina_in} = CTX_PE[i];
				CTX_PE_ena_in = 1'b1;
				CTX_PE_wea_in = 1'b1;
				#20;
			end
			CTX_PE_ena_in = 1'b0;
			CTX_PE_wea_in = 1'b0;
			#20;

			for (i = 0; i < `CTX_IM_DEPTH; i = i + 1) begin
				{CTX_IM_addra_rg, CTX_IM_dina_in} = CTX_IM[i];
				CTX_IM_ena_in = 1'b1;
				CTX_IM_wea_in = 1'b1;
				#20;
			end
			CTX_IM_ena_in = 1'b0;
			CTX_IM_wea_in = 1'b0;
			#20;
			for (i = 0; i < `LDM_DEPTH; i = i + 1) begin
				{LDM_addra_rg, LDM_dina_in} = LDM[i];
				LDM_ena_in = 1'b1;
				LDM_wea_in = 1'b1;
				#20;
			end
			LDM_ena_in = 1'b0;
			LDM_wea_in = 1'b0;
			LDM_addra_rg = 32'h0;
			#80;
			start_in   = 1'b1;
			#40;
			start_in   = 1'b0;
			
			#100;
			LDM_ena_in = 1'b1;
			LDM_wea_in = 1'b0;
			LDM_addra_rg = 32'h01008000;
			
			// #50000;
			// for (i = 0; i < `CTX_RC_DEPTH; i = i + 1) begin
				// {CTX_RC_addra_rg, CTX_RC_dina_rg} = CTX_RC[i];
				// CTX_RC_ena_in = 1'b1;
				// CTX_RC_wea_in = 1'b1;
				// #20;
			// end
			// CTX_RC_ena_in = 1'b0;
			// CTX_RC_wea_in = 1'b0;
			// #20;
			
			// for (i = 0; i < `CTX_PE_DEPTH; i = i + 1) begin
				// {CTX_PE_addra_rg, CTX_PE_dina_in} = CTX_PE[i];
				// CTX_PE_ena_in = 1'b1;
				// CTX_PE_wea_in = 1'b1;
				// #20;
			// end
			// CTX_PE_ena_in = 1'b0;
			// CTX_PE_wea_in = 1'b0;
			// #20;

			// for (i = 0; i < `CTX_IM_DEPTH; i = i + 1) begin
				// {CTX_IM_addra_rg, CTX_IM_dina_in} = CTX_IM[i];
				// CTX_IM_ena_in = 1'b1;
				// CTX_IM_wea_in = 1'b1;
				// #20;
			// end
			// CTX_IM_ena_in = 1'b0;
			// CTX_IM_wea_in = 1'b0;
			// #20;
			// for (i = 0; i < `LDM_DEPTH; i = i + 1) begin
				// {LDM_addra_rg, LDM_dina_in} = LDM[i];
				// LDM_ena_in = 1'b1;
				// LDM_wea_in = 1'b1;
				// #20;
			// end
			// LDM_ena_in = 1'b0;
			// LDM_wea_in = 1'b0;
			// LDM_addra_rg = 32'h0;
			// #80;
			// start_in   = 1'b1;
			// #20;
			// start_in   = 1'b0;
						// #50000;
			// for (i = 0; i < `CTX_RC_DEPTH; i = i + 1) begin
				// {CTX_RC_addra_rg, CTX_RC_dina_rg} = CTX_RC[i];
				// CTX_RC_ena_in = 1'b1;
				// CTX_RC_wea_in = 1'b1;
				// #20;
			// end
			// CTX_RC_ena_in = 1'b0;
			// CTX_RC_wea_in = 1'b0;
			// #20;
			
			// for (i = 0; i < `CTX_PE_DEPTH; i = i + 1) begin
				// {CTX_PE_addra_rg, CTX_PE_dina_in} = CTX_PE[i];
				// CTX_PE_ena_in = 1'b1;
				// CTX_PE_wea_in = 1'b1;
				// #20;
			// end
			// CTX_PE_ena_in = 1'b0;
			// CTX_PE_wea_in = 1'b0;
			// #20;

			// for (i = 0; i < `CTX_IM_DEPTH; i = i + 1) begin
				// {CTX_IM_addra_rg, CTX_IM_dina_in} = CTX_IM[i];
				// CTX_IM_ena_in = 1'b1;
				// CTX_IM_wea_in = 1'b1;
				// #20;
			// end
			// CTX_IM_ena_in = 1'b0;
			// CTX_IM_wea_in = 1'b0;
			// #20;
			// for (i = 0; i < `LDM_DEPTH; i = i + 1) begin
				// {LDM_addra_rg, LDM_dina_in} = LDM[i];
				// LDM_ena_in = 1'b1;
				// LDM_wea_in = 1'b1;
				// #20;
			// end
			// LDM_ena_in = 1'b0;
			// LDM_wea_in = 1'b0;
			// LDM_addra_rg = 32'h0;
			// #80;
			// start_in   = 1'b1;
			// #20;
			// start_in   = 1'b0;
		end
	endmodule
