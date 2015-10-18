.text

.extern main ;

ldr r3 , r2 ;
ldm rA , main ;
shl rB , rA ;
lgdt main ;
push r1 ;

.data
db 0xef ;