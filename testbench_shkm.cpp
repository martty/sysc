#include <systemc.h>
#include "matroz.cpp"
#include "sorhajokapitanymag.cpp"
//-------------------------------------------------------------------------------------------------
int sc_main(int argc, char* argv[]){
	sc_set_time_resolution(1,SC_NS);


	// component instantiations -------------------
	// clock generator ----------------------------
	ClockGenerator CLKGEN("CLKGEN");

	SorhajoKapitany SYSTEM("SYSTEM");


	// internal signals ---------------------------
	sc_signal < bool >        Clk;
	sc_signal < sc_lv<1> >    Reset;
	//---------------------------------------------
	sc_signal < sc_lv<1> >	Abort;

	// internal connections -----------------------
	CLKGEN.ClkOut(Clk);

	SYSTEM.Clk(Clk);
	SYSTEM.Reset(Reset);
	SYSTEM.Abort(Abort);

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
			SYSTEM.RAM[start + i] = tmp;
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
