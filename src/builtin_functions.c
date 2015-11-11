#define LCHECK(args, cond, fmt, ...)                                    \
  do { if (!(cond)) { lval* err = lval_err(fmt, ##__VA_ARGS__); lval_del(args); return err; } } while (0);

#define LCHECK_TYPE(func, args, index, expect) \
  LCHECK(args, args->cell[index]->type == expect,                       \
         "Function '%s' passed incorrect type for argument %i. Got %s, expected %s.", \
         func, index, ltype_name(args->cell[index]->type), ltype_name(expect))

#define LCHECK_NUM(func, args, num)             \
  LCHECK(args, args->count == num,              \
         "Function '%s' passed incorrect number of arguments. Got %i, expected %i.", \
         func, args->count, num)

#define LCHECK_NOT_EMPTY(func, args, index)     \
  LCHECK(args, args->cell[index]->count != 0,   \
         "Function '%s' passed {} for argument %i.", func, index);

#include "builtin_functions.h"
#include "lval.h"
#include "eval.h"

lval* builtin_sum(lenv* e, lval* a) {
  return builtin_op(e, a, "+");
}

lval* builtin_sub(lenv* e, lval* a) {
  return builtin_op(e, a, "-");
}

lval* builtin_mul(lenv* e, lval* a) {
  return builtin_op(e, a, "*");
}

lval* builtin_div(lenv* e, lval* a) {
  return builtin_op(e, a, "/");
}

lval* builtin_mod(lenv* e, lval* a) {
  return builtin_op(e, a, "%");
}

lval* builtin_head(lenv* e, lval* a) {
  LCHECK_NUM("head", a, 1);
  LCHECK_TYPE("head", a, 0, LVAL_QEXPR);
  LCHECK_NOT_EMPTY("head", a, 0);

  lval* v = lval_take(a, 0);
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lenv* e, lval* a) {
  LCHECK_NUM("tail", a, 1);
  LCHECK_TYPE("tail", a, 0, LVAL_QEXPR);
  LCHECK_NOT_EMPTY("tail", a, 0);

  lval* v = lval_take(a, 0);
  lval_del(lval_pop(v, 0));

  return v;
}

lval* builtin_list(lenv* e, lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lenv* e, lval* a) {
  LCHECK_NUM("eval", a, 1);
  LCHECK_TYPE("eval", a, 0, LVAL_QEXPR)

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(e, x);
}

lval* builtin_join(lenv* e, lval* a) {
  for (int i = 0; i < a->count; i++) {
    LCHECK_TYPE("join", a, i, LVAL_QEXPR)
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);

  return x;
}

lval* builtin_op(lenv* e, lval* a, char* op) {
  for (int i = 0; i < a->count; i++) {
    LCHECK_TYPE(op, a, i, LVAL_NUM);
  }

  lval* x = lval_pop(a, 0);

  if ((strcmp(op, "-") == 0) && a->count == 0) {
    x->num = -x->num;
  }

  while (a->count > 0) {
    lval* y = lval_pop(a, 0);

    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division by Zero!"); break;
      }

      x->num /= y->num;
    }
    if (strcmp(op, "%") == 0) {
      if (y->num == 0) {
        lval_del(x); lval_del(y);
        x = lval_err("Division by Zero!"); break;
      }

      x->num %= y->num;
    }

    lval_del(y);
  }

  lval_del(a); return x;
}

lval* builtin_def(lenv* e, lval* a) {
  return builtin_var(e, a, "def");
}

lval* builtin_put(lenv* e, lval* a) {
  return builtin_var(e, a, "=");
}

lval* builtin_var(lenv* e, lval* a, char* func) {
  LCHECK_TYPE(func, a, 0, LVAL_QEXPR);

  lval* syms = a->cell[0];

  for (int i = 0; i < syms->count; i++) {
    LCHECK(a, (syms->cell[i]->type == LVAL_SYM),
           "Function '%s' cannot define non-symbol."
           "Got %s, expected %s.", func,
           ltype_name(syms->cell[i]->type),
           ltype_name(LVAL_SYM));
  }

  LCHECK(a, syms->count == a->count-1,
         "Function '%s' passed too many arguments for symbols. "
         "Got %i, expected %i.", func, syms->count, a->count-1);

  for (int i = 0; i < syms->count; i++) {
    if (strcmp(func, "def") == 0) {
      lenv_def(e, syms->cell[i], a->cell[i+1]);
    }

    if (strcmp(func, "=") == 0) {
      lenv_put(e, syms->cell[i], a->cell[i+1]);
    }
  }

  lval_del(a);

  return lval_sexpr();
}

lval* builtin_lambda(lenv* e, lval* a) {
  LCHECK_NUM("\\", a, 2);
  LCHECK_TYPE("\\", a, 0, LVAL_QEXPR);
  LCHECK_TYPE("\\", a, 1, LVAL_QEXPR);

  for (int i = 0; i < a->cell[0]->count; i++) {
    LCHECK(a, (a->cell[0]->cell[i]->type == LVAL_SYM),
           "Cannot define non-symbol. Got %s, expected %s.",
           ltype_name(a->cell[0]->cell[i]->type), ltype_name(LVAL_SYM));
  }

  lval* formals = lval_pop(a, 0);
  lval* body = lval_pop(a, 0);
  lval_del(a);

  return lval_lambda(formals, body);
}

lval* builtin_gt(lenv* e, lval* a) {
  return builtin_ord(e, a, ">");
}

lval* builtin_lt(lenv* e, lval* a) {
  return builtin_ord(e, a, "<");
}

lval* builtin_ge(lenv* e, lval* a) {
  return builtin_ord(e, a, ">=");
}

lval* builtin_le(lenv* e, lval* a) {
  return builtin_ord(e, a, "<=");
}

lval* builtin_ord(lenv* e, lval* a, char* op) {
  LCHECK_NUM(op, a, 2);
  LCHECK_TYPE(op, a, 0, LVAL_NUM);
  LCHECK_TYPE(op, a, 1, LVAL_NUM);

  int r;
  if (strcmp(op, ">") == 0) {
    r = (a->cell[0]->num > a->cell[1]->num);
  }
  if (strcmp(op, "<") == 0) {
    r = (a->cell[0]->num < a->cell[1]->num);
  }
  if (strcmp(op, ">=") == 0) {
    r = (a->cell[0]->num >= a->cell[1]->num);
  }
  if (strcmp(op, "<=") == 0) {
    r = (a->cell[0]->num <= a->cell[1]->num);
  }

  lval_del(a);
  return lval_num(r);
}

lval* builtin_cmp(lenv* e, lval* a, char* op) {
  LCHECK_NUM(op, a, 2);

  int r;
  if (strcmp(op, "==") == 0) {
    r = lval_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lval_eq(a->cell[0], a->cell[1]);
  }
  lval_del(a);

  return lval_num(r);
}

lval* builtin_eq(lenv* e, lval* a) {
  return builtin_cmp(e, a, "==");
}

lval* builtin_ne(lenv* e, lval* a) {
  return builtin_cmp(e, a, "!=");
}

lval* builtin_if(lenv* e, lval* a) {
  LCHECK_NUM("if", a, 3);
  LCHECK_TYPE("if", a, 0, LVAL_NUM);
  LCHECK_TYPE("if", a, 1, LVAL_QEXPR);
  LCHECK_TYPE("if", a, 2, LVAL_QEXPR);

  lval* x;
  a->cell[1]->type = LVAL_SEXPR;
  a->cell[2]->type = LVAL_SEXPR;

  if (a->cell[0]->num) {
    x = lval_eval(e, lval_pop(a, 1));
  } else {
    x = lval_eval(e, lval_pop(a, 2));
  }

  lval_del(a);
  return x;
}

lval* builtin_load(lenv* e, lval* a, mpc_parser_t* p) {
  LCHECK_NUM("load", a, 1);
  LCHECK_TYPE("load", a, 0, LVAL_STR);

  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, p, &r)) {
    lval* expr = lval_read(r.output);
    mpc_ast_delete(r.output);

    while (expr->count) {
      lval* x = lval_eval(e, lval_pop(expr, 0));
      if (x->type == LVAL_ERR) { lval_println(x); }
      lval_del(x);
    }

    lval_del(expr);
    lval_del(a);

    return lval_sexpr();
  } else {
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);

    lval* err = lval_err("Could not load library %s", err_msg);
    free(err_msg);
    lval_del(a);

    return err;
  }
}
