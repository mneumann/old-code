
// 16. Bundeswettbewerb Informatik 1997/98    2. Runde
// Aufgabe 16.2.3 �bersetzungen

// Aufgabe3.Cpp
// Programmiert von Michael Neumann, H�lderlinstra�e 6, 75233 Tiefenbronn



// Dies ist die Hauptdatei, in der die Aufgaben bew�ltigt werden

# include "Aufgabe3.Hpp"



// wenn "which_way"==1 dann wird nur der optimale Weg genommen, ansonsten alle
int which_way=1;

vector<bool> can_trans_vgl(256), can_trans_x(256);
int cnt;

// wird "link_sprachen" als Parameter �bergeben
int link_art=1;


//==============================================================================
// Pr�ft ob s1 in s2 �bersetzt werden kann
void auf_1(Sprache* s1, Sprache* s2)
{
   if(kann_uebersetzen(s1,s2))
   	fs << s1->name << " kann in " << s2->name << " �bersetzt werden!" << endl;
}
//==============================================================================

//==============================================================================
// Stellt fest, �ber wieviele Zwischenstationen geht die �bersetzung mindestens
void auf_2_3(Sprache* s1, Sprache* s2)
{
	s1->starte_uebersetzung(s2);
   fs << s1->name << " -> " << s2->name << ":   ";
   if(alle_wege.empty()) fs << "keine �bersetzung m�glich!";
   else
   {
   	// wegen der Minimumermittlung. Kein Wert kann gr��er als 9999 sein.
      // Es k�nnte auch 1111 sein oder 2029 (einfach irgendein gro�er Wert)
      int min=9999;
   	list<list<WayDef> >::iterator a;
      for(a=alle_wege.begin(); a!=alle_wege.end(); a++)
      {
      	int i=(*a).size()-1;
         if( min > i ) min=i;
      }
      fs << min;
   }
   fs << endl;
}
//==============================================================================

//==============================================================================
// Diese Prozedure initialisiert das Array "can_trans_vgl".
// Das Array hat f�r jedes Sprachepaar einen Eintrag, der gesetzt wird, wenn
// die das Sprachpaar �bersetzt werden kann (nur die eine Richtung)
void auf_4a(Sprache* s1, Sprache* s2)
{
	if(kann_uebersetzen(s1,s2)) can_trans_vgl[cnt++] = true;
   else can_trans_vgl[cnt++] = false;
}
//==============================================================================

//==============================================================================
// Wie oben, nur da� die Werte nicht in "can_trans_vgl" sonder in "can_trans_x"
// geschrieben werden.
void auf_4(Sprache *s1, Sprache *s2)
{
	if(kann_uebersetzen(s1,s2)) can_trans_x[cnt++] = true;
   else can_trans_x[cnt++] = false;
}
//==============================================================================

//==============================================================================
// Stellt fest welche Sprachen als Zwischenstationen wichtiger sind
void auf_5(Sprache* s1, Sprache* s2)
{
  	list< list<WayDef> >::iterator a,ma;
  	list<WayDef>::iterator b;

	s1->starte_uebersetzung(s2);
   if(!alle_wege.empty())
   {
   	if(which_way==1)			// nur der optimale Weg
      {
         for( ma=a=alle_wege.begin(); a!=alle_wege.end(); a++ );
         	if( (*ma).size() > (*a).size() ) ma=a;
         	// ma ist die Liste mit dem k�rzesten Weg

         for( b=(*ma).begin(), b++;  b!=(*ma).end(); b++ )
         	(*b).src->important_counter++;
            // hier wird der Z�hler in der Klasse hochgez�hlt
      }
      else							// alle Wege
      {
      	for( a=alle_wege.begin(); a != alle_wege.end(); a++ )
        		for( b=(*a).begin(),++b; b != (*a).end(); b++ )
            	(*b).src->important_counter++;
               // hier wird der Z�hler in der Klasse hochgez�hlt
      }
   }
}
//==============================================================================



//==============================================================================
// A U F G A B E  1
void aufgabe_1()
{
   fs << "AUFGABE 1  -  Wann k�nnen zwei Sprachen �bersetzt werden?" << endl << endl;
   link_sprachen(link_art);
   fuer_alle_paare(auf_1);
}
//==============================================================================

//==============================================================================
// A U F G A B E  2
void aufgabe_2()
{
   fs << "AUFGABE 2  -  �ber wieviele Zwischenstationen geht die �bersetzung mindestens?" << endl << endl;
	max_persons=-1;
   link_sprachen(link_art);
   fuer_alle_paare(auf_2_3);
}
//==============================================================================

//==============================================================================
// A U F G A B E  3
// "mp" kann auch einen anderen Wert als 2 annehmen
void aufgabe_3(int mp=2)
{
	max_persons=mp;
   fs << "AUFGABE 3  -  �ber wieviele Zwischenstationen geht die �bersetzung mindestens, wenn nicht mehr als " << max_persons << " Personen daran beteiligt sein d�rfen?" << endl << endl;
   link_sprachen(link_art);
   fuer_alle_paare(auf_2_3);
}
//==============================================================================

