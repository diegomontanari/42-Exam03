#include "rip.h"

// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
// ft_strlen: conta i caratteri fino al '\0'. Standard, niente di nuovo.
int	ft_strlen(const char *s)
{
	int	len;

	len = 0;
	while (s[len])
		len++;
	return (len);
}

// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
// compute_removals: un solo passaggio da sinistra a destra.
//
// open = quante '(' ho "aperto" e non ancora richiuse.
// Ogni volta che trovo una ')':
//   - se open > 0, si "richiude" una parentesi aperta (open--).
//   - se open == 0, questa ')' è di troppo (non ha nessuna '(' da
//     richiudere): è per forza da rimuovere, quindi right_rem++.
//
// Alla fine del ciclo, quello che resta in "open" sono le '(' rimaste
// senza una ')' che le richiuda: sono per forza da rimuovere anche
// quelle, quindi left_rem = open.
//
// Perché è il numero MINIMO e non uno a caso: ogni ')' che conto in
// right_rem è stata "di troppo" nel momento esatto in cui l'ho letta,
// quindi non c'era NESSUN modo di salvarla (nessuna '(' prima di lei
// disponibile). Stesso discorso per le '(' rimaste alla fine: nessuna
// ')' successiva le ha mai richiuse. Sono rimozioni obbligate, non a
// piacere: ecco perché il totale è il minimo indispensabile.
void	compute_removals(const char *s, int len, int *left_rem, int *right_rem)
{
	int	open;
	int	i;

	open = 0;
	*right_rem = 0;
	i = 0;
	while (i < len)
	{
		if (s[i] == '(')
			open++;
		else if (s[i] == ')')
		{
			if (open > 0)
				open--;
			else
				(*right_rem)++;
		}
		i++;
	}
	*left_rem = open;
}

// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
// solve: il cuore del backtracking.
//
// Idea generale: per ogni indice i, ho una scelta binaria da fare sul
// carattere s[i]: TENERLO o RIMUOVERLO (sostituendolo con uno spazio
// in "out", MAI in "s": "s" resta sempre la stringa originale, così
// non perdo mai il carattere vero mentre backtrack-o).
//
// Non serve fare "undo" esplicito come nel powerset: qui, prima di
// ogni chiamata ricorsiva, scrivo io stesso out[i] con il valore
// giusto (spazio o carattere originale), quindi il buffer è sempre
// coerente per quella specifica discesa, a prescindere da cosa aveva
// lasciato la chiamata precedente.
//
// Perché non servono controlli anti-duplicato (a differenza di altri
// esercizi di backtracking): qui "rimuovere" significa sostituire con
// uno spazio, NON cancellare e accorciare la stringa. Quindi rimuovere
// la parentesi in posizione 2 invece che in posizione 4 produce sempre
// due stringhe DIVERSE (lo spazio è in un posto diverso), anche se le
// due parentesi erano identiche. Nessun rischio di stampare due volte
// la stessa identica soluzione.
//
// Le regole di validità (perché non serve controllare "open == 0" alla
// fine, viene garantito da solo):
//   - una '(' la posso sempre TENERE (open++), non crea mai problemi.
//   - una ')' la posso TENERE solo se open > 0 (altrimenti la sto
//     "chiudendo" senza aver aperto nulla: sequenza non valida, quel
//     ramo lo taglio proprio, non ricorro nemmeno).
//   - se rispetto sempre questa regola, e alla fine ho consumato
//     ESATTAMENTE left_rem e right_rem (il minimo calcolato prima),
//     allora il numero di '(' tenute è sempre uguale al numero di ')'
//     tenute, e la sequenza non è mai andata in negativo: quindi deve
//     per forza finire con open == 0. Non serve ricontrollarlo a mano.
void	solve(const char *s, char *out, int i, int len, int open,
			int left_rem, int right_rem)
{
	if (i == len)
	{
		if (left_rem == 0 && right_rem == 0)
			puts(out);
		return ;
	}
	// Pruning (non obbligatorio per la correttezza, ma taglia rami
	// morti in anticipo): se i caratteri rimasti da qui alla fine
	// sono meno del budget di rimozioni ancora da fare, è impossibile
	// arrivare a zero. Inutile continuare a scendere per niente.
	if (left_rem + right_rem > len - i)
		return ;
	if (s[i] == '(')
	{
		// Scelta 1: rimuovo questa '(' (solo se ho ancora budget).
		if (left_rem > 0)
		{
			out[i] = ' ';
			solve(s, out, i + 1, len, open, left_rem - 1, right_rem);
		}
		// Scelta 2: la tengo. Una '(' aperta non rompe mai nulla.
		out[i] = s[i];
		solve(s, out, i + 1, len, open + 1, left_rem, right_rem);
	}
	else
	{
		// Scelta 1: rimuovo questa ')' (solo se ho ancora budget).
		if (right_rem > 0)
		{
			out[i] = ' ';
			solve(s, out, i + 1, len, open, left_rem, right_rem - 1);
		}
		// Scelta 2: la tengo, ma SOLO se c'è una '(' aperta da chiudere.
		if (open > 0)
		{
			out[i] = s[i];
			solve(s, out, i + 1, len, open - 1, left_rem, right_rem);
		}
	}
}

// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
int	main(int ac, char **av)
{
	int	len;
	int	left_rem;
	int	right_rem;

	// Deve prendere esattamente un argomento (il nome del programma
	// conta come av[0], quindi ac == 2 vuol dire "un argomento reale").
	if (ac != 2)
	{
		write(2, "Error\n", 6);
		return (1);
	}
	len = ft_strlen(av[1]);
	compute_removals(av[1], len, &left_rem, &right_rem);

	// VLA (array a dimensione variabile, C99): niente malloc perché
	// non è tra le funzioni permesse. La dimensione dipende da "len",
	// che conosciamo solo a runtime, quindi non può essere una normale
	// array a dimensione fissa dichiarata staticamente.
	char	out[len + 1];

	// '\0' finale fissato una volta sola: solve() non tocca mai
	// l'indice "len", quindi resta sempre lì, e ogni puts(out) stampa
	// una stringa correttamente terminata.
	out[len] = '\0';
	solve(av[1], out, 0, len, 0, left_rem, right_rem);
	return (0);
}
