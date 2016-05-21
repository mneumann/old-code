/*****************************************************************************
*   Aufgabe Nr.2  Party Vorbereitungen
*   fÅr den Bundeswettbewerb Informatik 1997
*
*   Datum:   09.11.1997
*   Autor:   Michael Neumann
*   Sprache: Turbo C++ Version 3.0
*   Dauer:   0.5 Stunden programmieren, 0.75 Stunde dokumentieren
*****************************************************************************/


/*

   Lîsungsidee:
   --------------------------------------------------------------------------

   Meine Strategie geht wie folgt vor:
   Ich teile der ersten Person soviele Arbeiten wie mîglich zu,
   dann mache ich dasselbe mit der zweiten Person, dann mit der
   dritten usw., solange bis alle Arbeiten vergeben sind.
   Damit wird sichergestellt, da· nicht jeder Person einfach nur
   einer Arbeit zugeordnet wird. Es kann natÅrlich vorkommen, aufgrund
   bestimmter Zeit-Werten, und aufgrund der zur VerfÅgung stehender
   Zeit pro Person, da· jede Person nur eine Arbeit bekommt.


*/


//----------------------------------------------------------------------------
// Include-Files und Definitionen
//----------------------------------------------------------------------------

// wird fÅr clrscr und getch benîtigt
#include <conio.h>

// wird fÅr printf und scanf benîtigt
#include <stdio.h>

// wird fÅr random benîtigt
#include <stdlib.h>


//----------------------------------------------------------------------------
// Globale Variablen
//----------------------------------------------------------------------------

// Hier sind die Zeiten gespeichert, die fÅr einzelnen Aufgaben benîtigt
// werden.
int times[9] = {45,45,45,45,45,45,45,165,105};

int ZufallsArray[10000];		// hier werden die Zufallszahlen
					// aufgenommen

long real_time;				// wiviel Zeit benîtigen Sie wirklich
					// wird in 'AnzPersonen' gebraucht


//----------------------------------------------------------------------------
// Verschiedene Funktionen
//----------------------------------------------------------------------------


// Ermittelt die Gesamtzeit aus den einzelnen Zeiten im Array ti
long GesamtZeit( int n, int *ti )
{
   long gestime=0;
   for( int i=0; i<n; i++ ) gestime += ti[i];	// Gesamt-Zeit berechnen
   return gestime;
}


// Ermittelt, die Mindest-Anzahl der benîtigten Personen
int MinAnzPers( int n, int *ti, int time_person )
{
   long gestime = GesamtZeit( n, ti );
   int anz_pers = gestime / time_person;  	   // Anzahl der Personen
   if( (gestime % time_person) != 0 ) ++anz_pers;  // aufrunden
   return anz_pers;
}


// Diese Prozedure ermittelt nach meiner Stategie, wieviele Personen
// benîtigt werden.
int AnzPersonen( int n, int *ti, int zeit_pro_person )
{
   int aktpers=0;	       	// aktuelle Personen-Anzahl
   int task_avail;	       	// wenn == 1, dann ist noch eine
				// Aufgabe vorhanden
   int person_time;		// Zeit, welche die aktuelle Person
				// noch zur VerfÅgung hat

   do {
      person_time = zeit_pro_person;	// Person hat noch die gesamte Zeit
      task_avail=0;			// wird spÑter evtl. auf 1 gesetzt
      ++aktpers;			// Peronen-Anzahl um eins erhîhen

      for( int i=0;i<n;i++ )		// alle Aufgaben durchgehen
      {
	 if( person_time >= ti[i] && ti[i]>0 )	// hat Person noch Zeit
	 {
	    person_time-=ti[i];			// Zeit abziehen
	    real_time += ti[i];			// wirklich benîtigte
						// Zeit erhîhen
	    ti[i]=0;				// Aufgabe erledigt
	 }
	 if( ti[i] > 0 ) task_avail=1;		// Aufgabe noch vorhanden
      }
   } while( task_avail == 1 );			// solange noch Aufgaben
						// vorhanden

   return aktpers;
}


//----------------------------------------------------------------------------
// Hauptprogramm
//----------------------------------------------------------------------------


void main()
{
   int opt;		// hier wird die opt. Personenanzahl gespeichert
   int my;		// hier wird meine errechnete Pers.anzahl gespeichert
   clrscr();

   opt = MinAnzPers( 9, times, 300 );
   my  = AnzPersonen( 9, times, 300 );

   printf( "\nDie optimale Personenanzahl wÑre: %i\n", opt );
   printf( "Die von meiner Strategie berechnete Personenanzahl ist: %i\n", my );
   if( (my/opt) < 2 )
   {
      printf( "Meine Strategie benîtigt weniger als doppelte so viele " );
      printf( "Personen, wie im besten\nFall nîtig gewesen wÑren.\n" );
   }
   else
   {
      printf( "Meine Strategie benîtigt doppelt so viele oder mehr Personen,");
      printf( " wie im besten\nFall nîtig gewesen wÑre.\n" );
   }

   printf( "\n\nZufallsgenerator... (bitte warten)\n\n" );
   for( int i=0; i<10000; i++ ) ZufallsArray[i] = random( 20 )+1;


   real_time=0;			// wirklich benîtigte Zeit auf Null setzen

   my = AnzPersonen( 10000, ZufallsArray, 25 );


   printf( "Es werden %i Personen benîtigt.\n", my );
   printf( "Jede Person arbeitet im Durchschnitt %i Minuten.\n",
	   real_time / my );

   getch();
}
