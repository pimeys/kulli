#pragma once

#include "mpc.h"

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct lval {
  int type;
  long num;
  char* err;
  char* sym;
  int count;
  struct lval** cell;
} lval;

typedef struct lfn {
  char* name;
  lval* (*fn)(const lval* x, const lval* y);
} lfn;

lval* lval_num(long x);
lval* lval_err(const char* m);
lval* lval_sym(const char* s);
lval* lval_sexpr(void);
lval* lval_read_num(const mpc_ast_t* t);
lval* lval_read(const mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
void lval_expr_print(const lval *v, char open, char close);
void lval_del(lval* v);
void lval_print(const lval* v);
void lval_println(const lval* v);
