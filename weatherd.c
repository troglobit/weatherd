/* Simple weather monitor daemon using BME280 and TSL2561 from Adafruit
 *
 * Periodically queries sensors and stores resulting data in the db.
 * In verbose mode each sensor reading is also logged to syslog.
 */

#include "common.h"

int main(void)
{
	daemon(0, 0);
	openlog(NULL, LOG_PID, LOG_DAEMON);

	while (1) {
		float ch0, ch1;
		float temp, pressure, humidity;
		struct wdb entry;

		sleep(30);
		if (tsl2561(&ch0, &ch1) || bme280(&temp, &pressure, &humidity))
			continue;

		entry.temperature = temp;
		entry.pressure    = pressure;
		entry.humidity    = humidity;
		entry.illuminance = ch0;
		entry.infrared    = ch1;

		db_put(&entry);
#if 0
		printf("Full Spectrum(IR + Visible) : %.2f lux \n", ch0);
		printf("Infrared Value : %.2f lux \n", ch1);
		printf("Visible Value : %.2f lux \n", (ch0 - ch1));

		printf("Temperature in Celsius : %.2f C \n", temp);
		printf("Temperature in Fahrenheit : %.2f F \n", temp * 1.8 + 32);
		printf("Pressure : %.2f hPa \n", pressure);
		printf("Relative Humidity : %.2f RH \n", humidity);
#endif
	}

	return 0;
}
