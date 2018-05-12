#define LISPY_GRAMMAR \
        " digit    : /[0-9]/ ;                               " \
        " integer  : /-?/ <digit>+ ;                         " \
        " decimal  : /-?/ <digit>+ '.' <digit>+ ;            " \
        " number   : <decimal> | <integer> ;                 " \
        " operator : '+' | '-' | '*' | '/' ;                 " \
        " expr     : <number> | '(' <operator> <expr>+ ')' ; " \
        " lispy    : /^/ <expr>+ /$/ ;                       "
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <mpc.h>




int main(int argc, char *argv[])
{
    mpc_parser_t *Digit = mpc_new("digit");
    mpc_parser_t *Integer = mpc_new("integer");
    mpc_parser_t *Decimal = mpc_new("decimal");
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Operator = mpc_new("operator");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT, LISPY_GRAMMAR,
              Digit, Integer, Decimal, Number, Operator, Expr, Lispy);

    puts("Lispy v0.0.1");
    puts("Press ctrl-c to exit\n");

    bool nonempty;
    do {
        char *input = readline("> ");
        if ((nonempty = (input && *input))) {
            add_history(input);
            mpc_result_t res;
            if (mpc_parse("<stdin>", input, Lispy, &res)) {
                mpc_ast_print(res.output);
                mpc_ast_delete(res.output);
            } else {
                mpc_err_print(res.error);
                mpc_err_delete(res.error);
            }
        }

        free(input);            // N.B. This is a no-op when !input.
    } while (nonempty);

    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}
