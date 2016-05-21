/*
   DIASS86.CPP -> DIASS86.EXE

   Diassembliert eine Datei mit 8086-Maschinencodes, und
   speichert diese, in einer zweiten Datei.

   Dieses Programm kann nur mit WATCOM C++ (V.11.0) compiliert werden,
   und zwar im DOS-32-Bit Mode (DOS4GW oder PMODE/W).
   
   Copyright (c) 1997 by Michael Neumann (neumann@s-direktnet.de)
*/

#include <string.h>
#include <iostream.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <malloc.h>
#include <conio.h>
#include "C:\TERA_COD\OOP\STRING.HPP"   // definiert CString


FILE *srcfile, *destfile;
char cpyrght[] = "Copyright (c) 1997 by Little-Coders (Michael Neumann)";
#define COPYRIGHT_MELDUNG cout << cpyrght << endl
char dumstr[] = {13,10,0};
CString NEXTLINE = (char*) dumstr;

//

#define BYTE unsigned char
#define WORD unsigned short

#define ERROR      0
#define IMPLICIT   1
#define AL_IMM     2
#define AX_IMM     3
#define RM8_IMM    4
#define RM16_IMM   5
#define RM16_IMM8  6
#define RM8_REG    7
#define RM16_REG   8
#define REG8_RM    9
#define REG16_RM  10
#define REL16   11
#define IMM16   12
#define RM16    13
#define PTR16_16        14
#define OP1ADD_REG16     15      // z.B. DEC ax
#define CS_PREF         44
#define AL_RM   16
#define AX_RM  17
#define RM8   18
#define IMM8   19
#define REL8   20
#define IMM_AL 21
#define IMM_AX 22
#define M16 23
#define SCHIEBE_BEF 24
#define SREG_RM16 25
#define RM16_SREG 26
#define REG16_M   27
#define REG16_IMM 28
 
