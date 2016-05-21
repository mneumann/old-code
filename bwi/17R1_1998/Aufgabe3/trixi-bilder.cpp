/*****************************************************************************
*   Aufgabe Nr.3  Trixi-Bilder
*   für den 17. Bundeswettbewerb Informatik 1998
*
*   Datum:   06.11.1998
*   Autor:   Michael Neumann
*   Sprache: Borland C++ Version 5.02
*   Dauer:   Insgesamt ca. 6 Stunden
*****************************************************************************/


//
// Includefiles für "cout" und "getch"
//
#include <conio.h>
#include <iostream.h>


#define SCHWARZ 	1
#define WEISS 		0


//==============================================================================
//============================= Klasse "PixelFarbe" ============================
//==============================================================================

//
// Die Klasse "PixelFarbe" definiert die Farbe eines einzelnen Bildpunktes,
// welche sowohl im Rasterbild als auch im Wabenbild verwendet wird. Die
// Klasse "PixelFarbe" hat zwei Datenelemente, "anz_weiss" und "anz_schwarz",
// welche die Anzahl an weißen und schwarzen Farbpigmente des Pixels beinhalten.
// Je nachdem welcher Anteil überwiegt, ist ein Pixel schwarz oder weiß.
//
class PixelFarbe {
public:	// Öffentliche Konstruktoren
	PixelFarbe(int n_weiss, int n_schwarz);
   PixelFarbe();
   PixelFarbe(int farbe);

public:	// Öffentliche Methoden
   PixelFarbe operator +(const PixelFarbe& p);
	int LiefereFarbe();
   void Reduzieren();

private:	// Private Daten
	int anz_weiss, anz_schwarz;
};


//
// Default-Konstruktor: Setzt ein Pixel auf farblos
//
PixelFarbe::PixelFarbe()
{
	anz_weiss 	= 0;
   anz_schwarz = 0;
}


//
// Setzt die Anteile von schwarz und weiß eines Pixel auf die übergebenen Werte
//
PixelFarbe::PixelFarbe(int n_weiss, int n_schwarz)
{
	anz_weiss 	= n_weiss;
   anz_schwarz = n_schwarz;
}


//
// "farbe" muß entweder SCHWARZ(1) oder WEISS(0) sein.
// Es werden daraufhin die Farbanteile gesetzt.
// Dieser Konstruktor ist für die Array-Deklaration notwendig.
//
PixelFarbe::PixelFarbe(int farbe)
{
	if(farbe==SCHWARZ)
   {
   	anz_weiss   = 0;
      anz_schwarz = 1;
   }
   else if(farbe==WEISS)
   {
   	anz_weiss   = 1;
      anz_schwarz = 0;
   }
}


//
// Additions-Operator für das einfache Addieren zweier "PixelFarbe"-Objekten.
// Es werden die zwei Datenelemente (Farbanteile weiss u. schwarz) addiert.
//
PixelFarbe PixelFarbe::operator +(const PixelFarbe& p)
{
	return PixelFarbe(anz_weiss+p.anz_weiss, anz_schwarz+p.anz_schwarz);
}


//
// Gibt entweder SCHWARZ oder WEISS zurück, je nach dem welcher Farbanteil
// überwiegt.
//
int PixelFarbe::LiefereFarbe()
{
	if(anz_schwarz >= anz_weiss) return SCHWARZ;
   else return WEISS;
}


//
// Reduziert die Farbanteile, damit nicht irgendwann ein Overflow auftritt.
//
void PixelFarbe::Reduzieren()
{
   int farbe = LiefereFarbe();
	if(farbe==SCHWARZ)
   {
   	anz_weiss   = 0;
      anz_schwarz = 1;
   }
   else if(farbe==WEISS)
   {
   	anz_weiss   = 1;
      anz_schwarz = 0;
   }
}


//==============================================================================
//============================= Struktur "Position" ============================
//==============================================================================

