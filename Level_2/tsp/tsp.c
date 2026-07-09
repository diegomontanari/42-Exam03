#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

typedef struct s_city
{
	float x;
	float y;
}	t_city;

/*
** You must implement this function
** It should return the Euclidean distance between 2 cities
*/
float	distance(t_city a, t_city b);

/*
** You must implement this function
** It should compute total distance of the path
** including the return to the starting city
*/
float	total_distance(t_city *cities, int *path, int n);

/*
** You must implement this function
** Use backtracking to try all permutations
*/
void	solve(t_city *cities, int *path, int n, int pos, float *min);

int	main(void)
{
	t_city	cities[11];
	int		n = 0;

	// Read input from stdin
	while (n < 11 && fscanf(stdin, "%f, %f", &cities[n].x, &cities[n].y) == 2)
		n++;

	// If less than 2 cities → distance = 0
	if (n < 2)
	{
		printf("0.00\n");
		return (0);
	}

	// Initialize path: [0,1,2,...]
	int path[11];
	for (int i = 0; i < n; i++)
		path[i] = i;

	float min = FLT_MAX;

	/*
	** Optimization:
	** Fix first city → start from pos = 1
	*/
	solve(cities, path, n, 1, &min);

	printf("%.2f\n", min);
	return (0);
}
