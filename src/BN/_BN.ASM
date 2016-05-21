COMMENT ~
        BN - Fast Big Numbers
        
        Copyright (c) 1998 by Michael Neumann (18.06.1998...)
        email: neumann@s-direktnet.de
        All rights reserved
        
        Language: Turbo Assembler (TASM32)
~

.386p
.model flat
locals @@

;BN_LEN = 32             ; die anzahl der DWORDs 32*sizeof(DWORD)*8 = 32*4*8 = 128*8 = 1024 bits
BN_LEN = 512

public _bn_cmpflag, _bn_carryflag, bn_add_, bn_sub_, bn_clr_, bn_mov_, bn_inc_, bn_dec_, bn_int_
public bn_shl1_, bn_shr1_, bn_addP_, bn_isZero_, bn_and_, bn_or_, bn_xor_, bn_not_
public bn_shlX32_, bn_shrX32_



.data
_bn_cmpflag   db 0
_bn_carryflag db 0


.code

;----------------------------------------------------------------------
; bn_add dst, src -> eax, edx
;----------------------------------------------------------------------
bn_add_ proc
        mov ecx,[edx]
        add [eax],ecx           ; nur's erste mal cf        
        @@i = 4
        rept BN_LEN-1                
                mov ecx,[edx+@@i]
                adc [eax+@@i],ecx                
                @@i=@@i+4
        ENDM
        setc [_bn_carryflag]
        ret
bn_add_ endp

;----------------------------------------------------------------------
; bn_sub dst, src -> eax, edx
;----------------------------------------------------------------------
bn_sub_ proc
        mov ecx,[edx]
        sub [eax],ecx           ; nur's erste mal cf        
        @@i = 4
        rept BN_LEN-1                
                mov ecx,[edx+@@i]
                sbb [eax+@@i],ecx
                @@i=@@i+4
        ENDM
        setc [_bn_carryflag]
        ret
bn_sub_ endp


;----------------------------------------------------------------------
; bn_clr x -> eax
;----------------------------------------------------------------------
bn_clr_ proc
        mov edx,edi
        mov edi,eax
        xor eax,eax        
        mov ecx,BN_LEN
        rep stosd  
        mov edi,edx      
        ret
bn_clr_ endp

;----------------------------------------------------------------------
; bn_mov dst,src -> eax edx
;----------------------------------------------------------------------
bn_mov_ proc
        push edi
        push esi
        mov edi,eax
        mov esi,edx  
        mov ecx,BN_LEN
        rep movsd
        pop esi
        pop edi
        ret
bn_mov_ endp


;----------------------------------------------------------------------
; bn_inc x -> eax
;----------------------------------------------------------------------
bn_inc_ proc
        xor edx,edx
        inc edx
        add [eax],edx
        dec edx
        @@i = 4
        rept BN_LEN-1
                adc [eax+@@i],edx
                @@i=@@i+4
        ENDM
        setc [_bn_carryflag]
        ret
bn_inc_ endp

;----------------------------------------------------------------------
; bn_dec x -> eax
;----------------------------------------------------------------------
bn_dec_ proc
        xor edx,edx
        inc edx
        sub [eax],edx
        dec edx
        @@i = 4
        rept BN_LEN-1
                sbb [eax+@@i],edx
                @@i=@@i+4
        ENDM
        setc [_bn_carryflag]
        ret
bn_dec_ endp

;----------------------------------------------------------------------
; bn_int dst, val -> eax, edx
;----------------------------------------------------------------------
bn_int_ proc
        mov [eax],edx
        mov edx,edi
        mov edi,eax
        add edi,4        
        xor eax,eax
        mov ecx,BN_LEN-1
        rep stosd          
        mov edi,edx
        ret
bn_int_ endp


;----------------------------------------------------------------------
; bn_shl1 x -> eax
;----------------------------------------------------------------------
bn_shl1_ proc
        push ebx
        xor edx,edx     ;zero, wegen or
        mov ecx,BN_LEN
        
        @@shl1_loop:        
                mov ebx,edx
                mov edx,[eax]
                shl dword ptr[eax],1
                or [eax],ebx
                
                shr edx,31
                add eax,4
                
                dec ecx
                jnz @@shl1_loop               
        pop ebx
        ret
bn_shl1_ endp

;----------------------------------------------------------------------
; bn_shr1 x -> eax
;----------------------------------------------------------------------
bn_shr1_ proc
        push ebx
        xor edx,edx     ;zero, wegen or
        mov ecx,BN_LEN
        add eax,(BN_LEN-1)*4
        
        @@shl1_loop:        
                mov ebx,edx
                mov edx,[eax]
                shr dword ptr[eax],1
                or [eax],ebx
                
                shl edx,31
                sub eax,4
                
                dec ecx
                jnz @@shl1_loop               
        pop ebx
        ret
