
#ifndef __PrimRandom_Header__
#define __PrimRandom_Header__

# include "Primzahl.hpp"
# include <algorithm>
# include <vector>
# include <list>

using namespace std;


class PrimRandom
{
public:
	class KeinePrimzahlenMehr {};
	PrimRandom(unsigned x, int lsz) : n(x), list_size(lsz), pos(2) { primes=new Primzahl(n); }
	~PrimRandom() { delete primes; }

   unsigned getNext()
   {
   	fill_pr_list();
      shuffle_list();
      if(pr_list.empty())
      {
			pos=2;
      	throw KeinePrimzahlenMehr();
      }
      unsigned retval = pr_list.front();
      pr_list.pop_front();
      return retval;
   }

private:
	void shuffle_list()
   {

   	vector<unsigned> shuffle_vector( pr_list.size() );
		list<unsigned>::iterator j=pr_list.begin();
      for( int i=0; i < pr_list.size(); i++, j++ ) shuffle_vector[i] = (*j);

      random_shuffle(shuffle_vector.begin(), shuffle_vector.end());
      pr_list = list<unsigned>();	// Liste löschen
      pr_list.insert(pr_list.begin(),shuffle_vector.begin(), shuffle_vector.end());
   }
   void fill_pr_list()
   {
   	while(pr_list.size() < list_size && pos < n)
      {
			if( primes->istPrimzahl(pos) ) pr_list.push_back(pos);
         ++pos;
      }
   }

	Primzahl *primes;
   unsigned n, pos;
   int list_size;
   list<unsigned> pr_list;
};

#endif