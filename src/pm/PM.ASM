;
; PM.ASM -> PM.EXE
;
; Dieses Programm schaltet in den Protected-Mode und gibt
; dort einen Text aus.
;
; Copyright (c) 1997 by The Little Coders (Michael Neumann)
; email: neumann@s-direktnet.de
;

.386P                


PMODE_ON MACRO
   LOCAL @@inpm
   mov eax,cr0
   or eax,1
   mov cr0,eax
   db 0EBh,00h  
   
   db 0EAh
   dw OFFSET @@inpm
   dw CodeSel
   @@inpm:
ENDM

PMODE_OFF MACRO
   mov eax,cr0
   and eax,not 1
   mov cr0,eax
   db 0EBh,00h
ENDM

STAPEL SEGMENT USE16
  dw 100h
  StackPtr dw 0
STAPEL ENDS

INTERRUPTS SEGMENT USE16
   DivideError:
   push es
   push ax

   mov ax,VideoSel
   mov es,ax
   mov al,'d'
   mov ah,02h
   mov es:[0],ax

   pop ax
   pop es
   iret
INTERRUPTS ENDS


SAVE_SEGS MACRO
   push ax

   mov ax,cs
   mov ds:[CS_bk],ax
   mov ax,ds
   mov ds:[DS_bk],ax
   mov ax,es
   mov ds:[ES_bk],ax
   mov ax,fs
   mov ds:[FS_bk],ax
   mov ax,gs
   mov ds:[GS_bk],ax
   mov ax,ss
   mov ds:[SS_bk],ax

   pop ax
ENDM



CODE SEGMENT USE16
   ASSUME CS:CODE,DS:CODE,SS:STAPEL
   ORG 00h
Start:
   jmp Init
   ;

   CS_bk dw 0
   DS_bk dw 0
   ES_bk dw 0
   FS_bk dw 0
   GS_bk dw 0
   SS_bk dw 0


   GDT_Start LABEL DWORD
   dd 0
   dd 0

   dw 0FFFFh
   dw 8000h
   db 0Bh
   db 10010010b,0,0
   
   dw 0FFFFh
   DataDesc_l dw 0
   DataDesc_h db 0
   db 10010010b,0,0

   dw 0FFFFh
   CodeDesc_l dw 0
   CodeDesc_h db 0
   db 10011010b,0,0

   dw 100h
   StackDesc_l dw 0
   StackDesc_h db 0
   db 10010010b,0,0


   dw 0FFFFh
   IntrDesc_l dw 0
   IntrDesc_h db 0
   db 10011010b,0,0


   GDT_PTR LABEL FWORD
   dw 48
   dd ?

   IDT_PTR LABEL FWORD
   dw 8
   dd ?

   IDT_PTR_SAVE LABEL FWORD
   dw 0
   dd 0

   IDT_Start LABEL DWORD
   dw 0
   dw IntrSel
   db 0,10001110b
   dw 0


   VideoSel = 8
   DataSel  = 16
   CodeSel  = 24
   StackSel = 32
   IntrSel  = 40

   Meldung DB 'Hello Protected-Mode World!',0
   ;

   Init:
   mov ax,cs
   mov ds,ax
   mov ax,STAPEL
   mov ss,ax
   mov ax,OFFSET StackPtr
   mov sp,ax

   ;{ Setzt in GDT_PTR die lineare Adresse ein
   xor eax,eax
   mov ax,cs
   shl eax,4
   xor ebx,ebx
   mov bx,OFFSET GDT_Start
   add eax,ebx
   mov si,OFFSET GDT_PTR
   add si,2
   mov DWORD PTR ds:[si],eax
   ; ----------------------------------------}
   
   ;{ Setzt in IDT_PTR die lineare Adresse ein
   xor eax,eax
   mov ax,cs
   shl eax,4
   xor ebx,ebx
   mov bx,OFFSET IDT_Start
   add eax,ebx
   mov si,OFFSET IDT_PTR
   add si,2
   mov DWORD PTR ds:[si],eax
   ; ----------------------------------------}


   ;{ Schreibt die Adresse für das Data/Code/Stack-Segment
   xor eax,eax
   mov ax,cs
   mov ds:[RealCS],ax   ;für den Rücksprung in den RM
   mov ds:[RealCS2],ax   ;für den Rücksprung in den RM
   shl eax,4
   mov ds:[DataDesc_l],ax
   mov ds:[CodeDesc_l],ax
   shr eax,16
   mov ds:[DataDesc_h],al
   mov ds:[CodeDesc_h],al

   xor eax,eax
   mov ax,ss
   shl eax,4
   mov ds:[StackDesc_l],ax
   shr eax,16
   mov ds:[StackDesc_h],al

   xor eax,eax
   mov ax,INTERRUPTS
   shl eax,4
   mov ds:[IntrDesc_l],ax
   shr eax,16
   mov ds:[IntrDesc_h],al
   ; ---------------------------------------------}


   cli
   cld

   lgdt GDT_PTR
   
   
   PMODE_ON

   ;sidt IDT_PTR_SAVE
   ;lidt IDT_PTR
   sti

   mov ax,VideoSel
   mov es,ax
   mov ax,DataSel
   mov ds,ax
   xor esi,esi
   xor edi,edi
   mov si,OFFSET Meldung
   mov ah,0Fh
   @@lp:
      lodsb
      or al,al
      jz short @@ende
      stosw
   jmp short @@lp
   @@ende:
   
   
   
   PMODE_OFF
   db 0EAh
   dw OFFSET In_RM2
   RealCS2 dw 0
   In_RM2:
   xor ax,ax
   int 16h
   
   PMODE_ON


   ;{ schaltet den PM aus
   PMODE_OFF
   db 0EAh
   dw OFFSET In_RM
   RealCS dw 0
   In_RM:
   ;----------------------}
   cli

   mov ax,cs
   mov ds,ax
   mov ax,STAPEL
   mov ss,ax

   ;lidt IDT_PTR_SAVE

   sti
   xor ax,ax
   int 16h
   mov ax,4C00h
   int 21h

CODE ENDS
END Start

