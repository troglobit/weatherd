
#include <stdio.h>
#include "sensor.h"

int main(void)
{
	float ch0, ch1;
	float temp, pressure, humidity;

	if (!tsl2561(&ch0, &ch1)) {
		printf("Full Spectrum(IR + Visible) : %.2f lux \n", ch0);
		printf("Infrared Value : %.2f lux \n", ch1);
		printf("Visible Value : %.2f lux \n", (ch0 - ch1));
	}

	if (!bme280(&temp, &pressure, &humidity)) {
		printf("Temperature in Celsius : %.2f C \n", temp);
		printf("Temperature in Fahrenheit : %.2f F \n", temp * 1.8 + 32);
		printf("Pressure : %.2f hPa \n", pressure);
		printf("Relative Humidity : %.2f RH \n", humidity);
	}

	return 0;
}
