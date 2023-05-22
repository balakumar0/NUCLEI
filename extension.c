#include "general.h"
#include "extension.h"

int main(int argc, char* argv[])
{
  check_args(argc, argv);
  FILE* f = open_file(argv[1]);
  Program* p = initialise(f);

  Token** tokens = init_tokens();
  tokenise(tokens, p);

  free_prog(p);

  Node* n = parse_prog(tokens);

  free_tokens(tokens);

  char* preorder_traversal = tree_output(n);
  printf("N-ary tree - preorder traversal: \n\n");
  printf("%s\n\n", preorder_traversal);

  //Interpreter
  Variables* v = ncalloc(1, sizeof(Variables));
  Output* o = init_output();

  interpret(n, v, o);

  printf("Interpreter results: \n\n");

  for (int i = 0; i < o -> index; i++)
  {
    printf("%s\n", o -> line[i]);
  }

  free_nodes(n);
  free_variables(v);
  free_output(o);
  free(preorder_traversal);

  fclose(f);

  return(EXIT_SUCCESS);
}

void test_prog(void)
{
  Program* p = ncalloc(1, sizeof(Program));
  p -> input = ncalloc(STR_LENGTH, sizeof(char));
  p -> input[0] = '\0';

  char temp_str[STR_LENGTH];
  strcpy(temp_str, "SET C '5'");
  strcat(p -> input, temp_str);

  Token** t = init_tokens();
  tokenise(t, p);
  int i = 0;


  Node* n = parse_iofunc(t, &i);
  char* preorder_traversal = tree_output(n);

  strcpy(temp_str,  preorder_traversal);
  assert(strcmp(temp_str, "[SET C 5]") == 0);

}

Token** init_tokens(void)
{
  Token** tokens = ncalloc(MAX_TOKENS, sizeof(Token*));
  for (int i = 0; i < MAX_TOKENS; i++)
  {
    tokens[i] = ncalloc(1, sizeof(Token));
  }

  return tokens;
}

char* tree_output(Node* head)
{
  char* temp_arr = ncalloc(STR_LENGTH, sizeof(char));
  int k = 0;
  explore_nodes(head, temp_arr, &k, false);
  return temp_arr;
}

Output* init_output(void)
{
  Output* o = ncalloc(1, sizeof(Output));
  o -> stop = false;

  o -> line = ncalloc(MAX_LINES, sizeof(char*));

  for (int i = 0; i < MAX_LINES; i++)
  {
    o -> line[i] = ncalloc(STR_LENGTH, sizeof(char));
  }

  return o;
}

