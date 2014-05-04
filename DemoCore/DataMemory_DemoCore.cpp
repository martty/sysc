#ifndef DM
#define DM
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
struct DataMemory_DemoCore: public sc_module
{
	// ports --------------------------------------
	sc_in   < bool >      Clk;
  sc_in		< sc_lv<8> >	Address;
  sc_in   < sc_lv<1> >  Read;
  sc_in   < sc_lv<1> >  Write;
  sc_in   < sc_lv<8> >  DataIn;
	sc_out	< sc_lv<8> >	DataOut;
	// constructor --------------------------------
	SC_CTOR(DataMemory_DemoCore) 
	{
		SC_THREAD(FunctionThread);
		sensitive << Clk.neg();
		// Program memory initialization --
		for(unsigned i = 0;i<256;++i) { Content[i] = "00000000"; }
		//---------------------------------
	}
	// internal variables -------------------------
	sc_lv<8>		Content[256];
	// function thread ----------------------------
	void FunctionThread()
	{
		while (1)
		{
      if ( Write.read() == "1" ) { Content[Address.read().to_uint()] = DataIn.read(); }
      else if ( Read.read() == "1" ) { DataOut.write( Content[Address.read().to_uint()] ); }
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------
#endif; /* DM */
//-------------------------------------------------------------------------------------------------