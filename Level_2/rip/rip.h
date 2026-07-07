#ifndef RIP_H
# define RIP_H

# include <unistd.h> // → write
# include <stdio.h>  // → puts

// ft_strlen: standard strlen fatta a mano.
// Ci serve solo per sapere quanto è lunga la stringa in ingresso,
// niente di strano, la scriviamo noi perché strlen() non è nella lista
// delle funzioni permesse (solo puts e write lo sono).
int		ft_strlen(const char *s);

// compute_removals: fa UN passaggio sulla stringa e calcola il numero
// MINIMO di '(' e di ')' che vanno per forza rimosse per bilanciare.
// Questo numero è una proprietà del problema, si dimostra che è sempre
// raggiungibile: non esiste un modo di bilanciare rimuovendone di meno.
void	compute_removals(const char *s, int len, int *left_rem, int *right_rem);

// solve: la ricorsione vera e propria (backtracking).
// Per ogni carattere della stringa, prova TUTTE le scelte valide
// (tenerlo o rimuoverlo, quando è permesso) e stampa ogni volta che
// arriva in fondo avendo consumato esattamente il budget di rimozioni.
void	solve(const char *s, char *out, int i, int len, int open,
			int left_rem, int right_rem);

#endif
