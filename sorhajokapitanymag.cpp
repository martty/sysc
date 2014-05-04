#ifndef SORHAJO_BEHAVIORAL
#define SORHAJO_BEHAVIORAL
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
//-------------------------------------------------------------------------------------------------
// instruction set -------------------
	static const unsigned inst_adc_imm = 0x69;     
	static const unsigned inst_adc_abs = 0x6D;     
	static const unsigned inst_clc = 0x18;     
	static const unsigned inst_and_imm = 0x29;     
	static const unsigned inst_eor_imm = 0x49;
	static const unsigned inst_asl_a = 0x0a;
	static const unsigned inst_asl_abs = 0x0e;
	static const unsigned inst_rol_a = 0x2a;
	static const unsigned inst_rol_abs = 0x2e;
	static const unsigned inst_bcc_rel = 0x90;
	static const unsigned inst_bsc_rel = 0xb0;
	static const unsigned inst_beq_rel = 0xf0;
	static const unsigned inst_bne_rel = 0xd0;
	static const unsigned inst_com_imm = 0xc9;
	static const unsigned inst_cpx_imm = 0xe0;
	static const unsigned inst_cpy_imm = 0xc0;
	static const unsigned inst_dex_imp = 0xca;
	static const unsigned inst_dey_imp = 0x88;
	static const unsigned inst_inx_imp = 0xe8;
	static const unsigned inst_iny_imp = 0xc8;
	static const unsigned inst_inc_abs = 0xee;
	static const unsigned inst_jmp_abs = 0x4c;
	static const unsigned inst_jsr_abs = 0x20;
	static const unsigned inst_rts_imp = 0x60;
	static const unsigned inst_lda_imm = 0xa9;
	static const unsigned inst_lda_abs = 0xad;
	static const unsigned inst_lda_x = 0xa1;
	static const unsigned inst_ldx_imm = 0xa2;
	static const unsigned inst_ldx_abs = 0xae;
	static const unsigned inst_ldy_imm = 0xa0;
	static const unsigned inst_pla_imp = 0x68;
	static const unsigned inst_pha_imp = 0x48;
	static const unsigned inst_sta_abs = 0x8d;
	static const unsigned inst_sta_x = 0x81;
	static const unsigned inst_stx_abs = 0x8e;
	static const unsigned inst_sty_abs = 0x8c;
	static const unsigned inst_tax_imp = 0xaa;
	static const unsigned inst_tay_imp = 0xa8;
	static const unsigned inst_txa_imp = 0x8a;
	static const unsigned inst_tya_imp = 0x98;
	
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
#endif /* SORHAJO_BEHAVIORAL */
//-------------------------------------------------------------------------------------------------