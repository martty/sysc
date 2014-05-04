#ifndef SORHAJO_BEHAVIORAL
#define SORHAJO_BEHAVIORAL
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
//-------------------------------------------------------------------------------------------------
struct SorhajoKapitany: public sc_module {
  
  // ports -----------------------------
  sc_in		< bool >		Clk;
  sc_in		< sc_lv<1> >	Reset;
  
  sc_out	< sc_lv<1> >	Abort;
  // internal resources ----------------
  sc_lv<8>      PC;     // instruction pointer
  sc_lv<8>      P;		// zero flag
  
  
  // RAM
  unsigned short RAM[0x10000];

  
  //-----------------------------------------------------------------------------------------------
	void FunctionThread() {
		while (1) {
			if (Reset.read() == "1") {
			// reset all internal variables
				PC = 0;
				P = 0;
			} else {
				wait();
			}
		}
	}
  //-----------------------------------------------------------------------------------------------
  // contructor ------------------------
  SC_CTOR(SorhajoKapitany) {
    SC_THREAD(FunctionThread); sensitive << Clk.pos();
    // initialize internal variables ---
    PC = 0;
    P = 0;
  }

};
//-------------------------------------------------------------------------------------------------
#endif; /* SORHAJO_BEHAVIORAL */
//-------------------------------------------------------------------------------------------------