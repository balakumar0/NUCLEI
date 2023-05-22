
#include "nuclei.h"

int main(int argc, char* argv[])
{
  check_args(argc, argv);
  FILE* f = open_file(argv[1]);

  #ifdef INTERP

  test_interpreter();

  #else

  test_parser();

  #endif

  Program* p = Initialise(f);
  Token** t = init_tokens();
  tokenise(t, p);

  ProgramInfo* pi = ncalloc(1, sizeof(ProgramInfo));
  pi -> valid = true;
  pi -> tokens = t;

  Variables* v = ncalloc(1, sizeof(Variables));
  //Set LispStack top
  v -> s.top = -1;

  Output* o = init_output();

  Prog(pi, v, o);

  #ifdef INTERP

  #else
  if (pi -> valid)
  {
   add_output(o, "Parsed OK");
  }

  #endif

  for (int i = 0; i < o -> index; i++)
  {
    printf("%s\n",  o -> line[i]);
  }


  free_prog(p);
  free_variables(v);
  free_tokens(pi -> tokens);
  free_output(o);

  free(pi);

  fclose(f);
}

void test_parser(void)
{
  Program* p = ncalloc(1, sizeof(Program));
  p -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p -> input[0] = '\0';

  Token** t = init_tokens();

  //Testing tokeniser first with all example cases
  strcpy(p -> input, "((SET A '1'))");
  tokenise(t, p);

  assert(t[0] -> type == LEFTBRACKET);
  assert(t[1] -> type == LEFTBRACKET);
  assert(t[2] -> type == SET);
  assert(t[3] -> type == VAR);
  assert(t[4] -> type == LITERAL);
  assert(strcmp(t[4] -> value, "1") == 0);
  assert(t[5] -> type == RIGHTBRACKET);
  assert(t[6] -> type == RIGHTBRACKET);
  assert(t[7] -> type == END);

  //After scanning in input, all tokens are set to the end token
  //to allow for the recursive descent parser to stop working
  assert(t[100] -> type == END);

  free_prog(p);
  free_tokens(t);

  Program* p1 = ncalloc(1, sizeof(Program));
  p1 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p1 -> input[0] = '\0';

  Token** t1 = init_tokens();

  strcpy(p1-> input, "PRINT \"Hello world\"");
  tokenise(t1, p1);

  assert(t1[0] -> type == PRINT);
  assert(t1[1] -> type == STRING);
  assert(strcmp(t1[1] -> value, "Hello world") == 0);

  free_prog(p1);
  free_tokens(t1);

  //Testing ability of process_word to handle all keywords, and garbage

  Program* p2 = ncalloc(1, sizeof(Program));
  p2 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p2 -> input[0] = '\0';

  Token** t2 = init_tokens();

  strcpy(p2 -> input, "CAR CDR CONS PLUS LENGTH LESS GREATER EQUAL SET PRINT IF WHILE GARBAGE");
  tokenise(t2, p2);

  assert(t2[0] -> type == CAR);
  assert(t2[1] -> type == CDR);
  assert(t2[2] -> type == CONS);
  assert(t2[3] -> type == PLUS);
  assert(t2[4] -> type == LENGTH);
  assert(t2[5] -> type == LESS);
  assert(t2[6] -> type == GREATER);
  assert(t2[7] -> type == EQUAL);
  assert(t2[8] -> type == SET);
  assert(t2[9] -> type == PRINT);
  assert(t2[10] -> type == IF);
  assert(t2[11] -> type == LOOP);
  assert(t2[12] -> type == UNKNOWN);
  assert(strcmp(t2[12] -> value, "GARBAGE") == 0);


  free_prog(p2);
  free_tokens(t2);


  //Testing ability of process_type to handle strings that start, but don't end
  Program* p3 = ncalloc(1, sizeof(Program));
  p3 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p3 -> input[0] = '\0';

  Token** t3 = init_tokens();

  strcpy(p3 -> input, "PRINT \"Hello world");
  tokenise(t3, p3);

  assert(t3[0] -> type == PRINT);
  assert(t3[1] -> type == UNKNOWN);

  free_prog(p3);
  free_tokens(t3);

  //Testing ability of process_type to handle literals that start, but don't end
  Program* p4 = ncalloc(1, sizeof(Program));
  p4 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p4 -> input[0] = '\0';

  Token** t4 = init_tokens();

  strcpy(p4 -> input, "SET A '(1 2)");
  tokenise(t4, p4);

  assert(t4[0] -> type == SET);
  assert(t4[1] -> type == VAR);
  assert(t4[2] -> type == UNKNOWN);

  free_prog(p4);
  free_tokens(t4);

  //Testing ability of tokeniser to handle lowercase garbage input
  Program* p5 = ncalloc(1, sizeof(Program));
  p5 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p5 -> input[0] = '\0';

  Token** t5 = init_tokens();

  strcpy(p5 -> input, "hello");
  tokenise(t5, p5);

  assert(t5[0] -> type == UNKNOWN);
  assert(strcmp(t5[0] -> value, "h") == 0);
  assert(t5[1] -> type == UNKNOWN);
  assert(strcmp(t5[1] -> value, "e") == 0);
  assert(t5[2] -> type == UNKNOWN);
  assert(strcmp(t5[2] -> value, "l") == 0);
  assert(t5[3] -> type == UNKNOWN);
  assert(strcmp(t5[3] -> value, "l") == 0);
  assert(t5[4] -> type == UNKNOWN);
  assert(strcmp(t5[4] -> value, "o") == 0);

  free_prog(p5);
  free_tokens(t5);

  /*Parser testing - starting from exit points of terminal cases and
  working back up*/

  //Testing List - checking that each terminal case returns right value

  Program* p6 = ncalloc(1, sizeof(Program));
  p6 -> input = ncalloc(TEST_LEN, sizeof(char));
  p6 -> input[0] = '\0';

  Token** t6 = init_tokens();

  strcpy(p6 -> input, "A");

  tokenise(t6, p6);

  assert(t6[0] -> type == VAR);

  ProgramInfo* pi = ncalloc(1, sizeof(ProgramInfo));
  pi -> valid = true;
  pi -> tokens = t6;
  pi -> index = 0;

  Variables* v = ncalloc(1, sizeof(Variables));
  Output* o = init_output();

  bool parse1 = List(pi, v, o);
  assert(parse1);

  free_prog(p6);
  free_variables(v);
  free_tokens(t6);
  free_output(o);

  free(pi);

  //Testing exit from literal

  Program* p7 = ncalloc(1, sizeof(Program));
  p7 -> input = ncalloc(TEST_LEN, sizeof(char));
  p7 -> input[0] = '\0';

  Token** t7 = init_tokens();

  strcpy(p7 -> input, "'(1 2 3)'");

  tokenise(t7, p7);

  assert(t7[0] -> type == LITERAL);

  ProgramInfo* pi1 = ncalloc(1, sizeof(ProgramInfo));
  pi1 -> valid = true;
  pi1 -> tokens = t7;
  pi1 -> index = 0;

  Variables* v1 = ncalloc(1, sizeof(Variables));
  Output* o1 = init_output();

  bool parse2 = List(pi1, v1, o1);
  assert(parse2);

  free_prog(p7);
  free_variables(v1);
  free_tokens(t7);
  free_output(o1);

  free(pi1);

  //Testing exit from nil_type

  Program* p8 = ncalloc(1, sizeof(Program));
  p8 -> input = ncalloc(TEST_LEN, sizeof(char));
  p8 -> input[0] = '\0';

  Token** t8 = init_tokens();

  strcpy(p8 -> input, "NIL");

  tokenise(t8, p8);

  assert(t8[0] -> type == NIL_TYPE);

  ProgramInfo* pi2 = ncalloc(1, sizeof(ProgramInfo));
  pi2 -> valid = true;
  pi2 -> tokens = t8;
  pi2 -> index = 0;

  Variables* v2 = ncalloc(1, sizeof(Variables));
  Output* o2 = init_output();

  bool parse3 = List(pi2, v2, o2);
  assert(parse3);

  free_prog(p8);
  free_variables(v2);
  free_tokens(t8);
  free_output(o2);

  free(pi2);

  //Any other case (with exception of RetFunc) should return false

  Program* p9 = ncalloc(1, sizeof(Program));
  p9 -> input = ncalloc(TEST_LEN, sizeof(char));
  p9 -> input[0] = '\0';

  Token** t9 = init_tokens();

  strcpy(p9 -> input, "CAR");

  tokenise(t9, p9);

  assert(t9[0] -> type == CAR);

  ProgramInfo* pi3 = ncalloc(1, sizeof(ProgramInfo));
  pi3 -> valid = true;
  pi3 -> tokens = t9;
  pi3 -> index = 0;

  Variables* v3 = ncalloc(1, sizeof(Variables));
  Output* o3 = init_output();

  bool parse4 = List(pi3, v3, o3);
  assert(parse4 == false);

  free_prog(p9);
  free_variables(v3);
  free_tokens(t9);
  free_output(o3);

  free(pi3);

  //Testing exit from BoolFunc - as all cases (LESS, GREATER, EQUAL)
  //are parsed in the same manner, only testing one (LESS) and erroneous cases


  Program* p10 = ncalloc(1, sizeof(Program));
  p10 -> input = ncalloc(TEST_LEN, sizeof(char));
  p10 -> input[0] = '\0';

  Token** t10 = init_tokens();

  strcpy(p10 -> input, "LESS A B");

  tokenise(t10, p10);

  ProgramInfo* pi4 = ncalloc(1, sizeof(ProgramInfo));
  pi4 -> valid = true;
  pi4 -> tokens = t10;
  pi4 -> index = 0;

  Variables* v4 = ncalloc(1, sizeof(Variables));
  Output* o4 = init_output();

  bool parse5 = BoolFunc(pi4, v4, o4);
  assert(parse5);

  free_prog(p10);
  free_variables(v4);
  free_tokens(t10);
  free_output(o4);

  free(pi4);

  //Generate error due to incorrect format

  Program* p11 = ncalloc(1, sizeof(Program));
  p11 -> input = ncalloc(TEST_LEN, sizeof(char));
  p11 -> input[0] = '\0';

  Token** t11 = init_tokens();

  strcpy(p11 -> input, "LESS A CDR");

  tokenise(t11, p11);

  ProgramInfo* pi5 = ncalloc(1, sizeof(ProgramInfo));
  pi5 -> valid = true;
  pi5 -> tokens = t11;
  pi5 -> index = 0;

  Variables* v5 = ncalloc(1, sizeof(Variables));
  Output* o5 = init_output();

  bool parse6 = BoolFunc(pi5, v5, o5);
  assert(parse6 == false);
  assert(strcmp(o5 -> line[0], "Format is LESS/EQUAL/GREATER <LIST><LIST>") == 0);

  free_prog(p11);
  free_variables(v5);
  free_tokens(t11);
  free_output(o5);

  free(pi5);

  //Returns false if not LESS/EQUAL/GREATER

  Program* p12 = ncalloc(1, sizeof(Program));
  p12 -> input = ncalloc(TEST_LEN, sizeof(char));
  p12 -> input[0] = '\0';

  Token** t12 = init_tokens();

  strcpy(p12 -> input, "CDR A");

  tokenise(t12, p12);

  ProgramInfo* pi6 = ncalloc(1, sizeof(ProgramInfo));
  pi6 -> valid = true;
  pi6 -> tokens = t12;
  pi6 -> index = 0;

  Variables* v6 = ncalloc(1, sizeof(Variables));
  Output* o6 = init_output();

  bool parse7 = BoolFunc(pi6, v6, o6);
  assert(parse7 == false);

  free_prog(p12);
  free_variables(v6);
  free_tokens(t12);
  free_output(o6);

  free(pi6);

  //Testing IntFunc - returns true due to right format

  Program* p13 = ncalloc(1, sizeof(Program));
  p13 -> input = ncalloc(TEST_LEN, sizeof(char));
  p13 -> input[0] = '\0';

  Token** t13 = init_tokens();

  strcpy(p13 -> input, "PLUS A B");

  tokenise(t13, p13);

  ProgramInfo* pi7 = ncalloc(1, sizeof(ProgramInfo));
  pi7 -> valid = true;
  pi7 -> tokens = t13;
  pi7 -> index = 0;

  Variables* v7 = ncalloc(1, sizeof(Variables));
  Output* o7 = init_output();

  bool parse8 = IntFunc(pi7, v7, o7);
  assert(parse8 == true);

  free_prog(p13);
  free_variables(v7);
  free_tokens(t13);
  free_output(o7);

  free(pi7);

  //Testing IntFunc - returns false due to wrong format and generates error


  Program* p14 = ncalloc(1, sizeof(Program));
  p14 -> input = ncalloc(TEST_LEN, sizeof(char));
  p14 -> input[0] = '\0';

  Token** t14 = init_tokens();

  strcpy(p14 -> input, "PLUS A CAR");

  tokenise(t14, p14);

  ProgramInfo* pi8 = ncalloc(1, sizeof(ProgramInfo));
  pi8 -> valid = true;
  pi8 -> tokens = t14;
  pi8 -> index = 0;

  Variables* v8 = ncalloc(1, sizeof(Variables));
  Output* o8 = init_output();

  bool parse9 = IntFunc(pi8, v8, o8);
  assert(parse9 == false);
  assert(strcmp(o8 -> line[0], "Format is PLUS <LIST><LIST>") == 0);


  free_prog(p14);
  free_variables(v8);
  free_tokens(t14);
  free_output(o8);

  free(pi8);

  //Testing IntFunc - returns false due to wrong format and generates error


  Program* p15 = ncalloc(1, sizeof(Program));
  p15 -> input = ncalloc(TEST_LEN, sizeof(char));
  p15 -> input[0] = '\0';

  Token** t15 = init_tokens();

  strcpy(p15 -> input, "PLUS CDR");

  tokenise(t15, p15);

  ProgramInfo* pi9 = ncalloc(1, sizeof(ProgramInfo));
  pi9 -> valid = true;
  pi9 -> tokens = t15;
  pi9 -> index = 0;

  Variables* v9 = ncalloc(1, sizeof(Variables));
  Output* o9 = init_output();

  bool parse10 = IntFunc(pi9, v9, o9);
  assert(parse10 == false);
  assert(strcmp(o9 -> line[0], "Format is PLUS <LIST><LIST>") == 0);


  free_prog(p15);
  free_variables(v9);
  free_tokens(t15);
  free_output(o9);

  free(pi9);

  //Testing IntFunc - returns true due to right format

  Program* p16 = ncalloc(1, sizeof(Program));
  p16 -> input = ncalloc(TEST_LEN, sizeof(char));
  p16 -> input[0] = '\0';

  Token** t16 = init_tokens();

  strcpy(p16 -> input, "LENGTH A");

  tokenise(t16, p16);

  ProgramInfo* pi10 = ncalloc(1, sizeof(ProgramInfo));
  pi10 -> valid = true;
  pi10 -> tokens = t16;
  pi10 -> index = 0;

  Variables* v10 = ncalloc(1, sizeof(Variables));
  Output* o10 = init_output();

  bool parse11 = IntFunc(pi10, v10, o10);
  assert(parse11 == true);

  free_prog(p16);
  free_variables(v10);
  free_tokens(t16);
  free_output(o10);

  free(pi10);

  //Testing IntFunc - returns false due to wrong format
  //Less by itself does not constitute a list

  Program* p17 = ncalloc(1, sizeof(Program));
  p17 -> input = ncalloc(TEST_LEN, sizeof(char));
  p17 -> input[0] = '\0';

  Token** t17 = init_tokens();

  strcpy(p17 -> input, "LENGTH LESS");

  tokenise(t17, p17);

  ProgramInfo* pi11 = ncalloc(1, sizeof(ProgramInfo));
  pi11 -> valid = true;
  pi11-> tokens = t17;
  pi11 -> index = 0;

  Variables* v11 = ncalloc(1, sizeof(Variables));
  Output* o11 = init_output();

  bool parse12 = IntFunc(pi11, v11, o11);
  assert(parse12 == false);
  assert(strcmp(o11 -> line[0], "Format is LENGTH <LIST>") == 0);

  free_prog(p17);
  free_variables(v11);
  free_tokens(t17);
  free_output(o11);

  free(pi11);

  /*Testing ListFunc with CAR - returns true
  As CAR and CDR share same format of CAR/CDR <LIST>,
  and share nearly the same code except
  for I am only testing one, as tokeniser testing has shown
  that both are correctly tokenised.
  */

  Program* p18 = ncalloc(1, sizeof(Program));
  p18 -> input = ncalloc(TEST_LEN, sizeof(char));
  p18 -> input[0] = '\0';

  Token** t18 = init_tokens();

  strcpy(p18 -> input, "CAR A");

  tokenise(t18, p18);

  ProgramInfo* pi12 = ncalloc(1, sizeof(ProgramInfo));
  pi12 -> valid = true;
  pi12-> tokens = t18;
  pi12 -> index = 0;

  Variables* v12 = ncalloc(1, sizeof(Variables));
  Output* o12 = init_output();

  bool parse13 = ListFunc(pi12, v12, o12);
  assert(parse13 == true);

  free_prog(p18);
  free_variables(v12);
  free_tokens(t18);
  free_output(o12);

  free(pi12);

  /*Testing ListFunc with CAR and non-list - return false
  */

  Program* p19 = ncalloc(1, sizeof(Program));
  p19 -> input = ncalloc(TEST_LEN, sizeof(char));
  p19 -> input[0] = '\0';

  Token** t19 = init_tokens();

  strcpy(p19 -> input, "CAR \"Hello world\" ");

  tokenise(t19, p19);

  ProgramInfo* pi13 = ncalloc(1, sizeof(ProgramInfo));
  pi13 -> valid = true;
  pi13-> tokens = t19;
  pi13 -> index = 0;

  Variables* v13 = ncalloc(1, sizeof(Variables));
  Output* o13 = init_output();

  bool parse14 = ListFunc(pi13, v13, o13);
  assert(parse14 == false);
  assert(strcmp(o13 -> line[0], "Format is CAR <LIST>") == 0);

  free_prog(p19);
  free_variables(v13);
  free_tokens(t19);
  free_output(o13);

  free(pi13);

  /*Testing ListFunc with CONS and non-list - return false
  */

  Program* p20 = ncalloc(1, sizeof(Program));
  p20 -> input = ncalloc(TEST_LEN, sizeof(char));
  p20 -> input[0] = '\0';

  Token** t20 = init_tokens();

  strcpy(p20 -> input, "CONS \"Hello world\" ");

  tokenise(t20, p20);

  ProgramInfo* pi14 = ncalloc(1, sizeof(ProgramInfo));
  pi14 -> valid = true;
  pi14-> tokens = t20;
  pi14 -> index = 0;

  Variables* v14 = ncalloc(1, sizeof(Variables));
  Output* o14 = init_output();

  bool parse15 = ListFunc(pi14, v14, o14);
  assert(parse15 == false);
  assert(strcmp(o14 -> line[0], "Format is CONS <LIST><LIST>") == 0);


  free_prog(p20);
  free_variables(v14);
  free_tokens(t20);
  free_output(o14);

  free(pi14);

  /*Testing ListFunc with CONS and non-list - return false
  */

  Program* p21 = ncalloc(1, sizeof(Program));
  p21 -> input = ncalloc(TEST_LEN, sizeof(char));
  p21 -> input[0] = '\0';

  Token** t21 = init_tokens();

  strcpy(p21 -> input, "CONS A \"Hello world\" ");

  tokenise(t21, p21);

  ProgramInfo* pi15 = ncalloc(1, sizeof(ProgramInfo));
  pi15 -> valid = true;
  pi15-> tokens = t21;
  pi15 -> index = 0;

  Variables* v15 = ncalloc(1, sizeof(Variables));
  Output* o15 = init_output();

  bool parse16 = ListFunc(pi15, v15, o15);
  assert(parse16 == false);
  assert(strcmp(o15 -> line[0], "Format is CONS <LIST><LIST>") == 0);


  free_prog(p21);
  free_variables(v15);
  free_tokens(t21);
  free_output(o15);

  free(pi15);

  /*Testing ListFunc with CONS and list - returns true
  */

  Program* p22 = ncalloc(1, sizeof(Program));
  p22 -> input = ncalloc(TEST_LEN, sizeof(char));
  p22 -> input[0] = '\0';

  Token** t22 = init_tokens();

  strcpy(p22 -> input, "CONS A NIL");

  tokenise(t22, p22);

  ProgramInfo* pi16 = ncalloc(1, sizeof(ProgramInfo));
  pi16 -> valid = true;
  pi16-> tokens = t22;
  pi16 -> index = 0;

  Variables* v16 = ncalloc(1, sizeof(Variables));
  Output* o16 = init_output();

  bool parse17 = ListFunc(pi16, v16, o16);
  assert(parse17 == true);

  free_prog(p22);
  free_variables(v16);
  free_tokens(t22);
  free_output(o16);

  free(pi16);

  /*Testing Set - incorrect format
  */

  Program* p23 = ncalloc(1, sizeof(Program));
  p23 -> input = ncalloc(TEST_LEN, sizeof(char));
  p23 -> input[0] = '\0';

  Token** t23 = init_tokens();

  strcpy(p23 -> input, "SET CDR A");

  tokenise(t23, p23);

  ProgramInfo* pi17 = ncalloc(1, sizeof(ProgramInfo));
  pi17 -> valid = true;
  pi17-> tokens = t23;
  pi17 -> index = 0;

  Variables* v17 = ncalloc(1, sizeof(Variables));
  Output* o17 = init_output();

  bool parse18 = Set(pi17, v17, o17);
  assert(parse18 == false);
  assert(strcmp(o17 -> line[0], "Expected variable") == 0);

  free_prog(p23);
  free_variables(v17);
  free_tokens(t23);
  free_output(o17);

  free(pi17);

  /*Testing Set - incorrect format
  */

  Program* p24 = ncalloc(1, sizeof(Program));
  p24 -> input = ncalloc(TEST_LEN, sizeof(char));
  p24 -> input[0] = '\0';

  Token** t24 = init_tokens();

  strcpy(p24 -> input, "SET A SET");

  tokenise(t24, p24);

  ProgramInfo* pi18 = ncalloc(1, sizeof(ProgramInfo));
  pi18 -> valid = true;
  pi18 -> tokens = t24;
  pi18 -> index = 0;

  Variables* v18 = ncalloc(1, sizeof(Variables));
  Output* o18 = init_output();

  bool parse19 = Set(pi18, v18, o18);
  assert(parse19 == false);
  assert(strcmp(o18 -> line[0], "Expected list: format is SET <VAR> <LIST>") == 0);

  free_prog(p24);
  free_variables(v18);
  free_tokens(t24);
  free_output(o18);

  free(pi18);

  /*Testing Set - correct format - note that literal is not tested, so parses
  ok - this would fail the interpreter
  */

  Program* p25 = ncalloc(1, sizeof(Program));
  p25 -> input = ncalloc(TEST_LEN, sizeof(char));
  p25 -> input[0] = '\0';

  Token** t25 = init_tokens();

  strcpy(p25 -> input, "SET A '(1 2'");

  tokenise(t25, p25);

  ProgramInfo* pi19 = ncalloc(1, sizeof(ProgramInfo));
  pi19 -> valid = true;
  pi19 -> tokens = t25;
  pi19 -> index = 0;

  Variables* v19 = ncalloc(1, sizeof(Variables));
  Output* o19 = init_output();

  bool parse20 = Set(pi19, v19, o19);
  assert(parse20 == true);

  free_prog(p25);
  free_variables(v19);
  free_tokens(t25);
  free_output(o19);

  free(pi19);

  /*Testing Print - incorrect format
  */

  Program* p26 = ncalloc(1, sizeof(Program));
  p26 -> input = ncalloc(TEST_LEN, sizeof(char));
  p26 -> input[0] = '\0';

  Token** t26 = init_tokens();

  strcpy(p26 -> input, "PRINT CDR");

  tokenise(t26, p26);

  ProgramInfo* pi20 = ncalloc(1, sizeof(ProgramInfo));
  pi20 -> valid = true;
  pi20 -> tokens = t26;
  pi20 -> index = 0;

  Variables* v20 = ncalloc(1, sizeof(Variables));
  Output* o20 = init_output();

  bool parse21 = Print(pi20, v20, o20);
  assert(parse21 == false);
  assert(strcmp(o20 -> line[0], "Expected format is PRINT <LIST> || <STRING>") == 0);

  free_prog(p26);
  free_variables(v20);
  free_tokens(t26);
  free_output(o20);

  free(pi20);

  /*Testing Print - correct format with string
  */

  Program* p27 = ncalloc(1, sizeof(Program));
  p27 -> input = ncalloc(TEST_LEN, sizeof(char));
  p27 -> input[0] = '\0';

  Token** t27 = init_tokens();

  strcpy(p27 -> input, "PRINT \"Hello world\"");

  tokenise(t27, p27);

  ProgramInfo* pi21 = ncalloc(1, sizeof(ProgramInfo));
  pi21 -> valid = true;
  pi21 -> tokens = t27;
  pi21 -> index = 0;

  Variables* v21 = ncalloc(1, sizeof(Variables));
  Output* o21 = init_output();

  bool parse22 = Print(pi21, v21, o21);
  assert(parse22 == true);

  free_prog(p27);
  free_variables(v21);
  free_tokens(t27);
  free_output(o21);

  free(pi21);

  /*Testing Print - correct format with List
  */

  Program* p28 = ncalloc(1, sizeof(Program));
  p28 -> input = ncalloc(TEST_LEN, sizeof(char));
  p28 -> input[0] = '\0';

  Token** t28 = init_tokens();

  strcpy(p28 -> input, "PRINT (CAR A)");

  tokenise(t28, p28);

  ProgramInfo* pi22 = ncalloc(1, sizeof(ProgramInfo));
  pi22 -> valid = true;
  pi22 -> tokens = t28;
  pi22 -> index = 0;

  Variables* v22 = ncalloc(1, sizeof(Variables));
  Output* o22 = init_output();

  bool parse23 = Print(pi22, v22, o22);
  assert(parse23 == true);

  free_prog(p28);
  free_variables(v22);
  free_tokens(t28);
  free_output(o22);

  free(pi22);

  /*Testing If - incorrect format - should have left bracket
  */

  Program* p29 = ncalloc(1, sizeof(Program));
  p29 -> input = ncalloc(TEST_LEN, sizeof(char));
  p29 -> input[0] = '\0';

  Token** t29 = init_tokens();

  strcpy(p29 -> input, "IF A");

  tokenise(t29, p29);

  ProgramInfo* pi23 = ncalloc(1, sizeof(ProgramInfo));
  pi23 -> valid = true;
  pi23 -> tokens = t29;
  pi23 -> index = 0;

  Variables* v23 = ncalloc(1, sizeof(Variables));
  Output* o23 = init_output();

  bool parse24 = IfStatement(pi23, v23, o23);
  assert(parse24 == false);
  assert(strcmp(o23 -> line[0], "Expected left bracket") == 0);

  free_prog(p29);
  free_variables(v23);
  free_tokens(t29);
  free_output(o23);

  free(pi23);

  /*Testing If - incorrect format - should have bool func to evaluate
  */

  Program* p30 = ncalloc(1, sizeof(Program));
  p30 -> input = ncalloc(TEST_LEN, sizeof(char));
  p30 -> input[0] = '\0';

  Token** t30 = init_tokens();

  strcpy(p30 -> input, "IF (CDR A ");

  tokenise(t30, p30);

  ProgramInfo* pi24 = ncalloc(1, sizeof(ProgramInfo));
  pi24 -> valid = true;
  pi24 -> tokens = t30;
  pi24 -> index = 0;

  Variables* v24 = ncalloc(1, sizeof(Variables));
  Output* o24 = init_output();

  bool parse25 = IfStatement(pi24, v24, o24);
  assert(parse25 == false);
  assert(strcmp(o24 -> line[0], "Expected boolean function") == 0);

  free_prog(p30);
  free_variables(v24);
  free_tokens(t30);
  free_output(o24);

  free(pi24);

  /*Testing If - incorrect format - should have right bracket following bool
  */

  Program* p31 = ncalloc(1, sizeof(Program));
  p31 -> input = ncalloc(TEST_LEN, sizeof(char));
  p31 -> input[0] = '\0';

  Token** t31 = init_tokens();

  strcpy(p31 -> input, "IF (EQUAL A B ");

  tokenise(t31, p31);

  ProgramInfo* pi25 = ncalloc(1, sizeof(ProgramInfo));
  pi25 -> valid = true;
  pi25 -> tokens = t31;
  pi25 -> index = 0;

  Variables* v25 = ncalloc(1, sizeof(Variables));
  Output* o25 = init_output();

  bool parse26 = IfStatement(pi25, v25, o25);
  assert(parse26 == false);
  assert(strcmp(o25 -> line[0], "Expected right bracket after bool function") == 0);

  free_prog(p31);
  free_variables(v25);
  free_tokens(t31);
  free_output(o25);

  free(pi25);

  /*Testing If - incorrect format - should have left bracket before calling
  Instrcts (for If-true)
  */

  Program* p32 = ncalloc(1, sizeof(Program));
  p32 -> input = ncalloc(TEST_LEN, sizeof(char));
  p32 -> input[0] = '\0';

  Token** t32 = init_tokens();

  strcpy(p32 -> input, "IF (EQUAL A B) SET A ");

  tokenise(t32, p32);

  ProgramInfo* pi26 = ncalloc(1, sizeof(ProgramInfo));
  pi26 -> valid = true;
  pi26 -> tokens = t32;
  pi26 -> index = 0;

  Variables* v26 = ncalloc(1, sizeof(Variables));
  Output* o26 = init_output();

  bool parse27 = IfStatement(pi26, v26, o26);
  assert(parse27 == false);
  assert(strcmp(o26 -> line[0], "Expected left bracket") == 0);

  free_prog(p32);
  free_variables(v26);
  free_tokens(t32);
  free_output(o26);

  free(pi26);

  /*Testing If - incorrect format - should have left bracket before calling
  Instrcts (for IF-False)
  */

  Program* p33 = ncalloc(1, sizeof(Program));
  p33 -> input = ncalloc(TEST_LEN, sizeof(char));
  p33 -> input[0] = '\0';

  Token** t33 = init_tokens();

  strcpy(p33 -> input, "IF (EQUAL A B)((SET A '10')) SET B ");

  tokenise(t33, p33);

  ProgramInfo* pi27 = ncalloc(1, sizeof(ProgramInfo));
  pi27 -> valid = true;
  pi27 -> tokens = t33;
  pi27 -> index = 0;

  Variables* v27 = ncalloc(1, sizeof(Variables));
  Output* o27 = init_output();

  bool parse28 = IfStatement(pi27, v27, o27);
  assert(parse28 == false);
  assert(strcmp(o27 -> line[0], "Expected left bracket") == 0);

  free_prog(p33);
  free_variables(v27);
  free_tokens(t33);
  free_output(o27);

  free(pi27);

  /*Validly formed If statement returns true
  */

  Program* p34 = ncalloc(1, sizeof(Program));
  p34 -> input = ncalloc(TEST_LEN, sizeof(char));
  p34 -> input[0] = '\0';

  Token** t34 = init_tokens();

  strcpy(p34 -> input, "IF (EQUAL A B)((SET A '10'))((SET A '20'))");

  tokenise(t34, p34);

  ProgramInfo* pi28 = ncalloc(1, sizeof(ProgramInfo));
  pi28 -> valid = true;
  pi28 -> tokens = t34;
  pi28 -> index = 0;

  Variables* v28 = ncalloc(1, sizeof(Variables));
  Output* o28 = init_output();

  bool parse29 = IfStatement(pi28, v28, o28);
  assert(parse29 == true);

  free_prog(p34);
  free_variables(v28);
  free_tokens(t34);
  free_output(o28);

  free(pi28);

  /*While loop - if no left bracket following 'WHILE', then return false
  */

  Program* p35 = ncalloc(1, sizeof(Program));
  p35 -> input = ncalloc(TEST_LEN, sizeof(char));
  p35 -> input[0] = '\0';

  Token** t35 = init_tokens();

  strcpy(p35 -> input, "WHILE A");

  tokenise(t35, p35);

  ProgramInfo* pi29 = ncalloc(1, sizeof(ProgramInfo));
  pi29 -> valid = true;
  pi29 -> tokens = t35;
  pi29 -> index = 0;

  Variables* v29 = ncalloc(1, sizeof(Variables));
  Output* o29 = init_output();

  bool parse30 = WhileLoop(pi29, v29, o29);
  assert(parse30 == false);
  assert(strcmp(o29 -> line[0], "Expected left bracket") == 0);
  assert(pi29 -> valid == false);


  free_prog(p35);
  free_variables(v29);
  free_tokens(t35);
  free_output(o29);

  free(pi29);

  /*While loop - if no BoolFunc, then return false
  */

  Program* p36 = ncalloc(1, sizeof(Program));
  p36 -> input = ncalloc(TEST_LEN, sizeof(char));
  p36 -> input[0] = '\0';

  Token** t36 = init_tokens();

  strcpy(p36 -> input, "WHILE (CDR A)");

  tokenise(t36, p36);

  ProgramInfo* pi30 = ncalloc(1, sizeof(ProgramInfo));
  pi30 -> valid = true;
  pi30 -> tokens = t36;
  pi30 -> index = 0;

  Variables* v30 = ncalloc(1, sizeof(Variables));
  Output* o30 = init_output();

  bool parse31 = WhileLoop(pi30, v30, o30);
  assert(parse31 == false);
  assert(strcmp(o30 -> line[0], "Expected bool func: format is WHILE(BOOLFUNC)(INSTRCTS") == 0);
  assert(pi30 -> valid == false);

  free_prog(p36);
  free_variables(v30);
  free_tokens(t36);
  free_output(o30);

  free(pi30);

  /*While loop - if no right bracket following BoolFunc, then return false
  */

  Program* p37 = ncalloc(1, sizeof(Program));
  p37 -> input = ncalloc(TEST_LEN, sizeof(char));
  p37 -> input[0] = '\0';

  Token** t37 = init_tokens();

  strcpy(p37 -> input, "WHILE (LESS A B");

  tokenise(t37, p37);

  ProgramInfo* pi31 = ncalloc(1, sizeof(ProgramInfo));
  pi31 -> valid = true;
  pi31 -> tokens = t37;
  pi31 -> index = 0;

  Variables* v31 = ncalloc(1, sizeof(Variables));
  Output* o31 = init_output();

  bool parse32 = WhileLoop(pi31, v31, o31);
  assert(parse32 == false);
  assert(strcmp(o31 -> line[0], "Expected right bracket: format is WHILE(BOOLFUNC)(INSTRCTS") == 0);
  assert(pi31 -> valid == false);

  free_prog(p37);
  free_variables(v31);
  free_tokens(t37);
  free_output(o31);

  free(pi31);

  /*While loop - if no left bracket following (BoolFunc), is invalid
  */

  Program* p38 = ncalloc(1, sizeof(Program));
  p38 -> input = ncalloc(TEST_LEN, sizeof(char));
  p38 -> input[0] = '\0';

  Token** t38 = init_tokens();

  strcpy(p38 -> input, "WHILE (LESS A B) SET A '10'");

  tokenise(t38, p38);

  ProgramInfo* pi32 = ncalloc(1, sizeof(ProgramInfo));
  pi32 -> valid = true;
  pi32 -> tokens = t38;
  pi32 -> index = 0;

  Variables* v32 = ncalloc(1, sizeof(Variables));
  Output* o32 = init_output();

  bool parse33 = WhileLoop(pi32, v32, o32);
  assert(parse33 == false);
  assert(strcmp(o32 -> line[0], "Expected left bracket: format is WHILE(BOOLFUNC)(INSTRCTS") == 0);
  assert(pi32 -> valid == false);

  free_prog(p38);
  free_variables(v32);
  free_tokens(t38);
  free_output(o32);

  free(pi32);

  /*While loop - having valid Instrcts after means it is valid program
  */

  Program* p39 = ncalloc(1, sizeof(Program));
  p39 -> input = ncalloc(TEST_LEN, sizeof(char));
  p39 -> input[0] = '\0';

  Token** t39 = init_tokens();

  strcpy(p39 -> input, "WHILE (LESS A B)((SET A '10'))");

  tokenise(t39, p39);

  ProgramInfo* pi33 = ncalloc(1, sizeof(ProgramInfo));
  pi33 -> valid = true;
  pi33 -> tokens = t39;
  pi33 -> index = 0;

  Variables* v33 = ncalloc(1, sizeof(Variables));
  Output* o33 = init_output();

  bool parse34 = WhileLoop(pi33, v33, o33);
  assert(parse34 == true);
  assert(pi33 -> valid == true);

  free_prog(p39);
  free_variables(v33);
  free_tokens(t39);
  free_output(o33);

  free(pi33);

  /*Testing that List can be defined recursively via RetFunc
  through calling Prog directly
  */

  Program* p40 = ncalloc(1, sizeof(Program));
  p40 -> input = ncalloc(TEST_LEN, sizeof(char));
  p40 -> input[0] = '\0';

  Token** t40 = init_tokens();

  strcpy(p40 -> input, "((CAR (CDR (CONS '1' (CONS '2' NIL)))))");

  tokenise(t40, p40);

  ProgramInfo* pi34 = ncalloc(1, sizeof(ProgramInfo));
  pi34 -> valid = true;
  pi34 -> tokens = t40;
  pi34 -> index = 0;

  Variables* v34 = ncalloc(1, sizeof(Variables));
  Output* o34 = init_output();

  Prog(pi34, v34, o34);
  assert(pi34 -> valid == true);


  free_prog(p40);
  free_variables(v34);
  free_tokens(t40);
  free_output(o34);

  free(pi34);

  /*Testing Prog on lengthy program
  */

  Program* p41 = ncalloc(1, sizeof(Program));
  p41 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p41 -> input[0] = '\0';

  Token** t41 = init_tokens();

  strcpy(p41 -> input, "( \
   (IF (EQUAL (LENGTH NIL) '0')   ((PRINT \"NIL HAS ZERO LENGTH\")) ((PRINT \"FAILURE\"))) \
   (IF (EQUAL (LENGTH '(1)') '0') ((PRINT \"FAILURE\")) ((PRINT \"(1) DOESN'T HAVE ZERO LENGTH\")))\
   (IF (EQUAL (LENGTH A) '0')   ((PRINT \"A HAS ZERO LENGTH\")) ((PRINT \"FAILURE\")))\
   (SET L (CONS '2' NIL))\
   (PRINT L)\
   (IF (EQUAL (CAR L) '2') ((PRINT \"L1 has Car 2\"))((PRINT \"FAILURE\")))\
   (SET M (CONS '3' (CONS '4' (CONS '5' NIL))))\
   (PRINT M)\
   (SET N (CONS '1' L))\
   (PRINT N)\
   (SET P (CONS N M))\
   (PRINT P)\
)");

  tokenise(t41, p41);

  ProgramInfo* pi35 = ncalloc(1, sizeof(ProgramInfo));
  pi35 -> valid = true;
  pi35 -> tokens = t41;
  pi35 -> index = 0;

  Variables* v35 = ncalloc(1, sizeof(Variables));
  Output* o35 = init_output();

  Prog(pi35, v35, o35);
  assert(pi35 -> valid == true);

  free_prog(p41);
  free_variables(v35);
  free_tokens(t41);
  free_output(o35);

  free(pi35);

  /*Testing Prog on program where it ends early
  It is missing the final bracket
  */

  Program* p42 = ncalloc(1, sizeof(Program));
  p42 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p42 -> input[0] = '\0';

  Token** t42 = init_tokens();

  strcpy(p42 -> input, "( \
   (IF (EQUAL (LENGTH NIL) '0') \
   ((PRINT \"NIL HAS ZERO LENGTH\")) ((PRINT \"FAILURE\"))) \
   ");

  tokenise(t42, p42);

  ProgramInfo* pi36 = ncalloc(1, sizeof(ProgramInfo));
  pi36 -> valid = true;
  pi36 -> tokens = t42;
  pi36 -> index = 0;

  Variables* v36 = ncalloc(1, sizeof(Variables));
  Output* o36 = init_output();

  Prog(pi36, v36, o36);
  assert(pi36 -> valid == false);
  assert(strcmp(o36 -> line[0], "Expected INSTRCTS") == 0);

  free_prog(p42);
  free_variables(v36);
  free_tokens(t42);
  free_output(o36);

  free(pi36);

}

