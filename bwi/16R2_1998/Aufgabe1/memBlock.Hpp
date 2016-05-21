
# ifndef __memBlock_Header__
# define __memBlock_Header__

# include <mem.h>

struct memBlock
{
	memBlock() : ptr(0), size(0) {}
   memBlock(char *p, unsigned len) : ptr(0) { equ(p,len); }
   memBlock(const memBlock& mB) : ptr(0), size(mB.size)
   {
      if(size != 0)
      {
      	ptr=new char[size];
      	memcpy(ptr,mB.ptr,size);
      }
   }
   ~memBlock() { clr(); }

   void clr() { if(ptr != 0) delete[] ptr; ptr=0; size=0; }
   void add(const memBlock& mB) { add(mB.ptr, mB.size); }
   void add(char* p, unsigned len)
   {
      if(ptr == 0) equ(p,len);
      else
      {
   		unsigned newsize=size+len;
   		char* newptr = new char[newsize];
      	memcpy(newptr,ptr,size);
      	memcpy(newptr+size,p,len);
      	delete[] ptr;
      	ptr=newptr;
      	size=newsize;
      }
   }

   void equ(const memBlock& mB) { equ(mB.ptr, mB.size); }
   void equ(char *p, unsigned len)
   {
   	if(ptr != 0) delete[] ptr;
      size=len;
     	ptr=new char[size];
     	memcpy(ptr,p,size);
   }

   
	unsigned size; char* ptr;
};

# endif