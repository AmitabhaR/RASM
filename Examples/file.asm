.text

.public main ;
.public _fcall ;

main:

ldr r3 , 0xff ;
ldr r2 , 0x3 ;
ldr rbp , 0xffff2;
ldm rA , main ;
call main ;

_fcall:

.data

db 0xff ;

label_address:

dd 0xfff3 ;
                                       