
/*
   16. Bundeswettbewewrb Informatik 1997/98 Runde 2
	Aufgabe 1 - Trustserver
   Copyright (c) 1998 by Michael Neumann
*/

# ifndef __Aufgabe1_Header__
# define __Aufgabe1_Header__

# include "Random.Hpp"
# include "Schluessel.Hpp"
# include <list>
# include <algorithms>
# include <iostream.h>
# include <conio.h>
# include <fstream.h>
# include "memBlock.Hpp"
using namespace std;			// für "list"...

ofstream f;						// Ausgabe-Stream
Random rnd;						// eigenes Random-Objekt

// Schaltet die Schlüsselvergabe, und die Ver- bzw. Entschlüsselung aus/ein
// Standard ist aus, da es viel zu langsam ist
const bool should_crypt = false;

// Da Klassen, die in "list" oder "vector" aus der STL (Standard Template Library)
// verwendet werden, einen == und < Operator benötigen, auch, wenn er nie
// benutzt wird, habe ich dieses Makro definiert
# define DUMMY_LIST_OPS(TYP) 	bool operator==(const TYP&) {return false;} \
										bool operator <(const TYP&) {return false;}


// Die Verschiedenen Modis für eine Vertrauensketten-Abfrage
enum Modus_Vertrauenskette {
	MVK_RANDOM, MVK_SHORTEST, MVK_SAVEST
};

struct TrustServer; 			// Vorabdefinition
struct Person;					// Vorabdefinition
struct Vertrauen;				// Vorabdefinition
struct RAND;					// Vorabdefinition


// Wird einer Funktion zur Erstellung eines echten D.Umschlages übergeben
struct info_DigitalerUmschlag
{
	Person* empfaenger;
   // ist memBlk ein D.Umschlag oder eine Nachricht
   enum md_typ { MESSAGE, DIGITALER_UMSCHLAG } typ;
   memBlock memBlk;
};


// Person muß nur den "innererBlock" als D.Umschlag weiterschicken,
// davor muß Sie ihn jedoch entschlüsseln
struct DigitalerUmschlag
{
	DigitalerUmschlag() {}
	DigitalerUmschlag(const memBlock& m) : m_block(m) {}
	Person* leseEmpfaenger()
   {
   	info_DigitalerUmschlag* i = (info_DigitalerUmschlag*)m_block.ptr;
      return i->empfaenger;
   }
   bool istNachrichtDrin()
   {
   	info_DigitalerUmschlag* i = (info_DigitalerUmschlag*)m_block.ptr;
      return (i->typ == info_DigitalerUmschlag::MESSAGE);
   }
   bool istDigitalerUmschlagDrin()
   {
   	info_DigitalerUmschlag* i = (info_DigitalerUmschlag*)m_block.ptr;
      return (i->typ == info_DigitalerUmschlag::DIGITALER_UMSCHLAG);
   }
	memBlock innererBlock()
   {
   	char *beg = m_block.ptr + sizeof(Person*) + sizeof(info_DigitalerUmschlag::md_typ);
      unsigned len;
      memcpy( &len, beg, sizeof(len) );
      beg+=sizeof(len);
      return memBlock(beg,len);
   }
private:
	memBlock m_block;
};


// Eine Klasse, die Methoden, des Alterns bereitstellt
// RAND und Vertrauen sind von ihr abgeleitet
struct ZeitBasis
{
	ZeitBasis(int *mZ) : maximaleZeit(mZ) {}
	void starte() { vergangeneZeit=0; }
   void takt() { vergangeneZeit++; }
   bool istAbgelaufen() { return (vergangeneZeit>= *maximaleZeit); }
	DUMMY_LIST_OPS(ZeitBasis)
   int vergangeneZeit;
   int *maximaleZeit;
};

// Definiert die Klasse des RANDs
struct RAND : ZeitBasis
{
 	RAND(int idd=-1) : ZeitBasis(&maximaleZeit), id(idd), benutzt(false) {}
	bool operator ==(const RAND& r) { return (r.id==id); }
   void starte() { ZeitBasis::starte(); benutzt=true; }
   bool istBenutzt() { return benutzt; }	// wurde dieses RAND schon gestartet
   static int maximaleZeit;
   int id;
   bool benutzt;
};

// Definiert die Klasse des Vertrauens
struct Vertrauen : ZeitBasis
{
	Vertrauen(Person *x=0, Person *y=0) :ZeitBasis(&maximaleZeit), was_there(false), a(x), b(y) {}
   static int maximaleZeit;
   Person *a, *b;
   bool was_there;	// für das Suchen einer Vertrauenskette
   						// wird gesetzt um sicherzugehen, da die
                     // Rekursion zum Finden der Vertrauenskette
                     // nicht in eine Endlosschleife ausartet

	// operator für die generische Funktion "find"
	bool operator ==(const Vertrauen& v) {
   	return ( (a==v.a && b==v.b)||(a==v.b && b==v.a) );
   }
};


