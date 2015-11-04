#include "mpc.h"
#include "lval.h"
#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

int main(int argc, char** argv) {
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Symbol   = mpc_new("symbol");
  mpc_parser_t* Sexpr    = mpc_new("sexpr");
  mpc_parser_t* Qexpr    = mpc_new("qexpr");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Kulli    = mpc_new("kulli");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                 \
        number   : /-?[0-9]+/ ;                                         \
        symbol   : \"list\" | \"head\" | \"tail\"                       \
                 | \"join\" | \"eval\" | '+' | '-' | '*' | '/' | '%' ;  \
        sexpr    : '(' <expr>* ')' ;                                    \
        qexpr    : '{' <expr>* '}' ;                                    \
        expr     : <number> | <symbol> | <sexpr> | <qexpr> ;            \
        kulli    : /^/ <expr>* /$/ ;                                    \
      ", Number, Symbol, Sexpr, Qexpr, Expr, Kulli);

  puts("Kulli Version 0.0.0.0.1");
  puts("Press Ctrl+d to exit\n");

  while (1) {
    char* input = readline("kulli> ");
    if (!input) break;
    add_history(input);

    mpc_result_t r;

    if (mpc_parse("<stdin>", input, Kulli, &r)) {
      lval* result = lval_eval(lval_read(r.output));

      lval_println(result);
      lval_del(result);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);
  }

  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Kulli);

  return 0;
}
