# ntcheck

[![standard-readme compliant](https://img.shields.io/badge/standard--readme-OK-green.svg?style=flat-square)](https://github.com/RichardLitt/standard-readme)

A command line program (written in pure C89 / ANSI C) which checks if an Amiga module contains more than 64 patterns, whether it contains any notes outside the valid pitch range, whether any of its samples have a nonzero finetune value, and whether it contains any of the following commands in its patterns which are not supported by NoiseTracker.

| effect | condition     | function                                 | notes                                                                |
|:------:|:-------------:|:----------------------------------------:|:--------------------------------------------------------------------:|
| `5xy`  | any           | volume slide + portamento                |                                                                      |
| `6xy`  | any           | volume slide + vibrato                   |                                                                      |
| `7xy`  | any           | tremolo                                  |                                                                      |
| `8xx`  | any           | implementation-defined                   |                                                                      |
| `9xx`  | any           | sample offset                            |                                                                      |
| `Dxx`  | `$xx` > `$00` | pattern break with nonzero parameter     | NoiseTracker ignores the parameter, behaving like ProTracker's `D00` |
| `Eyx`  | `$y` > `$0`   | extended command with nonzero subcommand | NoiseTracker ignores `y`, behaving like ProTracker's `E0x`           |
| `Fxx`  | `$xx` > `$1F` | set tempo/BPM (CIA timing)               |                                                                      |

## Table of Contents

- [Usage](#usage)
- [Maintainers](#maintainers)
- [Contributing](#contributing)
- [License](#license)

## Usage

```bash
ntcheck [FILE1] [FILE2] [FILE3] ...
```

## Maintainers

[@cs127](https://github.com/cs127)
[@RepellantMold](https://github.com/RepellantMold)

## Contributing

PRs accepted.

Small note: If editing the README, please conform to the
[standard-readme](https://github.com/RichardLitt/standard-readme) specification.

## License

[GPL v3](LICENSE) © 2024 cs127
