#include "includes.hpp"
#include "defines.hpp"
#include "thread_comm.hpp"
#include "thread_main.hpp"
#include <mpi.h>

void check_thread_support(int provided);
void inicjuj_typ_pakietu();

int rank, size;
pthread_t threadKom;

// TODO: cleanup, retrofit
int main(int argc, char **argv){
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);
    /* zob. util.c oraz util.h */
    inicjuj_typ_pakietu(); // tworzy typ pakietu
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /* startKomWatek w watek_komunikacyjny.c 
     * w vi najedź kursorem na nazwę pliku i wciśnij klawisze gf
     * powrót po wciśnięciu ctrl+6
     * */
    pthread_create( &threadKom, NULL, startKomWatek , 0);

    /* mainLoop w watek_glowny.c 
     * w vi najedź kursorem na nazwę pliku i wciśnij klawisze gf
     * powrót po wciśnięciu ctrl+6
     * */
    
    
     //  TODO: pobrać dane z terminala

    int d = 4;
    int m = 15;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
    mainLoop(d, m, proc_number); // możesz także wcisnąć ctrl-] na nazwie funkcji
		// działa, bo używamy ctags (zob Makefile)
		// jak nie działa, wpisz set tags=./tags :)
    
    pthread_join(threadKom,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
    return 0;
}

void check_thread_support(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided) {
        case MPI_THREAD_SINGLE: 
            printf("Brak wsparcia dla wątków, kończę\n");
            /* Nie ma co, trzeba wychodzić */
	    fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
	    MPI_Finalize();
	    exit(-1);
	    break;
        case MPI_THREAD_FUNNELED: 
            printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
	    break;
        case MPI_THREAD_SERIALIZED: 
            /* Potrzebne zamki wokół wywołań biblioteki MPI */
            printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
	    break;
        case MPI_THREAD_MULTIPLE: printf("Pełne wsparcie dla wątków\n"); /* tego chcemy. Wszystkie inne powodują problemy */
	    break;
        default: printf("Nikt nic nie wie\n");
    }
}

void inicjuj_typ_pakietu()
{
    /* Stworzenie typu */
    /* Poniższe (aż do MPI_Type_commit) potrzebne tylko, jeżeli
       brzydzimy się czymś w rodzaju MPI_Send(&typ, sizeof(pakiet_t), MPI_BYTE....
    */
    /* sklejone z stackoverflow */
    int       blocklengths[NITEMS] = {1,1,1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint     offsets[NITEMS]; 
    offsets[0] = offsetof(packet, ts);
    offsets[1] = offsetof(packet, tag);
    offsets[2] = offsetof(packet, src);
    offsets[3] = offsetof(packet, data);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}