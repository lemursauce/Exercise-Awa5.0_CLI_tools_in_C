# AWA5.0 CLI tools (in C)

A simple collection of [AWA5.0](https://github.com/TempTempai/AWA5.0) CLI tools written in C.

The tools are an AWA5.0 compiler/conversion tool called `awac` that compiles/converts AWA5.0 files into different types, and a simple AWA5.0 interpreter called `awai` that interprets AWA5.0 code from all the specified filtypes.

These tools are designed based off of the minimal standard of the original [AWA5.0 specifications](https://github.com/TempTempai/AWA5.0/blob/main/Documentation/AWA5.0%20Specification.pdf) as well as the various behaviors influenced by the javascript implementation of the [original interpreter](https://temptempai.github.io/AWA5.0/) not discussed in the documentation.

If you have any questions about AWA5.0, **PLEASE** read the [AWA5.0 specifications](https://github.com/TempTempai/AWA5.0/blob/main/Documentation/AWA5.0%20Specification.pdf), as it is a well put together document with plenty of information on the intended use and format of code. The rest of this README assumes that you understand the basic format of how AWA5.0 code works.

## Building and Running

This project was made to work with cmake with c99 specifications (gcc compiler). The gcc version used during development was gcc 13.1.0, but any gcc version that can compile c99 code should suffice. The cmake version that is expected at minimum is 3.10.

To build on Windows:
* [Download](https://cmake.org/download/) the cmake gui application, run it and specify where the project directory is and where the build directory should be.
* Run the `configure` procedure, then `generate`.

To build on MacOS:
* Download and cmake through [Xcode](https://developer.apple.com/xcode/) developer tools or [homebrew](https://brew.sh/):
    - if using Xcode, you may have to run `sudo xcode-select --reset` or `sudo xcodebuild -license accept`.
    - if using homebrew, simply run the command `brew install cmake`
* From the project path, call `cmake -B <path-to-build>`

To build on Linux/WSL:
* Run the command `sudo apt install cmake -y`
* From the project path, call `cmake -B <path-to-build>`

When running on windows:
* It is recommended to run through Visual Studio.
* You may need to ensure that certain libraries are recognized as a prerequisite (this is untested).
* There should be ready-press buttons to build and run the code!

When running on other devices:
* From the project path, call `cmake --build <path-to-build>`, then navigate to the build directory and call the executable versions of `awac` or `awai`.
    - You could also call from a different directory using `<path-to-build>/awac` and `<path-to-build>/awai`.

Should your device allow you to do so, and if you whish to use these tools long term for some reason, feel free to research how to make an alias for the absolute directories to these executables. **As this is an exercise, I am not planning to make any automatic features to make this integrate itself as a ready-use command, sorry**.

## File specifications

### .awa

This extension is used to refer to any "Awatalk" files, which is the form of code described in the original AWA5.0 specifications. These files are essentially text files used to represent the binary values of commands and numbers using `awa` for `0` and `wa` for `1`.

These files are case insensitive, but `awa` keywords need to be at the beginning of the file or have some leadings space before being written. Additionally, both `awa` and `wa` keywords must not contain any whitespace between the specified characters.

Other characters are allowed within this extension format, but there is no format to specify comments so **be sure not to use the letters `a` or `w` unless they are within a keyword**.

These files **must** begin with a single `awa` keyword, which is "used" as an initializer. This keyword is separate from all the other commands and does not have any practical functionality.

### .awasm

This extension is used to refer to any files using "Awatism" shorthand for the notable commands, which can be found in the original specification documents.

Additionally, commands are separated by newlines or `;` characters. You are allowed to specify single line comments using the `#` character, in which all characters following on the same line are considered part of the comment and not actual code.

### .o

This extension is used to refer to any compiled AWA5.0 files with their commands (and respective numbers if necessary) written in a binary format. This is meant to be a simple form that is easy to parse/interpret, although it is easy enough to use `awac` to convert it back to any of the other previously described file types.

## Usage

### awac

As described when running `awac -h`:

    Usage: awac INPUT_FILE [-t | -s | -c] [-o OUTPUT_FILE]

    Tool to compile an Awa5.0 file into a different format, whether that be "Awatalk" (.awa), "Awatisms" (.awasm, also known as "Awasembly"), or "Awobject" (.o). The output will contain all the same logic as the source file but in a different format.

    The following describes the usage of the various options / flags:
    INPUT_FILE             the required Awa5.0 file to compile
    -o                     set the output file when finished compiling
        OUTPUT_FILE            the file being set as the output file when -o is called

    -t                     set the compile mode to "Awatalk"
    -s                     set the compile mode to "Awatisms"
    -c                     set the compile mode to "Awobject"

    -d                     compile with "details" on (only with -s or -t)

    -h                     display this help and exit

To summarize, this tool is meant to compile/convert the different forms of AWA5.0 code.

Some additional notes regarding this tool:
* Writing an output file with `-o` will add the correct extension to the file name unless a valid AWA5.0 file extension is already used, in which it will be replaced with the correct extension.
* This tool will exit and inform the user of any syntax errors in their code that need to be addressed.
* When converting to "Awatalk" or "Awatisms", using the `-d` flag allows for a "detailed" file to be generated instead, which just adds comments regarding what the code looks like in the unchosen format. Specifically:
    - converting to "Awatalk" with details adds comments that show what "Awatism" command is emulated
    - converting to "Awatisms" with details adds comments that show the corresponding "Awatalk" code.

### awai

As described when running `awai -h`:

    Usage: ./build/awai FILE [-s] [-x MAX_INST] 

    Tool to interpret an Awa5.0 file from one of the specified formats, whether that be "Awatalk" (.awa), "Awatisms" (.awasm, also known as "Awasembly"), or "Awobject" (.o). The interpreter will run the code and take inputs / print outputs via the console. If any inputs are required, they will be taken as specified by the original documentation.

    The following describes the usage of the various options / flags:
    FILE                   the required Awa5.0 file to interpret
    -s                     print the running stats
    -x                     set the maximum amount of instructions to run (to avoid infinite loops)
        MAX_INST               the maximum amount of instructions specified

    -h                     display this help and exit`

To summarize, given any valid AWA5.0 file, this tool is meant to interpret and run the code through the commandline console.

Some additional notes regarding this tool:
* Like how there are statistics on the [original AWA5.0 interpreter website](https://temptempai.github.io/AWA5.0/), using the `-s` command will give the statistics of the file being ran. These include:
    - The amount of commands perceived by the parser
    - The amount of commands ran (cycles) until completion [note that this is *currently* more accurate than the original website due to a bug where the number `16` will allow a user to "skip" a cycle being read]
    - The final state of the bubbleAbyss (which could be very useful for debugging your AWA5.0 code)
* There is actually runtime error detecting!
    - This was allegedly not included in the original since "AWA5.0 is perfect" and therefore there was no need for specification on what would happen
* Since infinite loops may be encountered, there is an optino to set a maximum amount of instructions before forced termination
    - This is **NOT** on by default, so if you are concerned about such loops occuring, make sure to set this flag.

## Testing

**currently**, there is no testing available, but this may be integrated in the future if I am feeling particularly motivated to do so.

## Examples

Aside from the src directory, there is included a small set of example programs that you are free to test and run for yourself. Likely, I will add to these before I add to the main source code. The examples currently in the directory have been tested on the CLI tools as well as the original interpreter.

The current files are as follows (in both `.awa` and `.awatism` form):
* `hello` - a simple hello world program
* `charcode` - prints the `AWASCII` values of all the characters entered in an input string
* `reverse` - prints the reverse of the input string
* `fib` - prints the fibonacci sequence up until a given `n`th value
* `collatz` - prints the collatz sequence generated by a positive integer `n`

## Future improvements

There are several ideas for how this project can improve, in which I am detailing in [TODO.md](./TODO.md)