//
// Die Struktur "Position" beschreibt die Lage eines bestimmten Punktes
// innerhalb eines Raster- oder Wabenbildes. Dazu wird eine x- und eine
// y-Koordinate benötigt, die durch die Variablen "x" und "y" beschrieben
// werden.
//
struct Position {
	Position(int xx=0, int yy=0) : x(xx), y(yy) {}
   int x,y;
};



//==============================================================================
//================================ Klasse "Bild" ===============================
//==============================================================================

//
// Die Klasse "Bild" ist die Basisklasse der Klassen "RasterBild" und
// "WabenBild". Sie enthält alle wichtigen Grundeigenschaften, die von
// beiden Bild-Typen verwendet werden. Die Klasse "Bild" beschreibt ein
// Bild welches eine bestimmte Breite und Höhe hat, und durch das Array
// "Daten" für jeden Pixel des Bildes einen Farbwert.
//
class Bild {
public:	// Öffentliche Konstruktoren und Destruktor
	Bild(int br, int ho, PixelFarbe* init);
   Bild(const Bild&);
   ~Bild();

public:	// Öffentliche Methoden
   int ZaehleSchwarzePixel();
   void ReduzierePixel();

protected:	// Geschützte Methoden
   void SetzePixel(Position p, PixelFarbe f);
   PixelFarbe LieferePixel(Position p);
	bool IstPositionOk(Position p);
   int PositionAlsIndex(Position p);

protected:	// Geschützte Datenelemente
	PixelFarbe* Daten;
   int Breite, Hoehe;
};


//
// Konstruktor der ein Bild der Breite "br" und Höhe "ho" erstellt.
// Wenn "init" == 0, dann ist das Bild farblos, ansonsten zeigt "init"
// auf ein Array, welches die Farbwerte angibt.
//
Bild::Bild(int br, int ho, PixelFarbe* init)
{
	Breite = br;
   Hoehe  = ho;
   Daten  = new PixelFarbe[Breite*Hoehe];
	for(int i=0; i<Breite*Hoehe; ++i)
   {
   	if(init != 0) Daten[i] = init[i];
   }
}


//
// Kopier-Konstruktor
//
Bild::Bild(const Bild& b)
{
	Breite = b.Breite;
   Hoehe  = b.Hoehe;
   Daten  = new PixelFarbe[Breite*Hoehe];
	for(int i=0; i<Breite*Hoehe; ++i) Daten[i] = b.Daten[i];
}


//
// Destruktor
//
Bild::~Bild()
{
	delete[] Daten;
}


//
// Zählt die Anzahl der schwarzen Pixel des Bildes und gibt den Wert zurück
//
int Bild::ZaehleSchwarzePixel()
{
	int anzahl = 0;
	for(int i=0;i<Breite*Hoehe; ++i)
   {
   	if( Daten[i].LiefereFarbe() == SCHWARZ ) ++anzahl;
   }
   return anzahl;
}


//
// Ruft für alle Pixel die Funktion "Reduzieren" auf. Wird in den Klassen
// "WabenBild" und "RasterBild" nach einer Konvertierung von einem Raster-
// in ein Wabenbild oder umgekehrt aufgerufen, damit kein Overflow auftritt.
//
void Bild::ReduzierePixel()
{
	for(int i=0; i<Breite*Hoehe; ++i) Daten[i].Reduzieren();
}


//
// Setzt den Farbwert des Pixel an Position "p" auf "f".
// Pixel wird nur gesetzt wenn die Position innerhalb des Bildes ist.
//
void Bild::SetzePixel(Position p, PixelFarbe f)
{
	if(IstPositionOk(p)) Daten[PositionAlsIndex(p)] = f;
}


//
// Liefert den Farbwert des durch "p" bestimmten Pixels. Wenn "p" ausserhalb
// des Bildes liegt, wird als farblos zurückgegeben.
//
PixelFarbe Bild::LieferePixel(Position p)
{
	if(IstPositionOk(p)) return Daten[PositionAlsIndex(p)];
   else return PixelFarbe(0,0);
}