// Klasse, die einen D.Umschlag beherbergt, und ein Flag, um sicherzugehen,
// das eine D.Umschl. erst nach einem Takt Aufenthalt weitergeschickt wird
struct Queue_DUm
{
	Queue_DUm() {}
	Queue_DUm(const DigitalerUmschlag& u) : warten(true), d(u) {}
   DigitalerUmschlag d;
	bool warten;
   DUMMY_LIST_OPS(Queue_DUm)
};


// Definiert die Klasse der Person
struct Person
{
	Person(int nr=-1) :personen_nr(nr), imNetz(false) {}
	void takt();
   RAND *benutztes_RAND;					// gerade benutztes RAND (nur wenn imNetz)
	list<RAND*> unbenutzte_RANDs;			// vorhandene unbenutzte RANDs
   static TrustServer* ts;					// Pointer auf Trustserver
   DUMMY_LIST_OPS(Person)
	int personen_nr;
   bool imNetz;
   Schluessel privater_schluessel;

   // diese Funktion, tut den Umschlag in die Liste eingegangene_Umschlaege
   void empfangeDigitalenUmschlag(const DigitalerUmschlag&);

   // diese Funktion entschlüsselt den D.U. und schickt ihn weiter, wenn es
   // keine Nachricht für sie war
	void bearbeiteDigitalenUmschlag(const DigitalerUmschlag&);

	// wird aufgerufen vom Trustserver wenn der Person gekündigt wird 
   void scheideAusDemNetz();

	// wird aufgerufen um einer Person p Vertrauen zu schenken
	void schenke_vertrauen(Person*p);

	list<Queue_DUm> eingegangene_Umschlaege;

   // Funktionen, damit die Person digitale Umschläge erstellen kann...
	memBlock create_memBlk_crypted(const info_DigitalerUmschlag&);
	DigitalerUmschlag erzeuge_DigitalenUmschlag(list<Person*>&, Person*, char*, int);
};


// Für die Suche nach einer Vertrauens-Beziehung
struct VNode {
	Vertrauen *v;
   Person *zp;
   DUMMY_LIST_OPS(VNode)
};

// Für einen Eintrag in der Weg-Liste (waylist)
struct waynode	{
	waynode(Person* p=0, int zt=0) : pers(p), zeit_vertrauen(zt) {}
	Person* pers;
   int zeit_vertrauen;
   DUMMY_LIST_OPS(waynode)
};

// a vertraut b nicht mehr
struct keinVertrauen {
	keinVertrauen(Person* x=0, Person* y=0) : a(x), b(y) {}
	Person *a, *b;
	bool operator ==(const keinVertrauen& v) { return ( a==v.a && b==v.b ); }
	bool operator <(const keinVertrauen& v) { return false; }
};


struct TrustServer
{
	TrustServer() {
   	Vertrauenskette_SAVEST_rand_prioritaet = true;

      // die Werte sollten möglichst hoch sein, damit "gute" Schlüssel
      // erzeugt werden
      schluessel_gen = new SchluesselGenerator(1000000, 1000);
   }
	~TrustServer() { delete schluessel_gen; }


	// hier werden die Öffnetlichen-Schlüssel und die angemeldeten Personen
   // festgehalten
   struct Person_Schluessel
   {
   	bool operator ==(const Person_Schluessel& ps) { return person==ps.person; }
      bool operator <(const Person_Schluessel&) { return false; }
		Person *person;
      Schluessel oeffentlicher_schluessel;
   };


	// Aktionen
	RAND* erzeugeNeuesRAND();
   void loescheRAND(RAND*);
   void schenkeVertrauen(Person*, Person*, RAND*);
	void entzieheVertrauen(Person*,Person*);
   void loescheVertrauensBeziehungen(Person*);
   void meldePersonAn(Person*, RAND*);
   void meldeZurueck(Person*, RAND*);
	Person_Schluessel suchePersonMitRAND(RAND*);
   void kuendigePersonMitRANDx(RAND*);
   bool istRANDregistriert(RAND&);
   bool istRANDunbenutzt(RAND&);

   // wird aufgerufen, um der Person p ein unbenutztes RAND zu schenken
   void verschenke_Selbst_RAND(Person *p)
   {
  		RAND *newrand = erzeugeNeuesRAND();
		f << "TrustServer schenkt RAND..." << endl;
		f << "	An Person: " << p->personen_nr << endl;
		f << "   RAND: " << newrand->id << endl;
		f << "-------------------------------------------------" << endl;
		p->unbenutzte_RANDs.push_back(newrand);
   }

   // findet alle Einträge, die mit dieser Person eine Vertrauensbeziehung haben 
	void findeVertrauen(Person*, list<VNode>&);

   // wird von der "Person" aufgerufen, wenn nach einer Vertrauenskette gesucht wird
   bool erfrageVertrauenskette(Person*, Person*, Modus_Vertrauenskette, list<Person*>&);

	Schluessel erfrageOeffentlichenSchluessel(Person*);
   void sendeDigitalenUmschlag(const DigitalerUmschlag&);
   void takt();

