#include "common.h"

int main(int argc, char *argv[])
{
	struct wdb entry;

	openlog(NULL, LOG_PID, LOG_DAEMON);

	if (db_get(&entry))
		return 1;

	printf("%s: temp %.1f °C, pressure %.1f hPa, humidity %.1f %%RH, light %.1f lux\n",
	       entry.timestamp, entry.temperature, entry.pressure, entry.humidity,
	       entry.illuminance - entry.infrared);

	return 0;
}
