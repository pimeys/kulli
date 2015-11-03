#include "builtin_functions.h"

lval sum(lval* x, lval* y)
{
  return lval_num(x->num + y->num);
}

lval subtract(lval* x, lval* y)
{
  return lval_num(x->num - y->num);
}

lval mul(lval* x, lval* y)
{
  return lval_num(x->num * y->num);
}

lval div(lval* x, lval* y)
{
  return y->num ? lval_num(x->num / y->num) : lval_err(LERR_DIV_ZERO);
}

lval mod(lval* x, lval* y)
{
  return y->num ? lval_num(x->num % y->num) : lval_err(LERR_DIV_ZERO);
}
