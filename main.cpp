#include "functions.hpp"

int main(int argc, char **argv)
{
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    checkThreadSupport(provided);
    sleep(1);
    MPI_Comm_size(MPI_COMM_WORLD, &number_processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_num);
    srand(p_num);
    initializePacket();
    pthread_create(&threadComm, NULL, startCommThread, 0);
    mainLoop(docks, mechanics, number_processes);
    pthread_join(threadComm, NULL);
    MPI_Type_free(&MPI_PACKET_T);
    MPI_Finalize();
    return 0;
}

void checkThreadSupport(int provided)
{
    printf("THREAD SUPPORT: chcemy %d. Co otrzymamy?\n", provided);
    switch (provided)
    {
    case MPI_THREAD_SINGLE:
        printf("Brak wsparcia dla watkow, koncze\n");
        fprintf(stderr, "Brak wystarczajacego wsparcia dla watkow - wychodze!\n");
        MPI_Finalize();
        exit(-1);
        break;
    case MPI_THREAD_FUNNELED:
        printf("Tylko te watki, ktore wykonaly mpi_init_thread moga wykonac wolania do biblioteki mpi\n");
        break;
    case MPI_THREAD_SERIALIZED:
        printf("Tylko jeden watek naraz moze wykonac wolania do biblioteki MPI\n");
        break;
    case MPI_THREAD_MULTIPLE:
        printf("Pelne wsparcie dla watkow\n");
        break;
    default:
        printf("Nikt nic nie wie\n");
    }
}

void initializePacket()
{
    int blocklengths[NITEMS] = {1, 1, 1, 1};
    MPI_Datatype typy[NITEMS] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint offsets[NITEMS];
    offsets[0] = offsetof(Packet, timestamp);
    offsets[1] = offsetof(Packet, tag);
    offsets[2] = offsetof(Packet, source);
    offsets[3] = offsetof(Packet, data);
    MPI_Type_create_struct(NITEMS, blocklengths, offsets, typy, &MPI_PACKET_T);
    MPI_Type_commit(&MPI_PACKET_T);
}