// ??? al->ax opcode+1 ??? aber egal
#define BEL_NONE   0
#define BEL_OP1    1
#define BEL_OP2    2
#define BEL_REGOP  4
#define BEL_OP1OP2 (BEL_OP1+BEL_OP2)
#define BEL_OP1REGOP (BEL_OP1+BEL_REGOP)
#define BEL_OP1ADDREG 8
#define BEL_MANY1 16
struct TCode
{
   BYTE op1,op2;
   BYTE regop;
   BYTE belegt;  // Maske die anzeigt, welche BYtes belegt sind
   BYTE state;   // AL_IMM e.g.
   char *name;
};
#define CODESLEN 159
TCode Codes[CODESLEN] =
{
   0x00,0x00,0x00,0x00,0x00,"         ",
   0x37,0x00,0x00,BEL_OP1,IMPLICIT,"AAA",
   0xD5,0x0A,0x00,BEL_OP1OP2,IMPLICIT,"AAD",
   0xD4,0x0A,0x00,BEL_OP1OP2,IMPLICIT,"AAM",
   0x3F,0x00,0x00,BEL_OP1,IMPLICIT,"AAS",
   0x10,0x00,0x02,BEL_MANY1,BEL_MANY1,"ADC",
   0x00,0x00,0x00,BEL_MANY1,BEL_MANY1,"ADD",
   0x20,0x00,0x04,BEL_MANY1,BEL_MANY1,"AND",   
   0xE8,0x00,0x00,BEL_OP1,REL16,"CALL",
   0xFF,0x00,0x02,BEL_OP1REGOP,RM16,"CALL",
   0x9A,0x00,0x00,BEL_OP1,PTR16_16,"CALL",
   0x98,0x00,0x00,BEL_OP1,IMPLICIT,"CBW",
   0xF8,0x00,0x00,BEL_OP1,IMPLICIT,"CLC",
   0xFC,0x00,0x00,BEL_OP1,IMPLICIT,"CLD",
   0xFA,0x00,0x00,BEL_OP1,IMPLICIT,"CLI",
   0xF5,0x00,0x00,BEL_OP1,IMPLICIT,"CMC",   
   0x38,0x00,0x07,BEL_MANY1,BEL_MANY1,"CMP",
   0xA6,0x00,0x00,BEL_OP1,IMPLICIT,"CMPSB",
   0xA7,0x00,0x00,BEL_OP1,IMPLICIT,"CMPSW",
   0x99,0x00,0x00,BEL_OP1,IMPLICIT,"CWD",
   0x27,0x00,0x00,BEL_OP1,IMPLICIT,"DAA",
   0x2F,0x00,0x00,BEL_OP1,IMPLICIT,"DAS",
   0xFE,0x00,0x01,BEL_OP1REGOP,RM8,"DEC",
   0xFF,0x00,0x01,BEL_OP1REGOP,RM16,"DEC",
   0x48,0x48+7,0x00,BEL_OP1ADDREG,OP1ADD_REG16,"DEC",
   0xF6,0x00,0x06,BEL_OP1REGOP,RM8,"DIV",
   0xF7,0x00,0x06,BEL_OP1REGOP,RM16,"DIV",
   0xF4,0x00,0x00,BEL_OP1,IMPLICIT,"HLT",
   0xF6,0x00,0x07,BEL_OP1,RM8,"IDIV",
   0xF7,0x00,0x07,BEL_OP1REGOP,RM16,"IDIV",
   0xF6,0x00,0x05,BEL_OP1REGOP,RM8,"IMUL",
   0xF7,0x00,0x05,BEL_OP1REGOP,RM16,"IMUL",
   0xE4,0x00,0x00,BEL_OP1,AL_IMM,"IN",
   0xE5,0x00,0x00,BEL_OP1,AX_IMM,"IN",
   0xEC,0x00,0x00,BEL_OP1,IMPLICIT,"IN AL, DX",
   0xED,0x00,0x00,BEL_OP1,IMPLICIT,"IN AX, DX",
   0xFE,0x00,0x00,BEL_OP1REGOP,RM8,"INC",
   0xFF,0x00,0x00,BEL_OP1REGOP,RM16,"INC",
   0x40,0x40+7,0x00,BEL_OP1ADDREG,OP1ADD_REG16,"INC",
   0xCC,0x00,0x00,BEL_OP1,IMPLICIT,"INT 3",
   0xCD,0x00,0x00,BEL_OP1,IMM8,"INT",
   0xCE,0x00,0x00,BEL_OP1,IMPLICIT,"INTO",
   0xCF,0x00,0x00,BEL_OP1,IMPLICIT,"IRET",
   0x77,0x00,0x00,BEL_OP1,REL8,"JA/JNBE",
   0x73,0x00,0x00,BEL_OP1,REL8,"JAE/JNB/JNC",
   0x72,0x00,0x00,BEL_OP1,REL8,"JC/JB/JNAE",
   0x76,0x00,0x00,BEL_OP1,REL8,"JBE/JNA",
   0xE3,0x00,0x00,BEL_OP1,REL8,"JCXZ",
   0x74,0x00,0x00,BEL_OP1,REL8,"JZ/JE",
   0x7F,0x00,0x00,BEL_OP1,REL8,"JG/JNLE",
   0x7D,0x00,0x00,BEL_OP1,REL8,"JGE/JNL",
   0x7C,0x00,0x00,BEL_OP1,REL8,"JL/JNGE",   
   0x7E,0x00,0x00,BEL_OP1,REL8,"JLE/JNG",
   0x75,0x00,0x00,BEL_OP1,REL8,"JNE/JNZ",
   0x71,0x00,0x00,BEL_OP1,REL8,"JNO",
   0x7B,0x00,0x00,BEL_OP1,REL8,"JNP/JPO",
   0x79,0x00,0x00,BEL_OP1,REL8,"JNS",
   0x70,0x00,0x00,BEL_OP1,REL8,"JO",
   0x7A,0x00,0x00,BEL_OP1,REL8,"JP/JPE",
   0x78,0x00,0x00,BEL_OP1,REL8,"JS",   
   0xEB,0x00,0x00,BEL_OP1,REL8,"JMP",
   0xE9,0x00,0x00,BEL_OP1,REL16,"JMP",
   0xFF,0x00,0x04,BEL_OP1REGOP,RM16,"JMP",
   0xEA,0x00,0x00,BEL_OP1,PTR16_16,"JMP",
   0xFF,0x00,0x05,BEL_OP1,M16,"JMP",    
   0x9F,0x00,0x00,BEL_OP1,IMPLICIT,"LAHF",
   
   0xC5,0x00,0x00,BEL_OP1,REG16_M,"LDS",
   0x0F,0xB2,0x00,BEL_OP1OP2,REG16_M,"LSS",//
   0xC4,0x00,0x00,BEL_OP1,REG16_M,"LES",
   0x0F,0xB4,0x00,BEL_OP1OP2,REG16_M,"LFS",//
   0x0F,0xB5,0x00,BEL_OP1OP2,REG16_M,"LGS",//
     
   0x8D,0x00,0x00,BEL_OP1,REG16_IMM,"LEA",
   0xF0,0x00,0x00,BEL_OP1,IMPLICIT,"LOCK",
   0xAC,0x00,0x00,BEL_OP1,IMPLICIT,"LODSB",
   0xAD,0x00,0x00,BEL_OP1,IMPLICIT,"LODSW",
   0xE2,0x00,0x00,BEL_OP1,REL8,"LOOP",
   0xE1,0x00,0x00,BEL_OP1,REL8,"LOOPZ",
   0xE0,0x00,0x00,BEL_OP1,REL8,"LOOPNZ",
   
   0x88,0x00,0x00,BEL_OP1,RM8_REG,"MOV",
   0x89,0x00,0x00,BEL_OP1,RM16_REG,"MOV",
   0x8A,0x00,0x00,BEL_OP1,REG8_RM,"MOV",
   0x8B,0x00,0x00,BEL_OP1,REG16_RM,"MOV",
   0x8C,0x00,0x00,BEL_OP1,RM16_SREG,"MOV",
   0x8D,0x00,0x00,BEL_OP1,SREG_RM16,"MOV",
   0xC6,0x00,0x00,BEL_OP1,RM8_IMM,"MOV",
   0xC7,0x00,0x00,BEL_OP1,RM16_IMM,"MOV",
   
   0xA4,0x00,0x00,BEL_OP1,IMPLICIT,"MOVSB",
   0xA5,0x00,0x00,BEL_OP1,IMPLICIT,"MOVSW",
   0xF6,0x00,0x04,BEL_OP1REGOP,RM8,"MUL",
   0xF7,0x00,0x04,BEL_OP1REGOP,RM16,"MUL",
   0xF6,0x00,0x03,BEL_OP1REGOP,RM8,"NEG",
   0xF7,0x00,0x03,BEL_OP1REGOP,RM16,"NEG",
   0x90,0x00,0x00,BEL_OP1,IMPLICIT,"NOP",
   0xF6,0x00,0x02,BEL_OP1REGOP,RM8,"NOT",
   0xF7,0x00,0x02,BEL_OP1REGOP,RM16,"NOT",
   0x08,0x00,0x01,BEL_MANY1,BEL_MANY1,"OR",
   0xE6,0x00,0x00,BEL_OP1,IMM_AL,"OUT",
   0xE7,0x00,0x00,BEL_OP1,IMM_AX,"OUT",
   0xEE,0x00,0x00,BEL_OP1,IMPLICIT,"OUT DX, AL",
   0xEF,0x00,0x00,BEL_OP1,IMPLICIT,"OUT DX, AX",
   0x8F,0x00,0x00,BEL_OP1REGOP,M16,"POP",
   0x58,0x58+7,0x00,BEL_OP1ADDREG,OP1ADD_REG16,"POP",
   0x1F,0x00,0x00,BEL_OP1,IMPLICIT,"POP DS",
   0x07,0x00,0x00,BEL_OP1,IMPLICIT,"POP ES",
   0x17,0x00,0x00,BEL_OP1,IMPLICIT,"POP SS",
   0x0F,0xA1,0x00,BEL_OP1OP2,IMPLICIT,"POP FS",//
   0x0F,0xA9,0x00,BEL_OP1OP2,IMPLICIT,"POP GS",//
   0x61,0x00,0x00,BEL_OP1,IMPLICIT,"POPA",//
   0x9D,0x00,0x00,BEL_OP1,IMPLICIT,"POPF",
   0xFF,0x00,0x06,BEL_OP1REGOP,M16,"PUSH",
   0x50,0x50+7,0x00,BEL_OP1ADDREG,OP1ADD_REG16,"PUSH",
   0x6A,0x00,0x00,BEL_OP1,IMM8,"PUSH",
   0x68,0x00,0x00,BEL_OP1,IMM16,"PUSH",
   0x0E,0x00,0x00,BEL_OP1,IMPLICIT,"PUSH CS",
   0x16,0x00,0x00,BEL_OP1,IMPLICIT,"PUSH SS",
   0x1E,0x00,0x00,BEL_OP1,IMPLICIT,"PUSH DS",
   0x06,0x00,0x00,BEL_OP1,IMPLICIT,"PUSH ES",
   0x0F,0xA0,0x00,BEL_OP1OP2,IMPLICIT,"PUSH FS",//
   0x0F,0xA8,0x00,BEL_OP1OP2,IMPLICIT,"PUSH GS",//

   0x2E,0x00,0x00,BEL_OP1,IMPLICIT,"CS:",
   0x3E,0x00,0x00,BEL_OP1,IMPLICIT,"DS:",
   0x26,0x00,0x00,BEL_OP1,IMPLICIT,"ES:",
   0x64,0x00,0x00,BEL_OP1,IMPLICIT,"FS:",//
   0x65,0x00,0x00,BEL_OP1,IMPLICIT,"GS:",//
   0x36,0x00,0x00,BEL_OP1,IMPLICIT,"SS:",
   0x66,0x00,0x00,BEL_OP1,IMPLICIT,"OPSIZE:",//
   0x67,0x00,0x00,BEL_OP1,IMPLICIT,"ADRSIZE:",//
   
   0x60,0x00,0x00,BEL_OP1,IMPLICIT,"PUSHA",//
   0x9C,0x00,0x00,BEL_OP1,IMPLICIT,"PUSHF",
   
   0xF3,0x00,0x00,BEL_OP1,IMPLICIT,"REP/REPE",
   0xF2,0x00,0x00,BEL_OP1,IMPLICIT,"REPNE",

   
   0xC3,0x00,0x00,BEL_OP1,IMPLICIT,"RET",
   0xCB,0x00,0x00,BEL_OP1,IMPLICIT,"RETF",
   0xC2,0x00,0x00,BEL_OP1,IMM16,"RET",
   0xCA,0x00,0x00,BEL_OP1,IMM16,"RETF",
   0x9E,0x00,0x00,BEL_OP1,IMPLICIT,"SAHF",
   0x18,0x00,0x03,BEL_MANY1,BEL_MANY1,"SBB",
   0xAE,0x00,0x00,BEL_OP1,IMPLICIT,"SCASB",
   0xAF,0x00,0x00,BEL_OP1,IMPLICIT,"SCASW",   
   0xF9,0x00,0x00,BEL_OP1,IMPLICIT,"STC",
   0xFD,0x00,0x00,BEL_OP1,IMPLICIT,"STD",
   0xFB,0x00,0x00,BEL_OP1,IMPLICIT,"STI",
   0xAA,0x00,0x00,BEL_OP1,IMPLICIT,"STOSB",
   0xAB,0x00,0x00,BEL_OP1,IMPLICIT,"STOSW",
   0x28,0x00,0x05,BEL_MANY1,BEL_MANY1,"SUB",
   0xA8,0x00,0x00,BEL_OP1,AL_IMM,"TEST",
   0xA9,0x00,0x00,BEL_OP1,AX_IMM,"TEST",
   0xF6,0x00,0x00,BEL_OP1REGOP,RM8_IMM,"TEST",
   0xF7,0x00,0x00,BEL_OP1REGOP,RM16_IMM,"TEST",
   0x84,0x00,0x00,BEL_OP1,RM8_REG,"TEST",
   0x85,0x00,0x00,BEL_OP1,RM16_REG,"TEST",  
   0x9B,0x00,0x00,BEL_OP1,IMPLICIT,"WAIT",
   0x86,0x00,0x00,BEL_OP1,REG8_RM,"XCHG",
   0x87,0x00,0x00,BEL_OP1,RM16_REG,"XCHG",
   0x90,0x90+7,0x00,BEL_OP1ADDREG,OP1ADD_REG16,"XCHG AX,",
//   0x87,0x00,0x00,BEL_OP1,REG16_RM,"XCHG", Ist dasselbe weil getauscht wird
   0xD7,0x00,0x00,BEL_OP1,IMPLICIT,"XLATB",
   0x30,0x00,0x06,BEL_MANY1,BEL_MANY1,"XOR"
};
// MOV CS,??? ist nicht erlaubt, da es dann ein Sprung w„re
//int cs_count=0;
int hex_offset;