void check_args(int argc, char* argv[])
{
  if (argc != CORRECT_ARGS)
  {
    //Check filename ends in NCL?
    fprintf(stderr, "Usage: %s <filename.ncl>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
}

void explore_nodes(Node* n, char arr[], int* i, bool node_to_left)
{
  if (n == NULL)
  {
    if (node_to_left)
    {
      arr[*i] = ']';
      (*i)++;
    }
    return;
  }

  if (!node_to_left)
  {
    arr[*i] = '[';
    (*i)++;
  }

  char* temp_str = get_name(&(n -> data));

  switch(n -> data.type)
  {
   case VAR: case LITERAL: case STRING: case NIL_TYPE:
   case CAR: case CDR: case CONS: case EQUAL: case LESS:
   case GREATER: case PLUS: case IF_TRUE: case IF_FALSE:
   case LENGTH:
   {
    if (*i > 0 && arr[(*i) - 1] != '[')
    {
      arr[*i] = ' ';
      (*i)++;
    }
   }
    break;
   default:
    break;
  }

  int j = 0;
  while (temp_str[j] != '\0')
  {
    arr[*i] = temp_str[j];
    j++;
    (*i)++;
  }

  explore_nodes(n -> child, arr, i, false);
  explore_nodes(n -> sibling, arr, i, true);
}

FILE* open_file(char* location)
{
  FILE* f = fopen(location, "r");
  if (f == NULL)
  {
    fprintf(stderr, "Could not open file %s\n", location);
    exit(EXIT_FAILURE);
  }
  return f;
}

Program* initialise(FILE* f)
{
  Program* p = ncalloc(1, sizeof(Program));
  p -> input = ncalloc(STR_LENGTH, sizeof(char));
  p -> input[0] = '\0';

  char temp_str[STR_LENGTH];
  while (fgets(temp_str, STR_LENGTH, f) != NULL)
  {
    strcat(p -> input, temp_str);
  }
  return p;
}

Node* error_node(char* str, Token** t, int* index)
{
  Node* n = ncalloc(1, sizeof(Node));
  n -> data.type = ERROR;
  n -> data.value = ncalloc(strlen(str) + 1, sizeof(char));
  strcpy(n -> data.value, str);

  /*Continue to consume tokens until ')' is at end */
  (*index)++;

  bool stop_loop = false;
  int counter = 0;

  while (stop_loop != true)
  {
    switch(t[*index] -> type)
    {
      case LEFTBRACKET:
      {
        counter++;
        (*index)++;
        break;
      }
      case RIGHTBRACKET:
      {
        counter--;
        if (counter <= 0)
        {
          stop_loop = true;
        }
        break;
      }

      case END:
      {
        stop_loop = true;
        break;
      }
      default:
        (*index)++;
        break;
    }
  }

  return n;
}

Node* parse_prog(Token** t)
{
  int current = 0;
  if (t[current] -> type != LEFTBRACKET)
  {
    return error_node("No left bracket found", t, &current);
  }

  current++;

  Node* n = ncalloc(1, sizeof(Node));
  n -> data.type = PROG;
  n -> child = parse_instrcts(t, &current);
  return n;
}


Node* parse_instrcts(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  if (t[*index] -> type == RIGHTBRACKET)
  {
    Node* n = ncalloc(1, sizeof(Node));
    n -> data.type = INSTRCTS;
    return n;
  }

  Node* n = ncalloc(1, sizeof(Node));
  n -> data = *(t[*index]);
  n -> data.type = INSTRCTS;

  n -> child = parse_instrct(t, index);

  if (n -> child != NULL && !has_ended(n -> child))
  {
    n -> sibling = parse_instrcts(t, index);
  }

  return n;
}

bool has_ended(Node* head)
{
  if (head == NULL)
  {
    return false;
  }


  if (head -> data.type == END)
  {
    return true;
  }

  bool result1 = has_ended(head -> child);
  bool result2 = has_ended(head -> sibling);

  return (result1 || result2);

}

Node* parse_instrct(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  if (t[*index] -> type != LEFTBRACKET)
  {
    (*index)++;
    return error_node("No left bracket", t, index);
  }

  (*index)++;

  Node* n = parse_func(t, index);

  if (n == NULL)
  {
    (*index)++;
    return error_node("No function", t, index);
  }

  if (has_ended(n))
  {
    return n;
  }

  if (t[*index] -> type != RIGHTBRACKET)
  {
    return error_node("No right bracket found", t, index);
  }

  (*index)++;

  return n;
}

Node* parse_func(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  Node* n  = NULL;

  Node* (*functions[4])(Token**, int*) = {parse_retfunc, parse_iofunc, parse_if, parse_loop};

  for (int i = 0; i < 4; i++)
  {
    Node* temp = functions[i](t, index);
    if (temp != NULL)
    {
      n = temp;
      return n;
    }

  }
  return n;
}

Node* parse_listfunc(Token** t, int* index)
{
  Node* n;
  switch(t[*index] -> type)
  {
    case END:
    {
      Node* n = create_node(t, index);
      return n;
    }
    case CAR:     case CDR:
    {
      Node* n = create_node(t, index);
      (*index)++;
      n -> sibling = parse_list(t, index);
      return n;
    }
    case CONS:
      n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      n -> child = parse_list(t, index);

      if (n -> child != NULL && !has_ended(n -> child))
      {
        n -> sibling = parse_list(t, index);
      }
      return n;

    case UNKNOWN:
      return error_node("Unknown", t, index);
    default:
      break;
  }
  return NULL;
}

Node* parse_iofunc(Token** t, int* index)
{
  Node* n;
  switch(t[*index] -> type)
  {
    case END:
      return error_node("Ended", t, index);
    case SET:
      n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      n -> sibling = parse_var(t, index);
      if (n -> sibling != NULL && !has_ended(n -> sibling))
      {
        n -> sibling -> sibling = parse_list(t, index);
      }
      break;
    case PRINT:
      {
        Node* n = ncalloc(1, sizeof(Node));
        n -> data = *(t[*index]);
        (*index)++;
        n -> child = parse_print(t, index);
        return n;
      }
      break;
     case UNKNOWN:
      return error_node("Unknown", t, index);
    default:
      return NULL;
  }
  return n;
}

Node* parse_set(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      return n;
  }
  return NULL;
}

Node* parse_print(Token** t, int* index)
{
  switch(t[*index] -> type)
  {
    case END:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      return n;
    }

    case STRING: case VAR:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      return n;
    }

    case LEFTBRACKET:
    {
      (*index)++;
      Node* n = parse_retfunc(t, index);
      (*index)++;
      return n;
    }
    default:
      break;
  }

  return NULL;
}

