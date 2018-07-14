#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <editline/readline.h>
#include <mpc.h>


#define LVAL_ASSERT(args, cond, err) \
    if (!(cond)) { \
        lval_del(args); \
        return lval_err(err); \
    }

#define LERR_BAD_FUNC "unknown function"
#define LERR_BAD_NUM "invalid number"
#define LERR_BAD_OP "invalid operation"
#define LERR_DIV_ZERO "division by zero"
#define LERR_BAD_SEXPR "invalid S-expression"


static const char LISPY_GRAMMAR[] = {
#include "lispy.xxd"
};


typedef enum {
    LVAL_ERR,
    LVAL_NUM,
    LVAL_QEXPR,
    LVAL_SEXPR,
    LVAL_SYM
} lval_type_t;



typedef struct lval {
    lval_type_t type;
    union {
        double num;
        char *err;
        char *sym;
    };
    int count;
    struct lval **cell;
} lval;


lval *lval_num(double num)
{
    lval *val = malloc(sizeof(lval));
    val->type = LVAL_NUM;
    val->num = num;

    return val;
}


bool lval_is_num(lval * val)
{
    return val->type == LVAL_NUM;
}


lval *lval_err(char *err)
{
    lval *val = malloc(sizeof(lval));
    val->type = LVAL_ERR;
    val->err = err;

    return val;
}


lval *lval_sym(char *s)
{
    lval *val = malloc(sizeof(lval));
    val->type = LVAL_SYM;
    val->sym = malloc(strlen(s) + 1);
    strcpy(val->sym, s);

    return val;
}


lval *lval_sexpr(void)
{
    lval *val = malloc(sizeof(lval));
    val->type = LVAL_SEXPR;
    val->count = 0;
    val->cell = NULL;

    return val;
}


lval *lval_qexpr(void)
{
    lval *val = malloc(sizeof(lval));
    val->type = LVAL_QEXPR;
    val->count = 0;
    val->cell = NULL;

    return val;
}


void lval_del(lval * val)
{
    switch (val->type) {
    case LVAL_ERR:
        free(val->err);
        break;
    case LVAL_NUM:
        break;
    case LVAL_QEXPR:
    case LVAL_SEXPR:
        for (int i = 0; i < val->count; i++)
            lval_del(val->cell[i]);
        free(val->cell);
        break;
    case LVAL_SYM:
        free(val->sym);
        break;
    }

    free(val);
}


lval *lval_add(lval * xs, lval * x)
{
    xs->count++;
    xs->cell = realloc(xs->cell, sizeof(lval *) * xs->count);
    xs->cell[xs->count - 1] = x;

    return xs;
}


lval *lval_pop(lval * xs, int i)
{
    lval *elem = xs->cell[i];

    memmove(&xs->cell[i], &xs->cell[i + 1],
            sizeof(lval *) * (xs->count - i - 1));

    xs->count--;

    xs->cell = realloc(xs->cell, sizeof(lval *) * xs->count);

    return elem;
}


lval *lval_take(lval * xs, int i)
{
    lval *elem = lval_pop(xs, i);
    lval_del(xs);

    return elem;
}


lval *lval_join(lval * xs, lval * ys)
{
    while (ys->count) {
        xs = lval_add(xs, lval_pop(ys, 0));
    }

    lval_del(ys);

    return xs;
}


void lval_print(lval * val);


void lval_expr_print(lval * expr, char open, char close)
{
    putchar(open);
    for (int i = 0; i < expr->count; i++) {
        lval_print(expr->cell[i]);
        if (i != (expr->count - 1))
            putchar(' ');
    }
    putchar(close);
}


void lval_print(lval * val)
{
    switch (val->type) {
    case LVAL_ERR:
        printf("Error: %s", val->err);
        break;
    case LVAL_NUM:
        printf("%g", val->num);
        break;
    case LVAL_QEXPR:
        lval_expr_print(val, '{', '}');
        break;
    case LVAL_SEXPR:
        lval_expr_print(val, '(', ')');
        break;
    case LVAL_SYM:
        fputs(val->sym, stdout);
        break;
    }
}


void lval_println(lval * val)
{
    lval_print(val);
    putchar('\n');
}


lval *builtin_list(lval * args)
{
    args->type = LVAL_QEXPR;
    return args;
}


lval *builtin_head(lval * args)
{
    LVAL_ASSERT(args, args->count == 1, "too many arguments for 'head'");
    LVAL_ASSERT(args, args->cell[0]->type == LVAL_QEXPR,
                "invalid argument for 'head'");
    LVAL_ASSERT(args, args->cell[0]->count,
                "cannot get 'head' of the empty list");
    lval *val = lval_take(args, 0);
    while (val->count > 1)
        lval_del(lval_pop(val, 1));
    return val;
}


lval *builtin_tail(lval * args)
{
    LVAL_ASSERT(args, args->count == 1, "too many arguments for 'tail'");
    LVAL_ASSERT(args, args->cell[0]->type == LVAL_QEXPR,
                "invalid argument for 'tail'");
    LVAL_ASSERT(args, args->cell[0]->count,
                "cannot get 'tail' of the empty list");

    lval *val = lval_take(args, 0);
    lval_del(lval_pop(val, 0));

    return val;
}


lval *builtin_join(lval * args)
{
    for (int i = 0; i < args->count; i++) {
        LVAL_ASSERT(args, args->cell[i]->type == LVAL_QEXPR,
                    "invalid argument for 'join'");
    }

    lval *res = lval_pop(args, 0);

    while (args->count) {
        res = lval_join(res, lval_pop(args, 0));
    }

    lval_del(args);

    return res;

}

lval *lval_eval(lval * val);


