#include "includes.hpp"
#include "defines.hpp"

void *startKomWatek(void *ptr)
{
    MPI_Status status;
    int is_message = false;
    packet pakiet;
    int tid;
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while ( stan!=FINISHED ) {
	debug("czekam na recv");
        MPI_Recv( &pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch ( status.MPI_TAG ) {
	    case REQ_DOCK: 
            // TODO: to implement
	        break;
	    case REQ_MECH: 
            // TODO: to implement
	        break;
        case T_DOCK:
            // TODO: to implement
            break;
        case T_MECH:
            // TODO: to implement
            break;
	    default:
            // Unknown message type; throw an error
            // TODO: to implement
            debug("[!] [id:%d] Malformed request from %d!", tid , status.MPI_SOURCE)

	        break;
        }
    }
}
