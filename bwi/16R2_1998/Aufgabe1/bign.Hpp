
/*
	bign.hpp
   Copyright (c) 1998 by Michael Neumann

   bign ist eine Klasse, mit der man mit sehr großen Zahlen rechnen kann.
*/


# ifndef __bign_Header__
# define __bign_Header__


# include <list>
# include "memBlock.Hpp"		// für den memory-Output

using namespace std;


class bign
{
public:

	// Exception-Klassen
   class Base_Exception {};				// ungleiche Basis
   class ZeroDivision_Exception {};		// Division durch Null


	// Konstruktor
   bign(int zahl=0,int bas=10) : base(bas) { set(zahl); }


	// Operator-Funktionen
  	bign operator -();
   bign operator +(const bign&);
   bign operator -(const bign&);
   bign operator *(const bign&);
   bign operator /(const bign&);
   bign operator %(const bign&);

   bool operator ==(const bign&);
   bool operator <(const bign&);
   bool operator >(const bign& a)  {return !(operator<(a) || operator==(a));}
   bool operator >=(const bign& a) { return (operator==(a) || operator>(a)); }
   bool operator <=(const bign& a) { return !operator>(a); }


	// Andere Funktionen
   bign pot(int);
   bign pot(const bign&);
   friend bign pot(const bign& a, int potenz) { return a.pot(potenz); }
   friend bign pot(const bign& a, const bign& b) { return a.pot(b); }

	int sign();
   void set(int);
	void print();
   bign correct();
   bign zero() { return bign(0,base); }
   int toInt();

	memBlock memoryOutput();
   int memoryInput(char*);	// lädt mit dem Wert, und liefert die Anzahl der gelesenen Bytes zurück

private:
   void correct_sign();
	void div_mod(bign,bign, bign&, bign&);

  	struct gr_struct
   {
   	gr_struct() : grad(0), coeff(0) {}
      int grad, coeff;
      bool operator ==(const gr_struct& a) { return a.grad==grad; }
      bool operator <(const gr_struct& a) { return a.grad<grad; }
   };

   typedef list<gr_struct> gr_list;
   typedef gr_list::iterator gr_iter;
   typedef gr_list::reverse_iterator gr_reverse_iter;
   typedef gr_list::const_iterator gr_const_iter;

	int base;
   list<gr_struct> vec;
};

# endif


