
;Bildschirmschoner neuste Version 18.05.97
;von Michael Neumann   Copyright (c) in 1997 by Little-Coders
;email: neumann@s-direktnet.de

.MODEL TINY
.286
 

Int2FKennung =   0FCh
 
_always      =   1      ;A
_1sec        =   18     ;B
_15sec       =   273    ;C
_30sec       =   546    ;D
_45sec       =   819    ;E
_1min        =   1092   ;F
_1_5min      =   1638   ;G
_2min        =   2184   ;H
_3min        =   3276   ;I
_4min        =   4368   ;J
_5min        =   5460   ;K
_7_5min      =   8190   ;L
_10min       =   10920  ;M
_15min       =   16380  ;L
_30min       =   32760  ;M
_45min       =   49140  ;N
_60min       =   65520  ;O

Dos MACRO value
   mov     ah,value
   int     21h
ENDM

GEBE_AUS MACRO msg_txtoff 
   lea dx,msg_txtoff
   Dos 09h
ENDM

EXITCODE_X MACRO
   mov     ax,4C00h
   int     21h
ENDM 

ScreenOff MACRO
   mov     dx,03C4h
   mov     al,1
   out     dx,al
   inc     dx
   in      al,dx
   mov     ah,al
   or      ah,20h
   mov     al,1
   dec     dx
   out     dx,al
   inc     dx
   mov     al,ah
   out     dx,al
ENDM

ScreenOn MACRO
   mov     dx,03C4h
   mov     al,1
   out     dx,al
   inc     dx
   in      al,dx
   mov     ah,al
   and     ah,0DFh
   dec     dx
   mov     al,1
   out     dx,al
   inc     dx
   mov     al,ah
   out     dx,al
ENDM

.CODE
 ORG 100h
         
@@Start:
   
   jmp     @@ScrSaverInit
          
; Variablen auf die der resistente Teil zugreift

ScrSaverAktiv   db 0
counter         dw _7_5min
SetCounter      dw _7_5min
 
times_array     dw _always,_1sec,_15sec,_30sec,_45sec,_1min,_1_5min
                dw _2min,_3min,_4min,_5min,_7_5min,_10min,_15min
                dw _30min,_45min,_60min

 
Int08Handler PROC FAR
   push ax
   push dx

   cmp     WORD PTR cs:[counter],WORD PTR 0
   jne     @@intr_ende
   cmp     cs:[ScrSaverAktiv],1
   je      @@intr_ende2
   inc     cs:[ScrSaverAktiv] 
   ScreenOff
   jmp     @@intr_ende2

@@intr_ende:   
   dec     WORD PTR cs:[counter]
   cmp     cs:[ScrSaverAktiv],1
   jne     short @@intr_ende2
   dec     cs:[ScrSaverAktiv]
  
   ;Adresse wieder auf normal sezten
   ScreenOn

@@intr_ende2:
   
   pop     dx
   pop     ax
   
   ;Alten Interrupt aufrufen   
   db     0EAh
   AlterInt8 dw ?,?
   
Int08Handler ENDP




Int09Handler PROC FAR

   push    ax
   in      al,60h
   or      al,al
   jz      short @@i_ende
   mov     ax,cs:[SetCounter]
   mov     cs:[counter],ax
   cmp     cs:[ScrSaverAktiv],1
   je      short @@own_exit 
   
@@i_ende:
   ;Alten Interrupt aufrufen      
   pop     ax
   db      0EAh
   AlterInt9 dw ?,?
   
@@own_exit:
   mov     al,20h
   out     20h,al
   pop     ax
   iret   
   
Int09Handler ENDP



Int2FHandler PROC FAR
   cmp     ah,Int2FKennung
   je      short @@ForMe
   db      0EAh
   AlterInt2F dw ?,?
   
@@ForMe:
   cmp     al,1
   jz      short @@GetSegm
   mov     al,ah
   ror     al,4
   iret

@@GetSegm:
   push    cs
   pop     es
   iret

Int2FHandler ENDP


IntMouseHandler PROC FAR
  push    WORD PTR cs:[SetCounter]
  pop     WORD PTR cs:[counter]
  retf
