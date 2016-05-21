/*****************************************************************************
*   Aufgabe Nr.3  Rekursive Besen
*   fÅr den Bundeswettbewerb Informatik 1997
*
*   Datum:   08.11.1997
*   Autor:   Michael Neumann
*   Sprache: Turbo C++ Version 3.0
*   Dauer:   2.5 Stunden programmieren, ca. 2 Stunden ausprobieren,
*	     3 Stunden dokumentieren.
*****************************************************************************/


/*

   1. Frage und Lîsungsidee:
   --------------------------------------------------------------------------

   Die Vision ist vermutlich wie folgt entstanden:

   Wenn man die waagrechte Linie als Basis-Linie nimmt,
   dann sind die zwei anderen Linien durch
     a) Verschiebung (Translation)
     b) Drehung (Rotation) und
     c) Stauchung bzw. Streckung ( im folgenden wird nur noch vom Stauchungs-
	faktor die Rede sein, der Einfachkeit halber )
   der Basis-Linie entstanden.
   Im zweiten Bild werden die durch unterschiedliche Parameter fÅr
   a), b) und c) entstandenen zwei Linien wiederum als Basis-Linien
   behandelt, aus denen nach dem gleichen Prinzip wiederum zwei neue
   Basis-Linien erzeugt werden. Die Basis-Linien verdoppeln sich also mit
   jedem Schritt. So ist es im ersten Bild eine Basis-Linie, im zweiten
   sind es schon zwei, im dritten vier usw. Dieses Problem lÑ·t sich am
   einfachsten durch Rekursion bewÑltigen. Dabei ist darauf zu achten,
   da· keine zu gro·e Rekursions-Tiefe gewÑhlt wird, weil dies
   sehr lange dauern kann, da sich bei jedem Schritt die Anzahl
   der Basis-Linien (éste) verdoppelt (2 hoch Rekursions-Tiefe).
   Schlimmstenfalls kann es zu einem Absturz des Systems fÅhren,
   oder zu einer Fehlermeldung wie z.B. "StackÅberlauf", da bei jedem
   Prozedure-Aufruf der Stack wÑchst.



   Programmdokumentation:
   --------------------------------------------------------------------------

   Im Programm sind zuerst verschiedene Strukturen definiert, wie
   z.B. Point, Line, Vektor, die im Programm hÑufig verwendet werden.
   Diese Strukturen sind meiner Ansicht nach selsterklÑrend.

   Die Strukur 'definition' ( Variable 'def' ) stellt alle Parameter fÅr
   die Transformation der Linien dar. Dabei ist zu beachten, da· alle
   Richtungsangaben in diesem Programm durch Bogenma· beschrieben werden.
   Man kann die Funktion 'grad2bog' benutzen um Grad in Bogenma· umzuwandeln.
   0¯ ist bei 12 Uhr (Norden), 90¯ bei 9 Uhr, 180¯ bei 6 Uhr, usw.
   Es gelten auch Minusanaben.

   Der Prozedure 'Calculate' werden Basislinie (l), Rotation (bogen),
   Stauchungsfaktor (faktor), Translationsvektor (v), resultierende
   Linie (retval) die durch die drei VerÑnderungen ( siehe oben a), b), c) )
   entstanden ist, und als letzter Parameter (next) auch die resultierenede
   Linie, jedoch nicht wie 'retval' als zwei Punkte Paare, sondern als
   ein Punkt und ein Vektor. Man beachte, da· die zwei letzten Parameter
   ( retval, next ) Referenzen sind, d.h. die Åbergebene Variable wird
   durch die Prozedure verÑndert.

   Die Prozedure 'Fraktal' ist die Prozedure, die rekursiv aufgerufen wird.
   Ihr wird immer eine Basis-Linie (l) Åbergeben. In der Prozedure werden
   dann durch diese Basis-Linie, und durch die Struktur 'definition'
   ( Variable 'def' ) zwei Linien erzeugt ( durch Rotation, Translation und
   Stauchung der Basis-Linie ), welche zuerst gezeichnet werden, und
   anschlie·end 'Fraktal' mit diesen neu errechneten Linien aufgrufen wird.
   Die Variable 'rekurs_anz' gibt an wie tief die rekursion ausgefÅhrt werden
   soll. Der Wert 1 steht fÅr das Anfangs-Objekt entsprechend Bild 1 auf dem
   Aufgabensblatt. In jedem Aufruf der Prozedure wird zuerst am Anfang
   die Variable 'rekurs_depth' erhîht. Sie gibt die aktuelle Tiefe der
   Rekursion an. Wenn diese nun 'rekurs_anz' Åberschreitet, wird nicht mehr
   erneut rekursiert, d.h. die Prozedure 'Fraktal' ruft sich nicht mehr
   selbst auf, sondern erniedrigt 'rekurs_depth' und beendet die Prozedure,
   d.h. es wird zur aufrufenden Prozedure gesprungen.

   Ich denke der Rest des Programms erklÑrt sich von selbst.
   Anzumerken ist noch, da· man dieses Programm in der Geschwindigkeit,
   sehr stark optimieren kînnte, indem man die Sinus- und Cosinus-Werte
   vorher berechnet, so da· diese nicht bei jedem Aufruf erneut berechnet
   werden mÅssen. Desweiteren kînnte man die Grafikausgabe sehr stark
   optimieren, in dem man zum Beispiel nicht die BGI-Treiber von Borland
   benutzt sondern sich seine eigenen Pixel- und Linien-Funktionen schreibt.



   Zur 2. Frage:
   --------------------------------------------------------------------------

   Die verlangten Bilder, sind in dem Verzeichnis A:\AUFGABE3 auf der
   beigelegten Diskette. Sie hei·en BESEN0.PCX, BESEN1.PCX bis BESEN6.PCX.
   Ich habe noch zwei weitere Bilder ( BESEN7 und BESEN8 ) hinzugefÅgt.
   Die Bilder sind im sehr bekannten PCX-Dateiformat von ZSoft.



   Zur 3. Frage:
   --------------------------------------------------------------------------

   Die verlangten Bilder sind im Verzeichnis A:\AUFGABE3 auf der
   beigelegten Diskette. Sie hei·en FRAKTAL1.PCX,FRAKTAL2.PCX,FRAKTAL3.PCX,
   usw.



   Zum diesem Beispiel-Programm:
   --------------------------------------------------------------------------

   Nach jedem Schritt wird auf eine Taste gewarten, damit Sie sich
   das Bild in Ruhe anschauen kînnen. Wenn Sie abbrechen wollen, tippen
   Sie einfach ESC ein. Beachten Sie, da· Sie nicht zu oft eine Taste
   drÅcken, denn es kînnte sein, da· Sie dann sehr lange warten mÅ·ten,
   oder, da· eventuell Ihr Computer abstÅrzt.
   Am Ende dieses Textes, ist noch ein 2. Hauptprogramm angegeben,
   welches auf Kommentar gesetzt ist. Wenn Sie dieses verwenden
   wollen, mÅssen Sie das erste Hauptprogramm auf Kommentar setzen,
   und beim 2. die Komentare wegmachen.

   Achtung: Sie mÅssen immer EGAVGA.BGI in dem Verzeichnis haben,
	    indem Sie auch dieses Programm ( AUFGABE3.EXE ) haben.
*/




