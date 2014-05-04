#include <SDL/SDL.h>
#include "syscvideo.h"

static unsigned char vga437[] = {
#include "437.h"
};

static struct rgbval {
    unsigned char r, g, b;
} palette[] = {
    { 0, 0, 0 }, { 0, 0, 42 }, { 0, 42, 0 }, { 0, 42, 42 }, 
    { 42, 0, 0 }, { 42, 0, 42 }, { 42, 21, 0 }, { 42, 42, 42 }, 
    { 21, 21, 21 }, { 21, 21, 63 }, { 21, 63, 21 }, { 21, 63, 63 }, 
    { 63, 21, 21 }, { 63, 21, 63 }, { 63, 63, 21 }, { 63, 63, 63 }, 
    { 0, 0, 0 }, { 5, 5, 5 }, { 8, 8, 8 }, { 11, 11, 11 }, 
    { 14, 14, 14 }, { 17, 17, 17 }, { 20, 20, 20 }, { 24, 24, 24 }, 
    { 28, 28, 28 }, { 32, 32, 32 }, { 36, 36, 36 }, { 40, 40, 40 }, 
    { 45, 45, 45 }, { 50, 50, 50 }, { 56, 56, 56 }, { 63, 63, 63 }, 
    { 0, 0, 63 }, { 16, 0, 63 }, { 31, 0, 63 }, { 47, 0, 63 }, 
    { 63, 0, 63 }, { 63, 0, 47 }, { 63, 0, 31 }, { 63, 0, 16 }, 
    { 63, 0, 0 }, { 63, 16, 0 }, { 63, 31, 0 }, { 63, 47, 0 }, 
    { 63, 63, 0 }, { 47, 63, 0 }, { 31, 63, 0 }, { 16, 63, 0 }, 
    { 0, 63, 0 }, { 0, 63, 16 }, { 0, 63, 31 }, { 0, 63, 47 }, 
    { 0, 63, 63 }, { 0, 47, 63 }, { 0, 31, 63 }, { 0, 16, 63 }, 
    { 31, 31, 63 }, { 39, 31, 63 }, { 47, 31, 63 }, { 55, 31, 63 }, 
    { 63, 31, 63 }, { 63, 31, 55 }, { 63, 31, 47 }, { 63, 31, 39 }, 
    { 63, 31, 31 }, { 63, 39, 31 }, { 63, 47, 31 }, { 63, 55, 31 }, 
    { 63, 63, 31 }, { 55, 63, 31 }, { 47, 63, 31 }, { 39, 63, 31 }, 
    { 31, 63, 31 }, { 31, 63, 39 }, { 31, 63, 47 }, { 31, 63, 55 }, 
    { 31, 63, 63 }, { 31, 55, 63 }, { 31, 47, 63 }, { 31, 39, 63 }, 
    { 45, 45, 63 }, { 49, 45, 63 }, { 54, 45, 63 }, { 58, 45, 63 }, 
    { 63, 45, 63 }, { 63, 45, 58 }, { 63, 45, 54 }, { 63, 45, 49 }, 
    { 63, 45, 45 }, { 63, 49, 45 }, { 63, 54, 45 }, { 63, 58, 45 }, 
    { 63, 63, 45 }, { 58, 63, 45 }, { 54, 63, 45 }, { 49, 63, 45 }, 
    { 45, 63, 45 }, { 45, 63, 49 }, { 45, 63, 54 }, { 45, 63, 58 }, 
    { 45, 63, 63 }, { 45, 58, 63 }, { 45, 54, 63 }, { 45, 49, 63 }, 
    { 0, 0, 28 }, { 7, 0, 28 }, { 14, 0, 28 }, { 21, 0, 28 }, 
    { 28, 0, 28 }, { 28, 0, 21 }, { 28, 0, 14 }, { 28, 0, 7 }, 
    { 28, 0, 0 }, { 28, 7, 0 }, { 28, 14, 0 }, { 28, 21, 0 }, 
    { 28, 28, 0 }, { 21, 28, 0 }, { 14, 28, 0 }, { 7, 28, 0 }, 
    { 0, 28, 0 }, { 0, 28, 7 }, { 0, 28, 14 }, { 0, 28, 21 }, 
    { 0, 28, 28 }, { 0, 21, 28 }, { 0, 14, 28 }, { 0, 7, 28 }, 
    { 14, 14, 28 }, { 17, 14, 28 }, { 21, 14, 28 }, { 24, 14, 28 }, 
    { 28, 14, 28 }, { 28, 14, 24 }, { 28, 14, 21 }, { 28, 14, 17 }, 
    { 28, 14, 14 }, { 28, 17, 14 }, { 28, 21, 14 }, { 28, 24, 14 }, 
    { 28, 28, 14 }, { 24, 28, 14 }, { 21, 28, 14 }, { 17, 28, 14 }, 
    { 14, 28, 14 }, { 14, 28, 17 }, { 14, 28, 21 }, { 14, 28, 24 }, 
    { 14, 28, 28 }, { 14, 24, 28 }, { 14, 21, 28 }, { 14, 17, 28 }, 
    { 20, 20, 28 }, { 22, 20, 28 }, { 24, 20, 28 }, { 26, 20, 28 }, 
    { 28, 20, 28 }, { 28, 20, 26 }, { 28, 20, 24 }, { 28, 20, 22 }, 
    { 28, 20, 20 }, { 28, 22, 20 }, { 28, 24, 20 }, { 28, 26, 20 }, 
    { 28, 28, 20 }, { 26, 28, 20 }, { 24, 28, 20 }, { 22, 28, 20 }, 
    { 20, 28, 20 }, { 20, 28, 22 }, { 20, 28, 24 }, { 20, 28, 26 }, 
    { 20, 28, 28 }, { 20, 26, 28 }, { 20, 24, 28 }, { 20, 22, 28 }, 
    { 0, 0, 16 }, { 4, 0, 16 }, { 8, 0, 16 }, { 12, 0, 16 }, 
    { 16, 0, 16 }, { 16, 0, 12 }, { 16, 0, 8 }, { 16, 0, 4 }, 
    { 16, 0, 0 }, { 16, 4, 0 }, { 16, 8, 0 }, { 16, 12, 0 }, 
    { 16, 16, 0 }, { 12, 16, 0 }, { 8, 16, 0 }, { 4, 16, 0 }, 
    { 0, 16, 0 }, { 0, 16, 4 }, { 0, 16, 8 }, { 0, 16, 12 }, 
    { 0, 16, 16 }, { 0, 12, 16 }, { 0, 8, 16 }, { 0, 4, 16 }, 
    { 8, 8, 16 }, { 10, 8, 16 }, { 12, 8, 16 }, { 14, 8, 16 }, 
    { 16, 8, 16 }, { 16, 8, 14 }, { 16, 8, 12 }, { 16, 8, 10 }, 
    { 16, 8, 8 }, { 16, 10, 8 }, { 16, 12, 8 }, { 16, 14, 8 }, 
    { 16, 16, 8 }, { 14, 16, 8 }, { 12, 16, 8 }, { 10, 16, 8 }, 
    { 8, 16, 8 }, { 8, 16, 10 }, { 8, 16, 12 }, { 8, 16, 14 }, 
    { 8, 16, 16 }, { 8, 14, 16 }, { 8, 12, 16 }, { 8, 10, 16 }, 
    { 11, 11, 16 }, { 12, 11, 16 }, { 13, 11, 16 }, { 15, 11, 16 }, 
    { 16, 11, 16 }, { 16, 11, 15 }, { 16, 11, 13 }, { 16, 11, 12 }, 
    { 16, 11, 11 }, { 16, 12, 11 }, { 16, 13, 11 }, { 16, 15, 11 }, 
    { 16, 16, 11 }, { 15, 16, 11 }, { 13, 16, 11 }, { 12, 16, 11 }, 
    { 11, 16, 11 }, { 11, 16, 12 }, { 11, 16, 13 }, { 11, 16, 15 }, 
    { 11, 16, 16 }, { 11, 15, 16 }, { 11, 13, 16 }, { 11, 12, 16 }, 
    { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 
    { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 63, 63, 63 }
};

// Ez a tulajdonképpeni konstruktor
void SysCVideo::init()
{
    // SDL inicializálása
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(MeretX*PixSize+4*PixSize, MeretY*PixSize+4*PixSize, 0, SDL_ANYFORMAT);
    SDL_WM_SetCaption("SysCVideo", NULL);
    // keret
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 64, 64, 192));
    
    SDL_RWops *rw = SDL_RWFromMem(vga437, sizeof(vga437));
    SDL_Surface *temp = SDL_LoadBMP_RW(rw, 1);
    vgafont = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    szoveges_mod();

    // SystemC modul dolgok
    SC_METHOD(vezerel);
    dont_initialize();
    sensitive << readneg << writeneg;
    
    SC_THREAD(utemez);
}

