#include "Puzzle.h"
#include <unordered_set>

bool Puzzle::isValid() const {
    if (n <= 0) return false;

    if ((int)regionId.size() != n) return false;
    for (const auto& row : regionId) {
        if ((int)row.size() != n) return false;
    }

    if ((int)queen.size() != n) return false;
    for (const auto& row : queen) {
        if ((int)row.size() != n) return false;
    }

    std::unordered_set<int> uniq;
    uniq.reserve((size_t)n);

    int maxId = -1;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            const int id = regionId[r][c];
            if (id < 0) return false;
            uniq.insert(id);
            if (id > maxId) maxId = id;
        }
    }

    if ((int)uniq.size() != n) return false;
    if (n > 26) return false; // max A-Z

    if (!regionColors.empty()) {
        if ((int)regionColors.size() != n) return false;
    }

    // pastikan semua id ada di range [0..n-1]
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            const int id = regionId[r][c];
            if (id < 0 || id >= n) return false;
        }
    }

    return true;
}

void Puzzle::clearQueens() {
    queen.assign(n, std::vector<bool>(n, false));
}

QString Puzzle::toAscii(char queenChar, char emptyChar) const {
    QString out;
    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            out += (queen[r][c] ? queenChar : emptyChar);
        }
        out += "\n";
    }
    return out;
}

QString Puzzle::toAsciiOverlay(char queenChar) const {
    QString out;
    if (n <= 0) return out;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            if (queen.size() == (size_t)n && queen[r].size() == (size_t)n && queen[r][c]) {
                out += queenChar;
            } else {
                int id = 0;
                if (regionId.size() == (size_t)n && regionId[r].size() == (size_t)n) {
                    id = regionId[r][c];
                }
                out += (id >= 0 && id < 26) ? QChar('A' + id) : QChar('?');
            }
        }
        out += "\n";
    }
    return out;
}