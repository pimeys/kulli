#pragma once

#include "lval.h"

int main(int argc, char** argv);
lval eval_op(lval x, char* op, lval y);
lval eval(mpc_ast_t* t);
