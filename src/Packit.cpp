/*
	Komprimier-Programm, welches das RLE- und
	das Bittable-Verfahren anwendet.

	Copyright (c) 1997 von Michael Neumann (neumann@s-direktnet.de)
*/

#include <stdio.h>
#include <io.h>

#define PACK_NONE 0
#define PACK_RLE  1
#define PACK_BTT  2


FILE *src, *dest;
long anzchars[256];
long pack_anz[256];
char pack_char[256];

int SGNEXT;
int tablesize;
long srclength;
FILE *bitstr_file;
int bitstr_count;
unsigned char bitstr_buf;

inline int zweihoch( int x )
{
   return (1L << x);
}

inline int min( int a, int b )
{
   if( a < b ) return a;
   return b;
}

struct bitpack_header
{
   long unpack_size;
   int  bitpacks;	// z.b. 7 (128)-Table
   int  tablesize;	// z.b. 127/63 oder auch nur 22
};



void Make_anzchars_Table( FILE *handle )
{
   for( int c=0; c<256; c++ ) anzchars[c] = 0;
   while( (c=fgetc( handle )) != EOF ) ++anzchars[c];
   fseek( handle, 0, SEEK_SET );
}


int FindLessUsed_Char( FILE *handle )
{
   int c, less=0;
   Make_anzchars_Table( handle );

   for( c=0; c<256; c++ )
   {
      if( anzchars[c] == 0 ) return c;
      if( anzchars[c] < anzchars[less] ) less=c;
   }
   return less;
}



void write_one_char( FILE *handle, char c )
{
   if( c == SGNEXT )
   {
      fputc( SGNEXT, handle );
      fputc( 0, handle );
   }
   fputc( c, handle );
}

void write_equal_chars( FILE *handle, char c, int anz )
{
   fputc( SGNEXT, handle );
   fputc( anz-1, handle );
   fputc( c, handle );
}

long GetEndSize_RLE()
{
   long endsize=0;
   SGNEXT = FindLessUsed_Char( src );
   ++endsize;
   int queue_anz=1,c,oldchar = fgetc( src );
   while( (c=fgetc( src )) != EOF )
   {
      if( c == oldchar )
      {
	 ++queue_anz;
	 if( queue_anz > 256 )
	 {
	    endsize+=3;
	    queue_anz -= 256;
	 }
      }
      else
      {
	 if( queue_anz > 3 ) endsize+=3;
	 else
	 {
	    if( oldchar != SGNEXT )
	    {
	       for( int i=0; i<queue_anz; i++ )
	       {
		  if( oldchar == SGNEXT ) endsize+=3;
		  else ++endsize;
	       }
	    }
	    else endsize+=3;
	 }
	 oldchar = c;
	 queue_anz=1;
      }
   }
   if( queue_anz > 3 ) endsize+=3;
   else
   {
      if( oldchar != SGNEXT )
      {
	 for( int i=0; i<queue_anz; i++ )
	 {
	    if( oldchar == SGNEXT ) endsize+=3;
	    else ++endsize;
	 }
      }
      else endsize+=3;
   }
   return endsize;
}

void pack_RLE()
{
   SGNEXT = FindLessUsed_Char( src );
   fputc( PACK_RLE, dest );

   fputc( SGNEXT, dest );
   int queue_anz=1,c,oldchar = fgetc( src );
   while( (c=fgetc( src )) != EOF )
   {
      if( c == oldchar )
      {
	 ++queue_anz;
	 if( queue_anz > 256 )
	 {
	    write_equal_chars( dest, c, 256 );
	    queue_anz -= 256;
	 }
      }
      else
      {
	 if( queue_anz > 3 ) write_equal_chars( dest, oldchar, queue_anz );
	 else
	 {
	    if( oldchar != SGNEXT )
	    {
	       for( int i=0; i<queue_anz; i++ ) write_one_char( dest, oldchar );
	    }
	    else write_equal_chars( dest, oldchar, queue_anz );
	 }
	 oldchar = c;
	 queue_anz=1;
      }
   }
   if( queue_anz > 3 ) write_equal_chars( dest, oldchar, queue_anz );
   else
   {
      if( oldchar != SGNEXT )
      {
	 for( int i=0; i<queue_anz; i++ ) write_one_char( dest, oldchar );
      }
      else write_equal_chars( dest, oldchar, queue_anz );
   }
}

