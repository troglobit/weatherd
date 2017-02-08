/* simulated monitor app */
#include "common.h"

int main(int argc, char *argv[])
{
	int i;
	struct empty {
		float temp;
		float pres;
		float humi;
		float illu;
		float infr;
	} vals[10] = {
		{ 1.0, 2.0, 3.0, 4.0, 5.0 },
		{ 1.1, 2.1, 3.1, 4.1, 5.1 },
		{ 1.2, 2.2, 3.2, 4.2, 5.2 },
		{ 1.3, 2.3, 3.3, 4.3, 5.3 },
		{ 1.4, 2.4, 3.4, 4.4, 5.4 },
		{ 1.5, 2.5, 3.5, 4.5, 5.5 },
		{ 1.6, 2.6, 3.6, 4.6, 5.6 },
		{ 1.7, 2.7, 3.7, 4.7, 5.7 },
		{ 1.8, 2.8, 3.8, 4.8, 5.8 },
		{ 1.9, 2.9, 3.9, 4.9, 5.9 },
	};

	openlog(NULL, LOG_PID, LOG_DAEMON);

	for (i = 0; i < 10; i++) {
		struct wdb entry;

		entry.temperature = vals[i].temp;
		entry.pressure    = vals[i].pres;
		entry.humidity    = vals[i].humi;
		entry.illuminance = vals[i].illu;
		entry.infrared    = vals[i].infr;

		db_put(&entry);
	}

	return 0;
}