	// Daten/Strukturen
   list<Person_Schluessel> angemeldete_Personen;
   list<RAND> registrierte_RANDs;
   list<Vertrauen> vertrauens_beziehungen;
   list<keinVertrauen> vertrauens_brueche;
   Random randomGenerator;
  	list< Queue_DUm > Umschlaege_weitersenden;

   // Siehe in der Prozedure erfrageVertrauenskette, nach der Erklärung
 	bool Vertrauenskette_SAVEST_rand_prioritaet;

   private:
   list<waynode> waylist;
	list< list<waynode> > allways;
	void finde_weg(Person*);
   Person *finde_weg_dst;
	bool isperson_in_waynode_list(Person* p, list<waynode>& weg);
	bool is_invalid_waynode_list(list<waynode>& weg);
   SchluesselGenerator *schluessel_gen;
};


//==============================================================================
//==============================================================================
//  GLOBALE-VARIABLEN UND KLASSEN-VARIABLEN (STATIC)
//==============================================================================
//==============================================================================


int RAND::maximaleZeit;
TrustServer* Person::ts;
int Vertrauen::maximaleZeit;
list<Person> Menschen;			// Hier sind alle Personen deklariert






//==============================================================================
//==============================================================================
//  V E R T R A U E N S - F U N K T I O N S - O B J E K T E
//==============================================================================
//==============================================================================

// Verhalten einer Person
struct Verhalten_Person
{
	void operator() (Person *p)
   {
		pers=p;
      Person *welcher_person = finde_person();	// irgendeine Person finden
		switch( Was_Machen() )		// was soll gemacht werden
      {
      	case V_ENTZIEHEN : p->ts->entzieheVertrauen(pers,welcher_person); break;
         case V_SCHENKEN : p->schenke_vertrauen(welcher_person); break;
         case NACHRICHT_SENDEN : nachricht_senden(welcher_person); break;
      }
   }


   // Versuchen eine Nachricht zu senden
   void nachricht_senden(Person *p)
   {
   	list<Person*> p_list;
   	switch( rnd(3) )
      {
      	case 0 :	if( !pers->ts->erfrageVertrauenskette(pers,p,MVK_RANDOM, p_list) ) return; break;
         case 1 :	if( !pers->ts->erfrageVertrauenskette(pers,p,MVK_SHORTEST, p_list) ) return; break;
         case 2 : if( !pers->ts->erfrageVertrauenskette(pers,p,MVK_SAVEST, p_list) ) return; break;
         default: return;
      }

		// Sendet eine Nachricht
		char *message="Hallo Welt, das ist ne Message";
      pers->ts->sendeDigitalenUmschlag(pers->erzeuge_DigitalenUmschlag(p_list,p,message,sizeof(message)));
   }


   // findet irgendeine zufällige Person
	Person *finde_person()
   {
   	list<Person>::iterator i;
		int count=0;
   	do {
      	int skip=rnd(Menschen.size());
  			i=Menschen.begin();
  			while(skip>0) { ++i; --skip; }
      } while( &(*i) == pers && count++ < 1000 );
      return &(*i);
   }

   enum was_machen_typ { NICHTS, V_ENTZIEHEN, V_SCHENKEN, NACHRICHT_SENDEN };

   // liegt der Wert c zwischen a[0] und a[1]?
   bool zwischen(int c,int* a) {
   	if( c > a[0] && c < a[1] ) return true;
      return false;
   }

	// Diese Funktion entscheidet was gemacht werden soll
   was_machen_typ Was_Machen()
   {
   	int r=rnd(1000);
      if( zwischen(r,nichts_machen) ) return NICHTS;
      else if( zwischen(r,verschenke_RANDs) ) return V_SCHENKEN;
      else if( zwischen(r,entziehe_RANDs) ) return V_SCHENKEN;
      else if( zwischen(r,nachricht_s) ) return NACHRICHT_SENDEN;
      else return NICHTS;
	}


	// Dies sind die Zufälligkeiten, mit denen eine bestimmte Aktion ausgeführt wird
   int nichts_machen[2];
	int verschenke_RANDs[2];
   int entziehe_RANDs[2];
   int nachricht_s[2];			// nachricht senden

   Person *pers;
} verhalten_ps;



