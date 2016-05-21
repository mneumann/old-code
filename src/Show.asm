 ;
 ; Copyright 1997 by Michael Neumann (neumann@s-direktnet.de)
 ;

 MODEL TINY
 LOCALS @@
   
 
 VidSeg      = 0B800h+ (PageNew/8)
 PageNew     = 2000h
 PageOld     = 0h
 
 ProgSize    = 1232
 BegAddr     = ProgSize + 100h
 MaxFileSize = 65533 - BegAddr
 CursorDown  = 80
 CursorUp    = 72
 EndEsc      = 27 
 TasteF1     = 59
 TastePos1   = 71
 TasteEnde   = 79
 HilfeBoxW   = 36
 HilfeBoxH   = 8
 HilfeBoxX   = ((80-HilfeBoxW)/2)     
 HilfeBoxY   = ((25-HilfeBoxH)/2)
		  
  
SetRandColor MACRO col
   mov     ax,1001h
   mov     bh,col
   int     10h
ENDM   

CursorOn MACRO
   mov ah,1
   mov cx,[CursorShape]
   int 10h
ENDM
CursorOff MACRO
   mov ah,3
   xor bh,bh
   int 10h
   mov [CursorShape],cx
;  mov cx,0040:[60]   
   mov ah,1
   mov cx,0100h
   int 10h
ENDM   

WaitVRetrace MACRO
LOCAL @@wait1, @@wait2
   mov     dx,03DAh
@@wait1:
   in      al,dx
   test    al,8
   jnz     @@wait1
@@wait2:
   in      al,dx
   test    al,8
   jz      @@wait2
ENDM






;北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北北







    
.CODE
   ORG 100h

@Start:
 jmp MainProgram


 Handle      dw 0
 FileLen     dw 0
 Zeile       db 0
;StatusLine  db " Tasten: F1 = Hilfe ",179," Cursortasten ",179," ",24,25," ",179," Maus ",179," ESC ",179," (c)1997 by Little-Coders ",13
 StatusLine  db " Tasten: F1 = Hilfe ",179," Cursortasten ",179," ",24,25," ",179," Maus ",179," ESC ",179," (c)1997 by Mich. Neumann ",13
 CursorShape dw 0
 TextOffPos  dw 0
 JustOffPos  dw 0
 MouseActiv  db 0
; EndeTxt     db 13,10,"-=== Michael Neumann  H攍derlinstra醗.6  75233 Tiefenbronn  Tel.:07234/7804 ===-","$"
		 
 HilfeBox db 201,34 dup(205),187
	  db 186," Cursor-Tasten: Zeile hoch/runter ",186
	  db 186," Bild-Tasten:   Seite hoch/runter ",186
	  db 186," Maus:          Zeile hoch/runter ",186
	  db 186," Maustasten:    schnell scrollen  ",186
	  db 186," Home,End:      Datei-Anfang/Ende ",186       
	  db 186," ESC:           Ende              ",186
	  db 200,34 dup(205),188
 
 
ClearScreen PROC NEAR
   ; ah = Attribut, al = Zeichen
   mov di,VidSeg
   mov es,di 
   xor di,di
   mov cx,2000-80
   rep stosw  
   
   mov cx,80
   mov ah,30h
   rep stosw
   
   ret
ENDP

comment #
DelaySome PROC
   mov ax,8600h
   xor cx,cx
   mov dx,65000
   int 15h
   ret
ENDP
#

SetPage PROC
;bx=adr
  mov ax,bx
  mov al,0ch
  mov dx,03d4h
  out dx,ax

  mov ax,bx
  and ax,0ffh
  mov ah,al
  mov al,0dh
  out dx,ax
  ret
ENDP


PrintLine PROC NEAR
   ; si = TextPos (Offset)
   ; al = Zeile
   ; beendet wenn Zeichen = 13 oder wenn PosX>=80
   
   mov bl,160
   mul bl
   ;ax = StartPos Bildschirm
   mov di,ax

   mov ax,VidSeg
   mov es,ax
   mov cx,80

@@loop2:
   mov al,cs:[si]
   cmp al,13
   je @@leerzeichen
   
   ;Tabulatoren als Leerzeichen ansehen
   cmp al,9
   jne @@fgf
   mov al,' '
   @@fgf:

   inc si
   jmp @@draw
   
@@leerzeichen:
   mov al,' '

@@draw:
   mov es:[di],al
   add di,2   
   
   dec cx
   jnz @@loop2
    
ret   
ENDP 
DrawBld PROC NEAR

   WaitVRetrace
   mov ax,1E20h  
   call ClearScreen
 
   mov [Zeile],0
   mov si,BegAddr
   add si,[TextOffPos]
   mov ax,[TextOffPos]
   mov [JustOffPos],ax

