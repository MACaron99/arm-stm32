
#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>


typedef unsigned char   u08;
typedef signed   char   s08;
typedef unsigned short  u16;
typedef signed   short  s16;
typedef unsigned long   u32;
typedef signed   long   s32;

typedef enum { false = 0, true } bool;

#define true                        1
#define false                       0

#define NULL                        ( 0 )


#define ARRAY_ELEMENT_COUNT(x)      (sizeof (x) / sizeof (x[0]))



#endif /* TYPES_H_ */

