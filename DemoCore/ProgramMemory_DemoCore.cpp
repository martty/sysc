#ifndef PM
#define PM
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
//-------------------------------------------------------------------------------------------------
struct ProgramMemory_DemoCore: public sc_module
{
	// ports --------------------------------------
	sc_in		< sc_lv<8> >	Address;
	sc_out	< sc_lv<15> >	DataOut;
	// internal variables -------------------------
	sc_lv<15>		Content[256];
	// function thread ----------------------------
	void FunctionThread()
	{
		while (1)
		{
			DataOut.write( Content[Address.read().to_uint()] );
			wait();
		}
	}
  // constructor --------------------------------
	SC_CTOR(ProgramMemory_DemoCore) 
	{
		SC_THREAD(FunctionThread);
		sensitive << Address;
		// Program memory initialization --
		Content[0] = "000100000000111"; // movi r0,7
    Content[1] = "000100100000110"; // movi r1,6
    Content[2] = "000101000000101"; // movi r2,5
    Content[3] = "000101100000100"; // movi r3,4
    Content[4] = "010010000000001"; // add r4,r0,r1
    Content[5] = "010110100010011"; // sub r5,r2,r3
    Content[6] = "011011000011000"; // and r6,r3,r0
    Content[7] = "011111100010001"; // or  r7,r2,r1
    Content[8] = "100010000111110"; // xor r4,r7,r6
    Content[9] = "001000000000000"; // sdm r0,0h
    Content[10] = "001000100000001"; // sdm r1,1h
    Content[11] = "001001000000010"; // sdm r2,2h
    Content[12] = "001001100000011"; // sdm r3,3h
    Content[13] = "001010000000100"; // sdm r4,4h
    Content[14] = "001010100000101"; // sdm r5,5h
    Content[15] = "001011000000110"; // sdm r6,6h
    Content[16] = "001011100000111"; // sdm r7,7h
    Content[17] = "001100000000111"; // ldm r0,7h
    Content[18] = "001100100000110"; // ldm r1,6h
    Content[19] = "001101000000101"; // ldm r2,5h
    Content[20] = "001101100000100"; // ldm r3,4h
    Content[21] = "001110000000011"; // ldm r4,3h
    Content[22] = "001110100000010"; // ldm r5,2h
    Content[23] = "001111000000001"; // ldm r6,1h
    Content[24] = "001111100000000"; // ldm r7,0h
    Content[25] = "000100001000101"; // movi r0,45h
    Content[26] = "000100101000110"; // movi r1,46h
    Content[27] = "010101100000001"; // sub r3,r0,r1
    Content[28] = "100100000100011"; // j 35
    Content[35] = "101000000101010"; // jz 42
    Content[36] = "000100101000101"; // movi r1,45h
    Content[37] = "010101100000001"; // sub r3,r0,r1
    Content[38] = "101000000101010"; // jz 42
    Content[42] = "000010000000111"; // mov r4,r7
    Content[43] = "000010100000000"; // mov r5,r0
    Content[44] = "110000000000000"; // abort
		//---------------------------------
	}
};
//-------------------------------------------------------------------------------------------------
#endif; /* PM */
//-------------------------------------------------------------------------------------------------