#ifndef _MPI_H
#define _MPI_H

#include "command.h"

/* MPI msgparse.c header file. */

#define MPI_ISBLESSED	0x10

#endif /* _MPI_H */

#ifdef DEFINE_HEADER_VERSIONS

#ifndef mpih_version
#define mpih_version
const char *mpi_h_version = "$RCSfile$ $Revision: 1.9 $";
#endif
#else
extern const char *mpi_h_version;
#endif