@@loop:
   mov     ax,[FileLen]
   cmp     [JustOffPos],ax
   jae     @@ende

   mov     al,[Zeile]
   call    PrintLine

   add     si,2
   mov     [JustOffPos],si
   sub     [JustOffPos],BegAddr

   inc     [Zeile]   
   cmp     [Zeile],24
   jb      @@loop
   
@@ende:

   mov    si,OFFSET StatusLine
   mov    al,24
   call   PrintLine 
   ret
ENDP



IsDownEnd PROC NEAR
   mov ax,[FileLen]
   cmp [TextOffPos],ax
   jae @@false_ende
   
   ;nach Return suchen und dann + 2   
   mov si,[TextOffPos]
   add si,BegAddr   


   mov bx,24   
   @@nochmal:

@@lp1:
   mov ax,BegAddr
   add ax,[FileLen]
   cmp si,ax
   jae @@false_ende

   cmp byte ptr cs:[si], byte ptr 13
   je @@endlp1
   inc si
   jmp @@lp1
@@endlp1:

    add si,2
    dec bx
    jnz @@nochmal


xor ax,ax
ret
    
@@false_ende:
   mov ax,-1   
   ret
ENDP


MainProgram:
   mov     ax,cs
   mov     ds,ax
   mov     ss,ax
   mov     sp,254
   
   xor     bx,bx
   mov     bl,cs:[80h]
   or      bl,bl
   jnz     @@notende
   jmp     ende
   @@notende:
   xor bx,bx
   mov bl,cs:[80h]
   mov byte ptr cs:[bx+81h],byte ptr 0


   mov     ax,3D00h
   mov     dx,82h;   OFFSET FileName
   int     21h
   jnc     @@weit
   jmp     ende
   @@weit:
   mov     [Handle],ax
   ;===- Datei ge攆fnet -===
      
   mov     ah,3fh
   mov     bx,[Handle]
   mov     cx,MaxFileSize
   mov     dx,BegAddr
   int     21h
   jnc     @@weit22   
   jmp     ende_fileclose
   @@weit22:
   mov     [FileLen],ax
   ;===- Datei gelesen -===
   mov     bx,[FileLen]
   add     bx,BegAddr
   mov     byte ptr cs:[bx],byte ptr 13
   mov     byte ptr cs:[bx+1],byte ptr 10
   
   mov bx,PageNew
   call SetPage
   CursorOff
   SetRandColor 1
   xor ax,ax
   int 33h
   cmp ax,0ffffh   
   jne @@keinemaus
   mov [MouseActiv],1

@@keinemaus:


MainLoop:
   call DrawBld
MainLoop_notdraw:
   mov ah,1
   int 16h
   jnz test_keys


   ;===- ProofMouse -===
   cmp [MouseActiv],1
   jne MainLoop_notdraw

   mov ax,3
   int 33h
   cmp bx,1
   jne @@mm1
      
 ;  call DelaySome
   jmp PageUpProc   

   @@mm1:
   cmp bx,2
   jne @@mwi
;   call DelaySome
   xor di,di      
   jmp PageDownProc
      

@@mwi:
   mov ax,0Bh
   int 33h

;   push dx
;   call DelaySome
;   pop  dx

   or dx,dx
   js @@mouse_up
   jz MainLoop_notdraw
   jns @@mouse_down
   @@mouse_down:
   jmp CursorDownProc
   @@mouse_up:
   jmp CursorUpProc

      
test_keys:   

   
   xor ah,ah
   int 16h
   
   or al,al
   jz extended_keys
   cmp al,EndEsc
   jne MainLoop_notdraw
   jmp  normal_ende
	  

extended_keys:
   cmp ah,CursorDown
   jne @@weiter1
   jmp CursorDownProc
   
@@weiter1:
   cmp ah,CursorUp
   jne @@weiter2   
   jmp CursorUpProc
   
@@weiter2:
   cmp ah,73  ;PageUp
   jne @@weiter3
   jmp PageUpProc
   
@@weiter3:
   cmp ah,81 ;PageDown
   jne @@weiter4
   xor di,di
      
   jmp PageDownProc
				

@@weiter4:
   cmp ah,TastePos1 ;Datei Anfang
   jne @@weiter5

   cmp [TextOffPos],0
   jne @@klf  
    jmp MainLoop_notdraw 
   @@klf:
   mov [TextOffPos],0
   jmp MainLoop

@@weiter5:
   cmp ah,TasteEnde ;Datei Ende
   jne @@weiter6
  mov di,1
  jmp PageDownProc
   