Node* parse_retfunc(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  Node* n  = NULL;

  Node* (*functions[3])(Token**, int*) = {parse_listfunc, parse_intfunc, parse_boolfunc};

  for (int i = 0; i < 3; i++)
  {
    Node* temp = functions[i](t, index);
    if (temp != NULL)
    {
      n = temp;
      return n;
    }
  }
  return n;
}

Node* parse_intfunc(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  switch(t[*index] -> type)
  {
    case PLUS:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      n -> child = parse_list(t, index);
      if (n -> child != NULL && !has_ended(n -> child))
      {
        n -> sibling = parse_list(t, index);
      }
      return n;
    }
    case LENGTH:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      n -> child = parse_list(t, index);
      return n;
    }

    case UNKNOWN:
      return error_node("Unknown", t, index);
    default:
    {
      break;
    }
  }
  return NULL;
}

Node* parse_if(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  Node* n = NULL;

  if (t[*index] -> type == IF)
  {
    n = ncalloc(1, sizeof(Node));
    n -> data = *(t[*index]);
    (*index)++;

    if (t[*index] -> type != LEFTBRACKET)
    {
      return error_node("Missing left bracket 1", t, index);
    }

   (*index)++;
   n -> child = parse_boolfunc(t, index);
   if (n -> child == NULL || has_ended(n -> child))
   {
     return error_node("No bool func found", t, index);
   }


   if (t[*index] -> type != RIGHTBRACKET)
   {
     return error_node("Missing right bracket", t, index);
   }

   (*index)++;

   if (t[*index] -> type != LEFTBRACKET)
   {
     return error_node("Missing left bracket 2", t, index);
   }

   n -> sibling = ncalloc(1, sizeof(Node));
   n -> sibling -> data.type = IF_TRUE;

   (*index)++;

   n -> sibling -> child = parse_instrcts(t, index);


   if (n -> sibling -> child == NULL || has_ended(n -> sibling -> child))
   {
     return error_node("No instrct found", t, index);
   }

   n -> sibling -> sibling  = ncalloc(1, sizeof(Node));
   n -> sibling -> sibling -> data.type = IF_FALSE;

   (*index)++;

   if (t[*index] -> type != LEFTBRACKET)
   {
     (*index)++;
     n -> sibling -> sibling =  error_node("Missing left bracket 3", t, index);
     return n;
   }

   (*index)++;

   n -> sibling -> sibling -> child = parse_instrcts(t, index);

   (*index)++;
  }
  return n;
}

Node* parse_loop(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }


  if (t[*index] -> type != LOOP)
  {
    (*index)++;
    return NULL;
  }

  Node* n = ncalloc(1, sizeof(Node));
  n -> data = *(t[*index]);

  (*index)++;


  if (t[*index] -> type != LEFTBRACKET)
  {
      (*index)++;
    return error_node("No left bracket found", t, index);
  }

  (*index)++;

  n -> child = parse_boolfunc(t, index);

  if (n -> child == NULL || has_ended(n -> child))
  {
    return error_node("No bool func found", t, index);
  }

  if (t[*index] -> type != RIGHTBRACKET)
  {
    return error_node("No right bracket found", t, index);
  }

  (*index)++;

  if (t[*index] -> type != LEFTBRACKET)
  {
    return error_node("No left bracket found", t, index);
  }

  (*index)++;

  n -> sibling = parse_instrcts(t, index);

    (*index)++;
  return n;

}

