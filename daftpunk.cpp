#ifndef DAFTPUNK
#define DAFTPUNK
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
struct DaftPunk: public sc_module {
	// ports --------------------------------------
	sc_in		< sc_lv<16> >	Address;
	sc_in<bool> readneg;
    sc_in<bool> writeneg;
	sc_in<bool> csneg;
	sc_inout_rv	< 8 >  Data;
	
	sc_uint<8> RAM[0x10000];
	// constructor --------------------------------
	SC_CTOR(DaftPunk){
		SC_METHOD(FunctionThread);
		sensitive << readneg << writeneg;
		// Program memory initialization --
		for (unsigned int i = 0; i < 0x1000; i++){
			RAM[i] = 0;
		}
	}
	// function thread ----------------------------
	void FunctionThread(){
		if(csneg.read() == 0){
			if ( writeneg.read() == 0 ) { 
				RAM[Address.read().to_uint()] = Data.read(); 
			}
			else if ( readneg.read() == 0 ) { 
				Data.write( RAM[Address.read().to_uint()] ); 
			}
		} else {
			Data = "ZZZZZZZZ";
		}
	}
};
//-------------------------------------------------------------------------------------------------
#endif /* DAFTPUNK */
//-------------------------------------------------------------------------------------------------