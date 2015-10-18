; OpenGL official library for RCPU.

.text

glBegin:

	push rbp ;
	ldr rbp , rsp ;
	ldr r24 , '\n' ;
	sub rbp , 0xff ;
	subi rbp , rA , 0x3 ;	

	out r12 , 0b0001 ;	
		
	ldr rsp , rbp ;
		
	pop rbp ;

	fret ;
.data
db "Assembler RASM!\n\n\n\n" ;