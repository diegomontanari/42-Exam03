#include <stdarg.h> // Variadic args
#include <stdio.h> // fgets, scanf, printf, perror ecc...
#include <ctype.h> // Char type, serve per ft che classificano i char (es isalpha, isdigit ecc...)

/* -------------------------------------------------------------------------- */
/*                             Gestione degli spazi                           */
/* -------------------------------------------------------------------------- */
// Funzione che lavora su uno stream (FILE *f) e prova a 
// saltare tutti i caratteri di spazio presenti subito nell’input.
int match_space(FILE *f)
{
    int c = fgetc(f); // Legge un carattere dallo stream. fgetc ritorna il carattere come int oppure EOF se fine file/errore.
    
    while (c != EOF && isspace(c)) // Finché spazio continua a leggere char by char
        c = fgetc(f);
    
    if (c == EOF)
        return -1; // Segnala che hai "finito" l'inupt 

    ungetc(c, f); // Rimette nell’input l’ultimo carattere non-spazio letto, così non viene perso: la prossima lettura lo vedrà ancora. (Nota: è garantito poter fare almeno un ungetc dell’ultimo char letto). Inoltre, ungetc prende in input sia il carattere che il file (a differenza di fgetc ad esempio) perché deve sapere quale char rimettere e in quale stream inserirlo.
    return 1; // Ritorna 1 per indicare che l’operazione di “skip whitespace” è andata a buon fine e lo stream ora è posizionato sul primo non-spazio (grazie a ungetc).
}

/* -------------------------------------------------------------------------- */
/*                         Match di caratteri letterali                       */
/* -------------------------------------------------------------------------- */
//Legge 1 carattere dall’input.

// Se combacia → lo consuma (non servirà più). (Anche perché vscanf incrementa format
// ad ogni iterazione quindi il FILE deve stare al passo e consumare un carattere)

// Se NON combacia → deve “rimetterlo” nell’input perché non dovrebbe essere stato 
// letto → e fallisce la conversione.
// Se la conversione fallisce, ovviamente, scanf deve lasciare l’input intatto,
// altrimenti rompe la logica del programma, e futuri scanf chiamati 
// non funzionerebbero correttamente.

