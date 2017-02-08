/* db interaction */
#include <sqlite3.h>
#include "common.h"

//#define DB    "/var/lib/weather.db"
#define DB    "/tmp/weather.db"
#define TABLE "WEATHER"


static int create_table(sqlite3 *db)
{
	char *errmsg = 0;
	int rc;
	char *sql;

	/*
	 * temperature in degrees Celcius
	 * luminosity in Lux
	 * pressure in hPa
	 * humidity is relative (%RH)
	 *
	 * ... because science, bitches.
	 */
	sql = "CREATE TABLE WEATHER("
		"ID             INTEGER PRIMARY KEY AUTOINCREMENT,"
		"PRESSURE       REAL    NOT NULL,"
		"TEMPERATURE    REAL    NOT NULL,"
		"HUMIDITY       REAL    NOT NULL,"
		"ILLUMINANCE    REAL    NOT NULL,"
		"INFRARED       REAL    NOT NULL,"
		"TIMESTAMP      DATETIME DEFAULT CURRENT_TIMESTAMP"
		");";

	rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
	if (rc != SQLITE_OK) {
		if (!strstr(errmsg, "already exists"))
			syslog(LOG_ERR, "SQL error %d: %s", rc, errmsg);

		sqlite3_free(errmsg);
	}

	return rc;
}

int db_put(struct wdb *entry)
{
	int rc;
	char sql[256];
	sqlite3 *db;
	char *errmsg;

	rc = sqlite3_open(DB, &db);
	if (rc) {
		syslog(LOG_ERR, "Failed opening database: %s", sqlite3_errmsg(db));
		return 1;
	}

	create_table(db);

	snprintf(sql, sizeof(sql),
		 "INSERT INTO WEATHER (PRESSURE, TEMPERATURE, HUMIDITY, ILLUMINANCE, INFRARED) "
		 "VALUES (%f, %f, %f, %f, %f);", entry->pressure, entry->temperature,
		 entry->humidity, entry->illuminance, entry->infrared);

	rc = sqlite3_exec(db, sql, NULL, 0, &errmsg);
	if (rc != SQLITE_OK)  {
		syslog(LOG_ERR, "SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}

	sqlite3_close(db);

	return 0;
}

static int callback(void *data, int argc, char **argv, char **colv)
{
	struct wdb *entry = (struct wdb *)data;

	if (argc < 7)
		return -1;

	/*
	 * The SQL in db_get() gives us the following one-line record:
	 * "ID PRESSURE TEMPERATURE HUMIDITY ILLUMINANCE INFRARED TIMESTAMP
	 */
	entry->id          = atoi(argv[0]);
	entry->pressure    = atof(argv[1]);
	entry->temperature = atof(argv[2]);
	entry->humidity    = atof(argv[3]);
	entry->illuminance = atof(argv[4]);
	entry->infrared    = atof(argv[5]);
	strlcpy(entry->timestamp, argv[6], sizeof(entry->timestamp));

	return 0;
}

int db_get(struct wdb *entry)
{
	int rc;
	sqlite3 *db;
	char *errmsg = 0;

	rc = sqlite3_open(DB, &db);
	if (rc) {
		syslog(LOG_ERR, "Failed opening database: %s", sqlite3_errmsg(db));
		return 1;
	}

	rc = sqlite3_exec(db,"select * from " TABLE " order by ID desc limit 1;",
			  callback, (void *)entry, &errmsg);
	if (rc != SQLITE_OK) {
		syslog(LOG_ERR, "SQL error: %s", errmsg);
		sqlite3_free(errmsg);
	}
	sqlite3_close(db);

	return 0;
}

/**
 * Local Variables:
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
