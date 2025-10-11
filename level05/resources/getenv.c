#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    printf("env address at %p\n", argv[1], getenv(argv[1]));
    return (0);
}