void test_interpreter(void)
{
  /*Testing interpreter with all types of interpreting cases for Set
  */
  Program* p1 = ncalloc(1, sizeof(Program));
  p1 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p1 -> input[0] = '\0';

  Token** t1 = init_tokens();

  strcpy(p1 -> input, "( \
  (SET A '10') \
  (SET B '(1 2 -100 (3))') \
  (SET C A) \
  (SET D NIL) \
  (SET E (CONS '1' (CONS '2' (CONS '-83' NIL)))) \
  (SET F '(543 -110)') \
  (SET G (CAR F)) \
  (SET H (CDR F)) \
  )" );

  tokenise(t1, p1);

  ProgramInfo* pi1 = ncalloc(1, sizeof(ProgramInfo));
  pi1 -> valid = true;
  pi1 -> tokens = t1;
  pi1 -> index = 0;

  Variables* v1 = ncalloc(1, sizeof(Variables));
  Output* o1 = init_output();

  Prog(pi1, v1, o1);
  assert(pi1 -> valid == true);

  //Check that only A to H have been set
  assert(v1 -> set_var['A' - 'A'] == true);
  assert(v1 -> set_var['B' - 'A'] == true);
  assert(v1 -> set_var['C' - 'A'] == true);
  assert(v1 -> set_var['D' - 'A'] == true);
  assert(v1 -> set_var['E' - 'A'] == true);
  assert(v1 -> set_var['F' - 'A'] == true);
  assert(v1 -> set_var['G' - 'A'] == true);
  assert(v1 -> set_var['H' - 'A'] == true);
  assert(v1 -> set_var['I' - 'A'] == false);
  assert(v1 -> set_var['Z' - 'A'] == false);

  /*Check that all possibilities of Set return right lisp result
  Also, testing CAR/CDR/CONS briefly
  */
  char temp_str[STR_LENGTH];
  lisp* temp_lisp;

  temp_lisp = v1 -> stored_vars['A' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "10") == 0);

  temp_lisp = v1 -> stored_vars['B' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "(1 2 -100 (3))") == 0);

  temp_lisp = v1 -> stored_vars['C' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "10") == 0);

  temp_lisp = v1 -> stored_vars['D' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "()") == 0);

  temp_lisp = v1 -> stored_vars['E' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "(1 2 -83)") == 0);

  temp_lisp = v1 -> stored_vars['F' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "(543 -110)") == 0);

  temp_lisp = v1 -> stored_vars['G' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "543") == 0);

  temp_lisp = v1 -> stored_vars['H' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "(-110)") == 0);

  free_prog(p1);
  free_variables(v1);
  free_tokens(t1);
  free_output(o1);

  free(pi1);

  /*Testing BoolFuncs
  */

  Program* p2 = ncalloc(1, sizeof(Program));
  p2 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p2 -> input[0] = '\0';
  Token** t2 = init_tokens();

  strcpy(p2 -> input, "( \
    (SET A '10') \
    (SET B '5') \
    (SET C (LESS A B)) \
    (SET D (EQUAL A B)) \
    (SET E (GREATER A B)) \
    (SET F '100') \
    (SET G '100') \
    (SET H (LESS F G)) \
    (SET I (EQUAL F G)) \
    (SET J (GREATER F G)) \
    (SET K '-100') \
    (SET L '1') \
    (SET M (LESS K L)) \
    (SET N (EQUAL K L)) \
    (SET O (GREATER K L)) \
  )" );

  tokenise(t2, p2);

  ProgramInfo* pi2 = ncalloc(1, sizeof(ProgramInfo));
  pi2 -> valid = true;
  pi2 -> tokens = t2;
  pi2 -> index = 0;

  Variables* v2 = ncalloc(1, sizeof(Variables));
  Output* o2 = init_output();

  Prog(pi2, v2, o2);

  temp_lisp = v2 -> stored_vars['D' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  temp_lisp = v2 -> stored_vars['E' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "1") == 0);

  temp_lisp = v2 -> stored_vars['H' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  temp_lisp = v2 -> stored_vars['I' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "1") == 0);

  temp_lisp = v2 -> stored_vars['J' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  temp_lisp = v2 -> stored_vars['M' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "1") == 0);

  temp_lisp = v2 -> stored_vars['N' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  temp_lisp = v2 -> stored_vars['O' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  free_prog(p2);
  free_variables(v2);
  free_tokens(t2);
  free_output(o2);

  free(pi2);

  /* Testing IntFunc
  */
  Program* p3 = ncalloc(1, sizeof(Program));
  p3 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p3 -> input[0] = '\0';
  Token** t3 = init_tokens();

  strcpy(p3 -> input, "( \
    (SET A (PLUS '1' '2')) \
    (SET B '(1 2 3 -700 5)') \
    (SET C (LENGTH B)) \
    (SET D (LENGTH E)) \
    (SET E (PLUS '(1 2)' '1')) \
    )" );

  tokenise(t3, p3);

  ProgramInfo* pi3 = ncalloc(1, sizeof(ProgramInfo));
  pi3 -> valid = true;
  pi3 -> tokens = t3;
  pi3 -> index = 0;

  Variables* v3 = ncalloc(1, sizeof(Variables));
  Output* o3 = init_output();

  Prog(pi3, v3, o3);

  temp_lisp = v3 -> stored_vars['A' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "3") == 0);

  temp_lisp = v3 -> stored_vars['C' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "5") == 0);

  temp_lisp = v3 -> stored_vars['D' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "0") == 0);

  //Cannot add non-atoms together
  temp_lisp = v3 -> stored_vars['E' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(o3 -> line[0], "Cannot add two non-atoms together") == 0);
  assert(pi3 -> valid == false);

  free_prog(p3);
  free_variables(v3);
  free_tokens(t3);
  free_output(o3);

  free(pi3);

  /* Testing ListFunc through complex function
  */
  Program* p4 = ncalloc(1, sizeof(Program));
  p4 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p4 -> input[0] = '\0';
  Token** t4 = init_tokens();

  strcpy(p4 -> input, "( \
    (SET A (CONS (CAR '(1 2)') (CONS (CDR '(3 4)') NIL))))) \
    )" );

  tokenise(t4, p4);

  ProgramInfo* pi4 = ncalloc(1, sizeof(ProgramInfo));
  pi4 -> valid = true;
  pi4 -> tokens = t4;
  pi4 -> index = 0;

  Variables* v4 = ncalloc(1, sizeof(Variables));
  Output* o4 = init_output();

  Prog(pi4, v4, o4);

  temp_lisp = v4 -> stored_vars['A' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "(1 (4))") == 0);


  free_prog(p4);
  free_variables(v4);
  free_tokens(t4);
  free_output(o4);

  free(pi4);

  /* Testing Print
  */
  Program* p5 = ncalloc(1, sizeof(Program));
  p5 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p5 -> input[0] = '\0';
  Token** t5 = init_tokens();

  strcpy(p5 -> input, "( \
  (PRINT \"Hello\") \
  (PRINT '(1 2 3)') \
  (PRINT (LESS '1' '2')) \
  (PRINT (GREATER '2' '3')) \
  (PRINT (CONS (CAR '(1 2)') (CONS (CDR '(3 4)') NIL))))) \
    )" );

  tokenise(t5, p5);

  ProgramInfo* pi5 = ncalloc(1, sizeof(ProgramInfo));
  pi5 -> valid = true;
  pi5 -> tokens = t5;
  pi5 -> index = 0;

  Variables* v5 = ncalloc(1, sizeof(Variables));
  Output* o5 = init_output();

  Prog(pi5, v5, o5);

  assert(strcmp(o5 -> line[0], "Hello") == 0);
  assert(strcmp(o5 -> line[1], "(1 2 3)") == 0);
  assert(strcmp(o5 -> line[2], "1") == 0);
  assert(strcmp(o5 -> line[3], "0") == 0);
  assert(strcmp(o5 -> line[4], "(1 (4))") == 0);

  free_prog(p5);
  free_variables(v5);
  free_tokens(t5);
  free_output(o5);

  free(pi5);

  /* Testing IF - skips garbage in both statements
  */
  Program* p6 = ncalloc(1, sizeof(Program));
  p6 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p6 -> input[0] = '\0';
  Token** t6 = init_tokens();

  strcpy(p6 -> input, "( \
   (IF (EQUAL '1' '1') ((PRINT \"ONE\")(PRINT \"TWO\"))((GARBAGE)(GARBAGE)(PRINT \"FAILURE\"))) \
   (IF (EQUAL '2' '1') ((GARBAGE) (PRINT \"FAILURE\"))((PRINT \"THREE\") (PRINT \"FOUR\"))) \
    )" );

  tokenise(t6, p6);

  ProgramInfo* pi6 = ncalloc(1, sizeof(ProgramInfo));
  pi6 -> valid = true;
  pi6 -> tokens = t6;
  pi6 -> index = 0;

  Variables* v6 = ncalloc(1, sizeof(Variables));
  Output* o6 = init_output();

  Prog(pi6, v6, o6);

  assert(strcmp(o6 -> line[0], "ONE") == 0);
  assert(strcmp(o6 -> line[1], "TWO") == 0);
  assert(strcmp(o6 -> line[2], "THREE") == 0);
  assert(strcmp(o6 -> line[3], "FOUR") == 0);

  free_prog(p6);
  free_variables(v6);
  free_tokens(t6);
  free_output(o6);

  free(pi6);

  /* Testing WHILE
  */
  Program* p7 = ncalloc(1, sizeof(Program));
  p7 -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p7 -> input[0] = '\0';
  Token** t7 = init_tokens();

  strcpy(p7 -> input, "( \
   (SET C '5') \
   (SET D '2') \
   (WHILE (LESS '0' C)( \
      (PRINT C) \
      (SET A (PLUS '-1' C)) \
      (SET C A) \
      ) ) \
   (SET E '10') \
   (WHILE (GREATER '1' '1')( \
   	(SET A (PLUS '-1' E)) \
    (SET E A)  \
   ) ) \
   (SET B '-15') \
   )" );

  tokenise(t7, p7);

  ProgramInfo* pi7 = ncalloc(1, sizeof(ProgramInfo));
  pi7 -> valid = true;
  pi7 -> tokens = t7;
  pi7 -> index = 0;

  Variables* v7 = ncalloc(1, sizeof(Variables));
  Output* o7 = init_output();

  Prog(pi7, v7, o7);

  assert(strcmp(o7 -> line[0], "5") == 0);
  assert(strcmp(o7 -> line[1], "4") == 0);
  assert(strcmp(o7 -> line[2], "3") == 0);
  assert(strcmp(o7 -> line[3], "2") == 0);
  assert(strcmp(o7 -> line[4], "1") == 0);

  //False WHILE loop never gets called
  temp_lisp = v7 -> stored_vars['E' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "10") == 0);

  //Continues to the instruction after the while loop
  temp_lisp = v7 -> stored_vars['B' - 'A'];
  lisp_tostring(temp_lisp, temp_str);
  assert(strcmp(temp_str, "-15") == 0);

  free_prog(p7);
  free_variables(v7);
  free_tokens(t7);
  free_output(o7);

  free(pi7);

}


