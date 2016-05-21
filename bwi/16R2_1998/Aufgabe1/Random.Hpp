
# ifndef __Random_Header__
# define __Random_Header__

# include <time.h>


class Random
{
public:
	Random() { setSeed(); }
	Random(int s) { setSeed(s); }
	void setSeed() { seed=time(0); }
	void setSeed(int s) { seed=s; }
	int getNext()
   {
    	const int ran=31415821;
		return (seed=(mult(seed,ran)+1)%m);
	}
	int getNext(int range) { return ((getNext()/m1)*range)/m1; }
	int operator()() { return getNext(); }
	int operator()(int range) { return getNext(range); }

private:
	int seed;
	static const int m, m1;
	static int mult(int p, int q)
   {
		int p1=p/m1, p0=p%m1, q1=q/m1, q0=q%m1;
		return (((p0*q1+p1*q0)%m1)*m1+p0*q0)%m;
	}
};

const int Random::m=100000000;
const int Random::m1=10000;

# endif
