#pragma once

#include "mpc.h"
#include <stdint.h>

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

typedef struct lval {
  int type;
  uint64_t num;
  char* err;
  char* sym;
  int count;
  struct lval** cell;
} lval;

typedef struct lop {
  char* name;
  lval* (*fn)(const lval* x, const lval* y);
} lop;

typedef struct lfn {
  char* name;
  lval* (*fn)(lval* a);
} lfn;

lval* lval_num(uint64_t x);
lval* lval_err(const char* m);
lval* lval_sym(const char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_read_num(const mpc_ast_t* t);
lval* lval_read(const mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_join(lval* x, lval* y);
void lval_del(lval* v);
void lval_expr_print(const lval *v, char open, char close);
void lval_print(const lval* v);
void lval_println(const lval* v);
