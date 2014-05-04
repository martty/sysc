#ifndef SYSCVIDEO_H
#define SYSCVIDEO_H

#include <systemc.h>
#include <SDL/SDL.h>
#include <deque>

// SysCVideo 0.2

SC_MODULE(SysCVideo) {
    // Konfiguráció
    enum {
        ParancsRegCim = 0x0,
        KurzorXRegCim = 0x1,
        KurzorYRegCim = 0x2,
        KarakterIrCim = 0x3,
        BillentyuPufCim = 0x4,
        BillentyuPuf256Cim = 0x5,
        
        BillentyuMatrCim=0x100,
        SzovegesCim=0x400,
        GrafikusCim=0x800,
        
        MeretX=40, MeretY=25,
        PixSize=16,     // csak a font (437.bmp) lecserélése mellett állítható
    };

    // Portok
    sc_in<sc_lv<12> > address;
    sc_inout_rv<8> data;
    sc_in<bool> readneg;
    sc_in<bool> writeneg;
    sc_in<bool> csneg;

    SC_CTOR(SysCVideo) {
        init();
    }

    unsigned char regiszterolvas(unsigned address);
    void regiszterir(unsigned address, unsigned char data);

    // belső állapot - SDL megjelenítéshez
    SDL_Surface *screen;
    SDL_Surface *vgafont;
    
    // grafikus kártya állapota 
    bool grafikusmod;
    unsigned char kurzorx, kurzory;
    unsigned char grafikus[MeretY][MeretX];
    unsigned char szoveges[MeretY][MeretX];
    unsigned char billentyu[SDLK_LAST/8+1];
    std::deque<unsigned> billentyupuf;
    unsigned lastkeyread;

    // segédfüggvények
    void init();
    void rajzol();
    void szoveges_mod();
    void grafikus_mod();
    void szoveges_torol();
    void kurzor_balfent();
    void grafikus_torol();
    void szoveges_leptet_fel();
    void karakter_ir(unsigned char ch);
    
    // szálak
    void utemez();
    void vezerel();
};

#endif