//----------------------------------------------------------------------------
// Include-Files und Definitionen
//----------------------------------------------------------------------------

// wird fÅr die Grafikausgabe benîtigt
#include <graphics.h>

// wird fÅr getch() verwendet
#include <conio.h>

// wird fÅr printf verwendet
#include <stdio.h>

// wird fÅr cos, sin und normalerweise fÅr M_PI verwendet.
#include <math.h>
#include <mem.h>
#include <iostream.h>
// Falls M_PI nicht definiert ist, wird hier M_PI definiert
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif




//----------------------------------------------------------------------------
// Typen-Deklarationen
//----------------------------------------------------------------------------

struct Point
{
   int x,y;
};

struct Vektor
{
   double len;	// LÑnge des Vektors
   double bog;	// Richtung in Bogenma· (siehe Dokumentation)
};

struct Line
{
   Point p;
   Vektor v;
};

struct RealLine
{
   Point p0,p1;
};

/*
   fak1,fak2 	     = Stauchungssfaktor fÅr Linie 1 und 2 in Bezug auf
		       die LÑnge der Basis-Linie
   vekfak1,vekfak2   = Stauchungsfaktoren der Verschiebungs-Vektoren in Bezug
		       auf die LÑnge der Basis-Linie
   bog1, bog2	     = Drehung der Linien 1 und 2 in Bezug auf die Richtung
		       der Basis-Linie
   vekbog1, vekbog2  = Richtungen der Verschiebungs-Vektoren


   Struktur fa·t alle Informationen fÅr das Aussehen des Fraktals zusammen.
*/
struct definition
{
   double fak1, fak2, vekfak1, vekfak2;
   double bog1, bog2, vekbog1, vekbog2;
};




