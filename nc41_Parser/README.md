# compiler_project
<strong> Student: Nai-Fan Chen </strong>  <br />
<strong> NetId : nc41 </strong>

This is the first part of the compiler of the language, DEMO. <br />
It just does the syntax detection for now.  <br />

I use the environment on the Clear to compile all the file(scanner.l, parser.y). <br />
Just type in "make" to execute the makefile to compile all the dependency. <br />
After that, it will produce a software application, "demo". <br />
"make clean" will delete all the files that produce after "make". <br />
This compiler implements error recovery, so the compiler will find the syntax errors as many as possible.

 <br />
"./demo -h" will show some basic instruction of the compiler(how to input and output the result).  <br />
The compile result will present through stdout. <br />
"scanner.l" is the scanner, and "parser.y" is the parser.  <br />