void SysCVideo::rajzol()
{
    // Kirajzolás
    if (grafikusmod) {
        // grafikus
        for (int y = 0; y<MeretY; ++y)
            for (int x = 0; x<MeretX; ++x) {
                SDL_Rect r = { Sint16(x*PixSize+2*PixSize), Sint16(y*PixSize+2*PixSize), PixSize, PixSize};
                unsigned char szin = grafikus[y][x];
                Uint32 v = SDL_MapRGB(screen->format, palette[szin].r<<2, palette[szin].g<<2, palette[szin].b<<2);
                SDL_FillRect(screen, &r, v);
            }
    } else {
        // szoveges
        for (int y = 0; y<MeretY; ++y)
            for (int x = 0; x<MeretX; ++x) {
                unsigned char c = szoveges[y][x];
                SDL_Rect src = { Sint16(c%16*PixSize), Sint16(c/16*PixSize), PixSize, PixSize}, dst = { Sint16(x*PixSize+2*PixSize), Sint16(y*PixSize+2*PixSize), 0, 0};
                SDL_BlitSurface(vgafont, &src, screen, &dst);
            }
        // kurzor
        static int villog = 0;
        villog = (villog+1) % 20;
        if (kurzorx>=0 && kurzorx<MeretX && kurzory>=0 && kurzory<MeretY && villog<10) {
            SDL_Rect r = { Sint16(kurzorx*PixSize+2*PixSize+2), Sint16(kurzory*PixSize+2*PixSize+PixSize-3), PixSize-4, 2};
            SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 255, 0, 255));
        }
    }
    SDL_Flip(screen);

    // Billentyűzet ellenőrzése
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        unsigned index, bit;

        switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym >= SDLK_LAST)
                    break;
                index = event.key.keysym.sym >> 3;
                bit = event.key.keysym.sym & 7;
                billentyu[index] |=  1<<bit;
                billentyupuf.push_back(event.key.keysym.sym);
                break;
            
            case SDL_KEYUP:
                if (event.key.keysym.sym >= SDLK_LAST)
                    break;
                index = event.key.keysym.sym >> 3;
                bit = event.key.keysym.sym & 7;
                billentyu[index] &=  ~(1<<bit);
                break;
            
            case SDL_QUIT:
                exit(1);
                break;
        }
    }
}

