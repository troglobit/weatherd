/* Distributed with a free-will license.
 *
 * Use it any way you want, profit or free, provided it fits in the
 * licenses of its associated works.
 *
 * BME280
 * This code is designed to work with the BME280_I2CS I2C Mini Module
 * available from ControlEverything.com.
 * https://www.controleverything.com/content/Humidity?sku=BME280_I2CS#tabs-0-product_tabset-2
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define I2C_BUS "/dev/i2c-1"

int bme280(float *temp, float *pressure, float *humidity)
{
	int fd;
	int dig_T1, dig_T2, dig_T3;
	int dig_P1, dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
	int dig_H1, dig_H2, dig_H3, dig_H4, dig_H5, dig_H6;
	long adc_p, adc_t, adc_h;
	float var1, var2, t_fine, p, var_H;
	char *bus = I2C_BUS;
	char reg[1] = {0x88};
	char data[8] = {0};
	char b1[24] = {0};
	char config[2] = {0};

	fd = open(bus, O_RDWR);
	if (fd < 0)
		return -1;

	/* BME280 I2C address defaults to 0x77(137) */
	ioctl(fd, I2C_SLAVE, 0x77);

	/* Read 24 bytes of data from register(0x88) */
	write(fd, reg, 1);
	if (read(fd, b1, 24) != 24) {
		close(fd);
		errno = EIO;
		return -1;
	}

	/* Convert the data */

	/* temp coefficents */
	dig_T1 = (b1[0] + b1[1] * 256);
	dig_T2 = (b1[2] + b1[3] * 256);
	if (dig_T2 > 32767)
		dig_T2 -= 65536;

	dig_T3 = (b1[4] + b1[5] * 256);
	if (dig_T3 > 32767)
		dig_T3 -= 65536;

	/* pressure coefficents */
	dig_P1 = (b1[6] + b1[7] * 256);
	dig_P2 = (b1[8] + b1[9] * 256);
	if (dig_P2 > 32767)
		dig_P2 -= 65536;

	dig_P3 = (b1[10] + b1[11] * 256);
	if (dig_P3 > 32767)
		dig_P3 -= 65536;

	dig_P4 = (b1[12] + b1[13] * 256);
	if (dig_P4 > 32767)
		dig_P4 -= 65536;

	dig_P5 = (b1[14] + b1[15] * 256);
	if (dig_P5 > 32767)
		dig_P5 -= 65536;

	dig_P6 = (b1[16] + b1[17] * 256);
	if (dig_P6 > 32767)
		dig_P6 -= 65536;

	dig_P7 = (b1[18] + b1[19] * 256);
	if (dig_P7 > 32767)
		dig_P7 -= 65536;

	dig_P8 = (b1[20] + b1[21] * 256);
	if (dig_P8 > 32767)
		dig_P8 -= 65536;

	dig_P9 = (b1[22] + b1[23] * 256);
	if (dig_P9 > 32767)
		dig_P9 -= 65536;

	/* Read 1 byte of data from register(0xA1) */
	reg[0] = 0xA1;
	write(fd, reg, 1);
	read(fd, data, 1);
	dig_H1 = data[0];

	/* Read 7 bytes of data from register(0xE1) */
	reg[0] = 0xE1;
	write(fd, reg, 1);
	read(fd, b1, 7);

	/* Convert the data */

	/* humidity coefficents */
	dig_H2 = (b1[0] + b1[1] * 256);
	if (dig_H2 > 32767)
		dig_H2 -= 65536;

	dig_H3 = b1[2] & 0xFF ;
	dig_H4 = (b1[3] * 16 + (b1[4] & 0xF));
	if (dig_H4 > 32767)
		dig_H4 -= 65536;

	dig_H5 = (b1[4] / 16) + (b1[5] * 16);
	if (dig_H5 > 32767)
		dig_H5 -= 65536;

	dig_H6 = b1[6];
	if (dig_H6 > 127)
		dig_H6 -= 256;

	/*
	 * Select control humidity register(0xF2)
	 * Humidity over sampling rate = 1(0x01)
	 */
	config[0] = 0xF2;
	config[1] = 0x01;
	write(fd, config, 2);

	/*
	 * Select control measurement register(0xF4)
	 * Normal mode, temp and pressure over sampling rate = 1(0x27)
	 */
	config[0] = 0xF4;
	config[1] = 0x27;
	write(fd, config, 2);

	/*
	 * Select config register(0xF5)
	 * Stand_by time = 1000 ms(0xA0)
	 */
	config[0] = 0xF5;
	config[1] = 0xA0;
	write(fd, config, 2);

	/*
	 * Read 8 bytes of data from register(0xF7):
	 * pressure msb1, pressure msb, pressure lsb, temp msb1, temp
	 * msb, temp lsb, humidity lsb, humidity msb
	 */
	reg[0] = 0xF7;
	write(fd, reg, 1);
	read(fd, data, 8);

	close(fd);		/* Done with i2c bus */

	/* Convert pressure and temperature data to 19-bits */
	adc_p = ((long)(data[0] * 65536 + ((long)(data[1] * 256) + (long)(data[2] & 0xF0)))) / 16;
	adc_t = ((long)(data[3] * 65536 + ((long)(data[4] * 256) + (long)(data[5] & 0xF0)))) / 16;

	/* Convert the humidity data */
	adc_h = (data[6] * 256 + data[7]);

	/* Temperature offset calculations => Celsius degrees */
	var1 = (((float)adc_t)  / 16384.0  - ((float)dig_T1) / 1024.0)  * ((float)dig_T2);
	var2 = ((((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0)  *
		(((float)adc_t) / 131072.0 - ((float)dig_T1) / 8192.0)) * ((float)dig_T3);
	t_fine = (long)(var1 + var2);
	*temp = (var1 + var2) / 5120.0; /* Fahrenheit = temp * 1.8 + 32 */

	/* Pressure offset calculations */
	var1 = ((float)t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((float)dig_P6) / 32768.0;
	var2 = var2 + var1 * ((float)dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((float)dig_P4) * 65536.0);
	var1 = (((float) dig_P3) * var1 * var1 / 524288.0 + ((float) dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((float)dig_P1);
	p = 1048576.0 - (float)adc_p;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((float) dig_P9) * p * p / 2147483648.0;
	var2 = p * ((float) dig_P8) / 32768.0;
	*pressure = (p + (var1 + var2 + ((float)dig_P7)) / 16.0) / 100;

	/* Humidity offset calculations */
	var_H = (((float)t_fine) - 76800.0);
	var_H = (adc_h - (dig_H4 * 64.0 + dig_H5 / 16384.0 * var_H)) * (dig_H2 / 65536.0 * (1.0 + dig_H6 / 67108864.0 * var_H * (1.0 + dig_H3 / 67108864.0 * var_H)));
	*humidity = var_H * (1.0 -  dig_H1 * var_H / 524288.0);

	if (*humidity > 100.0)
		*humidity = 100.0;
	if (*humidity < 0.0) 
		*humidity = 0.0;

	return 0;
}