//==============================================================================
// A U F G A B E  4
// Wenn Personen auf Dienstreise sind, k�nnen einige Sprachen nicht mehr
// �bersetzt werden und die Wege werden unter Umst�nden l�nger.
// Die M�glichkeiten einer �bersetzung (festgehalten in alle_wege) nehmen ab
// Das Befinden auf Dienstreise wird durch das Setzten von "person_da[x]" auf
// false angezeigt

void aufgabe_4()
{
   fs << "AUFGABE 4  -  Was wenn Personen auf Dienstreise gehen? Welche k�nnen gleichzeitig weg sein" << endl << endl;
   max_persons=-1;
   link_sprachen(link_art);

   // Feststellen welche Personen gleichzeitig weg sein k�nnen
   cnt=0;
   fuer_alle_paare(auf_4a);		// Vergleichs-Array initialisieren

   // Diese For-Schleife probiert alle M�glichkeiten durch
   // Da es 8 Personen sind, gibt es 2^8 M�glichkeiten
   for(int i=1;i<255;i++)
   {
   	int j=i;
      // i als Bitkombination sehen, und das Array "person_da" entsprechen setzen
      for(int bit=1;bit<9;bit++) {
      	person_da[bit] = j%2; j>>=1;
      }

      cnt=0; fuer_alle_paare(auf_4);	// Mit dieser Kombination probieren
      
      // pr�fen ob can_trans_vgl == can_trans_x
      bool is_equal=true;
      for(int v=0;v<256;v++) if(can_trans_vgl[v]!=can_trans_x[v] ) is_equal=false;

      if(is_equal)	// Keine Unterschiede festgestellt
     	{
      	fs << "Es k�nnen gleichzeitig folgende Dolmetscher fehlen: ";
         for( int k=1;k<9;k++ ) if( !person_da[k] ) fs << k << "  ";
         fs << endl;
      }
   }
}
//==============================================================================

//==============================================================================
// A U F G A B E  5
// Feststellen welche Sprachen als Zwischen-Stationen wichtiger/weniger wichtig sind
void aufgabe_5()
{
   fs << "AUFGABE 5  -  Welche Sprachen sind als Zwischen-Stationen wichtig/weniger wichtig?" << endl << endl;
	which_way=0;
   max_persons=-1;
   link_sprachen(link_art);
   fuer_alle_paare(auf_5);

   // important_counter wurde gesetzt, jetzt nur noch sortieren
   int c=1;
   int maxi;
   
   do
   {
   	maxi=-1;			// Wie oben 9999, da jedoch der maximal Wert gefragt ist -1      
      list<Sprache*>::iterator a;
      for(a=alle_sprachen.begin(); a != alle_sprachen.end(); a++)
      	if( maxi < (*a)->important_counter ) maxi=(*a)->important_counter;
         
      if( maxi != -1 )
      {
      	fs << "Stufe " << c << ":" << maxi << endl;

         // Nun alle Sprachen, die die selbe Wichtigkeit haben ausgeben
         for(a=alle_sprachen.begin(); a != alle_sprachen.end(); a++)
         {
         	if( maxi == (*a)->important_counter )		// gleiche Wichtigkeit
            {
            	fs << (*a)->name << endl;
               (*a)->important_counter=-1;				// zur�ck setzen
            }
         }
         fs << endl;
         c++;														// Stufenz�hler erh�hen
      }
    } while( maxi != -1 );
}
//==============================================================================

//==============================================================================
// A U F G A B E  6
// Die Personen haben Priorit�ten, und zwar �bersetzen sie nur noch aus allen
// beherrschten Sprachen in die Muttersprache
void aufgabe_6()
{
	link_art=2;
	fs.open("A6.1.TXT");
   fs << "AUFGABE 6   -   Personen haben Priorit�ten" << endl;
	aufgabe_1();
   fs.close();
   fs.open("A6.2.TXT");
   fs << "AUFGABE 6   -   Personen haben Priorit�ten" << endl;
	aufgabe_2();
   fs.close();
   fs.open("A6.3.TXT");
   fs << "AUFGABE 6   -   Personen haben Priorit�ten" << endl;
	aufgabe_3();
   fs.close();
   fs.open("A6.4.TXT");
   fs << "AUFGABE 6   -   Personen haben Priorit�ten" << endl;
	aufgabe_4();
   fs.close();
   fs.open("A6.5.TXT");
   fs << "AUFGABE 6   -   Personen haben Priorit�ten" << endl;
	aufgabe_5();
   link_art=1;
   fs.close();
}
//==============================================================================

//==============================================================================
void aufgabe_7()
{
		
}
//==============================================================================

void main()
{
	fs.open("A1.TXT");
	aufgabe_1();
   fs.close();
   fs.open("A2.TXT");
	aufgabe_2();
   fs.close();
   fs.open("A3.TXT");
	aufgabe_3();
   fs.close();
   fs.open("A4.TXT");
	aufgabe_4();
   fs.close();
   fs.open("A5.TXT");
	aufgabe_5();
   fs.close();
	aufgabe_6();
}
