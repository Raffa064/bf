# Brain F*ck

Bf is a esolang that consists only of 8 basic symbols:
- '\+', '\-' Add/Subtract to the current memory location
- '<', '>' Move memory pointer
- '.', ',' Print to stdout and read input from stdin
- '[', ']' Begin/End loop. (Loop stops when current memory cell is zero)

This implementation adds 4 new extension symbols:
- '!' Can be used to clear console
- '?' Show debug information
- '\#' Sleeps for 10ms
- '@' Wait for keypress (input is discarted)

## Build

To build the interpreter, the only thing needed is to call make on the project's root directory:

```bash
make
```

You can also `build & run` with:
```bash
make run
```

After compiled, you will have an executable file, that you can use to run any **bf** script file:
```bash
./bf main.bf
```

> [!TIP] If no arguments is set, the executable will look for `./main.bf`, and if there is more than one argument, only the last one will be used as path for script file.

You can also pass `-p` option for the executable, and it will print the source code with a cool debug highligher provided by [lex.h](https://github.com/Raffa064/lex.h).
