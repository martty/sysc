#include <systemc.h>
#include "matroz.cpp"
#include "daftpunk.cpp"
#include "syscvideo.h"
#include "sorhajokapitanymag.cpp"
//-------------------------------------------------------------------------------------------------
int sc_main(int argc, char* argv[]){
	sc_set_time_resolution(1,SC_NS);


	// component instantiations -------------------
	// clock generator ----------------------------
	ClockGenerator CLKGEN("CLKGEN");
	
	DaftPunk	DAFT("DAFT");

	SorhajoKapitany SYSTEM("SYSTEM");


	// internal signals ---------------------------
	sc_signal < bool >        Clk;
	sc_signal < sc_lv<1> >    Reset;
	//---------------------------------------------
	sc_signal < sc_lv<1> >	Abort;
	
	sc_signal < sc_lv<16> > Address("Address");
	sc_signal < sc_lv<12> > addrout("addrout");
	sc_signal < sc_lv<8> > Data("data");
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
	
	DAFT.Address(Address);
	DAFT.Data(Data);
	DAFT.readneg(readneg);
	DAFT.writeneg(writeneg);
	
	CSneg_Daft cd("csnegdaft");
	cd(Address, csnegdaft);
	
	CSneg_SysCVideo cs("csnegsyscv");
	cs(Address, csnegsyscv, addrout);
	
	SysCVideo gk("gk");
    gk(addrout, Data, readneg, writeneg, csnegsyscv);

	std::cout << "Sorhajokapitanymag v1" << std::endl;
  
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
		for (unsigned int i = 0; i < fsize; i++){
			unsigned int tmp = 0;
			fread(&tmp, 1, 1, f);
			DAFT.RAM[start + i] = tmp;
		}
		fclose(f);
	}
  
	std::cin.get();
  
	Reset.write("1");
	sc_start(340,SC_NS);
	Reset.write("0");
  
	while(true){
		sc_start(1,SC_NS);
		if(Abort.read() == "1"){
			break;
		}
	}
  
  std::cout << "Execution finished." << std::endl;
	//---------------------------------------------
  return 0;
}
//-------------------------------------------------------------------------------------------------
