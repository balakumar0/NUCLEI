#pragma once

#define MAX_TOKENS 1000
#define WORD_LENGTH 100
#define STR_LENGTH 1000
#define CORRECT_ARGS 2
#define NUM_OF_KEYWORDS 13
#define MAX_VARIABLES 26
#define MAX_LINES 1000
#define MAX_TEMP 10000

#include "specific.h"

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
END,
PROG,
ERROR,
INSTRCTS,
IF_TRUE,
IF_FALSE,
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

typedef struct Node
{
  Token data;
  struct Node* sibling;
  struct Node* child;
}Node;

typedef struct Variables
{
  lisp* stored_vars[MAX_VARIABLES];
  bool set_var[MAX_VARIABLES];
}Variables;

typedef struct Output
{
  char** line;
  int index;
  bool stop;
}Output;

void test_prog(void);

void check_args(int argc, char* argv[]);

FILE* open_file(char* location);
Output* init_output(void);
Token** init_tokens(void);
char* tree_output(Node* head);

void tokenise(Token** t, Program* p);
char peek(Program* p);
void process_word(Token** t, Program* p, int index);
void process_char(Token** t, Program* p, int index);
void process_type(Token** t, Program* p, int token_index, SyntaxType tt);
void process_comment(Program* p);
char* get_name(Token* t);

Program* initialise(FILE* f);


/*Recursive descent parser which generates syntax tree*/
Node* parse_prog(Token** t);
Node* parse_instrcts(Token** t, int* current);
Node* parse_instrct(Token** t, int* index);

Node* parse_func(Token** t, int* index);
Node* parse_retfunc(Token** t, int* index);
Node* parse_list(Token** t, int* index);
Node* parse_intfunc(Token** t, int* index);
Node* parse_boolfunc(Token** t, int* index);

Node* parse_if(Token** t, int* index);
Node* parse_loop(Token** t, int* index);

Node* parse_iofunc(Token** t, int* index);
Node* parse_set(Token** t, int* index);
Node* parse_print(Token** t, int* index);


Node* parse_literal(Token** t, int* index);
Node* parse_string(Token** t, int* index);
Node* parse_list(Token** t, int* index);

Node* parse_var(Token** t, int* index);

Node* error_node(char* str, Token** t, int* index);

void explore_nodes(Node* n, char arr[], int* i, bool node_to_left);

/* Free functions */
void free_prog(Program* p);
void free_nodes(Node* n);
void free_tokens(Token** t);
void free_variables(Variables* v);
void free_output(Output* o);

/* All interpret functions - evaluates nodes and writes to Output structure*/

void interpret(Node* head, Variables* v, Output* o);
void interpret_instrcts(Node* head, Variables* v, Output* o);
void interpret_retfunc(Node* head, Variables* v, Output* o);
lisp* interpret_list(Node* head, Variables* v, Output* o);
void interpret_intfunc(Node* head, Variables* v, Output* o);
void interpret_boolfunc(Node* head, Variables* v, Output* o);
void interpret_if(Node* head, Variables* v, Output* o);
void interpret_loop(Node* head, Variables* v, Output* o);
void interpret_set(Node* head, Variables* v, Output* o);
void interpret_print(Node* head, Variables* v, Output* o);

void interpret_if(Node* head, Variables* v, Output* o);
void interpret_loop(Node* head, Variables* v, Output* o);

lisp* interpret_car(Node* head, Variables* v, Output* o);
lisp* interpret_cdr(Node* head, Variables* v, Output* o);
lisp* interpret_cons(Node* head, Variables* v, Output* o);

bool interpret_equal(Node* head, Variables* v, Output* o);
bool interpret_less(Node* head, Variables* v, Output* o);
bool interpret_greater(Node* head, Variables* v, Output* o);

lisp* interpret_plus(Node* head, Variables* v, Output* o);
lisp* interpret_length(Node* head, Variables* v, Output* o);

/*Helper functions*/

//Used for checking lists equality
bool lists_same(lisp* a, lisp* b);

//Checks if list a is less than list b
bool list_less(lisp* a, lisp* b);

//Checks if list a is greater than list b
bool list_greater(lisp* a, lisp* b);

//Assigns variable value
void set_var(Node* head, Variables* v, Output* o);

bool has_ended(Node* head);
Node* create_node(Token** t, int* index);
