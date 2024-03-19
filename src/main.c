#include "envcheck.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "core.h"


#define C_BOLD  "\x1B[1m"
#define C_RESET "\x1B[0m" /* not using \x1B[22m because ANSI.SYS is stupid */


int main(int argc, char** argv)
{
    int exitcode;
    NTC_File file;
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
        file.name = argv[i];

        file.stream = fopen(file.name, "rb");
        if (!file.stream)
        {
            NTC_print
            (
                stderr, file.name,
                "could not open file (%s).\n", strerror(errno)
            );
            exitcode |= 1;
            continue;
        }

        magic = NTC_checkmagic(&file);
        if (magic <= 0)
        {
            fclose(file.stream);
            exitcode |= 2;
            continue;
        }

        patnum = NTC_getpatnum(&file);
        if (patnum == -1)
        {
            fclose(file.stream);
            exitcode |= 4;
            continue;
        }

        if (patnum > 64) compat = 0;
        else compat = 1;

        for (pat = 0; pat < patnum; pat++)
        {
            compat &= NTC_procpat(&file, pat);
        }

        NTC_print
        (
            stdout, file.name,
            C_BOLD "module %s compatible with NoiseTracker. %s\n" C_RESET,
            compat ? "IS" : "IS NOT",
            compat ? ":3" : ":("
        );

        fclose(file.stream);
    }

    return exitcode;
}
