#include <systemc.h>
#include "DemoCore_Structural_TopLevel.cpp"
#include "DemoCore_Behavioral.cpp"
#include "ProgramMemory_DemoCore.cpp"
#include "DataMemory_DemoCore.cpp"
//-------------------------------------------------------------------------------------------------
int sc_main(int argc, char* argv[])
{
  sc_set_time_resolution(1,SC_NS);


  // component instantiations -------------------
  // clock generator ----------------------------
  ClockGenerator CLKGEN("CLKGEN");

  // DemoCore uP --------------------------------
	// DemoCore_Behavioral SYSTEM("SYSTEM"); // FSMD Behavioral model
  DemoCore_Structural_TopLevel SYSTEM("SYSTEM"); // FSM+D structural model

  // program memory and data memory -------------
  ProgramMemory_DemoCore PRMEM("PRMEM");
  DataMemory_DemoCore DMEM("DMEM");


  // internal signals ---------------------------
  sc_signal < bool >        Clk;
  sc_signal < sc_lv<1> >    Reset;
  sc_signal < sc_lv<1> >    Start;
  sc_signal < sc_lv<1> >    Read;
  sc_signal < sc_lv<1> >    Write;
  sc_signal < sc_lv<8> >    Addr2DataMem;
  sc_signal < sc_lv<8> >    Data2DataMem;
  sc_signal < sc_lv<8> >    DataFromDataMem;
  sc_signal < sc_lv<8> >    Addr2PrMem;
  sc_signal < sc_lv<15> >   DataFromPrMem;
  sc_signal < sc_lv<1> >    Abort;
  //---------------------------------------------

	// internal connections -----------------------
  CLKGEN.ClkOut(Clk);

	SYSTEM.Clk(Clk);
  SYSTEM.Reset(Reset);
  SYSTEM.Start(Start);
  SYSTEM.Read(Read);
  SYSTEM.Write(Write);
  SYSTEM.Addr2DataMem(Addr2DataMem);
  SYSTEM.Data2DataMem(Data2DataMem);
  SYSTEM.DataFromDataMem(DataFromDataMem);
  SYSTEM.Addr2PrMem(Addr2PrMem);
  SYSTEM.DataFromPrMem(DataFromPrMem);
  SYSTEM.Abort(Abort);

  PRMEM.Address(Addr2PrMem);
  PRMEM.DataOut(DataFromPrMem);

  DMEM.Clk(Clk);
  DMEM.Address(Addr2DataMem);
  DMEM.Read(Read);
  DMEM.Write(Write);
  DMEM.DataIn(Data2DataMem);
  DMEM.DataOut(DataFromDataMem);
	//---------------------------------------------

  // trace signals ------------------------------
  sc_trace_file *tf;
  tf = sc_create_vcd_trace_file("trace");

  // traced signals of DemoCore_Behavioral ------
	/*sc_trace(tf,Clk,"Clk");
  sc_trace(tf,Reset,"Reset");
  sc_trace(tf,Start,"Start");
  sc_trace(tf,Abort,"Abort");
  sc_trace(tf,SYSTEM.Read,"Read");
  sc_trace(tf,SYSTEM.Write,"Write");
  sc_trace(tf,SYSTEM.State,"State");
  sc_trace(tf,SYSTEM.IR,"IR");
  sc_trace(tf,SYSTEM.IP,"IP");
  sc_trace(tf,SYSTEM.MAR,"MAR");
  sc_trace(tf,SYSTEM.MDR,"MDR");
  sc_trace(tf,SYSTEM.GPRF[0],"GPRF0");
  sc_trace(tf,SYSTEM.GPRF[1],"GPRF1");
  sc_trace(tf,SYSTEM.GPRF[2],"GPRF2");
  sc_trace(tf,SYSTEM.GPRF[3],"GPRF3");
  sc_trace(tf,SYSTEM.GPRF[4],"GPRF4");
  sc_trace(tf,SYSTEM.GPRF[5],"GPRF5");
  sc_trace(tf,SYSTEM.GPRF[6],"GPRF6");
  sc_trace(tf,SYSTEM.GPRF[7],"GPRF7");
  sc_trace(tf,SYSTEM.Zero,"Zero");
  sc_trace(tf,SYSTEM.Addr2PrMem,"Addr2PrMem");
  sc_trace(tf,SYSTEM.DataFromPrMem,"DataFromPrMem");
  sc_trace(tf,SYSTEM.Addr2DataMem,"Addr2DataMem");
  sc_trace(tf,SYSTEM.DataFromDataMem,"DataFromDataMem");
  sc_trace(tf,SYSTEM.Data2DataMem,"Data2DataMem");*/
  //---------------------------------------------

  // traced signals of DemoCore_Structural ------
  sc_trace(tf,SYSTEM.Clk,"Clk");
  sc_trace(tf,SYSTEM.Clk_n,"Clk_n");
  sc_trace(tf,SYSTEM.Reset,"Reset");
  sc_trace(tf,SYSTEM.Start,"Start");
  sc_trace(tf,SYSTEM.Abort,"Abort");
  sc_trace(tf,SYSTEM.Read,"Read");
  sc_trace(tf,SYSTEM.Write,"Write");
  sc_trace(tf,SYSTEM.CNT.State,"State");
  sc_trace(tf,SYSTEM.CNT.MX_GPRFDataIn,"MX_GPRFDataIn");
  sc_trace(tf,SYSTEM.CNT.MX_GPRFAddrA,"MX_GPRFAddrA");
  sc_trace(tf,SYSTEM.CNT.LD_IR,"LD_IR");
  sc_trace(tf,SYSTEM.CNT.OpCode,"OpCode");
  sc_trace(tf,SYSTEM.CNT.MX_IP,"MX_IP");
  sc_trace(tf,SYSTEM.CNT.LD_IP,"LD_IP");
  sc_trace(tf,SYSTEM.CNT.LD_GPRF,"LD_GPRF");
  sc_trace(tf,SYSTEM.CNT.LD_MAR,"LD_MAR");
  sc_trace(tf,SYSTEM.CNT.LD_MDR,"LD_MDR");
  sc_trace(tf,SYSTEM.CNT.ALUFunc,"ALUFunc");
  sc_trace(tf,SYSTEM.CNT.LD_Z,"LD_Z");
  sc_trace(tf,SYSTEM.DP.IR.DataOut,"IR");
  sc_trace(tf,SYSTEM.DP.IP.DataOut,"IP");
  sc_trace(tf,SYSTEM.DP.MAR.DataOut,"MAR");
  sc_trace(tf,SYSTEM.DP.MDR.DataOut,"MDR");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[0],"GPRF0");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[1],"GPRF1");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[2],"GPRF2");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[3],"GPRF3");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[4],"GPRF4");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[5],"GPRF5");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[6],"GPRF6");
  sc_trace(tf,SYSTEM.DP.GPRF.RegisterFile[7],"GPRF7");
  sc_trace(tf,SYSTEM.DP.Z.DataOut,"Zero");
  sc_trace(tf,SYSTEM.Addr2PrMem,"Addr2PrMem");
  sc_trace(tf,SYSTEM.DataFromPrMem,"DataFromPrMem");
  sc_trace(tf,SYSTEM.Addr2DataMem,"Addr2DataMem");
  sc_trace(tf,SYSTEM.DataFromDataMem,"DataFromDataMem");
  sc_trace(tf,SYSTEM.Data2DataMem,"Data2DataMem");
  //---------------------------------------------

  // trace data memory --------------------------
  sc_trace(tf,DMEM.Content[0],"DMEM0");
  sc_trace(tf,DMEM.Content[1],"DMEM1");
  sc_trace(tf,DMEM.Content[2],"DMEM2");
  sc_trace(tf,DMEM.Content[3],"DMEM3");
  sc_trace(tf,DMEM.Content[4],"DMEM4");
  sc_trace(tf,DMEM.Content[5],"DMEM5");
  sc_trace(tf,DMEM.Content[6],"DMEM6");
  sc_trace(tf,DMEM.Content[7],"DMEM7");
	//---------------------------------------------

  // stimulus -----------------------------------
  Reset.write("1");
  Start.write("0");
  sc_start(340,SC_NS);
  Reset.write("0");
  Start.write("1");
  sc_start(60,SC_US);
  std::cin.get(); 
	//---------------------------------------------
  sc_close_vcd_trace_file(tf);
  return (0);
}
//-------------------------------------------------------------------------------------------------