CC = gcc 
CFLAGS = -g 

OBJs = parser.tab.o hashtable.o att.o iloc_emitter.o lex.yy.o 

default: demo

demo: ${OBJs}
	${CC} ${CFLAGS} ${OBJs} -o demo 

lex.yy.c: scanner.l parser.tab.h att.h
	flex -i scanner.l

parser.tab.c: parser.y att.h hashtable.h iloc_emitter.h
	bison -dv parser.y

parser.tab.h: parser.tab.c

clean:
	rm -f lex.yy.c *.o parser.tab.[ch] parser.output iloc.i *.i demo

depend:
	makedepend -I. *.c

# DO NOT DELETE THIS LINE -- make depend depends on it.

