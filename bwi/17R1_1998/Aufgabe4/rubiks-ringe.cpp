/*****************************************************************************
*   Aufgabe Nr.4  Rubiks Ringe
*   für den 17. Bundeswettbewerb Informatik 1998
*
*   Datum:   05.11.1998
*   Autor:   Michael Neumann
*   Sprache: Borland C++ Version 5.02
*   Dauer:   Insgesamt ca. 5 Stunden
*****************************************************************************/




//
// Include-Dateien, für "cout", "getch" und "stricmp"
//
#include <iostream.h>
#include <conio.h>
#include <string.h>



//
// Definition des Typs "Kugel", und zwar als Aufzählungstyp.
//
enum Kugel { SCHWARZ=0, ROT, GELB, GRAU };


//
// Hier werden die Farben als Strings deklariert, damit sie später
// ausgegeben werden. Die Konstanten des Aufzählungstyp "Kugel" können
// in diesem Array einfach als Index benutz werden, um die Farbe als
// String zu bekommen.
//
char* KugelFarbNamen[] = { "schwarz", "rot", "gelb", "grau" };


//
// Definition des Types "Zustand" als ein Zeiger auf Kugeln
//
typedef Kugel* Zustand;


//
// Definition der Klasse "Ring".
//
class Ring {
public:	// Öffentlicher Konstruktor
	Ring(int anz, Zustand z, int schnitt_1, int schnitt_2);

public:	// Öffentliche Methoden
   void SetzePartnerRing(Ring* pk);
   void DreheLinks();
   void DreheRechts();
   void GebeZustandAus();
   bool IstZustandGleich(Zustand z);

private:	// Private Methoden
   void SetzeSchnittKugel_1(Kugel k);
   void SetzeSchnittKugel_2(Kugel k);

private:	// Private Datenelemente
	int AnzahlKugeln;
	int Schnitt_1;
   int Schnitt_2;
   Ring* PartnerRing;
	Zustand RingZustand;
};


//==============================================================================
// Hier folgt nun die Implementation der Klasse "Ring"
//==============================================================================

//
// Konstruktor der Klasse "Ring".
// "anz" = Anzahl der Kugeln im Ring
// "z"   = Zeiger auf ein Array mit den Farbwerten der Reihenfolge nach
// "schnitt_1", "schnitt_2" = Indizes der Schnittkugeln
//
Ring::Ring(int anz, Zustand z, int schnitt_1, int schnitt_2)
{
	AnzahlKugeln	= anz;
   RingZustand		= z;
   Schnitt_1		= schnitt_1;
   Schnitt_2 		= schnitt_2;
}


//
// Setzt die Variable "PartnerRing".
// "pk" = Zeiger auf den anderen Ring
//
void Ring::SetzePartnerRing(Ring* pk)
{
	PartnerRing = pk;
}


//
// Gibt den Zustand, d.h. die Farbwerte aus
//
void Ring::GebeZustandAus()
{
	for(int i=0; i<AnzahlKugeln; ++i)
   {
   	if(i>0) cout << ",";
      cout << KugelFarbNamen[RingZustand[i]];
   }
   cout << endl;
}


//
// Setzt den Farbwert der 1.Schnittkugel auf "k"
//
void Ring::SetzeSchnittKugel_1(Kugel k)
{
	RingZustand[Schnitt_1] = k;
}


//
// Setzt den Farbwert der 2.Schnittkugel auf "k"
//
void Ring::SetzeSchnittKugel_2(Kugel k)
{
	RingZustand[Schnitt_2] = k;
}


//
// Dreht den Ring nach links (gegen den Uhrzeigersinn)
//
void Ring::DreheLinks()
{
   Kugel erste = RingZustand[0];
   for(int i=0; i<AnzahlKugeln-1; ++i) RingZustand[i] = RingZustand[i+1];
   RingZustand[AnzahlKugeln-1] = erste;
   PartnerRing->SetzeSchnittKugel_1(RingZustand[Schnitt_1]);
   PartnerRing->SetzeSchnittKugel_2(RingZustand[Schnitt_2]);
}


//
// Dreht den Ring nach rechts (im Uhrzeigersinn)
//
void Ring::DreheRechts()
{
   Kugel letzte = RingZustand[AnzahlKugeln-1];
   for(int i=AnzahlKugeln-1; i>0; --i) RingZustand[i] = RingZustand[i-1];
   RingZustand[0] = letzte;
   PartnerRing->SetzeSchnittKugel_1(RingZustand[Schnitt_1]);
   PartnerRing->SetzeSchnittKugel_2(RingZustand[Schnitt_2]);
}


//
// Vergleicht den Zustand des Ringes ("RingZustand") mit dem übergebenen
// Zustand "z" und liefert im Falle von Gleichheit "true" ansonsten "false".
//
bool Ring::IstZustandGleich(Zustand z)
{
   for(int i=0; i<AnzahlKugeln; ++i)
   {
   	if(z[i] != RingZustand[i]) return false;
   }
   return true;
}


