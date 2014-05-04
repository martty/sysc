#include <systemc.h>
#include "syscvideo.h"
#include <list>

// testbench, amely meghajtja a syscvideo modult
SC_MODULE(SysCVideoTest) {
    sc_out<sc_lv<12> > address;
    sc_inout_rv<8> data;
    sc_out<bool> readneg;
    sc_out<bool> writeneg;
    sc_out<bool> csneg;
    
    sc_out<bool> quit;

    void regiszter_ir(unsigned addr, unsigned char byt)
    {
        address.write(addr);
        csneg.write(0);
        data.write(byt);
        wait(1, SC_US);
        writeneg.write(0);
        wait(1, SC_US);
        writeneg.write(1);
        address.write("ZZZZZZZZZZZZ");
        data.write("ZZZZZZZZ");
        csneg.write(1);
        wait(1, SC_US);
    }

    void parancsot_kuld(unsigned char command)
    {
        regiszter_ir(SysCVideo::ParancsRegCim, command);
    }

    unsigned char regiszter_olvas(unsigned addr)
    {
        address.write(addr);
        csneg.write(0);
        wait(1, SC_US);
        readneg.write(0);
        wait(1, SC_US);
        unsigned char read = data.read().to_uint();
        readneg.write(1);
        address.write("ZZZZZZZZZZZZ");
        csneg.write(1);
        wait(1, SC_US);

        return read;
    }

    // Kukacos játék. A vezérlő modult
    // a konstruktora veszi át. A játék a jatek()
    // tagfüggvénnyel fut le.
    class Kukac {
        SysCVideoTest &testbench;
    public:
        Kukac(SysCVideoTest &testbench): testbench(testbench) {}

    private:
        enum Irany { fel, jobbra, le, balra };
        struct KukacPont { int x, y; };
        enum Rajz { ures=18, kaja=12, fal=7, kukac=10, utkozes=4 }; // szinek

        std::list<KukacPont> kukaclista;
        Irany elozoirany, irany;    /* merre mozog epp */
        int novekszik;              /* hany egyseggel kell meg novekedjen */
        int kajax, kajay;           /* a kovetkezo kaja */

        void rajzol(int x, int y, Rajz sz) {
            testbench.regiszter_ir(SysCVideo::GrafikusCim+y*SysCVideo::MeretX+x, sz);
        }
         
        void kukac_ujpont(int x, int y)
        {
            KukacPont uj={x, y};
            kukaclista.push_front(uj);
            rajzol(x, y, kukac);        /* rajzol */
        }
         
        bool kukac_resze_e(int x, int y) {
            for (std::list<KukacPont>::const_iterator it=kukaclista.begin(); it!=kukaclista.end(); ++it)
                if (it->x==x && it->y==y)
                    return true;
            return false;
        }
         
        void uj_kaja() {
            do {
                kajax=rand()%(SysCVideo::MeretX-2)+1;       /* falra ne */
                kajay=rand()%(SysCVideo::MeretY-2)+1;
            } while (kukac_resze_e(kajax, kajay));
         
            rajzol(kajax, kajay, kaja);
        }
         
        /* kukac mozgatasa. az uj irany kiszamolasa utan
         * ellenorzi az utkozest; ha vege a jateknak, igazzal ter vissza.
         * ha nem, akkor rajzol, es uj kajat rak le, ha kell. */
        bool kukac_mozog() {
            int const dx[]={0, 1, 0, -1}; // enum irany szerinti sorrend (indexeles!)
            int const dy[]={-1, 0, 1, 0};
            int ujx, ujy;
         
            /* ezt azert jegyzi meg, hogy ne engedje sajat magara
             * hatrafordulni az iranyitasnal. */
            elozoirany=irany;
         
            ujx=kukaclista.front().x+dx[irany];
            ujy=kukaclista.front().y+dy[irany];
            if (ujx==0 || ujx==SysCVideo::MeretX-1
                || ujy==0 || ujy==SysCVideo::MeretY-1     /* falnak utkozik? */
                || kukac_resze_e(ujx, ujy)) {           /* maganak utkozik? */
                rajzol(ujx, ujy, utkozes);
                return true;
            }
            kukac_ujpont(ujx, ujy);                /* "fej" kirajzolasa */
         
            if (ujx==kajax && ujy==kajay) {         /* ha megkajalta */
                uj_kaja();
                novekszik+=5;
            }
            if (novekszik>0)         /* ha no, nem toroljuk a veget */
                novekszik--;
            else {                   /* amugy igen (ha nem 0 hosszu) */
                if (!kukaclista.empty()) {
                    KukacPont &utolso = kukaclista.back();
                    rajzol(utolso.x, utolso.y, ures);
                    kukaclista.pop_back();
                }
            }
            return false;
        }
        
    public:
        void jatek() {
            testbench.parancsot_kuld(1);    /* grafikus mód set */

            /* palya kirajzolasa - falak */
            /* falak */
            for (int y=0; y<SysCVideo::MeretY; y++)
                for (int x=0; x<SysCVideo::MeretX; x++)
                    rajzol(x, y, y==0||x==0||y==SysCVideo::MeretY-1||x==SysCVideo::MeretX-1 ? fal : ures);
         
            kukac_ujpont(SysCVideo::MeretX/2, SysCVideo::MeretY/2);
            elozoirany=irany=jobbra;
            novekszik=3;
            uj_kaja();
         
            bool jatek_vege=false;
            while (!jatek_vege) {
                unsigned key=testbench.regiszter_olvas(SysCVideo::BillentyuPufCim) + testbench.regiszter_olvas(SysCVideo::BillentyuPuf256Cim)*256;
                
                switch (key) {
                    case SDLK_UP: if (irany!=le) irany=fel; break;
                    case SDLK_DOWN: if (irany!=fel) irany=le; break;
                    case SDLK_LEFT: if (irany!=jobbra) irany=balra; break;
                    case SDLK_RIGHT: if (irany!=balra) irany=jobbra; break;
                }

                jatek_vege=kukac_mozog();
                testbench.wait(120, SC_MS);
            }
        }
    };
    
    void test() {
        quit.write(false);

        address.write("ZZZZZZZZZZZZ");
        data.write("ZZZZZZZZ");
        readneg.write(true);
        writeneg.write(true);
        csneg.write(true);
        
        wait(40, SC_MS);

        parancsot_kuld(0);    /* szöveges mód set */
        parancsot_kuld(2);    /* clear screen (szöveges) */
        parancsot_kuld(3);    /* crsr home */
        
        /* szöveg kiírása */
        char const *str="Hello vilag!\n\nVirtualis grafikus kartya\n\n";
        for (int i=0; str[i]!=0; ++i)
            regiszter_ir(SysCVideo::KarakterIrCim, str[i]);
        
        /* kiirja a szoveges kepernyo elso sorat a konzolra */
        std::cout<<"Memoria visszaolvasas: |";
        for (int x=0; x<40; ++x)
            std::cout << (char) regiszter_olvas(SysCVideo::SzovegesCim+x);
        std::cout<<"|\n";

        str="\n\n\n\nGepelj valamit (enterig): ";
        for (int i=0; str[i]!=0; ++i)
            regiszter_ir(SysCVideo::KarakterIrCim, str[i]);

        while (true) {
            unsigned key=regiszter_olvas(SysCVideo::BillentyuPufCim);
            key += regiszter_olvas(SysCVideo::BillentyuPuf256Cim)*256;
            if (key==SDLK_RETURN)
                break;
            if (key!=0)
                regiszter_ir(SysCVideo::KarakterIrCim, key);
            wait(40, SC_MS);
        }

        str="\n\n\nMindjart valtok grafikus modba.\n\nEgy kukacos jatek lesz,\n\namit a nyilakkal tudsz iranyitani.\n";
        for (int i=0; str[i]!=0; ++i)
            regiszter_ir(SysCVideo::KarakterIrCim, str[i]);
        for (int i=3; i>=0; --i) {
            regiszter_ir(SysCVideo::KarakterIrCim, i+'0');
            wait(1000, SC_MS);
        }

        Kukac k(*this);
        k.jatek();
        
        wait(3000, SC_MS);
        quit.write(true);
    };
    
    SC_CTOR(SysCVideoTest) {
        SC_THREAD(test);
    };
};
    
// Main program
int sc_main(int argc, char* argv[])
{
    sc_set_time_resolution(1, SC_US);

    /* szignálok */
    sc_signal<sc_lv<12> > address("addrsig");
    sc_signal_rv<8> data("datasig");
    sc_signal<bool> readneg("readnegsig");
    sc_signal<bool> writeneg("writenegsig");
    sc_signal<bool> csneg("csnegsig");

    sc_signal<bool> quit;   // ez a madzag a testbenchbol jon, ha true, akkor vege

    /* példányosítás és port binding */
    SysCVideo gk("gk");
    gk(address, data, readneg, writeneg, csneg);
    
    SysCVideoTest tester("tester");
    tester(address, data, readneg, writeneg, csneg, quit);
    
    /* a testbench ad egy quit=truet amikor vegzett */
    while (quit != true) {
        sc_start(20, SC_MS);
        SDL_Delay(20);
    }
    
    return 0;
}
