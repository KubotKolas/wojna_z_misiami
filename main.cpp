#include "includes.hpp"
#include "defines.hpp"
#include "thread_comm.hpp"
#include "thread_main.hpp"
#include <mpi.h>

void check_thread_support(int provided);
void inicjuj_typ_pakietu();

int rank, size;
pthread_t threadKom;

int main(int argc, char **argv)
{
    MPI_Status status;
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    check_thread_support(provided);
    srand(rank);

    inicjuj_typ_pakietu();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pthread_create(&threadKom, NULL, startKomWatek, 0);

    int docks = 4;
    int mechanics = 20;
    MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
    mainLoop(docks, mechanics, proc_number);

    pthread_join(threadKom, NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
    return 0;
}

void check_thread_support(int provided)
{
    // printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided)
    {
    case MPI_THREAD_SINGLE:
        // printf("Brak wsparcia dla wątków, kończę\n");/
        // fprintf(stderr, "Brak wystarczającego wsparcia dla wątków - wychodzę!\n");
        MPI_Finalize();
        exit(-1);
        break;
    case MPI_THREAD_FUNNELED:
        // printf("tylko te wątki, ktore wykonaly mpi_init_thread mogą wykonać wołania do biblioteki mpi\n");
        break;
    case MPI_THREAD_SERIALIZED:
        // printf("tylko jeden watek naraz może wykonać wołania do biblioteki MPI\n");
        break;
    case MPI_THREAD_MULTIPLE:
        // printf("Pełne wsparcie dla wątków\n");
        break;
    default:
        // printf("Nikt nic nie wie\n");
        break;
    }
}

void inicjuj_typ_pakietu()
{
    int blocklengths[NITEMS] = {1, 1, 1, 1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[NITEMS];
    offsets[0] = offsetof(packet, ts);
    offsets[1] = offsetof(packet, tag);
    offsets[2] = offsetof(packet, src);
    offsets[3] = offsetof(packet, data);
    offsets[4] = offsetof(packet, rndid);

    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PAKIET_T);

    MPI_Type_commit(&MPI_PAKIET_T);
}