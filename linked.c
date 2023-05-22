#include "specific.h"

void test(void)
{
   char str[MAX_CHARS];
   char str2[MAX_CHARS];
   int i = 0;

   assert(contains_number('2'));
   assert(contains_number('0'));
   assert(contains_number('-'));
   assert(!contains_number('X'));
   assert(!contains_number(LEFT_BRACKET));
   assert(!contains_number(RIGHT_BRACKET));

   write_num(str, 1, &i);
   assert(str[0] == '1');

   i = 0;
   write_num(str, 20, &i);
   assert(str[0] == '2');
   assert(str[1] == '0');

   i = 0;
   write_num(str, -15, &i);
   assert(str[0] == '-');
   assert(str[1] == '1');
   assert(str[2] == '5');

   lisp_tostring(NIL, str);
   assert(lisp_length(NIL)==0);
   assert(strcmp(str, "()")==0);

   lisp* l1 = lisp_atom(3);
   assert(lisp_isatomic(l1));

   list_info li_1;
   
   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l1, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "3") == 0);

   lisp_tostring(l1, str);
   assert(strcmp(str, "3")==0);

   lisp* l2 = lisp_atom(-21);

   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l2, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "-21") == 0);

   lisp_tostring(l2, str);
   assert(strcmp(str, "-21") == 0);
   assert(lisp_length(l2) == 0);
   assert(lisp_car(l2) == NIL);
   assert(lisp_cdr(l2) == NIL);
   assert(lisp_isatomic(l2));

   lisp* l3 = lisp_cons(l1, NIL);

   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l3, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "(3)") == 0);

   lisp_tostring(l3, str);
   assert(lisp_length(l3) == 1);
   assert(!lisp_isatomic(l3));
   assert(strcmp(str, "(3)")==0);
   
   lisp* l4 = NIL;
   assert(!lisp_isatomic(l4));
   lisp_free(&l4);
   assert(l4 == NIL);

   lisp* l5 = lisp_cons(lisp_atom(10), lisp_cons(lisp_atom(-20),
   lisp_cons(lisp_atom(7), NIL)));

   assert(l5);
   assert(lisp_length(l5)==3);

   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l5, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "(10 -20 7)") == 0);

   lisp_tostring(l5, str);
   assert(strcmp(str, "(10 -20 7)") == 0);
   
   //No need to free l5_car separately to l5 as no new pointer created
   lisp* l5_car = lisp_car(l5);
   lisp_tostring(l5_car, str);
   assert(strcmp(str, "10") == 0);

   lisp* l6 = lisp_cons(lisp_cons(lisp_atom(-3033333), NIL), l5);
   assert(l6);

   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l6, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "((-3033333) 10 -20 7)") == 0);
   lisp_tostring(l6, str);
   assert(strcmp(str, "((-3033333) 10 -20 7)")==0);
   
   lisp* l6_car = lisp_car(l6);
   lisp* l6_cdr = lisp_cdr(l6);
   
   lisp_tostring(l6_car, str);
   assert(strcmp(str, "(-3033333)") == 0);
 
   lisp_tostring(l6_cdr, str);
   assert(strcmp(str, "(10 -20 7)") == 0);
   
   lisp* l7 = lisp_cons(lisp_cons(lisp_atom(1392), NIL), l6);
   li_1.node_to_left = false;
   li_1.index = 0;
   traverse_lisp(l7, &li_1);
   li_1.list[li_1.index] = '\0';

   assert(strcmp(li_1.list, "((1392) (-3033333) 10 -20 7)") == 0);

   lisp_tostring(l7, str);
   assert(strcmp(str, "((1392) (-3033333) 10 -20 7)")==0);

   //Testing lisp_copy - free earlier lisp struct
   //to ensure that deep copy has been made
   lisp* l8 = lisp_copy(l7);

   //Lisp structures l5 and l6 have been freed too
   lisp_free(&l7);

   lisp_tostring(l8, str);
   assert(strcmp(str, "((1392) (-3033333) 10 -20 7)")==0);

   //Freeing all lists, checking that free function works
   lisp_free(&l2);
   lisp_free(&l3);
   lisp_free(&l8);

   //Freed lisp structures return NIL
   assert(l2 == NIL);
   assert(l3 == NIL);
   assert(l8 == NIL);
   
   //At index 2 for string "1 23" and at index 1 for string "1 "
   //New atom is to be added to string 1 at this stage
   assert(new_atom("1 23", "1 ", 1, 2));
   
   //At index 3 for string "1 23", and at index 2 for string "1 2 "
   //As 3 is part of same atom as 2, no new atom found
   assert(!new_atom("1 23", "1 2 ", 2, 3));
   
   
   //String where no spaces should be added
   strcpy(str, "(3)");
   assert(!should_add_space(str, 0));
   assert(!should_add_space(str, 1));
   assert(!should_add_space(str, 2));
   
   //Space should be added before the second left bracket
   strcpy(str, "(1(2))");
   assert(!should_add_space(str, 0));
   assert(should_add_space(str, 2));
   
   //No spaces added past length of string
   assert(!should_add_space(str, 15));
   
   //Spaces to be added before 3 and after 3, but nowhere else
   strcpy(str, "((1)3(2))");
   assert(!should_add_space(str, 0));
   assert(!should_add_space(str, 1));
   assert(!should_add_space(str, 2));
   assert(!should_add_space(str, 3));
   assert(should_add_space(str, 4));
   assert(should_add_space(str, 5));
   assert(!should_add_space(str, 6));
   assert(!should_add_space(str, 7));
 
   strip_string("    1   ", str);
   assert(strcmp(str, "1") == 0);
   
   strip_string(" (   1)", str);
   assert(strcmp(str, "(1)") == 0);
   
   strip_string(" (   1)      ", str);
   assert(strcmp(str, "(1)") == 0);
   
   strip_string(" (   1)    )  ", str);
   assert(strcmp(str, "(1))") == 0);
   
   strip_string(" (   1        2   )  ", str);
   assert(strcmp(str, "(1 2)") == 0);
   
   
   strip_string("((-1 2) (3 4) (5 (6 7)))", str);
   
   assert(strcmp(str, "((-1 2)(3 4)(5(6 7)))") == 0);
   clean_string(str, str2);
   assert(strcmp(str2, "((-1 2) (3 4) (5 (6 7)))") == 0);
   strip_string("((1    2)   3)   ", str);
   assert(strcmp(str, "((1 2)3)") == 0);
   clean_string(str, str2);
   assert(strcmp(str2, "((1 2) 3)") == 0);
 
   strip_string("((3)(((-132434 1)))15 20 323 -12 ((((1)(2)(3)(4 3 2 2 11)))   )   )", str);
   assert(strcmp(str, "((3)(((-132434 1)))15 20 323 -12((((1)(2)(3)(4 3 2 2 11)))))") == 0);
   clean_string(str, str2);
   assert(strcmp(str2, "((3) (((-132434 1))) 15 20 323 -12 ((((1) (2) (3) (4 3 2 2 11)))))") == 0);
   
   lisp* l9 = lisp_fromstring("()");
   lisp_tostring(l9, str);
   assert(strcmp(str, "()") == 0);

   //i represents number of chars processed, which is used
   //to keep track of where I am along string
   i = 0;
   lisp* l10 = ncalloc(1, sizeof(lisp));
   process_number(l10, "1", &i);
   assert(l10 -> info == 1);
   assert(i == 1);

   i = 0;
   process_number(l10, "1000", &i);
   assert(l10 -> info == 1000);
   assert(i == 4);

   i = 0;
   process_number(l10, "-14", &i);
   assert(l10 -> info == -14);
   assert(i == 3);
   
   i = 0;
   lisp* l11 = build_lisp("(1 2 3)", &i);
   lisp_tostring(l11, str);
   assert(strcmp(str, "(1 2 3)") == 0);

   i = 0;
   lisp* l12 = build_lisp("(1 (2 3))", &i);
   lisp_tostring(l12, str);
   assert(strcmp(str, "(1 (2 3))") == 0);

   lisp* l13 = lisp_fromstring("((5))");
   lisp_tostring(l13, str);
   assert(strcmp(str, "((5))") == 0);
   
   lisp* l14 = lisp_fromstring("((3)(  ((-132434 1)))15 20 323 -12 ( (( (1)(2)(3)(4 3 2 2 11)) ))  )");
   lisp_tostring(l14, str);
   assert(strcmp(str, "((3) (((-132434 1))) 15 20 323 -12 ((((1) (2) (3) (4 3 2 2 11)))))") == 0);
  
   assert(is_valid_char(LEFT_BRACKET));
   assert(is_valid_char(RIGHT_BRACKET));
   assert(is_valid_char(SPACE));
   assert(!is_valid_char('K'));
   
   assert(is_valid_string("1"));
   assert(is_valid_string("11"));
   assert(is_valid_string("-32221"));
   assert(!is_valid_string("-32221-"));
   assert(!is_valid_string("-311-12"));
   assert(!is_valid_string("(1"));
   assert(!is_valid_string("1)"));
   assert(is_valid_string("(123 45 (67) (-8) -9)"));
   
   //Invalid string returns NIL lisp
   lisp* l15 = lisp_fromstring("-3221-");
   assert(l15 == NIL);
   
   lisp_free(&l9);
   lisp_free(&l10);
   lisp_free(&l11);
   lisp_free(&l12);
   lisp_free(&l13);
   lisp_free(&l14);
   
    /*--------------------*/
   /* lisp_list() tests  */
   /*--------------------*/
   
   lisp* m1 = lisp_list(0);
   assert(m1 == NIL);
   
   lisp* m2 = lisp_list(1, lisp_atom(5));
   assert(lisp_getval(m2 -> car) == 5);
   
   lisp* m3 = lisp_list(3, lisp_fromstring("(1 2)"), lisp_fromstring("((-1000)(89))"), lisp_atom(-124));
   lisp_tostring(m3, str);
   assert(strcmp(str, "((1 2) ((-1000) (89)) -124)") == 0);
   
   lisp* m4 = lisp_list(4, m2, lisp_fromstring("(1 2   ) "), lisp_fromstring(" (1  )  "), lisp_fromstring("((((5))))"));
   lisp_tostring(m4, str);
   assert(strcmp(str, "((5) (1 2) (1) ((((5)))))") == 0);
   
   lisp_free(&m3);
   lisp_free(&m4);
   assert(m3 == NIL);
   assert(m4 == NIL);
  
    /*------------------------*/
   /* lisp_reduce() tests -  */
   /*------------------------*/
   
   atomtype accumulator = 0;
   
   lisp_reduce(atms, NIL, &accumulator);
   assert(accumulator == 0);
   
   lisp_reduce(times, NIL, &accumulator);
   assert(accumulator == 0);
   
   lisp* n1 = lisp_fromstring("(1 7 9 20 2 3 5)");
   assert(lisp_length(n1) == 7);
   lisp_reduce(atms, n1, &accumulator);
   assert(accumulator == 7);
   
   accumulator = 1;
   lisp_reduce(times, n1, &accumulator);
   assert(accumulator == 37800);
   
   accumulator = 0;
   
   lisp* n2 = lisp_fromstring("((1 2)(2 3)(3))");
   assert(lisp_length(n2) == 3);
   lisp_reduce(atms, n2, &accumulator);
   assert(accumulator == 5);
   
   accumulator = 1;
   lisp_reduce(times, n2, &accumulator);
   assert(accumulator == 36);
   
   lisp_free(&n1);
   lisp_free(&n2);
}

