#ifndef NTC_H_CORE
#define NTC_H_CORE

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define NTC_CHNNUM       4
#define NTC_ROWNUM       64
#define NTC_ORDNUM_MAX   128
#define NTC_SMPNUM       31
#define NTC_EVENTSIZE    4
#define NTC_PATSIZE      (NTC_EVENTSIZE * NTC_CHNNUM * (NTC_ROWNUM))
#define NTC_SMPSIZE      30

#define NTC_ORDNUM_PTR   0x03B6
#define NTC_ORDERS_PTR   0x03B8
#define NTC_MAGIC_PTR    0x0438
#define NTC_PATTERNS_PTR 0x043C
#define NTC_SAMPLES_PTR  0x0014
#define NTC_SMPTUNE_RPTR 0x0018

#define NTC_PERIOD_C1    856
#define NTC_PERIOD_B3    113

#define NTC_MAGIC_COUNT  17
#define NTC_MAGIC_SIZE   4

extern const char* NTC_MAGIC[NTC_MAGIC_COUNT];

typedef struct NTC_File
{
    char* name;
    FILE* stream;
} NTC_File;

typedef struct NTC_Display
{
    FILE* stream;
    const char* fname;
    size_t pat;
    size_t row;
    size_t chn;
} NTC_Display;

typedef struct NTC_Pattern
{
    unsigned short int prd;
    unsigned char cmd;
    unsigned char prm;
    NTC_Display display;
} NTC_Pattern;

void NTC_vprint(
    FILE* stream, const char* fname, const char* fmt, va_list ap
);

void NTC_print(
    FILE* stream, const char* fname, const char* fmt, ...
);

void NTC_printcell(NTC_Display* context, const char* fmt, ...);

int NTC_checkmagic(NTC_File* file);

int NTC_getpatnum(NTC_File* file);

int NTC_procpat(NTC_File* file, size_t pat);

int NTC_procsmp(NTC_File* file, size_t smp);

#endif /* NTC_H_CORE */
