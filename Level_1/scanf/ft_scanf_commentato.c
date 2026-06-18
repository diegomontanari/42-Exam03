#include <stdarg.h> // Variadic args
#include <stdio.h> // fgets, scanf, printf, perror ecc...
#include <ctype.h> // Char type, serve per ft che classificano i char (es isalpha, isdigit ecc...)

/* -------------------------------------------------------------------------- */
/*                             Gestione degli spazi                           */
/* -------------------------------------------------------------------------- */
// Funzione che lavora su uno stream (FILE *f) e prova a
// saltare tutti i caratteri di spazio presenti subito nell'input.
int match_space(FILE *f)
{
    int c = fgetc(f); // Legge un carattere dallo stream. fgetc ritorna il carattere come int oppure EOF se fine file/errore.

    while (c != EOF && isspace(c)) // Finché spazio continua a leggere char by char
        c = fgetc(f);

    if (c == EOF)
        return -1; // Segnala che hai "finito" l'input

    ungetc(c, f); // Rimette nell'input l'ultimo carattere non-spazio letto, così non viene perso: la prossima lettura lo vedrà ancora. (Nota: è garantito poter fare almeno un ungetc dell'ultimo char letto). Inoltre, ungetc prende in input sia il carattere che il file (a differenza di fgetc ad esempio) perché deve sapere quale char rimettere e in quale stream inserirlo.
    return 1; // Ritorna 1 per indicare che l'operazione di "skip whitespace" è andata a buon fine e lo stream ora è posizionato sul primo non-spazio (grazie a ungetc).
}

/* -------------------------------------------------------------------------- */
/*                         Match di caratteri letterali                       */
/* -------------------------------------------------------------------------- */
//Legge 1 carattere dall'input.

// Se combacia → lo consuma (non servirà più). (Anche perché vscanf incrementa format
// ad ogni iterazione quindi il FILE deve stare al passo e consumare un carattere)

// Se NON combacia → deve "rimetterlo" nell'input perché non dovrebbe essere stato
// letto → e fallisce la conversione.
// Se la conversione fallisce, ovviamente, scanf deve lasciare l'input intatto,
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

int scan_char(FILE *f, va_list *ap)
{
    int c = fgetc(f); // legge un carattere dal file
                      // Attenzione: fgetc restituisce int perché deve poter restituire sia un carattere valido (0-255) sia EOF che di solito vale -1.
                      // Se restituisse char, non ci sarebbe modo di distinguere EOF da un carattere legittimo con valore 255.

    if (c == EOF)
        return -1;

    char *ptr = va_arg(*ap, char *); // "Dammi il prossimo elemento della lista di arg variadici, e interpretalo come un char *, salva il risultato in un char *ptr

    *ptr = (char)c; // Ora il valore dell'utente viene assegnato al valore di ptr deferenziato, abbiamo "letto" l'n-esimo valore inserito dall'utente. ATTENZIONE: la conversione (char) va fatta esplicita perché i cast impliciti sono non portabili e poco sicuri. Come regola generale: sai che in una riga passi da int a char? Esplicitalo sempre.

    return 1;
}

