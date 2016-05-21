
/*
	bign.cpp
   Copyright (c) 1998 by Michael Neumann

   bign ist eine Klasse, mit der man mit sehr groﬂen Zahlen rechnen kann.
*/


# include "bign.Hpp"
# include <algorithm>
# include <utility>
# include <iostream.h>



bign bign::operator -()
{
	bign retval = *this;
   for(gr_iter i = retval.vec.begin(); i != retval.vec.end(); i++)
   		(*i).coeff = -(*i).coeff;
   return retval.correct();
}


bign bign::operator +(const bign& a)
{
	if(a.base != base) throw Base_Exception();

	bign retval = a;
   gr_iter src = vec.begin(), dst = retval.vec.begin();

   while(src != vec.end())
   {
   	if(dst == retval.vec.end())
      {
      	while(src != vec.end())
         {
         	retval.vec.push_back((*src));
            src++;
         }
         break;
      }
      if((*src).grad < (*dst).grad)
      {
      	dst = retval.vec.insert(dst,(*src));
         ++src;
      }
      else if((*src).grad > (*dst).grad) ++dst;
      else
      {
      	(*dst).coeff += (*src).coeff;
      	++dst; ++src;
      }
   }

   return retval.correct();
}


bign bign::operator -(const bign& a)
{
	return (*this)+(-a);
}


bign bign::operator *(const bign& a)
{
	if(a.base != base) throw Base_Exception();

   gr_struct s;
   gr_iter x,z;
   gr_const_iter y;
	bign retval(0,base);

   for(x=vec.begin(); x != vec.end(); x++)
   {
   	for(y=a.vec.begin(); y!=a.vec.end(); y++)
      {
			s.coeff = (*x).coeff * (*y).coeff;
         s.grad = (*x).grad + (*y).grad;
         z = retval.vec.begin();
         while(z != retval.vec.end() && s.grad > (*z).grad) ++z;
			if(z != retval.vec.end() && (*z).grad == s.grad) (*z).coeff += s.coeff;
         else retval.vec.insert(z,s);
      }
   }
   return retval.correct();
}


bign bign::operator /(const bign& a)
{
	bign dummy, retval;
   div_mod((*this),a, retval, dummy);
   return retval;
}


bign bign::operator %(const bign& a)
{
	bign dummy, retval;
   div_mod((*this),a, dummy, retval);
   return retval;
}


bool bign::operator ==(const bign& a)
{
   return ((*this)-a).sign() == 0;
}


bool bign::operator <(const bign& a)
{
   if( sign() <= 0 && a.sign() >= 0 ) return true;
   if( sign() >= 0 && a.sign() <= 0 ) return false;
   if( sign() < 0 && a.sign() < 0 )
   {
      if((-(*this)+a).sign() > 0 ) return false;
      else return true;
   }
   else
   {
      if(((*this)-a).sign() > 0 ) return false;
      else return true;
   }
}


bign bign::pot(int potenz)
{
	if(potenz < 0) return zero();
	bign retval(1,base);
	for(; potenz!=0; --potenz) retval = retval * (*this);
   return retval;
}

bign bign::pot(const bign& potenz)
{
	if(potenz < potenz.zero()) return zero();
   bign retval(1,base);
	for(; potenz!=potenz.zero(); potenz=potenz-bign(1,potenz.base))
   		retval = retval * (*this);
   return retval;
}






int bign::sign()
{
	if(vec.empty()) return 0;
   else return ((*vec.rbegin()).coeff < 0 ? -1 : 1);
}


void bign::set(int zahl)
{
	vec = gr_list();
	gr_struct a; a.grad = 0;
	while(zahl != 0)
   {
		a.coeff = zahl % base; zahl /= base;
      if(a.coeff != 0) vec.push_back(a);
      a.grad++;
   }
}


void bign::print()
{
	correct_sign();
	if(vec.empty()) cout << 0 << endl;
   else
   {
   	gr_reverse_iter r = vec.rbegin();
      if((*r).coeff < 0) cout << "-";
      else cout << "+";
      for(int gr=(*r).grad; gr>=0; gr--)
      {
			if(r != vec.rend())
         {
         	if((*r).grad < gr) cout << 0;
         	else { cout << abs((*r).coeff); ++r; }
         }
         else cout << 0;
      }
      cout << endl;
   }
}


