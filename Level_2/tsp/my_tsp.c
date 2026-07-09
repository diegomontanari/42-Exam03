#include <math.h>

// Ridichiarata identica a quella di tsp.c: il file ufficiale non ha un
// header, quindi ogni .c che la usa deve ridefinirla localmente con lo
// stesso nome, stessi campi, stesso ordine. Il linker non controlla i
// tipi, solo i simboli delle funzioni: basta che i due lati concordino.
typedef struct s_city
{
	float x;
	float y;
}	t_city;

// distance: distanza euclidea tra due città. sqrtf perché lavoriamo in
// float (sqrt vorrebbe double), coerente col resto dell'esercizio.
float	distance(t_city a, t_city b)
{
	float	dx;
	float	dy;

	dx = a.x - b.x;
	dy = a.y - b.y;
	return (sqrtf(dx * dx + dy * dy));
}

// total_distance: somma le distanze tra città consecutive nel path e
// chiude il ciclo aggiungendo il ritorno dall'ultima alla prima.
// Usata solo per verifiche indipendenti; il ramo caldo di solve calcola
// il totale in modo incrementale (vedi solve_rec) per poter fare pruning.
float	total_distance(t_city *cities, int *path, int n)
{
	float	total;
	int		i;

	total = 0.0f;
	i = 0;
	while (i < n - 1)
	{
		total += distance(cities[path[i]], cities[path[i + 1]]);
		i++;
	}
	total += distance(cities[path[n - 1]], cities[path[0]]);
	return (total);
}

// solve_rec: il vero backtracking con branch and bound, tenuto separato
// da solve() perché la firma richiesta da tsp.c è fissa (5 parametri,
// niente accumulatore). Qui aggiungiamo "current_dist" per poter
// potare un ramo appena il suo costo parziale raggiunge il minimo
// attuale, senza aspettare di aver generato l'intera permutazione.
//
// Le città vengono permutate in-place su "path" tramite swap: si scambia
// path[pos] con path[i], si ricorre, poi si annulla lo swap (backtracking
// esplicito, necessario perché qui lo stato modificato — l'array path —
// è condiviso tra tutti i livelli della ricorsione, non passato per
// valore).
static void	solve_rec(t_city *cities, int *path, int n, int pos,
				float *min, float current_dist)
{
	int		i;
	int		tmp;
	float	edge;
	float	closing;

	if (pos == n)
	{
		closing = distance(cities[path[n - 1]], cities[path[0]]);
		if (current_dist + closing < *min)
			*min = current_dist + closing;
		return ;
	}
	i = pos;
	while (i < n)
	{
		edge = distance(cities[path[pos - 1]], cities[path[i]]);
		// Pruning: se il percorso parziale ha già raggiunto (o superato)
		// il minimo noto, aggiungere la chiusura finale (>= 0) non potrà
		// mai batterlo. Tutto il sottoalbero da qui viene scartato senza
		// esplorarlo.
		if (current_dist + edge < *min)
		{
			tmp = path[pos];
			path[pos] = path[i];
			path[i] = tmp;
			solve_rec(cities, path, n, pos + 1, min, current_dist + edge);
			tmp = path[pos];
			path[pos] = path[i];
			path[i] = tmp;
		}
		i++;
	}
}

// solve: rispetta esattamente la firma richiesta da tsp.c. main chiama
// solve(cities, path, n, 1, &min) con path già inizializzato a
// [0, 1, ..., n-1]: la città 0 è fissata in posizione 0 (rompe la
// simmetria rotazionale del ciclo, riduce n! a (n-1)! permutazioni da
// esplorare). Da qui in poi il lavoro vero è tutto in solve_rec.
void	solve(t_city *cities, int *path, int n, int pos, float *min)
{
	solve_rec(cities, path, n, pos, min, 0.0f);
}
