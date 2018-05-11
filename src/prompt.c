#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
bool eval(char *input)
{
    if (input && *input) {
        add_history(input);
        printf("< %s\n", input);
    }
    // N.B. This is a no-op when !input.
    free(input);

    return (bool) input;
}

int main(int argc, char *argv[])
{
    puts("Lispy v0.0.1");
    puts("Press ctrl-c to exit\n");

    bool keep_going = true;
    while (keep_going) {
        keep_going = eval(readline("> "));
    }

    return 0;
}
