
## Example Program Ideas

As I prefer algorithmic development over other types of development, I would probably end up making these first (if they are possible of course):

* Pseudo random number generator - a program to generate a random number (or set of random numbers) given a seed `s` (and count `n`).
* 99 Bottles - a program that prints out the "99 Bottles of Beer" song
* ROT13 - a program to generate the ROT13 encryption of a string (may not be possible since not all alphabetical letters are allowed in AWASCII...)
* Quine - a program that prints its own (Awatalk) form
* ...

## Source Code Features

While I probably won't implement these, these do provide some potential ideas of what to do next:

* More detailed error messages
    - I would like to be able to give the runtime error locations within the source file being parsed, especially if interpreting a `.awa` or `.awasm` file.
    - Additionally, for all error messages, I would like to better present where the error occurs, maybe by finding the specific line of occurence and printing the entire line out with a `^` pointing to where the error is believed to occur? This would require some major rewriting of how the files are read.
* A live code interpreter
    - It would be cool to allow `awai` to function without an input file and instead allow the user to type lines of code one at a time (likely in "Awatisms", or maybe prespecified)
    - While a lot more difficult, it also may be interesting to allow for function definitions. This would allow for the live interpreter to accept blocks of code at a time to be ran rather than just single instructions.
* A code optimizer (?)
    - A good amount of research would need to be done, but perhaps simple structures could be parsed and then optimized when compiling? (e.g. combining instructions using double-bubbles)
    - Find a way to minimize repeated lines of code?

## GitHub QoL

These are things I probably should do with my GitHub Repo that I either don't have the time for or I need to research more about:

* Implementing testing with CI/CodeCov
* Setting a proper release w/ release version