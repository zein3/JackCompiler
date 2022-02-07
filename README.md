# Jack Compiler

## About
Jack is a language from the Nand2Tetris course. This compiler is written in C++.  
Go to the [Nand2Tetris website](https://www.nand2tetris.org/software) to get the emulator and example code.


## Requirements
  - Boost C++ Libraries


## Installation
1. Make sure you have C/C++ build tools (make, gcc, etc)  
2. run the command
```bash
make release
```
3. the binary is in build/apps/jackc, move this somewhere in your path  


## Usage
  - Compiling a file (filename.jack)
```bash
jackc {filename}
```
  - Compiling a folder (dirname/file1.jack, dirname/file2.jack, ...)
```bash
jackc {dirname}
```
  - Tips: use the --print-xml after the filename to print your parsed code to the standard output, this can be useful for debugging to see which part of your code generates the error
```bash
jackc {filename/dirname} --print-xml
```
