#ifndef COMPONENTS
#define COMPONENTS
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
struct Inverter : public sc_module
{
  // ports ----------------------------
  sc_in   < bool >  Input;
  sc_out  < bool >  Output;
  // constructor ----------------------
  SC_CTOR(Inverter) { SC_THREAD(FunctionThread); sensitive << Input; Output.initialize(0); }
  // function thread ------------------
  void FunctionThread() {
    while (1) {
      Output.write(!Input.read());
      wait();
    }
  }
};
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
template <unsigned Width,unsigned High,unsigned Low>
struct RangeSelector: public sc_module
{
	// ports --------------------------------------
	sc_in		< sc_lv<Width> >				Input;
	sc_out	< sc_lv<(High-Low)+1> >	Output;
	// constructor --------------------------------
	SC_CTOR(RangeSelector) { SC_THREAD(FunctionThread); sensitive << Input; Output.initialize(0); }
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			Output.write( Input.read().range(High,Low) );
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------------------
template <unsigned Width>
struct StandardRegister: public sc_module
{
	// ports --------------------------------------
	sc_in   < bool >					Clk;
	sc_in		< sc_lv<1> >			Clr;
	sc_in 	< sc_lv<1> >			Load;
	sc_in 	< sc_lv<Width> >	DataIn;
	sc_out 	< sc_lv<Width> >	DataOut;
	// constructor --------------------------------
	SC_CTOR(StandardRegister) {
		SC_THREAD(FunctionThread);
		sensitive << Clk.pos() << Clr;
    DataOut.initialize(0);
	}
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			if (Clr.read() == "1") { DataOut.write(sc_lv<Width>('0')); }
			else { if (Load.read() == "1") DataOut.write(DataIn.read()); }
      wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------------------
template <unsigned AddressWidth,unsigned Capacity,unsigned DataWidth>
struct DPRF: public sc_module
{
	// ports --------------------------------------
	sc_in   < bool >  							Clk;
	sc_in  	< sc_lv<1> >						WrEnable;
	sc_in		< sc_lv<AddressWidth> >	AddressA;
	sc_in		< sc_lv<AddressWidth> >	AddressB;
	sc_in		< sc_lv<DataWidth> >		DataIn;
	sc_out	< sc_lv<DataWidth> >		DataOutA;
	sc_out	< sc_lv<DataWidth> >		DataOutB;
	// internal signals ---------------------------
	sc_lv <DataWidth> RegisterFile[Capacity];
  // constructor --------------------------------
	SC_CTOR(DPRF) { SC_THREAD(FunctionThread); sensitive << Clk.pos(); DataOutA.initialize(0); DataOutB.initialize(0);
                  for(unsigned i = 0;i<Capacity;++i) { RegisterFile[i] = static_cast< sc_lv<DataWidth> >(0); } }
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			if (WrEnable.read() == "1") { RegisterFile[AddressA.read().to_uint()] = DataIn.read(); }
			DataOutA.write(RegisterFile[AddressA.read().to_uint()]);
			DataOutB.write(RegisterFile[AddressB.read().to_uint()]);
      wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------------------
template <unsigned Width>
struct Mux2: public sc_module
{
	// ports --------------------------------------
	sc_in		< sc_lv<Width> >	Input0;
	sc_in		< sc_lv<Width> >	Input1;
	sc_in		< sc_lv<1> >			Sel;
	sc_out	< sc_lv<Width> >	Output;
	// constructor --------------------------------
	SC_CTOR(Mux2) { SC_THREAD(FunctionThread); sensitive << Sel << Input0 << Input1; Output.initialize(0); }
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			if 			(Sel.read() == "0") Output.write(Input0.read());
			else if (Sel.read() == "1") Output.write(Input1.read());
			else 												Output.write(sc_lv<Width>('X'));
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------------------
template <unsigned Width>
struct Mux4: public sc_module
{
	// ports --------------------------------------
	sc_in		< sc_lv<Width> >	Input0;
	sc_in		< sc_lv<Width> >	Input1;
	sc_in		< sc_lv<Width> >	Input2;
	sc_in		< sc_lv<Width> >	Input3;
	sc_in		< sc_lv<2> >			Sel;
	sc_out	< sc_lv<Width> >	Output;
	// constructor --------------------------------
	SC_CTOR(Mux4) { SC_THREAD(FunctionThread); sensitive << Sel << Input0 << Input1 << Input2 << Input3; Output.initialize(0); }
	// function thread ----------------------------
	void FunctionThread() {
		while (1) {
			if 			(Sel.read() == "00") 	Output.write(Input0.read());
			else if (Sel.read() == "01") 	Output.write(Input1.read());
			else if (Sel.read() == "10") 	Output.write(Input2.read());
			else if (Sel.read() == "11") 	Output.write(Input3.read());
			else 													Output.write(sc_lv<Width>('X'));
			wait();
		}
	}
};
//-------------------------------------------------------------------------------------------------









//-------------------------------------------------------------------------------------------------
template <unsigned Width>
struct Adder: public sc_module
{
	// ports ---------------------------------------------------------------
	sc_in 	< sc_lv<Width> >  Input1;
	sc_in 	< sc_lv<Width> >  Input2;
	sc_out 	< sc_lv<Width> > 	Output;
	// constructor ---------------------------------------------------------
	SC_CTOR(Adder) { SC_THREAD(FunctionThread); sensitive << Input1 << Input2; Output.initialize(0); }
	// function thread -----------------------------------------------------
	void FunctionThread() {
		while (1) {
			Output.write(static_cast< sc_lv<Width> >(Input1.read().to_uint() + Input2.read().to_uint()));
			wait();
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