void unpack_RLE()
{
   SGNEXT = fgetc( src );
   int c;
   while( (c=fgetc(src)) != EOF )
   {
      if( c == SGNEXT )
      {
	 int anzahl = fgetc( src );
	 if( anzahl != EOF )
	 {
	    int ch = fgetc( src );
	    if( ch != EOF )
	    {
	       ++anzahl;
	       for( int i=0; i<anzahl; i++ ) fputc( ch, dest );
	    }
	 }
      }
      else fputc( c, dest );
   }
}



void CopyTableSize()
{
   tablesize=0;
   for( int i=0;i<256;i++)
   {
      if( anzchars[i] != 0 )
      {
	 pack_anz[tablesize] = anzchars[i];
	 pack_char[tablesize] = (char)i;
	 ++tablesize;
      }
   }
}

void SortTable()
{
   CopyTableSize();
   int change;
   do {
      change = 0;
      for( int i=0; i< tablesize-1; i++ )           // Fehler einbauen
      {
	 if( pack_anz[i] < pack_anz[i+1] )
	 {
	    long dummy    = pack_anz[i+1];
	    pack_anz[i+1] = pack_anz[i];
	    pack_anz[i]   = dummy;

	    char dum      = pack_char[i+1];
	    pack_char[i+1]= pack_char[i];
	    pack_char[i]  = dum;
	    change=1;
	 }
      }
   } while( change == 1 );
}

long GetBestCompr(int &bestbits)
{
   long oldendsize=-1;
   bestbits=0;
   for( int bits=3; bits<8; bits++ )
   {
      long endsize=0;
      int anz = zweihoch(bits);
      for( int j=0; j<tablesize; j++)
      {
	 if( j<anz-1 ) endsize+=(pack_anz[j]*bits);
	 else endsize+=(pack_anz[j]*(bits+8));
      }
      endsize += ((sizeof(bitpack_header)*8)+ min(tablesize,anz-1));
      if( oldendsize == -1 )
      {
	 oldendsize = endsize;
	 bestbits = bits;
      }
      else if( endsize < oldendsize )
      {
	 oldendsize = endsize;
	 bestbits = bits;
      }
   }
   return (oldendsize/8)+1;
}


void init_bitstream( FILE *f )
{
   bitstr_file = f;
   bitstr_count = 0;
   bitstr_buf = 0;
}


void wr_bstr()
{
   fputc( bitstr_buf, bitstr_file );
   bitstr_count = 0;
}
void rd_bstr()
{
   bitstr_buf = fgetc( bitstr_file );
   bitstr_count = 8;
}

int read_bs( int anz )
{
   char reti;
   if( bitstr_count == 0 ) rd_bstr();

   reti = (bitstr_buf>>(8-anz));
   bitstr_buf = (bitstr_buf<<anz);
   bitstr_count-=anz;
   return reti;
}

int read_bitstream( int anz )
{
   char reti;
   if( bitstr_count == 0 ) rd_bstr();
   if( anz <= bitstr_count )
   {
      return read_bs( anz );
   }
   else
   {
      int schon = bitstr_count;
      reti = read_bs( bitstr_count );
      reti <<= (anz-schon);
      reti += read_bs( anz-schon );
      return reti;
   }
}

void write_bitstream( int anz, unsigned char c )
{
   char lastanz;
   if( bitstr_count == 8 ) wr_bstr();
   int nochanz = 8-bitstr_count;

   if( anz <= nochanz )
   {
      bitstr_buf = (bitstr_buf << anz)+c;
      bitstr_count+=anz;
      if( bitstr_count == 8 ) wr_bstr();
   }
   else
   {
      bitstr_buf = bitstr_buf<<nochanz;
      lastanz = anz-nochanz;
      char d;
      d=c>>lastanz;
      bitstr_buf += d;

      wr_bstr();
      c=c<<(8-lastanz);
      bitstr_buf=c>>(8-lastanz);

      bitstr_count = lastanz;
   }
}


