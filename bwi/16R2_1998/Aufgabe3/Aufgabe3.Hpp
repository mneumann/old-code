
// 16. Bundeswettbewerb Informatik 1997/98    2. Runde
// Aufgabe 16.2.3 �bersetzungen


// Aufgabe3.Hpp
// Programmiert von Michael Neumann, H�lderlinstra�e 6, 75233 Tiefenbronn


// Diese Header-Datei enth�lt alle Klassendefinitionen und auch alle
// notwendigen Funktionen, die f�r die gestellten Aufgaben gebraucht
// werden. Damit bleibt das Hauptprogramm sch�n �bersichtlich.


// Include-Dateien
# include <string>		// bindet die Klasse "string" ein
# include <list>        // bindet die Klasse "list" ein
# include <vector>      // bindet die Klasse "vector" ein
# include <fstream>		// wird ben�tigt f�r ofstream

using namespace std;		// macht den namespace "std" sichtbar, in dem
								// "string", "list" und "vector" definiert sind

typedef int Person;		// Jede Person hat eine Nummer zur Erkennung
struct Sprache;			// Vorabdefinition der Klasse "Sprache"

list<Sprache*> alle_sprachen;		// Hier tragen sich alle Sprachen selbst�ndig
											// ein -> Sprachmenge

ofstream fs;				// File-Stream, in den die Daten ausgegeben werden


//==============================================================================
// Die Klasse "Link"
//==============================================================================
struct Link
{
	Link(Person p=0, Sprache* s=0) : person(p), sprache(s) {}
   Sprache *sprache;  Person person;
   bool operator==(const Link& l)
   	{return(l.person==person && l.sprache==sprache);}
	bool operator <(const Link&) {return false;}		// Dummy
};
//==============================================================================
//==============================================================================



//==============================================================================
// Die Klasse "WayDef"
//==============================================================================
struct WayDef
{
	WayDef(Sprache *s=0, Sprache *d=0, Person p=0) : src(s), dst(d), pers(p) {}
	Sprache *src,*dst;  Person pers;
	bool operator==(const WayDef&) {return false;}	// Dummy
	bool operator <(const WayDef&) {return false;}	// Dummy
};
//==============================================================================
//==============================================================================



//==============================================================================
// Die Klasse "Sprache"
//==============================================================================
struct Sprache
{
	// Konstruktor, der die Sprache initialisiert und in die Sprachmenge einf�gt
	Sprache(string n) : name(n), was_there(false) {
   	alle_sprachen.push_back(this);
   }

	// tr�gt ein Link in die Liste "link_liste" ein
   void addLink(const Link& l) {link_liste.push_back(l);}

	// "uebersetze" wird rekursiv aufgerufen, um alle Wege herauszufinden
   void uebersetze();

	// initialisiert alle wichtigen Werte f�r "uebersetzte", und startet dann
   // die Wegerkennung 
   void starte_uebersetzung(Sprache*);

	// zeigt an ob �ber diese Sprache schon �bersetzt wurde(true)
   bool was_there;

   string name;							// der Name der Sprache
   list<Link> link_liste;				// hier sind alle Links eingetragen

   // ist nur einmal f�r alle "Sprache"-Objekte vorhanden
   // hiermit wird die Ziel-Sprache bezeichnet, in die �bersetzt werden soll
   static Sprache *ziel_sprache;

   // ist nur auch nur einmal f�r alle "Sprache"-Objekte vorhanden
   // in dieser Liste wird der aktuelle Weg beschrieben, der in der
   // Funktion "uebersetze" immer ver�ndert wird. Wenn ein Weg bis zu Ziel
   // gefunden wird, wird diese Liste in die "alle_wege" eingef�gt
   static list<WayDef> weg_liste;

   // wird benutzt um die H�ufigkeit der �ber diese Sprache laufenden �ber-
   // setzungen mitzuz�hlen
   int important_counter;
};
//==============================================================================
//==============================================================================


//==============================================================================

// m�ssen nochmal definiert werden, da sie statisch sind (Bedeutung siehe oben)
Sprache* Sprache::ziel_sprache;
list<WayDef> Sprache::weg_liste;

// gibt an �ber wieviele Personen die �bersetzung laufen darf
// -1 bedeutet, da� die �bersetzung �ber beliebig viele Personen laufen darf
int max_persons=-1;

// hier wird die Anzahl der an der �bersetzung beteiligten Personen mitgez�hlt
int person_counter=0;

// Dieses Array hat f�r jede Person(Dolmetscher) einen Eintrag. Wenn dieser
// true ist, dann hei�t das, da� die Person nicht in Urlaub ist.
// Neun Eintr�ge deswegen, da es keine Person 0 gibt!
vector <bool> person_da(9,true);

// Hier werden alle Wege beschrieben, die ans Ziel f�hren
list<list<WayDef> > alle_wege;

//==============================================================================