lisp* lisp_atom(const atomtype a)
{
  lisp* l = ncalloc(1, sizeof(lisp));
  l -> info = a;
  return l;
}

lisp* lisp_cons(const lisp* l1,  const lisp* l2)
{
  lisp* l = ncalloc(1, sizeof(lisp));
  l -> car = (lisp*)l1;
  l -> cdr = (lisp*)l2;
  return l;
}

int lisp_length(const lisp* l)
{
  if (l == NIL)
  {
    return 0;
  }

  if (lisp_isatomic(l))
  {
    return 0;
  }

  return 1 + lisp_length(l -> cdr);
}

void lisp_tostring(const lisp* l, char* str)
{
  if (l == NIL)
  {
    strcpy(str, "()");
    return;
  }

  list_info li;
  li.node_to_left = false;
  li.index = 0;

  traverse_lisp(l, &li);
  li.list[li.index] = '\0';

  //Testing return value in assert tests
  strcpy(str, li.list);
}

void traverse_lisp(const lisp* l, list_info* li)
{
  if (lisp_isatomic(l))
  {
    write_num(li -> list, l -> info, &(li -> index));
    return;
  }
  
  if (!li -> node_to_left)
  {
    li -> list[li -> index] = LEFT_BRACKET;
  }
  else
  {
    if (l == NIL)
    {
      li -> list[li -> index] = RIGHT_BRACKET;
      (li -> index)++;
      return;
    }
    li -> list[li -> index] = SPACE;
  }
  
  (li -> index)++;
  
  li -> node_to_left = false;
  traverse_lisp(l -> car, li);
  
  li -> node_to_left = true;
  traverse_lisp(l -> cdr, li);
}

