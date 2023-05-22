#pragma once

#include "lisp.h"
#include <ctype.h>

#define NIL NULL
#define LISPIMPL "Linked"
#define MAX_CHARS 1000
#define BASE 10
#define SPACE ' '
#define LEFT_BRACKET '('
#define RIGHT_BRACKET ')'

struct lisp
{
  atomtype info;
  struct lisp* car;
  struct lisp* cdr; 
};

typedef struct list_info
{
  char list[MAX_CHARS];
  int index;
  bool node_to_left;
}list_info;

/* Recursively traverses lisp list, adding information to char array inside list_info structure*/
void traverse_lisp(const lisp* l, list_info* li);

/*Recursively builds lisp list from string*/
lisp* build_lisp(char* str, int* index);

/*Checks if a char is a number*/
bool contains_number(char c);

/*Adds number to lisp struct from string*/
void process_number(lisp* l, const char* str, int* i);

/*Writes num to temp char array */
void write_num(char c[], int num, int* index);

/*Adds necessary spaces to input string, so that input string is of 
same format as output string of lisp_tostring */
void clean_string(const char* str, char new_str[]);

/*Removes input string of all spaces except those between atoms*/
void strip_string(const char* str, char new_str[]);

/*Checks if string is invalid: does it contains invalid chars or non-equal number of left and right brackets */
bool is_valid_string(const char* str);

/*Checks if character is valid, for lisp_fromstring function*/
bool is_valid_char(char c);

/*Returns whether to add space at a location in a unformatted string*/
bool should_add_space(char* str, int index);

/*Function checks if about to add a new atom to new_str, given indices for both
old_str (i) and new_str (j) */
bool new_atom(const char* str, char* new_str, int i, int j);

/*Only for testing lisp_reduce in linked.c; prototypes not in lisp.h, so added here*/
void times(lisp* l, atomtype* n);
void atms(lisp* l, atomtype* n);
