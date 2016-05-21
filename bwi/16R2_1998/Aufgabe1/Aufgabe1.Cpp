// 16. Bundeswettbewerb Informatik 1997/98    2. Runde
// Aufgabe 16.2.1 Trust-Server

// Aufgabe1.Cpp
// Programmiert von Michael Neumann, Hölderlinstraße 6, 75233 Tiefenbronn


#include "Aufgabe1.Hpp"



// Die ist der Simulator, dem man Wert übergeben kann, mit denen dann
// eine Simulation gestartet werden kann
struct Simulator
{
	Simulator(
		int anzahl_der_Menschen,
   	int anzahl_anfaenglicher_vergebener_RANDs,
	   int zeit_Vertrauen,
	   int zeit_RANDs,
	   int simulations_dauer,

	   // <T>rust<S>erver <V>erhalten
	   int TSV_mehr_als_RANDs,
	   int TSV_weniger_als_takte,
	   float TSV_wieviele_RANDs_pro_Takt,
	   int TSV_wann_zufaellig_Verschenken,
	   int TSV_anz_zufalls_Verschenken,

		// <P>erson <V>erhalten
	   int PV_nichts_machen0, int PV_nichts_machen1,
		int PV_verschenke_RANDs0, int PV_verschenke_RANDs1,
	   int PV_entziehe_RANDs0, int PV_entziehe_RANDs1,
	   int PV_nachricht_s0, int PV_nachricht_s1,

	 	char *protokol_datei			// Diese Datei wird generiert
   )
   {
   	simul_dauer = simulations_dauer;
   	f.open(protokol_datei);
		ts = new TrustServer;
		Person::ts = ts;
		Vertrauen::maximaleZeit = zeit_Vertrauen;
		RAND::maximaleZeit =	zeit_RANDs;
		Menschen = list<Person>();

	   for( int i=0; i<anzahl_der_Menschen; i++ ) Menschen.push_back(Person(i));

		// Vergibt i RANDs an zufällig ausgewählte Personen
		int i=anzahl_anfaenglicher_vergebener_RANDs;
	   list<Person>::iterator p=Menschen.begin();
		while(i > 0)
	   {
			if(rnd(100) < 37)	// Zufällige RAND Vergabe
	      {
				(*p).unbenutzte_RANDs.push_back(ts->erzeugeNeuesRAND());
	         --i;
			}
	      if(++p==Menschen.end()) p=Menschen.begin();
	   }

	   verhalten_ts.mehr_als_RANDs = TSV_mehr_als_RANDs;
	   verhalten_ts.weniger_als_takte = TSV_weniger_als_takte;
		verhalten_ts.takt_verschenk=0;
	   verhalten_ts.wieviele_RANDs_pro_Takt = TSV_wieviele_RANDs_pro_Takt;
	   verhalten_ts.wann_zufaellig_Verschenken = TSV_wann_zufaellig_Verschenken;
	   verhalten_ts.anz_zufalls_Verschenken = TSV_anz_zufalls_Verschenken;

	   verhalten_ps.nichts_machen[0] = PV_nichts_machen0;
		verhalten_ps.verschenke_RANDs[0] = PV_verschenke_RANDs0;
	   verhalten_ps.entziehe_RANDs[0] = PV_entziehe_RANDs0;
	   verhalten_ps.nachricht_s[0] = PV_nachricht_s0;

	   verhalten_ps.nichts_machen[1] = PV_nichts_machen1;
  		verhalten_ps.verschenke_RANDs[1] = PV_verschenke_RANDs1;
	   verhalten_ps.entziehe_RANDs[1] = PV_entziehe_RANDs1;
	   verhalten_ps.nachricht_s[1] = PV_nachricht_s1;
   }

	~Simulator() {
		f.close();
		delete ts;
   }


	// Startet eine Simulation
	void Starte()
   {
	for(int i=0; i<simul_dauer; i++)
   {
		f << "###################### Takt " << i << " ###################" << endl;
		for( list<Person>::iterator x=Menschen.begin(); x!=Menschen.end(); x++ )
      		(*x).takt();
   	ts->takt();
   }
   }

private:
	TrustServer *ts;
   int simul_dauer;
};





//==============================================================================
//==============================================================================
// S I M U L A T I O N E N
//==============================================================================
//==============================================================================

Simulator* simul_0()
{
	// 100 Meschen, 50 Rands zu Begin (mehr Menschen als RANDs)
   // Vertrauensbeziehungen haben eine Länge von 5 Takten (sehr kurz)
   // RANDs haben eine Länge von 10
   // Betrachtung läuft 20 Takte (sehr kurz)

   // TS-Verhalten:
   // 	qualifizierte Personen sind: Personen mit mehr als 2 unbenutztes RANDs
   //											  Personen mit weniger als 3 Vertrauenstakten
	//		jeden 2. Takt wird ein RAND verschenkt
   //		zufälliges Verschenken von RANDs: ausgeschaltet (1000,0)

   // PS-Verhalten:
   //		nichts tun:				0-600
   //		Vertrauen schenkem:	600-700
   //		Vertrauen entziehen:	700-700 (gar nicht)
   //		Nachrichten senden:	700-1000

	return new Simulator(
   	100, 50, 5, 10, 20, 2, 3, 0.5,
      1000, 0,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_0.TXT"
   );
}

Simulator* simul_1()
{
	// 5 Meschen, 50 Rands zu Begin	(viel mehr RANDs als Leute!!!!!, sehr wenig Meschen)
	// ansonsten wie oben
	return new Simulator(
   	100, 50, 5, 10, 20, 2, 3, 0.5, 1000, 0, 0,600, 600,700, 700,700, 700,1000,
      "PROT_1.TXT"
   );
}