void exit_bitstream()
{
   if( bitstr_count != 0 ) write_bitstream( 8-bitstr_count, 0 );
}

int IsInTable( char c )
{
   for( int i=0;i<tablesize; i++ )
   {
      if( c == pack_char[i] ) return i;
   }
   return -1;
}


long GetEndSize_BTT()
{
   Make_anzchars_Table( src );
   SortTable();
   int bestbits;
   return GetBestCompr(bestbits);
}

void pack_BTT()
{
   Make_anzchars_Table( src );
   SortTable();
   int bestbits;
   GetBestCompr(bestbits);

   fputc( PACK_BTT, dest );

   bitpack_header header;
   header.unpack_size = srclength;
   header.bitpacks = bestbits;
   header.tablesize = tablesize = min(tablesize,zweihoch( bestbits )-1);
   fwrite( (void*) &header, sizeof( header ), 1, dest );
   fwrite( (void*) pack_char, header.tablesize, 1, dest );

   init_bitstream( dest );

   int c;
   while( (c=fgetc( src )) != EOF )
   {
      int key = IsInTable( c );
      if( key != -1 ) write_bitstream( bestbits, key );
      else
      {
	 write_bitstream( bestbits, tablesize );
	 write_bitstream( 8, c );
      }
   }
   exit_bitstream();
}

void unpack_BTT()
{
   bitpack_header header;
   fread( (void*) &header, sizeof( header ), 1, src );
   fread( (void*) pack_char, header.tablesize, 1, src );
   tablesize = header.tablesize;
   int bestbits = header.bitpacks;
   int c;
   init_bitstream( src );
   for( long i=0; i<header.unpack_size; i++ )
   {
      c = read_bitstream( bestbits );
      if( c == tablesize ) fputc( read_bitstream( 8 ), dest );
      else fputc( pack_char[c] ,dest );
   }
}

void main( int narg, char *varg[] )
{
   if( narg != 4 || (varg[3][0]!='+' && varg[3][0]!='-') )
   {
      printf("USES: PACKIT.EXE file1 file2 +/-\n");
      return;
   }

   src = fopen( varg[1], "rb" );
   if( src == NULL )
   {
      printf("Coudn't open source-file\n");
      return;
   }
   srclength = filelength( fileno( src ) );
   dest = fopen( varg[2], "w+b" );
   if( dest == NULL )
   {
      printf("Coudn't open destination-file\n");
      fclose( src );
      return;
   }
   if( varg[3][0]=='+' )
   {
      long len_rle, len_btt;
      len_btt = GetEndSize_BTT();
      rewind(src);
      len_rle = GetEndSize_RLE();
      rewind(src);
      fputc( '$', dest );

      if( min( len_btt, len_rle ) >= srclength)
      {
	 fputc( PACK_NONE, dest );
	 int c;
	 while( (c=fgetc(src)) != EOF ) fputc( c, dest );
      }
      else if( len_btt >= len_rle ) pack_RLE();
      else pack_BTT();

      rewind( src );
      rewind( dest );
      printf("Kompressionsrate: %i%%\n",100-((filelength( fileno( dest ) )*100)/srclength));
   }
   else
   {
      if( fgetc( src ) == '$' )
      {
	 int c = fgetc(src);

	 if( c == PACK_NONE )
	 {
	    while( (c=fgetc(src)) != EOF ) fputc( c, dest );
	 }
	 else if( c == PACK_RLE ) unpack_RLE();
	 else if( c == PACK_BTT ) unpack_BTT();
	 else
	 {
	    fclose( src );
	    fclose( dest );
	    printf("ERROR: Falsches Fileformat!\n");
	    return;
	 }
	 printf("...ready\n");
      }
      else
      {
	 fclose( src );
	 fclose( dest );
	 printf("ERROR: Falsches Fileformat!");
	 return;
      }
   }

   fclose( src );
   fclose( dest );
}