//----------------------------------------------------------------------------
// grad2bog - Funktion
//----------------------------------------------------------------------------

// Funktion zum Umwandeln von Grad nach Bogenma·, da sin und cos als
// Parameter nur Bogenma· akzeptieren.
double grad2bog( double grad )
{
   return ( grad*M_PI/180.0 );
}



//----------------------------------------------------------------------------
// Globale Variablen
//----------------------------------------------------------------------------


int rekurs_anz   = 10;		// Anzahl der Rekursionen
int rekurs_depth = 0;		// Aktuelle Tiefe der Rekursion

// Besen-Gebilde
definition def0 =
{
	0.6, 0.6, 0.24, 1.0,     //0.6,0.6,0.3,1.1
	grad2bog( 45.0 ), grad2bog( 80.0 ), //45 75
	grad2bog( 150.0 ), grad2bog( 21.0 ) //150 32
};
Line urlinie0 =
{
	270, 100, 160, grad2bog( -90.0 )
};

definition def;
Line urlinie;


//----------------------------------------------------------------------------
// Weitere Gebilde
//----------------------------------------------------------------------------

// schînes kreisfîrmiges Gebilde
definition def1 =
{
   -0.33, 0.999, 1.0, -1.0,
   grad2bog(-40.0), grad2bog(-30.0),
   grad2bog(-99.0), grad2bog(-55.0)
};
Line urlinie1 =
{
   320, 200, 20, grad2bog( -90.0 )
};


// ein weiteres sehr schînes Gebilde
definition def2 =
{
   -0.9,0.6,2.0,-2.0,
   grad2bog(40.0), grad2bog(30.0),
   grad2bog(99.0), grad2bog(55.0)
};
Line urlinie2 =
{
   350, 240, 50, grad2bog( 0.0 )
};


// wie oben, nur ein wenig exakter
definition def3 =
{
   -0.9,0.6,2.0,-2.0,
   grad2bog(40.0), grad2bog(30.0),
   grad2bog(-99.0), grad2bog(55.0)
};
Line urlinie3 =
{
   240, 275, 100, grad2bog( 0.0 )
};


// Ñhnlich wie oben
definition def4 =
{

   -0.9,0.5,1.0,-2.0,
   grad2bog(40.0), grad2bog(30.0),
   grad2bog(-99.0), grad2bog(55.0)
};
Line urlinie4 =
{
   250, 200, 100, grad2bog( 45.0 )
};


// Ein Baum, wie auf Ihrem Aufgaben-Blatt
definition def5 =
{
   0.9,0.9,1.0,1.0,
   grad2bog(20.0), grad2bog(-20.0),
   grad2bog(0.0), grad2bog(0.0)
};
Line urlinie5 =
{
   300, 100, 40, grad2bog( 0.0 )
};



//----------------------------------------------------------------------------
// Grafik-Funktionen
//----------------------------------------------------------------------------

// Linie wird nach dem normalen mathematischen Koordinatensystem
// gezeichnet, nicht nach dem des Computers.
void rline( RealLine l )
{
   line( l.p0.x, getmaxy()-l.p0.y, l.p1.x, getmaxy()-l.p1.y );
}

// Schaltet in den Grafikmodus (12h), mit der Auflîsung 640x480 bei 16-Farben
int InitGfx()
{
   int gdriver = DETECT, gmode, errorcode;
   initgraph(&gdriver, &gmode, "");
   errorcode = graphresult();

   if( errorcode != grOk )	// Fehler aufgetreten
   {
      printf("Fehler beim Initialisieren des Grafiktreibers");
      getch();
      return -1;
   }
   return 0;
}

