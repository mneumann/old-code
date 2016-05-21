;
; Microsoft kompatibler Maustreiber für den Textmode 0x03
;
; Copyright (c) 1997 by Michael Neumann (neumann@s-direktnet.de)
;
; Assemblieren und Linken: TASM msmaus.asm   -->   TLINK msmaus.obj
;

.386P
LOCALS @@
COM1INTR  EQU  0Ch
COM1PORT  EQU  03F8h
MIRQ      EQU  04h


CODE SEGMENT USE16
ASSUME CS:CODE,DS:CODE
START:          JMP     BEGIN


; Daten
bytenum  DB 0           ; nummer des bytes, welches gerade übertragen wurde
combytes DB 3 DUP(0)    ; byte buffer, für die 3 empfangenen bytes
mousek   DB 0           ; mousekey - gedrückte maustaste
relx     DW 0
rely     DW 0
mousex   DW 0           ; mousex
mousey   DW 0           ; mousey
oldmx    DW 0           ; oldx
oldmy    DW 0           ; oldy
showm    DB 0           ; showmouse? 0=nicht 1=zeigen
oldoff   DW 0           ; alter Bld-offset
oldmousehandler DD 0    ; alter maushandler
inbyte   DB 0           ; nimmt in al,dx auf
VID_OFFS     DW 0
VID_SEG      DW 0b800h

MouseHandler PROC FAR
		PUSH    AX
		PUSH    BX
		PUSH    DX
		PUSH    DS
		MOV     AX,CS
		MOV     DS,AX


		MOV     DX,COM1PORT
		IN      AL,DX
		MOV     [inbyte],al
		AND     AL,64
		CMP     AL,64
		JNE     NOT64
		MOV     [bytenum],0                

NOT64:          XOR     AX,AX
		MOV     AL,[bytenum]
		MOV     BX,AX
		ADD     BX,OFFSET combytes
		MOV     AL,[inbyte]
		MOV     [BX],al
		INC     [bytenum]
		CMP     [bytenum],3
		JNE     NOT3
		MOV     AL,[combytes]
		AND     AL,3
		SHL     AL,6
		ADD     AL,[combytes+1]
		CBW
		
		CMP     AX,128
		JL      NOT_B128_X
		SUB     AX,256
NOT_B128_X:     ADD     [mousex],AX
		MOV     AL,[combytes]
		AND     AL,12
		SHL     AL,4
		ADD     AL,[combytes+2]
		CBW
		CMP     AX,128
		JL      NOT_B128_Y
		SUB     AX,256
NOT_B128_Y:     ADD     [mousey],AX
		MOV     AL,[combytes]
		AND     AL,32
		OR      AL,AL
		SETNZ   AL
		MOV     [mousek],al
		MOV     AL,[combytes]
		AND     AL,16
		OR      AL,AL
		SETNZ   AL
		SHL     AL,1                         
		ADD     [mousek],AL
		MOV     [bytenum],0






		CMP     [mousex],0
		JGE     @@WEIT1
		MOV     [mousex],0
@@WEIT1:        CMP     [mousex],639
		JLE     @@WEIT2
		MOV     [mousex],639
@@WEIT2:        CMP     [mousey],0
		JGE     @@WEIT3
		MOV     [mousey],0
@@WEIT3:        CMP     [mousey],399
		JLE     @@WEIT4
		MOV     [mousey],399
@@WEIT4:
		CMP     [showm],0
		JE      NOT3
		CALL    REDRAWCURSOR;oldmx,oldmy
		MOV     AX,[mousex]
		MOV     [oldmx],AX
		MOV     AX,[mousey]
		MOV     [oldmy],AX
		CALL    REDRAWCURSOR

NOT3:
		MOV     AL,20H
		OUT     20H,AL
		POP     DS
		POP     DX
		POP     BX
		POP     AX
		IRET
MouseHandler ENDP

 ;0Ch = COM1
 ;0Dh = COM2

