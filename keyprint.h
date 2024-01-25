#ifndef __PRINTABLE_BINARY_KEY_HEADER__
#define __PRINTABLE_BINARY_KEY_HEADER__

#include "aarray.h"


/**
 * Fill a buffer with the key value
 */
int printableKey(char *buffer, int bufferlen, AAKeyType key, size_t keylen);

#endif