#define MODE_NONE          0
#define MODE_DISPL8        1
#define MODE_DISPL16       2
#define MODE_REGISTER      3

struct TEffAddr
{
    char *adrname;       // Name
    BYTE mode;           // Mode siehe oben e.g. MODE_NONE
    BYTE defseg;         //default-Segment
};

char *Register8[] = {"AL","CL","DL","BL","AH","CH","DH","BH"};
char *Register16[] = {"AX","CX","DX","BX","SP","BP","SI","DI"};
char *SRegister[] = {"ES","CS","SS","DS","FS","GS"};

#define SEG_ES 0
#define SEG_CS 1
#define SEG_SS 2
#define SEG_DS 3
#define SEG_NONE 55


//Mod*8+R/M=index
TEffAddr EffAddr[] =
{
   "BX+SI",MODE_NONE,SEG_DS,
   "BX+DI",MODE_NONE,SEG_DS,
   "BP+SI",MODE_NONE,SEG_SS,
   "BP+DI",MODE_NONE,SEG_SS,
   "SI",MODE_NONE,SEG_DS,
   "DI",MODE_NONE,SEG_DS,
   "",MODE_DISPL16,SEG_DS,
   "BX",MODE_NONE,SEG_DS,

   "BX+SI",MODE_DISPL8,SEG_DS,
   "BX+DI",MODE_DISPL8,SEG_DS,
   "BP+SI",MODE_DISPL8,SEG_SS,
   "BP+DI",MODE_DISPL8,SEG_SS,
   "SI",MODE_DISPL8,SEG_DS,
   "DI",MODE_DISPL8,SEG_DS,
   "BP",MODE_DISPL8,SEG_SS,
   "BX",MODE_DISPL8,SEG_DS,

   "BX+SI",MODE_DISPL16,SEG_DS,
   "BX+DI",MODE_DISPL16,SEG_DS,
   "BP+SI",MODE_DISPL16,SEG_SS,
   "BP+DI",MODE_DISPL16,SEG_SS,
   "SI",MODE_DISPL16,SEG_DS,
   "DI",MODE_DISPL16,SEG_DS,
   "BP",MODE_DISPL16,SEG_SS,
   "BX",MODE_DISPL16,SEG_DS,

   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE,
   "",MODE_REGISTER,SEG_NONE
};


