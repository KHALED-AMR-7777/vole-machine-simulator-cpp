# Vole Machine Simulator (C++)

A C++ Object-Oriented Programming project that simulates the Vole Machine and executes programs written in its machine language.

## Project Overview

The Vole Machine Simulator provides a simple console-based environment for loading, executing, and inspecting Vole machine-language programs. It models the machine's memory, CPU, registers, and screen, and supports both step-by-step execution and running a complete program.

This README documents the **console simulator only**. GUI files and GUI setup instructions are not included.

## Main Features

- 256 memory cells, addressed from `00` to `FF`.
- 16 general-purpose registers, named `R0` to `RF`.
- Two-byte instructions represented by four hexadecimal digits.
- Program loading from a text file.
- Instruction validation before execution.
- Fetch, Decode, and Execute processing.
- Step-by-step execution.
- Run-all (batch) execution.
- Manual register and memory editing through the menu.
- Machine-state display, including the Program Counter, Instruction Register, and register values.
- Screen output through memory-mapped address `FF`.
- Program halt instruction.

## Machine Components

The project is organized into classes, with each class responsible for a specific part of the simulator.

| Class | Responsibility |
|---|---|
| `Machine` | Coordinates the simulator components, loads programs, controls execution, and provides access to machine state. |
| `CPU` | Fetches, decodes, validates, and executes instructions. It maintains the Program Counter (PC), Instruction Register (IR), and registers. |
| `Memory` | Stores the machine's 256 bytes and provides memory read, write, clear, and display operations. |
| `Registers` | Stores and manages the 16 general-purpose registers. |
| `Screen` | Stores and displays values written to the screen output. |
| `ProgramLoader` | Reads a program file, checks instruction formats, and loads instructions into memory. |
| `Menu` | Provides the console menu through which the user interacts with the simulator. |

## Machine Specifications

- **Memory size:** 256 cells.
- **Memory address range:** `00`–`FF` (hexadecimal).
- **Cell size:** 1 byte.
- **Number of registers:** 16.
- **Register range:** `R0`–`RF`.
- **Instruction size:** 2 bytes (4 hexadecimal digits).
- **Program Counter (PC):** Holds the address of the next instruction.
- **Instruction Register (IR):** Holds the currently fetched instruction.
- **Screen address:** `FF`. A store instruction targeting address `FF` sends the selected register value to the screen output.

The CPU follows the **Fetch → Decode → Validate → Execute** cycle. Normally, the PC advances by two after an instruction is fetched. A jump instruction can change the PC to the specified address.

## Supported Instruction Set

Each instruction is four hexadecimal digits:

`OP R X Y`

The exact meaning of the fields depends on the opcode.

| Opcode | Instruction | Description |
|---|---|---|
| `1` | Load from memory | Loads the byte at memory address `XY` into register `R`. |
| `2` | Load immediate | Loads the hexadecimal value `XY` directly into register `R`. |
| `3` | Store | Stores the value in register `R` at memory address `XY`. If `XY` is `FF`, the value is sent to the screen. |
| `4` | Move register | Copies the value of register `RX` into register `RY`. The second field must follow the instruction's required format (`0`). |
| `5` | Integer addition | Adds the values in registers `RX` and `RY`, then stores the result in register `R`. |
| `6` | Floating-point addition | Adds the floating-point values represented by registers `RX` and `RY`, then stores the result in register `R`. |
| `7` | OR | Performs bitwise OR on registers `RX` and `RY`, storing the result in register `R`. |
| `8` | AND | Performs bitwise AND on registers `RX` and `RY`, storing the result in register `R`. |
| `9` | XOR | Performs bitwise XOR on registers `RX` and `RY`, storing the result in register `R`. |
| `A` | Rotate right | Rotates the value in register `R` to the right by `Y` bit positions. The `X` field must be `0`. |
| `B` | Conditional jump | If the value in register `R` equals the value in `R0`, the PC is set to address `XY`. |
| `C` | Halt | Stops program execution. The remaining instruction fields must be zero. |

## Program File Format

Programs are stored as plain text files (`.txt`).

- Write one instruction per line.
- Each instruction must contain four hexadecimal characters (`0`–`9`, `A`–`F`).
- Instructions are loaded into consecutive memory locations.
- The starting address must be a valid even address.
- A line beginning with `;` is treated as a comment by the program loader.
- Blank lines are ignored.

### Example Program

```text
; Load 5 into R1
2105
; Load 3 into R2
2203
; Add R1 and R2, store the result in R3
5312
; Send R3 to the screen
33FF
; Stop execution
C000
```

Expected result: `R3` contains `08`, and the screen output displays `08`.

## Requirements

- A C++ compiler that supports C++17 or later.
- A terminal or command prompt.
- No external libraries are required for the console simulator.

## Build Instructions

Compile the source files from the project directory.

### Using g++

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp CPU.cpp Machine.cpp Memory.cpp Menu.cpp Program_Loader.cpp Registers.cpp Screen.cpp -o vole
```

On Windows, the output executable can be named `vole.exe`:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp CPU.cpp Machine.cpp Memory.cpp Menu.cpp Program_Loader.cpp Registers.cpp Screen.cpp -o vole.exe
```

If `g++` is not recognized, install a C++ compiler such as MinGW-w64 or MSYS2 and make sure its `bin` directory is added to the system `PATH`.

## Run the Simulator

On Windows:

```bash
vole.exe
```

On Linux or macOS:

```bash
./vole
```

The console menu provides these options:

1. **Load Program from File** — loads a text program into memory at the selected starting address.
2. **Set Register Value Manually** — sets a register (`0`–`15`) to a two-digit hexadecimal value.
3. **Set Memory Cell Value Manually** — writes a two-digit hexadecimal value to a memory address.
4. **Run Step-by-Step** — executes one instruction.
5. **Run All (Batch Execution)** — executes instructions until the program halts or execution stops.
6. **Display Machine State** — displays the current machine state.
7. **Exit** — closes the simulator.

### Running a program

1. Start the simulator.
2. Select **Load Program from File**.
3. Enter the program file name or path, for example `test_program.txt`.
4. Enter a valid starting address, usually `0`.
5. Select **Run All** to execute the program, or **Run Step-by-Step** to execute one instruction at a time.
6. Select **Display Machine State** to inspect the registers and current CPU state.

Run each test program from a clean machine state. Use the simulator's reset functionality where available, or restart the application before loading the next test.

## Test Programs

The repository includes `test_program.txt` as a sample input program.

Additional simple Vole test programs can be placed in a separate folder, for example:

```text
Test_Programs/
    01_add_and_display.txt
    02_substitute_memory.txt
    03_copy_register.txt
    ...
```

Load and run each file separately. Record the actual output from the simulator when preparing test evidence and screenshots.

## Repository Structure

```text
CS213_A2/
├── CPU.cpp
├── CPU.h
├── Machine.cpp
├── Machine.h
├── Memory.cpp
├── Memory.h
├── Menu.cpp
├── Menu.h
├── Program_Loader.cpp
├── Program_Loader.h
├── Registers.cpp
├── Registers.h
├── Screen.cpp
├── Screen.h
├── main.cpp
├── test_program.txt
├── README.md
└── LICENSE
```

## Team

This project was developed as a team assignment for **CS213 – Object-Oriented Programming**.

| Team Member | Student ID |
|---|---:|
| Amr Emad ElDin Toukhy | 20250453 |
| Khaled Amr Mohamed | 20250211 |
| Yousef Sameh Abdelsamad | 20250769 |
| Ahmed Aymen Fouad | 20250014 |

**Section:** S5 / S6   Leader:** Khaled Amr Mohamed

