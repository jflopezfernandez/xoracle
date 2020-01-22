
#ifndef PROJECT_INCLUDES_XORACLE_H
#define PROJECT_INCLUDES_XORACLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#include <unistd.h>

#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/random.h>
#include <sys/resource.h>

#if !defined(FALSE) || !defined(TRUE)
enum { FALSE = 0, TRUE = !FALSE };
#endif

#include "base64.h"
#include "memory.h"
#include "rc4.h"
#include "sha1.h"

#endif /* PROJECT_INCLUDES_XORACLE_H */
