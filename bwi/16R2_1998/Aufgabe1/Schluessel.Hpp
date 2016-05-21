
# ifndef __Schluessel_Header__
# define __Schluessel_Header__

# include <list>
# include "bign.Hpp"
# include "memBlock.Hpp"
# include "PrimRandom.Hpp"

using namespace std;


struct Schluessel
{
	bign pot_key;
   bign mod_N;
   bool operator ==(const Schluessel& k) { return (pot_key==k.pot_key); }
   bool operator <(const Schluessel&) { return false; }	// Dummy
};

// x wird mit key ver/entschlüsselt
inline bign krypt(const bign& x, const Schluessel& key) {
	return (pot(x,key.pot_key) % key.mod_N);
}

inline void verschluesseln(const memBlock& in, memBlock& out, const Schluessel& key, bool should_crypt=false)
// input=byte-weise,  output=bign-weise
{
   out.clr();
	for( int i=0; i < in.size; i++ )
   {
   	bign a(in.ptr[i]);
      if(should_crypt) a=krypt(a,key);
      out.add(a.memoryOutput());
   }
}

inline void entschluesseln(const memBlock& in, memBlock& out, const Schluessel& key, bool should_crypt=false)
// input=bign-weise,  output=byte-weise
{
   out.clr();
   int size = in.size,x;
	char *ptr = in.ptr;
	bign a;
   char oi;
   while( size > 0 )
   {
      x = a.memoryInput(ptr);
      size -= x; ptr += x;
      if(should_crypt) a=krypt(a,key);
      oi = a.toInt();
      out.add(&oi,sizeof(char));
   }
}




class SchluesselGenerator
{
public:
	SchluesselGenerator(unsigned max_pr, int r_range)
   { pr_rand=new PrimRandom(max_pr,r_range); }
   ~SchluesselGenerator() { delete pr_rand; }

	void erzeugeSchluesselPaar(Schluessel& public_k, Schluessel& private_k)
   {
  		do {
      	unsigned r;
      	bign x,y,s,p,N;
      	list<unsigned> l;

         // 3 Primzahlen erzeugen, alle >= 5
         try {
      		for( int i=0;i<3;i++ )
            {
            	while((r=pr_rand->getNext()) < 16);	// 16, weil 16*16 > 255, und da ich Bytes verschlüssele muß das sein (N>255)
               l.push_back(r);
            }
         } catch(PrimRandom::KeinePrimzahlenMehr) { continue; }
			l.sort();	// sortieren

			x.set(l.front()); l.pop_front();	// x
      	y.set(l.front()); l.pop_front();	// y
      	s.set(l.front()); l.pop_front();	// private key
      	N=x*y;
      	p=find_p(s,x,y);					// public key

			public_k.pot_key = p;
      	public_k.mod_N = N;
			private_k.pot_key = s;
         private_k.mod_N = N;
		} while(find(public_keys.begin(), public_keys.end(), public_k) != public_keys.end());
      // solange er gefunden wird
      public_keys.push_back(public_k);
   }
	void loescheSchluessel(Schluessel& key)
   {
   	// Hier stellt sich die Frage, ob der öffentliche Schlüssel gelöscht
      // werden soll, da wenn er gelöscht wird, der private Schlüssel auch
      // quasi gelöscht wird, und somit die Person, die sich verabschiedet,
      // ein privaten Schlüssel kennt, der wieder verwendet werden könnte.

   	// public_keys.remove(key);		// ?????????
   }

private:
	bign find_p(bign s, bign x, bign y)
	{
		bign p, one(1), xy = (x-one)*(y-one);
	   for( p=one;;p=p+one ) if( (((p*s)%xy) == one) && p!=s ) return p;
	}

   PrimRandom *pr_rand;
   list<Schluessel> public_keys;
};

# endif
 