@@weiter6:
   cmp ah,TasteF1
   je @@wi
   jmp MainLoop_notDraw
   @@wi:
   ;===- Hilfe-Box anzeigen -=== 
   mov si,OFFSET HilfeBox
   mov di,VidSeg
   mov es,di
   mov di,((HilfeBoxY*80)+HilfeBoxX)*2

   mov ah,30h
   mov cl,HilfeBoxH

@@YLoop:

   mov ch,HilfeBoxW
@@XLoop:
   mov al,cs:[si]
   inc si
   mov es:[di],ax
   add di,2
   
   dec ch
   jnz @@XLoop 
   add di,160-(HilfeBoxW*2)
   dec cl
   jnz @@YLoop

   xor ax,ax
   int 16h
   mov ax,0Bh
   int 33h


   jmp MainLoop



   
normal_ende:    

   mov bx,PageOld
   call SetPage
   SetRandColor 0
   CursorOn
   

   mov ax,0F20h    
   mov di,VidSeg
   mov es,di 
   xor di,di
   mov cx,2000
   rep stosw  
   


jmp ende_fileclose

;wenn di=0 dann nur eine Seite nach unten
; sonst ganz runter
PageDownProc:
   mov cx,24
   @@nochmall:

   call IsDownEnd
   or ax,ax
   jz @@weiterrr
   jmp MainLoop

@@weiterrr:
   ;===- Cursor nach Unten -===
      mov ax,[FileLen]
      cmp [TextOffPos],ax
      jb @@wiir
      jmp MainLoop      
      @@wiir:

      ;nach Return suchen und dann + 2   
      mov si,[TextOffPos]
      add si,BegAddr   
      @@lp4:
      cmp byte ptr cs:[si], byte ptr 13
      je @@endlp4
      inc si
      jmp @@lp4
      @@endlp4:
      add si,2
      sub si,BegAddr
      mov [TextOffPos],si   

   ;===- -===   
   or di,di
   jnz @@nochmall
   
   dec cx
   jnz @@nochmall   
   
   jmp MainLoop






PageUpProc:
   ;===-PageUp -===
   ;===- Cursor nach Oben -===
      cmp [TextOffPos],0
      jne @efgf
       
      jmp MainLoop_notdraw
      @efgf:
      mov bx,24
      
      @@nochmal:
      mov si,[TextOffPos]
      add si,BegAddr  ;-3
      sub si,3  ;Davorige Return nicht z刪len
      
      @@lp2:
	 cmp si,BegAddr
	 jae @@we
	 mov [TextOffPos],0
	 jmp MainLoop
	 @@we:
	 
	 cmp byte ptr cs:[si],byte ptr 13
	 je @@endlp2
	 
	 dec si
	 jmp @@lp2
	 
      @@endlp2:
      add si,2
      sub si,BegAddr
      mov [TextOffPos],si
      
   ;===- -===
   
   dec bx
   jnz @@nochmal
   jmp MainLoop







CursorUpProc:
   ;===- Cursor nach Oben -===
      cmp [TextOffPos],0
      jne  @@wi
      jmp MainLoop_notdraw
      @@wi:
      mov si,[TextOffPos]
      add si,BegAddr  ;-3
      sub si,3  ;Davorige Return nicht z刪len
      
      @@lp2:
	 cmp si,BegAddr
	 jae @@we
	 mov [TextOffPos],0
	 jmp MainLoop
	 @@we:
	 
	 cmp byte ptr cs:[si],byte ptr 13
	 je @@endlp2
	 
	 dec si
	 jmp @@lp2
	 
      @@endlp2:
      add si,2
      sub si,BegAddr
      mov [TextOffPos],si
      jmp MainLoop
      
   ;===- -===







CursorDownProc:
   call IsDownEnd
   or ax,ax
   jz @@weiterr
   jmp MainLoop_notdraw

@@weiterr:
   ;===- Cursor nach Unten -===
      mov ax,[FileLen]
      cmp [TextOffPos],ax
      jb @@wii
      jmp MainLoop_notdraw
      @@wii:
      ;nach Return suchen und dann + 2   
      mov si,[TextOffPos]
      add si,BegAddr   
      @@lp1:
      cmp byte ptr cs:[si], byte ptr 13
      je @@endlp1
      inc si
      jmp @@lp1
      @@endlp1:
      add si,2
      sub si,BegAddr
      mov [TextOffPos],si   

      jmp MainLoop
   ;===- -===   



ende_fileclose:
   mov ah,3eh
   mov bx,[Handle]
   int 21h
ende:    
;    mov ah,9
;    mov dx,OFFSET EndeTxt
;    int 21h
    
    mov ax,4c00h
    int 21h

END @Start






