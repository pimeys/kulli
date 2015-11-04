#include "builtin_functions.h"
#include "lval.h"

lval* lfn_sum(const lval* x, const lval* y)
{
  return lval_num(x->num + y->num);
}

lval* lfn_sub(const lval* x, const lval* y)
{
  return lval_num(x->num - y->num);
}

lval* lfn_mul(const lval* x, const lval* y)
{
  return lval_num(x->num * y->num);
}

lval* lfn_div(const lval* x, const lval* y)
{
  return y->num ? lval_num(x->num / y->num) : lval_err("ERROR: Division by zero!");
}

lval* lfn_mod(const lval* x, const lval* y)
{
  return y->num ? lval_num(x->num % y->num) : lval_err("ERROR: Division by zero!");
}
