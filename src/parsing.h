#pragma once

#include "lval.h"

int main(int argc, char** argv);
lval eval_op(const lval* x, const char* op, const lval* y);
lval eval(const mpc_ast_t* t);
