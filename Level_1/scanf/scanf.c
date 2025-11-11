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
int match_char(FILE *f, char c)
{
    int input = fgetc(f);
    
    if (input == EOF)
        return -1;

    if (input == c)
        return 1;
    
    ungetc(input, f); // Arrivo qui se non combacia, quindi torno al char prima
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
    
    if (c == EOF)
        return -1;
    
    char *ptr = va_arg(ap, char *); // “Dammi il prossimo elemento della lista di arg variadici, e interpretalo come un char *, salva il risultato in un char *ptr
    
    *ptr = (char)c; // Ora il valore dell'utente viene assegnato al valore di ptr deferenziato, abbiamo "letto" l'n-esimo valore inserito dall'utente.
    
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

    // rimettiamo l’ultimo carattere non numerico nel buffer
    if (c != EOF)
        ungetc(c, f); // se "123A", il while di lettura cifre si ferma ad A, ma A non può essere consumato, devo tornare indietro per leggerlo, quindi lo rimetto nello stream

    if (digits == 0)
        return 0;

    int *ptr = va_arg(ap, int *);
    *ptr = num * sign;
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
        c = fgetc(f);
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
int match_conv(FILE *f, const char **format, va_list ap)
{
    switch (**format)
    {
        case 'c':
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
    ungetc(c, f);

    // Ciclo principale: legge il formato
    while (*format) // *format è il puntatore alla format string (es: "%d, %s: %c!  ")
    {
        if (*format == '%')
        {
            format++; // Passi al successivo char di format
            if (match_conv(f, &format, ap) != 1)
                break;
            else
                nconv++;
        }
        else if (isspace(*format))
        {
            if (match_space(f) == -1) // Salto tutti gli spazi e returna ultimo char non spazio letto
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