// Verhalten des TrustServers
struct Verhalten_TrustServer
{
	void operator() (TrustServer *t)
   {
   	ts=t;

      // Zufälliges Verschenken von RANDs
		if(rnd(1000) > wann_zufaellig_Verschenken)
      {
			int anz=rnd(anz_zufalls_Verschenken);
         list<Person_Schluessel>::iterator p=ts->angemeldete_Personen.begin();
         while(anz>0)
         {
  				if(rnd(100) < 37)	// Zufällige RAND Vergabe
      		{
            	if( p !=  ts->angemeldete_Personen.end() ) ts->verschenke_Selbst_RAND( (*p).person );
           		--anz;
				}
         	if(++p==ts->angemeldete_Personen.end()) p=ts->angemeldete_Personen.begin();
   		}
      }


      // ist die Zeit um, daß ein RAND verschenk wird (oder mehrere)
		if(takt_verschenk >= 1.0)
      {
      	int anz = takt_verschenk;
			takt_verschenk -= anz;
         while( anz > 0 )
         {
         	Person *qp = suche_qualifizierte_Person();
            if( qp != 0 ) ts->verschenke_Selbst_RAND( qp );
            --anz;
         }
      }
		takt_verschenk += wieviele_RANDs_pro_Takt;	// bei jedem Takt addierend 

   }
  	Person* suche_qualifizierte_Person()
   {
      // qualifizierte Personen sind:
      // Personen mit vielen Rands, und mit langer Dauer

      list<Person_Schluessel>::iterator p=ts->angemeldete_Personen.begin();
      int n=ts->angemeldete_Personen.size();
		int rn=rnd(n);
      for( int r=0; r<rn; r++ ) ++p;
      while( n > 0 )
      {
      	if( (*p).person->unbenutzte_RANDs.size() > mehr_als_RANDs
         	 || (*p).person->benutztes_RAND->vergangeneZeit < weniger_als_takte ) return (*p).person;
      	++p;
			if( p == ts->angemeldete_Personen.end() ) p=ts->angemeldete_Personen.begin();
         n--;
      }
      return 0;	// keine qualifizierte Person gefunden
   }



	// welche Personen sind qualifiziert
   int mehr_als_RANDs;		// die,die mehr als "mehr_als_RANDs" haben
   int weniger_als_takte;	// oder die weniger als "weniger_als_takte" alte
   								// Vertrauensbeziehungen haben

	// an qualifizierte Personen
   float wieviele_RANDs_pro_Takt;	// wieviele RANDs sollen pro Takt verschenk werden

   float takt_verschenk;				// wird nur hochgezählt, keine Verhaltensoption

	// wenn rnd(1000) größer ist dann verschenken (1000=nicht zufällig verschenken -1=immer)
   int wann_zufaellig_Verschenken;
   int anz_zufalls_Verschenken;	// rnd(..) verschenken

private:
	TrustServer* ts;
} verhalten_ts;










//==============================================================================
//==============================================================================
//  M E T H O D E N   D E R   K L A S S E   T R U S T S E R V E R
//==============================================================================
//==============================================================================


// Person src entzieht dst das Vertrauen
// das verfällt nur, wenn die src, dst wieder Vertrauen schenkt,
// es verfällt nicht mal, wenn eine oder beide Personen aus dem Netz fallen!!!!
// TS bildet keinen Weg mehr, indem beide Personen vorkommen
void TrustServer::entzieheVertrauen(Person* src, Person* dst)
{
	Vertrauen x(src,dst);
   remove(vertrauens_beziehungen.begin(), vertrauens_beziehungen.end(), x);
	keinVertrauen y(src,dst); vertrauens_brueche.push_back(y);
}


// sucht in der Liste der angemeldeten Personen, nach der Person, und gibt den
// ihr zugeordneten Oeffentlichen Schluessel zurück
Schluessel TrustServer::erfrageOeffentlichenSchluessel(Person* p)
{
	Person_Schluessel x; x.person=p;	// erzeugen wegen Suchfunktion
   return (*find(angemeldete_Personen.begin(),angemeldete_Personen.end(),x)).oeffentlicher_schluessel;
}


// Sucht in der Liste "vertrauens_beziehungen" nach der Person p
void TrustServer::findeVertrauen(Person* p, list<VNode> &vnl)
{
	vnl=list<VNode>();	// Liste leeren
	VNode vn;
	for(list<Vertrauen>::iterator i=vertrauens_beziehungen.begin();
   i!=vertrauens_beziehungen.end(); i++ )
   {
   	if( (*i).a == p )
      {
      	vn.v=&(*i); vn.zp=(*i).b;
         vnl.push_back(vn);
      }
      else if( (*i).b == p )
      {
      	vn.v=&(*i); vn.zp=(*i).a;
         vnl.push_back(vn);
      }
   }
}


// Löscht das RAND aus der Registrierung
void TrustServer::loescheRAND(RAND* r) {
	registrierte_RANDs.remove(*r);
}

// Erzeugt ein neues RAND, und fügt dies in die Registrierung ein
RAND* TrustServer::erzeugeNeuesRAND() {
	RAND new_rand;
   while( istRANDregistriert( (new_rand=randomGenerator()) ) );	// eigentlich unnütz
   registrierte_RANDs.push_back(new_rand);
   return &registrierte_RANDs.back();
}


// erneuert ein abgelaufenes RAND->Person fliegt nicht aus dem Netz
// Person: benutztes_RAND setzen, unbenutztes.poppen
void TrustServer::meldeZurueck(Person* p, RAND* r)
{
	f << "Melde Zurück..." << endl;
   f << "	Person: " << p->personen_nr <<endl;
	f << "   mit RAND: " << r->id << endl;
   f << "-------------------------------------------------" << endl;
  	loescheRAND(p->benutztes_RAND);
	r->starte();
}

