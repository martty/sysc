#ifndef DAFTPUNK
#define DAFTPUNK
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
struct DaftPunk: public sc_module {
	// ports --------------------------------------
	sc_in 		< bool >      Clk;
	sc_in		< sc_lv<16> >	Address;
	sc_in		< sc_lv<1> >  RWneg;
	sc_inout	< sc_lv<8> >  Data;
	
	sc_uint<8> RAM[0x10000];
	// constructor --------------------------------
	SC_CTOR(DaftPunk){
		SC_THREAD(FunctionThread);
		sensitive << Clk.neg();
		// Program memory initialization --
		//---------------------------------
	}
	// function thread ----------------------------
	void FunctionThread(){
		while (1){
			if ( RWneg.read() == "0" ) { 
				RAM[Address.read().to_uint()] = Data.read(); 
			}
			else if ( RWneg.read() == "1" ) { 
				Data.write( RAM[Address.read().to_uint()] ); 
			}
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------
#endif; /* DAFTPUNK */
//-------------------------------------------------------------------------------------------------