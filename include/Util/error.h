#ifndef __U_ERROR_H__
#define __U_ERROR_H__

#include <stdio.h>

// Error and warning and info logging
#define ERROR(x) printf("[e] @%s:%i %s\n", __FILE__, __LINE__, x)

#endif