// Beendet den Grafik-Modus
void ExitGfx()
{
   closegraph();
}




//----------------------------------------------------------------------------
// Funktion zum Rotieren, Verschieben und Stauchen einer Linie
//----------------------------------------------------------------------------

/*
   Hier wird folgende Formel benutzt:
   x = ( x * cos( winkel ) ) - ( y * sin( winkel ) )
   y = ( x * sin( winkel ) ) + ( y * cos( winkel ) )
*/
void Calculate( Line l, double bogen, double faktor, Vektor v,
     RealLine &retval, Line &next )
{
   Point add;
   add.x = - (int)( v.len * sin(v.bog) );	// Verschiebungs-Vektor
   add.y =   (int)( v.len * cos(v.bog) );	// wird berechnet



   // Hier werden zwei Punkt-Paare errechnet, die zum Zeichnen
   // benîtigt werden...

   // Formel wie oben, nur da· x=0 ( damit fÑllt x*sin oder x*cos weg ).
   // Ich berechne erstmal von einem vorgestellten Nullpunkt
   // und der LÑnge der gestauchten Basis-Linie ( l.v.len*faktor )
   // als Radius eines Kreises ( 12 Uhr Position, y=LÑnge ), den
   // Endpunkt der gestauchten und rotierten Linie. Diese wird spÑter
   // verschoben.
   retval.p1.x = - (int)( l.v.len * faktor * sin(l.v.bog+bogen) );
   retval.p1.y =   (int)( l.v.len * faktor * cos(l.v.bog+bogen) );


   // Der angenommene Nullpunkt mu· noch um den Vektor (add), und
   // um den Startpunkt der Basis-Linie verschoben werden -> Startpunkt
   // der errechneten Linie
   retval.p0.x = l.p.x + add.x;
   retval.p0.y = l.p.y + add.y;


   // Der Endpunkt der errechneten Linie mu· ebenfalls noch um
   // den Vektor (add), und um den Startpunkt der Basis-Linie
   // verschoben werden.
   retval.p1.x += ( add.x + l.p.x );
   retval.p1.y += ( add.y + l.p.y );


   // Hier wird die oben errechnete Linie in einen Punkt und Vektor
   // umgerechnet. Das wird benîtigt, da diese spÑter wieder als
   // Basis-Linie ist dieser Form hier Åbergeben wird ( Struktur Line ).

   next.v.len = l.v.len * faktor;	// Stauchen der LÑnge
   next.v.bog = l.v.bog + bogen;	// Rotieren
   next.p.x = retval.p0.x;              // Startpunkt ist wie der,
   next.p.y = retval.p0.y;		// der in retval berechneten
					// Linie in Punkteform
}



//----------------------------------------------------------------------------
// rekursiv aufgerufenen Funktion, die das ganze Fraktal berechnet
//----------------------------------------------------------------------------

void Fraktal( Line l )
{
   Vektor v1, v2;	// Verschiebungs-Vektoren
   RealLine rl1,rl2;	// Erzeugten Linien
   Line lin1,lin2;	// ebenfalls erzeugt Linien, jedoch anderes Format

   ++rekurs_depth;	// Rekursions-Tiefe erhîhen

   v1.len = l.v.len*def.vekfak1;	// Verschiebungs-Vektoren berechnen
   v1.bog = l.v.bog+def.vekbog1;        //     "           "          "
   v2.len = l.v.len*def.vekfak2;        //     "           "          "
   v2.bog = l.v.bog+def.vekbog2;        //     "           "          "

   // Nun werden die zwei neuen Linien berechnet
   Calculate( l, def.bog1, def.fak1, v1, rl1, lin1 );
   Calculate( l, def.bog2, def.fak2, v2,rl2, lin2 );

   // Hier werden die zwei neuen Linien gezeichnet
   rline( rl1 );
   rline( rl2 );

   // Wenn die maximale Rekursionstiefe noch nicht erreicht wurde, dann
   // erneut 'Fraktal' mit den neu errechneten Linien aufrufen
   if( rekurs_depth<rekurs_anz )
   {
      Fraktal( lin1 );
      Fraktal( lin2 );
   }

   --rekurs_depth;	// Rekursiontiefe erniedrigen
			// sonst wird z.B. nur der am linksten
			// stehende Ast gezeichnet
}



