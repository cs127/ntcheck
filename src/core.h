#ifndef NTC_H_CORE
#define NTC_H_CORE


#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>


#define NTC_CHNNUM          4
#define NTC_ROWNUM          64
#define NTC_ORDNUM_MAX      128
#define NTC_EVENTSIZE       4
#define NTC_PATSIZE         NTC_EVENTSIZE * NTC_CHNNUM * NTC_ROWNUM

#define NTC_ORDNUM_PTR      0x03B6
#define NTC_ORDERS_PTR      0x03B8
#define NTC_MAGIC_PTR       0x0438
#define NTC_PATTERNS_PTR    0x043C

#define NTC_PERIOD_C1       856
#define NTC_PERIOD_B3       113

#define NTC_MAGIC_COUNT     17
#define NTC_MAGIC_SIZE      4
extern const char* NTC_MAGIC [NTC_MAGIC_COUNT];


void NTC_vprint(FILE* stream, const char* fname, const char* fmt, va_list ap);

void NTC_print(FILE* stream, const char* fname, const char* fmt, ...);

void NTC_printcell
(
    FILE* stream, const char* fname,
    size_t pat, size_t row, size_t chn,
    const char* fmt, ...
);


int NTC_checkmagic(const char* fname, FILE* file);

int NTC_getpatnum(const char* fname, FILE* file);

int NTC_procpat(const char* fname, FILE* file, size_t pat);


#endif /* NTC_H_CORE */
