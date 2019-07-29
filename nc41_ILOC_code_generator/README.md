# compiler_project_Lab2
# Compiler_code_generator
<strong> Student: Nai-Fan Chen </strong>  <br />
<strong> NetId : nc41 </strong>

This is the Second part of the compiler of the language, DEMO. <br />
It just does the syntax detection and iloc code generation for now.  <br />

I use the environment on the Clear to compile all the file(scanner.l, parser.y). <br />
Just type in "make" to execute the makefile to compile all the dependency. <br />
After that, it will produce a software application, "demo". <br />
"make clean" will delete all the files that produce after "make". <br />
This compiler implements error recovery, so the compiler will find the syntax errors as many as possible. <br />

When typing ./demo <basename>.demo, it will produce <basename>.i which contains iloc.
If you does not add file name, it will take stdin as input. exameple ./demo < <basename>.demo and then generating a "iloc.i" file.

 <br />
"./demo -h" will show some basic instruction of the compiler(how to input and output the result).  <br />
The compile result will present through stdout and iloc.i. <br />
"scanner.l" is the scanner, and "parser.y" is the parser. hashtable.c is symble table. <br />
