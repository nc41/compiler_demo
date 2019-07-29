# compiler_project_Lab3
# Compiler_iloc code_optimizer
<strong> Student: Nai-Fan Chen </strong>  <br />
<strong> NetId : nc41 </strong>

This is the Thirth part of the compiler of the language, DEMO. <br />
It does the syntax detection, iloc code generation and optimization for now.  <br />

I use the environment on the Clear to compile all the file(scanner.l, parser.y). <br />
Just type in "make" to execute the makefile to compile all the dependency. <br />
After that, it will produce a software application, "opt". <br />
"make clean" will delete all the files that produce after "make". <br />
This compiler implements error recovery, so the compiler will optimizer the iloc code. <br />

When typing ./opt [-v] [-u] <basename>.i, it will produce output.i which contains iloc.
If you does not add file name, it will fail since this program does not support stdin.


 
