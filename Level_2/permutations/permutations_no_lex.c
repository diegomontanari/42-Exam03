#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// ATTENZIONE: questo codice funziona ma non rispetta l'ordine lessicografico.
// Il motivo: lo swap modifica la stringa in-place, quindi dopo ogni scambio
// i caratteri rimanenti non sono più in ordine crescente.
// Esempio: con input "bca", l'output è bca, bac, cba, cab, acb, abc
// invece di abc, acb, bac, bca, cab, cba.

// start = posizione che sto decidendo
// i = candidato che voglio provare in quella posizione
void permutations(char *s, int start, int len)
{
	if (start == len) {
		printf("%s\n", s); // caso base, se ho finito la stringa
		return ;
	}

	// qui parto con i = start perché Sì, i = start perché tutto ciò che viene prima di start è già stato deciso
	// infatti: mmagina di essere qui: start = 1; Cosa significa? Significa: b | a c
	// La parte sinistra: b è già fissata. Non devi più toccarla.
	for (int i = start; i < len; i++) { // scorre sulla stringa
		char tmp = s[start]; // salvo start che sta per essere sovrascritto
		s[start]= s[i]; // assegno s[i] a start
		s[i] = tmp; // assegno tmp (il vecchio start) a s[i])

		permutations(s, start + 1, len);

		// in matematica si direbbe: swap è un'involuzione
		// lo swap è una di quelle operazioni speciali che sono il proprio inverso.
		// per questo riscrivo identico (unica diff. è che tmp stavolta già dichiarato)
		tmp = s[start];
		s[start] = s[i];
		s[i] = tmp;
	}
}

int main(int ac, char **av)
{
	if (ac != 2) return 1;

	int start = 0;
	int len;
	for (len = 0; av[1][len]; len++)
		;
	permutations(av[1], start, len);
	return 0;
}

// il for nel main è identico a:
//   while (av[1][len]) len++;
// il main è identico a: stessa logica, solo stile diverso