Node* parse_boolfunc(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

   switch(t[*index] -> type)
  {
    case LESS: case GREATER: case EQUAL:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      n -> child = parse_list(t, index);
      if (n -> child != NULL || !has_ended(n -> child))
      {
        n -> sibling = parse_list(t, index);
      }
      return n;
    }
    default:
    {
      break;
    }
  }
  return NULL;
}

Node* parse_list(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  switch(t[*index] -> type)
  {
    case LITERAL: case NIL_TYPE: case VAR:
    {
      Node* n = ncalloc(1, sizeof(Node));
      n -> data = *(t[*index]);
      (*index)++;
      return n;
    }
    default:

    break;

  }

  if (t[*index] -> type != LEFTBRACKET)
  {
   (*index)++;
   return NULL;
  }

   (*index)++;
   Node* n = parse_retfunc(t, index);

   if (n != NULL && !has_ended(n))
   {
     if (t[*index] -> type != RIGHTBRACKET)
     {
       n = NULL;
     }
     (*index)++;

   }

  return n;
}

Node* parse_var(Token** t, int* index)
{
  if (t[*index] -> type == END)
  {
    return error_node("Ended", t, index);
  }

  if (t[*index] -> type != VAR)
  {
    return NULL;
  }

  Node* n = ncalloc(1, sizeof(Node));
  n -> data = *(t[*index]);
  (*index)++;
  return n;
}

Node* create_node(Token** t, int* index)
{
  Node* n = ncalloc(1, sizeof(Node));
  n -> data = *(t[*index]);
  return n;
}

void interpret(Node* head, Variables* v, Output* o)
{
  switch(head -> data.type)
  {
    case PROG:
      interpret_instrcts(head -> child, v, o);
      break;
    default:
      strcpy(o -> line[o -> index], "Could not find opening bracket");
      (o -> index)++;
  }
}

void interpret_instrcts(Node* head, Variables* v, Output* o)
{
  while (head != NULL && o -> stop != true && o -> index < MAX_LINES)
  {
    switch(head -> data.type)
    {
      case INSTRCTS:
        if (head -> child != NULL)
        {
          switch(head -> child -> data.type)
          {
            case SET:
              interpret_set(head -> child, v, o);
              break;
            case PRINT:
              interpret_print(head -> child, v, o);
              break;
            case IF:
              interpret_if(head -> child, v, o);
              break;
            case LOOP:
              interpret_loop(head -> child, v, o);
              break;
            case CAR: case CDR: case CONS:
            {
	      lisp* l = interpret_list(head -> child, v, o);
	      lisp_free(&l);
              break;
            }
            case ERROR:
            {
              strcpy(o -> line[o -> index], head -> child ->  data.value);
              o -> stop = true;
              (o -> index)++;
              break;
            }
            default:
              break;
          }
        }
        break;
      case ERROR:
      {
       strcpy(o -> line[o -> index], head -> data.value);
       o -> stop = true;
       (o -> index)++;
       break;
      }

      case END:
      {
       strcpy(o -> line[o -> index], head -> data.value);
       o -> stop = true;
      (o -> index)++;
       break;
      }

      default:
      {
       strcpy(o -> line[o -> index], "Could not find instruction");
       o -> stop = true;
      (o -> index)++;
      }
      break;
    }
    head = head -> sibling;
  }

}

void interpret_set(Node* head, Variables* v, Output* o)
{
  if (head -> sibling != NULL)
  {
    if(head -> sibling -> data.type == VAR)
    {
      if (head -> sibling -> sibling != NULL)
      {
        set_var(head -> sibling, v, o);
      }
    }
  }
}

void set_var(Node* head, Variables* v, Output* o)
{
  lisp* temp1 = interpret_list(head -> sibling, v, o);
  lisp* l = lisp_copy(temp1);
  lisp_free(&temp1);

  char temp_var = (head -> data.value)[0];

  if (v -> set_var[temp_var - 'A'] == true)
  {
    lisp* temp2 = v -> stored_vars[temp_var - 'A'];
    lisp_free(&temp2);
  }

  v -> stored_vars[temp_var - 'A'] = l;
  v -> set_var[temp_var - 'A'] = true;
}

