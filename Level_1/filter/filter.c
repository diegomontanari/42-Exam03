#include <stdlib.h> //  → malloc, calloc, realloc, free
#include <unistd.h> //  → read
#include <stdio.h> // → printf, perror
#include <string.h> // → strlen

// Def. MACRO (num. di Bytes da provare a leggere  ad ogni chiamata a read())
// Non serve la header guard perché ho un solo file
// 4KB è un buon compromesso: riduce chiamate a read(), + veloce e non spreca memoria provando a leggere oltre, + efficiente 
#define CHUNK 4096

/*
Differenza tra printf() e perror():
Entrambe scrivono messaggi sullo schermo, ma hanno scopi diversi.
printf()
- Scrive su stdout (file descriptor 1)
- Stampa un messaggio formattato che decidi tu
- Usalo per output “normale” del programma
perror()
- Scrive su stderr (file descriptor 2)
- Stampa automaticamente la descrizione dell’errore di sistema
  associato a errno, preceduta dalla stringa che gli passi
- Usalo per messaggi di errore di sistema (es. read, malloc, open, ecc.)
- In pratica, se escrivi "Error: " lui ti stampa quello e poi l'errore
  di sistema che si è verificato. Bellissimo!
*/

// Libera fino a due puntatori se non NULL.
// Il manuale C non da problemi se fai free ad un ptr NULL,
// ma è comunque una buona pratica difensiva (se aumenta complessità rischi di sfaciolare).
// ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
// static perché voglio che questa funzione sia visibile solo da questo file .c
static int fail(char *p1, char *p2)
{
    if (p1) free(p1);
    if (p2) free(p2);
    perror("Error: ");
    return 1;
}

int	main(int ac, char **av)
{
	if (ac != 2 || !av[1][0]) // Deve prendere solo un argomento e deve esistere
		return 1;
	
	char *needle = av[1];
	size_t nlen = strlen(needle);
	
	// Malloc prende solo malloc(dim_da_allocare)
	// Calloc prende calloc(num_elem_da_allocare, dim_singolo_elem) e inoltre inizializza tutti gli elem a zero binario
	// -> quindi se è un arr di int inizializza tutto a 0, se è un arr di char tutto a \0 (perché entrambi si indicano
	// -> con zero in binario, ovviamente char 00 (1 byte), int 00 00 00 00 (4 byte) pointer 8 byte quindi 8 volte 00 per NULL,
	// -> ma sempre zero binario è, semplicemente viene interpretato in modo diverso a seconda del num. di byte)
	// -> key takeaway: a distinguere i tipi in C è solo il numero di byte che occupano! Bellissimo

	char *acc = calloc(1, 1); // accumulatore di tutti i byte letti. Dato che è la variabile principale del programma, mentre buf è solo di appoggio a read(), dal punto di vista stilistico e di eleganza del codice (e quindi anche chiarezza di pensiero) è meglio dichiararla prima di buf (Dettagli interessanti!!)
	if (!acc)
		return fail(NULL, NULL);

	// Immagina di leggere un libro pagina per pagina e ricopiarlo tutto in un quaderno unico:
	// buf è la pagina (una porzione temporanea di testo, es. 4096 caratteri)
	// acc è il quaderno dove stai riscrivendo tutto il libro intero
	// Ogni volta che leggi una nuova pagina, la copi in fondo al quaderno, poi passi alla successiva.
	char *buf = malloc(CHUNK);
	if (!buf)
		return fail(NULL, NULL);

	// int = signed int → tipo intero con segno
	// size_t = intero senza segno, usato come standard per misure/dimensioni
	// ssize_t = intero con segno, per quantità che possono essere -1 (negative)
	ssize_t r;
    size_t total = 0; // totale di byte letti
    
	// read() restituisce:
	// > 0  → numero di byte letti correttamente (es: se legge 42 byte ritorna 42)
	// = 0  → EOF (fine input, nessun altro dato da leggere)
	// = -1 → errore di lettura (errno contiene il motivo)
	// In conclusione, questo ciclo itera CHUNK per CHUNK, fino all'EOF (End Of File) 
	while ((r = read(0, buf, CHUNK)) > 0) // read prova a leggere fino a CHUNK byte e li salva dentro buf
	{
        // realloc(acc, new_size) non allarga direttamente acc,
		// ma allarga (o sposta) il blocco a cui acc punta,
		// e ti restituisce un nuovo indirizzo dove ora si trova quel blocco.
		// Poi, con acc = tmp, aggiorni il puntatore per puntare al nuovo indirizzo.
		char *tmp = realloc(acc, total + (size_t)r + 1); // prototico realloc: void *realloc(void *ptr, size_t new_size); Ricorda: se new_size è 0, in generale realloc equivale a free(ptr); Ricorda2: aggiunto + 1 per spazio per '\0'. 
        if (!tmp)
            return fail(acc, buf);
    	// realloc tenta di ridimensionare il blocco di memoria puntato da acc:
		// - se c’è spazio libero accanto, allarga il blocco “sul posto” (stesso indirizzo);
		// - altrimenti alloca un nuovo blocco più grande da un'altra parte libera in memoria, copia i vecchi dati e libera quello vecchio;
		// - se fallisce, restituisce NULL e lascia acc invariato (ancora valido).
		//   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---   ---
		// Per questo salviamo prima il return realloc in tmp: se realloc fallisse, non perderemmo acc, ovvero:
		// se facessimo acc = realloc() e realloc fallisse, imposterebbe acc a NULL, memory leak! Addio byte salvati.
		acc = tmp; // Noi siamo previdenti e assegnamo tmp ad acc solo dopo esserci assicurati che realloc non restituisca NULL. Intelligente (!!) 

		// Il for copia byte per byte in acc
        for (ssize_t i = 0; i < r; i++) // ssize_t qui perché c'è comparison con ssize_t r
            acc[total + (size_t)i] = buf[i];

        total += (size_t)r; // Aumento il contatore dei byte totali letti
        acc[total] = '\0'; // metto '\0' perché sto lavorando con una stringa. '\0' in questo caso è solo un terminatore temporaneo, perché dopo lo sovrascriverò con gli altri byte. Potrei tranquillamente metterlo solo fuori dal ciclo, perché tanto mi serve solo per l'ultima stampa, così il printf si ferma quando lo incontra, ma metterlo dentro è più standard.
    }
    if (r < 0) // se read ha dato errore (restituito -1) 
        return fail(acc, buf);
	
	free(buf);

    // Scansione + sostituzione in streaming su stdout (via printf)
	// Non c'è incremento nella dichiarazione perché i viene incrementato direttamente dentro al for
	// i è l'iteratore principale, che incrementa di 1 per ogni carattere
    for (size_t i = 0; acc[i] != '\0'; )
    {
		// j invece si resetta ad ogni stampa, e scorre solo il needle
		size_t j = 0;

		// Se match tra acc[i + j] e neddle[j], incrementi j per vedere se combacia tutto il needle (quindi se va sostituito con *)
        while (j < nlen && acc[i + j] != '\0' && acc[i + j] == needle[j])
            j++;

		// Se effettivamente tutto il needle combacia
        if (j == nlen && nlen > 0)
        {
            // match completo: stampa nlen asterischi
            for (size_t k = 0; k < nlen; k++)
                printf("*");
            i += nlen;
        }
        else
        {
            // nessun match qui: stampa il carattere corrente e avanza di 1
            printf("%c", acc[i]);
            i++;
        }
    }

    free(acc);
    return 0;
}