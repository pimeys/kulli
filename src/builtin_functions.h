#pragma once

#include "lval.h"

lval* builtin_sum(const lval* x, const lval* y);
lval* builtin_sub(const lval* x, const lval* y);
lval* builtin_mul(const lval* x, const lval* y);
lval* builtin_div(const lval* x, const lval* y);
lval* builtin_mod(const lval* x, const lval* y);
lval* builtin_head(lval* a);
lval* builtin_tail(lval* a);
lval* builtin_list(lval* a);
lval* builtin_eval(lval* a);
lval* builtin_join(lval* a);
