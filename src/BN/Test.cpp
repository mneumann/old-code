
// Die Stack-Größe sollte nicht zu klein gewählt werden... 10000 ist ein guter Wert

# include "bn.hpp"
# include <iostream>
# include <conio>


bn x,y,n,erg,rest; int xx,yy; char c;
enum {HEX,DEC} mode=HEX;


void input_norm(bnP a) {
    if(mode==HEX) bn_inputhex(a);
    else if(mode==DEC) bn_inputdec(a);
}
void print_norm(bnP a) {
    if(mode==HEX) bn_printhex(a);
    else if(mode==DEC) bn_printdec(a);
}

void i2() {
    cout << "x: "; input_norm(x);
    cout << "y: "; input_norm(y);
}
void i1() {
    cout << "x: "; input_norm(x);
}
void i3() {
    cout << "x: "; input_norm(x);
    cout << "y: "; input_norm(y);
    cout << "n: "; input_norm(n); 
}





void main() {

    for(;;) { //endlos
    
    cout <<
    "BigNumbers - Copyright (c) 1998 by Michael Neumann" << endl <<
    "--------------------------------------------------" << endl <<
    "1) Addition (x+y) " << endl <<
    "2) Subtraktion (x-y)" << endl <<
    "3) Multiplikation (x*y)" << endl <<
    "4) Division (x/y)"  << endl <<
    "5) Potenzieren modulo n (x**y%n)" << endl <<
    "6) Groesster-Gemeinsamer-Teiler (von x und y)" << endl <<
    "7) 2er Logarithmus (von x)" << endl <<
    "8) 2 hoch x (x ist Integer)" << endl <<
    "9) Potenzieren (x**y) (y ist Integer)" << endl <<

    "A) AND-Verknuepfung (x AND y)" << endl << 
    "B) OR-Verknuepfung (x OR y)" << endl <<
    "C) XOR-Verknuepfung (x XOR y)" << endl <<
    "D) NOT-Verknuepfung (NOT x)" << endl <<
    "E) Links-Verschiebung (x SHL y) (y ist Integer)" << endl <<
    "F) Rechts-Verschiebung (x SHR y) (y ist Integer)" << endl <<
    "G) Hex in Dec umwandelt (x)" << endl <<
    "H) Dec in Hex umwandelt (x)" << endl <<
    "I) Input/Print-Mode = DEC" << endl <<
    "J) Input/Print-Mode = HEX (default)" << endl <<    
    "Q) Programm verlassen" << endl << endl <<    
    "Ihre Wahl: ";

    cin >> c;
    cout << endl;

    
                   
    switch(c) {
        case 'q' :
        case 'Q' : cout << endl << endl << "Auf Wiedersehen!!!" << endl; return; break;
        case '1' : i2(); bn_add(x,y); cout << "Ergebnis: "; print_norm(x); break;
        case '2' : i2(); bn_sub(x,y); cout << "Ergebnis: ";
                   if(bn_carryflag) {
                       cout << "-";
                       for(int i=0;i<BN_LEN;++i) y[i] = 0xFFFFFFFF;
                       bn_sub(y,x);
                       bn_inc(y);
                       print_norm(y);                       
                   }
                   else print_norm(x);
                   break;                
        case '3' : i2(); bn_mul(x,y,erg); cout << "Ergebnis: "; print_norm(erg); break;       
        case '4' : i2(); bn_div(x,y,erg,rest); cout << "Ergebnis: "; print_norm(erg); cout << "Rest: " ; print_norm(rest); break;
        case '5' : i3(); bn_powm(x,y,n,rest); cout << "Ergebnis: "; print_norm(rest); break;
        case '6' : i2(); bn_gcd(x,y,erg); cout << "Ergebnis: "; print_norm(erg); break;
        case '7' : i1(); cout << "Ergebnis: " << bn_log2(x) << endl; break;
        case '8' : cout << "x: "; cin >> xx; bn_pow2(erg,xx); cout << "Ergebnis: "; print_norm(erg); break;
        case '9' : i1(); cout << "y: "; cin >> yy; bn_pow(x,yy,erg); cout << "Ergebnis: "; print_norm(erg); break;        
        case 'a' :
        case 'A' : i2(); bn_and(x,y); cout << "Ergebnis: "; print_norm(x); break;
        case 'b' :
        case 'B' : i2(); bn_or(x,y); cout << "Ergebnis: "; print_norm(x); break;
        case 'c' :
        case 'C' : i2(); bn_xor(x,y); cout << "Ergebnis: "; print_norm(x); break;
        case 'd' :
        case 'D' : i1(); bn_not(x); cout << "Ergebnis: "; print_norm(x); break;
        case 'e' :
        case 'E' : i1(); cout << "y: "; cin >> yy; bn_shl(x,yy); cout << "Ergebnis: "; print_norm(x); break;
        case 'f' :
        case 'F' : i1(); cout << "y: "; cin >> yy; bn_shr(x,yy); cout << "Ergebnis: "; print_norm(x); break;
        case 'g' :
        case 'G' : cout << "x (in Hex): "; bn_inputhex(x); cout << "x (in Dec): "; bn_printdec(x); break;
        case 'h' :
        case 'H' : cout << "x (in Dec): "; bn_inputdec(x); cout << "x (in Hex): "; bn_printhex(x); break;
        case 'i' : 
        case 'I' : mode=DEC;cout << "Input/Print-Mode ist nun DEC" << endl; break;
        case 'j' :
        case 'J' : mode=HEX;cout << "Input/Print-Mode ist nun HEX" << endl; break;

        default : cout << "Falsche Eingabe!!!" << endl; break;
        break;
    };
    getch();
    cout << endl << endl << endl;

    }

    
    
}


