/*
        BN.CPP - Fast Big Numbers
        
        Copyright (c) 1998 by Michael Neumann (18.06.1998)
        email: neumann@s-direktnet.de
        All rights reserved
        
        Language: Watcom C++ Version 11.0


        Zahl: 32 DWORDs = 1024 bits = 256 hex.Ziffern (bign)
*/        

#include <iostream>
#include <stdlib>
#include "bn.hpp"


// converts a character 'c' to a hexadecimal value
inline int hexdigit_toi(int c) {
        if(c>='0' && c<='9') return (c-'0');
        else if(c>='a' && c<='f') return (c-'a'+10);
        else if(c>='A' && c<='F') return (c-'A'+10);
        else return -1;
}

//
// This function loads r with the hex-string given in str
//
void bn_strhex(bnP r, const char* str) {
    bn dummy;
    bn_clr(dummy);
    bn_clr(r);
    while(*str) {
        bn_shlX32(r,4);   // mal 16
        dummy[0] = hexdigit_toi(*str++);
        bn_add(r,dummy);  // und addieren
    }
}

//
// This function converts a null-terminated string into a <bn>
//
void bn_strdec(bnP r, const char* str) {
    bn dummy, ten;
    bn_int(ten,10);
    bn_clr(r);
    while(*str) {
        bn_mov(dummy,r);
        bn_mul(dummy,ten,r);    // mal 10

        bn_clr(dummy);
        dummy[0] = *str++ -'0';
        bn_add(r,dummy);  // und addieren
    }
}

void bn_printhex(bnP r) {
    cout << "0x";
    for(int i=BN_LEN-1, x=0; i>=0;--i) {
        if(r[i]) x=1;
        if(x) {
            cout.width(8); cout.fill('0');
            cout << hex << r[i];
        }
    }
    if(!x) cout << 0;
    cout << endl;
}

char str[BN_DEC_ZIF_LEN+1];

//
// reads a string from the keyboard and converts it into a <bn>
//
void bn_inputdec(bnP r) {
    cin >> str;
    bn_strdec(r,str);
}

//
// reads a string from the keyboard and converts it into a <bn>
//
void bn_inputhex(bnP r) {
    cin >> str;
    bn_strhex(r,str);
}

void bn_printdec(bnP r) {
    static bn d,rest,zehn;    
    bn_int(zehn,10);
    bn_mov(d,r);        
    int i;
    str[BN_DEC_ZIF_LEN] = 0; i=BN_DEC_ZIF_LEN-1;
    
    do {
	cout << ".";
        bn_div(d,zehn,d,rest);
        str[i--] = rest[0]+'0';	
    } while( !bn_isZero(d) );
    cout << &str[i+1] << endl;
}

extern "C" void mul_dwords(ulong* res, ulong mul1, ulong mul2);
#pragma aux mul_dwords =                \
        "mul ecx"                       \
        "mov [ebx+00], eax"             \
        "mov [ebx+04], edx"             \
        parm [ebx] [eax] [ecx];
        


void bn_mul(bnP m1, bnP m2,bnP res) {
    static ulong mul_result[2];
    int bi1, bi2;
    bn_clr(res);
    bi1 = bi2 = BN_LEN-1;
        
    while(bi1>=0 && m1[bi1] == 0) --bi1; if(bi1 < 0) return;
    while(bi2>=0 && m2[bi2] == 0) --bi2; if(bi2 < 0) return;

    for(int i=bi1;i>=0;--i) for(int j=bi2;j>=0;--j)
        if(m1[i] && m2[j]) {
            mul_dwords(mul_result, m1[i], m2[j] );
            bn_addP(res,i+j,mul_result[0]);
            bn_addP(res,i+j+1,mul_result[1]);
        }    
}


int bn_cmp(bnP a, bnP b) {
    static bn t;
    bn_mov(t,a);
    bn_sub(t,b);    
    if(bn_isZero(t)) return(bn_cmpflag=BN_E);
    else if(bn_carryflag) return(bn_cmpflag=BN_L);
    else return(bn_cmpflag=BN_G);
}

    
void bn_error(int err) {
    if(err==BN_ERR_ZERODIVISION) {
        cout << "division by zero" << endl;
        exit(-1);
    }
    if(err==BN_ERR_GGT) {
        cout << "ggT error occured" << endl;
        exit(-1);
    }
    
}

