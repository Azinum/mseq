// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI32 3.14159265359f
#define ARR_SIZE(ARR) ((sizeof(ARR)) / (sizeof(ARR[0])))

typedef void (*callback_func)();

#endif
