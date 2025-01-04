#ifndef NDDB_H
#define NDDB_H

#ifdef __OpenBSD__
#include <db4/db.h>
#else
#include <db.h>
#endif

extern DB_ENV *env;
extern DB_TXN *txnid;

#endif