// meldet eine Person, die noch nicht im Netz ist an
// Person: imNetz=true, benutztes_RAND setzen, unbenutztes.poppen
void TrustServer::meldePersonAn(Person* p, RAND* r)
{
	f << "Melde Person An..." << endl;
   f << "	Person: " << p->personen_nr <<endl;
	f << "   mit RAND: " << r->id << endl;
   f << "-------------------------------------------------" << endl;
   r->starte();

   Person_Schluessel ps; ps.person = p;
   if(should_crypt) schluessel_gen->erzeugeSchluesselPaar(ps.oeffentlicher_schluessel, p->privater_schluessel);
	angemeldete_Personen.push_front(ps);
}


// sucht in der Liste "registrierte_RANDs" nach dem RAND r -> true wenn gefunden
bool TrustServer::istRANDregistriert(RAND& r) {
	return ( find(registrierte_RANDs.begin(), registrierte_RANDs.end(), r)
   != registrierte_RANDs.end() );
}


// sucht in der Liste "registrierte_RANDs" nach dem RAND r (muß gefunden werden)
// wenn dieses benutzt wird->false, ansonsten true
bool TrustServer::istRANDunbenutzt(RAND& r) {
   return !(*find(registrierte_RANDs.begin(),registrierte_RANDs.end(), r)).istBenutzt();
}


// prüft alle angemeldeten Personen, ob Sie das RAND r benutzen, wenn ja ->
// dann wird diese Person zurückgeliefert
Person_Schluessel TrustServer::suchePersonMitRAND(RAND* r)
{
	for( list<Person_Schluessel>::iterator i=angemeldete_Personen.begin();
   i!=angemeldete_Personen.end(); i++ ) if( (*i).person->benutztes_RAND == r ) return (*i);
   // hierher kann es nie kommen
}


// löscht alle Vertrauensbeziehungen, die mit dieser Person verbunden sind
void TrustServer::loescheVertrauensBeziehungen(Person* p) {
	list<Vertrauen>::iterator i = vertrauens_beziehungen.begin(), j;
   while( i != vertrauens_beziehungen.end() ) {
   	j=i++;
   	if( (*j).a == p || (*j).b == p ) vertrauens_beziehungen.erase(j);
   }
}



// Person src schenkt dst Vertrauen, durch verschenken von RAND r
// Person: muß das RAND aus seiner Liste entfernen
void TrustServer::schenkeVertrauen(Person* src, Person* dst, RAND* r)
{
	f << "Schenke Vertrauen..." << endl;
   f << "	Quell-Person: " << src->personen_nr << endl;
   f << "   Ziel-Person: " << dst->personen_nr << endl;
	f << "   mit RAND: " << r->id << endl;
   f << "-------------------------------------------------" << endl;
	dst->unbenutzte_RANDs.push_back(r);	// RAND verschenken
   Vertrauen x(src,dst); x.starte();

	// wenn keinVertrauen(src,dst) vorhanden, dann löschen, d.h. Vertrauensbruch auflösen
   keinVertrauen kv(src,dst);
   remove(vertrauens_brueche.begin(), vertrauens_brueche.end(), kv);

   // vorher alte Vertrauensbeziehung löschen
   remove(vertrauens_beziehungen.begin(), vertrauens_beziehungen.end(), x);

   vertrauens_beziehungen.push_back(x);
}


// Diese Prozedure kündigt der Person, die das RAND r besitzt,
// da dieses abgelaufen ist.
void TrustServer::kuendigePersonMitRANDx(RAND* r)
{
	Person_Schluessel ps = suchePersonMitRAND(r);
	f << "Kündige Person..." << endl;
   f << "	Person: " << ps.person->personen_nr <<endl;
	f << "   mit RAND: " << r->id << endl;
   f << "-------------------------------------------------" << endl;
   if(should_crypt)schluessel_gen->loescheSchluessel(ps.oeffentlicher_schluessel);
   loescheVertrauensBeziehungen(ps.person);
	loescheRAND(r);
   ps.person->scheideAusDemNetz();
}


// stellt den D.Umschlag in eine Liste, die von der Prozedure "takt" geleert wird
void TrustServer::sendeDigitalenUmschlag(const DigitalerUmschlag& d) {
	Umschlaege_weitersenden.push_back( Queue_DUm(d) );
}


// Wird rekursiv aufgerufen; ermittelt alle möglichen Vertrauensketten 
void TrustServer::finde_weg(Person *src)
{
	list<VNode> vnl;
	list<VNode>::iterator v_it;
  	waylist.push_back(waynode(src));
	if( src == finde_weg_dst ) allways.push_back(waylist);
   else
   {
   	findeVertrauen(src, vnl);
   	for( v_it = vnl.begin(); v_it != vnl.end(); v_it++ )
   	{
         if( (*v_it).v->was_there == false )
         {
         	waylist.pop_back();
            waylist.push_back(waynode(src,(*v_it).v->vergangeneZeit));
         	(*v_it).v->was_there = true;
         	finde_weg((*v_it).zp);
				(*v_it).v->was_there = false;
            waylist.pop_back();
          	waylist.push_back(waynode(src));
         }
   	}
   }
   waylist.pop_back();
}