void SysCVideo::szoveges_mod()
{
    grafikusmod = false;
}

void SysCVideo::grafikus_mod()
{
    grafikusmod = true;
}

void SysCVideo::szoveges_torol()
{
    for (int y = 0; y<MeretY; ++y)
        for (int x = 0; x<MeretX; ++x)
            szoveges[y][x] = ' ';
}

void SysCVideo::kurzor_balfent()
{
    kurzorx = 0;
    kurzory = 0;
}

void SysCVideo::grafikus_torol()
{
    for (int y = 0; y<MeretY; ++y)
        for (int x = 0; x<MeretX; ++x)
            grafikus[y][x] = 0;
}

void SysCVideo::szoveges_leptet_fel()
{
    for (int y = 0; y<MeretY-1; ++y)
        for (int x = 0; x<MeretX; ++x)
            szoveges[y][x] = szoveges[y+1][x];
    for (int x = 0; x<MeretX; ++x)
        szoveges[MeretY-1][x] = ' ';
}

void SysCVideo::karakter_ir(unsigned char ch)
{
    if (ch != '\n') {
        szoveges[kurzory%MeretY][kurzorx%MeretX] = ch;
        kurzorx++;
    }
    else {
        kurzorx = 0;
        kurzory++;
    }
    if (kurzorx>=MeretX) {
        kurzory++;
        kurzorx = 0;
    }
    if (kurzory>=MeretY) {
        szoveges_leptet_fel();
        kurzory--;
    }
}

