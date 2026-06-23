#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// WARNING: this code works but does not respect lexicographic order.
// Reason: swap modifies the string in-place, so after each swap
// the remaining characters are no longer in ascending order.
// Example: with input "bca", the output is bca, bac, cba, cab, acb, abc
// instead of abc, acb, bac, bca, cab, cba.

// start = position we are currently deciding
// i = candidate we want to try in that position
void permutations(char *s, int start, int len)
{
	if (start == len) {
		printf("%s\n", s); // base case: we have filled all positions
		return ;
	}

	// we start with i = start because everything before start is already decided
	// imagine being here: start = 1; it means: b | a c
	// the left side: b is fixed. we must not touch it anymore.
	for (int i = start; i < len; i++) { // iterates over the string
		char tmp = s[start]; // save s[start] before it gets overwritten
		s[start]= s[i]; // put s[i] in position start
		s[i] = tmp; // put the old s[start] in position i

		permutations(s, start + 1, len);

		// in math terms: swap is an involution
		// swap is one of those special operations that is its own inverse.
		// that's why we write the exact same code (only difference: tmp is already declared)
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

// the for in main is identical to:
//   while (av[1][len]) len++;
// the main is identical to: same logic, different style only
