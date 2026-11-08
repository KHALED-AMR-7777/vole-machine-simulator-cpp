VOLE MACHINE SIMULATOR - GUI VERSION
====================================

A graphical front end (Qt Widgets) for our Vole machine simulator. It shows the
whole machine at once: the program, the 256 memory cells, the 16 registers, the
PC and IR, and the screen. You can load a program, run it one instruction at a
time or automatically, pause at any moment, and change memory or registers by
hand.

The GUI uses our own simulator classes (CPU, Machine, Memory, Registers,
Screen, ProgramLoader). It does not contain a second copy of the machine.


WHAT YOU NEED TO INSTALL
------------------------
1. Qt 6 (free, open source). Download the "Qt Online Installer" from
   https://www.qt.io/download-open-source
   In the installer choose:
      - Qt 6.x  ->  "MinGW 64-bit"
      - Developer and Designer Tools -> "Qt Creator", "CMake", "Ninja",
        "MinGW 64-bit" compiler
   (Any Qt 6 version works. Qt 5.15 should also work.)

2. Nothing else. The simulator classes are plain standard C++.


HOW TO BUILD AND RUN (easiest way: Qt Creator)
----------------------------------------------
1. Put the GUI files in the project folder next to the simulator files:

      CMakeLists.txt        <- from this package (project root)
      gui/MainWindow.h
      gui/MainWindow.cpp
      gui/main_gui.cpp
      CPU.cpp, Machine.cpp, Memory.cpp, Program_Loader.cpp, Screen.cpp,
      Registers.cpp (or Registery.cpp), and all the .h files

   Before building, read CORE_GETTERS.txt: a few getter functions must exist in
   CPU and Machine.

2. Open Qt Creator -> File -> Open File or Project -> choose CMakeLists.txt.
3. Pick the "Desktop Qt 6.x MinGW 64-bit" kit and press Configure Project.
4. Press the green Run button (Ctrl+R).

Using VS Code instead:
   Install the "CMake Tools" extension, then in .vscode/settings.json add
      "cmake.configureArgs": ["-DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/mingw_64"]
   (use your real Qt folder), choose the MinGW kit, then Build and Run.

Command line (MinGW):
   cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/mingw_64
   cmake --build build
   build/vole_gui.exe test_program.txt
   (If it complains about missing DLLs, run  windeployqt build/vole_gui.exe
    or start it from Qt Creator.)

The console version of the simulator (Menu) is unchanged and still builds from
its own main file.


HOW TO USE IT
-------------
1. Program
   - Click "Open program..." and choose a .txt file. Each line is one
     instruction: 4 hex digits (for example 2105). Lines starting with ;
     are comments.
   - "Start at address" says where in memory the first instruction goes
     (hex, must be even: 00, 02, 04 ...). Press "Load" to apply a change.
   - If the file has a problem (wrong digits, too long, bad address) the
     banner at the top and the Messages panel explain why.

2. Run
   - Step    runs exactly one instruction.
   - Run     runs automatically. Press again (it becomes "Pause") to stop at
             any moment. The slider sets the speed.
   - Restart reloads the file and starts over.
   - Clear machine empties memory, registers and screen.

What you see
   - Program list   every instruction with a plain-English explanation.
                    Blue = next to run, green = the one just run.
   - Memory grid    256 cells. Double-click a cell to change it (hex).
                    Cell FF is the screen and cannot be edited.
   - Registers      R0-RF in hex, decimal and binary. Double-click the Hex
                    column to change a register.
   - CPU            PC, IR, the next instruction, and the number of steps.
   - Screen         everything the program stored to address FF, as hex and
                    as text.
   - Messages       what happened (loading, edits, errors).
   Anything that changed in the last step is highlighted in yellow.

Keyboard shortcuts
   Ctrl+O  open program      Ctrl+L  load       F10  step
   F5      run / pause       Ctrl+R  restart


TRY IT
------
test_program.txt (in the repository) adds 5 and 3, stores the result 08 on the
screen, and halts. Open it, press Step five times and watch R3 become 08 and
"08" appear on the Screen.