/* -------------------------------------------------------------------------- */
/*                              Conversione %d                                */
/* -------------------------------------------------------------------------- */
int scan_int(FILE *f, va_list *ap)
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

    // Rimettiamo l'ultimo carattere non numerico nel buffer
    if (c != EOF) // Così perché ungetc non accetta EOF in input.
        ungetc(c, f); // se "123A", il while di lettura cifre si ferma ad A, ma A non può essere consumato, devo tornare indietro per leggerlo, quindi lo rimetto nello stream

    if (digits == 0) // Se la conversione non è andata a buon fine (es: EOF subito nello stream)
        return 0;

    int *ptr = va_arg(*ap, int *);
    *ptr = num * sign;
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                              Conversione %s                                */
/* -------------------------------------------------------------------------- */
// Legge una stringa (senza spazi) e la scrive nel buffer passato.
int scan_string(FILE *f, va_list *ap)
{
    char *str = va_arg(*ap, char *);
    int c = fgetc(f);
    int i = 0;

    // Salta spazi iniziali
    while (c != EOF && isspace(c))
        c = fgetc(f);

    // Legge finché trova caratteri non spazi
    while (c != EOF && !isspace(c))
    {
        str[i++] = (char)c;
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

/*
NOTA INTERESSANTE: Perché ci serve un doppio puntatore?

Pensaci: se match_conv dovesse avanzare format, non potrebbe se da vscanf
avessi passato in input format.

Passare un puntatore alla funzione passa solo la sua copia, se vuoi avanzarlo
o retrocederlo (aritmetica dei puntatori) devi passare l'indirizzo del puntatore.
In pratica, devi fare il passaggio PER RIFERIMENTO DEL PUNTATORE.
Infatti in vscanf chiamo match_conv(f, &format, ap), l'indirizzo del
puntatore format.

Ora, questa è una scanf semplificata e non dobbiamo avanzare format
dentro match_conv, ma se avessimo dovuto gestire anche opzioni tipo
"%02d", "%ld", "%10s", ecc., allora sì, avrei dovuto avanzare
(*format) dentro match_conv facendo (*format)++;

Allora perché lascio **format? Per portabilità: se in futuro voglio
aggiungere quelle opzioni, non devo modificare i parametri in input.

Stesso discorso per va_list *ap: viene passato per puntatore perché
ogni va_arg deve consumare l'originale, non una copia. Se passassi
ap per valore, ogni funzione scan lavorerebbe su una copia e ft_vfscanf
non saprebbe mai dove sei arrivato nella lista degli argomenti variadici.
*/

int match_conv(FILE *f, const char **format, va_list *ap)
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
// Scansiona l'input in base al formato e ritorna il numero di conversioni riuscite.
int ft_vfscanf(FILE *f, const char *format, va_list ap)
{
    int nconv = 0; // Contatore delle conversioni completate con successo (%d, %s, %c letti).
    int c = fgetc(f);

    // Controllo EOF immediato
    if (c == EOF)
        return EOF;
    ungetc(c, f);

    // Ciclo principale: legge il formato
    while (*format)
    {
        if (*format == '%')
        {
            format++;
            if (match_conv(f, &format, &ap) != 1) // &ap: da qui in poi ap viene passato per riferimento a tutte le funzioni scan
                break;
            else
                nconv++;
        }
        else if (isspace(*format))
        {
            if (match_space(f) != 1)
                break;
        }
        else if (match_char(f, *format) != 1)
            break;
        format++;
    }

    if (ferror(f))
        return EOF;
    return nconv;
}

/* -------------------------------------------------------------------------- */
/*                              Funzione pubblica                             */
/* -------------------------------------------------------------------------- */
// Punto d'ingresso: gestisce gli argomenti variadici e chiama ft_vfscanf.
int ft_scanf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);

    int ret = ft_vfscanf(stdin, format, ap);

    va_end(ap);
    return ret;
}

/*
Le funzioni match hanno un compito di verifica: devono confermare che l'input
corrisponde a qualcosa di preciso. Se arriva EOF prima di poter verificare,
la risposta è "non posso sapere, l'input è finito troppo presto" e questo
è un fallimento netto, quindi -1.

Le funzioni scan hanno un compito di raccolta: devono leggere più dati
possibile. scan_string ha letto dei caratteri validi, li ha salvati, li ha
terminati con '\0'. EOF non ha interrotto niente di sbagliato, ha solo
segnalato che non c'è altro da leggere. Il risultato è comunque una stringa
valida, quindi ritorna 1.

Il caso di fallimento di scan_string è diverso: ritorna 0 solo se non ha
letto nemmeno un carattere, cioè EOF è arrivato prima di qualsiasi dato
utile. Lì sì, la conversione è fallita.
*/
