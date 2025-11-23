#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <stddef.h>

// Funzione ricorsiva che esplora tutti i percorsi possibili per il TSP
// Implementa backtracking con branch and bound per aggiornare la distanza minima
static void tsp_rec(float (*cities)[2], ssize_t size, ssize_t current_city,
                    bool *visited, ssize_t visited_count,
                    float current_dist, float *best_distance)
{
    // Caso base: tutte le città sono state visitate
    if (visited_count == size)
    {
        // Aggiungi la distanza per tornare alla città di partenza
        float total_dist = current_dist + sqrtf(
            (cities[current_city][0] - cities[0][0]) * (cities[current_city][0] - cities[0][0]) +
            (cities[current_city][1] - cities[0][1]) * (cities[current_city][1] - cities[0][1])
        );

        // Se il percorso completo è migliore della migliore distanza attuale, aggiornala
        if (total_dist < *best_distance)
            *best_distance = total_dist;

        return; // Esci dalla ricorsione
    }

    // Ciclo su tutte le città possibili per il prossimo passo
    for (ssize_t i = 0; i < size; i++)
    {
        if (visited[i]) // Se la città è già stata visitata, salta
            continue;

        // Calcola la distanza tra la città corrente e la città i-esima
        float dist = sqrtf(
            (cities[current_city][0] - cities[i][0]) * (cities[current_city][0] - cities[i][0]) +
            (cities[current_city][1] - cities[i][1]) * (cities[current_city][1] - cities[i][1])
        );

        // Branch and bound: se il percorso attuale già supera la migliore distanza nota, salta questo ramo
        if (current_dist + dist >= *best_distance)
            continue;

        visited[i] = true; // Marca la città come visitata
        // Chiamata ricorsiva alla funzione con la nuova città corrente
        tsp_rec(cities, size, i, visited, visited_count + 1, current_dist + dist, best_distance);
        visited[i] = false; // Backtracking: deseleziona la città per esplorare altri percorsi
    }
}

// Funzione principale da chiamare dal file tsp.c fornito dalla 42
// Gestisce l'inizializzazione dell'array visited e richiama tsp_rec
float tsp(float (*cities)[2], ssize_t size)
{
    if (size <= 1)
        return 0.0f; // Nessuna distanza se c'è una sola città

    bool visited[size];
    for (ssize_t i = 0; i < size; i++)
        visited[i] = false; // Inizializza tutte le città come non visitate

    visited[0] = true; // Partenza dalla città 0
    float best_distance = INFINITY; // Distanza minima inizializzata a infinito

    // Chiamata alla funzione ricorsiva
    tsp_rec(cities, size, 0, visited, 1, 0.0f, &best_distance);

    return best_distance; // Restituisce la distanza minima trovata
}