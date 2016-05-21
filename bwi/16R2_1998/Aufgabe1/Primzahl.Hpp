
# ifndef __Primzahl_Header__
# define __Primzahl_Header__

# include <mem.h>
 
class Primzahl
{
public:
	class ToLessMemory {};
   class ToBigIndex {};

	Primzahl(unsigned n) : N(n)
   {
		size=(N>>3)+1;
   	if( (array=new char[size]) == 0 ) throw ToLessMemory();
      memset(array,0xFF,size);
      clrbit(array,0); clrbit(array,1);
      unsigned i,j;
      for( i=2; i<=N/2; i++ ) for( j=2; j<=N/i; j++ )	clrbit(array,i*j);
   }
   ~Primzahl() { delete[] array; }
   bool istPrimzahl(unsigned x)
   {
   	if( x > N ) throw ToBigIndex();
      return getbit(array,x);
   }

private:
	unsigned size,N;
	char *array;
   static void clrbit(char*,unsigned);
   static bool getbit(char*,unsigned);
};


inline void Primzahl::clrbit(char *array, unsigned bitnr)
{
	char *arr = array+(bitnr>>3); bitnr &= 7;
	char set=1;
   if( bitnr > 0 ) set <<= bitnr;
	*arr &= (~set);
}

inline bool Primzahl::getbit(char *array, unsigned bitnr)
{
	char *arr = array+(bitnr>>3); bitnr &= 7;
	char set=1;
   if( bitnr > 0 ) set <<= bitnr;
	if( (*arr & set) == 0 ) return false;
   return true;
}

# endif
