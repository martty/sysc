#ifndef SORHAJO_BEHAVIORAL
#define SORHAJO_BEHAVIORAL
#define N P[0]
#define V P[1]
#define D P[3]
#define Z P[5]
#define C P[6]

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
	static const unsigned inst_bcs_rel = 0xb0;
	static const unsigned inst_beq_rel = 0xf0;
	static const unsigned inst_bne_rel = 0xd0;
	static const unsigned inst_cmp_imm = 0xc9;
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
	static const unsigned inst_lda_abs_x = 0xbd;
	static const unsigned inst_ldx_imm = 0xa2;
	static const unsigned inst_ldx_abs = 0xae;
	static const unsigned inst_ldy_imm = 0xa0;
	static const unsigned inst_pla_imp = 0x68;
	static const unsigned inst_pha_imp = 0x48;
	static const unsigned inst_sta_abs = 0x8d;
	static const unsigned inst_sta_abs_x = 0x9d;
	static const unsigned inst_stx_abs = 0x8e;
	static const unsigned inst_sty_abs = 0x8c;
	static const unsigned inst_tax_imp = 0xaa;
	static const unsigned inst_tay_imp = 0xa8;
	static const unsigned inst_txa_imp = 0x8a;
	static const unsigned inst_tya_imp = 0x98;
	static const unsigned inst_brk_imp = 0x00;
	
int inst_type[42]={ inst_brk_imp, inst_clc_imp, inst_dex_imp, inst_dey_imp, inst_inx_imp, inst_iny_imp, inst_pla_imp, inst_pha_imp, inst_rts_imp, inst_tax_imp, inst_tay_imp, inst_txa_imp, inst_tya_imp,
					inst_asl_a, inst_rol_a,
					inst_adc_imm, inst_and_imm, inst_cmp_imm, inst_cpx_imm, inst_cpy_imm, inst_eor_imm, inst_lda_imm, inst_ldx_imm, inst_ldy_imm,
					inst_bcc_rel, inst_beq_rel, inst_bne_rel, inst_bcs_rel,
					inst_adc_abs, inst_asl_abs, inst_inc_abs, inst_jmp_abs, inst_jsr_abs, inst_lda_abs, inst_ldx_abs, inst_rol_abs, inst_sta_abs, inst_stx_abs,
					inst_sty_abs,
					inst_lda_abs_x, inst_sta_abs_x
					};
//	0-14: 1
//	15-27: 2
//	28-40: 3
	
	
struct SorhajoKapitany: public sc_module {
  
  // ports -----------------------------
  sc_in		< bool >		Clk;
  sc_in		< sc_lv<1> >	Reset;
  
  
  sc_out	< bool >	readneg;
  sc_out	< bool >	writeneg;
  sc_inout_rv	< 8 >	Data;
  sc_out	< sc_lv<16> >	Address;
  sc_out	< sc_lv<1> >	Abort;
  // internal resources ----------------
  sc_uint<16>	PC;     // instruction pointer
  sc_lv<8>      P;		// zero flag
  sc_lv<24>		IR;		// intermediate register
  
  sc_uint<8>		A, X, Y, SP;
  unsigned current_fetch;

  
  //-----------------------------------------------------------------------------------------------
	void FunctionThread() {
		while (1) {
			if (Reset.read() == "1") {
			// reset all internal variables
				PC = 0xC000;
				P = 0;
				IR = 0;
			} else {
				current_fetch = 1;
				do {
					Fetch();
				} while (Decode());
				PC++;
				Execute();
			}
			wait();
		}
	}
	
	void Fetch() {
		IR.range((current_fetch-1)*8+7, (current_fetch-1)*8) = get(PC);
		std::cerr << "Fetched " << std::hex << IR.range((current_fetch-1)*8+7, (current_fetch-1)*8).to_uint() << std::endl;
	}
	
	bool Decode() {
		if (getFetchCount(IR.range(7,0).to_uint()) > current_fetch){
			PC++;
			current_fetch++;
			wait();
			return true;
		} else {
			return false;
		}
	}
	
