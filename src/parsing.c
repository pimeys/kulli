#include "mpc.h"

#include "parsing.h"
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

int main(int argc, char** argv) {
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Kulli    = mpc_new("kulli");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                         \
        number   : /-?[0-9]+/ ;                                                 \
        operator : '+' | '-' | '*' | '/' | '%' ;                                \
        expr     : <number> | '(' <operator> <expr>+ ')' ;                      \
        kulli    : /^/ <operator> <expr>+ /$/ ;                                 \
      ",
      Number, Operator, Expr, Kulli);

  puts("Kulli Version 0.0.0.0.1");
  puts("Press Ctrl+d to exit\n");

  while (1) {
    char* input = readline("kulli> ");
    if (!input) break;
    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Kulli, &r)) {
      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }


    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, Kulli);

  return 0;
}

lval eval_op(lval x, char* op, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num); }
  if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num); }
  if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num); }
  if (strcmp(op, "/") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num / y.num);
  }
  if (strcmp(op, "%") == 0) {
    return y.num == 0
      ? lval_err(LERR_DIV_ZERO)
      : lval_num(x.num % y.num);
  }
  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    return lval_num(atoi(t->contents));
  }

  char* op = t->children[1]->contents;
  lval x = eval(t->children[2]);

  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }

  return x;
}

lval lval_num(long x) {
  lval v;
  v.type = LVAL_NUM;
  v.num = x;
  return v;
}

lval lval_err(int x) {
  lval v;
  v.type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.type) {
  case LVAL_NUM:
    printf("%li", v.num); break;
  case LVAL_ERR:
    switch (v.err) {
    case LERR_DIV_ZERO:
      printf("Error: Division By Zero!"); break;
    case LERR_BAD_OP:
      printf("Error: Invalid Operator!"); break;
    case LERR_BAD_NUM:
      printf("Error: Invalid Number!"); break;
    }

    break;
  }
}

void lval_println(lval v) {
  lval_print(v);
  putchar('\n');
}