// Funktion um zu ermitteln, ob eine Person, in der Liste "waynode" vorhanden ist
bool TrustServer::isperson_in_waynode_list(Person* p, list<waynode>& weg)
{
   for( list<waynode>::iterator i=weg.begin(); i!=weg.end(); i++ )
		if( (*i).pers == p ) return true;
   return false;
}

// gibt true zurück, wenn eine Person vorkommt, über die der Weg nicht gehen darf
bool TrustServer::is_invalid_waynode_list(list<waynode>& weg)
{
	for( list<keinVertrauen>::iterator i=vertrauens_brueche.begin(); i!=vertrauens_brueche.end(); i++ )
   {
		if( isperson_in_waynode_list( (*i).a, weg ) && isperson_in_waynode_list( (*i).b, weg ) )
		return true;
   }
   return false;
}


// Diese Funktion ermittelt, die gewünschte Vertrauenskette
bool TrustServer::erfrageVertrauenskette(Person* absender, Person* empfaenger, Modus_Vertrauenskette modus, list<Person*>& liste)
{
	list< list<waynode> >::iterator ret_it,i,k;
   waylist = list<waynode>();
	allways = list< list<waynode> >();
   liste = list<Person*>();

   if( absender==empfaenger ) return false;

   finde_weg_dst = empfaenger;
   finde_weg(absender);	// findet alle mgl. Wege raus

	// Wege, indenen Personen vorkommen, die kein Vertrauen zueinander haben( Struktur keinVertrauen ),
   // werden gelöscht
   i=allways.begin();
   while( i!= allways.end() ) {
   	k=i++;
      if( is_invalid_waynode_list(*k) ) allways.erase(k);
   }


	// wenn kein Weg gefunden, zurück
   if( allways.empty() ) return false;

   // Absender und Empfänger aus allen Liste löschen
	for( i=allways.begin(); i!=allways.end(); i++ ) {
		(*i).pop_front();	(*i).pop_back();
   }

   list<waynode>::iterator j;
   ret_it = allways.begin();
   int skip;

   #define UNBENUTZTE_RANDS 		0
   #define LAENGE_VERTRAUEN 		1
   #define ANZ_ZWISCHENPERSONEN 	2

   int that[3],best[3];		// Arrays um die Maximas oder Minimas herauszufinden


   switch( modus )
   {
		case MVK_RANDOM :
      	skip = rnd(allways.size());
         while( skip > 0 ) { --skip; ++ret_it; }
         break;

      case MVK_SHORTEST:
			for( i=allways.begin(); i!=allways.end(); i++ )
         	if( (*ret_it).size() > (*i).size() ) ret_it=i;
      	break;

      case MVK_SAVEST:
        	// je mehr RANDs desto besser
         // je weniger Vertrauenszeit desto besser
         // je kleiner die Anzahl der Zwischenpersonen desto besser

         best[UNBENUTZTE_RANDS]=best[LAENGE_VERTRAUEN]=0;
         best[ANZ_ZWISCHENPERSONEN]=(*ret_it).size();
         for( j=(*ret_it).begin(); j!= (*ret_it).end(); j++ )
         {
        		best[UNBENUTZTE_RANDS]  += (*j).pers->unbenutzte_RANDs.size();
            best[LAENGE_VERTRAUEN]  += (*j).zeit_vertrauen;
         }


         i=ret_it;
         if( i!=allways.end() ) ++i;		// best-Array schon gefüllt, mit dem 1.Weg
         for( ; i!=allways.end(); i++ )
         {
            that[UNBENUTZTE_RANDS]=that[LAENGE_VERTRAUEN]=0;
            that[ANZ_ZWISCHENPERSONEN]=(*i).size();
            for( j=(*i).begin(); j!= (*i).end(); j++ )
            {
            	that[UNBENUTZTE_RANDS] += (*j).pers->unbenutzte_RANDs.size();
            	that[LAENGE_VERTRAUEN] += (*j).zeit_vertrauen;
            }

            // Wenn dieses Flag gesetzt ist, dann hat die Anzahl der unbenutzten
            // RANDs vor der Dauer der Vertrauensbeziehung Vorrang
            if( Vertrauenskette_SAVEST_rand_prioritaet )
            {
            // ------------~~~~~~~-----~~~~~~~~---------~~~~~~~~~~----------
            float a=(float)best[UNBENUTZTE_RANDS]/best[ANZ_ZWISCHENPERSONEN];
            float b=(float)that[UNBENUTZTE_RANDS]/that[ANZ_ZWISCHENPERSONEN];
				if( a < b )
            // best hat weniger unbenutzte RANDs
            {
            	best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               ret_it=i;
            }
            else if( a == b )
            {
          		float a=(float)best[LAENGE_VERTRAUEN]/best[ANZ_ZWISCHENPERSONEN];
            	float b=(float)that[LAENGE_VERTRAUEN]/that[ANZ_ZWISCHENPERSONEN];
               if( a > b )
               // best hat längere Vertrauensbeziehungen(schlecht)
               {
            		best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               	best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               	best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               	ret_it=i;
               }
               else if( a == b )
               {
               	if( best[ANZ_ZWISCHENPERSONEN] > that[ANZ_ZWISCHENPERSONEN] )
                  // best hat mehr Zwischenstationen(schlecht)
                  {
            			best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               		best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               		best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               		ret_it=i;
                  }
               }
            }
            // ------------~~~~~~~-----~~~~~~~~---------~~~~~~~~~~----------
            }
            else	/* Zeit der Vertrauensbeziehungen haben Vorrang */
            {
            // ------------~~~~~~~-----~~~~~~~~---------~~~~~~~~~~----------
        		float a=(float)best[LAENGE_VERTRAUEN]/best[ANZ_ZWISCHENPERSONEN];
          	float b=(float)that[LAENGE_VERTRAUEN]/that[ANZ_ZWISCHENPERSONEN];
				if( a > b )
            // best hat längere Vertrauensbeziehungen(schlecht)
            {
            	best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               ret_it=i;
            }
            else if( a == b )
            {
            	float a=(float)best[UNBENUTZTE_RANDS]/best[ANZ_ZWISCHENPERSONEN];
            	float b=(float)that[UNBENUTZTE_RANDS]/that[ANZ_ZWISCHENPERSONEN];
               if( a > b )
            	// best hat weniger unbenutzte RANDs
               {
            		best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               	best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               	best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               	ret_it=i;
               }
               else if( a == b )
               {
               	if( best[ANZ_ZWISCHENPERSONEN] > that[ANZ_ZWISCHENPERSONEN] )
                  // best hat mehr Zwischenstationen(schlecht)
                  {
            			best[UNBENUTZTE_RANDS]=that[UNBENUTZTE_RANDS];
               		best[ANZ_ZWISCHENPERSONEN]=that[ANZ_ZWISCHENPERSONEN];
               		best[LAENGE_VERTRAUEN] = that[LAENGE_VERTRAUEN];
               		ret_it=i;
                  }
               }
            }
            
            // ------------~~~~~~~-----~~~~~~~~---------~~~~~~~~~~----------
            }
         }
      	break;
   }	/* switch ende */

   // ret_it zeigt auf die Wegliste, die am besten geeignet ist

   // jetzt nur noch die Wegliste<waynode> in eine Wegliste<Person*> transformieren
   for( j=(*ret_it).begin(); j!=(*ret_it).end(); j++ ) liste.push_back( (*j).pers );
	return true;
}


