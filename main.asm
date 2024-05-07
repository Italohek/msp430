;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

NUM			.equ 	3156					; Indicar o número a ser convertido

			mov 	#NUM, R5				; R5 =  número a ser convertido
			mov 	#RESP, R6				; R6 = ponteiro para escrever a resposta
			call 	#ALG_ROM
			jmp		$
			nop

ALG_ROM:
			clr 	R10
			mov 	R5, R4
			call 	#Milhas

Retorna:
			ret

Milhas:
        	cmp 	#1000, R5
			jn 		Novecentos
			sub		#1000, R5
			mov.b	#77, 0(R6)
			inc 	R6
			jmp 	Milhas

Novecentos:
			clrn
			cmp 	#900, R5
			jn 		Quinhentos
			sub		#900, R5
			mov.b 	#67, 0(R6)
			inc 	R6
			mov.b 	#77, 0(R6)
			inc 	R6
			jmp 	Novecentos

Quinhentos:
			clrn
        	cmp 	#500, R5
        	jn 		Quatrocentos
        	sub 	#500, R5					; Subtraimos 1000 de R4
        	mov.b 	#68, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Quinhentos

Quatrocentos:
			clrn
			cmp 	#400, R5
			jn 		Centenas
			sub		#400, R5
			mov.b 	#67, 0(R6)
			inc 	R6
			mov.b 	#68, 0(R6)
			inc 	R6
			jmp 	Quatrocentos

Centenas:
			clrn
			cmp 	#100, R5
        	jn 		Noventa
        	sub 	#100, R5					; Subtraimos 1000 de R4
        	mov.b 	#67, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Centenas

Noventa:
			clrn
			cmp 	#90, R5
			jn 		Cinquenta
			sub		#90, R5
			mov.b 	#88, 0(R6)
			inc 	R6
			mov.b 	#67, 0(R6)
			inc 	R6
			jmp 	Noventa

Cinquenta:
			clrn
			cmp 	#50, R5
			jn 		Quarenta
        	sub 	#50, R5					; Subtraimos 1000 de R4
        	mov.b 	#76, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Cinquenta

Quarenta:
			clrn
			cmp 	#40, R5
			jn 		Dezenas
			sub		#40, R5
			mov.b 	#88, 0(R6)
			inc 	R6
			mov.b 	#76, 0(R6)
			inc 	R6
			jmp 	Dezenas

Dezenas:
			clrn
			cmp 	#10, R5
			jn 		Nove
        	sub 	#10, R5					; Subtraimos 1000 de R4
        	mov.b 	#88, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Dezenas

Nove:
			clrn
			cmp 	#9, R5
			jn 		Cinco
			sub		#9, R5
			mov.b 	#73, 0(R6)
			inc 	R6
			mov.b 	#88, 0(R6)
			inc 	R6
			jmp 	Nove

Cinco:
			clrn
			cmp 	#5, R5
			jn 		Quatro
        	sub 	#5, R5					; Subtraimos 1000 de R4
        	mov.b 	#86, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Cinco

Quatro:
			clrn
			cmp 	#4, R5
			jn 		Um
			sub		#4, R5
			mov.b 	#73, 0(R6)
			inc 	R6
			mov.b 	#86, 0(R6)
			inc 	R6
			jmp 	Um

Um:
			cmp 	#1, R5
			jn 		Retorna
        	sub 	#1, R5				; Subtraimos 1000 de R4
        	jn 		Retorna
        	mov.b 	#73, 0(R6)            	; Armazena o número romano calculado
        	inc 	R6                  	; Avança para o próximo espaço de memória
			jmp 	Um
			nop


  			.data
RESP:
 			.byte	"RRRRRRRRRRRRRRRRRR", 0

;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
