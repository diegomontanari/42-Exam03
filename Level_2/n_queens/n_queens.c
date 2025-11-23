#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int	is_number(const char *s)
{
	if (!s || !*s)
		return (0);
	for (int i = 0; s[i]; i++)
		if (!isdigit((unsigned char)s[i])) // cast perché is_digit accetta solo valori positivi mentre char su alcuni sistemi può essere signed e questo può portare a undefined behavior.
			return (0);
	return (1);
}

int	is_safe(int *pos, int col, int row)
{
	for (int c = 0; c < col; c++) // controlla tutte le colonne precedenti
	{
		int r = pos[c]; // r è la riga della regina già piazzata in colonna c
		if (r == row // stessa riga → attacco orizzontale
			|| r - c == row - col // stessa diagonale principale ↘
			|| r + c == row + col) // stessa diagonale secondaria ↙
			return (0); // non sicuro: conflitto
	}
	return (1); // sicuro: nessun conflitto
}

void	solve(int *pos, int col, int n)
{
	if (col == n) // Se ho finito, stampo la soluzione
	{
		for (int i = 0; i < n; i++)
		{
			printf("%d", pos[i]);
			if (i < n - 1) // Qua ho riconosciuto il pattern incontrato in fprime: ovvero, puoi fare o così, oppure stampi "pos[i] " (con spazio) fino a n-1, e poi stampi fuori dal for "pos[i]" finale senza spazio alla fine. Pattern riconosciuto (!!)
				printf(" ");
		}
		printf("\n");
		return ;
	}
	for (int row = 0; row < n; row++)
	{
		if (is_safe(pos, col, row))
		{
			pos[col] = row; // Se è safe, nella colonna metto il numero della riga (es: colonna zero "incrocia" con riga zero (posizione: 0;0)
			solve(pos, col + 1, n); // Passiamo a posizionare la regina nella colonna successiva.
		}
	}
}

int	main(int ac, char **av)
{
	if (ac != 2 || !is_number(av[1]))
		return (1);

	int n = atoi(av[1]);
	if (n <= 0)
		return (1);

	int *positions = malloc(sizeof(int) * n);
	if (!positions)
		return (1);

	solve(positions, 0, n);
	free(positions);
	return (0);
}