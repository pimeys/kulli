#define LCHECK(args, cond, err)                \
  do { if (!(cond)) { lval_del(args); return lval_err(err); } } while (0);

#include "builtin_functions.h"
#include "lval.h"
#include "eval.h"

lval* builtin_sum(const lval* x, const lval* y)
{
  return lval_num(x->num + y->num);
}

lval* builtin_sub(const lval* x, const lval* y)
{
  return lval_num(x->num - y->num);
}

lval* builtin_mul(const lval* x, const lval* y)
{
  return lval_num(x->num * y->num);
}

lval* builtin_div(const lval* x, const lval* y)
{
  return y->num ? lval_num(x->num / y->num) : lval_err("Division by zero!");
}

lval* builtin_mod(const lval* x, const lval* y)
{
  return y->num ? lval_num(x->num % y->num) : lval_err("Division by zero!");
}

lval* builtin_head(lval* a) {
  LCHECK(a, a->count == 1,
          "Function 'head' passed too many arguments!");
  LCHECK(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'head' passed incorrect type!");
  LCHECK(a, a->cell[0]->count > 0,
          "Function 'head' cannot operate on empty list!")

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  LCHECK(a, a->count == 1,
          "Function 'head' passed too many arguments!");
  LCHECK(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'head' passed incorrect type!");
  LCHECK(a, a->cell[0]->count > 0,
          "Function 'head' cannot operate on empty list!")

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));

  return v;
}

lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lval* a) {
  LCHECK(a, a->count == 1,
          "Function 'head' passed too many arguments!");
  LCHECK(a, a->cell[0]->type == LVAL_QEXPR,
          "Function 'head' passed incorrect type!");

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(x);
}

lval* builtin_join(lval* a) {
  for (int i = 0; i < a->count; i++) {
    LCHECK(a, a->cell[i]->type == LVAL_QEXPR,
            "Function 'join' passed incorrect type.");
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);

  return x;
}
