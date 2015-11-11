#include "mpc.h"
#include "lval.h"
#include "builtin_functions.h"
#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

void load_lib(char* f, mpc_parser_t* p, lenv* e) {
  lval* args = lval_add(lval_sexpr(), lval_str(f));
  lval* x = builtin_load(e, args, p);

  if (x->type == LVAL_ERR) {
    lval_println(x);
  }

  lval_del(x);
}

int main(int argc, char** argv) {
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Symbol   = mpc_new("symbol");
  mpc_parser_t* Sexpr    = mpc_new("sexpr");
  mpc_parser_t* Qexpr    = mpc_new("qexpr");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* String   = mpc_new("string");
  mpc_parser_t* Comment  = mpc_new("comment");
  mpc_parser_t* Kulli    = mpc_new("kulli");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                             \
        number   : /-?[0-9]+/ ;                                                     \
        symbol   : /[a-zA-Z0-9_+%\\-*\\/\\\\=<>!&]+/ ;                              \
        comment  : /;[^\\r\\n]*/ ;                                                  \
        string   : /\"(\\\\.|[^\"])*\"/ ;                                           \
        sexpr    : '(' <expr>* ')' ;                                                \
        qexpr    : '{' <expr>* '}' ;                                                \
        expr     : <number> | <symbol> | <string> | <sexpr> | <qexpr> | <comment> ; \
        kulli    : /^/ <expr>* /$/ ;                                                \
      ", Number, Symbol, Comment, String, Sexpr, Qexpr, Expr, Kulli);

  lenv* e = lenv_new();
  lenv_add_builtins(e);
  load_lib("./lib/std.kulli", Kulli, e);

  if (argc == 1) {
    puts("Kulli Version 0.0.0.0.1");
    puts("Press Ctrl+d to exit\n");

    while (1) {
      char* input = readline("kulli> ");
      if (!input) break;
      add_history(input);

      mpc_result_t r;

      if (mpc_parse("<stdin>", input, Kulli, &r)) {
        lval* result = lval_eval(e, lval_read(r.output));

        lval_println(result);
        lval_del(result);

        mpc_ast_delete(r.output);
      } else {
        mpc_err_print(r.error);
        mpc_err_delete(r.error);
      }

      free(input);
    }
  }

  if (argc >= 2) {
    for (int i = 1; i < argc; i++) {
      load_lib(argv[i], Kulli, e);
    }
  }

  lenv_del(e);

  mpc_cleanup(8, Number, Symbol, Comment, String, Sexpr, Qexpr, Expr, Kulli);

  return 0;
}
