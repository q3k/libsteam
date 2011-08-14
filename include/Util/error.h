/* The following code is subject to the terms and conditions defined in the
   file 'COPYING' which is part of the source code distribution. */

#ifndef __U_ERROR_H__
#define __U_ERROR_H__

#include <stdio.h>
#include <errno.h>

// Error and warning and info logging
#define ERROR(x) printf("[e] @%s:%i: %s\n", __FILE__, __LINE__, x); \
if (errno != 0) \
{ \
    fprintf(stderr, "[e] POSIX error %i: ", errno); \
    perror(""); \
    errno = 0; \
}

#endif
