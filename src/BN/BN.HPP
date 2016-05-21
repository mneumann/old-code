/*
        BN.HPP - Fast Big Numbers
        
        Copyright (c) 1998 by Michael Neumann (18.06.1998)
        All rights reserved
        
        Language: Watcom C++ Version 11.0

*/        

#ifndef __BN_HEADER__ZORO__
#define __BN_HEADER__ZORO__

const int
        //BN_LEN                  =32,   // Anzahl DWORDs im Array
        BN_LEN = 512,

        BN_DEC_ZIF_LEN          = BN_LEN * 10,
	
        BN_HEX_ZIF_LEN          = BN_LEN * 8,
        BN_E                    =  1,   // gleich
        BN_G                    =  2,   // größer
        BN_L                    =  4,   // kleiner
        BN_ERR_ZERODIVISION     = -4,   // Division durch Null - Fehler
        BN_ERR_GGT              = -5;   // Fehler bei ggT
        


typedef unsigned long ulong;
typedef ulong *bnP;
typedef ulong bn[BN_LEN];

extern "C" char bn_carryflag;
extern "C" char bn_cmpflag;

extern "C" {    
    void bn_clr(bnP);
    void bn_mov(bnP,bnP);
    void bn_add(bnP,bnP);
    void bn_sub(bnP,bnP);
    void bn_inc(bnP);
    void bn_dec(bnP);
        
    void bn_shl1(bnP);
    void bn_shr1(bnP);

    void bn_shlX32(bnP,int);    // rotates max. 32
    void bn_shrX32(bnP,int);    // rotates max. 32
    void bn_shl(bnP,int);
    void bn_shr(bnP,int);
    
    void bn_int(bnP,ulong);
    void bn_addP(bnP,int,ulong);
    int bn_isZero(bnP);

    void bn_and(bnP,bnP);
    void bn_or(bnP,bnP);
    void bn_xor(bnP,bnP);
    void bn_not(bnP);
}

// die zahlen geben die parameter an
extern void bn_printhex(bnP);
extern void bn_printdec(bnP);
extern void bn_mul(bnP,bnP,bnP); //3=1*2
extern int bn_cmp(bnP,bnP);
extern void bn_div(bnP,bnP,bnP,bnP); // 1/2 = 3 rest 4
extern bool bn_isOne(bnP);
extern void bn_powm(bnP,bnP,bnP,bnP); // 1 ** 2 % 3 = 4
extern void bn_pow(bnP,int,bnP);        // 1 ** 2 = 3
extern void bn_strhex(bnP, const char*); // 1 = 2
extern void bn_strdec(bnP, const char*);
extern void bn_gcd(bnP,bnP,bnP);        // 3 = gcd(1,2) ggT

extern int bn_log2(bnP x);
extern void bn_pow2(bnP r, int x);
extern void bn_setbit(bnP x, int nr, int state);
extern int bn_getbit(bnP x, int nr);

extern void bn_inputdec(bnP);
extern void bn_inputhex(bnP);
#endif