/* Inline Funktionen */
extern BYTE GetRM( BYTE );
extern BYTE GetRegOp( BYTE );
extern BYTE GetMod( BYTE );
extern short BB2WORD(BYTE low, BYTE high);
#pragma aux GetRM =     \
        "and al,7"      \
        parm [al]       \
        value [al]      \
        modify nomemory exact [al];
#pragma aux GetRegOp =  \
        "shr al,3"      \
        "and al,7"      \
        parm [al]       \
        value [al]      \
        modify nomemory exact [al];
#pragma aux GetMod =    \
        "shr al,6"      \
        parm [al]       \
        value [al]      \
        modify nomemory exact [al];
#pragma aux BB2WORD =   \
        parm [al] [ah]  \
        value [ax]     \
        modify nomemory exact [];
/* Ende Inline Funktionen */



int GetExtBeg( TCode *c )
{
   switch( c->belegt )
   {
        case BEL_OP1:           return 1; break;
        case BEL_OP1OP2:                return 2; break;
        case BEL_OP1REGOP:      return 2; break;
   };
   return 0;
}



// -1 wenn nicht gefunden -> DB Direktive
// -2 wenn Ende
// sonst den Index
// mnem wird immer erh”ht und anz veringert

BYTE *mnem;
int anz;
BYTE *oldmnem;
int index;
CString a;
int add;



void fputstr( FILE *f, char *str )
{
   long i=0;
   while( str[i] != 0 )
   {
          fputc( str[i++], f );
   }
}


void Byte2HexStr( BYTE zahl, char *hexstr )
{
   static char hexarray[] = "0123456789ABCDEF";
   hexstr[0] = hexarray[ (zahl>>4) % 16 ];
   hexstr[1] = hexarray[ zahl % 16 ];
   hexstr[2] = 0;
}

void DWord2HexStr( long zahl, char *hexstr )
{
    BYTE *ptr = (BYTE*) &zahl;
    for( int i=3;i>=0;i--) Byte2HexStr( ptr[i], hexstr+((3-i)<<1) );
}

char hexconv[9];

void Output_String( CString &str )
{
    CString str2;
    if(mnem > oldmnem )
    {
        DWord2HexStr(hex_offset, hexconv);
        str2 += hexconv;
        str2 += "   ";
        for( int i = 0; i < mnem-oldmnem; i++ )
        {
            Byte2HexStr( oldmnem[i] , hexconv );
            str2 += hexconv;
            str2 += " ";
        }
        
        while( str2.GetLen() < 37 ) str2+=" ";
        str2 += "   " +str;
        
        str2 += NEXTLINE;
        fputstr( destfile ,(char*)str2.GetPtr() );
        hex_offset += (mnem-oldmnem);
    }
}




