#pragma once

#include "lval.h"

lval lfn_sum(const lval* x, const lval* y);
lval lfn_sub(const lval* x, const lval* y);
lval lfn_mul(const lval* x, const lval* y);
lval lfn_div(const lval* x, const lval* y);
lval lfn_mod(const lval* x, const lval* y);