// Egyik szál: ez végzi az ütemezést és a kirajzolást
void SysCVideo::utemez()
{
    data = "ZZZZZZZZ";
    while (true) {
        wait(20, SC_MS);
        rajzol();
    }
}

unsigned char SysCVideo::regiszterolvas(unsigned address)
{
    if (address == KurzorXRegCim)
        return kurzorx;
    else
    if (address == KurzorYRegCim)
        return kurzory;
    else
    if (address == BillentyuPufCim) {
        if (!billentyupuf.empty()) {
            lastkeyread = billentyupuf.front();
            billentyupuf.pop_front();
            return lastkeyread % 256;
        }
        else
            return 0;
    }
    else
    if (address == BillentyuPuf256Cim) {
        unsigned ret = lastkeyread / 256;
        lastkeyread = 0;
        return ret;
    }
    else
    if (address>=SzovegesCim && address<SzovegesCim+MeretX*MeretY)
        return szoveges[(address-SzovegesCim)/MeretX][(address-SzovegesCim)%MeretX];
    else
    if (address>=GrafikusCim && address<GrafikusCim+MeretX*MeretY)
        return grafikus[(address-GrafikusCim)/MeretX][(address-GrafikusCim)%MeretX];
    else
    if (address>=BillentyuMatrCim && address<BillentyuMatrCim+sizeof(billentyu))
        return billentyu[address-BillentyuMatrCim];
    else
        return 0;   /* a nem használt címek 0-t adnak olvasásra */
}

// Másik szál: ez kezeli a portokat
void SysCVideo::vezerel()
{
    if (!csneg && !writeneg) {    // iras?
        unsigned address = this->address.read().to_uint();
        unsigned char data = this->data.read().to_uint();
        regiszterir(address, data);
    }
    else
    if (!csneg && !readneg) {     // olvasas?
        unsigned address = this->address.read().to_uint();
        data = regiszterolvas(address);
    }
    else
        data = "ZZZZZZZZ";
}

void SysCVideo::regiszterir(unsigned address, unsigned char data)
{
    // command reg
    if (address == ParancsRegCim) {
        switch (data) {
            case 0: szoveges_mod(); break;
            case 1: grafikus_mod(); break;
            case 2: szoveges_torol(); break;
            case 3: kurzor_balfent(); break;
            case 4: grafikus_torol(); break;
        }
    }
    if (address == KurzorXRegCim)
        kurzorx = data;
    if (address == KurzorYRegCim)
        kurzory = data;
    if (address == KarakterIrCim)
        karakter_ir(data);
    if (address>=SzovegesCim && address<SzovegesCim+MeretX*MeretY)
        szoveges[(address-SzovegesCim)/MeretX][(address-SzovegesCim)%MeretX] = data;
    if (address>=GrafikusCim && address<GrafikusCim+MeretX*MeretY)
        grafikus[(address-GrafikusCim)/MeretX][(address-GrafikusCim)%MeretX] = data;
}

