#include "lval.h"
#include "builtin_functions.h"

struct {
  const char* name;
  lval (*fn) (lval x, lval y);
} lfn;

lfn operators[] = {
  { .name = "+", .fn = &sum },
  { .name = "-", .fn = &subtract },
  { .name = "*", .fn = &mul },
  { .name = "/", .fn = &div },
  { .name = "%", .fn = &mod }
}

lval eval_op(lval x, char* op, lval y) {
  if (x.type == LVAL_ERR) { return x; }
  if (y.type == LVAL_ERR) { return y; }

  int op_len = sizeof(operators) / sizeof(struct lfn)

  for (int i = 0; i < op_len; i++) {
    lfn op = operators[i];

    if (strcmp(op.name, op) == 0) {
      return op.fn(x, y);
    }
  }

  return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x= strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
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
