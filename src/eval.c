#include "mpc.h"
#include "eval.h"
#include "lval.h"
#include "builtin_functions.h"

lop operators[] = {
  { .name = "+", .fn = &builtin_sum },
  { .name = "-", .fn = &builtin_sub },
  { .name = "*", .fn = &builtin_mul },
  { .name = "/", .fn = &builtin_div },
  { .name = "%", .fn = &builtin_mod }
};

lfn functions[] = {
  { .name = "head", .fn = &builtin_head },
  { .name = "tail", .fn = &builtin_tail },
  { .name = "list", .fn = &builtin_list },
  { .name = "eval", .fn = &builtin_eval },
  { .name = "join", .fn = &builtin_join }
};

lval* lval_eval_sexpr(lval* v) {
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }

  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) {
      return lval_take(v, i);
    }
  }

  if (v->count == 0) {
    return v;
  }

  if (v->count == 1) {
    return lval_take(v, 0);
  }

  lval* f = lval_pop(v, 0);

  if(f->type != LVAL_SYM) {
    lval_del(f);
    lval_del(v);

    return lval_err("S-expression does not start with a symbol!");
  }

  lval* result = builtin(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_SEXPR) {
    return lval_eval_sexpr(v);
  }

  return v;
}

lval* builtin(lval* a, char* func) {
  int fn_len = sizeof(functions) / sizeof(*functions);

  for (int i = 0; i < fn_len; i++) {
    lfn function = functions[i];

    if (strcmp(function.name, func) == 0) {
      return function.fn(a);
    }
  }

  return builtin_op(a, func);
}

lval* builtin_op(lval* a, char* op)
{
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);

      return lval_err("Cannot operate on non-number!");
    }
  }

  lval* x = lval_pop(a, 0);

  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;

    return x;
  }

  int op_len = sizeof(operators) / sizeof(*operators);

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);

    for (int i = 0; i < op_len; i++) {
      lop operator = operators[i];

      if (strcmp(operator.name, op) == 0) {
        x = operator.fn(x, y);
      }
    }

    lval_del(y);
  }

  lval_del(a);

  return x;
}
