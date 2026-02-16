#pragma once
#include "Puzzle.h" 
#include "Solver.h"
#include <QString>

namespace ImageIO {
    bool loadFromTxt(const QString& path, Puzzle& out, QString* err = nullptr);
    bool loadFromImage(const QString& path, Puzzle& out, QString* err = nullptr);
    bool saveSolutionTxt(const QString& path, const Puzzle& pz, const SolveStats& stats, QString* err = nullptr);
    bool saveSolutionImage(const QString& path, const Puzzle& pz, QString* err = nullptr);
}
