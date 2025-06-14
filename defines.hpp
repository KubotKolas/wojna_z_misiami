/* macro debug - działa jak printf, kiedy zdefiniowano
   DEBUG, kiedy DEBUG niezdefiniowane działa jak instrukcja pusta 
   
   używa się dokładnie jak printfa, tyle, że dodaje kolorków i automatycznie
   wyświetla rank

   w związku z tym, zmienna "rank" musi istnieć.

   w printfie: definicja znaku specjalnego "%c[%d;%dm [%d]" escape[styl bold/normal;kolor [RANK]
                                           FORMAT:argumenty doklejone z wywołania debug poprzez __VA_ARGS__
					   "%c[%d;%dm"       wyczyszczenie atrybutów    27,0,37
                                            UWAGA:
                                                27 == kod ascii escape. 
                                                Pierwsze %c[%d;%dm ( np 27[1;10m ) definiuje styl i kolor literek
                                                Drugie   %c[%d;%dm czyli 27[0;37m przywraca domyślne kolory i brak pogrubienia (bolda)
                                                ...  w definicji makra oznacza, że ma zmienną liczbę parametrów
                                            
                                                */
                                                
#pragma once
#include <functional>
#include <mpi.h>
#include <queue>
#include <utility>
#include <vector>

extern int rank;

#ifdef DEBUG
#define debug(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);
#else
#define debug(...) ;
#endif

// makro println - to samo co debug, ale wyświetla się zawsze
#define println(FORMAT,...) printf("%c[%d;%dm [%d]: " FORMAT "%c[%d;%dm\n",  27, (1+(rank/7))%2, 31+(6+rank)%7, rank, ##__VA_ARGS__, 27,0,37);


enum MessType{
    REQ_DOCK,
    REQ_MECH,
    T_DOCK,
    T_MECH
};

enum State{
    INIT,
    IDLE,
    AWAIT_MECH,
    AWAIT_DOCK,
    REPAIR,
    FINISHED
};

// State stan=INIT;
extern State stan;

struct packet{
    int ts;
    int tag;
    int src;
    int data;
};

#define NITEMS 4

extern int MAX_INT;

extern MPI_Datatype MPI_PAKIET_T;

extern int timer;
extern int dock_counter;
extern int mech_counter;
extern int my_priority;

using request = std::pair<int, int>;

extern std::priority_queue<request, std::vector<request>, std::greater<request>> dock_requests;
extern std::priority_queue<request, std::vector<request>, std::greater<request>> mech_requests;

extern int proc_number;