void check_args(int argc, char* argv[])
{
  if (argc != CORRECT_ARGS)
  {
    fprintf(stderr, "Usage: %s <filename.ncl>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
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

Token** init_tokens(void)
{
  Token** tokens = ncalloc(MAX_TOKENS, sizeof(Token*));
  for (int i = 0; i < MAX_TOKENS; i++)
  {
    tokens[i] = ncalloc(1, sizeof(Token));
  }

  return tokens;
}

Program* Initialise(FILE* f)
{
  Program* p = ncalloc(1, sizeof(Program));
  p -> input = ncalloc(MAX_INPUT_LEN, sizeof(char));
  p -> input[0] = '\0';

  char temp_str[STR_LENGTH];
  while (fgets(temp_str, STR_LENGTH, f) != NULL)
  {
    strcat(p -> input, temp_str);
  }
  return p;
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


Output* init_output(void)
{
  Output* o = ncalloc(1, sizeof(Output));

  o -> line = ncalloc(MAX_LINES, sizeof(char*));

  for (int i = 0; i < MAX_LINES; i++)
  {
    o -> line[i] = ncalloc(STR_LENGTH, sizeof(char));
  }

  return o;
}

void Prog(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Could not find opening bracket");
    return;
  }

  (p -> index)++;
  Instrcts(p, v, o);
}

void Instrcts(ProgramInfo* p, Variables* v, Output* o)
{
  if (o -> full)
  {
    return;
  }
  if (p -> tokens[p -> index] -> type == END)
  {
    p -> valid = false;
    add_output(o, "Expected INSTRCTS");
    return;
  }

  if (p -> tokens[p -> index] -> type == RIGHTBRACKET)
  {
    return;
  }

  Instrct(p, v, o);

  if (p -> valid)
  {
      Instrcts(p, v, o);
  }
}

void Instrct(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type == END)
  {
    p -> valid = false;
    add_output(o, "Expected INSTRCT");
    return;
  }

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
    p -> valid = false;
    add_output(o,  "No left bracket found");
    return;
  }

 (p -> index)++;

 bool func_result = Func(p, v, o);

 if (!func_result)
 {
   p -> valid = false;
   add_output(o, "No function found");
   return;
 }

 if (p -> valid == false)
 {
   return;
 }

 if (p -> tokens[p -> index] -> type != RIGHTBRACKET)
  {
   p -> valid = false;
   add_output(o,  "Expected to find right bracket");
   return;
  }

  (p -> index)++;

}

bool Func(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type == END)
  {
    p -> valid = false;
    add_output(o, "Expected function, but program ended");
    return false;
  }

  bool ret_result = RetFunc(p, v, o);
  bool io_result = IOFunc(p, v, o);
  bool if_result = IfStatement(p, v, o);
  bool while_result = WhileLoop(p,v, o);

  if (!ret_result && !io_result && !if_result && !while_result)
  {
    p -> valid = false;
    return false;
  }

  return true;
}

