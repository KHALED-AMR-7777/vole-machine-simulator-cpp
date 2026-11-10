VOLE MACHINE SIMULATOR (C++)

A C++ Object-Oriented Programming project that simulates the Vole
Machine and executes machine-language programs through a graphical
interface built with Qt.

OVERVIEW

The simulator models memory, registers, a CPU, and screen output. This
document explains how to run the full GUI application and rebuild it
from source.

FEATURES

-   256 memory cells (00-FF), each one byte.
-   16 general-purpose registers (R0-RF).
-   Two-byte instructions represented by four hexadecimal digits.
-   Load Vole programs from .txt files.
-   Validate and execute instructions using Fetch -> Decode -> Validate
    -> Execute.
-   Run one instruction at a time or run continuously.
-   Adjust execution speed.
-   Inspect the program, memory, CPU state, registers, screen output,
    and messages.
-   Edit memory cells and register values through the GUI.
-   Restart execution and clear the machine.
-   Memory-mapped screen output at address FF.

MACHINE SPECIFICATIONS

Memory: 256 cells (00-FF) Cell size: 1 byte Registers: 16 (R0-RF)
Instruction size: 2 bytes / 4 hexadecimal digits Program Counter (PC):
Address of the next instruction Instruction Register (IR): Currently
fetched instruction Screen address: FF

The PC normally advances by two for each instruction. A jump instruction
can change the PC to another address.

SUPPORTED INSTRUCTION SET

Each instruction contains four hexadecimal digits, commonly represented
as OP R X Y.

1 - Load from memory: Loads the byte at address XY into register R. 2 -
Load immediate: Loads hexadecimal value XY into register R. 3 - Store:
Stores register R at address XY. Writing to FF sends the value to the
screen. 4 - Move register: Copies a value between registers using the
required instruction field format. 5 - Integer addition: Adds registers
RX and RY, storing the result in R. 6 - Floating-point addition: Adds
floating-point values represented by RX and RY, storing the result in R.
7 - OR: Performs bitwise OR on RX and RY, storing the result in R. 8 -
AND: Performs bitwise AND on RX and RY, storing the result in R. 9 -
XOR: Performs bitwise XOR on RX and RY, storing the result in R. A -
Rotate right: Rotates register R right by Y bit positions. B -
Conditional jump: If register R equals R0, jumps to address XY. C -
Halt: Stops program execution.

PROGRAM FILE FORMAT

Programs are plain-text .txt files: - One instruction per line. - Each
instruction contains four hexadecimal characters (0-9, A-F). -
Instructions are loaded into consecutive memory locations. - Use a valid
even starting address. - Lines beginning with semicolon (;) are
comments. - Blank lines are ignored.

Example: ; Load 5 into R1 2105 ; Load 3 into R2 2203 ; Add R1 and R2,
store the result in R3 5312 ; Send R3 to the screen 33FF ; Stop
execution C000

Expected result: R3 contains 08, and the screen output displays 08.

RUN THE FULL GUI APPLICATION (WINDOWS)

A ready-to-run Windows build is provided in the build folder.

1.  Download or copy the entire build folder.
2.  If shared as a ZIP file, extract the ZIP completely.
3.  Open the extracted build folder.
4.  Double-click vole_gui.exe.
5.  Keep all DLL files and Qt plugin folders together with the
    executable. Do not move or delete individual runtime files.

The prepared portable folder is intended to run without a separate Qt,
MSYS2, or MinGW installation. It has been tested after being copied to
another location on the developer’s computer; testing on another Windows
computer is still recommended.

USING THE GUI

1.  Click “Open program…” and select a Vole .txt program.
2.  Set the starting address if needed (default: 00).
3.  Click “Load”.
4.  Click “Step” to execute one instruction, or “Run” to execute
    continuously.
5.  Adjust the steps-per-second slider to change execution speed.
6.  Inspect the Program, Memory, CPU, Registers, Screen, and Messages
    areas.
7.  Use “Restart” to restart execution and “Clear machine” to clear the
    machine state.

BUILD THE GUI FROM SOURCE

Requirements: - Windows. - CMake and Ninja. - A C++ compiler compatible
with the selected Qt kit. - Qt 6 development files.

The development build used MSYS2 UCRT64, with Qt installed at C:.

Run these commands from the source directory containing CMakeLists.txt:

cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64
-DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe cmake –build build

The GUI executable is generated in the build directory.

PORTABLE WINDOWS FOLDER

The portable folder must include the executable, required runtime DLL
files, and the Qt platform plugin directory (including the Windows
platform plugin). Keep the folder structure intact.

Qt’s windeployqt tool can copy Qt runtime files. Additional
compiler/runtime DLL dependencies may be required depending on the
toolchain. Test the final folder with development-tool directories
removed from PATH, and preferably on another Windows computer.

CONSOLE VERSION

If the repository also contains the console version, it is a separate
application and may be built independently using its own source files
and a C++17-compatible compiler. Use the GUI instructions above to run
the full graphical application.

TEAM

Amr Emad ElDin Toukhy - 20250453 Khaled Amr Mohamed - 20250211 Yousef
Sameh Abdelsamad - 20250769 Ahmed Aymen Fouad - 20250014

Section: S5 / S6 Team Leader: Khaled Amr Mohamed

NOTES

-   Keep the portable build folder intact when sharing the application.
-   Use valid Vole machine-language .txt files.
-   Test each sample program from a clean machine state.
-   Include only screenshots, videos, and test results actually produced
    by the team.