void interpret_print(Node* head, Variables* v, Output* o)
{
  char temp_str[STR_LENGTH];
  switch(head -> child -> data.type)
  {
    case VAR:
    {
      char temp_var = (head -> child -> data.value)[0];
      if (v -> set_var[temp_var - 'A'] != true)
      {
        strcpy(o -> line[o -> index], "Variable not set before use");
        o -> stop = true;
        (o -> index)++;
        return;
      }

      lisp_tostring(v -> stored_vars[temp_var - 'A'], temp_str);
      strcpy(o -> line[o -> index], temp_str);
      (o -> index)++;
      break;
    }

    case CAR: case CDR: case CONS: case PLUS: case LESS: case LENGTH:
    {
      lisp* l = interpret_list(head -> child, v, o);
      lisp_tostring(l, temp_str);
      lisp_free(&l);
      strcpy(o -> line[o -> index], temp_str);
      (o -> index)++;
      break;
    }

    case STRING:
    {
     strcpy(o -> line[o -> index], head -> child -> data.value);
     (o -> index)++;
     break;
    }

    case ERROR:
    {
      strcpy(o -> line[o -> index], head -> child -> data.value);
      o -> stop = true;
      return;
    }

    default:
    {
      strcpy(o -> line[o -> index], "Nothing to be printed");
    }
  }
}

lisp* interpret_car(Node* head, Variables* v, Output* o)
{
  lisp* temp1 = interpret_list(head -> sibling, v, o);
  lisp* temp2 = lisp_car(temp1);
  lisp* l = lisp_copy(temp2);
  lisp_free(&temp1);
  return l;
}

lisp* interpret_cdr(Node* head, Variables* v, Output* o)
{
  lisp* temp1 = interpret_list(head -> sibling, v, o);
  lisp* temp2 = lisp_cdr(temp1);
  lisp* l = lisp_copy(temp2);
  lisp_free(&temp1);
  return l;
}

lisp* interpret_cons(Node* head, Variables* v, Output* o)
{
  lisp* component_1 = interpret_list(head -> child, v, o);
  lisp* component_2 = interpret_list(head -> sibling, v, o);

  lisp* temp = lisp_cons(component_1, component_2);
  lisp* l = lisp_copy(temp);

  lisp_free(&temp);

  return l;
}

bool lists_same(lisp* a, lisp* b)
{
  if (a == NULL && b == NULL)
  {
    return true;
  }

  if (a == NULL || b == NULL)
  {
    return false;
  }

  if (lisp_isatomic(a) && lisp_isatomic(b))
  {
    if (lisp_getval(a) == lisp_getval(b))
    {
      return true;
    }
    return false;
  }

  bool result1 = lists_same(a -> car, b -> car);
  bool result2 = lists_same(a -> cdr, b -> cdr);

  return (result1 && result2);
}

bool list_less(lisp* a, lisp* b)
{
  if (a == NULL && b == NULL)
  {
    return false;
  }

  if (a == NULL)
  {
    return true;
  }

  if (b == NULL)
  {
    return false;
  }

  if (lisp_isatomic(a) && lisp_isatomic(b))
  {
    if (lisp_getval(a) < lisp_getval(b))
    {
      return true;
    }
    return false;
  }

  bool result1 = list_less(a -> car, b -> car);
  bool result2 = list_less(a -> cdr, b -> cdr);

  return (result1 && result2);
}

bool list_greater(lisp* a, lisp* b)
{
  if (a == NULL && b == NULL)
  {
    return false;
  }

  if (a == NULL)
  {
    return false;
  }

  if (b == NULL)
  {
    return true;
  }

  if (lisp_isatomic(a) && lisp_isatomic(b))
  {
    if (lisp_getval(a) > lisp_getval(b))
    {
      return true;
    }
    return false;
  }

  bool result1 = list_greater(a -> car, b -> car);
  bool result2 = list_greater(a -> cdr, b -> cdr);

  return (result1 && result2);
}

