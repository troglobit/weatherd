#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

#include "db.h"
#include "sensor.h"

size_t strlcpy(char *dst, const char *src, size_t dsize);
