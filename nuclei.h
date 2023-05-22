#pragma once

#define MAX_TOKENS 1000
#define WORD_LENGTH 100
#define STR_LENGTH 1000
#define MAX_INPUT_LEN 10000
#define CORRECT_ARGS 2
#define NUM_OF_KEYWORDS 13
#define MAX_VARIABLES 26
#define MAX_LINES 1000
#define MAX_TEMP 10000
#define MAX_STACK 500
#define TEST_LEN 100

#include "specific.h"
#include "lisp.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum SyntaxType
{
LEFTBRACKET,
RIGHTBRACKET,
CAR,
CDR,
CONS,
PLUS,
LENGTH,
LESS,
GREATER,
EQUAL,
SET,
PRINT,
IF,
LOOP,
NIL_TYPE,
VAR,
STRING,
LITERAL,
UNKNOWN,
END
}SyntaxType;

typedef struct Token
{
  SyntaxType type;
  char* value;
}Token;

typedef struct Program
{
  char* input;
  int index;
}Program;

typedef struct LispStack
{
  lisp* data[MAX_STACK];
  int top;
}LispStack;

typedef struct IntStack
{
  int data[MAX_STACK];
  int top;
}IntStack;

typedef struct Variables
{
  lisp* stored_vars[MAX_VARIABLES];
  bool set_var[MAX_VARIABLES];
  LispStack s;
}Variables;

typedef struct Output
{
  char** line;
  int index;
  bool full;
}Output;

typedef struct ProgramInfo
{
  Token** tokens;
  int index;
  bool valid;
  IntStack s;
}ProgramInfo;


void test_parser(void);
void test_interpreter(void);

void check_args(int argc, char* argv[]);

FILE* open_file(char* location);
Output* init_output(void);
Token** init_tokens(void);

void tokenise(Token** t, Program* p);
char peek(Program* p);
void process_word(Token** t, Program* p, int index);
void process_char(Token** t, Program* p, int index);
void process_type(Token** t, Program* p, int token_index, SyntaxType tt);
void process_comment(Program* p);
char* get_name(Token* t);

Program* Initialise(FILE* f);

/*Recursive descent parser*/
void Prog(ProgramInfo* p, Variables* v, Output* o);
void Instrcts(ProgramInfo* p, Variables* v, Output* o);
void Instrct(ProgramInfo* p, Variables* v, Output* o);
bool Func(ProgramInfo* p, Variables* v, Output* o);
bool RetFunc(ProgramInfo* p, Variables* v, Output* o);
bool ListFunc(ProgramInfo* p, Variables* v, Output* o);
bool IntFunc(ProgramInfo* p, Variables* v, Output* o);
bool BoolFunc(ProgramInfo* p, Variables* v, Output* o);

bool IOFunc(ProgramInfo* p, Variables* v, Output* o);
bool Set(ProgramInfo* p, Variables* v, Output* o);
bool Print(ProgramInfo* p, Variables* v, Output* o);

bool IfStatement(ProgramInfo* p, Variables* v, Output* o);
bool WhileLoop(ProgramInfo* p, Variables* v, Output* o);
bool List(ProgramInfo* p, Variables* v, Output* o);

void set_var(ProgramInfo* p, Variables* v);

void lisp_tostring(const lisp* l, char* str);
lisp* lisp_copy(const lisp* l);

/*LispStack functions*/

void push_lisp(LispStack* s, lisp* l);
lisp* pop_lisp(LispStack* s);
bool is_empty_lisp(LispStack* s);
bool is_full_lisp(LispStack* s);

/*IntStack functions*/

void push_int(IntStack* s, int i);
int pop_int(IntStack* s);
bool is_empty_int(IntStack* s);
bool is_full_int(IntStack* s);

void interpret_boolfunc(ProgramInfo* p, Variables* v, Output* o);
void interpret_intfunc(ProgramInfo* p, Variables* v, Output* o);
void interpret_listfunc(Variables* v, SyntaxType s);
void interpret_list(ProgramInfo* p, Variables* v, Output* o, SyntaxType s);
void interpret_plus(ProgramInfo* p, Variables* v, Output* o);
void interpret_length(Variables* v);
void interpret_loop(ProgramInfo* p, Variables* v, Output* o, bool* skip);
void skip_section(ProgramInfo* p);
bool should_skip(Variables* v);

void add_output(Output* o, char* str);

/*Free functions*/

void free_variables(Variables* v);
void free_tokens(Token** t);
void free_output(Output* o);
void free_prog(Program* p);
