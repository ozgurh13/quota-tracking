# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <string.h>

# include <sqlite3.h>

# include "database.h"


/*
 * CREATE TABLE hotspot_quota_tracking(
 *     id_number         INT        AUTO_INCREMENT     PRIMARY KEY,
 *     username          TEXT                             NOT NULL,
 *     upload_bytes      INT                              NOT NULL,
 *     download_bytes    INT                              NOT NULL,
 *     mac_address       TEXT                             NOT NULL,
 *     is_device         INT                              NOT NULL,
 *     quota_day         TEXT                             NOT NULL
 * );
 */




# define  __SQL_VERBOSE__  false

# define  SQL_ERROR_MSG(errmsg)                                     \
    do {                                                            \
        fprintf(stderr, "SQL error: %s\n", errmsg);                 \
        sqlite3_free(errmsg);                                       \
    } while (0);

# define  SQL_VERBOSE(format, ...)                                  \
    do {                                                            \
        if (__SQL_VERBOSE__)                                        \
            fprintf(stdout, format, __VA_ARGS__);                   \
    } while (0);

# define  SQL_TRY(function, errstr)                                 \
    do {                                                            \
        if (function != SQLITE_OK)                                  \
            SQL_ERROR_MSG(errstr)                                   \
    } while (0);




static sqlite3 *db;
static const char* sql_database = "hotspot_quota_tracking.db";
static char* sql_error_msg;            // error messages from sqlite3

void
db_open()
{
	sqlite3_open(sql_database, &db);
}



static int
sql_callback(void *result, int argc, char* argv[], char* columns[])
{
	int *count = (int *) result;

	for (int i = 0; i < argc; i++)
		if (strcmp("COUNT(*)", columns[i]) == 0)
			*count = atoi(argv[i]);

	return 0;
}

static int
sql_select(const char* string)
{
	int result = 0;
	char sqlstr_select[128];

	sprintf( sqlstr_select
	       , "SELECT COUNT(*) FROM hotspot_quota_tracking WHERE mac_address = '%s';"
	       , string );

	SQL_VERBOSE("sql select = %s\n", sqlstr_select)

	SQL_TRY( sqlite3_exec(db, sqlstr_select, sql_callback, (void *) &result, &sql_error_msg)
	       , sql_error_msg )

	return result;
}


static void
sql_insert(const char* string, const size_t upload, const size_t download)
{
	char sqlstr_insert[256];
	sprintf( sqlstr_insert
	       , "INSERT INTO hotspot_quota_tracking "
	         "(username, upload_bytes, download_bytes, mac_address, is_device, quota_day) "
	         "VALUES ('---', %zu, %zu, '%s', 1, 'yyyy-mm-dd');"
	       , upload, download, string );

	SQL_VERBOSE("sql insert = %s\n\n", sqlstr_insert)

	SQL_TRY(sqlite3_exec(db, sqlstr_insert, NULL, NULL, &sql_error_msg), sql_error_msg)
}


static void
sql_update(const char* string, const size_t upload, const size_t download)
{
	char sqlstr_update[256];

	// update upload & download
	sprintf( sqlstr_update
	       , "UPDATE hotspot_quota_tracking "
	         "SET upload_bytes = upload_bytes + %zu, download_bytes = download_bytes + %zu "
	         "WHERE mac_address = '%s';"
	       , upload, download, string );

	SQL_VERBOSE("sql update = %s\n\n", sqlstr_update)

	SQL_TRY(sqlite3_exec(db, sqlstr_update, NULL, NULL, &sql_error_msg), sql_error_msg)
}






/*
 * this is the function that handles writing to database
 */
void
db_write(const kv_t kv)
{
	if (sql_select(kv.key) == 0)         // if no values are present, then INSERT
		sql_insert(kv.key, kv.up_value, kv.down_value);
	else                                  // else UPDATE
		sql_update(kv.key, kv.up_value, kv.down_value);
}

