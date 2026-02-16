#include "MainWindow.h"
#include "Puzzle.h"
#include "Solver.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qRegisterMetaType<Puzzle>();
    qRegisterMetaType<SolveStats>();

    MainWindow w;
    w.show();

    return app.exec();
}