// Diese Prozedure verleiht dem Trustserver einen Takt, sie wird
// durch den Simulator, bei jedem Takt aufgerufen
void TrustServer::takt()
{
   // Alle eingegangenen Digitalen Umschläge prüfen ob, schon ein Takt hier,
   // wenn ja, dann an die Person senden, die durch den Empfänger im Digitalen-
   // Umschlag gekennzeichnet ist
	for( list< Queue_DUm >::iterator i=Umschlaege_weitersenden.begin();
      	  i!=Umschlaege_weitersenden.end(); i++ )
	{
		if( !(*i).warten ) (*i).d.leseEmpfaenger()->empfangeDigitalenUmschlag((*i).d);
		(*i).warten=false;
	}

	// Dies ist ein Funktions-Objekt, welches das Verhalten des Trustservers
   // wiederspiegelt
   verhalten_ts(this);

	// Vertrauensbeziehungen takten
   list<Vertrauen>::iterator v=vertrauens_beziehungen.begin(),w;
   while( v != vertrauens_beziehungen.end() )
   {
   	w=v++;  (*w).takt();
      if( (*w).istAbgelaufen() )
      {
      	f << "Vertrauensbeziehung abgelaufen..." << endl;
         f << "	Person a: " << (*w).a->personen_nr << endl;
   		f << "   Person b: " << (*w).b->personen_nr << endl;
   		f << "-------------------------------------------------" << endl;
         vertrauens_beziehungen.erase(w);
      }
   }


	// RANDs takten
   list<RAND>::iterator i=registrierte_RANDs.begin(),j;
   while( i != registrierte_RANDs.end() )
   {
   	j=i++;
      if( (*j).istBenutzt() )
      {
      	if( (*j).istAbgelaufen() )
         {
      		f << "RAND abgelaufen..." << endl;
         	f << "	RAND: " << (*j).id << endl;
				kuendigePersonMitRANDx(&(*j));
      	}
         else (*j).takt();
   	}
	}
}




