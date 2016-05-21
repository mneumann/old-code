
# include "HashTable.Hpp"
# include <iostream>
# include <conio>

void main() {

	// Constructor:
	// der Integer im Konstruktor sollte eine Primzahl sein
	// er gibt an auf wieviele Plätze die Daten abgebildet werden
	// er kann auch weggelassen werden, da als Standard 101 verwendet wird
	// <int> ist der Typ der verknüpften Information mit dem angegebenen Text
	HashTable<int> h(13);	
	

	// insert:
	// der erste Parameter ist ein String, nach dem gesucht werden kann
	// der zweite ist eine Zahl (wenn <int> z.B. <string> ist, dann kann als 
	// zweiter Parameter ein String angegeben werden) die als Datenwert dem 
	// ersten Parameter zugeordnet wird, und bei der Suche zurückgegeben wird.	
   	h.insert("ADC",1);
   	h.insert("ADD",23);
   	h.insert("SHL",2);
   	h.insert("SHR",5);
   	h.insert("MOV",3);
   	h.insert("MUL",3);
   	h.insert("SAR",5);
   	h.insert("SAL",0);
   	h.insert("DIV",0);
   	h.insert("IDIV",5);
   	h.insert("BOUND",4);
   	h.insert("AND",6);
   	h.insert("OR",5);
   	h.insert("XOR",6);
   	h.insert("NEG",0);
   	h.insert("NOT",3);
   	h.insert("SUB",4);
	

	// remove:
	// entfernt das Element mit dem String "NEG"
	h.remove("NEG");
	

	// find:
	// sucht nach dem übergebene String und liefert einen Zeiger auf das 
	// Datenelement zurück, oder NULL wenn der String nicht gefunden wurde.
	//
	int *i = h.find("NOT");
	cout << *i << endl;
	// es erscheint eine 3 auf dem Bildschirm
	
	getch();
}
