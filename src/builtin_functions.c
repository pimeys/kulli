#define LCHECK(args, cond, fmt, ...)                                    \
  do { if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; } } while (0);

#define LCHECK_TYPE(func, args, index, expect) \
  LCHECK(args, args->cell[index]->type == expect,                       \
         "Function '%s' passed incorrect type for argument %i. Got %s, expected %s.", \
         func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LCHECK_NUM(func, args, num)             \
  LCHECK(args, args->count == num,              \
         "Function '%s' passed incorrect number of arguments. Got %i, expected %i.", \
         func, args->count, num)

#define LCHECK_NOT_EMPTY(func, args, index)     \
  LCHECK(args, args->cell[index]->count != 0,   \
         "Function '%s' passed {} for argument %i.", func, index);

#include "builtin_functions.h"
#include "lval.h"
#include "eval.h"

lval* builtin_sum(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_mod(lenv* e, lval* a) {
  return builtin_op(e, a, "%");
}

lval* builtin_head(lenv* e, lval* a) {
  LCHECK_NUM("head", a, 1);
  LCHECK_TYPE("head", a, 0, LVAL_QEXPR);
  LCHECK_NOT_EMPTY("head", a, 0);

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LCHECK_NUM("tail", a, 1);
  LCHECK_TYPE("tail", a, 0, LVAL_QEXPR);
  LCHECK_NOT_EMPTY("tail", a, 0);

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));

  return v;
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lenv* e, lval* a) {
  LCHECK_NUM("eval", a, 1);
  LCHECK_TYPE("eval", a, 0, LVAL_QEXPR)

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    LCHECK_TYPE("join", a, i, LVAL_QEXPR)
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);

  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    LCHECK_TYPE(op, a, i, LVAL_NUM);
  }

  lval* x = lval_pop(a, 0);

  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);

    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division by Zero!"); break;
      }

      x->num /= y->num;
    }
    if (strcmp(op, "%") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division by Zero!"); break;
      }

      x->num %= y->num;
    }

    lval_del(y);
  }

  lval_del(a); return x;
}

lval* builtin_def(lenv* e, lval* a) {
  LCHECK(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'def' passed incorrect type!");

  lval* syms = a->cell[0];

  for (int i = 0; i < syms->count; i++) {
    LCHECK(a, syms->cell[i]->type == LVAL_SYM,
           "Function 'def' cannot define non-symbol");
  }

  LCHECK(a, syms->count == a->count-1,
         "Function 'def' cannot define incorrect "
         "number of values to symbols");

  for (int i = 0; i < syms->count; i++) {
    lenv_put(e, syms->cell[i], a->cell[i+1]);
  }

  lval_del(a);

  return lval_sexpr();
}