bool RetFunc(ProgramInfo* p, Variables* v, Output* o)
{
  bool list_result = ListFunc(p, v, o);
  bool int_result = IntFunc(p, v, o);
  bool bool_result = BoolFunc(p, v, o);

  if (!list_result && !int_result && !bool_result)
  {
    return false;
  }

  return true;
}

bool ListFunc(ProgramInfo* p, Variables* v, Output* o)
{
  switch(p -> tokens[p -> index] -> type)
  {
    case END:
    {
      p -> valid = false;
      add_output(o, "Ended early");
      return false;
    }
    case CAR:
    {
      (p -> index)++;
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is CAR <LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_listfunc(v, CAR);
      #endif

      return true;
    }

    case CDR:
    {
      (p -> index)++;
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is CDR <LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_listfunc(v, CDR);
      #endif
      return true;

    }
    case CONS:
    {
      (p -> index)++;
      if (!List(p, v, o))
      {
        add_output(o, "Format is CONS <LIST><LIST>");
        return false;
      }
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is CONS <LIST><LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_listfunc(v, CONS);
      #endif

      return true;
    }

    default:
    {
      return false;
    }

  }

  return false;
}

#ifdef INTERP
void interpret_listfunc(Variables* v, SyntaxType s)
{
  switch(s)
  {
    case CAR:
    {
      lisp* temp1 = pop_lisp(&(v -> s));
      lisp* temp2 = lisp_car(temp1);
      lisp* temp3 = lisp_copy(temp2);

      lisp_free(&temp1);
      push_lisp(&(v -> s), temp3);
      break;
    }

    case CDR:
    {
      lisp* temp1 = pop_lisp(&(v -> s));
      lisp* temp2 = lisp_cdr(temp1);
      lisp* temp3 = lisp_copy(temp2);

      lisp_free(&temp1);
      push_lisp(&(v -> s), temp3);
      break;
    }

    case CONS:
    {
      lisp* temp1 = pop_lisp(&(v -> s));
      lisp* temp2 = pop_lisp(&(v -> s));
      lisp* temp3 = lisp_cons(temp2, temp1);
      lisp* temp4 = lisp_copy(temp3);

      lisp_free(&temp3);
      push_lisp(&(v -> s), temp4);
    }

    default:
    {

    }
  }
}