int Find()
{
   if( anz <= 0 ) return -2;

   // MOV AL/AX,MOFFS8/16 und MOV MOFFS8/16, AL/AX
   if( anz >= 3 && mnem[0] >=0xA0 && mnem[0] <=0xA3 )
   {
       WORD moffs = BB2WORD( mnem[1], mnem[2] );
       a = "MOV ";
       switch( mnem[0] )
       {
           case 0xA0: a+=CString("AL, DS:[") + CString( moffs ) + CString("]");break;
           case 0xA1: a+=CString("AX, DS:[") + CString( moffs ) + CString("]");break;
           case 0xA2: a+=CString("DS:[") + CString( moffs ) + CString("], AL");break;
           case 0xA3: a+=CString("DS:[") + CString( moffs ) + CString("], AX");break;
       };
       mnem+=3;
       anz-=3;
       Output_String( a );
       return -3;       // Fertig
   }
   /*********************************/


   // MOV REG8+i,IMM8   und   MOV REG16+i,IMM16   
   if( mnem[0] >= 0xB0 && mnem[0] <= 0xB0+7 && anz >=2 )
   {
       a = CString("MOV ") + CString(Register8[mnem[0]-0xB0]) +CString(", BYTE ") + CString((WORD)mnem[1]);
       mnem+=2;
       anz-=2;
       Output_String( a );
       return -3;
   }

   if( mnem[0] >= 0xB8 && mnem[0] <= 0xB8+7 && anz >=3 )
   {
       a = CString("MOV ") + CString(Register16[mnem[0]-0xB8]) +CString(", WORD ") + CString((WORD)BB2WORD(mnem[1],mnem[2]));
       mnem+=3;
       anz-=3;
       Output_String( a );
       return -3;
   }
   /*********************************/




   // RCL/RCR/ROL/ROR/SAL/SAR/SHL/SHR
   // Merke: SAL und SHL sind absolut identisch -> Nur ein Mnem-Opcode
   static char *Schiebe[] = {"ROL","ROR","RCL","RCR","SHL/SAL","SHR","","SAR"};
   if( anz >= 2 )
   {
       int regop = GetRegOp(mnem[1]);
       if( regop >=0 && regop <=7 && (regop!=6) )
       {
           strcpy( Codes[0].name, Schiebe[regop] );
           Codes[0].belegt = BEL_OP1REGOP;
           switch( mnem[0] )
           {
              case 0xD0: //Codes[0].state = RM8_1;
              case 0xD2: //Codes[0].state = RM8_CL;
              case 0xC0: //Codes[0].state = RM8_IMM;
              case 0xD1: //Codes[0].state = RM16_1;
              case 0xD3: //Codes[0].state = RM16_CL;
              case 0xC1: //Codes[0].state = RM16_IMM8;
              Codes[0].state = SCHIEBE_BEF; return 0;
              break;
          };
      }
   }
   /*********************************/
   
   for( int i = 1; i < CODESLEN; i++ )
   {
        switch( Codes[i].belegt )
        {
           case BEL_OP1:
                        if( (anz >= 1) && (mnem[0] == Codes[i].op1) ) return i;
                        break;

           case BEL_OP1OP2:
                        if( (anz >= 2) && (mnem[0] == Codes[i].op1) &&
                                (mnem[1] == Codes[i].op2) ) return i;
                        break;

           case BEL_OP1REGOP:
                        if( (anz >= 2) && (mnem[0] == Codes[i].op1) &&
                                (GetRegOp( mnem[1] ) == Codes[i].regop) ) return i;
                        break;
           case BEL_OP1ADDREG:
                        if( (anz >= 1) && (mnem[0]>=Codes[i].op1) && (mnem[0]<=Codes[i].op1+7))  return i;break;

           case BEL_MANY1:
                        strcpy(Codes[0].name,Codes[i].name);
                        
                        if( anz >= 2 )
                        {                            
                            if( GetRegOp(mnem[1]) == Codes[i].regop )
                            {
                                Codes[0].belegt = BEL_OP1REGOP;
                                switch( mnem[0] )
                                {
                                    case 0x80: Codes[0].state = RM8_IMM;        return 0;break;
                                    case 0x81: Codes[0].state = RM16_IMM;       return 0;break;
                                    case 0x83: Codes[0].state = RM16_IMM8;      return 0;break;
                                };
                            }
                        }
                        if( anz >= 1 )
                        {
                            if( (mnem[0]-Codes[i].op1 >=0) && (mnem[0]-Codes[i].op1 <=5)) Codes[0].belegt=BEL_OP1;
                            switch( mnem[0]-Codes[i].op1 )
                            {
                                case 0: Codes[0].state = RM8_REG;       return 0;break;
                                case 1: Codes[0].state = RM16_REG;      return 0;break;
                                case 2: Codes[0].state = REG8_RM;       return 0;break;
                                case 3: Codes[0].state = REG16_RM;      return 0;break;
                                case 4: Codes[0].state = AL_IMM;        return 0;break;         
                                case 5: Codes[0].state = AX_IMM;        return 0;break;
                            };
                        }
                        break;                     
        };
   }
   return -1;
}

void Make_DB()
{
   a = "DB " + CString( (int)mnem[0] );
   anz-=1; mnem+=1;
   Output_String( a );
}

