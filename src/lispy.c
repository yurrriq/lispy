#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <editline/readline.h>
#include <mpc.h>


static const char LISPY_GRAMMAR[] = {
#include "lispy.xxd"
};


double eval_binop(char *op, double x, double y)
{
    if (!strcmp(op, "+"))
        return x + y;

    if (!strcmp(op, "-"))
        return x - y;

    if (!strcmp(op, "*"))
        return x * y;

    if (!strcmp(op, "/"))
        return x / y;

    return 0;
}


double eval(mpc_ast_t * ast)
{
    if (strstr(ast->tag, "number"))
        return atof(ast->contents);

    int i = 0;

    char *op = ast->children[++i]->contents;

    double result = eval(ast->children[++i]);

    if (!strcmp(op, "-") && ast->children_num == 4)
        return -result;

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

    puts("Lispy v0.7.0");
    puts("Press ctrl-c to exit\n");

    bool nonempty;
    do {
        char *input = readline("> ");
        if ((nonempty = (input && *input))) {
            add_history(input);

            mpc_result_t parsed;
            if (mpc_parse("<stdin>", input, Lispy, &parsed)) {
                mpc_ast_t *ast = parsed.output;

                double result = eval(ast);
                printf("%g\n", result);

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
