# Makefile for ILOC optimizer

CFLAGS=-Wall -O2

opt:		opt.o machine.o instruction.o hash.o lex.yy.o iloc.tab.o
		gcc $(CFLAGS) -o opt opt.o machine.o instruction.o hash.o lex.yy.o iloc.tab.o

opt.o:		opt.c instruction.h machine.h opt.h
		gcc $(CFLAGS) -c opt.c

machine.o:	machine.c machine.h
		gcc $(CFLAGS) -c machine.c

instruction.o:	instruction.c instruction.h hash.h
		gcc $(CFLAGS) -c instruction.c

hash.o:		hash.c hash.h
		gcc $(CFLAGS) -c hash.c

lex.yy.o:	lex.yy.c
		gcc -g -c lex.yy.c

iloc.tab.o:	iloc.tab.c
		gcc -g -c iloc.tab.c

lex.yy.c:	iloc.l iloc.tab.c instruction.h
		flex iloc.l

iloc.tab.c:	iloc.y instruction.h
		bison -v -t -d iloc.y

clean:
		rm *.o
		rm output.i
		rm lex.yy.c
		rm iloc.tab.c
		rm iloc.tab.h
		rm otp

wc:		
		wc -l iloc.y iloc.l hash.h hash.c instruction.h instruction.c machine.h machine.c opt.h opt.c

export:		iloc.y iloc.l hash.c instruction.c machine.c opt.c hash.h instruction.h machine.h opt.h Makefile TestCodes nc41_LabReport.docx README.md 
		tar cvf export.tar Makefile *.c *.h *.l *.y TestCodes nc41_LabReport.docx README.md
