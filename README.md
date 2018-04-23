# sic-assembler

This was a semester project for my Systems Programming class, I had to build a Command Line Interpeter and an Assembler for [Simplified Instructional Computer](https://en.wikipedia.org/wiki/Simplified_Instructional_Computer).

## Running

To build:

```sh
$ git clone https://github.com/despinoza1/sic-assembler
$ cd sic-assembler
$ make
```

Then run `./sic-cli` to enter the CLI.

### Inside the CLI

`> help` Will display all the commands.

`> assemble <assembly file>` Will assemble an SIC assembly file, there are a few examples included.

Afterwards use `> load <object file>` to load the object file into memory and `> execute` to run the program.

### Inside the Debugger

`$ h` Will display all the commands.

`$ s` To step into the next instruction.

`$ r` To display content of registers.

`$ e` To edit either memory or registers.