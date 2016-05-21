/*
        KEYBOARD.CPP -> KEYBOARD.EXE

        This program demonstrates the use of the 09h keyboard-handler.
        This handler is always called when an key is pressed or released.

        Note: If you compile this program with C and not with C++, 
              I think you have to change '(...)' to '()'.

        
        Compiled with Turbo C++
        Copyright (c) 1997 by Michael Neumann (neumann@s-direktnet.de)
*/


#include <dos.h>
#include <iostream.h>   // only for printing the result (works only in C++)


void interrupt ( *OldKeyboardHandler)(...);	// note: in C change to the (...) to ()
unsigned char key;

void interrupt KeyboardHandler(...)
{
  // following assembler code reads the port 0x60, on which lies keyboard inputs
  asm {
     in al,60h
     mov [key],al
  }
  // in key is now the press/release-code of the pressed/relesed key.
  // if key>128 then the key was released, else the key was pressed.
  
  // If you want to use the BIOS 16h-interrupt you have to
  // call it now.

  OldKeyboardHandler();

  // For action-games:
  // If you want only the scancode of the pressed key, you can
  // erase the 'OldKeyboardHandler();' and write following:
  // asm {
  //   mov al,20h
  //   out 20h,al
  // }
}


void InitHandler()
{
   OldKeyboardHandler = getvect(0x09);
   setvect( 0x09, KeyboardHandler);
}

void ExitHandler()
{
   setvect(0x09, OldKeyboardHandler);
}


// main program
void main()
{
   InitHandler();
   
   // repeat until escape is pressed and released
   while( key != 1+128 )        // escape release-code 
                                // scancode of escape is 1 -> release=1+128   
   {
      if( key > 128 ) cout << "Released key: " << (int) key-128 << endl;
      else cout << "Pressed key: " << (int) key << endl;
   }
   ExitHandler();
}
