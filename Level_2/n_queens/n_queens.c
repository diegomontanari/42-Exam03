#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Questa controlla sia che n sia un numero ma anche che atoi non crei problemi
// Infatti atoi non gestisce l'overflow: se gli passi "99999999999", legge i caratteri e fa i conti in int, 
// ma quando il risultato supera INT_MAX il comportamento è undefined — può restituire un numero negativo, zero, 
// qualsiasi cosa. Quindi is_number verifica che la stringa rappresenti un numero che atoi può convertire correttamente 
// senza andare in undefined behavior.
int is_number(const char *s)
{
	if (!s || !*s)
		return (0);
	int i = 0;
	for (; s[i]; i++)
	{
		if (!(s[i] >= '0' && s[i] <= '9')) // if (!isdigit((unsigned char)s[i])) era prima con cast perché isdigit accetta solo valori positivi mentre char su alcuni sistemi può essere signed e questo può portare a undefined behavior, ma qui non posso usare is_digit poiché non ammessa dalla 42, quindi uso le vecchie maniere
			return 0;
	}
	if (i > 10) return 0;
	if (i == 10)
	{
		const char *max = "2147483647";
		for (int j = 0; j < 10; j++)
		{
			if (s[j] > max[j]) return 0; // se anche una cifra da sx è maggiore allora n è maggiore allora ritorno 0
			if (s[j] < max[j]) break; // allora minore posso uscire e ritorna 1 alla fine del ciclo
			// if s[j] == max[j] continua il ciclo. Nel caso peggiore sono identici allora esce dal ciclo e ritorna 1
		}
	}
	return 1;
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
	// prima is_number si assicura che la stringa av
	// sia numerica e non sia overflow...
	if (ac != 2 || !is_number(av[1]))
		return (1);

	// ... Solo dopo possiamo chiamare atoi con 
	// sicurezza
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