bn_shr1_ endp

;----------------------------------------------------------------------
; bn_addP dst, inx, val -> eax, edx, ebx
;----------------------------------------------------------------------
bn_addP_ proc
        push esi
        xor esi,esi
        mov ecx,BN_LEN-1
        sub ecx,edx
                
        add [eax+4*edx],ebx
        inc edx
        
        @@addLoop:
                jnc @@ende
                adc [eax+4*edx],esi
                inc edx
                dec ecx
                jnz @@addLoop
                
        @@ende:
        setc [_bn_carryflag]
        pop esi
        ret
bn_addP_ endp

;----------------------------------------------------------------------
; bn_isZero x -> eax returns eax
;----------------------------------------------------------------------
bn_isZero_ proc
        push ebx
        mov ebx,4
        xor edx,edx
        mov ecx,BN_LEN
        
        @@cmp_loop:
                cmp [eax],edx
                jnz @@notzero
                add eax,ebx
                dec ecx
                jnz @@cmp_loop
                                
        pop ebx
        mov eax,edx
        inc eax
        ret
        
        @@notzero:
        pop ebx
        mov eax,edx
        ret                
bn_isZero_ endp

;----------------------------------------------------------------------
; bn_and dst, src -> eax edx
;----------------------------------------------------------------------
bn_and_ proc
        push ebx
        push esi
        mov esi,4        
        mov ecx,BN_LEN
        
        @@and_loop:
                mov ebx,[edx]
                and [eax],ebx
                add eax,esi
                add edx,esi
                dec ecx
                jnz @@and_loop                        
        pop esi
        pop ebx
        ret                
bn_and_ endp

;----------------------------------------------------------------------
; bn_or dst, src -> eax edx
;----------------------------------------------------------------------
bn_or_ proc
        push ebx
        push esi
        mov esi,4        
        mov ecx,BN_LEN
        
        @@or_loop:
                mov ebx,[edx]
                or [eax],ebx
                add eax,esi
                add edx,esi
                dec ecx
                jnz @@or_loop                        
        pop esi
        pop ebx
        ret                
bn_or_ endp

;----------------------------------------------------------------------
; bn_xor dst, src -> eax edx
;----------------------------------------------------------------------
bn_xor_ proc
        push ebx
        push esi
        mov esi,4        
        mov ecx,BN_LEN
        
        @@xor_loop:
                mov ebx,[edx]
                xor [eax],ebx
                add eax,esi
                add edx,esi
                dec ecx
                jnz @@xor_loop                        
        pop esi
        pop ebx
        ret                
bn_xor_ endp

;----------------------------------------------------------------------
; bn_not x -> eax
;----------------------------------------------------------------------
bn_not_ proc
        mov edx,4
        mov ecx,BN_LEN
        
        @@not_loop:
                not dword ptr [eax]
                add eax,edx
                dec ecx
                jnz @@not_loop                        
        ret                
bn_not_ endp

;----------------------------------------------------------------------
; bn_shlX32 x,shiftanz -> eax edx
;----------------------------------------------------------------------
bn_shlX32_ proc
        push ebx
        push esi
        
        and dl,31
        mov cl,dl
        mov dh,32
        sub dh,dl
        mov ch,dh
              
        or cl,cl
        jz @@ende
                        
        xor edx,edx     ;zero, wegen or
        mov esi,BN_LEN
        
        @@shlX32_loop:        
                mov ebx,edx
                mov edx,[eax]
                shl dword ptr[eax],cl  ; =rotanz%32
                or [eax],ebx
                xchg cl,ch
                shr edx,cl             ;  = 32-rotanz
                xchg cl,ch
                add eax,4
                
                dec esi
                jnz @@shlX32_loop               
        @@ende:
        pop esi
        pop ebx
        ret
bn_shlX32_ endp

;----------------------------------------------------------------------
; bn_shrX32 x,shiftanz -> eax edx
;----------------------------------------------------------------------
bn_shrX32_ proc
        push ebx
        push esi
        
        and dl,31
        mov cl,dl
        mov dh,32
        sub dh,dl
        mov ch,dh
        
        or cl,cl
        jz @@ende


        
        xor edx,edx     ;zero, wegen or
        mov esi,BN_LEN
        add eax,(BN_LEN-1)*4
        
        @@shrX32_loop:        
                mov ebx,edx
                mov edx,[eax]
                shr dword ptr[eax],cl
                or [eax],ebx
                
                xchg cl,ch
                shl edx,cl
                xchg cl,ch
                sub eax,4
                
                dec esi
                jnz @@shrX32_loop               
        @@ende:
        pop esi
        pop ebx
        ret
bn_shrX32_ endp



end
