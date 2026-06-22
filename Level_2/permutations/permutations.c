#include <stdio.h>

void permutations(char *s, int start, int len)
{
    if (start == len)
    {
        printf("%s\n", s);
        return;
    }
    for (int i = start; i < len; i++)
    {
        // swap s[start] con s[i]
        char tmp = s[start];
        s[start] = s[i];
        s[i] = tmp;

        permutations(s, start + 1, len);

        // undo: rimetti a posto
        tmp = s[start];
        s[start] = s[i];
        s[i] = tmp;
    }
}

int main(int ac, char **av)
{
    if (ac != 2)
        return 1;
    int len = 0;
    while (av[1][len])
        len++;
    permutations(av[1], 0, len);
    return 0;
}
