`timescale 1ns / 1ps
//-------------------------------------------------------------------------------------------------//
//  File name	: TB_PEA.v							                      			     		   //
//  Project		: SoC Simulation															       //
//  Author		: Pham Hoai Luan                                                                   //
//  Description	: Test bench file 			    		                                           //
//  Referents	: none.																		       //
//-------------------------------------------------------------------------------------------------//
`include "common.vh"
module TB_PEA();
  reg								  				CLK;
  reg								  				RST;

  //-----------------------------------------------------//
  //          			Input Signals                    // 
  //-----------------------------------------------------//
  reg [7:0]				              				start_in;
  
  ///*** Confiiguration Memory ***///
  reg [`PE_NUM_BITS+`PE_CFG_ADDR_BITS-1:0]          CFG_addra_in;
  reg [`PE_CFG_BITS-1:0]              				CFG_dina_in;
  reg 					              				CFG_ena_in;
  reg 					              				CFG_wea_in;
				
  reg [7:0]				              				CFG_incr_in;
				
  ///*** Local Data Memory ***///				
  reg [`PE_NUM_BITS+`LDM_ADDR_BITS-1:0]         	LDM_addra_in;
  reg [`AXI_DWIDTH_BITS-1:0]          				LDM_dina_in;
  reg 					              				LDM_ena_in;
  reg 					              				LDM_wea_in;

  //-----------------------------------------------------//
  //          			Output Signals                   // 
  //-----------------------------------------------------//  
  wire [`AXI_DWIDTH_BITS-1:0]         				LDM_douta_out;
  wire 						         				LDM_douta_valid_out;
    reg     [95:0] CFG [0:2115];
	reg     [159:0] LDM [0:47];
    integer         i, w;

 PEA pea(
  .CLK(CLK),
  .RST(RST),
  .start_in(start_in),
  .CFG_addra_in(CFG_addra_in),
  .CFG_dina_in(CFG_dina_in),
  .CFG_ena_in(CFG_ena_in),
  .CFG_wea_in(CFG_wea_in),				
  .CFG_incr_in(CFG_incr_in),
  .LDM_addra_in(LDM_addra_in),
  .LDM_dina_in(LDM_dina_in),
  .LDM_ena_in(LDM_ena_in),
  .LDM_wea_in(LDM_wea_in),
  .LDM_douta_out(LDM_douta_out),
  .LDM_douta_valid_out(LDM_douta_valid_out)
	);
		
    initial begin
        CLK = 1'b0;
        forever #10 CLK = ~CLK;
    end

    initial begin
        $readmemh("data.txt", data);
        w = $fopen("C:\\Users\\ASUS\\Desktop\\debug\\result2.txt", "w");
        for (i = 0; i < 1000; i = i + 1) begin
            RST = 1'b0;
            start_in = 1'b0;
            {a_in, b_in, c_in, d_in} = data[i];
            #20;

            RST = ~RST;
            start_in = ~start_in;
            #15 $fwrite(w, "%h\n", result_out);
            #5;
            
            start_in = ~start_in;
            #20;
        end
        $stop;
    end
endmodule
