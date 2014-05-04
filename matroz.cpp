#ifndef MATROZ
#define MATROZ
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct ClockGenerator : public sc_module{
  sc_out  < bool >  ClkOut;
  SC_CTOR(ClockGenerator) { SC_THREAD(FunctionThread); }
  void FunctionThread() {
    while (1) {
      ClkOut.write(false);
      wait(100,SC_NS);
      ClkOut.write(true);
      wait(100,SC_NS);
    }
  }
};

struct CSneg_Daft : public sc_module{
	sc_in	< sc_lv<16> > Address;
	sc_out  < bool >  csneg;
	SC_CTOR(CSneg_Daft) { 
		SC_METHOD(CS); 
		dont_initialize();
		sensitive << Address;
	}
	void CS() {
		if (Address.read().to_uint() < 0xD000)
			csneg.write(0);
		else
			csneg.write(1);
	}
};

struct CSneg_SysCVideo : public sc_module{
	sc_in	< sc_lv<16> > Address;
	sc_out  < bool >  csneg;
	SC_CTOR(CSneg_Daft) { 
		SC_METHOD(CS); 
		dont_initialize();
		sensitive << Address;
	}
	void CS() {
		if ((Address.read().to_uint() > 0xD000) && (Address.read().to_uint() < 0xDFFF))
			csneg.write(0);
		else
			csneg.write(1);
	}
};
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
#endif /* COMPONENTS */
//-------------------------------------------------------------------------------------------------