//==============================================================================
//==============================================================================
//  M E T H O D E N   D E R   K L A S S E   P E R S O N
//==============================================================================
//==============================================================================


// Erzeugt aus dem übergebenen info_DigitalUmschlag, eine
// Präsentation im Speicher, welche verschlüsselt ist
memBlock Person::create_memBlk_crypted(const info_DigitalerUmschlag& d)
{
	memBlock retval, x;
   retval.add((char*)&d.empfaenger, sizeof(Person*));
   retval.add((char*)&d.typ, sizeof(d.typ));
   verschluesseln(d.memBlk, x, ts->erfrageOeffentlichenSchluessel(d.empfaenger),should_crypt);
   retval.add((char*)&x.size, sizeof(x.size));
   retval.add(x);
   return retval;
}


// Ist im Umschlag eine Nachricht oder ein weiterer Umschlag?
void Person::bearbeiteDigitalenUmschlag(const DigitalerUmschlag& d)
{
	memBlock x;
	entschluesseln(d.innererBlock(),x, privater_schluessel, should_crypt);

   if( d.istNachrichtDrin() )
   {
   	f << "Nachricht für mich drin" << endl;
      f << "Person: " << personen_nr << endl;
      f << x.ptr << endl;
      f << "--------------------------------------" << endl;
   }
   else if( d.istDigitalerUmschlagDrin() ) ts->sendeDigitalenUmschlag(DigitalerUmschlag(x));
}


// Wird vom Trustserver aufgerfuen, um derjenigen Person einen D.Umschlag zuzuteilen
void Person::empfangeDigitalenUmschlag(const DigitalerUmschlag& d) {
	eingegangene_Umschlaege.push_back(Queue_DUm(d));
}


// Erzeugt einen Digitalen Umschlag
DigitalerUmschlag Person::erzeuge_DigitalenUmschlag(list<Person*>& weg, Person *ziel, char* nachricht, int sizeof_nachricht)
{
	info_DigitalerUmschlag d;
   d.empfaenger = ziel;
   d.typ = info_DigitalerUmschlag::MESSAGE;
   d.memBlk.equ(nachricht, sizeof_nachricht);

   memBlock digit_U = create_memBlk_crypted(d);

   for( list<Person*>::reverse_iterator i=weg.rbegin(); i!=weg.rend(); i++ )
   {
   	d.empfaenger = (*i);
      d.typ = info_DigitalerUmschlag::DIGITALER_UMSCHLAG;
      d.memBlk.equ(digit_U);
      digit_U.equ(create_memBlk_crypted(d));
   }
	return DigitalerUmschlag(digit_U);
}


// Wird aufgerufen, um einer Person p Vertrauen, d.h. ein unbenutztes RAND zu scheneken
void Person::schenke_vertrauen(Person* p)
{
	if( !unbenutzte_RANDs.empty() )
   {
		ts->schenkeVertrauen(this, p, unbenutzte_RANDs.front() );
      unbenutzte_RANDs.pop_front();
   }
}


// Diese Prozedure wird vom Simulator, bei jedem Takt für jede
// Person aufgerufen
void Person::takt()
{
	if(imNetz)			// Wenn die Person schon im Netz ist
   {
      if(!eingegangene_Umschlaege.empty())	// Digitale-Umschläge angekommen
      {
			if(!eingegangene_Umschlaege.front().warten)	// hat schon 1 Takt gewartet
         {
         	bearbeiteDigitalenUmschlag(eingegangene_Umschlaege.front().d);
				eingegangene_Umschlaege.pop_front();
         }
      }

		// das benutzte RAND ist abgelaufen, aber hat noch ein ungebrauchtes im Besitz      
		if( benutztes_RAND->istAbgelaufen() && !unbenutzte_RANDs.empty() )
   	{
			ts->meldeZurueck(this, unbenutzte_RANDs.front()); // zurückmelden
		   benutztes_RAND=unbenutzte_RANDs.front();
      	unbenutzte_RANDs.pop_front();
   	}

      // in diesem Funktions-Objekt wird das Verhalten der Personen gemanaged
      else verhalten_ps(this);

		// alle eingegangenen Umschläge, den Warten-Status entziehen
		for( list<Queue_DUm>::iterator i=eingegangene_Umschlaege.begin();
      	  i!=eingegangene_Umschlaege.end(); i++ ) (*i).warten=false;
   }

   // Nicht im Netz, hat aber unbenutztes RAND zur Verfügung
   else if( !unbenutzte_RANDs.empty() )
   {
   	ts->meldePersonAn(this, unbenutzte_RANDs.front() );	// anmelden
      imNetz = true;
      benutztes_RAND = unbenutzte_RANDs.front();
      unbenutzte_RANDs.pop_front();
   }
}


// Wird aufgerufen von Trustserver, wenn einer Person gekündigt wird
void Person::scheideAusDemNetz()
{
	// hier alle Werte zurück setzen
	imNetz=false;
	eingegangene_Umschlaege=list<Queue_DUm>();
}


# endif



