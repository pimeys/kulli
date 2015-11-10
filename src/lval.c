#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "lval.h"
#include "builtin_functions.h"

lval* lval_num(uint64_t x) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(const char* fmt, ...) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;

  va_list va;
  va_start(va, fmt);

  v->err = malloc(512);
  vsnprintf(v->err, 511, fmt, va);

  v->err = realloc(v->err, strlen(v->err)+1);

  va_end(va);

  return v;
}

lval* lval_sym(const char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lval* lval_qexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lval* lval_fun(lbuiltin func) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_FUN;
  v->fun = func;
  return v;
}

void lval_del(lval* v) {
  switch (v->type) {
  case LVAL_NUM: break;
  case LVAL_FUN: break;
  case LVAL_ERR: free(v->err); break;
  case LVAL_SYM: free(v->sym); break;
  case LVAL_QEXPR:
  case LVAL_SEXPR:
    for (int i = 0; i < v->count; i++) {
      lval_del(v->cell[i]);
    }

    free(v->cell);
    break;
  }

  free(v);
}

lval* lval_read_num(const mpc_ast_t* t) {
  errno = 0;
  uint64_t x = strtol(t->contents, NULL, 10);

  return errno != ERANGE
    ? lval_num(x)
    : lval_err("invalid number");
}

lval* lval_read(const mpc_ast_t* t) {
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

  lval* x = NULL;
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
  if (strstr(t->tag, "qexpr")) { x = lval_qexpr(); }

  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "}") == 0) { continue; }
    if (strcmp(t->children[i]->contents, "{") == 0) { continue; }
    if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }

  return x;
}

lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count - 1] = x;
  return v;
}

lval* lval_pop(lval* v, int i) {
  lval* x = v->cell[i];

  memmove(&v->cell[i], &v->cell[i + 1], sizeof(v->cell[i]) * (v->count - i - 1));

  v->count--;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);

  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);

  return x;
}

lval* lval_join(lval* x, lval* y) {
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  lval_del(y);

  return x;
}

lval* lval_copy(lval* v) {
  lval* x = malloc(sizeof(lval));
  x->type = v->type;

  switch (v->type) {
  case LVAL_FUN: x->fun = v->fun; break;
  case LVAL_NUM: x->num = v->num; break;
  case LVAL_ERR:
    x->err = malloc(strlen(v->err) + 1);
    strcpy(x->err, v->err); break;
  case LVAL_SYM:
    x->sym = malloc(strlen(v->sym) + 1);
    strcpy(x->sym, v->sym); break;
  case LVAL_SEXPR:
  case LVAL_QEXPR:
    x->count = v->count;
    x->cell = malloc(sizeof(lval*) * x->count);

    for (int i = 0; i < x->count; i++) {
      x->cell[i] = lval_copy(v->cell[i]);
    }

    break;
  }

  return x;
}

void lval_expr_print(const lval *v, char open, char close) {
  putchar(open);

  for (int i = 0; i < v-> count; i++) {
    lval_print(v->cell[i]);

    if (i != (v->count - 1)) {
      putchar(' ');
    }
  }

  putchar(close);
}

void lval_print(const lval* v) {
  switch (v->type) {
  case LVAL_NUM:
    printf("%li", v->num); break;
  case LVAL_ERR:
    printf("Error: %s", v->err); break;
  case LVAL_SYM:
    printf("%s", v->sym); break;
  case LVAL_SEXPR:
    lval_expr_print(v, '(', ')'); break;
  case LVAL_QEXPR:
    lval_expr_print(v, '{', '}'); break;
  case LVAL_FUN:
    printf("<function>"); break;
  }
}

void lval_println(const lval* v) {
  lval_print(v);
  putchar('\n');
}


void lenv_add_builtin(lenv* e, char* name, lbuiltin func) {
  lval* k = lval_sym(name);
  lval* v = lval_fun(func);
  lenv_put(e, k, v);
  lval_del(k); lval_del(v);
}

void lenv_add_builtins(lenv* e) {
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "+", builtin_sum);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);
}

lenv* lenv_new(void) {
  lenv* e = malloc(sizeof(lenv));
  e->count = 0;
  e->syms = NULL;
  e->vals = NULL;
  return e;
}

void lenv_del(lenv* e) {
  for (int i = 0; i < e->count; i++) {
    free(e->syms[i]);
    lval_del(e->vals[i]);
  }
  free(e->syms);
  free(e->vals);
  free(e);
}

lval* lenv_get(lenv* e, lval* k) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      return lval_copy(e->vals[i]);
    }
  }

  return lval_err("Unbound Synmbol '%s'", k->sym);
}

void lenv_put(lenv* e, lval* k, lval* v) {
  for (int i = 0; i < e->count; i++) {
    if (strcmp(e->syms[i], k->sym) == 0) {
      lval_del(e->vals[i]);
      e->vals[i] = lval_copy(v);
      return;
    }
  }

  e->count++;
  e->vals = realloc(e->vals, sizeof(lval*) * e->count);
  e->syms = realloc(e->syms, sizeof(char*) * e->count);

  e->vals[e->count-1] = lval_copy(v);
  e->syms[e->count-1] = malloc(strlen(k->sym)+1);
  strcpy(e->syms[e->count-1], k->sym);
}

char* ltype_name(int t) {
  switch(t) {
  case LVAL_FUN:
    return "Function";
  case LVAL_NUM:
    return "Number";
  case LVAL_ERR:
    return "Error";
  case LVAL_SYM:
    return "Symbol";
  case LVAL_SEXPR:
    return "S-Expression";
  case LVAL_QEXPR:
    return "Q-Expression";
  default:
    return "Unknown";
  }
}