void bn_div(bnP x, bnP b, bnP res, bnP rest) {
    static bn a, anz, zw;
    int bi1, bi2;
    
    bn_mov(a,x);
    bn_clr(anz);
    bn_clr(zw);
    bn_clr(res);

    if(bn_isZero(b)) bn_error(BN_ERR_ZERODIVISION);    
    if(bn_isZero(a) || bn_cmp(a,b)==BN_L) {
        bn_mov(rest,a); return;
    }

    bi1 = bi2 = BN_LEN-1;
    while(bi1>=0 && a[bi1] == 0) --bi1;
    while(bi2>=0 && b[bi2] == 0) --bi2;
    
    anz[0] = 0xFFFFFFFF;
    bi1-=bi2;
    if(bi1 > 0) {
        for(int i=bi1; i>0; --i) anz[i] = 0xFFFFFFFF;
        anz[bi1] = 0xFFFFFFF;
    }
    
    bn_mul(b,anz,zw);

    do {
        while(bn_cmp(a,zw) == BN_L) {
            while(bi1>0 && anz[bi1] == 0) --bi1;
            anz[bi1] >>=1;
            bn_mul(b,anz,zw);
        }
        bn_sub(a,zw);
        bn_add(res,anz);        
        bn_cmp(a,b);
    } while(bn_cmpflag==BN_G || bn_cmpflag==BN_E);
    bn_mov(rest,a);
}



bool bn_isOne(bnP x) {
    if(x[0] != 1) return false;
    for(int i=BN_LEN-1; i>0; --i) if(x[i]!=0) return false;        
    return true;
}



bn _n, _a, _x; bnP _res;
void _bn_powmod() {
    static bn r[2];
    
    if(bn_isZero(_x)) bn_int(_res,1);
    else if(bn_isOne(_x)) bn_div(_a,_n,r[0],_res);
    else if((_x[0]&1) == 0) {
        bn_shr1(_x);
        _bn_powmod();   // _res zurück
        bn_mov(r[0],_res);
        bn_mul(r[0],_res,r[1]); // r[1] = _res²
        bn_div(r[1],_n,r[0],_res);      // % n 
    } else {
        bn_dec(_x);        
        bn_shr1(_x);        
        _bn_powmod();      //_res zurück
        bn_mov(r[0],_res);
        bn_mul(r[0],_res,r[1]); // r[1] = _res²
        bn_div(r[1],_n,r[0],_res);      // % n                
        // res²%n
        
        bn_mul(_res,_a,r[0]);
        bn_div(r[0],_n,r[1],_res);
    }
}

void bn_powm(bnP a, bnP x, bnP n, bnP res) {
    bn_mov(_a,a);
    bn_mov(_x,x);
    bn_mov(_n,n);
    _res = res;
    _bn_powmod();
}

// calculates a ** x
void bn_pow(bnP a,int x, bnP res) {
    static bn b;
    if(x==0) {
        bn_int(res,1);
        return;
    }
    bn_mov(b,a);
    bn_mov(res,a);
    while(--x>0) {
        bn_mul(a,b,res);
        bn_mov(b,res);
    }
}
  
//
// folgende Funktionen rotiert r um shiftanz Bits nach links oder nach Rechts
void bn_shl(bnP r,int shiftanz) {
    int i;
    bn_shlX32(r,shiftanz);
    shiftanz >>= 5;                   // durch 32
    if(shiftanz==0) return;
    for(i=BN_LEN-1; i>=shiftanz; --i) r[i] = r[i-shiftanz];
    for(i=0;i<shiftanz;++i) r[i] = 0;
}

void bn_shr(bnP r,int shiftanz) {
    int i;
    bn_shrX32(r,shiftanz);
    shiftanz >>= 5;                   // durch 32
    if(shiftanz==0) return;
    for(i=0;i<BN_LEN-shiftanz;++i) r[i] = r[i+shiftanz];
    for(i=BN_LEN-shiftanz;i<BN_LEN;++i) r[i] = 0;
}


//
// Euklidische Algorithmus - größter gemeinsamer Teiler ggT - greates common divisor gcd
//
void bn_gcd(bnP x, bnP y, bnP res) {
    bn dummy;
    if( bn_isZero(x) && bn_isZero(y) ) bn_error(BN_ERR_GGT);
    
    bn_mov(res,y);
    while( !bn_isZero(x) ) {
        bn_mov(res,x);
        bn_div(y,x,dummy,x);
        bn_mov(y,res);
    }
}

inline void setbit(void *mem, int bitnr,int state) {
        int addy = bitnr>>3; bitnr &= 7;
   // setzen, d.h. OR-Verknüpfung
        if(state) *(((char*)mem)+addy) |= (1<<bitnr);
   // löschen, d.h. AND-Verknüpfung
   else *(((char*)mem)+addy) &= ~(1<<bitnr);
}

inline int getbit(void *mem, int bitnr) {
        int addy = bitnr>>3; bitnr &= 7;
   return ( (*(((char*)mem)+addy)) &(1<<bitnr)) >> bitnr;
}


void bn_setbit(bnP x, int nr, int state) {setbit(x,nr,state);}
int bn_getbit(bnP x, int nr) {return bn_getbit(x,nr);}


// gibt das höchste gesetzte Bit zurück (dessen nummer)
int bn_log2(bnP x) {
    int i,j;
    for(i=BN_LEN-1; i>=0 && x[i]==0; --i);
    for(j=31;j>=0;--j) if(getbit(&x[i],j)) return ((i<<5)+j);    
    return 0;
}

// r = 2**x
void bn_pow2(bnP r, int x) {
    bn_clr(r);
    setbit(r,x,1);
}