#endif

bool IntFunc(ProgramInfo* p, Variables* v, Output* o)
{
  switch(p -> tokens[p -> index] -> type)
  {
    case END:
    {
      p -> valid = false;
      add_output(o, "Ended early");
      return false;
    }

    case PLUS:
    {
      (p -> index)++;
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is PLUS <LIST><LIST>");
      	(o -> index)++;
        return false;
      }

      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is PLUS <LIST><LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_plus(p, v, o);
      #endif
      return true;
    }

    case LENGTH:
    {
      (p -> index)++;
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is LENGTH <LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_length(v);
      #endif

      return true;
    }
    default:
    {
      return false;
    }

  }

}

#ifdef INTERP
void interpret_plus(ProgramInfo* p, Variables* v, Output* o)
{
  lisp* l1 = pop_lisp(&(v -> s));
  lisp* l2 = pop_lisp(&(v -> s));
  if (!lisp_isatomic(l1) || !lisp_isatomic(l2))
  {
    add_output(o, "Cannot add two non-atoms together");
    push_lisp(&(v -> s), lisp_fromstring("()"));
    lisp_free(&l1);
    lisp_free(&l2);
    p -> valid = false;
    return;
  }

  int val1 = lisp_getval(l1);
  int val2 = lisp_getval(l2);

  char temp_str[STR_LENGTH];
  sprintf(temp_str, "%d", val1 + val2);

  lisp* l = lisp_fromstring(temp_str);

  lisp_free(&l1);
  lisp_free(&l2);

  push_lisp(&(v -> s), l);

}
#endif

