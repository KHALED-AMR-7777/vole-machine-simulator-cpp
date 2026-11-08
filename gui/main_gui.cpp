// Part 3 (GUI) - entry point of the graphical Vole Machine Simulator.
// The console version keeps its own main() in the A2-... file.
//
// Optional: pass a program file on the command line to open it right away:
//     vole_gui test_program.txt

#include <QApplication>
#include <QString>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");

    MainWindow window;
    window.show();

    if (argc > 1)
        window.openProgram(QString::fromLocal8Bit(argv[1]));

    return app.exec();
}