//==============================================================================
// Diese Prozedure ist so ziehmlich die wichtigste, da sie alle Wege einer
// �bersetzung ermittelt.
// zur�ck bedeutet immer, da� an dieser Stelle die Rekursion nicht vertieft wird
void Sprache::uebersetze()
{
	// die Anzahl der festgelegten Personen wurde �berschritten -> zur�ck
	if(max_persons!=-1 && person_counter > max_persons) return;

   // die �bersetzung lief schon �ber diese Sprache -> zur�ck
   if(was_there) return;

   was_there=true;

   // die Sprache ist mit der "ziel_sprache" identisch -> �bersetzungs-Weg gefunden
   if(name==ziel_sprache->name) alle_wege.push_back(weg_liste);
   else	// noch nicht am Ziel
   {
  		// Alle Links durchprobieren
     	list<Link>::iterator f;
      for(f=link_liste.begin();f!=link_liste.end();f++)
      {
      	if(person_da[(*f).person])					// wenn Person nicht im Urlaub
         {

				// Anzahl der beteiligten Personen erh�hen, Person auf "nicht da"
            // setzen, damit nicht nochmal �ber sie �bersetzt wird, und
            // Eintrag in "weg_liste" setzen
            ++person_counter;
         	person_da[(*f).person]=false;
            weg_liste.push_back(WayDef(this,(*f).sprache,(*f).person));

            (*f).sprache->uebersetze();			// weiter rekursieren

            // Alle oben gemachen Einstellungen zur�cksetzen
            weg_liste.pop_back();
            person_da[(*f).person]=true;
            --person_counter;
         }
      }
   }
   was_there=false;
}
//==============================================================================

//==============================================================================
// Diese Prozedure startet die �bersetzung
void Sprache::starte_uebersetzung(Sprache* ziel)
{
	alle_wege=list< list<WayDef> >();	// Liste leeren
   ziel_sprache=ziel;						// ziel_sprache setzen
   person_counter=0;                   //
   uebersetze();                       // Rekursion beginnen
}
//==============================================================================






//==============================================================================
// Diese Funktion sucht in der Sprachmenge "alle_sprachen" nach der Sprache
// mit dem Name "n"
Sprache* ist_Sprache_vorhanden(string n)
{
	for(list<Sprache*>::iterator i=alle_sprachen.begin();i!=alle_sprachen.end();i++)
   	if((*i)->name==n) return (*i);	// Sprache gefunden, also vorhanden
   return 0;									// Sprache nicht gefunden
}
//==============================================================================

//==============================================================================
// Funktion pr�ft ob die a nach b �bersetz werden kann.
// Wenn ein Weg gefunden wird, k�nnen sie �bersetzt werden.
bool kann_uebersetzen(Sprache* a, Sprache* b)
{
   a->starte_uebersetzung(b);					// Probiere zu �bersetzen
   if(alle_wege.empty()) return false;		// kein Weg gefunden
   else return true;								// es wurde ein Weg gefunden
}
//==============================================================================

//==============================================================================
// Funktion pr�ft ob beide Sprachen vorhanden sind, wenn nicht gibt sie false
// zur�ck. Wenn beide Sprachen vorhanden sind, wird probiert von a nach b
// (src->dst) zu �bersetzen. Wenn ein Weg gefunden wird, k�nnen sie �bersetzt
// werden.
bool kann_uebersetzen(string src, string dst)
{
	Sprache *a=ist_Sprache_vorhanden(src), *b=ist_Sprache_vorhanden(dst);
   if( a==0 || b==0 ) return false;
   return kann_uebersetzen(a,b);				// siehe Funktion oben
}
//==============================================================================

//==============================================================================
// Diese Prozedure, ruft die �bergebene Funktion "func" f�r alle Sprachpaare auf
void fuer_alle_paare(void (*func)(Sprache*,Sprache*))
{
	list<Sprache*>::iterator a,b;
	for(a=alle_sprachen.begin(); a!=alle_sprachen.end(); a++)
		for(b=alle_sprachen.begin(); b!=alle_sprachen.end(); b++)
      	if( a != b ) func(*a,*b);
}
//==============================================================================

//==============================================================================
// Diese Prozedure setzt die Link-Listen aller Sprachen zur�ck, d.h. l�scht
// diese Listen. Das Array "person_da" wird wird mit true gef�llt, was bedeutet,
// da� alle Personen da sind, das ist die Standardeinstellung.
// Die Variable "important_counter" wird auch auf 0 gesetzt. Diese Variable
// wird von den Aufgaben benutzt um die Wichtigkeit der Sprachen darzustellen.
void reset_link_sprachen()
{
	person_da=vector<bool>(9,true);
   list<Sprache*>::iterator a;
   for(a=alle_sprachen.begin(); a!=alle_sprachen.end(); a++)
   {
   	(*a)->link_liste = list<Link>();
      (*a)->important_counter = 0;
   }
}
//==============================================================================

//==============================================================================
// Hier werden die Sprachen deklariert
Sprache	deutsch("Deutsch"), 						englisch("Englisch"),
			franzoesisch("Franz�sisch"),			italienisch("Italienisch"),
         spanisch("Spanisch"),					portugisisch("Portugisisch"),
    		schwyzerduetsch("Schwyzerd�tsch"),	daenisch("D�nisch"),
    		niederlaendisch("Niederl�ndisch"),	schwedisch("Schwedisch"),
    		russisch("Russisch"),					kroatisch("Kroatisch"),
    		polnisch("Polnisch"),					tschechisch("Tschechisch"),
    		ungarisch("Ungarisch"),					serbisch("Serbisch");
