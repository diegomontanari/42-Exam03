#include <stdio.h>
#include <stdlib.h>

// Approccio diverso rispetto a permutations_no_lex.c:
// invece di swappare in-place (che rompe l'ordine), si ordina prima la stringa
// e si costruisce la permutazione carattere per carattere usando un array visited.
// Scegliendo sempre il prossimo carattere disponibile in ordine, l'output è lessicografico.

void permutations(char *s, int len, char *buf, int *visited, int depth)
{
	if (depth == len)
	{
		buf[len] = '\0';
		printf("%s\n", buf);
		return ;
	}
	for (int i = 0; i < len; i++)
	{
		if (visited[i])
			continue ;
		visited[i] = 1;
		buf[depth] = s[i];
		permutations(s, len, buf, visited, depth + 1);
		visited[i] = 0;
	}
}

int main(int ac, char **av)
{
	if (ac != 2)
		return 1;

	int len = 0;
	while (av[1][len])
		len++;

	// ordino la stringa (bubble sort) per garantire output lessicografico
	char *s = av[1];
	for (int i = 0; i < len - 1; i++)
		for (int j = 0; j < len - 1 - i; j++)
			if (s[j] > s[j + 1])
			{
				char tmp = s[j];
				s[j] = s[j + 1];
				s[j + 1] = tmp;
			}

	char *buf = malloc(len + 1); // aggiungo spazio per terminatore stringa
	int *visited = malloc(len * sizeof(int));
	if (!buf || !visited)
	{
		free(buf);
		free(visited);
		return 1;
	}
	for (int i = 0; i < len; i++)
		visited[i] = 0;

	permutations(s, len, buf, visited, 0);

	free(buf);
	free(visited);
	return 0;
}