//
// Prüft ob "p" innerhalb des Bildes liegt, und liefert "true" wenn dies der
// Fall ist, ansonsten "false".
//
bool Bild::IstPositionOk(Position p)
{
	if(p.x >= 0 && p.x < Breite && p.y >= 0 && p.y < Hoehe) return true;
   else return false;
}


//
// Wandelt die Position "p" in einen Index um, der für den Zugriff im Array
// "Daten" benötigt wird, um den Farbwert eines Pixels zu bekommen.
//
int Bild::PositionAlsIndex(Position p)
{
	return (p.y * Breite) + p.x;
}


//==============================================================================
//===================== Klassen "RasterBild" und "WabenBild" ===================
//==============================================================================

// Forward-Deklarationen
class RasterBild;
class WabenBild;


//
// Die Klasse "RasterBild" spezifiziert die Basisklasse "Bild" und erweitert
// diese um einige Methoden. Sie stellt ein Rasterbild dar.
//
class RasterBild : public Bild {
public:	// Öffentlicher Konstruktor
	RasterBild(int br, int ho, PixelFarbe* init=0)  : Bild(br,ho,init) {}

public:	// Öffentliche Datenstruktur
	struct Umgebung {	PixelFarbe pix[9]; };

public:	// Öffentliche Methoden
	void ZeigeAmBildschirm();
	Umgebung LiefereUmgebung(Position p);
   void SetzeUmgebung(Position p, Umgebung u);
   WabenBild KonvertiereZuWabenBild();
};


//
// Die Klasse "WabenBild" besitzt dasselbe Interface wie "RasterBild" nur
// stellt sie ein Wabenbild statt einem Rasterbild dar.
//
class WabenBild : public Bild {
public:	// Öffentlicher Konstruktor
	WabenBild(int br, int ho, PixelFarbe* init=0)  : Bild(br,ho,init) {}

public:	// Öffentliche Datenstruktur
	struct Umgebung {	PixelFarbe pix[7]; };

public:	// Öffentliche Methoden
	void ZeigeAmBildschirm();
	Umgebung LiefereUmgebung(Position p);
   void SetzeUmgebung(Position p, Umgebung u);
   RasterBild KonvertiereZuRasterBild();
};



//==============================================================================
//==================== Implementierung der Klasse "RasterBild" =================
//==============================================================================

//
// Gibt das Rasterbild auf dem Bilschirm aus
//
void RasterBild::ZeigeAmBildschirm()
{
	Position p;
	for(p.y=0; p.y<Hoehe; ++p.y)
   {
   	for(p.x=0; p.x<Breite; ++p.x)
      {
        	if(LieferePixel(p).LiefereFarbe() == SCHWARZ) cout << '*';
      	else cout << '.';

         cout << ' ';
      }
   	cout << endl;
   }
}


//
// Liefert die 8 umkreisenden Pixel ausgehen von "p" und den Pixel an Position
// "p" ebenso zurück. Die 9 Punkte werden in der Struktur "Umgebung"
// gespeichert.
// Die Array-Reihenfolge ist:
//   									0  1  2
//   									7  8  3
//   									6  5  4
//
RasterBild::Umgebung RasterBild::LiefereUmgebung(Position p)
{
	Umgebung x;
   x.pix[0] = LieferePixel(Position(p.x - 1, p.y - 1));
   x.pix[1] = LieferePixel(Position(p.x, p.y - 1));
   x.pix[2] = LieferePixel(Position(p.x + 1, p.y - 1));
   x.pix[3] = LieferePixel(Position(p.x + 1, p.y));
   x.pix[4] = LieferePixel(Position(p.x + 1, p.y + 1));
   x.pix[5] = LieferePixel(Position(p.x, p.y + 1));
   x.pix[6] = LieferePixel(Position(p.x - 1, p.y + 1));
   x.pix[7] = LieferePixel(Position(p.x - 1, p.y));
   x.pix[8] = LieferePixel(p);
   return x;
}


