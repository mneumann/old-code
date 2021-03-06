/*****************************************************************************
*   Aufgabe Nr.4  Wetter in Quadratien
*   f�r den Bundeswettbewerb Informatik 1997
*
*   Datum:   09.11.1997
*   Autor:   Michael Neumann
*   Sprache: Turbo C++ Version 3.0
*   Dauer:   1.0 Stunden programmieren, 1.5 Stunde dokumentieren
*****************************************************************************/


/*

   L�sungsidee:
   --------------------------------------------------------------------------

   Die Eingabe l��t sich relativ einfach bewerkstelligen, indem man
   f�r jede Wolke die Spalte und die Zeile eingibt. Nachdem die
   Positionen bestimmt sind, wird geschaut, welche Wolken sich
   in welche Richtung bewegen. Wolken links von Quadratien, bewegen
   sich von links nach rechts, Wolken oberhalb von Quadratien, bewegen
   sich von oben nach unten. Jetzt simuliert man die Wolken, indem man
   die Wolken so oft verschiebt, da� alle Quadratien verlassen haben m�ssen.
   Wolken von links werden 3 Schritte pro Takt verschoben, Wolken von oben
   2 Schritte. Nach jedem Takt wird �berpr�ft, ob sich
   zwei Wolken getroffen haben, wenn ja, dann regnet es.
   Nach der Simulation kann man dann feststellen wo und wie oft es
   geregnet hat.


   Programmdokumentation:
   --------------------------------------------------------------------------

   Ich benutze f�r 'regen_arr' und 'wolken_arr' ein dynamisches Array,
   das eine variable Gr��e des Arrays erlaubt, wenn gen�gend Speicher
   vorhanden ist. Wenn bei der Speicher-Reservierung nicht gen�gend
   Speicher vorhanden ist, dann wird eine Fehlermeldung ausgegeben, und
   das Programm beendet. Es erfolgt auch eine Fehlermeldung, wenn man
   einen unzul�ssigen Wert  eingibt. Danach darf man jedoch erneut einen
   Wert daf�r eingeben, bis dieser richtig ist.

   Ich denke der Rest ist aus dem Programm ersichtlich.


   Beispiele:
   --------------------------------------------------------------------------
   1.)  Werte von dem Aufgabenblatt
	Gr��e: 10x10
	Wolkenvorhersage:
	2/-3, -5/5, -3/4, 1/-4, 6/-12, -3/5, -7/5, 3/-10, -6/6,
	6/-11, -7/4, 3/-4, -4/5, -3/3, -6/9, 2/-4
	Regen:
	1-mal: 2/9, 1/5, 6/6, 2/5
	2-mal: 3/5

   2.) Gr��e: 3x3
       Wolkenvorhersage:
       -4/1, -3/2, 2/-4, 0/-4
       Regen: Es regnet �berhaupt nicht

   3.) Gr��e: 8x8
       Wolkenvorhersage:
       -5/3, 5/-3, 5/-1, -1/1, 0/-4
       Regen: Es regnet �berhaupt nicht
*/



//----------------------------------------------------------------------------
// Include-Files und Definitionen
//----------------------------------------------------------------------------

// wird f�r malloc und free ben�tigt
#include <malloc.h>

// wird f�r clrscr und getch ben�tigt
#include <conio.h>

// wird f�r printf und scanf ben�tigt
#include <stdio.h>



//----------------------------------------------------------------------------
// Konstanten
//----------------------------------------------------------------------------

// Status der Wolken
const int STATUS_NICHT = 0;	// nachdem zusammentreffen zweier Wolken
				// hat eine den Status STATUS_NICHT,
				// und die andere STATUS_REGEN.

const int STATUS_LINKS = 1;	// Wolke kommt von links
const int STATUS_OBEN  = 2;	// Wolke kommt von oben
const int STATUS_REGEN = 3;	// Hier hat es geregnet

const int SCHRITTE_LINKS = 3;	// Geschwindigkeit der Wolken von links
const int SCHRITTE_OBEN  = 2;	// Geschwindigkeit der Wolken von oben


//----------------------------------------------------------------------------
// Typen-Deklarationen
//----------------------------------------------------------------------------

struct wolken_pos
{
    int x,y;			// aktuelle Wolkenposition
				// wird in der Simulation-Schleife
				// ver�ndert.
    int status;			// siehe oben STATUS_XXXXX
};

struct regen_info
{
    int x,y;			// Regen-Position
    int anz;			// Anzahl der an dieser Stelle vorkommenden
				// Regenf�lle
};


