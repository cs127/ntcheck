#include "envcheck.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "core.h"


#define C_BOLD   "\x1B[1m"
#define C_UNBOLD "\x1B[0m"


int main(int argc, char** argv)
{
    int exitcode;
    FILE* file;
    size_t i;
    int magic;
    int compat;
    int patnum;
    size_t pat;

    if (argc < 2)
    {
        NTC_print(stderr, "", "no filenames specified.\n");
        return 64;
    }

    exitcode = 0;

    for (i = 1; i < argc; i++)
    {
        file = fopen(argv[i], "rb");
        if (!file)
        {
            NTC_print
            (
                stderr, argv[i],
                "could not open file (%s).\n", strerror(errno)
            );
            exitcode |= 1;
            continue;
        }

        magic = NTC_checkmagic(argv[i], file);
        if (magic <= 0)
        {
            fclose(file);
            exitcode |= 2;
            continue;
        }

        patnum = NTC_getpatnum(argv[i], file);
        if (patnum == -1)
        {
            fclose(file);
            exitcode |= 4;
            continue;
        }

        if (patnum > 64) compat = 0;
        else compat = 1;

        for (pat = 0; pat < patnum; pat++)
        {
            compat &= NTC_procpat(argv[i], file, pat);
        }

        NTC_print
        (
            stdout, argv[i],
            C_BOLD "module %s compatible with NoiseTracker. %s\n" C_UNBOLD,
            compat ? "IS" : "IS NOT",
            compat ? ":3" : ":("
        );

        fclose(file);
    }

    return exitcode;
}
