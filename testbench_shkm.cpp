#include <systemc.h>
#include "matroz.cpp"
#include "daftpunk.cpp"
#include "syscvideo.h"
#include "sorhajokapitanymag.cpp"

DaftPunk	DAFT("DAFT");
SorhajoKapitany SYSTEM("SYSTEM");
//-------------------------------------------------------------------------------------------------
int sc_main(int argc, char* argv[]){
	sc_set_time_resolution(1,SC_NS);


	// component instantiations -------------------
	// clock generator ----------------------------
	ClockGenerator CLKGEN("CLKGEN");


	// internal signals ---------------------------
	sc_signal < bool >        Clk;
	sc_signal < sc_lv<1> >    Reset;
	//---------------------------------------------
	sc_signal < sc_lv<1> >	Abort;

	sc_signal < sc_lv<16> > Address("Address");
	sc_signal < sc_lv<12> > addrout("addrout");
	sc_signal_rv < 8 > Data("data");
	sc_signal < bool >	readneg("rneg");
	sc_signal < bool >	writeneg("wneg");
	sc_signal < bool >	csnegsyscv("cssyscv");
	sc_signal < bool >	csnegdaft("csnegdaft");

	// internal connections -----------------------
	CLKGEN.ClkOut(Clk);

	SYSTEM.Clk(Clk);

	SYSTEM.Reset(Reset);
	SYSTEM.Abort(Abort);
	SYSTEM.Data(Data);
	SYSTEM.Address(Address);
	SYSTEM.readneg(readneg);
	SYSTEM.writeneg(writeneg);

	CSneg_Daft cd("csnegdaftd");
	cd(Address, csnegdaft);

	DAFT.Address(Address);
	DAFT.Data(Data);
	DAFT.readneg(readneg);
	DAFT.writeneg(writeneg);
	DAFT.csneg(csnegdaft);

	CSneg_SysCVideo cs("csnegsyscv");
	cs(Address, csnegsyscv, addrout);

	SysCVideo gk("gk");
    gk(addrout, Data, readneg, writeneg, csnegsyscv);

	std::cout << "Sorhajokapitanymag v1" << std::endl;


  sc_trace_file *tf;
  tf = sc_create_vcd_trace_file("trace");
  sc_trace(tf,SYSTEM.Reset,"Reset");
  sc_trace(tf,SYSTEM.Abort,"Abort");
  sc_trace(tf,SYSTEM.readneg,"RNEG");
  sc_trace(tf,SYSTEM.writeneg,"WNEG");
  sc_trace(tf,SYSTEM.Data,"Data");
  sc_trace(tf,SYSTEM.Address,"Address");
  sc_trace(tf,DAFT.csneg,"CSNEG_DAFT");
  sc_trace(tf,gk.csneg,"CSNEG_SysCVideo");



	Reset.write("1");
	sc_start(340,SC_NS);
	Reset.write("0");

	if(argc != 3 ){
		std::cout << "Pass binary file to load as parameter and address to load at" << std::endl;
		return 1;
	} else {
		FILE* f = fopen(argv[1], "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		unsigned long start = strtoul(argv[2], NULL, 16);
		std::cout << "Loaded " << std::hex << fsize << " bytes at " << std::hex << start << std::endl;
		fread((DAFT.RAM+start), fsize, 1, f);
		/*for (unsigned int i = 0; i < fsize; i++){
			unsigned int tmp = 0;
			fread(&tmp, 1, 1, f);
			DAFT.RAM[start + i] = tmp;
		}*/
		fclose(f);
	}

	for(int i = 0; ; i++){
		sc_start(1,SC_NS);
		if(Abort.read() == "1"){
			break;
		}
	}

  std::cout << "Execution finished." << std::endl;
   sc_close_vcd_trace_file(tf);
	//---------------------------------------------
  return 0;
}
//-------------------------------------------------------------------------------------------------
