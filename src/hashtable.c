#include "hashtable.h"
#include <string.h>
#include "debug.h"

void
hash_init(DB **db)
{
	CBUG(db_create(db, NULL, 0));
	CBUG((*db)->open(*db, NULL, NULL, NULL, DB_HASH, DB_CREATE, 0644));
}

void
hash_put(DB *db, const char *key_str, void *data_ptr)
{
	DBT key, data;

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));

	key.data = (void *) key_str;
	key.size = strlen(key_str);
	data.data = &data_ptr;
	data.size = sizeof(void *);

	CBUG(db->put(db, NULL, &key, &data, 0));
}

void *
hash_get(DB *db, const char *key_str)
{
	DBT key, data;
	int ret;

	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	key.data = (void *) key_str;
	key.size = strlen(key_str);

	ret = db->get(db, NULL, &key, &data, 0);

	if (ret == DB_NOTFOUND)
		return NULL;

	CBUG(ret);

	return * (void **) data.data;
}

void
hash_delete(DB *db, const char *key_str)
{
	DBT key;

	memset(&key, 0, sizeof(key));

	key.data = (void *) key_str;
	key.size = strlen(key_str);

	CBUG(db->del(db, NULL, &key, 0));
}