void Make_AL_IMM()
{
   add = GetExtBeg( &Codes[index] )+1;
   if( anz >= add )
   {
        a = Codes[index].name + CString(" AL, BYTE ") + CString( (unsigned) mnem[add-1] );
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
   else Make_DB();
}

void Make_IMM_AL()
{
   add = GetExtBeg( &Codes[index] )+1;
   if( anz >= add )
   {
        a = Codes[index].name + CString(" BYTE ") + CString( (unsigned) mnem[add-1] ) + CString(", AL");
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
   else Make_DB();
}

void Make_AX_IMM()
{
   add = GetExtBeg( &Codes[index] )+2;
   if( anz >= add )
   {
        a = Codes[index].name + CString(" AX, WORD ") + CString( (WORD)BB2WORD(mnem[add-2],mnem[add-1]) );
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
   else Make_DB();
}

void Make_IMM_AX()
{
   add = GetExtBeg( &Codes[index] )+2;
   if( anz >= add )
   {
        a = Codes[index].name + CString(" WORD ") + CString( (WORD)BB2WORD(mnem[add-2],mnem[add-1]) ) + CString(", AX");
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
   else Make_DB();
}

void Make_IMPLICIT()
{
   add = GetExtBeg( &Codes[index] );
   a = Codes[index].name;
   mnem += add;
   anz  -= add;
   Output_String( a );
}

void str_seg( BYTE defseg, BYTE size, CString &s )
{
   CString seg_str;
   
/*   if(cs_count>0) seg_str = "CS";
   else
   {*/
        if( defseg == SEG_SS ) seg_str = "SS";
        else if( defseg == SEG_DS ) seg_str = "DS";
//   };

   switch( defseg )
   {
        case SEG_DS:
        case SEG_SS:
                if( size == 8 ) s += "BYTE PTR ";
                else if( size == 16 ) s += "WORD PTR ";
                s += seg_str + ":[";
                break;
//        case SEG_NONE:
//                if( cs_count > 0 ) cout << "DB 2Eh  ; CS-Segpr„fix falscher Platz" << endl;
        break;
   };
}

// gibt -1 zurck wenn Fehler
// sonst add
int GetEff( TCode *c, CString &s, BYTE size )
{
   int inx;
   if( c->belegt == BEL_OP1OP2 )
   {
        if( anz >= 3 )
        {
           inx = (((int)GetMod( mnem[2] ))*8L)+ GetRM( mnem[2] );
           str_seg( EffAddr[inx].defseg, size, s );
           switch( EffAddr[inx].mode )
           {

                case MODE_NONE: s += EffAddr[inx].adrname + CString("]"); return 3; break;

                case MODE_DISPL8: if( anz >= 4 )
                                        {
                                           s += EffAddr[inx].adrname;
                                           int displ = (signed char)mnem[3];
                                           if( displ >= 0 && strlen(EffAddr[inx].adrname)!=0 ) s += "+";
                                           s += CString( displ ) + "]";
                                           return 4;
                                        }
                                        else return -1;
                                        break;

                case MODE_DISPL16:if(anz >= 5 ) 
                                        {
                                            s += EffAddr[inx].adrname;
                                           int displ = (short)BB2WORD( mnem[3],mnem[4] );
                                           if( displ >= 0 && strlen(EffAddr[inx].adrname)!=0 ) s += "+";
                                           s += CString( displ ) + "]";                                                           
                                           return 5;
                                        }
                                        else return -1;
                                        break;

                case MODE_REGISTER:
                                        if( size == 8 ) s += Register8[inx-24];
                                        else if( size == 16 ) s+= Register16[inx-24];
                                        return 3;
                                        break;
           };
        }
        else return -1;
   }
   else// if( c->belegt == BEL_OP1REGOP )
   {
        if( anz >= 2 )
        {
           inx = (((int)GetMod( mnem[1] ))*8L)+ GetRM( mnem[1] );
           str_seg( EffAddr[inx].defseg, size, s );
           switch( EffAddr[inx].mode )
           {

                case MODE_NONE: s += EffAddr[inx].adrname + CString("]"); return 2; break;

                case MODE_DISPL8: if( anz >= 3 )
                                        {
                                            s += EffAddr[inx].adrname;
                                            int displ = (signed char)mnem[2];
                                            if( displ >= 0 && strlen(EffAddr[inx].adrname)!=0 ) s += "+";
                                            s += CString( displ ) + "]";
                                            return 3;
                                        }
                                        else return -1;
                                        break;

                case MODE_DISPL16:if(anz >= 4 )
                                        {
                                            s += EffAddr[inx].adrname;
                                            int displ = (short)BB2WORD( mnem[2],mnem[3] );
                                            if( displ >= 0 && strlen(EffAddr[inx].adrname)!=0) s += "+";
                                            s += CString( displ ) + "]";
                                            return 4;
                                        }
                                        else return -1;
                                        break;

                case MODE_REGISTER:
                                        if( size == 8 ) s += Register8[inx-24];
                                        else if( size == 16 ) s += Register16[inx-24];
                                        return 2;
                                        break;
           };
        }
        else return -1;
   }
   return -1;
}
 
void Make_RM8_IMM()
{ 
   a = Codes[index].name + CString(" "); 

   add = GetEff( &Codes[index], a, 8 );
   if( add == -1 ) Make_DB();
   else
   {
        if( anz >= add+1 )
        {
            a += ", BYTE " + CString( (int) mnem[add] );
            mnem += ( add + 1 );
            anz  -= ( add + 1 );
            Output_String( a );
        }
        else Make_DB();
   }
}


void Make_RM16_IMM()
{
    a = Codes[index].name + CString(" ");
    add = GetEff( &Codes[index], a, 16 );
    if( add == -1 ) Make_DB();
    else
   {
        if( anz >= add+2 )
        {
           a += ", WORD " + CString( BB2WORD(mnem[add],mnem[add+1]) );
           mnem += ( add + 2 );
           anz  -= ( add + 2 );
           Output_String( a );
        }
        else Make_DB();
   }
}

void Make_RM16_IMM8()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        if( anz >= add+1 )
        {
            a += ", BYTE " + CString( (int)mnem[add] );
           mnem += ( add + 1 );
           anz  -= ( add + 1 );
            Output_String( a );
        }
        else Make_DB();
   }
}

void Make_RM8_REG()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 8 );
   if( add == -1 ) Make_DB();
   else
   {
        if( anz >= add )
        {
            a += ", ";
           if( Codes[index].belegt == BEL_OP1OP2 ) a += Register8[GetRegOp(mnem[2])];
           else a+= Register8[GetRegOp(mnem[1])];
           mnem += add;
           anz  -= add;
           Output_String( a );                
        }
        else Make_DB();
   }

}

void Make_RM16_REG()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        if( anz >= add )
        {
            a += ", ";
           if( Codes[index].belegt == BEL_OP1OP2 ) a +=  Register16[GetRegOp(mnem[2])] ;
           else a+= Register16[GetRegOp(mnem[1])];
           mnem += add;
           anz  -= add;
           Output_String( a );        
        }
        else Make_DB();
   }

}

