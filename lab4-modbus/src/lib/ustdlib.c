/*
    Copyright 2001-2021 Georges Menie
    https://www.menie.org/georges/embedded/small_printf_source_code.html

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
    putchar is the only external dependency for this file,
    if you have a working putchar, just remove the following
    define. If the function should be called something else,
    replace outbyte(c) by your own function call.
*/

#include <stdint.h>

#include "ustdlib.h"


//#define putchar(c)                     outbyte(c)


#define PAD_RIGHT                      ( 1 << 0 )
#define PAD_ZERO                       ( 1 << 1 )

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN                  (     12 )



static void
printchar( char **str, int c )
{
    extern int putchar(int c);

    if( str )
    {
        **str = c;
        ++(*str);
    }
    else
    {
        (void)putchar(c);
    }
}



static int32_t
prints( char **out, const char *string, int32_t width, int32_t pad )
{
    int32_t  pc = 0, padchar = ' ';

    if( width > 0 )
    {
        int32_t  len = 0;
        const char *ptr;

        for( ptr = string; *ptr; ++ptr )
            ++len;

        if( len >= width )
        {
            width = 0;
        }
        else
        {
            width -= len;
        }

        if( pad & PAD_ZERO )
            padchar = '0';
    }

    if( !(pad & PAD_RIGHT) )
    {
        for( ; width > 0; --width )
        {
            printchar(out, padchar);
            ++pc;
        }
    }

    for( ; *string ; ++string )
    {
        printchar(out, *string);
        ++pc;
    }

    for( ; width > 0; --width )
    {
        printchar(out, padchar);
        ++pc;
    }

    return pc;
}


static int32_t
printi( char **out, int32_t i, int32_t b, int32_t sg, int32_t width, int32_t precision, int32_t pad, int32_t letbase )
{
    char  print_buf[PRINT_BUF_LEN];
    char  *s;
    int32_t  t, neg = 0, pc = 0;
    uint32_t  u = i;
    int32_t  digit_count = 0;

    if( sg && b == 10 && i < 0 )
    {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    do
    {
        t = u % b;

        if( t >= 10 )
            t += letbase - '0' - 10;

        *--s = t + '0';
        u /= b;

        if( precision == ++digit_count )
            *--s = '.';

    } while( u || (precision > digit_count) );

    if( (precision > 0) && (*s == '.') )
        *--s = '0';

    if( neg )
    {
        if( width && (pad & PAD_ZERO) )
        {
            printchar (out, '-');
            ++pc;
            --width;
        }
        else
        {
            *--s = '-';
        }
    }

    return pc + prints(out, s, width, pad);
}


int32_t
uvsprintf( char **out, const char *format, va_list argp )
{
    int32_t width;
    int32_t precision;
    int32_t  pad;
    int32_t pc = 0;
    char scr[2];


    for( ; *format != 0; ++format )
    {
        if( *format == '%' )
        {
            ++format;
            precision = width = pad = 0;
            if( *format == '\0' ) break;
            if( *format == '%' ) goto out;
            if( *format == '-' )
            {
                ++format;
                pad = PAD_RIGHT;
            }
            while( *format == '0' )
            {
                ++format;
                pad |= PAD_ZERO;
            }
            for( ; *format >= '0' && *format <= '9'; ++format )
            {
                width *= 10;
                width += *format - '0';
            }
            if( *format == '.' )
            {
                ++format;
                for( ; *format >= '0' && *format <= '9'; ++format )
                {
                    precision *= 10;
                    precision += *format - '0';
                }
            }
            if( *format == 's' )
            {
                char *s = va_arg(argp, char *);
                pc += prints(out, s?s:"(null)", width, pad);
                continue;
            }
            if( *format == 'd' )
            {
                pc += printi(out, va_arg(argp, int32_t), 10, 1, width, precision, pad, 'a');
                continue;
            }
            if( *format == 'x' )
            {
                pc += printi(out, va_arg(argp, int32_t), 16, 0, width, precision, pad, 'a');
                continue;
            }
            if( *format == 'X' )
            {
                pc += printi(out, va_arg(argp, int32_t), 16, 0, width, precision, pad, 'A');
                continue;
            }
            if( *format == 'u' )
            {
                pc += printi(out, va_arg(argp, int32_t), 10, 0, width, precision, pad, 'a');
                continue;
            }
            if( *format == 'c' )
            {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char) va_arg(argp, int);
                scr[1] = '\0';
                pc += prints(out, scr, width, pad);
                continue;
            }
        }
        else
        {
        out:
            printchar (out, *format);
            ++pc;
        }
    }

    if( out ) **out = '\0';

    return pc;
}


/* assuming sizeof(void *) == sizeof(int) */
int32_t  uprintf( const char *format, ... )
{
    va_list  argp;
    int32_t  ret;

    va_start(argp, format);

    ret = uvsprintf(0, format, argp);

    va_end(argp);

    return ret;
}


int32_t
usprintf( char *out, const char *format, ... )
{
    va_list  argp;
    int32_t  ret;

    va_start(argp, format);

    ret = uvsprintf(&out, format, argp);

    va_end(argp);

    return ret;
}

