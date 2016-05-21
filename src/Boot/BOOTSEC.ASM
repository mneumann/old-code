;
; BOOTSEC.ASM
; This program cannot be larger than 512 bytes, because this is
; the size of one sector.
;
; Copyright (c) 1997 by Michael Neumann (neumann@s-direktnet.de)
;
; To create BOOTSEC.H:
;        ==> TASM BOOTSEC.ASM
;        ==> TLINK BOOTSEC
;        ==> EXE2HEXC BOOTSEC.EXE BOOTSEC.H
;
;        ==> change in BOOTSEC.H 'xxxxxx' to 'bootbuf'
;        ==> compile MK_BOOT.CPP and execute it with an
;            formatted disk in drive A   
;

; If BIOS has load itself into memory and had check all hardware,
; it loads the first physical sector from A: or from C: into memory
; at the location 0000:7C00, and starts at this point.
; Registers:
;                       DL      = bootdrive
;                       CS      = 0
;                       SS      = 0
;                       (E)IP = 7C00h
;                       (E)SP = 7C00h
;

OFFS = 7C00h

; You have to add OFFS or 7C00h whenever an offset is used.
; For example:
;             normal:           mov bx,OFFSET Message
;             in bootsector:    mov bx,OFFS+OFFSET Message

; I first tryed to do this by writing ORG 7C00h, but this is a big
; error, because the created EXE-file is larger than 32K 
; (normaly less than 1K), and the most of the 32K are zeros.


CODE SEGMENT
ASSUME CS:CODE,DS:CODE
   ORG 00h
   sti
   cld
   push cs
   pop  ds

   ; load es:di with 0B800h:0000h (text-screen memory)
   mov di,0B800h
   mov es,di
   xor di,di

   ; clear the screen
   mov cx,2000         ; 80*25 words
   mov ax,2700h+' '    ; green-white, space
   rep stosw

   mov si,OFFS+OFFSET OsMsg
   mov ah,1Fh                   ; background blue
   xor di,di                    ; destination

   ; writes text
   @@begin:
      lodsb
      or al,al
      jz short @@ende
      stosw
      jmp short @@begin
   @@ende:

   ; wait for key
   xor ax,ax
   int 16h

   
   OsMsg DB 'LCOS Version 1.00 - Little Coders Operating System  Cpr(c) 1997 by Little-Coders',0

CODE ENDS


END
