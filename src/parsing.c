#include "mpc.h"
#include "lval.h"
#include "builtin_functions.h"

lfn operators[] = {
  { .name = "+", .fn = &lfn_sum },
  { .name = "-", .fn = &lfn_sub },
  { .name = "*", .fn = &lfn_mul },
  { .name = "/", .fn = &lfn_div },
  { .name = "%", .fn = &lfn_mod }
};

lval eval_op(const lval* x, const char* op, const lval* y) {
  if (x->type == LVAL_ERR) { return *x; }
  if (y->type == LVAL_ERR) { return *y; }

  int op_len = sizeof(operators) / sizeof(*operators);

  for (int i = 0; i < op_len; i++) {
    lfn fn = operators[i];

    if (strcmp(fn.name, op) == 0) {
      return fn.fn(x, y);
    }
  }

  return lval_err(LERR_BAD_OP);
}

lval eval(const mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
  }

  lval x = eval(t->children[2]);
  int i = 3;

  while (strstr(t->children[i]->tag, "expr")) {
    lval y = eval(t->children[i]);

    x = eval_op(&x, t->children[1]->contents, &y);
    i++;
  }

  return x;
}