//----------------------------------------------------------------------------
// Globale Variablen
//----------------------------------------------------------------------------

regen_info *regen_arr;		// Wo regnet es wie oft? (wird sp�ter allociert)
int q_lange;			// Kantenl�nge von Quadratien
int wolken_anz;			// Anzahl Wolken
wolken_pos *wolken_arr;		// Wo sind die Wolken? (wird sp�ter allociert)
int regen_anz = 0;		// Anzahl der Regenf�lle




//----------------------------------------------------------------------------
// Funktion Eingabe ( liefert -1 wenn Fehler )
//----------------------------------------------------------------------------
int Eingabe()
{
   int isok;				// Flag f�r verschiedene Schleifen

   // Einlesen der Kantenl�nge von Quadratien solange bis eine
   // richtige Eingabe erfolgt
   do {
      isok=0;
      printf( "Kantenl�nge von Quadratien: " );
      scanf( "%i", &q_lange );
      printf( "\n" );
      if( q_lange <= 0 )
      {
	 printf( "Fehler: Falsche Gr��e von Quadratien!\n" );
	 isok=1;
      }
   } while( isok!=0 );

   // Anzahl der Wolken eingeben, solange bis eine richtige Eingabe erfolgt
   do {
      isok=0;
      printf( "Anzahl Wolken: " );
      scanf( "%i", &wolken_anz );
      if( wolken_anz <=0 )
      {
	 printf( "Falsche Anzahl an Wolken\n" );
	 isok=1;
      }
   } while( isok != 0 );


   // dynamisches Wolkenarray allocieren
   wolken_arr = (wolken_pos*) malloc( sizeof(wolken_pos)*wolken_anz );
   if( wolken_arr == NULL )
   {
      printf( "Zu wenig Speicher vorhanden!\n" );
      return -1;
   }

   // Positionen von Wolken eingeben
   for( int i=0;i<wolken_anz;i++ )
   {
      printf( "\nWolke %i: \n", i+1 );
      printf( "Zeile (y): " );
      scanf( "%i", &wolken_arr[i].y );
      printf( "Spalte (x): " );
      scanf( "%i", &wolken_arr[i].x );

      if( wolken_arr[i].y>q_lange-1 || wolken_arr[i].x>q_lange-1 )
      {
	 printf( "Wolke au�erhalb der Welt! Nochmal...\n\n" );
	 --i; continue;
      }
      else if( (wolken_arr[i].x>=0 && wolken_arr[i].y>=0) ||
	       (wolken_arr[i].x<0 && wolken_arr[i].y<0) )
      {
	 // Wolken-Richtung kann nicht bestimmt werden
	 printf( "Wolke falsch positioniert! Nochmal...\n\n" );
	 --i; continue;
      }

      // Von wo kommt die Wolke?
      if( wolken_arr[i].x<0 ) wolken_arr[i].status = STATUS_LINKS; // links
      else wolken_arr[i].status = STATUS_OBEN;			   // oben
   }
   return 0;		// Alles hat geklappt
}


