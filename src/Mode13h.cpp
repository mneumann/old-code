/*
   Mode 13h - Beispielprogramm
   von Michael Neumann (neumann@s-direktnet.de)

   Dies ist alles mein Code außer der Linienfunktion.
   Sie können alle Funktionen benutzen und verändern
   wie sie wollen.

   Getestet mit Turbo C++ Version 3.0
*/

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned long

#define VIDEO_SEG 0xA000

#include <conio.h>
#include <stdlib.h>


// Schaltet in den 13h Grafikmodus
void Init_13h()
{
   asm {
	mov ax,0x0013
	int 0x10
   }
}
//Schaltet in den Textmodus
void Exit_13h()
{
   asm {
	mov ax,0x0003
	int 0x10
   }
}

// Wartet auf einen vertikalen Retrace
// wird benötigt, damit der Bildschirm nicht flimmert
void WaitVRetrace()
{
   asm {
	push dx
	push ax
	mov dx,0x03DA
   }
   wait1:
   asm {
	in al,dx
	test al,8
	jnz wait1
   }
   wait2:
   asm {
	in al,dx
	test al,8
	jz wait2
	pop ax
	pop dx
   }
}

// Ohne X/Y Umrechnung - Offset errechnet sich: OFFS = (Y*320)+X
void PutPixel( WORD offs, BYTE color )
{
   asm {
	pusha
	mov di,VIDEO_SEG
	mov es,di
	mov di,[offs]
	mov al,[color]
	mov es:[di],al
	popa
   }
}

void PutPixel( WORD x, WORD y, BYTE color )
{
   asm {
	pusha
	mov di,VIDEO_SEG
	mov es,di
	mov ax,320
	mul [y]
	add ax,[x]
	mov di,ax
	mov al,[color]
	mov es:[di],al	// Könnte auch mit einer STOSB-Anweisung
				// gemacht werden, aber dann ist darauf zu
				// achten, CLD zuvor ausgeführt wird.
	popa
   }
}

// Horizontale Linie
void HLine( WORD x, WORD y, WORD w, BYTE color )
{
   WORD offs = (y*320)+x;
   for( WORD i=0; i<w; i++ ) PutPixel( offs++, color );
}

// Vertikale Linie
void VLine( WORD x, WORD y, WORD h, BYTE color )
{
   WORD offs = (y*320)+x;
   for( WORD i=0; i<h; i++ )
   {
	PutPixel( offs, color );
	offs+=320;
   }
}

// Zeichnet ein Rechteck
void Box( WORD x, WORD y, WORD w, WORD h, BYTE color )
{
   HLine( x, y, w, color );
   HLine( x, y+h-1, w, color );
   VLine( x, y, h, color );
   VLine( x+w-1, y, h, color );
}

// Zeichnet ein gefülltes Rechteck, randcolor ist die Randfarbe, fillcolor
// die Füllfarbe
void FilledBox( WORD x, WORD y, WORD w, WORD h, BYTE randcolor, BYTE fillcolor )
{
   Box( x,y,w,h,randcolor );
   for( WORD i=0;i<h-2;i++) HLine( x+1,y+1+i,w-2,fillcolor);
}

// Löscht den gesamten Bildschirm mit einer Farbe
void ClearScreen( BYTE color )
{
   asm {
	cld
	pusha
	mov di,VIDEO_SEG
	mov es,di
	xor di,di
	mov al,[color]
	mov ah,al
	mov cx,32000
	rep stosw
	popa
   }
}

// wird benötigt für die Line-Funktion
inline void Switch(int &First, int &Second)
{
   int Temp = First;
   First = Second;
   Second = Temp;
}

// Zeichnet eine Linie vom Punkt(X1,Y1) zum Punkt(X2,Y2)
void Line(int X1, int Y1, int X2, int Y2, BYTE color)
{
   int LgDelta = X2 - X1;
   int ShDelta = Y2 - Y1;
   int LgStep, ShStep, Cycle, PointAddr;

   if(LgDelta < 0) ~LgDelta,LgStep = -1;
   else LgStep = 1;

   if(ShDelta < 0) ~ShDelta, ShStep = -1;
   else ShStep = 1;

   if(LgDelta > ShDelta)
   {
	Cycle = LgDelta >> 1;
	while(X1 != X2)
	{
	 PutPixel(X1,Y1,color);
	 X1 += LgStep;
	 Cycle += ShDelta;
	 if(Cycle > LgDelta) Y1+=ShStep,Cycle-=LgDelta;
	}
   }
   else
   {
	Cycle = ShDelta >> 1;
	Switch(LgDelta,ShDelta);
	Switch(LgStep,ShStep);
	while(Y1 != Y2)
	{
	 PutPixel(X1,Y1,color);
	 Y1 += LgStep;
	   Cycle += ShDelta;
	   if(Cycle > LgDelta) X1+=ShStep,Cycle-=LgDelta;
	}
   }
}


// Beispiel Programm
void main()
{
   Init_13h();
   PutPixel( 100,100, 15 );
   HLine( 0, 0,320, 15 );
   VLine( 160, 0,200,15);
   Box( 0,0,320,200,2);
   FilledBox( 50,50,20,20,15,1);
   getch();
   while( !kbhit() )
   {
	Line( random(320), random(200), random(320), random(200), random(256) );
   }

   for(int i=0;i<256;i++)
   {
	WaitVRetrace();
	ClearScreen(i);
   }
   getch();
   Exit_13h();
}