//==============================================================================

#define aL(a,b,c) a.addLink(Link(b,&c))		// dient zur Schreiberleichterung

//==============================================================================
// Diese Prozedure baut die Links auf, die gebraucht werden um festzustellen
// ob Sprachen �bersetzt werden k�nnen oder nicht.
// Wenn x==1 werden die Standard-Links, die �berall bis auf Aufgabe 6 gebraucht
// werden aufgebaut. Wenn x==2, dann haben die Personen Priorit�ten und
// �bersetzen nur noch aus allen beherrschten Sprachen in ihre Muttersprache,
// wie es in Aufgabe 6 verlangt wird
void link_sprachen(int x=1)
{
	reset_link_sprachen();		  			// Setze alle Einstellungen zur�ck
   if( x==1 )
   {
	aL(deutsch,1,englisch);					aL(deutsch,1,franzoesisch);
   aL(deutsch,1,italienisch);			  	aL(deutsch,2,spanisch);
   aL(deutsch,2,portugisisch);			aL(deutsch,5,daenisch);
   aL(deutsch,5,schwedisch);				aL(englisch,1,deutsch);
   aL(englisch,1,franzoesisch);			aL(englisch,1,italienisch);
   aL(englisch,4,daenisch);				aL(englisch,4,franzoesisch);
   aL(englisch,4,niederlaendisch);		aL(franzoesisch,1,deutsch);
   aL(franzoesisch,1,englisch);			aL(franzoesisch,1,italienisch);
   aL(franzoesisch,3,schwyzerduetsch);	aL(franzoesisch,3,italienisch);
   aL(franzoesisch,4,englisch);			aL(franzoesisch,4,daenisch);
   aL(franzoesisch,4,niederlaendisch);	aL(italienisch,1,deutsch);
   aL(italienisch,1,englisch);			aL(italienisch,1,franzoesisch);
   aL(italienisch,3,schwyzerduetsch);	aL(italienisch,3,franzoesisch);
   aL(spanisch,2,deutsch);					aL(spanisch,2,portugisisch);
   aL(portugisisch,2,deutsch);			aL(portugisisch,2,spanisch);
   aL(schwyzerduetsch,3,franzoesisch);	aL(schwyzerduetsch,3,italienisch);
   aL(daenisch,4,englisch);				aL(daenisch,4,franzoesisch);
   aL(daenisch,4,niederlaendisch);		aL(daenisch,5,deutsch);
   aL(daenisch,5,schwedisch);				aL(niederlaendisch,4,englisch);
   aL(niederlaendisch,4,daenisch);		aL(niederlaendisch,4,franzoesisch);
   aL(schwedisch,5,deutsch);				aL(schwedisch,5,daenisch);
   aL(russisch,6,kroatisch);				aL(russisch,6,polnisch);
   aL(russisch,6,tschechisch);			aL(russisch,6,ungarisch);
   aL(russisch,7,tschechisch);			aL(russisch,7,kroatisch);
   aL(kroatisch,6,russisch);				aL(kroatisch,6,polnisch);
   aL(kroatisch,6,tschechisch);			aL(kroatisch,6,ungarisch);
   aL(kroatisch,7,russisch);				aL(kroatisch,7,tschechisch);
   aL(kroatisch,8,serbisch);				aL(polnisch,6,russisch);
   aL(polnisch,6,kroatisch);				aL(polnisch,6,tschechisch);
   aL(polnisch,6,ungarisch);				aL(tschechisch,6,russisch);
   aL(tschechisch,6,kroatisch);			aL(tschechisch,6,polnisch);
   aL(tschechisch,6,ungarisch);			aL(tschechisch,7,russisch);
   aL(tschechisch,7,kroatisch);			aL(ungarisch,6,russisch);
   aL(ungarisch,6,kroatisch);				aL(ungarisch,6,polnisch);
   aL(ungarisch,6,tschechisch);			aL(serbisch,8,kroatisch);
   }
   else if(x==2)
   {
   aL(englisch,1,deutsch);					aL(franzoesisch,1,deutsch),
   aL(italienisch,1,deutsch);				aL(spanisch,2,deutsch);
   aL(portugisisch,2,deutsch);			aL(franzoesisch,3,schwyzerduetsch);
   aL(italienisch,3,schwyzerduetsch);	aL(daenisch,4,englisch);
   aL(franzoesisch,4,englisch);			aL(niederlaendisch,4,englisch);
   aL(daenisch,5,deutsch);					aL(schwedisch,5,deutsch);
   aL(kroatisch,6,russisch);				aL(polnisch,6,russisch);
   aL(tschechisch,6,russisch);			aL(ungarisch,6,russisch);
   aL(tschechisch,7,russisch);			aL(kroatisch,7,russisch);
   aL(serbisch,8,kroatisch);
   }
}
//==============================================================================

