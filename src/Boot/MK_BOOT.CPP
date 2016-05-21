/*
   MK_BOOT.CPP -> MK_BOOT.EXE

   Writes an own bootsector on disk in drive A.
   You have to create bootsec.h (look at bootsec.asm), and change
   in bootsec.h 'xxxxxx' to 'bootbuf' before you compile this program.
   Note: the boot-program cannot be larger than 512 bytes.

   Compiled with Turbo C++.
   Copyright (c) 1997 by Michael Neumann (neumann@s-direktnet.de)
*/

#include <iostream.h>
#include <dos.h>
#include <conio.h>
#define BYTE unsigned char

#define DISK_DRIVE 0		// 0=A:, 1=B, 80=C
#define ANZ_VERSUCHE 3

int Versuche, i;
union REGS regs;
struct SREGS sregs;

#include "bootsec.h"	// this is the program which will be written
				// to the bootsector


void DiskReset()
{
   regs.h.ah = 0;
   regs.h.dl = DISK_DRIVE;
   int86( 0x13, &regs, &regs );
}

int WriteSektors( BYTE anz, BYTE drive, BYTE seite, BYTE seknum, BYTE spur, char *buf)
{
   Versuche = ANZ_VERSUCHE;
   do {
	regs.h.ah = 3;
	regs.h.al = anz;
	regs.h.dl = drive;
	regs.h.dh = seite;
	regs.h.cl = seknum;
	regs.h.ch = spur;
	regs.x.bx = FP_OFF( buf );
	sregs.es  = FP_SEG( buf );
	int86x( 0x13, &regs, &regs, &sregs );
	if( regs.x.cflag )
	{
	   DiskReset();
	   --Versuche;
	   if( Versuche == 0 ) return -1;
	}
	else Versuche = 0;
   } while( Versuche != 0 );
   return 0;
}


void main()
{
   cout << "Create a bootsector in drive A [1.44 HD]." << endl <<
	     "Press <Space> to create or any other key to cancel." << endl;

   if( getch() != ' ' )
   {
	cout << "Abort creating bootsector!!!" << endl;
	return;
   }

   cout << "Writing bootsector...";
   if( WriteSektors( 1, DISK_DRIVE, 0, 1, 0, bootbuf ) == -1 )
   {
	cout << " Error!" << endl;
	return;
   }
   cout << " ready." << endl;
   cout << endl << "Please reboot your computer with the disk in drive A." << endl;
}

