/*****************************************************************************
*   Aufgabe Nr.1  Cheese-Champions
*   für den 17. Bundeswettbewerb Informatik 1998
*
*   Datum:   08.11.1998
*   Autor:   Michael Neumann
*   Sprache: Borland C++ Version 5.02
*   Dauer:   Insgesamt ca. 4 Stunden
*****************************************************************************/


#include <conio.h>		// für "getch"
#include <iostream.h>	// für "cout"
#include <list.h>			// für Template-Klasse "list"
using namespace std;		// "list" liegt im namespace "std"!



//
// definiert eine einzelne Maus, mit Eintrittnummer und Anzahl der
// gewonnenen Zweikämpfe
//
struct Maus {

	// Default-Konstruktor wird von "list" benötigt
	Maus() {}

	Maus(int nr) : nummer(nr), anz_gewinne(0) {}

   // "<" wird von der Klasse "list" benötigt. Ist dummy, da ich keine
   // Kleiner-Vergleiche benötige, sondern nur Vergleiche auf Gleich- und
   // Ungleichheit
	bool operator <(const Maus& m)  { return false; }
   bool operator ==(const Maus& m) { return nummer == m.nummer; }
   bool operator !=(const Maus& m) { return nummer != m.nummer; }

   int nummer;				// Eintrittsnummer
   int anz_gewinne;		// Anzahl der gewonnenen Zweikämpfe
};


//
// Die in der zweiten Zeichnung der Dokumentation gezeigten Listen.
// i=Eingang, r=rechte Seite, o=Ausgang, a=Austrittsreihenfolge
//
list<Maus> liste_i, liste_r, liste_o, liste_a;


//
// Status der Funktion F, der zurückgeliefert wird.
// ENDE = Simulation zu Ende
// FEHLER_IN_REIHENFOLGE bedeutet = Austrittsreihenfolge ist nicht möglich
// NOCHMAL = Funktion F muß nochmal aufgerufen werden
//
enum status {ENDE, FEHLER_IN_REIHENFOLGE, NOCHMAL};


//
// Die Funktion F, die die Simulation durchführt
//
status F()
{
	// ist Simulation abgeschlossen?
	if( liste_i.empty() && liste_r.empty() ) return ENDE;

   // Eingang ist leer
   else if( liste_i.empty() )
   {
   	// rechte Seite stimmt mit Austrittsreihenfolge nicht überein -> Fehler
   	if( liste_r.front() != liste_a.front() ) return FEHLER_IN_REIHENFOLGE;
      // ...stimmt überein
      else
      {
      	// von der rechten Seite das erste Element entfernen und beim Ausgang
         // hinten anhängen und das erste Element der Austrittsreihenfolge
         // löschen
			liste_o.push_back(liste_r.front());
         liste_r.pop_front();
         liste_a.pop_front();
         return NOCHMAL;
      }
   }

   // rechte Seite ist leer
   else if( liste_r.empty() )
   {
   	// vom Eingang erstes Element entfernen und auf der rechten Seite vorne
      // einfügen
   	liste_r.push_front(liste_i.front());
      liste_i.pop_front();
      return NOCHMAL;
   }

   // Eingang und rechte Seite haben noch Elemente
   else
   {
   	// rechte Seite hat gewonnen
   	if( liste_r.front() == liste_a.front() )
   	{
      	// bei dem rechten Element(Maus) "anz_gewinne" erhöhen, und von der
         // rechten Seite entfernen und am Ausgang hinten einfügen und das
         // erste Element der Austrittsreihenfolge entfernen.
      	++liste_r.front().anz_gewinne;
      	liste_o.push_back(liste_r.front());
      	liste_r.pop_front();
      	liste_a.pop_front();
   	}
      // Eingang hat gewonnen
   	else
   	{
      	// bei der Maus vom Eingang "anz_gewinne" erhöhen, und vom Eingang
         // entfernen und auf der rechten Seite vorne einfügen
      	++liste_i.front().anz_gewinne;
      	liste_r.push_front(liste_i.front());
      	liste_i.pop_front();
   	}
   	return NOCHMAL;
   }
}


//
// Das Hauptprogramm
//
void main()
{
	int anz, nr;
   status res;

   cout << "Anzahl der Mäuse: "; cin >> anz;
   cout << "nun bitte die Austrittsreihenfolge eingeben" << endl;

	// Austrittsreihenfolge und Eintrittsfolge eingeben bzw. generieren
	for(int i=1; i<=anz; ++i)
   {
   	cout << i << ".: "; cin >> nr;
   	liste_a.push_back(nr);
   	liste_i.push_back(i);
   }

   // Funktion F solange kein Fehler auftritt oder das Ende erreicht
   // ist aufrufen
   while( (res=F()) == NOCHMAL );


   if( res == FEHLER_IN_REIHENFOLGE )
   {
   	cout << "Falsche Austrittsreihenfolge!" << endl;
   }
   else if( res == ENDE )
   {
   	cout << "Austrittsreihenfolge war in Ordnung." << endl;
	   while( !liste_o.empty() )
      {
      	cout << "Maus Nr." << liste_o.front().nummer << " mit " <<
         liste_o.front().anz_gewinne << " gewonnenen Zweikämpfe" << endl;
         liste_o.pop_front();
		}
   }
   
	getch();
}
