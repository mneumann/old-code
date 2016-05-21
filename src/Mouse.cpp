/*
Programmierung der Maus unter MS-DOS
------------------------------------

geschrieben von Michael Neumann, 1997
email: neumann@s-direktnet.de


Die Programmierung der Maus ist eigentlich sehr einfach.
Man muß nur die verschiedenen Funktionen aufrufen, die
vom Maustreiber zur Verfügung gestellt werden.
Der Maustreiber stellt seine Funktionen über den Interrupt 0x33
zur Verfügung, wobei in AX immer die Funktionsnummer übergeben wird.
Die Koordinaten beziehen sich auf 640x200, egal in welchem Modus.
Deshalb muß man im Textmode die X und Y Koordinaten durch 8 teilen,
um die Textmodus-Koordinaten zu bekommen ( 640/8=80, 200/8=25 ).
Einfacher ist es wenn man die Koordinaten um 3 nach rechts schiebt,
mittels '>> 3', wie es auch unten gemacht wird.


Beispiel-Programm in C++
*/



#include <dos.h>
#include <iostream.h> // nur für cout im Hauptprogramm
union REGS regs;


// 'CallMouse' ruft lädt die Funktionsnummer in AX und ruft
// den Mausinterrupt 0x33 auf.
void CallMouse( int fktnr )
{
   regs.x.ax = fktnr;
   int86( 0x33, &regs, &regs );
}

// Diese Funktion setzt die Maushardware zurück. Muß ganz am Anfang
// aufgerufen werden.
// 'MouseReset' gibt -1 zurück wenn kein Maustreiber vorhanden ist,
// und 0 wenn alles geklappt hat.
int MouseReset()
{
   CallMouse( 0x00 );
   if( regs.x.ax == 0xFFFF ) return 0;
   else return -1;
}

//'MouseOn' schaltet den Mauszeiger ein.
void MouseOn()
{
   CallMouse( 0x01 );
}


//'MouseOff' schaltet den Mauszeiger aus.
void MouseOff()
{
   CallMouse( 0x02 );
}



//'MouseX' gibt die X-Position des Cursors zurück.
int MouseX()
{
   CallMouse( 0x03 );
   return regs.x.cx;
}



//'MouseY' gibt die Y-Position des Cursors zurück.
int MouseY()
{
   CallMouse( 0x03 );
   return regs.x.dx;
}


//'MouseK' gibt den Status der Maustasten zurück, 0=links, 1=rechts.
int MouseK()
{
   CallMouse( 0x03 );
   return regs.x.bx;
}


// Hauptprogramm
void main()
{
   if( MouseReset() == -1 )
   {
	cout << "Kein Maustreiber/Maus vorhanden!" << endl;
	return;
   }
   MouseOn();	// Zeigt den Maus-Cursor
   while( MouseK() == 0 )
   {
	cout << "X: " << (MouseX()>>3) << " Y: " << (MouseY()>>3) << endl;
   }
   MouseOff();	// Versteckt den Maus-Cursor
}
