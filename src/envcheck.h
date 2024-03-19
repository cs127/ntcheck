#ifndef NTC_H_ENVCHECK
#define NTC_H_ENVCHECK


#include <limits.h>


#if CHAR_BIT != 8

#error "[ntcheck] invalid architecture, a byte must be 8 bits wide!"

#endif /* CHAR_BIT != 8 */


#endif /* NTC_H_ENVCHECK */