#ifdef INTERP
void interpret_length(Variables* v)
{
  lisp* temp = pop_lisp(&(v -> s));
  int val = lisp_length(temp);
  lisp_free(&temp);
  char temp_str[STR_LENGTH];
  sprintf(temp_str, "%d", val);
  lisp* l = lisp_fromstring(temp_str);
  push_lisp(&(v -> s), l);
}

#endif

bool BoolFunc(ProgramInfo* p, Variables* v, Output* o)
{
  switch(p -> tokens[p -> index] -> type)
  {
    case END:
    {
      p -> valid = false;
      add_output(o, "Ended early");
      return false;
    }

    case LESS: case EQUAL: case GREATER:
    {
      #ifdef INTERP
      push_int(&(p -> s), p -> index);
      #endif
      (p -> index)++;
      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is LESS/EQUAL/GREATER <LIST><LIST>");
        return false;
      }

      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Format is LESS/EQUAL/GREATER <LIST><LIST>");
        return false;
      }

      #ifdef INTERP
      interpret_boolfunc(p, v, o);
      #endif
      return true;
    }

    default:
    {
      return false;
    }

  }
  return false;
}

#ifdef INTERP
void interpret_boolfunc(ProgramInfo* p, Variables* v, Output* o)
{

  lisp* l2 = pop_lisp(&(v -> s));
  lisp* l1 = pop_lisp(&(v -> s));

  int val1 = lisp_getval(l1);
  int val2 = lisp_getval(l2);

  int temp_index = pop_int(&(p -> s));

  if (!lisp_isatomic(l1) || !lisp_isatomic(l2))
  {
    add_output(o, "Cannot interpret non-atoms for less/equal/greater");
    push_lisp(&(v -> s), lisp_fromstring("()"));
    lisp_free(&l1);
    lisp_free(&l2);
    o -> full = true;
    p -> valid = false;
    return;
  }

  switch(p -> tokens[temp_index] -> type)
  {
    case LESS:
    {
      if (val1 < val2)
      {
        push_lisp(&(v -> s), lisp_fromstring("1"));
        break;
      }

      push_lisp(&(v -> s), lisp_fromstring("0"));
      break;

    }

    case EQUAL:
    {
      if (val1 == val2)
      {
        push_lisp(&(v -> s), lisp_fromstring("1"));
        break;
      }

      push_lisp(&(v -> s), lisp_fromstring("0"));
      break;
    }

    case GREATER:
    {
      if (val1 > val2)
      {
        push_lisp(&(v -> s), lisp_fromstring("1"));
        break;
      }
      push_lisp(&(v -> s), lisp_fromstring("0"));
      break;
    }

    default:
    {
      p -> valid = false;
    }
  }

    lisp_free(&l1);
    lisp_free(&l2);
}

