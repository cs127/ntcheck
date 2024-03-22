# ntcheck

a command-line program to check if an Amiga module file is compatible with NoiseTracker.

written in pure C89 C.

## overview

the program checks if a module contains more than 64 patterns, whether it contains any notes outside the valid pitch range, and whether it contains any of the following commands in its patterns which are not supported by NoiseTracker.

| effect - condition | function | notes |
|:---:|:---:|:---:|
| `5xy` | volume slide + portamento |  |
| `6xy` | volume slide + vibrato |  |
| `7xy` | tremolo |  |
| `8xx` | implementation-defined |  |
| `9xx` | sample offset |  |
| `Dxx` - xx > 0 | pattern break with xx not equal to 0 | NoiseTracker ignores the parameter, always behaving like ProTrackers `D00` |
| `Eyx` - y > 0 | extended command with y not equal to 0 | NoiseTracker ignores `y`, behaving like ProTrackers `E0x` |
| `Fxx` - xx > $1F | set tempo/BPM (CIA timing) |  |

## installation

### releases

go to the [releases page](https://github.com/cs127/ntcheck/releases) to get compiled executables for your platform.

the currently supported platforms are:

* Linux (64-bit)
* Linux (32-bit)
* DOS + CWSDPMI (32-bit)
* Windows (64-bit)
* Windows (32-bit)

for other OSes, you need to compile manually. fortunately, the code is very portable, and can be compiled for any platform! :3

### manual compilation

you can use CMake to compile the project.

if you don't want to use CMake, simply compile `src/main.c` and `src/core.c` (e.g. `cc -o ntcheck src/main.c src/core.c`).
any C89-compatible compiler should work.

## usage

```bash
ntcheck [FILE1] [FILE2] [FILE3] ...
```

no command-line options. simply specify the file (or files) you want to check.

### exit codes

| number | case | explanation |
|---:|:---:|---|
| 0 | success | all specified files were successfully checked |
| 1 | error | at least one file could not be opened |
| 2 | error | at least one file was not a valid Amiga module file |
| 4 | error | at least one file was malformed in some way |
| 64 | error | no files were specified on the command line |

> [!NOTE]
> combinations of 1, 2, and 4 will be added together if they occur simultaneously.
