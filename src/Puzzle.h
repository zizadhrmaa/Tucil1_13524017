#pragma once
#include <vector>
#include <QString>
#include <QColor>
#include <QMetaType>

struct Puzzle {
    int n = 0;
    std::vector<std::vector<int>> regionId;
    std::vector<std::vector<bool>> queen;
    std::vector<QColor> regionColors;

    bool isValid() const;
    void clearQueens();
    QString toAscii(char queenChar = '#', char emptyChar = '.') const;
    QString toAsciiOverlay(char queenChar = '#') const;
};

Q_DECLARE_METATYPE(Puzzle)