void Make_REG8_RM()
{
   a = Codes[index].name + CString(" ");
   if( Codes[index].belegt == BEL_OP1OP2 )
   {
        if( anz >= 3 ) a += Register8[GetRegOp(mnem[2])];
        else
        {
            Make_DB();
            return;
        }
   }
   else
   {
        if( anz >= 2 ) a += Register8[GetRegOp(mnem[1])];
        else
        {
            Make_DB();
            return;
        }
   }
   a += ", ";
   add = GetEff( &Codes[index], a, 8 );
   if( add == -1 ) Make_DB();
   else
   {
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
}

void Make_REG16_RM()
{
   a = Codes[index].name + CString(" ");
   if( Codes[index].belegt == BEL_OP1OP2 )
   {
        if( anz >= 3 ) a += Register16[GetRegOp(mnem[2])];
        else
        {
            Make_DB();
            return;
        }
   }
   else
   {
        if( anz >= 2 ) a += Register16[GetRegOp(mnem[1])];
        else
        {
            Make_DB();
            return;
        }
   }
   a += ", ";
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
}


void Make_SREG_RM16()
{
   a = Codes[index].name + CString(" ");
   
   if( Codes[index].belegt == BEL_OP1OP2 )
   {
        if( anz >= 3 ) a += SRegister[GetRegOp(mnem[2])];
        else
        {
            Make_DB();
            return;
        }
   }
   else
   {
        if( anz >= 2 ) a += SRegister[GetRegOp(mnem[1])];
        else
        {
            Make_DB();
            return;
        }
   }
   a += ", ";
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
}

void Make_RM16_SREG()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        if( anz >= add )
        {
            a += ", ";
           if( Codes[index].belegt == BEL_OP1OP2 ) a +=  SRegister[GetRegOp(mnem[2])] ;
           else a+= SRegister[GetRegOp(mnem[1])];
           mnem += add;
           anz  -= add;
           Output_String( a );        
        }
        else Make_DB();
   }

}

void Make_RM16()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
}

void Make_RM8()
{
   a = Codes[index].name + CString(" ");
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
        mnem += add;
        anz  -= add;
        Output_String( a );
   }
}


void Make_PTR16_16()
{
   add = GetExtBeg( &Codes[index] );
   if( anz >= add+4 )
   {
        a = Codes[index].name + CString(" ") + CString( (WORD)BB2WORD(mnem[3],mnem[4]) ) + ":" + CString( (WORD)BB2WORD(mnem[1],mnem[2]) );
        mnem += (add+4);
        anz  -= (add+4);
        Output_String( a );
   }
   else Make_DB();
}


void Make_REL16()
{
   if( Codes[index].belegt == BEL_OP1 )
   {
        if( anz >= 3 )
        {
            int imm = BB2WORD( mnem[1],mnem[2] );
            a = Codes[index].name + CString(" NEAR ") + CString( imm );
            mnem += 3;
            anz  -= 3;
            Output_String( a );
        }
        else Make_DB();
   }
   else Make_DB();
}

void Make_REL8()
{
   if( Codes[index].belegt == BEL_OP1 )
   {
        if( anz >= 2 )
        {
            signed char imm = (signed char)mnem[1];
            a = Codes[index].name + CString(" SHORT ") + CString( (short)imm );
            mnem += 2;
            anz  -= 2;
            Output_String( a );
        }
        else Make_DB();
   }
   else Make_DB();
}


void Make_OP1ADD_REG16()
{
    a = Codes[index].name;
    a += CString(" ") + Register16[(int)mnem[0]-Codes[index].op1];
    ++mnem;
    --anz;
    Output_String( a );
}


void Make_AL_RM()
{
   a = Codes[index].name;
   a += " AL, ";
   add = GetEff( &Codes[index], a, 8 );
   if( add == -1 ) Make_DB();
   else
   {
       mnem += add;
       anz  -= add;
       Output_String( a );
   }
}

void Make_AX_RM()
{
   a = Codes[index].name;
   a += " AX, ";
   add = GetEff( &Codes[index], a, 16 );
   if( add == -1 ) Make_DB();
   else
   {
       mnem += add;
       anz  -= add;
       Output_String( a );
   }
}


void Make_IMM8()
{
    a = Codes[index].name;
    a += " BYTE ";
    add = GetExtBeg( &Codes[index] );
    if( anz > add+1 )
    {
        a += CString((unsigned int)mnem[add]);
        mnem += (add+1);
        anz  -= (add+1);
        Output_String( a );
    }
    else Make_DB();
}

void Make_IMM16()
{
    a = Codes[index].name;
    a += " WORD ";
    add = GetExtBeg( &Codes[index] );
    if( anz > add+2 )
    {
        a += CString((WORD) BB2WORD(mnem[add],mnem[add+1]) );
        mnem += (add+2);
        anz  -= (add+2);
        Output_String( a );
    }
    else Make_DB();
}

void Make_M16()
{
    a = Codes[index].name;
    a += " WORD PTR DS:[";
    add = GetExtBeg( &Codes[index] );
    if( anz > add+2 )
    {
        a += CString((WORD) BB2WORD(mnem[add],mnem[add+1]) ) + "]";
        mnem += (add+2);
        anz  -= (add+2);
        Output_String( a );
    }
    else Make_DB();
}

void Make_REG16_M()
{
    a = Codes[index].name;
    a+= " ";
    if( Codes[index].belegt == BEL_OP1 )
    {
        if( anz >= 4 )
        {
           a += Register16[GetRegOp(mnem[1])];
           a += ", WORD PTR DS:[";
           a += CString((WORD) BB2WORD(mnem[2],mnem[3]) ) + "]";
           mnem += 4;
           anz  -= 4;
           Output_String( a );
           return;
        }
        else {Make_DB();return;}
    }
    else if( Codes[index].belegt == BEL_OP1OP2 )
    {
        if( anz >= 5 )
        {
           a += Register16[GetRegOp(mnem[2])];
           a += ", WORD PTR DS:[";
           a += CString((WORD) BB2WORD(mnem[3],mnem[4]) ) + "]";
           mnem += 5;
           anz  -= 5;
           Output_String( a );
           return;
        }
        else {Make_DB();return;}
    }
    else Make_DB();    
}