lval *builtin_eval(lval * args)
{
    LVAL_ASSERT(args, args->count == 1, "too many arguments for 'eval'");

    LVAL_ASSERT(args, args->cell[0]->type == LVAL_QEXPR,
                "invalid argument for 'eval'");

    lval *expr = lval_take(args, 0);
    expr->type = LVAL_SEXPR;

    return lval_eval(expr);
}


lval *builtin_op(char *op, lval * args)
{
    for (int i = 0; i < args->count; i++) {
        if (!lval_is_num(args->cell[i])) {
            lval_del(args);
            return lval_err(LERR_BAD_NUM);
        }
    }

    lval *result = lval_pop(args, 0);

    if (!strcmp(op, "-") && !args->count)
        result->num = -result->num;

    while (args->count > 0) {
        lval *y = lval_pop(args, 0);

        if (!strcmp(op, "+")) {
            result->num += y->num;
        } else if (!strcmp(op, "-")) {
            result->num -= y->num;
        } else if (!strcmp(op, "*")) {
            result->num *= y->num;
        } else if (!strcmp(op, "/")) {
            if (!y->num) {
                lval_del(result);
                lval_del(y);
                result = lval_err(LERR_DIV_ZERO);
                break;
            }
            result->num /= y->num;
        } else if (!strcmp(op, "%")) {
            if (!y->num) {
                lval_del(result);
                lval_del(y);
                result = lval_err(LERR_DIV_ZERO);
                break;
            }
            result->num = fmod(result->num, y->num);
        } else if (!strcmp(op, "^")) {
            result->num = pow(result->num, y->num);
        } else {
            lval_del(result);
            lval_del(y);
            result = lval_err(LERR_BAD_OP);
            break;
        }
        lval_del(y);
    }

    lval_del(args);

    return result;
}


lval *builtin(char *fname, lval * args)
{
    if (!strcmp("list", fname))
        return builtin_list(args);

    if (!strcmp("head", fname))
        return builtin_head(args);
    if (!strcmp("tail", fname))
        return builtin_tail(args);
    if (!strcmp("join", fname))
        return builtin_join(args);
    if (!strcmp("eval", fname))
        return builtin_eval(args);
    if (strstr("+-/*^%", fname))
        return builtin_op(fname, args);

    lval_del(args);

    return lval_err(LERR_BAD_FUNC);
}

lval *lval_eval_sexpr(lval * args)
{
    if (!args->count)
        return args;
    for (int i = 0; i < args->count; i++) {
        args->cell[i] = lval_eval(args->cell[i]);
        if (args->cell[i]->type == LVAL_ERR)
            return lval_take(args, i);
    }

    if (args->count == 1)
        return lval_take(args, 0);

    lval *car = lval_pop(args, 0);;
    if (car->type != LVAL_SYM) {
        lval_del(car);
        lval_del(args);

        return lval_err(LERR_BAD_SEXPR);
    }

    lval *result = builtin(car->sym, args);
    lval_del(car);

    return result;
}


lval *lval_eval(lval * val)
{
    if (val->type == LVAL_SEXPR)
        return lval_eval_sexpr(val);

    return val;
}


lval *lval_read_num(mpc_ast_t * ast)
{
    errno = 0;
    double num = strtod(ast->contents, NULL);
    return errno != ERANGE ? lval_num(num) : lval_err(LERR_BAD_NUM);
}


lval *lval_read(mpc_ast_t * ast)
{
    if (strstr(ast->tag, "number"))
        return lval_read_num(ast);

    if (strstr(ast->tag, "symbol"))
        return lval_sym(ast->contents);

    lval *val = NULL;
    if (!strcmp(ast->tag, ">"))
        val = lval_sexpr();
    if (strstr(ast->tag, "qexpr"))
        val = lval_qexpr();
    if (strstr(ast->tag, "sexpr"))
        val = lval_sexpr();

    for (int i = 0; i < ast->children_num; i++) {
        if (!strcmp(ast->children[i]->contents, "("))
            continue;
        if (!strcmp(ast->children[i]->contents, ")"))
            continue;
        if (!strcmp(ast->children[i]->contents, "{"))
            continue;
        if (!strcmp(ast->children[i]->contents, "}"))
            continue;
        if (!strcmp(ast->children[i]->tag, "regex"))
            continue;
        val = lval_add(val, lval_read(ast->children[i]));
    }

    return val;
}


int main(int argc, char *argv[])
{
    mpc_parser_t *Number = mpc_new("number");
    mpc_parser_t *Symbol = mpc_new("symbol");
    mpc_parser_t *Sexpr = mpc_new("sexpr");
    mpc_parser_t *Qexpr = mpc_new("qexpr");
    mpc_parser_t *Expr = mpc_new("expr");
    mpc_parser_t *Lispy = mpc_new("lispy");

    mpc_err_t *err = mpca_lang(MPCA_LANG_PREDICTIVE, LISPY_GRAMMAR,
                               Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    if (err != NULL) {
        puts(LISPY_GRAMMAR);
        mpc_err_print(err);
        mpc_err_delete(err);
        exit(100);
    }

    puts("Lispy v1.4.0");
    puts("Press ctrl-c to exit\n");

    bool nonempty;
    do {
        char *input = readline("> ");
        if ((nonempty = (input && *input))) {
            add_history(input);

            mpc_result_t parsed;
            if (mpc_parse("<stdin>", input, Lispy, &parsed)) {
                mpc_ast_t *ast = parsed.output;

                lval *result = lval_eval(lval_read(ast));
                lval_println(result);

                mpc_ast_delete(ast);
            } else {
                mpc_err_print(parsed.error);
                mpc_err_delete(parsed.error);
            }
        }

        free(input);
    } while (nonempty);

    mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, Lispy);

    return 0;
}
