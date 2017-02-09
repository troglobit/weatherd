/* Distributed with a free-will license.
 *
 * Use it any way you want, profit or free, provided it fits in the
 * licenses of its associated works.
 *
 * TSL2561
 * This code is designed to work with the TSL2561_I2CS I2C Mini Module
 * available from ControlEverything.com.
 * https://www.controleverything.com/content/Light?sku=TSL2561_I2CS#tabs-0-product_tabset-2
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define I2C_BUS "/dev/i2c-1"

int tsl2561(float *ch0, float *ch1)
{
	int fd;
	char *bus = I2C_BUS;
	char config[2] = {0};
	char reg[1] = {0x0C | 0x80};
	char data[4] = {0};

	fd = open(bus, O_RDWR);
	if (fd < 0)
		return -1;

	/* Get I2C device, TSL2561 I2C address is 0x39(57) */
	ioctl(fd, I2C_SLAVE, 0x39);

	/*
	 * Select control register(0x00 | 0x80)
	 * Power ON mode(0x03)
	 */
	config[0] = 0x00 | 0x80;
	config[1] = 0x03;
	write(fd, config, 2);

	/*
	 * Select timing register(0x01 | 0x80)
	 * Nominal integration time = 402ms(0x02)
	 */
	config[0] = 0x01 | 0x80;
	config[1] = 0x02;
	write(fd, config, 2);
	sleep(1);

	/*
	 * Read 4 bytes of data from register(0x0C | 0x80)
	 * ch0 lsb, ch0 msb, ch1 lsb, ch1 msb
	 */
	write(fd, reg, 1);
	if (read(fd, data, 4) != 4) {
		close(fd);
		errno = EIO;
		return -1;
	}
	close(fd);

	/* Convert the data */
	*ch0 = (data[1] * 256 + data[0]);
	*ch1 = (data[3] * 256 + data[2]);

	return 0;
}
