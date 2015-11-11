#pragma once

#include "lval.h"

lval* builtin_sum(lenv* e, lval* a);
lval* builtin_sub(lenv* e, lval* a);
lval* builtin_mul(lenv* e, lval* a);
lval* builtin_div(lenv* e, lval* a);
lval* builtin_mod(lenv* e, lval* a);
lval* builtin_head(lenv* e, lval* a);
lval* builtin_tail(lenv* e, lval* a);
lval* builtin_list(lenv* e, lval* a);
lval* builtin_eval(lenv* e, lval* a);
lval* builtin_join(lenv* e, lval* a);
lval* builtin_op(lenv* e, lval* a, char* op);
lval* builtin_def(lenv* e, lval* a);
lval* builtin_put(lenv* e, lval* a);
lval* builtin_lambda(lenv* e, lval* a);
lval* builtin_var(lenv* e, lval* a, char* func);
lval* builtin_gt(lenv* e, lval* a);
lval* builtin_lt(lenv* e, lval* a);
lval* builtin_ge(lenv* e, lval* a);
lval* builtin_le(lenv* e, lval* a);
lval* builtin_ord(lenv* e, lval* a, char* op);
lval* builtin_cmp(lenv* e, lval* a, char* op);
lval* builtin_eq(lenv* e, lval* a);
lval* builtin_ne(lenv* e, lval* a);
lval* builtin_if(lenv* e, lval* a);
lval* builtin_load(lenv* e, lval* a, mpc_parser_t* p);