InitMouse PROC NEAR
   ; DTR - Data Transfer Ready auf 1
   ; warten, wenn Datenleitungen = 'M'

	   CLI
	   IN       AL,21h
	   JMP      $+2
	   JMP      $+2

	   AND      AL, NOT 2 SHL (MIRQ-1)	; Maus-IRQ aktivieren
	   OUT      21h, AL

	   MOV      dx, COM1PORT+3
	   MOV      AL, 80h                	; Baudrate auf 1200 setzen
	   OUT      DX, AL
	   SUB      DX, 3
	   MOV      AX, 115200 / 1200
	   OUT      DX, AL
	   INC      DX
	   MOV      AL, AH
	   OUT      DX, AL

	   ADD      DX, 2
	   MOV      AL, 2
	   OUT      DX, AL                 

	   SUB      DX, 2
	   MOV      AL, 1
	   OUT      DX, AL                 	; Interrupts aktivieren

	   ADD      DX, 3
	   MOV      AL, 1 OR 2 OR 8        	; DTR, RTS, OUT2
	   OUT      DX, AL

	   SUB      DX, 4
	   IN       AL, DX                 	

	   MOV      AL, 20h                	; Interrupt beenden
	   OUT      20h, AL



   	   RET
InitMouse ENDP

InstallMouse PROC NEAR
   CALL InitMouse
   CLI
   XOR BX,BX
   MOV ES,BX
   MOV BX,(COM1INTR*4)
   MOV EAX,ES:[BX]
   MOV [oldmousehandler],EAX

   MOV AX,CS
   SHL EAX,16
   MOV AX,OFFSET MouseHandler
   MOV ES:[BX],EAX
   STI
   RET
InstallMouse ENDP

ResetMouse PROC NEAR
		CALL    HideMouse
		XOR     AX,AX
		MOV     [bytenum],AL
		MOV     [mousek],AL
		MOV     [mousex],AX
		MOV     [mousey],AX
		MOV     [oldmx],AX
		MOV     [oldmy],AX
		MOV     [showm],AL
		MOV     [oldoff],AX
		RET
ENDP
REDRAWCURSOR PROC NEAR

		MOV     BX,[oldmy]
		SHR     BX,4
		MOV     AX,160
		MUL     BX

		MOV     BX,[oldmx]
		SHR     BX,3
		SHL     BX,1
		ADD     BX,AX
		ADD     BX,[VID_OFFS]
		MOV     DX,ES  ;    XCHG
		MOV     AX,[VID_SEG]
		MOV     ES,AX
		MOV     AX,ES:[BX]
		ROL     AH,4
		MOV     ES:[BX],AX
		MOV     ES,DX
		RET
ENDP

HideMouse PROC NEAR
		CMP     [showm],1
		JNE     @@ENDE
		CALL    REDRAWCURSOR
		MOV     [showm],0
@@ENDE:         RET
ENDP

ShowMouse PROC NEAR
		CMP     [showm],0
		JNE     @@ENDE
		MOV     AX,[mousex]
		MOV     [oldmx],AX
		MOV     AX,[mousey]
		MOV     [oldmy],AX
		CALL    REDRAWCURSOR
		MOV     [showm],1
@@ENDE:         RET

ENDP

DeInstallMouse PROC NEAR   
   XOR BX,BX
   MOV ES,BX
   MOV BX,(COM1INTR*4)
   MOV EAX,[oldmousehandler]
   MOV ES:[BX],EAX
   RET
DeInstallMouse ENDP




BEGIN:
   MOV AX,CS
   MOV DS,AX
   MOV SS,AX
   MOV SP,20000

   CALL InstallMouse
   CALL  ResetMouse
   CALL  ShowMouse
@@again:

   MOV  AH,01
   INT  16H
   JZ   @@check

   CALL HideMouse
   MOV  AH,01
   INT  21h
   CMP  AL,13
   JNE @@ok
   MOV  AH,02
   MOV  DL,10
   INT  21H
   @@ok:
   CALL ShowMouse

   @@check:
   MOV AL,[mousek]
   OR AL,AL
   JZ @@again

   @@ende:
   CALL HideMouse
   CALL DeInstallMouse
   MOV ax,4c00h
   INT 21h

ENDS CODE
END START

