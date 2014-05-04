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
	static const unsigned inst_clc_imp = 0x18;     
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
	
int inst_type[39]={ inst_clc_imp, inst_dex_imp, inst_dey_imp, inst_inx_imp, inst_iny_imp, inst_pha_imp, inst_rts_imp, inst_tax_imp, inst_tay_imp, inst_txy_imp, inst_tya_imp,
					inst_asl_a, inst_rol_a,
					inst_adc_imm, inst_and_imm, inst_com_imm, inst_cpx_imm, inst_cpy_imm, inst_eor_imm, inst_lda_imm, inst_ldx_imm, inst_ldy_imm,
					inst_bcc_rel, inst_beq_rel, inst_bne_rel, inst_bsc_rel,
					inst_adc_abs, inst_asl_abs, inst_inc_abs, inst_jmp_abs, inst_jsr_abs, inst_lda_abs, inst_ldx_abs, inst_rol_abs, inst_sta_abs, inst_stx_abs,
					inst_sty_abs, inst_sty_abs,
					inst_lda_x, inst_sta_x
					};
//	0-11: 1
//	13-20: 2
//	21-24: 2
//	25-38: 3
	
	
struct SorhajoKapitany: public sc_module {
  
  // ports -----------------------------
  sc_in		< bool >		Clk;
  sc_in		< sc_lv<1> >	Reset;
  
  sc_out	< sc_lv<1> >	Abort;
  // internal resources ----------------
  sc_int<8>		PC;     // instruction pointer
  sc_lv<8>      P;		// zero flag
  sc_lv<24>		IR;		// intermediate register
  
  sc_lv<8>		A, X, Y, SP;
  // RAM
  unsigned short RAM[0x10000];
  unsigned current_fetch;
  
  
  

  
  //-----------------------------------------------------------------------------------------------
	void FunctionThread() {
		while (1) {
			if (Reset.read() == "1") {
			// reset all internal variables
				PC = 0;
				P = 0;
			} else {

				current_fetch = 0;
				do {
					Fetch();
				} while (Decode());
				Execute();
			}
		}
		wait();
	}
	
	void Fetch() {
		IR.range(current_fetch*8+7, current_fetch*8) = RAM[PC];
		std::cerr << "Fetched " << IR.range(current_fetch*8+7, current_fetch*8).to_uint() << std::endl;
	}
	
	bool Decode() {
		if (getFetchCount(IR.range(7,0).to_uint()) > current_fetch){
			PC++;
			current_fetch++;
			return true;
		} else {
			return false;
		}
	}
	
	void Execute() {
	}
	
	unsigned int getFetchCount(unsigned int op){
		return 1;
	}
	
  //-----------------------------------------------------------------------------------------------
  // contructor ------------------------
  SC_CTOR(SorhajoKapitany) {
    SC_THREAD(FunctionThread); sensitive << Clk.pos();
    // initialize internal variables ---
    PC = 0xC000;
    P = 0;
	A = X = Y = 0;
	SP = 0x01FF;
  }

};
//-------------------------------------------------------------------------------------------------
#endif /* SORHAJO_BEHAVIORAL */
//-------------------------------------------------------------------------------------------------