
/*
 *  HashTable, Copyright (c) 1998 by Michael Neumann
 *  hashpjw by P.J.Weinberg
 *
 *  03.06.1998, implemented by Michael Neumann
 */

# ifndef __HASHTABLE_HEADER__
# define __HASHTABLE_HEADER__

# include <string>
# include <list>

using namespace std;

template<class attrType>
class HashTable {
public:

	struct R
	{
		R() {}
		R(const string& s, const attrType& a) : str(s), attr(a) {}
		string str;
		attrType attr;
		bool operator  <(const R& r) {return r.str <  str;}
		bool operator ==(const R& r) {return r.str == str;}
	};


	HashTable(int sz=101)	// sz should be a prime
	{
		table = new list<R>[size=sz];
		if(!table) throw MemoryException();
	}

	
	~HashTable()
	{
		delete[] table;
	}


	bool insert(const string& str, const attrType& attr)
	{
		int i=hash(str);
		list<R>::iterator b = table[i].begin();
		while( b != table[i].end() )
		{
			if( (*b).str == str ) return false;
			++b;
		}
		table[i].push_front(R(str,attr));
		return true;
         }


	bool remove(const string& str)
	{
		int i=hash(str);
		list<R>::iterator b = table[i].begin();
		while( b != table[i].end() )
		{
			if( (*b).str == str )
			{
				table[i].erase(b);
				return true;
			}
			++b;
		}
		return false;
	}


	attrType* find(const string& str)
	{
		int i=hash(str);
		list<R>::iterator b = table[i].begin();
		while( b != table[i].end() )
		{		
			if( (*b).str == str ) return &(*b).attr;
			++b;
		}
		return 0;
	}


	list<R> *getTable()
	{
		return table;
	}


	int getSize()
	{
		return size;
	}


	class MemoryException {};



private:

	int size;
	list<R> *table;


	int hash(const string& str)
	{
		unsigned h=0, g;
		for(int i=0;i<str.length();++i)
		{
			h = (h << 4)+ str[i];
			if(g=h&0xf0000000)
			{
				h ^= g>>24;
				h ^= g;
			}
		}
		return h%size;		// size is table-size
	}

};

#endif

