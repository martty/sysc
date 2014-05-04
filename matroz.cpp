#ifndef MATROZ
#define MATROZ
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
struct ClockGenerator : public sc_module
{

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
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
template <unsigned WL>
struct ALU: public sc_module
{
	// ports --------------------------------------
	sc_in		< sc_lv<3> >	Function;
	sc_in		< sc_lv<WL> >	OpA;
	sc_in		< sc_lv<WL> >	OpB;
	sc_out	< sc_lv<WL> >	Result;
	sc_out	< sc_lv<1> >	ZOut;
	// constructor --------------------------------
	SC_CTOR(ALU) { SC_THREAD(FunctionThread); sensitive << Function << OpA << OpB; Result.initialize(0); ZOut.initialize(0); }
	// constants ----------------------------------
	#define f_add		0x0
	#define f_sub		0x1
	#define f_and		0x2
	#define f_or		0x3
	#define f_xor		0x4
	// internal variables -------------------------
	sc_lv<WL> vResult;
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			// Calculate result -----------------------
			switch (Function.read().to_uint()) {
				case f_add: 	vResult = static_cast< sc_lv<WL> >( OpA.read().to_uint() + OpB.read().to_uint() ); break;
				case f_sub: 	vResult = static_cast< sc_lv<WL> >( OpA.read().to_uint() - OpB.read().to_uint() ); break;
				case f_and: 	vResult = (OpA.read() & OpB.read()); break;
				case f_or: 		vResult = (OpA.read() | OpB.read()); break;
				case f_xor: 	vResult = (OpA.read() ^ OpB.read()); break;
				default: 			vResult = vResult;
											break;
			}
			Result.write(vResult);
			// Generate Zero flag ---------------------
			if ( vResult.to_uint() == 0 ) ZOut.write("1"); else ZOut.write("0");
			//-----------------------------------------
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------
#endif /* COMPONENTS */
//-------------------------------------------------------------------------------------------------