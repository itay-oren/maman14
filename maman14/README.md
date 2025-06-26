
# Assembler for Custom Assembly Language

## Overview
This project implements an assembler for the custom assembly language designed by the course staff. The assembler takes assembly code files (.as) as input and generates three output files for each input file: `.ob`, `.ext`, and `.ent`.

## Files
- **Assembler.c**: The main program code for the assembler.
- **Assembler.h**: Header file containing function prototypes and constants.
- **Makefile**: Makefile for compiling the assembler program.
- **README.md**: This file, providing an overview of the project and instructions for usage.

## Usage
To use the assembler, follow these steps:
1. **Compilation**: Compile the assembler using the provided Makefile.
   ```bash
   make
   ```
   
2. **Execution**: Run the assembler program, providing the `.as` files as arguments.
   ```bash
   ./driver test1.as.as file2.as ...
   ```
   Replace `test1.as`, `file2.as`, etc., with the actual assembly code files you want to assemble.

3. **Output Files**: After running the assembler, it will generate the following output files for each input `.as` file:
   - `file1.ob`, `file2.ob`, ...: Object files containing the machine code and relevant information.
   - `file1.ext`, `file2.ext`, ...: External files listing references to external symbols.
   - `file1.ent`, `file2.ent`, ...: Entry files listing entry points defined in the assembly code.

## Example
Suppose you have two assembly code files: `example1.as` and `example2.as`. To assemble them, run:
```bash
./driver example1.as example2.as
```
This command will produce `example1.ob`, `example1.ext`, `example1.ent` and `example2.ob`, `example2.ext`, `example2.ent` accordingly.

## Notes
- Ensure that the assembly code follows the syntax and rules provided by the course staff.
- The assembler assumes a specific structure for the assembly code and expects labels, instructions, and directives in the correct format.

---

Copyright (c) 2024 Itay Oren