atomtype lisp_getval(const lisp* l)
{
  if (l == NIL)
  {
    return 0;
  }
  atomtype a = l -> info;
  return a;
}

lisp* lisp_car(const lisp* l)
{
  if (l == NIL)
  {
    return NIL;
  }
  return l -> car;
}

lisp* lisp_cdr(const lisp* l)
{
  if (l == NIL)
  {
    return NIL;
  }
  return l -> cdr;
}

lisp* lisp_copy(const lisp* l)
{
  if (l == NIL)
  {
    return NIL;
  }

  lisp* l1 = ncalloc(1, sizeof(lisp));
  l1 -> info = l -> info;
  l1 -> car = lisp_copy(lisp_car(l));
  l1 -> cdr = lisp_copy(lisp_cdr(l));

  return l1;
}

void lisp_free(lisp** l)
{
  if (*l == NIL)
  {
    return;
  }

  lisp* l1 = (*l) -> car;
  lisp* l2 = (*l) -> cdr;

  if (l1 != NIL)
  {
    lisp_free(&l1);
  }

  if (l2 != NIL)
  {
    lisp_free(&l2);
  }

  free(*l);
  *l = NULL;
}

lisp* lisp_fromstring(const char* str)
{
  if (strcmp(str, "()") == 0)
  {
    return NIL;
  }

  bool valid = is_valid_string(str);
  if (!valid)
  {
    return NIL;
  }

  int index = 0;
  char temp_arr[MAX_CHARS];
  clean_string(str, temp_arr);
  lisp* l = build_lisp(temp_arr, &index);
  return l;
}

