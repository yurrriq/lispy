#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <editline/readline.h>
#include <mpc.h>


static const char LISPY_GRAMMAR[] = {
#include "lispy.xxd"
};


typedef enum {
    LVAL_NUM,
    LVAL_ERR
} lval_type_t;

typedef enum {
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
} lval_err_t;

typedef struct {
    lval_type_t type;
    union {
        double num;
        lval_err_t err;
    };
} lval;


lval lval_num(double num)
{
    lval val;
    val.type = LVAL_NUM;
    val.num = num;

    return val;
}


lval lval_err(lval_err_t err)
{
    lval val;
    val.type = LVAL_ERR;
    val.err = err;

    return val;
}


void lval_print(lval val)
{
    switch (val.type) {
    case LVAL_NUM:
        printf("%g", val.num);
        break;

    case LVAL_ERR:
        switch (val.err) {
        case LERR_BAD_OP:
            puts("Error: invalid operator");
            break;
        case LERR_BAD_NUM:
            puts("Error: invalid number");
            break;
        case LERR_DIV_ZERO:
            fputs("Error: division by zero", stdout);
            break;
        }
        break;
    }
}


void lval_println(lval val)
{
    lval_print(val);
    putchar('\n');
}


lval eval_binop(char *op, lval x, lval y)
{
    if (!strcmp(op, "+"))
        return lval_num(x.num + y.num);

    if (!strcmp(op, "-"))
        return lval_num(x.num - y.num);

    if (!strcmp(op, "*"))
        return lval_num(x.num * y.num);

    if (!strcmp(op, "/"))
        return !y.num ? lval_err(LERR_DIV_ZERO)
            : lval_num(x.num / y.num);

    if (!strcmp(op, "%"))
        return !y.num ? lval_err(LERR_DIV_ZERO)
            : lval_num(fmod(x.num, y.num));

    if (!strcmp(op, "^"))
        return lval_num(pow(x.num, y.num));

    return lval_err(LERR_DIV_ZERO);
}


lval eval(mpc_ast_t * ast)
{
    if (strstr(ast->tag, "number")) {
        errno = 0;
        double x = strtod(ast->contents, NULL);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);
    }

    int i = 0;

    char *op = ast->children[++i]->contents;

    lval result = eval(ast->children[++i]);

    if (!strcmp(op, "-") && ast->children_num == 4) {
        result.num = -result.num;
        return result;
    }

    while (++i < ast->children_num
           && strstr(ast->children[i]->tag, "expr"))
        result = eval_binop(op, result, eval(ast->children[i]));

    return result;
}


int main(int argc, char *argv[])
{
    mpc_parser_t *Integer = mpc_new("integer");
    mpc_parser_t *Decimal = mpc_new("decimal");
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT, LISPY_GRAMMAR,
              Integer, Decimal, Number, Operator, Expr, Lispy);

    puts("Lispy v0.9.0");
    puts("Press ctrl-c to exit\n");

    bool nonempty;
    do {
        char *input = readline("> ");
        if ((nonempty = (input && *input))) {
            add_history(input);

            mpc_result_t parsed;
            if (mpc_parse("<stdin>", input, Lispy, &parsed)) {
                mpc_ast_t *ast = parsed.output;

                lval result = eval(ast);
                lval_println(result);

                mpc_ast_delete(ast);
            } else {
                mpc_err_print(parsed.error);
                mpc_err_delete(parsed.error);
            }
        }

        free(input);
    } while (nonempty);

    mpc_cleanup(6, Integer, Decimal, Number, Operator, Expr, Lispy);

    return 0;
}