#endif

bool IOFunc(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type == END)
  {
    p -> valid = false;
    add_output(o, "Ended early");
    return false;
  }

  bool set_result = Set(p, v, o);
  bool print_result = Print(p, v, o);

  if (!set_result && !print_result)
  {
    return false;
  }

  return true;
}

bool Set(ProgramInfo* p, Variables* v, Output* o)
{
 switch(p -> tokens[p -> index] -> type)
 {
   case END:
   {
    p -> valid = false;
    add_output(o, "Ended early");
    return false;
   }

   case SET:
   {
    (p -> index)++;
    if (p -> tokens[p -> index] -> type != VAR)
    {
      p -> valid = false;
      add_output(o, "Expected variable");
      return false;
    }

    #ifdef INTERP
    push_int(&(p -> s), p -> index);
    #endif

    (p -> index)++;

    if (!List(p, v, o))
    {
      p -> valid = false;
      add_output(o, "Expected list: format is SET <VAR> <LIST>");
      return false;
    }

    #ifdef INTERP
    set_var(p, v);
    #endif

    return true;
   }

   default:
   {
     return false;
   }
 }
  return false;
}

#ifdef INTERP
void set_var(ProgramInfo* p, Variables* v)
{
  lisp* temp = pop_lisp(&(v -> s));
  lisp* l = lisp_copy(temp);
  lisp_free(&temp);

  int temp_index = pop_int(&(p -> s));
  char temp_var = (p -> tokens[temp_index] -> value)[0];

  if (v -> set_var[temp_var - 'A'] == true)
  {
    temp = v -> stored_vars[temp_var - 'A'];
    lisp_free(&temp);
  }

  v -> stored_vars[temp_var - 'A'] = l;
  v -> set_var[temp_var - 'A'] = true;
}
#endif