//
// Setzt die 9 Punkte gegeben in "u", in der Reihenfolge wie oben.
//
void RasterBild::SetzeUmgebung(Position p, RasterBild::Umgebung u)
{
	SetzePixel(Position(p.x - 1, p.y - 1), u.pix[0]);
   SetzePixel(Position(p.x, p.y - 1), u.pix[1]);
   SetzePixel(Position(p.x + 1, p.y - 1), u.pix[2]);
   SetzePixel(Position(p.x + 1, p.y), u.pix[3]);
   SetzePixel(Position(p.x + 1, p.y + 1), u.pix[4]);
   SetzePixel(Position(p.x, p.y + 1), u.pix[5]);
   SetzePixel(Position(p.x - 1, p.y + 1), u.pix[6]);
   SetzePixel(Position(p.x - 1, p.y), u.pix[7]);
   SetzePixel(p,u.pix[8]);
}


//
// Konvertiert sich selbst in ein Wabenbild, welches als Objekt
// zurückgelierfert wird.
//
WabenBild RasterBild::KonvertiereZuWabenBild()
{
   Position p;
	WabenBild x(Breite, Hoehe);
	RasterBild::Umgebung r;
   WabenBild::Umgebung  w;

   for(p.x=0; p.x<Breite; ++p.x)
   {
   	for(p.y=0; p.y<Hoehe; ++p.y)
      {
			r = LiefereUmgebung(p);
         w = x.LiefereUmgebung(p);
         w.pix[0] = w.pix[0] + r.pix[0] + r.pix[1];
         w.pix[1] = w.pix[1] + r.pix[1] + r.pix[2];
         w.pix[2] = w.pix[2] + r.pix[3];
         w.pix[3] = w.pix[3] + r.pix[4] + r.pix[5];
         w.pix[4] = w.pix[4] + r.pix[5] + r.pix[6];
         w.pix[5] = w.pix[5] + r.pix[7];
         w.pix[6] = w.pix[6] + r.pix[8];
         x.SetzeUmgebung(p, w);
      }
   }
   x.ReduzierePixel();
   return x;
}



//==============================================================================
//==================== Implementierung der Klasse "WabenBild" ==================
//==============================================================================

//
// Gibt das Wabenbild auf dem Bilschirm aus
//
void WabenBild::ZeigeAmBildschirm()
{
	Position p;
	for(p.y=0; p.y<Hoehe; ++p.y)
   {
   	if(p.y % 2 == 1) cout << ' ';		// Ungerade Zeilen werden eingerückt
   	for(p.x=0; p.x<Breite; ++p.x)
      {
      	if(LieferePixel(p).LiefereFarbe() == SCHWARZ) cout << '*';
         else cout << '.';

         cout << ' ';
      }
   	cout << endl;
   }
}


//
// Liefert die 6 umkreisenden Pixel ausgehen von "p" und den Pixel an Position
// "p" ebenso zurück. Die 7 Punkte werden in der Struktur "Umgebung"
// gespeichert.
// Die Array-Reihenfolge ist:
//   									 0   1
//   								  5   6   2
//   									 4   3
//
WabenBild::Umgebung WabenBild::LiefereUmgebung(Position p)
{
   Umgebung x;
   x.pix[2] = LieferePixel(Position(p.x + 1, p.y));
   x.pix[5] = LieferePixel(Position(p.x - 1, p.y));
	x.pix[6] = LieferePixel(p);

   if( p.y % 2 == 0 )	// gerader y-Wert
   {
	   x.pix[0] = LieferePixel(Position(p.x - 1, p.y - 1));
      x.pix[1] = LieferePixel(Position(p.x, p.y - 1));
	   x.pix[3] = LieferePixel(Position(p.x, p.y + 1));
      x.pix[4] = LieferePixel(Position(p.x - 1 , p.y + 1));
   }
   else						// ungerader y-Wert
   {
	   x.pix[0] = LieferePixel(Position(p.x, p.y - 1));
      x.pix[1] = LieferePixel(Position(p.x + 1, p.y - 1));
	   x.pix[3] = LieferePixel(Position(p.x + 1, p.y + 1));
      x.pix[4] = LieferePixel(Position(p.x, p.y + 1));
   }
   return x;
}


