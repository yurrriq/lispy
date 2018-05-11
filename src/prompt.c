#include <stdio.h>

#include <editline/readline.h>


#define INPUT_SIZE 2048

static char input[INPUT_SIZE];

char *read(const char *prompt)
{
    fputs(prompt, stdout);
    return fgets(input, INPUT_SIZE, stdin);
}

void eval()
{
    printf("< %s", input);
}

int main(int argc, char *argv[])
{
    puts("Lispy v0.0.1");
    puts("Press ctrl-c to exit\n");

    while (read("> ") != NULL) {
        eval();
    }


    return 0;
}
