#ifndef DC_BEHAVIORAL
#define DC_BEHAVIORAL
//-------------------------------------------------------------------------------------------------
#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <set>
#include <map>
//-------------------------------------------------------------------------------------------------
struct DemoCore_Behavioral: public sc_module {
  
  // ports -----------------------------
  sc_in   < bool >        Clk;
  sc_in   < sc_lv<1> >    Reset;
  sc_in   < sc_lv<1> >    Start;
  //------------------------------------
  sc_out  < sc_lv<1> >    Read;
  sc_out  < sc_lv<1> >    Write;
  sc_out  < sc_lv<8> >    Addr2DataMem;
  sc_out  < sc_lv<8> >    Data2DataMem;
  sc_in   < sc_lv<8> >    DataFromDataMem;
  //------------------------------------
  sc_out  < sc_lv<8> >    Addr2PrMem;
  sc_in   < sc_lv<15> >   DataFromPrMem;
  //------------------------------------
  sc_out  < sc_lv<1> >    Abort;

  // enumeration type for FSM states ---
  enum FSMStates {WaitForStart,Fetch,Decode,
                  state_mov,state_movi,
                  state_sdm1,state_sdm2,state_ldm1,state_ldm2,
                  state_add,state_sub,
                  state_and,state_or,state_xor,
                  state_j,state_jz,state_jnz,
                  state_abort,
                  Error};
  FSMStates State;
  // instruction set -------------------
  static const unsigned inst_mov = 0;     // GPRF[IR[10:8]] <= GPRF[IR[2:0]]
  static const unsigned inst_movi = 1;    // GPRF[IR[10:8]] <= IR[7:0]
  static const unsigned inst_sdm = 2;     // DATAMEM[IR[7:0]] <= GPRF[IR[10:8]]
  static const unsigned inst_ldm = 3;     // GPRF[IR[10:8]] <= DATAMEM[IR[7:0]]
  static const unsigned inst_add = 4;     // GPRF[IR[10:8]] <= GPRF[IR[5:3]] + GPRF[IR[2:0]]; Zero flag
  static const unsigned inst_sub = 5;     // GPRF[IR[10:8]] <= GPRF[IR[5:3]] - GPRF[IR[2:0]]; Zero flag
  static const unsigned inst_and = 6;     // GPRF[IR[10:8]] <= GPRF[IR[5:3]] & GPRF[IR[2:0]]; Zero flag
  static const unsigned inst_or = 7;      // GPRF[IR[10:8]] <= GPRF[IR[5:3]] | GPRF[IR[2:0]]; Zero flag
  static const unsigned inst_xor = 8;     // GPRF[IR[10:8]] <= GPRF[IR[5:3]] ^ GPRF[IR[2:0]]; Zero flag
  static const unsigned inst_j = 9;       // IP <= IR[7:0]
  static const unsigned inst_jz = 10;     // if Zero = 1 then IP <= IR[7:0] else no operation
  static const unsigned inst_jnz = 11;    // if Zero = 0 then IP <= IR[7:0] else no operation
  static const unsigned inst_abort = 12;  // stop the execution
  // internal resources ----------------
  sc_lv<15>     IR;       // register:     oooo ddd xx sss sss    
                          // memory:       oooo ddd aaaaaaaa
                          // jump:         oooo xxx aaaaaaaa
  sc_lv<8>      IP;       // instruction pointer
  sc_lv<1>      Zero;     // zero flag
  sc_lv<8>      MAR;      // memory address register
  sc_lv<8>      MDR;      // memory data register
  sc_lv<8>      GPRF[8];  // general-purpose register file

