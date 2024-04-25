#include "envcheck.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "core.h"

#define C_BOLD "\x1B[1m"
#define C_RESET                                                \
    "\x1B[0m" /* not using \x1B[22m because ANSI.SYS is stupid */

enum NTC_ExitCode
{
    NTC_EXIT_SUCCESS = 0x00,

    NTC_EXIT_OPEN_FAILURE   = 0x01,
    NTC_EXIT_NOT_AMIGA_MOD  = 0x02,
    NTC_EXIT_MALFORMED_FILE = 0x04,

    NTC_EXIT_NO_FILENAMES = 0x40
};

int main(int argc, char** argv)
{
    int exitcode = NTC_EXIT_SUCCESS;
    NTC_File file;
    size_t i = 0;
    int magic;
    int compat;
    int patnum;
    size_t pat = 0;
    size_t smp = 0;

    if (argc < 2)
    {
        NTC_print(stderr, "", "no filenames specified.\n");
        return NTC_EXIT_NO_FILENAMES;
    }

    while (++i < argc)
    {
        file.name = argv[i];

        file.stream = fopen(file.name, "rb");
        if (!file.stream)
        {
            NTC_print(
                stderr, file.name,
                "could not open file (%s).\n", strerror(errno)
            );
            exitcode |= NTC_EXIT_OPEN_FAILURE;
            continue;
        }

        magic = NTC_checkmagic(&file);
        if (magic <= 0)
        {
            fclose(file.stream);
            exitcode |= NTC_EXIT_NOT_AMIGA_MOD;
            continue;
        }

        patnum = NTC_getpatnum(&file);
        if (patnum == -1)
        {
            fclose(file.stream);
            exitcode |= NTC_EXIT_MALFORMED_FILE;
            continue;
        }

        if (patnum > 64)
        {
            compat = 0;
        }
        else
        {
            compat = 1;
        }

        for (; pat < patnum; ++pat)
        {
            compat &= NTC_procpat(&file, pat);
        }

        for (; smp < NTC_SMPNUM; ++smp)
        {
            compat &= NTC_procsmp(&file, smp);
        }

        NTC_print(
            stdout, file.name,
            C_BOLD "module %s compatible with NoiseTracker. "
                   "%s\n" C_RESET,
            compat ? "IS" : "IS NOT", compat ? ":3" : ":("
        );

        fclose(file.stream);
    }

    return exitcode;
}