IntMouseHandler ENDP



Transient LABEL word

@@ScrSaverInit:  

   GEBE_AUS MsgAll
 
   xor     bx,bx
   mov     bl,cs:[80h]
   or      bl,bl
   jz      @@nostatusline

   mov     bl,BYTE PTR cs:[82h]
   cmp     bl,'O'
   jbe     @@notsub
   sub     bl,'a'-'A'
   @@notsub:
   cmp     bl,'O'
   ja      @@nostatusline 
   cmp     bl,'A'
   jb      @@nostatusline
   sub     bl,'A'
   
   shl     bx,1
   add     bx,OFFSET times_array
   mov     ax,cs:[bx]
   mov     cs:[SetCounter],ax
   
 
@@nostatusline:



 
   mov     ah,Int2FKennung
   xor     al,al
   push    ax
   int     2Fh
   pop     bx
   ror     bh,4
   or      al,al
   jnz     @@weiter2
   jmp     @@Install
   @@weiter2:
   cmp     bh,al
   jz      short @@IstSchon

   GEBE_AUS Msg3
   EXITCODE_X
   
@@IstSchon:
   ;===- Deinstallieren -===

   mov     al,1
   int     2Fh
   mov     si,es

   mov     al,2Fh
   Dos     35h
   mov     bx,es
   cmp     bx,si
   jnz     @@GehtNicht
   
   ;===- Interrupt 08 - Timer -===
   mov     es,si
   mov     al,8
   mov     dx,es:[AlterInt8]
   mov     ds,es:[AlterInt8+02]
   Dos     25h

   ;===- Interrupt 09 - Keyboard -===
   mov     es,si
   mov     al,9
   mov     dx,es:[AlterInt9]
   mov     ds,es:[AlterInt9+02]
   Dos     25h
    
   ;===- Mousehandler -===
   xor     ax,ax
   int     33h


   mov     al,2Fh
   mov     dx,es:[AlterInt2F]
   mov     ds,es:[AlterInt2F+02]
   Dos     25h  
   Dos     49h   
   push    cs
   pop     ds
 
   GEBE_AUS Msg4
   EXITCODE_X
   
@@GehtNicht:
   GEBE_AUS Msg2
   EXITCODE_X
   
@@Install:
   ;=== Mouse-Handler installieren -===
   xor     ax,ax
   int     33h
   mov     ax,0Ch
   mov     cx,1111111b
   lea     dx,IntMouseHandler
   push    es 
   push    cs
   pop     es
   int     33h
   pop     es

   ;===- Alten Interrupt 08 sichern -===
   mov     al,08h
   Dos     35h
   mov     cs:[AlterInt8],bx
   mov     cs:[AlterInt8+02],es
   
   ;===- Neuen Interrupt 08 setzten -===
   lea     dx,Int08Handler
   Dos     25h

   ;=== Alten Interrupt 09 sichern -===
   mov     al,09h
   Dos     35h
   mov     cs:[AlterInt9],bx
   mov     cs:[AlterInt9+02],es
  
   ;===- Neuen Interrupt 09 setzten -===
   lea     dx,Int09Handler
   Dos     25h


   mov     al,2Fh
   Dos     35h
   mov     cs:[AlterInt2F],bx
   mov     cs:[AlterInt2F+02],es
   lea     dx,Int2FHandler
   Dos     25h

   
   ;===- Umgebung freigeben -===
   mov     es,ds:[2Ch]
   Dos     49h
 
   GEBE_AUS Msg
   
   ;===- Programm resident beenden -===
   lea     dx,Transient
   shr     dx,4
   inc     dx
   Dos     31h


;===- Variablen fÅr den Transistenten Teil -===
MsgAll db 13,10,'SCReenSAVER  Version 1.01 Copyright (c) 1997 by Little-Coders (M.N.)',13,10,36
Msg    db       '-----------  ... wurde erfolgreich installiert.',13,10,36
Msg2   db       '-----------  ... kann nicht entfernt werden.',13,10,36
Msg3   db       '-----------  ... Kennung bereits belegt.',13,10,36
Msg4   db       '-----------  ... wurde erfolgreich entfernt.',13,10,36

end @@Start 
