#pragma once

enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct {
  int type;
  long num;
  int err;
} lval;

typedef struct {
  char* name;
  lval (*fn)(lval* x, lval* y);
} lfn;

lval lval_num(long x);
lval lval_err(int x);
void lval_print(lval v);
void lval_println(lval v);
