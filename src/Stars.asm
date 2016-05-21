;
; This is a starfield demo =)
;
; Copyright (c) 1998 by Michael Neumann (neumann@s-direktnet.de)
;
; TASM stars -> TLINK /t stars -> stars.com
;

.MODEL TINY
.CODE
ORG 100h

anzstars = 1000         ; the number of stars on the screen
                        ; (max. ca. 65000/8 = 8000)


col_r = 3               ; max 13
col_g = 0               ; max 13
col_b = 12              ; max 13


START:
	mov ax,cs
	mov ds,ax
	mov ax,0a000h
	mov es,ax
	mov ax,13h
	int 10h

create_pal:
        mov dx,3c8h
        mov al,1
        out dx,al       ; start with color 1, color 0 is black
        inc dx

        mov ah,50
        mov al,50

lp:
        add al,col_r
        out dx,al
        sub al,col_r
        add al,col_g
        out dx,al
        sub al,col_g
        add al,col_b
        out dx,al
        sub al,col_b

        dec al
        dec ah
        jnz lp

        cli
        mov cx,anzstars
	mov di,offset stars

copystars:
        call random_xy
        mov [di],dx
        call random_xy
        mov [di+02],dx
        call random_z
        mov [di+04],dx
        add di,6
	dec cx
	jnz copystars
mainloop:

waitvlb:
	mov dx,3dah
	wait1:
	in al,dx
	test al,8
	jnz wait1
	wait2:
	in al,dx
	test al,8
	jz wait2

clrscr:
	mov cx,anzstars
	xor al,al
	mov si,OFFSET oldstr

redrawloop:
	mov bx,[si]
	add si,2
	mov es:[bx],al
	dec cx
	jnz redrawloop

drawstars:
	mov di,OFFSET stars
	mov si,OFFSET oldstr

nochmal:
	mov ax,[di+00]
	cwd
	idiv word ptr[di+04]
	add ax,160
	mov [x],ax		; X' = X / Z
	cmp ax,320
	ja notdraw

	mov ax,[di+02]
	cwd
	idiv word ptr[di+04]
	add ax,100
	mov [y],ax		; Y' = Y / Z
	cmp ax,200
	ja notdraw

	mov ax,320
	mul [y]
	add ax,[x]
	mov bx,ax

	mov [si],bx
        mov ax,[di+04]
        mov byte ptr es:[bx],al         ; al = color

notdraw:
	dec word ptr [di+04]
	jnz ok
        call random_xy
        mov [di+00],dx
        call random_xy
        mov [di+02],dx
        call random_z
        mov [di+04],dx

ok:
	add si,2
	add di,6
	cmp di,anzstars*6+OFFSET stars
	jb nochmal

        mov ah,1
        int 16h
        jnz ende
	jmp mainloop

ende:
	sti
        xor ax,ax
        int 16h
	mov ax,3
	int 10h
	int 20h



; returns a random value between -3999 and 3999 in dx, changes ax
random_xy proc
        mov dx,[random_seed]
        mov ax,31421
        imul dx
        add ax,13
        mov [random_seed],ax
        xor dx,dx
        div [xy_divisor]
        sub dx,3999
        ret
random_xy endp

; returns a random value between 1 and 50 in dx, changes ax
random_z proc
        mov dx,[random_seed]
        mov ax,31421
        imul dx
        add ax,13
        mov [random_seed],ax
        xor dx,dx
        div [z_divisor]
        inc dx
        ret
random_z endp




x               DW 0
y               DW 0
random_seed     DW ?
xy_divisor      DW 7999
z_divisor       DW 50
stars           DW anzstars*3 DUP(?)
oldstr          DW anzstars DUP(?)


END START
