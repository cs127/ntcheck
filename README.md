# ntcheck

## overview

a command-line program to check if an Amiga module file is compatible with
NoiseTracker.

written in pure C89.

the program checks if a module contains more than 64 patterns, and whether it
contains the following commands in its patterns, which are not supported by
NoiseTracker:

* `5xy` (volume slide + portamento)
* `6xy` (volume slide + vibrato)
* `7xy` (tremolo)
* `8xx` (implementation-defined)
* `9xx` (sample offset)
* `Dxx` (pattern break) with nonzero `$xx`.
  * NoiseTracker ignores the parameter, always behaving like ProTracker's `D00`.
* `Eyx` (extended command) with nonzero `$y`
  * NoiseTracker treats any `Eyx` as "set filter", behaving like ProTracker's
  `E0x`.
* `Fxx` with `$xx` larger than `$1F` (set tempo/BPM in CIA timing)

## installation

### releases

go to the [releases page](https://github.com/cs127/ntcheck/releases) to get
compiled executables for your platform.

the currently supported platforms are:

* Linux (64-bit)
* Linux (32-bit)
* DOS + CWSDPMI (32-bit)
* Windows (64-bit)
* Windows (32-bit)

for other OSes, you need to compile manually. fortunately, the code is very
portable, and can be compiled for any platform! :3

### manual compilation

you can use CMake to compile the project.

if you don't want to use CMake, simply compile `src/main.c` and `src/core.c`.
any C89-compatible compiler should work.

## usage

```bash
ntcheck [FILE1] [FILE2] [FILE3] ...
```

no command-line options. simply specify the file (or files) you want to check.

### exit codes

* 0: success: all specified files were successfully checked.
* 1: error: at least one file could not be opened.
* 2: error: at least one file was not a valid Amiga module file.
* 4: error: at least one file was malformed in some way.
* 64: error: no files were specified on the command line.

combinations of 1, 2, and 4 will be added together if they occur simultaneously.