//
// Setzt die 7 Punkte gegeben in "u", in der Reihenfolge wie oben.
//
void WabenBild::SetzeUmgebung(Position p, WabenBild::Umgebung u)
{
   SetzePixel(Position(p.x + 1, p.y), u.pix[2]);
   SetzePixel(Position(p.x - 1, p.y), u.pix[5]);
	SetzePixel(p, u.pix[6]);

   if( p.y % 2 == 0 ) 	// gerader y-Wert
   {
	   SetzePixel(Position(p.x - 1, p.y - 1), u.pix[0]);
      SetzePixel(Position(p.x, p.y - 1), u.pix[1]);
	   SetzePixel(Position(p.x, p.y + 1), u.pix[3]);
      SetzePixel(Position(p.x - 1 , p.y + 1), u.pix[4]);
   }
   else						// ungerader y-Wert
   {
	   SetzePixel(Position(p.x, p.y - 1), u.pix[0]);
      SetzePixel(Position(p.x + 1, p.y - 1), u.pix[1]);
	   SetzePixel(Position(p.x + 1, p.y + 1), u.pix[3]);
      SetzePixel(Position(p.x, p.y + 1), u.pix[4]);
   }
}


//
// Konvertiert sich selbst in ein Wabenbild, welches als Objekt
// zurückgelierfert wird.
//
RasterBild WabenBild::KonvertiereZuRasterBild()
{
   Position p;
	RasterBild x(Breite, Hoehe);
	RasterBild::Umgebung r;
   WabenBild::Umgebung  w;

   for(p.x=0; p.x<Breite; ++p.x)
   {
   	for(p.y=0; p.y<Hoehe; ++p.y)
      {
			w = LiefereUmgebung(p);
         r = x.LiefereUmgebung(p);
         r.pix[0] = r.pix[0] + w.pix[0];
         r.pix[1] = r.pix[1] + w.pix[0] + w.pix[1];
         r.pix[2] = r.pix[2] + w.pix[1];
         r.pix[3] = r.pix[3] + w.pix[2];
         r.pix[4] = r.pix[4] + w.pix[3];
         r.pix[5] = r.pix[5] + w.pix[3] + w.pix[4];
         r.pix[6] = r.pix[6] + w.pix[4];
         r.pix[7] = r.pix[7] + w.pix[5];
         r.pix[8] = r.pix[8] + w.pix[6];
         x.SetzeUmgebung(p, r);
      }
   }
   x.ReduzierePixel();   
	return x;
}



//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================


//
// Definiert drei Rasterbilder der Größe 13x11
//
PixelFarbe rasterbild[3][13*11] =
{
	// 1.Rasterbild
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,0,0,0,0,0,0,0,0,0,
	0,1,0,0,1,0,0,0,0,0,0,0,0,
	0,1,0,0,0,0,0,1,0,0,0,1,0,
	0,1,0,0,0,0,1,1,1,0,1,1,1,
	0,1,0,0,1,0,0,1,0,0,0,1,0,
	0,0,1,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,

   // 2.Rasterbild (Rasterbild-Beispiel vom Aufgabenblatt)
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,0,0,0,0,1,1,1,0,0,0,0,0,
	0,0,0,0,1,1,1,1,1,0,0,0,0,
	0,0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,1,1,1,1,1,1,1,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,

	// 3.Rasterbild
   1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,0,0,0,0,0,0,0,0,0,0,0,1,
   1,0,0,0,0,0,0,0,0,0,0,0,1,
   1,0,0,1,1,1,1,1,1,1,1,1,1,
   1,0,0,1,0,0,0,0,0,0,0,0,1,
   1,0,0,1,0,0,0,0,0,0,0,0,1,
   1,0,0,1,0,0,1,1,1,1,1,1,1,
   1,0,0,1,0,0,1,0,0,0,0,0,1,
   1,0,0,1,0,0,1,0,0,0,0,0,1,
   1,0,0,1,0,0,1,0,0,0,0,0,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1
};


