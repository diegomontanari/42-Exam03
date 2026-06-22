#include <stdio.h>
#include <stdlib.h>

// Stampa un sottoinsieme di dimensione 'size'
void print_subset(int *subset, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d", subset[i]);
        if (i < size - 1)
            printf(" ");
    }
    printf("\n");
}

// Funzione di backtracking che trova tutti i subset con somma target

// index → indica da quale posizione del set partire per provare a includere nuovi elementi nel subset.
// serve a non riesplorare elementi già considerati e a mantenere l’ordine del set.

// sum → tiene traccia della somma attuale degli elementi inclusi in subset.
// serve a sapere se hai già raggiunto il target.

void backtrack(int *set, int set_size, int *subset, int sub_size,
               int index, int sum, int target)
{
    if (sum == target && sub_size > 0)
        print_subset(subset, sub_size);

    // Esplora tutti gli elementi da index in poi
    for (int i = index; i < set_size; i++)
    {
        subset[sub_size] = set[i];     // sub_size   // index  // sum
        backtrack(set, set_size, subset, sub_size + 1, i + 1, sum + set[i], target);
    }
}

int main(int ac, char **av)
{
    if (ac < 2)
        return 1;

    int target = atoi(av[1]);
    int set_size = ac - 2;

    if (set_size == 0)
        return 0;

    int *set = malloc(set_size * sizeof(int));
    int *subset = malloc(set_size * sizeof(int));
    if (!set || !subset)
    {
        free(set);
        free(subset);
        return 1;
    }

    // Metto gli elementi nel set
    for (int i = 0; i < set_size; i++)
        set[i] = atoi(av[i + 2]); // Parto dal secondo argomento quindi da av[i + 2]

    // Avvio il backtracking
    backtrack(set, set_size, subset, 0, 0, 0, target);

    free(set);
    free(subset);
    return 0;
}

/*

Prima avevo fatto un'ottimizzazione, basata sull'assunzione che tutti gli argomenti fossero positivi. In quel caso, appena superi il target puoi fare branch & bound, potare e passare al ramo successivo.
Tuttavia, qui non ho informazioni sul segno degli argomenti, quindi non posso ottimizzare perché rischierei di potare prematuramente dei rami che portano ad un match.

Scrivo questo come commento perché quest'ottimizzazione mi ha fatto scoprire che in C il continue vale solo nei loop, e quindi salta l'iterazione corrente. Non funziona nelle condizioni. Per questo, riporto il commento qui:

 ottimizzazione: se target >=0 e somma supera target, salto
        if (target >= 0 && sum + set[i] > target)
            continue; // Il continue serve solo all’interno di un ciclo (for, while o do-while). 
                      // Quando viene eseguito, salta tutto ciò che resta nel corpo del ciclo per l’iterazione corrente 
                      // e passa direttamente alla prossima iterazione. // Fuori da un ciclo, usare continue non ha 
                      // senso e genera errore di compilazione.
*/