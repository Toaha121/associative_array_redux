#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "aarray.h"


/** utility function to format a hex number */
static char toHex(int val)
{
	if (val < 10) return (char) ('0' + val);
	return (char) ('a' + (val - 10));
}

/**
 * Provide the key in a printable form.  Uses a static buffer,
 * which means that not only is this not thread-safe, but
 * even runs into trouble if called twice in the same printf().
 *
 * That said, is does provide a memory clean way to give a 
 * printable string return value to the calling code
 */
int
printableKey(char *buffer, int bufferlen, AAKeyType key, size_t keylen)
{
	int i, allChars = 1;
	char *loadptr;


	for (i = 0; allChars && i < keylen; i++) {
		if ( ! isprint(key[i])) allChars = 0;
	}

	if (allChars) {
		snprintf(buffer, bufferlen, "char key:[%s]", (char *) key);
	} else {
		snprintf(buffer, bufferlen, "hex key:[0x");
		loadptr = &buffer[strlen(buffer)];
		for (i = 0; i < keylen && loadptr - buffer < bufferlen - 4; i++) {
			*loadptr++ = toHex((key[i] & 0xf0) >> 4); // top nybble -> first hext digit
			*loadptr++ = toHex(key[i] & 0x0f);        // bottom nybble -> second digit
		}
		*loadptr++ = ']';
		*loadptr++ = 0;
	}
	return 1;
}