//
// Die von Ihnen gewünschten Bilderfolgen werden hier angezeigt.
// Siehe Dokumentations-Ausdruck.
//
void GeneriereBilder(PixelFarbe* p)
{
   cout << "Originalzustand:" << endl;
	RasterBild r(13,11,p);
   r.ZeigeAmBildschirm();

   getch();

   cout << "Nach 1. Waben-Transformation:" << endl;
   WabenBild w(r.KonvertiereZuWabenBild());
   w.ZeigeAmBildschirm();

   getch();

   cout << "Nach 1. Raster-Transformation:" << endl;
   RasterBild r2(w.KonvertiereZuRasterBild());
   r2.ZeigeAmBildschirm();

	getch();

   cout << "Nach 2. Waben-Transformation:" << endl;
   WabenBild w2(r2.KonvertiereZuWabenBild());
   w2.ZeigeAmBildschirm();

   getch();

   cout << "Nach 2. Raster-Transformation:" << endl;
   RasterBild r3(w2.KonvertiereZuRasterBild());
   r3.ZeigeAmBildschirm();

   getch();
}


//
// Testet das Umwandeln von einem Rasterbild in ein Wabenbild.
// "p" gibt das umzuwandeltde Rasterbild an.
//
void Test1(PixelFarbe* p)
{
	int vorher, nachher;			// Anzahl der schwarzen Pixel davor und danach

   // Deklariert ein "RasterBild"-Objekt der Größe 13x11 und mit dem Bild "p"
	RasterBild r(13,11,p);

   r.ZeigeAmBildschirm();		// Gibt das Rasterbild auf dem Bildschirm aus
   vorher = r.ZaehleSchwarzePixel();

   cout << endl;

	// Deklariert ein "WabenBild"-Objekt mit den Daten des zurückgegebenen
   // "WabenBild"-Objektes der Funktion "KonvertiereZuWabenBild".
   WabenBild w(r.KonvertiereZuWabenBild());

   w.ZeigeAmBildschirm();	// Gibt das Wabenbild auf dem Bildschirm aus
   nachher = w.ZaehleSchwarzePixel();

   cout << "Anzahl Pixel des neuen Bildes in Prozent vom vorherigen: " <<
   	(float) nachher / (float) vorher * 100.0 << "%" << endl;

   getch();
}


//
// Testet das Umwandeln von einem Wabenbild in ein Rasterbild.
// "p" gibt das umzuwandeltde Wabenbild an.
//
void Test2(PixelFarbe* p)
{
	int vorher, nachher;			// Anzahl der schwarzen Pixel davor und danach

   // Deklariert ein "WabenBild"-Objekt der Größe 13x11 und mit dem Bild "p"
	WabenBild w(13,11,p);

   w.ZeigeAmBildschirm();		// Gibt das Wabenbild auf dem Bildschirm aus
   vorher = w.ZaehleSchwarzePixel();

   cout << endl;

	// Deklariert ein "RasterBild"-Objekt mit den Daten des zurückgegebenen
   // "RasterBild"-Objektes der Funktion "KonvertiereZuWabenBild".
   RasterBild r(w.KonvertiereZuRasterBild());

   r.ZeigeAmBildschirm();	// Gibt das Rasterbild auf dem Bildschirm aus
   nachher = r.ZaehleSchwarzePixel();

   cout << "Anzahl Pixel des neuen Bildes in Prozent vom vorherigen: " <<
   	(float) nachher / (float) vorher * 100.0 << "%" << endl;

   getch();
}



//
// Hauptprogramm
//
void main()
{
	for(int i=0; i<3; ++i) { GeneriereBilder(rasterbild[i]); cout << "\n\n\n"; }
//	Test1(rasterbild[0]);
}



