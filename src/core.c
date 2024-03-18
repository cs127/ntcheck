#include "core.h"

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


const char* NTC_MAGIC [NTC_MAGIC_COUNT] =
{
    /* taken from OpenMPT's source code (soundlib/Load_mod.cpp) */

    "M.K.", "M!K!", "PATT", "NSMS", "LARD", "M&K!", "FEST", "N.T.", "M\0\0\0",
    "FA04", "FLT4", "EXO4", "TDZ4", ".M.K", "4CHN", "04CH", "04CN"
};


void NTC_vprint(FILE* stream, const char* fname, const char* fmt, va_list ap)
{
    fprintf(stream, "[%s] ", fname);
    vfprintf(stream, fmt, ap);
}

void NTC_print(FILE* stream, const char* fname, const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    NTC_vprint(stream, fname, fmt, ap);
    va_end(ap);
}

void NTC_printcell
(
    FILE* stream, const char* fname,
    size_t pat, size_t row, size_t chn,
    const char* fmt, ...
)
{
    va_list ap;

    NTC_print
    (
        stream, fname,
        "(PAT %03u ROW %02u CHN %01u) ",
        (unsigned int)pat, (unsigned int)row, (unsigned int)(chn + 1)
    );

    va_start(ap, fmt);
    vfprintf(stream, fmt, ap);
    va_end(ap);
}

int NTC_checkmagic(const char* fname, FILE* file)
{
    int fseekres;
    size_t freadres;
    char magic [NTC_MAGIC_SIZE];
    size_t i;

    fseekres = fseek(file, NTC_MAGIC_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, fname,
            "could not find the magic bytes (%s).\n", strerror(errno)
        );
        return 0;
    }

    freadres = fread(magic, 1, 4, file);
    if (freadres < 4)
    {
        NTC_print
        (
            stderr, fname,
            feof(file)
            ? "could not read the magic bytes (file ends prematurely).\n"
            : "could not read the magic bytes.\n"
        );
        return 0;
    }

    for (i = 0; i < NTC_MAGIC_COUNT; i++)
    {
        if (!memcmp(magic, NTC_MAGIC[i], NTC_MAGIC_SIZE)) return 1;
    }

    NTC_print
    (
        stderr, fname,
        "not a valid 4-channel Amiga module file.\n"
    );
    return 0;
}

int NTC_getpatnum(const char* fname, FILE* file)
{
    int fseekres;
    size_t freadres;
    int patnum;
    unsigned char ordnum;
    unsigned char orders [NTC_ORDNUM_MAX];
    size_t i;

    fseekres = fseek(file, NTC_ORDNUM_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, fname,
            "could not find the number of orders (%s).\n", strerror(errno)
        );
        return -1;
    }

    freadres = fread(&ordnum, 1, 1, file);
    if (!freadres)
    {
        NTC_print
        (
            stderr, fname,
            feof(file)
            ? "could not read the number of orders (file ends prematurely).\n"
            : "could not read the number of orders.\n"
        );
        return -1;
    }

    fseekres = fseek(file, NTC_ORDERS_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, fname,
            "could not find the order list (%s).\n", strerror(errno)
        );
        return -1;
    }

    freadres = fread(orders, 1, NTC_ORDNUM_MAX, file);
    if (freadres < NTC_ORDNUM_MAX)
    {
        NTC_print
        (
            stderr, fname,
            feof(file)
            ? "could not read the orders (file ends prematurely).\n"
            : "could not read the orders.\n"
        );
        return -1;
    }

    patnum = 0;

    for (i = 0; i < ordnum; i++)
    {
        if (orders[i] + 1 > patnum) patnum = orders[i] + 1;
    }

    fseekres = fseek(file, NTC_PATTERNS_PTR + patnum * NTC_PATSIZE, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, fname,
            "could not locate the end of pattern data (%s).\n", strerror(errno)
        );
        return -1;
    }

    if (patnum > 64)
    {
        NTC_print
        (
            stdout, fname,
            "module has more than 64 patterns."
        );
    }

    return patnum;
}

int NTC_procpat(const char* fname, FILE* file, size_t pat)
{
    size_t row;
    size_t chn;
    unsigned char cmd;
    unsigned char prm;
    int compat;

    fseek(file, NTC_PATTERNS_PTR + pat * NTC_PATSIZE, SEEK_SET);

    compat = 1;

    for (row = 0; row < NTC_ROWNUM; row++)
    {
        for (chn = 0; chn < NTC_CHNNUM; chn++)
        {
            fseek(file, 2, SEEK_CUR);
            fread(&cmd, 1, 1, file);
            fread(&prm, 1, 1, file);
            cmd &= 0x0F;

            if (cmd > 0x4 && cmd < 0xA)
            {
                NTC_printcell
                (
                    stdout, fname, pat, row, chn,
                    "invalid command %01X.\n", cmd
                );
            }
            else if (cmd == 0xD && prm)
            {
                NTC_printcell
                (
                    stdout, fname, pat, row, chn,
                    "pattern break (D) with nonzero parameter.\n"
                );
            }
            else if (cmd == 0xE && (prm & 0xF0))
            {
                NTC_printcell
                (
                    stdout, fname, pat, row, chn,
                    "extended command (E) with nonzero subcommand.\n"
                );
            }
            else if (cmd == 0xF && prm >= 0x20)
            {
                NTC_printcell
                (
                    stdout, fname, pat, row, chn,
                    "CIA tempo command (F with parameter larger than $1F).\n"
                );
            }
            else continue;

            compat = 0;
        }
    }

    return compat;
}