lisp* build_lisp(char* str, int* index)
{
  if (*index >= (int)strlen(str))
  {
    return NIL;
  }

  if (str[*index] == RIGHT_BRACKET)
  {
    (*index)++;
    return NIL;
  }

  lisp* l = ncalloc(1, sizeof(lisp));

  if (contains_number(str[*index]))
  {
    process_number(l, str, index);
    return l;
  }

  (*index)++;

  l -> car = build_lisp(str, index);
  l -> cdr = build_lisp(str, index);

  return l;
}

void clean_string(const char* str, char new_str[])
{
  char temp_str[MAX_CHARS];
  strip_string(str, temp_str);
  int i = 0;
  int j = 0;

  while (temp_str[j] != '\0')
  {
    if (should_add_space(temp_str, j))
    {
      new_str[i] = SPACE;
      new_str[i + 1] = temp_str[j];
      i+=2;
    }
    else
    {
      new_str[i] = temp_str[j];
      i++;
    }
    j++;
  }
  new_str[i] = '\0';
}

bool should_add_space(char* str, int index)
{
  if (index < 1 || index >= (int)strlen(str))
  {
    return false;
  }

  if (str[index] == LEFT_BRACKET)
  {
    if (contains_number(str[index - 1]))
    {
      return true;
    }
    
    if (str[index - 1] == RIGHT_BRACKET)
    {
      return true;
    }
  }
  
  if (contains_number(str[index]))
  {
    if (str[index - 1] == RIGHT_BRACKET)
    {
      return true;
    }
  }  
  return false;
}

void strip_string(const char* str, char new_str[])
{
  int i = 0;
  int j = 0;
  
  while (str[j] != '\0')
  {
    if (str[j] == SPACE)
    {
      j++;
    }
    
    else
    {
      /*If program is about to add an atom from the old string 
      next to an existing atom in the new string, space needs to be added
      in the new string before adding the new atom */
      if (new_atom(str, new_str, i, j))
      {
        new_str[i] = SPACE;
        new_str[i + 1] = str[j];
        i+=2;
      }
      
      else
      {
        new_str[i] = str[j];
        i++;
      }
      j++;
    }
  }
  new_str[i] = '\0';
  
}

