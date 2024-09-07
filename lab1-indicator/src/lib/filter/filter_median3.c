
#include <stdint.h>

#include "filter_median3.h"



int32_t filter_median3( int32_t a, int32_t b, int32_t c )
{
    int32_t middle;

   if( (a <= b) && (a <= c) )
   {
      middle = (b <= c) ? b : c;
   }
   else
   {
      if( (b <= a) && (b <= c) )
      {
         middle = (a <= c) ? a : c;
      }
      else
      {
         middle = (a <= b) ? a : b;
      }
   }

   return middle;
}

