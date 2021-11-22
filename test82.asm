            .model small
            .586    
            .stack 100h
            .data   
A           DW      ?
B           DW      ?
_S0         DB      "Enter a number ","$"
_S1         DB      "The answer is ","$"
            .code   
            include io.asm
start       proc    
            mov     ax,@data
            mov     ds,ax
            call    TWO
            mov     ax,04ch
            int     21h
start       endp    
                    
X           proc    
            push    bp
            mov     bp,sp
            sub     sp,12
                    
            mov     dx, offset _S0
            call    writestr
                    
            call    readint
            mov     A, bx
                    
            mov     ax,10
            mov     [bp-6],ax
                    
            mov     ax,[bp-6]
            mov     B,ax
                    
            mov     ax,20
            mov     [bp-8],ax
                    
            mov     ax,[bp-8]
            mov     [bp-4],ax
                    
            mov     ax,A
            mov     bx,B
            imul    bx
            mov     [bp-10],ax
                    
            mov     ax,[bp-4]
            add     ax,[bp-10]
            mov     [bp-12],ax
                    
            mov     ax,[bp-12]
            mov     [bp-2],ax
                    
            mov     dx, offset _S1
            call    writestr
                    
            mov     dx, [bp-2]
            call    writeint
                    
            call    writeln
                    
            add     sp,12
            pop     bp
            ret     0
X           endp    
                    
TWO         proc    
            push    bp
            mov     bp,sp
            sub     sp,4
                    
            call    X
                    
            add     sp,4
            pop     bp
            ret     0
TWO         endp    
                    
            end start