//==============================================================================
// Ende der Implementation der Klasse "Ring"
//==============================================================================



//
// Liest in das Array auf dessen Anfang "z" zeigt, die Zustände ein.
// "anz" gibt die Anzahl der Kugeln in einem Ring an.
// Der User muß die Farbwert als Strings eingeben (z.B. "schwarz").
//
void LeseZustandEin(int anz, Zustand z)
{
	char buffer[20];				// Eingabe-Buffer
	for(int i=0; i<anz; ++i)
   {
      z[i] = -1;					// als ungültig markieren

   	do
      {
	   	cout << "Farbe der Kugel Nr." << i << "  [schwarz|rot|gelb|grau]: ";
	      cin >> buffer;
      	for(int j=SCHWARZ; j<=GRAU; ++j)		// alle Farben durchgehen
      	{
      		if(stricmp(buffer,KugelFarbNamen[j])==0) z[i] = j;  // gefunden
      	}
	      if(z[i] == -1) cout << "Falsche Eingabe! Nochmal!" << endl;
      } while(z[i] == -1);		// Solange wie eine falsche Farbe eingegeben wird
   }
}


//
// Das Hauptprogramm
//
void main()
{
	const int ANZAHL_KUGELN = 20;

	Kugel a1[ANZAHL_KUGELN], a2[ANZAHL_KUGELN];	// Anfangszustände (Ring 1 u. 2)
	Kugel z1[ANZAHL_KUGELN], z2[ANZAHL_KUGELN];	// Zielzustände (Ring 1 und 2)

   cout << "Anfangszustand eingeben:" << endl;
	cout << "Zustand des Ringes Nr.1" << endl;
	LeseZustandEin(ANZAHL_KUGELN,a1);
	cout << "Zustand des Ringes Nr.2" << endl;
   LeseZustandEin(ANZAHL_KUGELN,a2);
   cout << endl;

   cout << "Endzustand eingeben:" << endl;
	cout << "Zustand des Ringes Nr.1" << endl;
	LeseZustandEin(ANZAHL_KUGELN,z1);
	cout << "Zustand des Ringes Nr.2" << endl;
   LeseZustandEin(ANZAHL_KUGELN,z2);
   cout << endl << endl;


	// 2 und 17 sind die ersten Schnittpunkte von Ring 1 und 2,
   // 7 und 12 die zweiten. (siehe Zeichnung der Dokumentation)
   Ring Ring1(ANZAHL_KUGELN, a1,  2,  7 );
	Ring Ring2(ANZAHL_KUGELN, a2, 17, 12 );

   // PartnerRing setzen
	Ring1.SetzePartnerRing(&Ring2);
	Ring2.SetzePartnerRing(&Ring1);

   int AnzahlZuege = 0;		// Anzahl der benötigten Züge
	char c;						// für Tastatureingabe

	do {
      cout << "\n----------------------------------------------------------\n";

      // Zustände beider Ringe ausgeben
      cout << "Zustand des 1. Ringes: " << endl;
      Ring1.GebeZustandAus();
      cout << endl;
      cout << "Zustand des 2. Ringes: " << endl;
      Ring2.GebeZustandAus();
      cout << endl;

      cout << "Ihre Eingabe: 1.Ring - [l]inks [r]echts; "
   		  << "2.Ring - [L]inks [R]echts; [b]eenden" << endl;
   	c = getch();

		switch(c) {
      	case 'l':	cout << "Drehe 1. Ring nach links" << endl;
                     Ring1.DreheLinks();
         				++AnzahlZuege;
                     break;

      	case 'r':	cout << "Drehe 1. Ring nach rechts" << endl;
				         Ring1.DreheRechts();
         				++AnzahlZuege;
                     break;

      	case 'L':	cout << "Drehe 2. Ring nach links" << endl;
							Ring2.DreheLinks();
         				++AnzahlZuege;
                     break;

      	case 'R':	cout << "Drehe 2. Ring nach rechts" << endl;
                     Ring2.DreheRechts();
                     ++AnzahlZuege;
                     break;

         case 'b':	cout << "Ende" << endl;
         				break;

         default:		cout << "Falsche Eingabe!" << endl;
      };		// Ende der switch-Anweisung

   } while(c != 'b');	// Ende der do-Schleife


   // Aktueller Zustand stimmt mit dem Zielzustand überein -> Spiel gewonnen
   if(Ring1.IstZustandGleich(z1) && Ring2.IstZustandGleich(z2))
   {
   	cout << "Herzlichen Glückwunsch, Sie haben gewonnen, und das mit 'nur' "
      << AnzahlZuege << " Zügen." << endl;
   }
   else cout << "Leider hatten Sie kein Erfolg bei diesem Spiel" << endl;
   getch();
}

