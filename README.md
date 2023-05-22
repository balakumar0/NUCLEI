# NUCLEI
## Background
A project submitted as coursework for the Programming in C module, for the MSc Computer Science (Conversion) course at the University of Bristol

## Project description

The program is a parser and interpreter for a LISP-like language with the following BNF:

    <PROG>     ::= "(" <INSTRCTS>

    <INSTRCTS> ::= <INSTRCT> <INSTRCTS> | ")"

    <INSTRCT>  ::= "(" <FUNC> ")"

    <FUNC>     ::= <RETFUNC> | <IOFUNC> | <IF> | <LOOP>

    <RETFUNC>  ::= <LISTFUNC> | <INTFUNC> | <BOOLFUNC>

    <LISTFUNC> ::= "CAR" <LIST> | "CDR" <LIST> | "CONS" <LIST> <LIST>

    <INTFUNC>  ::= "PLUS" <LIST> <LIST> | "LENGTH" <LIST>

    <BOOLFUNC> ::= "LESS" <LIST> <LIST> | "GREATER" <LIST> <LIST> | "EQUAL" <LIST> <LIST>

    <IOFUNC>   ::= <SET> | <PRINT>
    <SET>      ::= "SET" <VAR> <LIST>
    <PRINT>    ::= "PRINT" <LIST> | "PRINT" <STRING>

    # (IF (CONDITION) ((IF-TRUE-INSTRCTS)) ((IF_FALSE_INSTRCTS)))
    <IF>       ::= "IF" "(" <BOOLFUNC> ")" "(" <INSTRCTS> "(" <INSTRCTS>

    <LOOP>     ::= "WHILE""(" <BOOLFUNC> ")" "(" <INSTRCTS>

    <LIST>     ::= <VAR> | <LITERAL> | "NIL" | "(" <RETFUNC> ")"

    # Variables are just one letter A, B, Z etc.
    <VAR>      ::= [A-Z]

    # For printing only:
    <STRING>   ::= Double-quoted string constant e.g. "Hello, World!", or "FAILURE ?"

    # Since lists appear in bracket, to differentiate them
    # from instructions, we use 'single' quotes:
    # For the parser, anything in single-quotes is fine - you don't need to "investigate" whether it's valid or not.
    <LITERAL> ::= Single-quoted list e.g. '(1)', '(1 2 3 (4 5))', or '2'


### Implementation 

I implemented a recursive descent parser, which if in interpreter mode, interprets the code as it is parsed. If any statement is a PRINT statement, the output is printed out to the console. 

## Extension

The extension also implements a recursive descent parser, but it parses the entire syntax first to build an Abstract Syntax Tree (AST), and then attempts to evaluate the AST if in interpreter mode.  The program attempts to ignore parsing errors where possible by finding the next closing bracket, and so can handle incorrect syntax within branches of an IF statement that are never evaluated. 