Simulator* simul_2()
{
	// 5 Meschen, 5 Rands zu Begin, gleich viele Menschen und RANDs

	return new Simulator(
   	5, 5, 5, 10, 20, 2, 3, 0.5,
      1000, 0,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_2.TXT"
   );
}

Simulator* simul_3()
{
	// 10 Meschen, 5 Rands zu Begin
   // Vertrauensbeziehungen haben eine Länge von 10 Takten
   // RANDs haben eine Länge von 5
   // Betrachtung läuft 20 Takte (sehr kurz)
	return new Simulator(
   	10, 5, 10, 10, 20, 2, 3, 0.5,
      1000, 0,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_3.TXT"
   );
}

Simulator* simul_4()
{
	// 10 Meschen, 10 Rands zu Begin
   // Vertrauensbeziehungen haben eine Länge von 2 Takten (sehr sehr  kurz)
   // Betrachtung läuft 10 Takte (sehr kurz)
	return new Simulator(
   	10, 10, 2, 10, 10, 2, 3, 0.5,
      1000, 0,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_4.TXT"
   );
}


Simulator* simul_5()
{
	// 10 Meschen, 100 Rands zu Begin (viele RANDs)
   // Vertrauensbeziehungen haben eine Länge von 2 Takten (sehr kurz)
   // RANDs haben eine Länge von 2
   // Betrachtung läuft 10 Takte (sehr kurz)
   // TS-Verhalten: gibt keine qualifiziertes Personen

	return new Simulator(
   	10, 100, 2, 2, 10, 0, 3, 0.5,
      1000, 0,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_5.TXT"
   );
}

Simulator* simul_6()
{
	// 10 Meschen, keine RANDs am Anfang
   // Vertrauensbeziehungen haben eine Länge von 10 Takten
   // RANDs haben eine Länge von 10
   // Betrachtung läuft 10 Takte (sehr kurz)
   // TS-Verhalten: gibt keine qualifiziertes Personen
	//	jeden 10. Takt wird ein RAND verschenkt -> keine
   //		zufälliges Verschenken von RANDs: eingeschaltet zu 50% (500,10)

	return new Simulator(
   	10, 0, 10, 10, 10, 10, 0, 10,
      500, 10,
      0,600,    600,700,   700,700,   700,1000,
      "PROT_6.TXT"
   );
}

Simulator* simul_7()
{
	// 100 Meschen, 50 Rands zu Begin
   // Vertrauensbeziehungen haben eine Länge von 5 Takten (sehr kurz)
   // RANDs haben eine Länge von 10
   // Betrachtung läuft 20 Takte (sehr kurz)

   // TS-Verhalten:
   // 	qualifizierte Personen sind: Personen mit mehr als 2 unbenutztes RANDs
   //											  Personen mit weniger als 3 Vertrauenstakten
	//		jeden 2. Takt wird ein RAND verschenkt
   //		zufälliges Verschenken von RANDs: ausgeschaltet (1000,0)

   // PS-Verhalten:
   //		nichts tun:				0-0 (gar nicht)
   //		Vertrauen schenkem:	0-500
   //		Vertrauen entziehen:	0-0 (gar nicht)
   //		Nachrichten senden:	500-1000

	return new Simulator(
   	100, 50, 5, 10, 20, 2, 3, 0.5,
      1000, 0,
      0,0,    0,500,   0,0,   500,1000,
      "PROT_7.TXT"
   );
}

Simulator* simul_8()
{
   // Wie oben, nur, daß die Personen kein Vertrauen schenken
	return new Simulator(
   	100, 50, 5, 10, 20, 2, 3, 0.5,
      1000, 0,
      0,0,    0,0,   0,0,   500,1000,
      "PROT_8.TXT"
   );
}

Simulator* simul_9()
{
	// Langzeit-Test, mit mittleren Werten
	// 20 Meschen, 5 Rands zu Begin
   // Vertrauensbeziehungen haben eine Länge von 7 Takten
   // RANDs haben eine Länge von 7
   // Betrachtung läuft 100 Takte (sehr lang)

   // TS-Verhalten:
   // 	qualifizierte Personen sind: Personen mit mehr als 6 unbenutztes RANDs (fast unmöglich)
   //											  Personen mit weniger als 4 Vertrauenstakten (hälfte)
	//		jeden 7. Takt wird ein RAND verschenkt
   //		zufälliges Verschenken von RANDs: ausgeschaltet 1/100, maximal 2(10,3)

   // PS-Verhalten:
   //		nichts tun:				0-99
   //		Vertrauen schenken:	99-300
   //		Vertrauen entziehen:	300-340
   //		Nachrichten senden:	340-1000

	return new Simulator(
   	20, 5, 7, 7, 100, 6, 4, 1.0/7.0,
      10, 3,
      0,99,    99,300,   300,340,   340,1000,
      "PROT_9.TXT"
   );
}




void main()
{
   Simulator *sim;

   sim = simul_0(); sim->Starte(); delete sim;
   sim = simul_1(); sim->Starte(); delete sim;
   sim = simul_2(); sim->Starte(); delete sim;
   sim = simul_3(); sim->Starte(); delete sim;
   sim = simul_4(); sim->Starte(); delete sim;
   sim = simul_5(); sim->Starte(); delete sim;
   sim = simul_6(); sim->Starte(); delete sim;
   sim = simul_7(); sim->Starte(); delete sim;
   sim = simul_8(); sim->Starte(); delete sim;
   sim = simul_9(); sim->Starte(); delete sim;
}