void Make_REG16_IMM()
{
    a = Codes[index].name;
    a+= " ";
    if( Codes[index].belegt == BEL_OP1 )
    {
        if( anz >= 4 )
        {
           a += Register16[GetRegOp(mnem[1])];
           a += ", WORD ";
           a += CString((WORD) BB2WORD(mnem[2],mnem[3]) );
           mnem += (add+4);
           anz  -= (add+4);
           Output_String( a );
           return;
        }
        else {Make_DB();return;}
    }
    else if( Codes[index].belegt == BEL_OP1OP2 )
    {
        if( anz >= 5 )
        {
           a += Register16[GetRegOp(mnem[2])];
           a += ", WORD ";
           a += CString((WORD) BB2WORD(mnem[3],mnem[4]) );
           mnem += (add+5);
           anz  -= (add+5);
           Output_String( a );
           return;
        }
        else {Make_DB();return;}
    }
    else Make_DB();    
}


void Make_SCHIEBE_BEF()
{
    a = Codes[index].name + CString(" ");

    int sz = 8;
    CString d=", CL";

    switch( mnem[0] )
    {
        case 0xD0: d = ", 1"; break;
        case 0xC0: Make_RM8_IMM(); return;break;
        case 0xD1: d = ", 1";
        case 0xD3: sz = 16;break;
        case 0xC1: Make_RM16_IMM8();return;break;
    };
    add = GetEff( &Codes[index], a, sz );
    if( add == -1 ) Make_DB();
    else
    {
        a += d;
        mnem += add;
        anz  -= add;
        Output_String( a );
    }
}

void Encode( BYTE *m, int an ) //mnem anz
{
        mnem = m; anz = an;
        hex_offset = 0;
        do {
           oldmnem = mnem;
           //if(cs_count>0) --cs_count;
           index = Find();

            if( index == -1 ) Make_DB();
            //else if(index==-3) {}; //Make_Nothing();
            else if( index>= 0 )
           {
                switch( Codes[index].state )
                {
                   //case CS_PREF:        cs_count=2;             break;
                   case IMPLICIT:       Make_IMPLICIT();        break;
                   case AL_IMM:         Make_AL_IMM();          break;
                   case AX_IMM:         Make_AX_IMM();          break;
                   case IMM_AL:         Make_IMM_AL();          break;
                   case IMM_AX:         Make_IMM_AX();          break;
                   case RM8_IMM:        Make_RM8_IMM();         break;
                   case RM16_IMM:       Make_RM16_IMM();        break;
                   case RM16_IMM8:      Make_RM16_IMM8();       break;
                   case RM8_REG:        Make_RM8_REG();         break;
                   case RM16_REG:       Make_RM16_REG();        break;
                   case REG8_RM:        Make_REG8_RM();         break;
                   case REG16_RM:       Make_REG16_RM();        break;
                   case REL16:          Make_REL16();           break;
                   case REL8:           Make_REL8();            break;
                   case RM8:            Make_RM8();             break;
                   case RM16:           Make_RM16();            break;
                   case PTR16_16:       Make_PTR16_16();        break;
                   case OP1ADD_REG16:   Make_OP1ADD_REG16();    break;
                   case AL_RM:          Make_AL_RM();           break;
                   case AX_RM:          Make_AX_RM();           break;
                   case IMM8:           Make_IMM8();            break;
                   case IMM16:          Make_IMM16();           break;
                   case M16:            Make_M16();             break;
                   case REG16_M:        Make_REG16_M();         break;
                   case REG16_IMM:      Make_REG16_IMM();       break;
                   case SCHIEBE_BEF:    Make_SCHIEBE_BEF();     break;
                   default:             Make_DB();break;
                };
           }

        } while( index != -2 );
}




void main( int nargs, char *argc[] )
{
   COPYRIGHT_MELDUNG;
   if( nargs != 3 )
   {
          cout << "USES: DIASS86 sourcefile destfile" << endl;
        return;
   }

   srcfile = fopen( argc[1], "rb" );
   if( srcfile == NULL )
   {
        cout << "Error while opening sourcefile <" << argc[1] << ">." << endl;
        return;
   }

   long int flen = filelength( fileno( srcfile ) );
   if( flen <= 0 )
   {
        cout << "Sourcefile has illegal size." << endl;
        fclose( srcfile );
        return;
   }

   BYTE *all = (BYTE*) malloc( flen );
   if( all == NULL )
   {
       cout << "Error: To less memory to allocate." << endl; 
       fclose( srcfile );
       return;
   }
   fread( all, 1, flen, srcfile );
   fclose( srcfile );


   destfile = fopen( argc[2],"w+b");
   if( destfile == NULL )
   {
          cout << "Error while opening or creating destfile <" << argc[2] << ">." << endl;
          free( all );
          return;
   }


   CString dummy = cpyrght;
   dummy += NEXTLINE + NEXTLINE;
   dummy += "Diassemblat der Datei: <";
   dummy += CString(argc[1]) +  CString(">") + NEXTLINE + NEXTLINE + NEXTLINE;
   fputstr( destfile, (char*)dummy.GetPtr() );
   Encode(all,flen);

   free( all );
   dummy = NEXTLINE + NEXTLINE;
   fputstr( destfile, (char*)dummy.GetPtr() );
   fclose( destfile );
   cout << "Ready!!!" << endl;
}