void copydef( definition *d, Line *l )
{
   memcpy( &def, d, sizeof( definition ) );
   memcpy( &urlinie, l, sizeof( Line ) );
}
void eingabe()
{
   clrscr();
   printf( "FRAKTAL-Programm (c) 1997 by Michael Neumann\n\n\n" );

   printf("Urlinie bestimmen...( im math. Koordinaten-System)\n");
   printf("Startpunkt X: ");
   cin >> urlinie.p.x;
   printf("Startpunkt Y: ");
   cin >> urlinie.p.y;
   printf("LÑnge des Vektors: ");
   cin >> urlinie.v.len;
   printf("Richtung des Vektors (in Grad 0¯=12 Uhr  90¯=9 Uhr...): ");
   cin >> urlinie.v.bog;
   urlinie.v.bog = grad2bog(urlinie.v.bog);

   printf("\n\nNun die Parameter fÅr das Fraktal...\n");
   printf("Stauchungs-Faktor 1: ");
   cin >> def.fak1;
   printf("Stauchungs-Faktor 2: ");
   cin >> def.fak2;
   printf("Stauchungs-Faktor Vektor 1: ");
   cin >> def.vekfak1;
   printf("Stauchungs-Faktor Vektor 2: ");
   cin >> def.vekfak2;

   printf("Drehung 1: ");
   cin >> def.bog1;
   printf("Drehung 2: ");
   cin >> def.bog2;
   printf("Vektor-Drehung 1: ");
   cin >> def.vekbog1;
   printf("Vektor-Drehung 2: ");
   cin >> def.vekbog2;

   def.bog1	= grad2bog( def.bog1 );
   def.bog2	= grad2bog( def.bog2 );
   def.vekbog1	= grad2bog( def.vekbog1 );
   def.vekbog2	= grad2bog( def.vekbog2 );

}

//----------------------------------------------------------------------------
// Hauptprogramm
//----------------------------------------------------------------------------
   RealLine a;				// wird zum zeichnen benîtigt
   Line     b;				// wird nicht benîtigt
   Vektor   x = {0, grad2bog(0.0)};	// Richtung
   int i;

void main()
{
   banf:

   clrscr();
   printf( "FRAKTAL-Programm (c) 1997 by Michael Neumann\n\n\n" );
   printf( "0-5, <E>ingabe, <Q>uit:  " );

   switch(getche())
   {
      case '0': copydef(&def0, &urlinie0); break;
      case '1': copydef(&def1, &urlinie1); break;
      case '2': copydef(&def2, &urlinie2); break;
      case '3': copydef(&def3, &urlinie3); break;
      case '4': copydef(&def4, &urlinie4); break;
      case '5': copydef(&def5, &urlinie5); break;
      case 'E':
      case 'e': eingabe(); break;
      case 'Q':
      case 'q': goto bend;
      default: goto banf;
   };


   if( InitGfx() != 0 ) return;		// Grafik initialisieren

   Calculate( urlinie, grad2bog(0.0), 1.0, x, a, b ); // Urlinie berechnen...
   rline( a );				// ... und zeichnen

   i=1;
   do {
      rekurs_anz = i;			// Rekursions-Tiefe einstellen
      ++i;				// beim nÑchsten mal, eins tiefer
      Fraktal( urlinie );		// Rekursion starten
   } while( getch() != 27 );		// solange nicht ESC gedrÅckt wird

   ExitGfx();				// Grafikmodus ausschalten
   goto banf;
   bend:
}


/*
//----------------------------------------------------------------------------
// 2. Hauptprogramm ( hiermit werden gleich alle Schritte gezeichnet )
//----------------------------------------------------------------------------

void main()
{
   if( InitGfx() != 0 ) return;		// Grafik initialisieren

   RealLine a;				// wird zum zeichnen benîtigt
   Line     b;				// wird nicht benîtigt
   Vektor   x = {0, grad2bog(0.0)};	// Richtung
   Calculate( urlinie, grad2bog(0.0), 1.0, x, a, b ); // Urlinie berechnen...
   rline( a );				// ... und zeichnen

   Fraktal( urlinie );			// Rekursion starten

   getch();
   ExitGfx();				// Grafikmodus ausschalten
}
*/
