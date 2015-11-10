#pragma once

#include "mpc.h"
#include <stdint.h>

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

struct lval {
  int type;
  uint64_t num;
  char* err;
  char* sym;
  lbuiltin builtin;
  lenv* env;
  lval* formals;
  lval* body;
  int count;
  lval** cell;
};

struct lenv {
  lenv* par;
  int count;
  char** syms;
  lval** vals;
};

typedef struct lop {
  char* name;
  lval* (*fn)(const lval* x, const lval* y);
} lop;

typedef struct lfn {
  char* name;
  lval* (*fn)(lval* a);
} lfn;


lval* lval_num(uint64_t x);
lval* lval_err(const char* fmt, ...);
lval* lval_sym(const char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_read_num(const mpc_ast_t* t);
lval* lval_read(const mpc_ast_t* t);
lval* lval_add(lval* v, lval* x);
lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_join(lval* x, lval* y);
lval* lval_copy(lval* v);
void lval_del(lval* v);
void lval_expr_print(const lval *v, char open, char close);
void lval_print(const lval* v);
void lval_println(const lval* v);
lenv* lenv_new(void);
void lenv_del(lenv* e);
lval* lenv_get(lenv* e, lval* k);
void lenv_put(lenv* e, lval* k, lval* v);
void lenv_add_builtin(lenv* e, char* name, lbuiltin func);
void lenv_add_builtins(lenv* e);
char* ltype_name(int t);
lval* lval_lambda(lval* formals, lval* body);
lenv* lenv_copy(lenv* e);
void lenv_def(lenv* e, lval* k, lval* v);
lval* lval_call(lenv* e, lval* f, lval* a);
