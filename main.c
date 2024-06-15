/*
 * Camillini Luca
 * Matricola: 0001078023
 * Gruppo A
 * luca.camillini@studio.unibo.it
 * */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/*struttura di una singola cella della matrice*/
typedef struct Cella{
    char direzione;     /*inizializzata con '\0' ed ha un carattere N,S,W,E che indica la direzione di provenienza*/
    bool bagnato;       /*0 se non colpita da pioggia, 1 se colpita da pioggia*/
    int distanza;       /*inizializzata con -1 e contiene il numero di caselle composte*/
    int valore;         /*valore preso dal file di testo*/
}cella;

/*struttura della coda a priorità*/
typedef int ListInfo;
typedef struct ListNode {
    ListInfo riga;      /*riga della matrice*/
    ListInfo colonna;   /*colonna della matrice*/
    struct ListNode *succ, *pred;
} ListNode;

typedef struct {
    int length;
    ListNode sentinel;
} List;


/*Protocolli*/
ListNode *list_new_node(int , int );
void list_join(ListNode *, ListNode *);
ListNode *list_succ(const ListNode *);
ListNode *list_first(const List *);
void list_insert_after(List *, ListNode *, ListInfo , ListInfo );
void list_add_last(List *, ListInfo , ListInfo );
void list_remove(List *, ListNode *);
void list_clear(List *);
void list_destroy(List *);
ListNode *list_end(const List *);
ListNode *list_pred(const ListNode *);
int list_is_empty(const List *);
void list_remove_first(List *, int*, int*);
List *list_create();


/*Creazione di una matrice di puntatori alla struct cella*/
cella*** creazione_matrice(FILE *filein, int righe, int colonne) {
    int i, j;
    char *carattere = malloc(colonne+righe * sizeof(char));
    cella*** matrice = (cella***)malloc(righe * sizeof(cella**));
    fgets(carattere, colonne, filein);
    for (i = 0; i < righe; i++) {
        matrice[i] = (cella**)malloc(colonne * sizeof(cella*));
        fgets(carattere, colonne+righe, filein);
        for (j = 0; j < colonne; j++) {
            matrice[i][j] = (cella*)malloc(sizeof(cella));
            matrice[i][j]->valore = carattere[j] - 48;
            matrice[i][j]->direzione = '\0';
            matrice[i][j]->bagnato = true;
            matrice[i][j]->distanza = -1;
        }
    }
    matrice[0][0]->distanza = 0;
    free(carattere);


    return matrice;

}


/*Controlla la validità delle caselle, evita gli out_of_bounce */
bool valida(int i, int j, int righe, int colonne){
    return (i >= 0 && i < righe && j >= 0 && j < colonne);
}

/* BFS della matrice che parte dalla casella (0,0) e trova il cammino minimo.
 * La coda, invece che un arco, prende come parametro le coordinate della tabella*/
cella*** bfs( cella*** matrice, int righe, int colonne){
    int rig, col;
    List *q;
    int i;


    /*vettori che permettono di muoversi nelle 4 direzioni incrementando e decrementando gli indici rig e col e scrivendo la direzione in cui si va*/
    int sh_rig[4] = {1, -1, 0, 0};
    int sh_col[4] = {0, 0, 1, -1};
    char ch[4] = {'S','N', 'E', 'W' };

    q = list_create();
    list_add_last(q, 0, 0);

    while ( ! list_is_empty(q) ) {
        list_remove_first(q, &rig, &col);
        for ( i = 0; i < 4; i++) {
            /*controllo degli out_of_bounce*/
            if(valida(rig + sh_rig[i], col+sh_col[i], righe, colonne) ){
                /*controllo se la casella dove mi muovo è un marcapiede e se non ci sono ancora passato*/
                if(matrice[rig + sh_rig[i]][col + sh_col[i]]->valore == 0 && (matrice[rig + sh_rig[i]][col + sh_col[i]]->distanza == -1)){
                    matrice[rig + sh_rig[i]][col + sh_col[i]]->distanza = matrice[rig][col]->distanza + 1;
                    matrice[rig + sh_rig[i]][col + sh_col[i]]->direzione = ch[i];

                    list_add_last(q, rig + sh_rig[i], col + sh_col[i]);

                }
            }
        }

    }

    list_destroy(q);
    return matrice;
}

/*Calcola le caselle non bagnate dalla pioggia*/
cella*** pioggia(cella*** matrice, int righe, int colonne){
    int i, j, k;
    for(i = 0; i < righe; i++){
        for (j = 0; j < colonne; j++) {
            if(matrice[i][j]->valore != 0){
                for (k = matrice[i][j]->valore; k != 0 ; k--) {
                    if(valida(i, j+k, righe, colonne)){
                        matrice[i][j+k]->bagnato = false;
                    }
                }
            }
        }

    }

    return matrice;
}

