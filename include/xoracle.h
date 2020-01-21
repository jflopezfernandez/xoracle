
#ifndef PROJECT_INCLUDES_XORACLE_H
#define PROJECT_INCLUDES_XORACLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/random.h>

#if !defined(FALSE) || !defined(TRUE)
enum { FALSE = 0, TRUE = !FALSE };
#endif

#include "base64.h"
#include "memory.h"

#endif /* PROJECT_INCLUDES_XORACLE_H */