  // statistical analysis utilities ----
  std::map<std::string,unsigned> instructionOccurence;
  std::set<unsigned> addressSet_DataMem;
  //-----------------------------------------------------------------------------------------------
  void FunctionThread() {
    while (1) {
      if (Reset.read() == "1") {
        // reset all outputs -----------
        Read.write("0");
        Write.write("0");
        Addr2DataMem.write("00000000");
        Data2DataMem.write("00000000");
        Addr2PrMem.write("00000000");
        Abort.write("0");
        // reset all internal variables
        State = WaitForStart;
        IR = 0;
        IP = 0;
        Zero = 0;
        MAR = 0;
        MDR = 0;
        for(unsigned i = 0;i<8;++i) { GPRF[i] = 0; }
      }
      else {
        switch (State) {
          case WaitForStart:    if ( Start.read() == "1" ) State = Fetch;
                                else State = WaitForStart;
                                break;
          case Fetch:           IR = DataFromPrMem.read();
                                State = Decode;
                                break;
          case Decode:          switch (IR(14,11).to_uint()) {
                                  case inst_mov:    State = state_mov; instructionOccurence["mov"]++; break;
                                  case inst_movi:   State = state_movi; instructionOccurence["movi"]++; break;
                                  case inst_sdm:    State = state_sdm1; instructionOccurence["sdm"]++; break;
                                  case inst_ldm:    State = state_ldm1; instructionOccurence["ldm"]++; break;
                                  case inst_add:    State = state_add; instructionOccurence["add"]++; break;
                                  case inst_sub:    State = state_sub; instructionOccurence["sub"]++; break;
                                  case inst_and:    State = state_and; instructionOccurence["and"]++; break;
                                  case inst_or:     State = state_or; instructionOccurence["or"]++; break;
                                  case inst_xor:    State = state_xor; instructionOccurence["xor"]++; break;
                                  case inst_j:      State = state_j; instructionOccurence["j"]++; break;
                                  case inst_jz:     State = state_jz; instructionOccurence["jz"]++; break;
                                  case inst_jnz:    State = state_jnz; instructionOccurence["jnz"]++; break;
                                  case inst_abort:  State = state_abort; instructionOccurence["abort"]++; break;
                                  default:          State = Error; break;
                                }
                                break;
          //--------------------------------------------------------------
          case state_mov:       GPRF[IR(10,8).to_uint()] = GPRF[IR(2,0).to_uint()];
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_movi:      GPRF[IR(10,8).to_uint()] = IR(7,0);
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_sdm1:      MAR = IR(7,0); addressSet_DataMem.insert(MAR.to_uint());
                                MDR = GPRF[IR(10,8).to_uint()];
                                Write.write("1");
                                State = state_sdm2;
                                break;
          case state_sdm2:      Write.write("0");
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_ldm1:      MAR = IR(7,0); addressSet_DataMem.insert(MAR.to_uint());
                                Read.write("1");
                                State = state_ldm2;
                                break;
          case state_ldm2:      GPRF[IR(10,8).to_uint()] = DataFromDataMem.read();
                                Read.write("0"); 
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_add:       GPRF[IR(10,8).to_uint()] = static_cast< sc_lv<8> >(GPRF[IR(5,3).to_uint()].to_uint() + GPRF[IR(2,0).to_uint()].to_uint());
                                if ( GPRF[IR(10,8).to_uint()] == 0 ) Zero = "1"; else Zero = "0";
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_sub:       GPRF[IR(10,8).to_uint()] = static_cast< sc_lv<8> >(GPRF[IR(5,3).to_uint()].to_uint() - GPRF[IR(2,0).to_uint()].to_uint());
                                if ( GPRF[IR(10,8).to_uint()] == 0 ) Zero = "1"; else Zero = "0";
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_and:       GPRF[IR(10,8).to_uint()] = GPRF[IR(5,3).to_uint()] & GPRF[IR(2,0).to_uint()];
                                if ( GPRF[IR(10,8).to_uint()] == 0 ) Zero = "1"; else Zero = "0";
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_or:        GPRF[IR(10,8).to_uint()] = GPRF[IR(5,3).to_uint()] | GPRF[IR(2,0).to_uint()];
                                if ( GPRF[IR(10,8).to_uint()] == 0 ) Zero = "1"; else Zero = "0";
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_xor:       GPRF[IR(10,8).to_uint()] = GPRF[IR(5,3).to_uint()] ^ GPRF[IR(2,0).to_uint()];
                                if ( GPRF[IR(10,8).to_uint()] == 0 ) Zero = "1"; else Zero = "0";
                                IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_j:         IP = IR(7,0);
                                State = Fetch;
                                break;
          case state_jz:        if (Zero == "1") IP = IR(7,0); else IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_jnz:       if (Zero == "0") IP = IR(7,0); else IP = static_cast< sc_lv<8> >(IP.to_uint() + 1);
                                State = Fetch;
                                break;
          case state_abort:     Abort.write("1");
                                std::cout<<endl<<endl<<"Instruction occurence:"<<endl;
                                for(std::map<std::string,unsigned>::iterator p = instructionOccurence.begin();p != instructionOccurence.end();++p) {
                                  std::cout<<std::setw(8)<<std::left<<(*p).first<<" "<<(*p).second<<endl;
                                }
                                std::cout<<endl<<endl;
                                std::cout<<"Data memory utilization:"<<endl;
                                for(std::set<unsigned>::iterator p = addressSet_DataMem.begin();p != addressSet_DataMem.end();++p) {
                                  std::cout<<(*p)<<endl;
                                }
                                State = Error;
                                break;
          //--------------------------------------------------------------
          case Error:          State = Error; break;
          default:             State = Error; break;
        }
      }
      // output port assignments -------
      Addr2DataMem.write(MAR);
      Data2DataMem.write(MDR);
      Addr2PrMem.write(IP);
      wait();
    }
  }
  //-----------------------------------------------------------------------------------------------
  // contructor ------------------------
  SC_CTOR(DemoCore_Behavioral) {
    SC_THREAD(FunctionThread); sensitive << Clk.pos();
    // initialize outputs --------------
    Read.initialize("0");
    Write.initialize("0");
    Addr2DataMem.initialize("00000000");
    Data2DataMem.initialize("00000000");
    Addr2PrMem.initialize("00000000");
    Abort.initialize("0");
    // initialize internal variables ---
    State = WaitForStart;
    IR = 0;
    IP = 0;
    Zero = 0;
    MAR = 0;
    MDR = 0;
    for(unsigned i = 0;i<8;++i) { GPRF[i] = 0; }
    instructionOccurence["mov"] = 0;
    instructionOccurence["movi"] = 0;
    instructionOccurence["sdm"] = 0;
    instructionOccurence["ldm"] = 0;
    instructionOccurence["add"] = 0;
    instructionOccurence["sub"] = 0;
    instructionOccurence["and"] = 0;
    instructionOccurence["or"] = 0;
    instructionOccurence["xor"] = 0;
    instructionOccurence["j"] = 0;
    instructionOccurence["jz"] = 0;
    instructionOccurence["jnz"] = 0;
    instructionOccurence["abort"] = 0;
  }

};
//-------------------------------------------------------------------------------------------------
#endif; /* DC_BEHAVIORAL */
//-------------------------------------------------------------------------------------------------