int match_char(FILE *f, char c)
{
    int input = fgetc(f);
    
    if (input == EOF)
        return -1;

    if (input == c)
        return 1;
    
    ungetc(input, f); // Arrivo qui se non combacia, quindi torno al char prima e ritorno non match (non errore grave da mettere -1, ma comunque segnalo che non combacia)
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                              Conversione %c                                */
/* -------------------------------------------------------------------------- */
// Funzione che gestisce la conversione %c: legge un carattere dallo stream f 
// e lo scrive nella "variabile dell'utente" ovvero la variabile c che se l'user scrive 'a' contiene a.
// es: prima della compilazione, è previsto che l'user scriva un char c: allora la va_list sarà solo [&c]
// poi fgetc (oppure scanf) legge la lettera, 

// Ritorna: 1 (conversione riuscita), -1 (EOF/errore prima di leggere).

// ES: se l'user scrive 'a', fgets estrae a, char *ptr = va_arg(ap, char *) dice che come &c punta a c anche ptr punta a c.
// ptr deferenziato, quindi il suo valore, diventa c, ovvero la 'a' dell'utente. Ho finito di salvare il valore dell'utente.

int scan_char(FILE *f, va_list ap)
{
    int c = fgetc(f); // legge un carattere dal file
                      // Attenzione: fgetc restituisce int perché deve poter restituire sia un carattere valido (0–255) sia EOF che di solito vale -1. 
                      // Se restituisse char, non ci sarebbe modo di distinguere EOF da un carattere legittimo con valore 255.
    
    if (c == EOF)
        return -1;
    
    char *ptr = va_arg(ap, char *); // “Dammi il prossimo elemento della lista di arg variadici, e interpretalo come un char *, salva il risultato in un char *ptr
    
    *ptr = (char)c; // Ora il valore dell'utente viene assegnato al valore di ptr deferenziato, abbiamo "letto" l'n-esimo valore inserito dall'utente. ATTENZIONE: la conversione (char) va fatta esplicita perché i cast impliciti sono non portabili e poco sicuri. Come regola generale: sai che in una riga passi da int a char? Esplicitalo sempre. 
    
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                              Conversione %d                                */
/* -------------------------------------------------------------------------- */
int scan_int(FILE *f, va_list ap)
{
    int sign = 1;
    int num = 0;
    int digits = 0;
    int c = fgetc(f);

    // gestione del segno
    if (c == '+' || c == '-')
    {
        if (c == '-')
            sign = -1;
        c = fgetc(f);
    }

    // lettura delle cifre
    while (c != EOF && isdigit(c))
    {
        num = num * 10 + (c - '0');
        digits++;
        c = fgetc(f);
    }

    // Rimettiamo l’ultimo carattere non numerico nel buffer
    if (c != EOF) // Così perché ungetc non accetta EOF in input.
        ungetc(c, f); // se "123A", il while di lettura cifre si ferma ad A, ma A non può essere consumato, devo tornare indietro per leggerlo, quindi lo rimetto nello stream

    if (digits == 0) // Se la conversione non è andata a buon fine (es: EOF subito nello stream)
        return 0;

    int *ptr = va_arg(ap, int *); // Perché uso puntatore? RICORDA: sncaf legge dallo stdin e lo salva nel codice, se lo salvassi in una variabile avresti una ...
    *ptr = num * sign; // ... copia, ma poi non posso modificarla, e allora l'unico modo è per riferimento, usando un puntatore. Il puntatore serve quindi per questo.
                       // --main----------|
                       // int x;          |
                       // scanf("%d", &x);|
                       // ----------------|
                       // hai questo, ora dentro scan_int, va_arg punta a &x. Chiaro? Quindi, int *ptr = va_arg(ap, int*) non fa altro che ...
                       // ... collegare due freccette (puntatori) alla stessa variabile (x). Modificare *ptr (la variabile puntata da ptr) EQUIVALE A MODIFICARE X, ed ecco ...
                       // ... che ora nel main (x è stato appena letto con scanf) puoi fare tutto (ad esempio printf("%d\n", x);) Chiaro!
                       // ALTRA COSA IMPORTANTE:
                       // ma l'int * non solo a sinistra dell'uguale, serve anche a destra, perché scanf prende in input solo puntatori, quindi mettere int nelle parentesi, 
                       // seppur corretto in termini di promozione dei tipi, è un ERRORE GRAVE perché leggeresti 4 byte (int) invece di 8 (int*)
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                              Conversione %s                                */
/* -------------------------------------------------------------------------- */
// Legge una stringa (senza spazi) e la scrive nel buffer passato.
int scan_string(FILE *f, va_list ap)
{
    char *str = va_arg(ap, char *);
    int c = fgetc(f);
    int i = 0;

    // Salta spazi iniziali
    while (c != EOF && isspace(c))
        c = fgetc(f);

    // Legge finché trova caratteri non spazi
    while (c != EOF && !isspace(c))
    {
        str[i++] = (char)c; // Stesso discorso. Se avessi voluto usare ptr come negli altri casi sarebbe bastato fare *str++ = (char)c; ma è la stessa cosa dato che una stringa è un array di char.
        c = fgetc(f);       // Perché str[i++] è valido? Perché assumiamo che il chiamante abbia passato un buffer abbastanza grande (es buffer[1000]). Dato che dipende dal chiamante, scanf non è considerata sicura, 
    }

    // Rimette nel buffer il primo spazio o EOF
    if (c != EOF)
        ungetc(c, f);

    str[i] = '\0';

    // Se non ha letto nulla, conversione fallita
    if (i == 0)
        return 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                        Gestione delle conversioni generiche                */
/* -------------------------------------------------------------------------- */
// Determina il tipo di conversione e chiama la funzione corrispondente.

/*
NOTA INTERESSANTE: Perché ci serve un doppio puntatore? 

Pensaci:  se match_conv dovesse 
avanzare format, non potrebbe se da vscanf avessi passato in input format.

Passare un puntatore alla funzione passa solo la sua copia, se vuoi avanzarlo
o retrocederlo (aritmetica dei puntatori) devi passare l'indirizzo del puntatore.
In pratica, devi fare il passaggio PER RIFERIMENTO DEL PUNTATORE.
Infatti in vscanf chiamo match_conv(f, &format, ap), l'indirizzo del
puntatore format.

Ora, questa è una scanf semplificata e non dobbiamo avanzare format 
dentro match_conv, ma se avessimo dovuto gestire anche opzioni tipo 
"%02d", "%ld", "%10s", ecc., allora sì, avrei dovuto avanzare 
(*format) dentro match_conv facendo (*format)++; 

Allora perché lascio **format ? Per portabilità: se in futuro voglio
aggiungere quelle opzioni, non devo modificare i parametri in input.

Chiaro!
*/

int match_conv(FILE *f, const char **format, va_list ap)
{
    switch (**format)
    {
        case 'c':
            // Non metto match_space perché ' ' è un char, se sta nell'input va letto come tale (lo spazio è un carattere valido e va letto così com'è)
            return scan_char(f, ap);
        case 'd':
            match_space(f);
            return scan_int(f, ap);
        case 's':
            match_space(f);
            return scan_string(f, ap);
        case EOF:
            return -1;
        default:
            return -1;
    }
}

/* -------------------------------------------------------------------------- */
/*                      Implementazione interna: ft_vfscanf                   */
/* -------------------------------------------------------------------------- */
// Scansiona l’input in base al formato e ritorna il numero di conversioni riuscite.
int ft_vfscanf(FILE *f, const char *format, va_list ap)
{
    int nconv = 0; // Contatore delle conversioni completate con successo (%d, %s, %c letti).
    int c = fgetc(f);

    // Controllo EOF immediato
    if (c == EOF)
        return EOF;
    ungetc(c, f); // Tutte queste righe servono solo a verificare che il file non sia vuoto

    // Ciclo principale: legge il formato
    while (*format) // format è il puntatore alla format string (es: "%d, %s: %c!  ")
    {
        if (*format == '%')
        {
            format++; // Passi al successivo char di format
            if (match_conv(f, &format, ap) != 1) // Se errore di parsing, break
                break;
            else
                nconv++; // Fatta una conversione
        }
        else if (isspace(*format)) 
        {
            if (match_space(f) != 1) // Se non riesco a saltare tutti gli spazi e returnare ultimo char non spazio letto, break
                break;
        }
        else if (match_char(f, *format) != 1) // Es: se la format string è "Age: %d", l'input dell'utente DEVE essere "Age: " match_char semplicemente verifica che l'user abbia scritto "Age : ". Poi, appena *format == '%' si va a scansionare il formato (qui: scan_int)
            break;
        format++;
    }

    if (ferror(f)) // Definita in stdio.h, sta per "file (stream) error". Restituisce vero se c'è un errore di lettura nel file stream (stdin)
        return EOF; // EOF è definito in varie librerie come -1, quindi significa errore. Mettere -1 o EOF è indifferente.
    return nconv;
}

/* -------------------------------------------------------------------------- */
/*                              Funzione pubblica                             */
/* -------------------------------------------------------------------------- */
// Punto d’ingresso: gestisce gli argomenti variadici e chiama ft_vfscanf.
int ft_scanf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    int ret = ft_vfscanf(stdin, format, ap);

    va_end(ap);
    return ret;
}


/*
NOTA: 
fgets è più sicuro di scanf perché, a diff. di scanf, sa in 
anticipo quanto è grande il buffer in cui deve scrivere.

Esempio:

// char s[5];
// scanf("%s", s);   // ⚠️ se scrivi "ciao!", overflow → crash o comportamento indefinito

scanf("%s") legge fino al primo spazio, ma non limita i caratteri: continuerà a scrivere
anche oltre i 5 byte di s.

fgets, invece, conosce la dimensione del buffer:

// fgets(s, 5, stdin);   // ✅ legge massimo 4 caratteri + '\0'

➡️ quindi fgets è sicura, scanf("%s") no, a meno che tu non specifichi una larghezza massima, tipo:
scanf("%4s", s);   // ✅ legge al massimo 4 caratteri + '\0'
*/

/*
RESOCONTO DELLE FUNZIONI:
Parte-----------Cosa fa-------------------------------------------------------------------------------------------------------------------|
match_space()	Salta tutti gli spazi nell’input. Restituisce -1 se raggiunge EOF.                                                        |
match_char()	Controlla che il prossimo carattere dell’input corrisponda a quello nel formato.                                          |
scan_char()	    Legge un singolo carattere e lo salva nel puntatore fornito.                                                              |
scan_int()	    Legge un intero (con eventuale segno) finché trova cifre, rimette l’ultimo carattere non numerico nel buffer.             |
scan_string()	Legge una sequenza di caratteri non spazi, termina con \0.                                                                |
ft_scanf()	    Scansiona l’intero formato, gestendo %, spazi e caratteri letterali. Restituisce il numero di conversioni riuscite o EOF. |
------------------------------------------------------------------------------------------------------------------------------------------|
*/