lisp* interpret_list(Node* head, Variables* v, Output* o)
{
  switch (head -> data.type)
  {
    case CAR:
    {
      lisp* temp1 = interpret_car(head, v, o);
      lisp* l = lisp_copy(temp1);
      lisp_free(&temp1);
      return l;
    }

    case CDR:
    {
      lisp* temp1 = interpret_cdr(head, v, o);
      lisp* l = lisp_copy(temp1);
      lisp_free(&temp1);
      return l;
    }

    case CONS:
    {
      lisp* temp1 = interpret_cons(head, v, o);
      lisp* l = lisp_copy(temp1);
      lisp_free(&temp1);
      return l;
    }

    case VAR:
    {
      char temp_var = (head -> data.value)[0];
      lisp* l = lisp_copy(v -> stored_vars[temp_var - 'A']);
      return l;
    }

    case LITERAL:
    {
      if (!is_valid_string(head -> data.value))
      {
        strcpy(o -> line [o -> index], "Interpreter encountered invalid literal");
        o -> stop = true;
        (o -> index)++;
        return NULL;
      }
      lisp* l = lisp_fromstring(head -> data.value);
      return l;
    }

    case NIL_TYPE:
    {
      return NULL;
    }

    case PLUS:
    {
      lisp* temp1 = interpret_plus(head, v, o);
      lisp* l = lisp_copy(temp1);
      lisp_free(&temp1);
      return l;
    }

    case LENGTH:
    {
      lisp* temp1 = interpret_length(head, v, o);
      lisp* l = lisp_copy(temp1);
      lisp_free(&temp1);
      return l;
    }

    case EQUAL:
    {
      bool result = interpret_equal(head, v, o);
      char* val = result ? "1": "0";
      lisp* l = lisp_fromstring(val);
      return l;
    }

    case LESS:
    {
      bool result = interpret_less(head, v, o);
      char* val = result ? "1": "0";
      lisp* l = lisp_fromstring(val);
      return l;
    }

    case GREATER:
    {
      bool result = interpret_greater(head, v, o);
      char* val = result ? "1": "0";
      lisp* l = lisp_fromstring(val);
      return l;
    }

    case ERROR: case END:
    {
      o -> stop = true;
      strcpy(o -> line [o -> index], head -> data.value);
      (o -> index)++;
      return NULL;
    }
    default:
      return NULL;
  }
  return NULL;
}


lisp* interpret_plus(Node* head, Variables* v, Output* o)
{
  lisp* l;

  lisp* l1 = interpret_list(head -> child, v, o);
  lisp* l2 = interpret_list(head -> sibling, v, o);

  //Only supports adding atoms to atoms
  if (!lisp_isatomic(l1) || !lisp_isatomic(l2))
  {
    strcpy(o -> line[o -> index], "Cannot add two non-atoms together");
    o -> stop = true;
    (o -> index)++;
    return NULL;
  }

  int val1 = lisp_getval(l1);
  int val2 = lisp_getval(l2);


  char temp_str[STR_LENGTH];
  sprintf(temp_str, "%d", val1 + val2);

  l = lisp_fromstring(temp_str);

  lisp_free(&l1);
  lisp_free(&l2);

  return l;
}

lisp* interpret_length(Node* head, Variables* v, Output* o)
{
  lisp* temp = interpret_list(head -> child, v, o);
  int val = lisp_length(temp);
  char temp_str[STR_LENGTH];
  sprintf(temp_str, "%d", val);

  lisp* l = lisp_fromstring(temp_str);
  lisp_free(&temp);
  return l;
}

bool interpret_equal(Node* head, Variables* v, Output* o)
{
  lisp* l1 = interpret_list(head -> child, v, o);
  lisp* l2 = interpret_list(head -> sibling, v, o);
  bool result = lists_same(l1, l2);
  lisp_free(&l1);
  lisp_free(&l2);
  return result;
}

bool interpret_less(Node* head, Variables* v, Output* o)
{
  lisp* l1 = interpret_list(head -> child, v, o);
  lisp* l2 = interpret_list(head -> sibling, v, o);
  bool result = list_less(l1, l2);
  lisp_free(&l1);
  lisp_free(&l2);
  return result;
}

bool interpret_greater(Node* head, Variables* v, Output* o)
{
  lisp* l1 = interpret_list(head -> child, v, o);
  lisp* l2 = interpret_list(head -> sibling, v, o);
  bool result = list_greater(l1, l2);
  lisp_free(&l1);
  lisp_free(&l2);
  return result;
}