/*Funzione ricorsiva che stampa il percorso e il numero di caselle bagnate*/
void stampa_matrice(cella*** matrice, int righe, int colonne, int count){

    /*conteggio caselle bagnate attraversate*/
    if(matrice[righe][colonne]->bagnato){
        count++;
    }
    /*caso base, ovvero se mi trovo nella casella (0,0)*/
    if(righe == 0 && colonne == 0){
        printf("%d\n", count);
        return;
    }else{
        switch (matrice[righe][colonne]->direzione) {
            case 'N':
                righe++;
                stampa_matrice(matrice, righe, colonne, count);
                printf("N");
                break;

            case 'S':
                righe--;
                stampa_matrice(matrice, righe, colonne, count);
                printf("S");
                break;

            case 'E':
                colonne--;
                stampa_matrice(matrice, righe, colonne, count);
                printf("E");
                break;

            case 'W':
                colonne++;
                stampa_matrice(matrice, righe, colonne, count);
                printf("W");
                break;
            /*caso in cui non si è trovato un cammino*/
            case '\0':printf("-1");
                return;
        }
    }
}



int main(int argc, char* argv[]){
    FILE* filein = stdin;
    int righe, colonne, i, j;
    cella*** matrice;

    if (argc != 2) {
        fprintf(stderr, "Usare: %s input_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    filein = fopen(argv[1], "r");

    if (filein == NULL) {
        fprintf(stderr, "Can not open \"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }
    fscanf(filein, "%d", &righe);
    fflush(stdin);
    fscanf(filein, "%d", &colonne);
    fflush(stdin);

    matrice = creazione_matrice(filein, righe, colonne);

    fclose(filein);

    matrice = bfs(matrice,righe, colonne);

    matrice = pioggia(matrice, righe, colonne);

    /*stampa della distanza percorsa*/
    if(matrice[righe-1][colonne-1]->distanza == -1){
        printf("%d      ", matrice[righe-1][colonne-1]->distanza);
    }else{
        printf("%d      ", matrice[righe-1][colonne-1]->distanza+1);
    }

    stampa_matrice(matrice, righe-1, colonne-1, 0);

    /*liberazione memoria matrice*/
    for (i = 0; i < righe; i++) {
        for (j = 0; j < colonne; j++) {
            free(matrice[i][j]);
        }
        free(matrice[i]);
    }
    free(matrice);

    return EXIT_SUCCESS;
}


/*funzioni della coda a priorità*/

ListNode *list_new_node(int rig, int col){
    ListNode *r = (ListNode *)malloc(sizeof(ListNode));
    assert(r != NULL); /* evitiamo un warning con VS */
    r->riga = rig;
    r->colonna = col;
    r->succ = r->pred = r;
    return r;
}

void list_join(ListNode *pred, ListNode *succ){
    assert(pred != NULL);
    assert(succ != NULL);

    pred->succ = succ;
    succ->pred = pred;
}

ListNode *list_succ(const ListNode *n){
    assert(n != NULL);

    return n->succ;
}

ListNode *list_first(const List *L){
    assert(L != NULL);

    return L->sentinel.succ;
}

void list_insert_after(List *L, ListNode *n, ListInfo riga, ListInfo colonna){
    ListNode *new_node, *succ_of_n;
    assert(L != NULL);
    assert(n != NULL);

    new_node = list_new_node(riga, colonna);
    succ_of_n = list_succ(n);
    list_join(n, new_node);
    list_join(new_node, succ_of_n);
    L->length++;
}

void list_add_last(List *L, ListInfo riga, ListInfo colonna){
    assert(L != NULL);

    list_insert_after(L, L->sentinel.pred, riga, colonna);
}

void list_remove(List *L, ListNode *n){
    assert(L != NULL);
    assert(n != NULL);
    assert(n != list_end(L));
    list_join(list_pred(n), list_succ(n));
    free(n);
    L->length--;
}

void list_clear(List *L){
    ListNode *node;

    assert(L != NULL);

    node = list_first(L);
    while (node != list_end(L)) {
        ListNode *succ = list_succ(node);
        free(node);
        node = succ;
    }
    L->length = 0;
    L->sentinel.pred = L->sentinel.succ = &(L->sentinel);
}

void list_destroy(List *L){
    list_clear(L);
    free(L);
}

ListNode *list_end(const List *L){
    assert(L != NULL);

    return (ListNode*)(&(L->sentinel));
}

ListNode *list_pred(const ListNode *n){
    assert(n != NULL);

    return n->pred;
}

int list_is_empty(const List *L){
    assert(L != NULL);

    return (list_first(L) == list_end(L));
}

void list_remove_first(List *L, int *rig, int *col){
    ListNode *first;

    assert( !list_is_empty(L) );

    first = list_first(L);
    *rig = first->riga;
    *col = first->colonna;
    list_remove(L, first);
    return;
}

List *list_create( void ){
    List *L = (List*)malloc(sizeof(List));
    assert(L != NULL);

    L->length = 0;
    L->sentinel.pred = L->sentinel.succ = &(L->sentinel);
    return L;
}


