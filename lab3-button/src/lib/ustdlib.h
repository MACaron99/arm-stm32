
#ifndef LIB_USTDLIB_H_
#define LIB_USTDLIB_H_


#include <stdarg.h>
#include <stdint.h>
#include <string.h>


int32_t  uvsprintf( char **out, const char *format, va_list argp );
int32_t  usprintf( char *out, const char *format, ... );

int32_t  uprintf(const char *format, ...);


#endif /* LIB_USTDLIB_H_ */