void interpret_if(Node* head, Variables* v, Output* o)
{
  bool result;
  switch(head -> child -> data.type)
  {
    case EQUAL:
    {
      result = interpret_equal(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling -> child, v, o);
        break;
      }

      interpret_instrcts(head -> sibling -> sibling -> child, v, o);
      break;
    }
    case LESS:
    {
      result = interpret_less(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling -> child, v, o);
        break;
      }

      interpret_instrcts(head -> sibling -> sibling -> child, v, o);
      break;
    }

    case GREATER:
    {
      result = interpret_greater(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling -> child, v, o);
        break;
      }

      interpret_instrcts(head -> sibling -> sibling -> child, v, o);
      break;
    }
    case ERROR:
    {
      strcpy(o -> line[o -> index], head -> child ->  data.value);
      o -> stop = true;
      (o -> index)++;
      return;
    }

    default:
    {
      strcpy(o -> line[o -> index], "Semantically meaningless input");
      o -> stop = true;
      (o -> index)++;
    }
  }
}

void interpret_loop(Node* head, Variables* v, Output* o)
{
  bool result = true;

  while (result && o -> index < MAX_LINES)
  {
  switch(head -> child -> data.type)
  {
    case EQUAL:
    {
      result = interpret_equal(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling , v, o);
        break;
      }

      break;
    }
    case LESS:
    {
      result = interpret_less(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling, v, o);
        break;
      }

      break;
    }

    case GREATER:
    {
      result = interpret_greater(head -> child, v, o);
      if (result)
      {
        interpret_instrcts(head -> sibling, v, o);
        break;
      }

      break;
    }
    case ERROR:
    {
      strcpy(o -> line[o -> index], head -> child ->  data.value);
      o -> stop = true;
      (o -> index)++;
      return;
    }

    default:
    {
      strcpy(o -> line[o -> index], "Semantically meaningless input");
      o -> stop = true;
      (o -> index)++;
    }
  }


  }
}

char* get_name(Token* t)
{
  SyntaxType tt = t -> type;
  switch(tt)
  {
    case LEFTBRACKET:
      return "(";

    case RIGHTBRACKET:
      return ")";

    case CAR:
      return "CAR";

    case CDR:
      return "CDR";

    case CONS:
      return "CONS";

    case PLUS:
      return "PLUS";

    case LENGTH:
      return "LENGTH";

    case LESS:
      return "LESS";

    case GREATER:
      return "GREATER";

    case EQUAL:
      return "EQUAL";

    case SET:
      return "SET";

    case PRINT:
      return "PRINT";

    case IF:
      return "IF";

    case LOOP:
      return "WHILE";

    case NIL_TYPE:
      return "NIL";

    case VAR:
      return t -> value;

    case STRING:
    {
      return t -> value;
    }
    case LITERAL:
      return t -> value;

    case UNKNOWN:
      return t -> value;

    case PROG:
      return "PROG";

    case INSTRCTS:
      return "INSTRCTS";

    case ERROR:
      return t -> value;

    case IF_TRUE:
      return "IF-TRUE";

    case IF_FALSE:
      return "IF-FALSE";

    default:
      return "END OF FILE";
  }

  return "END OF FILE";
}

void tokenise(Token** t, Program* p)
{
  int token_index = 0;

  while (p -> index < (int)strlen(p -> input))
  {
    switch(p -> input[p -> index])
    {
      case '#':
        process_comment(p);
        break;
      case '\n':
        (p -> index)++;
        break;
      case ' ':
        (p -> index)++;
        break;
      case '(':
        t[token_index] -> type = LEFTBRACKET;
        token_index++;
        (p -> index)++;
        break;
      case ')':
        t[token_index] -> type = RIGHTBRACKET;
        token_index++;
        (p -> index)++;
        break;

      case 'A': case 'B': case 'C': case 'D': case 'E':
      case 'F': case 'G': case 'H': case 'I': case 'J':
      case 'K': case 'L': case 'M': case 'N': case 'O':
      case 'P': case 'Q': case 'R': case 'S': case 'T':
      case 'U': case 'V': case 'W': case 'X': case 'Y':
      case 'Z':
        if (isalpha(peek(p)))
        {
          process_word(t, p, token_index);

        }
        else
        {
          process_char(t, p, token_index);
        }
        token_index++;
        break;

      case '\'':
        process_type(t, p, token_index, LITERAL);
        token_index++;
        break;

      case '"':
        process_type(t, p, token_index, STRING);
        token_index++;
        break;

      default:
        t[token_index] -> type = UNKNOWN;
        t[token_index] -> value = ncalloc(2, sizeof(char));
        sprintf(t[token_index] -> value, "%c", p -> input[p -> index]);
        (p -> index)++;
        token_index++;
    }
  }

  for (int i = token_index; i < MAX_TOKENS; i++)
  {
    t[i] -> type = END;
  }
}