void main()
{
  int tackt;			// Schritt-Variable beim simulieren
  int i,j;			// Z�hlvariablen

  clrscr();			// L�scht den Bildschirm
  // Wenn Eingabe -1 zur�ckgibt, dann ist ein Fehler aufgetreten
  if( Eingabe() != 0 ) return;  // Liest Gr��e von Quadr. und Wolkenpos.

  printf( "\n\n" );


  // Nun beginnt die Simulation....

  int maxanz=0;
  // Hier wird berechnet, welches x oder y am kleinsten ist
  for( i=0;i<wolken_anz; i++ )
  {
     if( wolken_arr[i].x < maxanz ) maxanz=wolken_arr[i].x;
     if( wolken_arr[i].y < maxanz ) maxanz=wolken_arr[i].y;
  }
  // nun wird aus der Angabe errechnet, wie oft man die Wolken verschieben
  // mu�, da� alle Quadratien verlassen haben.
  maxanz= (-maxanz) + q_lange;


  for( tackt=0; tackt<maxanz; tackt++ )	    // solange bis alle Wolken weg sind
  {

     // Wolken bewegen
     for( i=0;i<wolken_anz; i++ )	// Alle Wolken durchmachen
     {
	// Nur Wolke bewegen, wenn von sie von oben oder von links kommt
	switch( wolken_arr[i].status )
	{
	   case STATUS_OBEN:  wolken_arr[i].y+=SCHRITTE_OBEN; break;
	   case STATUS_LINKS: wolken_arr[i].x+=SCHRITTE_LINKS; break;
	};
     }


     // Testen ob sich zwei Wolken �berlappen
     for( i=0;i<wolken_anz;i++)		// Alle Wolken durchmachen
     {
	for( j=0;j<wolken_anz;j++ )	// Alle Wolken durchmachen
	{
	   if( i != j )			// Wenn nicht die selbe Wolke
	   {
	      // Wenn die beiden Wolken auf dem selben Quadrat sind,
	      // und die eine von links und die andere von oben, oder
	      // umgedreht, kommen.
	      if( ( wolken_arr[i].x == wolken_arr[j].x ) &&
		  ( wolken_arr[i].y == wolken_arr[j].y ) &&
		  ((wolken_arr[i].status == STATUS_OBEN &&
		    wolken_arr[j].status == STATUS_LINKS ) ||
		   (wolken_arr[i].status == STATUS_LINKS &&
		    wolken_arr[j].status == STATUS_OBEN)) )
	      {
		 // die eine Wolke regnen lassen, die andere 'l�schen'
		 wolken_arr[i].status = STATUS_REGEN;
		 wolken_arr[j].status = STATUS_NICHT;
		 ++regen_anz;		// Regenz�hler erh�hen
	      }
	   }
	}
     }
   }

   clrscr();				// Bildschirm l�schen

   // Nun auf Regen pr�fen ( wo und wieoft )
   if( regen_anz==0 ) printf("Es regnet �berhaupt nicht\n");
   else
   {
      regen_arr = (regen_info*) malloc( sizeof(regen_info)*regen_anz );
      if( regen_arr == NULL )
      {
	 printf( "Zu wenig Speicher vorhanden!\n" );
	 free( wolken_arr );		// Wolken-Array freigeben
	 return;			// Programm beenden
      }

      for( i=0;i<regen_anz;i++) regen_arr[i].anz = 0;  // Array l�schen


      // Hier werden nun die Regenf�lle ausgegeben
      for( i=0;i<wolken_anz;i++ )
      {
	 if( wolken_arr[i].status == STATUS_REGEN )
	 {
	    int empty=-1;		// Wird weiter unten erkl�rt

	    for( j=0;j<regen_anz;j++ )
	    {
	       // ein leeres Feld -> Index ist dann in empty
	       if( regen_arr[j].anz == 0 && empty==-1 ) empty=j;

	       // Es hat schon geregnet
	       if( regen_arr[j].anz > 0 )
	       {
		  // Stimmt die Wolkenposition mit der
		  // Position wo es schon geregnet hat �berein?
		  if( regen_arr[j].x == wolken_arr[i].x &&
		      regen_arr[j].y == wolken_arr[i].y )
		  {
		     ++regen_arr[j].anz;	// Anzahl der Regenf�lle
						// an diesm Punkt erh�hen
		     empty=-1;			// kein leeres Feld
		     break;			// for-Schleife beenden
		  }
	       }
	    }

	    if( empty != -1 )			// keine �bereinstimmung
						// bisheriger Regenorte
	    {
	       // In empty steht ein noch nicht benutztes Feld
	       regen_arr[empty].x = wolken_arr[i].x;
	       regen_arr[empty].y = wolken_arr[i].y;
	       regen_arr[empty].anz = 1;	// einmal hat es geregnet
	    }
	 }
      }

      for( int n=1;n<regen_anz;n++ )		// n = Anzahl der Regenf�lle
      {
	 int nochnicht=0;			//
	 for( i=0; i<regen_anz;i++ )		// Alle Regen-Felder durchmachen
	 {
	    if( regen_arr[i].anz == n )		// Wenn die Anzahl der Regen-
						// F�lle �bereinstimmt
	    {
	       // Wenn "Es regnete "... noch nicht ausgegeben wurde, dann
	       // jetzt ausgeben
	       if( nochnicht==0 ) printf( "Es regnete %i-mal: \n",n );
	       nochnicht=1;
	       printf( "Zeile (y): %i   Spalte (x): %i\n", regen_arr[i].y, regen_arr[i].x );
	    }
	 }
	 // Wenn es 'n'-mal wo geregnet hat, dann Zeilenvorschub
	 if( nochnicht == 1 ) printf( "\n\n" );
      }
      free( regen_arr );		// Speicher freigeben
   }
   free( wolken_arr );			// Speicher freigeben
   getch();				// Auf Taste warten
}