bign bign::correct()
{
	gr_iter i = vec.begin(), j;
   gr_struct ueb; ueb.grad=0; ueb.coeff=0;

   while(i != vec.end())
   {
   	if(ueb.coeff != 0)
      {
      	if((*i).grad == ueb.grad) (*i).coeff += ueb.coeff;
         else i = vec.insert(i,ueb);
      }

		ueb.coeff = (*i).coeff / base;
      (*i).coeff %= base;
  		ueb.grad = (*i).grad+1;
		j=i++;
      if((*j).coeff == 0) vec.erase(j);
   }

   if(ueb.coeff != 0)
   {
   	int x = ueb.coeff;
      while(x != 0)
      {
      	ueb.coeff = x % base;
         x /= base;
      	if(ueb.coeff != 0) vec.insert(i,ueb);
         ueb.grad++;
		}
   }
   return (*this);
}


void bign::correct_sign()
{
	if(vec.empty()) return;
   int sign = (*vec.rbegin()).coeff;

 	gr_iter i=vec.begin(), j;
   gr_struct ueb; ueb.grad=0; ueb.coeff=0;

   while(i != vec.end())
   {
   	if(ueb.coeff != 0)
      {
      	if((*i).grad == ueb.grad) (*i).coeff += ueb.coeff;
         else i = vec.insert(i,ueb);
      }
    	ueb.coeff = (*i).coeff / base;
      (*i).coeff %= base;
  		ueb.grad = (*i).grad+1;

      if(sign > 0)
      {
			if((*i).coeff < 0)
         {
         	(*i).coeff += base;
            ueb.coeff--;
         }
      }
      else
      {
			if((*i).coeff > 0)
         {
         	(*i).coeff -= base;
            ueb.coeff++;
         }
      }
		j=i++;
      if((*j).coeff == 0) vec.erase(j);
   }
}


void bign::div_mod(bign a, bign b, bign& erg, bign& rest)
{
	// errechnet: erg = a/b,  rest = a%b
   
   if(b == b.zero()) throw ZeroDivision_Exception();
	erg = erg.zero();

   int mode;

   if(a.sign() >= 0 && b.sign() > 0) mode=0;
   else if(a.sign() >= 0 && b.sign() < 0)
   {
   	b = -b;
      mode = 1;
   }
   else if(a.sign() <= 0 && b.sign() > 0)
   {
      a = -a;
      mode = 2;
   }
   else
   {
      a = -a; b = -b;
      mode = 3;
   }

   bign dummy, bas(base);
   int actual_pot = max(1, (*a.vec.rbegin()).grad-(*b.vec.rbegin()).grad);
   while(a >= b)
   {
		if(actual_pot > 1)
      {
         dummy = a - (bas.pot(actual_pot)*b);
			if(dummy >= b)
         {
         	erg = erg + bign(actual_pot);
            a = dummy;
         }
         else actual_pot--;
      }
      else
      {
			a = a - b;
      	erg = erg + bign(1,erg.base);
      }
   }

   switch( mode )
   {
   	case 0 : rest = a; break;
      case 1 : rest = a; erg = -erg; break;
      case 2 : rest = -a; erg = -erg; break;
      case 3 : rest = -a; break;
   }
}



memBlock bign::memoryOutput()	// ist eine Abwandlung von print
{
	memBlock output;
	correct_sign();
   int len=vec.size();
  	output.add((char*)&len,sizeof(len));
   output.add((char*)&base,sizeof(base));

	if(!vec.empty())
   {
		for( gr_iter i = vec.begin(); i != vec.end(); i++ )
      {
			output.add((char*)&(*i).grad,sizeof(int));
			output.add((char*)&(*i).coeff,sizeof(int));
      }
   }
   return output;
}

int bign::memoryInput(char* mem)
{
   int *i=(int*)mem;
   int pos=0;
   int len=i[pos++];
   base=i[pos++];

  	vec=list<gr_struct>();	// leere Liste
	gr_struct g;
   
   if(len > 0)
   {
		for(int j=0;j<len;j++)
      {
      	g.grad = i[pos++];
         g.coeff = i[pos++];
      	vec.push_back(g);
      }
   }
   return pos*sizeof(int);
}

int bign::toInt()		// auche eine Abwandlung von print
{
	int retval=0,sign=1;
	correct_sign();
	if(!vec.empty())
   {
   	gr_reverse_iter r = vec.rbegin();
      if((*r).coeff < 0) sign=-1;
      
      for(int gr=(*r).grad; gr>=0; gr--)
      {
      	retval*=base;
			if(r != vec.rend())
         {
         	if((*r).grad >= gr) { retval += abs((*r).coeff); ++r; }
         }
      }
   }
   return retval*sign;
}
