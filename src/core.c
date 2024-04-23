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

void NTC_printcell(NTC_Display* context, const char* fmt, ...)
{
    FILE* stream = context->stream;
    const char* fname = context->fname;
    unsigned int pat = context->pat;
    unsigned int row = context->row;
    unsigned int chn = context->chn;
    va_list ap;

    NTC_print
    (
        stream, fname,
        "(PAT %03u ROW %02u CHN %01u) ",
        pat, row, (chn + 1)
    );

    va_start(ap, fmt);
    vfprintf(stream, fmt, ap);
    va_end(ap);
}

int NTC_checkmagic(NTC_File* file)
{
    int fseekres;
    size_t freadres;
    char magic [NTC_MAGIC_SIZE];
    size_t i = 0;

    fseekres = fseek(file->stream, NTC_MAGIC_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, file->name,
            "could not find the magic bytes (%s).\n", strerror(errno)
        );
        return 0;
    }

    freadres = fread(magic, 1, 4, file->stream);
    if (freadres < 4)
    {
        NTC_print
        (
            stderr, file->name,
            feof(file->stream)
            ? "could not read the magic bytes (file ends prematurely).\n"
            : "could not read the magic bytes.\n"
        );
        return 0;
    }

    for (; i < NTC_MAGIC_COUNT; ++i)
    {
        if (!memcmp(magic, NTC_MAGIC[i], NTC_MAGIC_SIZE)) return 1;
    }

    NTC_print
    (
        stderr, file->name,
        "not a valid 4-channel Amiga module file.\n"
    );
    return 0;
}

int NTC_getpatnum(NTC_File* file)
{
    int fseekres;
    size_t freadres;
    int patnum;
    unsigned char ordnum;
    unsigned char orders [NTC_ORDNUM_MAX];
    size_t i = 0;

    fseekres = fseek(file->stream, NTC_ORDNUM_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, file->name,
            "could not find the number of orders (%s).\n", strerror(errno)
        );
        return -1;
    }

    freadres = fread(&ordnum, 1, 1, file->stream);
    if (!freadres)
    {
        NTC_print
        (
            stderr, file->name,
            feof(file->stream)
            ? "could not read the number of orders (file ends prematurely).\n"
            : "could not read the number of orders.\n"
        );
        return -1;
    }

    fseekres = fseek(file->stream, NTC_ORDERS_PTR, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, file->name,
            "could not find the order list (%s).\n", strerror(errno)
        );
        return -1;
    }

    freadres = fread(orders, 1, NTC_ORDNUM_MAX, file->stream);
    if (freadres < NTC_ORDNUM_MAX)
    {
        NTC_print
        (
            stderr, file->name,
            feof(file->stream)
            ? "could not read the orders (file ends prematurely).\n"
            : "could not read the orders.\n"
        );
        return -1;
    }

    patnum = 0;

    for (; i < ordnum; ++i)
    {
        if (orders[i] + 1 > patnum) patnum = orders[i] + 1;
    }

    fseekres =
    fseek(file->stream, NTC_PATTERNS_PTR + patnum * NTC_PATSIZE, SEEK_SET);
    if (fseekres)
    {
        NTC_print
        (
            stderr, file->name,
            "could not locate the end of pattern data (%s).\n", strerror(errno)
        );
        return -1;
    }

    if (patnum > 64)
    {
        NTC_print
        (
            stdout, file->name,
            "module has more than 64 patterns."
        );
    }

    return patnum;
}

int NTC_procpat(NTC_File* file, size_t pat)
{
    size_t row = 0;
    size_t chn;
    unsigned char prdbyte;
    unsigned short int prd;
    unsigned char cmd;
    unsigned char prm;
    int prdcompat = 1;
    int cmdcompat = 1;
    NTC_Display context;

    fseek(file->stream, NTC_PATTERNS_PTR + pat * NTC_PATSIZE, SEEK_SET);

    for (; row < NTC_ROWNUM; ++row)
    {
        for (chn = 0; chn < NTC_CHNNUM; ++chn)
        {
            prd = 0;

            fread(&prdbyte, 1, 1, file->stream);
            prd |= prdbyte << 8;
            fread(&prdbyte, 1, 1, file->stream);
            prd |= prdbyte;
            prd &= 0x0FFF;

            fread(&cmd, 1, 1, file->stream);
            fread(&prm, 1, 1, file->stream);
            cmd &= 0x0F;

            context.pat = pat;
            context.row = row;
            context.chn = chn;
            context.fname = file->name;
            context.stream = stdout;

            if (prd && (prd > NTC_PERIOD_C1 || prd < NTC_PERIOD_B3))
            {
                NTC_printcell
                (
                    &context,
                    "note pitch outside the allowed range.\n", prd
                );
                prdcompat = 0;
            }

            if (cmd > 0x4 && cmd < 0xA)
            {
                NTC_printcell
                (
                    &context,
                    "invalid command %01X.\n", (unsigned int)cmd
                );
            }
            else if (cmd == 0xD && prm)
            {
                NTC_printcell
                (
                    &context,
                    "pattern break (D) with nonzero parameter.\n"
                );
            }
            else if (cmd == 0xE && (prm & 0xF0))
            {
                NTC_printcell
                (
                    &context,
                    "extended command (E) with nonzero subcommand.\n"
                );
            }
            else if (cmd == 0xF && prm >= 0x20)
            {
                NTC_printcell
                (
                    &context,
                    "CIA tempo command (F with parameter larger than $1F).\n"
                );
            }
            else continue;

            cmdcompat = 0;
        }
    }

    return prdcompat && cmdcompat;
}

int NTC_procsmp(NTC_File* file, size_t smp)
{
    char tune;

    fseek
    (
        file->stream,
        NTC_SAMPLES_PTR + smp * NTC_SMPSIZE + NTC_SMPTUNE_RPTR,
        SEEK_SET
    );

    fread(&tune, 1, 1, file->stream);

    if (tune & 0x0F)
    {
        NTC_print
        (
            stdout, file->name,
            "sample %u has a nonzero finetune value.\n", (unsigned int)(smp + 1)
        );
        return 0;
    }

    return 1;
}