void process_word(Token** t, Program* p, int index)
{
  char* words[NUM_OF_KEYWORDS] = {"CAR", "CDR", "CONS",
  "PLUS", "LENGTH", "LESS", "GREATER", "EQUAL", "SET", "PRINT",
   "IF", "WHILE", "NIL"};

  SyntaxType word_types[NUM_OF_KEYWORDS] = {CAR, CDR, CONS,
  PLUS, LENGTH, LESS, GREATER, EQUAL, SET, PRINT, IF, LOOP, NIL_TYPE};

  for (int i = 0; i < NUM_OF_KEYWORDS; i++)
  {
    if (strstr((p -> input) + p -> index, words[i]) == (p -> input) + p -> index)
    {
      t[index] -> type = word_types[i];
      (p -> index) += (int)strlen(words[i]);
      return;
    }

  }

  int start = p -> index;

  //Process whole word
  while isalpha(p -> input[p -> index])
  {
    (p -> index)++;
  }

  t[index] -> type = UNKNOWN;
  t[index] -> value = ncalloc((p -> index) - start + 1, sizeof(char));
  strncpy(t[index] -> value, (p -> input) + start, p -> index - start);

}

void process_char(Token** t, Program* p, int index)
{
  t[index] -> type = VAR;
  t[index] -> value = ncalloc(2, sizeof(char));
  sprintf(t[index] -> value, "%c", p -> input[p -> index]);
  (p -> index)++;
}

void process_type(Token** t, Program* p, int token_index, SyntaxType tt)
{
  int start = (p -> index);
  char delimit;

  switch(tt)
  {
    case STRING:
      delimit = '"';
      break;
    case LITERAL:
     delimit = '\'';
     break;
    default:
      return;
  }

  while (p -> index < (int)strlen(p -> input))
  {
    (p -> index)++;
    if (p -> input[p -> index] == delimit)
    {
      t[token_index] -> type = tt;
      t[token_index] -> value = ncalloc((p -> index) - start, sizeof(char));
      //To remove '' or "" from saved output
      strncpy(t[token_index] -> value, (p -> input) + start + 1, p -> index - start - 1);
      (p -> index)++;
      return;
    }
  }

  t[token_index] -> type = UNKNOWN;

}

void process_comment(Program* p)
{
  while (p -> input[p -> index] != '\n')
  {
    (p -> index)++;
  }
}

char peek(Program* p)
{
  if ((p -> index + 1) >= (int)strlen(p -> input))
  {
    return '\0';
  }

  return (p -> input[(p -> index) + 1]);
}
void free_nodes(Node* n)
{
  if (n == NULL)
  {
    return;
  }

  free_nodes(n -> child);
  free_nodes(n -> sibling);

  switch(n -> data.type)
  {
    case STRING: case LITERAL: case ERROR: case VAR:
    {
      free(n -> data.value);
    }
    default:
    {

    }
  }

  free(n);

}

void free_tokens(Token** t)
{
  for (int i = 0; i < MAX_TOKENS; i++)
  {
    free(t[i]);
  }
  free(t);
}

void free_variables(Variables* v)
{
  for (int i = 0; i < MAX_VARIABLES; i++)
  {
    if (v -> stored_vars[i] != NULL)
    {
      lisp_free(&(v -> stored_vars[i]));
    }
  }

  free(v);
}

void free_output(Output* o)
{
  for (int i = 0; i < MAX_LINES; i++)
  {
    free(o -> line[i]) ;
  }
  free(o -> line);

  free(o);
}

void free_prog(Program* p)
{
  free(p -> input);
  free(p);
}