bool Print(ProgramInfo* p, Variables* v, Output* o)
{
  switch(p -> tokens[p -> index] -> type)
  {
    case END:
    {
      p -> valid = false;
      add_output(o, "Ended early");
      return false;
    }

    case PRINT:
    {
      (p -> index)++;
      if (p -> tokens[p -> index] -> type == STRING)
      {
        #ifdef INTERP
        add_output(o, p -> tokens[p -> index] -> value);
        #endif
        (p -> index)++;
        return true;
      }

      if (!List(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Expected format is PRINT <LIST> || <STRING>");
        return false;
      }

     #ifdef INTERP
     char temp_str[STR_LENGTH];
     lisp* temp_lisp = pop_lisp(&(v -> s));
     lisp_tostring(temp_lisp, temp_str);
     add_output(o, temp_str);
     lisp_free(&temp_lisp);
     #endif

      return true;
    }

    default:
    {
      return false;
    }

  }
 return false;
}

bool IfStatement(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type == END)
  {
    p -> valid = false;
    add_output(o, "Ended early");
    return false;
  }

  if (p -> tokens[p -> index] -> type != IF)
  {
    return false;
  }

  (p -> index)++;

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Expected left bracket");
    return false;
  }

  (p -> index)++;

  if (!BoolFunc(p, v, o))
  {
    p -> valid = false;
    add_output(o, "Expected boolean function");
    return false;
  }

  if (p -> tokens[p -> index] -> type != RIGHTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Expected right bracket after bool function");
    return false;
  }

  (p -> index)++;

  #ifdef INTERP
  bool skip_iftrue = should_skip(v);
  #endif

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Expected left bracket");
    return false;
  }

  (p -> index)++;

  #ifdef INTERP
    if (skip_iftrue == true)
    {
      skip_section(p);
      (p -> index)++;
      Instrcts(p, v, o);
      (p -> index)++;
      return true;
    }
  #endif

  Instrcts(p, v, o);


  if (p -> tokens[p -> index] -> type != RIGHTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Expected right bracket");
    return false;
  }

  (p -> index)++;//Skipping right bracket

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
    p -> valid = false;
    add_output(o, "Expected left bracket");
    return false;
  }

  //Skipping left bracket
  (p -> index)++;

  #ifdef INTERP
  if (skip_iftrue == false)
  {
    skip_section(p);
    return true;
  }
  #endif

  Instrcts(p, v, o);


  (p -> index)++;

  return true;
}

void skip_section(ProgramInfo* p)
{
  int left_brackets = 1;
  int right_brackets = 0;

  while (p -> tokens[p -> index] -> type != END && (right_brackets != left_brackets))
  {
    switch (p -> tokens[p -> index] -> type)
    {
      case LEFTBRACKET:
        left_brackets++;
        break;
      case RIGHTBRACKET:
        right_brackets++;
        break;
      default:
      {

      }
    }

    (p -> index)++;
  }

}

#ifdef INTERP
bool should_skip(Variables* v)
{
  lisp* l = pop_lisp(&(v -> s));
  char temp_str[STR_LENGTH];
  lisp_tostring(l, temp_str);
  lisp_free(&l);
  if (strcmp(temp_str, "1") == 0)
  {
    return false;
  }

  else
  {
    return true;
  }
}

#endif

bool WhileLoop(ProgramInfo* p, Variables* v, Output* o)
{
  if (p -> tokens[p -> index] -> type == END)
  {
  	p -> valid = false;
  	add_output(o, "Ended early");
  	return false;
  }

  if (p -> tokens[p -> index] -> type != LOOP)
  {
  	return false;
  }

  (p -> index)++;

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
  	p -> valid = false;
  	add_output(o, "Expected left bracket");
  	return false;
  }

   (p -> index)++;

   #ifdef INTERP
   int temp_index = (p -> index);
   push_int(&(p -> s), temp_index);
   #endif

  if (!BoolFunc(p, v, o))
  {
    p -> valid = false;
    add_output(o, "Expected bool func: format is WHILE(BOOLFUNC)(INSTRCTS");
    return false;
  }

  if (p -> tokens[p -> index] -> type != RIGHTBRACKET)
  {
  	p -> valid = false;
  	add_output(o, "Expected right bracket: format is WHILE(BOOLFUNC)(INSTRCTS");
  	return false;
  }

  (p -> index)++;

  if (p -> tokens[p -> index] -> type != LEFTBRACKET)
  {
  	p -> valid = false;
  	add_output(o, "Expected left bracket: format is WHILE(BOOLFUNC)(INSTRCTS");
  	return false;
  }

  (p -> index)++;

  #ifdef INTERP
  bool skip_loop = should_skip(v);

  if (skip_loop)
  {
    skip_section(p);
    return true;
  }

  interpret_loop(p, v, o, &skip_loop);

  #else
  Instrcts(p, v, o);

  #endif
  (p -> index)++;
  return true;
}

#ifdef INTERP
void interpret_loop(ProgramInfo* p, Variables* v, Output* o, bool* skip)
{
  if (*skip == true)
  {
    return;
  }

  int bool_position = pop_int(&(p -> s));
  int instrcts_position = p -> index;
  int end_position;
  while (!(*skip) && o -> full != true)
  {
    p -> index = bool_position;
    bool b = BoolFunc(p, v, o);
    *(skip) = should_skip(v);
    /*If should not skip due to BoolFunc result, then continue to execute loop
    */
    if (b && !(*skip))
    {
      p -> index = instrcts_position;
      Instrcts(p, v, o);
      end_position = p -> index;
    }
  }

  p -> index = end_position;
}
#endif

bool List(ProgramInfo* p, Variables* v, Output* o)
{
  switch(p -> tokens[p -> index] -> type)
  {
    case END:
    {
      p -> valid = false;
      add_output(o, "Ended early");
      return false;
    }

    case VAR:
    {
      #ifdef INTERP
      interpret_list(p, v, o, VAR);
      #endif
      (p -> index)++;
      return true;

    }

    case LITERAL:
    {
      #ifdef INTERP
      interpret_list(p, v, o, LITERAL);
      #endif
      (p -> index)++;
      return true;
    }

    case NIL_TYPE:
    {
      #ifdef INTERP
      interpret_list(p, v, o, NIL_TYPE);
      #endif
      (p -> index)++;
      return true;
    }

    case LEFTBRACKET:
    {
      (p -> index)++;
      if (!RetFunc(p, v, o))
      {
        p -> valid = false;
        add_output(o, "Expected RetFunc here");
        return false;
      }
      (p -> index)++;
      return true;
    }

    default:
    {
      return false;
    }
  }
  return false;
}

#ifdef INTERP
void interpret_list(ProgramInfo* p, Variables* v, Output* o, SyntaxType s)
{
  switch(s)
  {
    case VAR:
    {
      char temp_var = (p -> tokens[p -> index] -> value)[0];
      //Second condition added so that one can still get length of non-existent variable
      if (v -> set_var[temp_var - 'A'] != true && p -> tokens[p -> index - 1] -> type != LENGTH)
      {
        add_output(o,  "Variable not set before use");
        p -> valid = false;
        (p -> index)++;
        return;
      }
      push_lisp(&(v -> s), lisp_copy(v -> stored_vars[temp_var - 'A']));
      break;
    }

    case LITERAL:
    {
      if (!is_valid_string(p -> tokens[p -> index] -> value))
      {
        add_output(o, "Interpreter encountered invalid literal");
        p -> valid = false;
        return;
      }
      push_lisp(&(v -> s), lisp_fromstring(p -> tokens[p -> index] -> value));
      break;
    }

   case NIL_TYPE:
   {
     push_lisp(&(v -> s), lisp_fromstring("()"));
   }

   default:
   {

   }

  }
}
#endif

void push_lisp(LispStack* s, lisp* l)
{
  if (is_full_lisp(s))
  {
    return;
  }

  (s -> top)++;
  s -> data[s -> top] = l;

}

lisp* pop_lisp(LispStack* s)
{
  if (is_empty_lisp(s))
  {
    return NULL;
  }

  lisp* l = s -> data[s -> top];
  (s -> top)--;
  return l;
}

bool is_empty_lisp(LispStack* s)
{
  return (s -> top == -1);
}

bool is_full_lisp(LispStack* s)
{
  return (s -> top == MAX_STACK);
}

void push_int(IntStack* s, int i)
{
  if (is_full_int(s))
  {
    return;
  }

  (s -> top)++;
  s -> data[s -> top] = i;
}

int pop_int(IntStack* s)
{
  if (is_empty_int(s))
  {
    return -1;
  }

  int i = s -> data[s -> top];
  (s -> top)--;
  return i;
}

bool is_empty_int(IntStack* s)
{
  return (s -> top == -1);
}

bool is_full_int(IntStack* s)
{
  return (s -> top == MAX_STACK);
}

void add_output(Output* o, char* str)
{
  if (o -> index == MAX_LINES - 1)
  {
    strcpy(o -> line[o -> index], "No more space in Output structure");
    o -> full = true;
    (o -> index)++;
    return;
  }
  if (o -> index >= MAX_LINES)
  {
    return;
  }

  strcpy(o -> line[o -> index], str);
  (o -> index)++;
}

//Used in debugging
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
      return t -> value;

    case LITERAL:
      return t -> value;

    case UNKNOWN:
      return t -> value;

    default:
      return "END OF FILE";
  }

  return "END OF FILE";
}

char peek(Program* p)
{
  if ((p -> index + 1) >= (int)strlen(p -> input))
  {
    return '\0';
  }

  return (p -> input[(p -> index) + 1]);
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

  //Free any leftover lisp structs on stack
  while (v -> s.top != -1)
  {
    lisp* temp = pop_lisp(&(v -> s));
    lisp_free(&temp);
  }

  free(v);
}

void free_tokens(Token** t)
{
  for (int i = 0; i < MAX_TOKENS; i++)
  {
    switch(t[i] -> type)
    {
      case STRING: case LITERAL: case VAR: case UNKNOWN:
      {
        free(t[i] -> value);
      }

      default:
      {

      }
    }
    free(t[i]);
  }
  free(t);
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