bool new_atom(const char* str, char* new_str, int i, int j)
{
  if (i <= 0 || j <= 0)
  {
    return false;
  }
  
  if ((contains_number(new_str[i - 1])) && contains_number(str[j]))
  {
    if (new_str[i - 1] != str[j - 1])
    {
      return true;
    }
  }
  return false;
}

bool lisp_isatomic(const lisp* l)
{
  if (l == NIL)
  {
    return false;
  }

  if ((lisp_car(l) == NIL) && (lisp_cdr(l) == NIL))
  {
    return true;
  }

  return false;
}

bool contains_number(char c)
{
  if (isdigit(c) != 0)
  {
    return true;
  }
  //Means negative number ahead in string
  if (c == '-')
  {
    return true;
  }

  return false;
}

void process_number(lisp* l, const char* str, int* i)
{
  int j = *i;

  while (contains_number(str[j]))
  {
    j++;
  }

  long result = strtol(&(str[*i]), NULL, BASE);
  l -> info = (int) result;
  *i = j;
}

void write_num(char c[], int num, int* index)
{
  char temp[MAX_CHARS];
  int length = snprintf(temp, MAX_CHARS, "%d", num);

  if (length > MAX_CHARS || length < 0)
  {
    fprintf(stderr, "Input number greater than %i digits.", MAX_CHARS);
    fprintf(stderr, "Cannot process number %i\n", num);
    return;
  }

  int j = 0;

  while (temp[j] != '\0')
  {
    c[*index] = temp[j];
    (*index)++;
    j++;
  }
}

lisp* lisp_list(const int n, ...)
{
  if (n == 0)
  {
    return NIL;
  }
  
  va_list ap;
  va_start(ap, n);

  lisp* l = ncalloc(1, sizeof(lisp));
  lisp* tail = l;

  for (int i = 0; i < n; i++)
  {
    tail -> car = va_arg(ap, lisp*);
    if (i < n - 1)
    {
      tail -> cdr = ncalloc(1, sizeof(lisp));
      tail = tail -> cdr;
    }
  }

  va_end(ap);
  return l;
}

void lisp_reduce(void (*func)(lisp* l, atomtype* n), lisp* l, atomtype* acc)
{
  if (l == NIL)
  {
    return;
  }

  if (lisp_isatomic(l))
  {
    (*func)(l, acc);
    return;
  }

  lisp_reduce((*func), lisp_car(l), acc);
  lisp_reduce((*func), lisp_cdr(l), acc);
}

bool is_valid_string(const char* str)
{
  int left_brackets = 0, right_brackets = 0;
  int i = 0;
  bool valid = true;

  while (str[i] != '\0' && valid == true)
  {
    if (!is_valid_char(str[i]))
    {
      //fprintf(stderr, "Input string '%s' has invalid characters\n", str);
      valid = false;
    }

    if (str[i] == LEFT_BRACKET)
    {
      left_brackets++;
    }

    if (str[i] == RIGHT_BRACKET)
    {
      right_brackets++;
    }
    
    if (i > 0 && str[i] == '-' && contains_number(str[i - 1]))
    {
      valid = false;
    }

    i++;
  }

  if (left_brackets != right_brackets)
  {
    //fprintf(stderr, "Number of left and right brackets are not equal");
    valid = false;
  }
  return valid;
}

bool is_valid_char(char c)
{
  if (contains_number(c) || c == SPACE)
  {
    return true;
  }

  if (c == LEFT_BRACKET ||  c == RIGHT_BRACKET)
  {
    return true;
  }

  return false;
}

// Multiplies getval() of all atoms
void times(lisp* l, atomtype* accum)
{
   *accum = *accum * lisp_getval(l);
}

// To count number of atoms in list, including sub-lists
void atms(lisp* l, atomtype* accum)
{
   // Could just add one (since each node must be atomic),
   // but prevents unused warning for variable 'l'...
   *accum = *accum + lisp_isatomic(l);
}

