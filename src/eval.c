#include "mpc.h"
#include "eval.h"
#include "lval.h"
#include "builtin_functions.h"

lfn operators[] = {
  { .name = "+", .fn = &lfn_sum },
  { .name = "-", .fn = &lfn_sub },
  { .name = "*", .fn = &lfn_mul },
  { .name = "/", .fn = &lfn_div },
  { .name = "%", .fn = &lfn_mod }
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

    return lval_err("ERROR: S-expression does not start with a symbol!");
  }

  lval* result = builtin_op(v, f->sym);
  lval_del(f);

  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_SEXPR) {
    return lval_eval_sexpr(v);
  }

  return v;
}

lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];

  memmove(&v->cell[i], &v->cell[i + 1], sizeof(lval*) * (v->count - i - 1));

  v->count--;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);

  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);

  return x;
}

lval* builtin_op(lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);

      return lval_err("ERROR: Cannot operate on non-number!");
    }
  }

  lval* x = lval_pop(a, 0);

  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  int op_len = sizeof(operators) / sizeof(*operators);

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);

    for (int i = 0; i < op_len; i++) {
      lfn fn = operators[i];

      if (strcmp(fn.name, op) == 0) {
        x = fn.fn(x, y);
      }
    }

    lval_del(y);
  }

  lval_del(a);

  return x;
}