	unsigned int get(unsigned int addr){
		readneg.write(1);
		writeneg.write(1);
		Address.write(addr);
		readneg.write(0);
		wait();
		sc_uint<8> rd  = (Data.read().to_uint());
		readneg.write(1);
		writeneg.write(1);
		wait();
		return rd;
	}
	
	void set(unsigned int addr, unsigned int value){
		readneg.write(1);
		writeneg.write(1);
		Address.write(addr);
		Data.write(value);
		writeneg.write(0);
		wait();
		readneg.write(1);
		writeneg.write(1);
		wait();
	}
	
	void Execute() {
		std::cerr << "Executing " << std::hex << IR.range((current_fetch-1)*8+7, 0).to_uint() << "@" << PC.to_uint() << std::endl;
		unsigned int opcode = IR.range(7, 0).to_uint();
		unsigned int op1 = IR.range(15, 8).to_uint();
		unsigned int op2 = IR.range(23, 16).to_uint();

		sc_uint<8> M, t, l, h;
		int REL;

		bool temp;
		
		switch (opcode){
			case inst_adc_abs:
				M = get((op2<<8) + op1);
				
				t = C.to_bool() ? A + M + 1 :  A + M ;
				V = (A[7]!=t[7]);
				N =  A[7].to_bool();
				Z = (t==0);
				if (D.to_bool()){
					t = 10*(A.range(7,4).to_uint() + M.range(7,4)) + A.range(3,0).to_uint() + M.range(3,0) + C.to_char();
					C = (t>99);
					}
				else{
					C = (t>255);
					A = t & 0xFF  ;
					}
				return;
				
			case inst_adc_imm:
				M = get((PC.range(15,0)<<8) + op1);
				t = C.to_bool() ? A + M + 1 :  A + M ;
				V = (A[7]!=t[7]);
				N =  A[7].to_bool();
				Z = (t==0);
				if (D.to_bool()){
					t = 10*(A.range(7,4).to_uint() + M.range(7,4)) + A.range(3,0).to_uint() + M.range(3,0) + C.to_char();
					C = (t>99);
					}
				else{
					C = (t>255);
					A = t & 0xFF  ;
					}
				return;
	
			case inst_brk_imp:
				PC = PC + 1;
				set(SP, PC.range(15,8));
				SP = SP - 1;
				set(SP, PC.range(7,0));
				SP = SP - 1;
				set(SP, P.to_uint()|0x10);
				SP = SP - 1;
				l = get(0xFFFE);
				h = get(0xFFFF)<<8;
				PC = h|l;
				return;	
			
			case inst_cpx_imm:
				t = op1 - X;
				N = t[7].to_bool();
				C = (X>=op1);
				Z = (t==0); 
				return;
			
			case inst_cpy_imm:
				t = op1 - Y;
				N = t[7].to_bool();
				C = (Y>=op1);
				Z = (t==0); 

			case inst_clc_imp:
				C = 0 ;
				return;
				
			case inst_and_imm:
				A = A & op1;
				N = A[7].to_bool();
				Z = (A==0);
				return;
			
			case inst_eor_imm:
				A = A ^ op1;
				N = A[7].to_bool();
				Z = (A==0); 
				return;
			
			case inst_asl_a:
				C = A[7].to_bool();
				A = (A << 1) & 0xFE;
				N = A[7].to_bool();
				Z = (A==0);
				return;
				
			case inst_asl_abs:
				M = get((op2<<8) + op1);
				C = M[7].to_bool();
				M = (M << 1) & 0xFE;
				N = M[7].to_bool();
				Z = (M==0);
				return;
			
			case inst_rol_a:
				temp = A[7].to_bool();
				A = (A << 1) & 0xFE;
				A = A | C.to_char();
				C = temp;
				Z = (A==0);
				N = A[7].to_bool();    
				return;
				
			case inst_rol_abs:
				M = get((op2<<8) + op1);
				temp = A[7].to_bool();
				M = (M << 1) & 0xFE;
				M = M | C.to_char();
				C = temp;
				Z = (A==0);
				N = A[7].to_bool(); 
				set((op2<<8) + op1, M);
				return;
				
			case inst_bcc_rel:
				REL = IR.range(15, 8).to_int();
				if (C == 0)  PC = PC + REL;
				return;
				
			case inst_bcs_rel:
				REL = IR.range(15, 8).to_int();
				if (C == 0)  PC = PC + REL;
				return;
	
			case inst_beq_rel:
				REL = IR.range(15, 8).to_int();
				if (Z == 1)  PC = PC + REL;
				return;
				
			case inst_bne_rel:
				REL = IR.range(15, 8).to_int();
				if (Z == 0)  PC = PC + REL;
				return;
				
			case inst_cmp_imm:
				t = op1 - A;
				N = t[7].to_bool();
				C = (A>=M);
				Z = (t==0);

			case inst_dex_imp:
				X--;
				Z = X==0;
				N = X[7].to_bool();
				return;
			
			case inst_dey_imp:
				Y--;
				Z = Y==0;
				N = Y[7].to_bool();
				return;
				
			case inst_inx_imp:
				X++;
				Z = X==0;
				N = X[7].to_bool();
				return;
	
			case inst_iny_imp:
				Y++;
				Z = Y==0;
				N = Y[7].to_bool();
				return;
	
			case inst_inc_abs:
				M = get((op2 << 8)+op1) + 1;
				set((op2 << 8)+op1, M);
				N = M[7].to_bool();
				Z = M.to_uint() == 0;
				return;
	
			case inst_jmp_abs:
				PC = (op2 << 8) + op1;
				return;
	
			case inst_jsr_abs:
				t = PC-1;
				set(SP, t.range(15,8));
				SP--;
				set(SP, t.range(7,0));
				SP--;
				PC = (op2 << 8) + op1;
				return;
	
			case inst_rts_imp:
				SP++;
				l = get(SP);
				SP++;
				h = get(SP);
				PC = (h << 8) | l + 1;
				return;
			
			case inst_lda_imm:
				A = op1;
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;
	
			case inst_lda_abs:
				A = get((op2<<8) + op1);;
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;
			
			case inst_lda_abs_x:
				A = get((op2<<8) + op1 + X.to_uint());
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;
	
			case inst_ldx_imm:
				X = op1;
				N = X[7].to_bool();
				Z = (X.to_uint() == 0);
				return;
	
			case inst_ldx_abs:
				X = get((op2<<8) + op1);
				N = X[7].to_bool();
				Z = (X.to_uint() == 0);
				return;
	
			case inst_ldy_imm:
				Y = op1;
				N = Y[7].to_bool();
				Z = (Y.to_uint() == 0);
				return;
	
			case inst_pla_imp:
				SP++;
				A = get(SP.to_uint());
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;
				
			case inst_pha_imp:
				set(SP, A.to_uint());
				SP = SP-1;
				return;
				
			case inst_sta_abs:
				set((op2<<8) + op1, A);
				return;
			
			case inst_sta_abs_x:
				set((op2<<8) + op1 + X.to_uint(), A);
				return;
			
			case inst_stx_abs:
				set((op2<<8) + op1, X);
				return;

			case inst_sty_abs:
				set((op2<<8) + op1, Y);
				return;
			
			case inst_tax_imp:
				X = A;
				N = X[7].to_bool();
				Z = (X.to_uint() == 0);
				return;
			
			case inst_tay_imp:
				Y = A;
				N = Y[7].to_bool();
				Z = (Y.to_uint() == 0);
				return;
			
			case inst_txa_imp:
				A = X;
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;
			
			case inst_tya_imp:
				A = Y;
				N = A[7].to_bool();
				Z = (A.to_uint() == 0);
				return;

		};
		
		
	}
	
	unsigned int getFetchCount(unsigned int op){
		int i;
		for(i=0; i<40; i++){
			if(op == inst_type[i])
				break;
		}
		if(i < 15) return 1;
		if(i < 28) return 2;
		return 3;
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