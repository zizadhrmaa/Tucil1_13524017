#include "Solver.h"
#include <QThread>
#include <vector>

Solver::Solver(QObject* parent) : QObject(parent) {}

void Solver::cancel() {
    m_cancel.store(true);
}

void Solver::setOptimized(bool on) {
    m_optimized.store(on);
}

void Solver::solve(Puzzle pz) {
    m_cancel.store(false);
    SolveStats st;

    if (!pz.isValid()) {
        emit finished(pz, st, "Puzzle invalid.");
        return;
    }

    pz.clearQueens();

    auto start = std::chrono::steady_clock::now();

    const int emitEvery = 2000;
    bool ok = m_optimized.load() ? dfsOptimized(pz, 0, st, emitEvery) 
                                 : dfsPure(pz, 0, st, emitEvery);

    auto end = std::chrono::steady_clock::now();
    st.elapsedMs = (int)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    st.found = ok;

    QString msg = ok ? "Solusi ditemukan." 
                     : (m_cancel.load() ? "Dibatalkan." : "Tidak ada solusi.");
    emit finished(pz, st, msg);
}

// Brute force murni
bool Solver::dfsPure(Puzzle& pz, int row, SolveStats& st, int emitEvery) {
    if (m_cancel.load()) return false;

    if (row == pz.n) {
        return isCompleteValid(pz);
    }

    for (int c = 0; c < pz.n; c++) {
        if (m_cancel.load()) return false;

        st.casesChecked++;

        // reset baris ini, lalu taruh queen di kolom c
        for (int k = 0; k < pz.n; k++) 
            pz.queen[row][k] = false;
        pz.queen[row][c] = true;

        if (emitEvery > 0 && (st.casesChecked % emitEvery == 0)) {
            emit progress(pz, st.casesChecked);
            QThread::yieldCurrentThread();
        }

        if (dfsPure(pz, row + 1, st, emitEvery)) 
            return true;

        pz.queen[row][c] = false;
    }
    return false;
}

// Versi dengan pruning untuk skip posisi yang jelas invalid
bool Solver::dfsOptimized(Puzzle& pz, int row, SolveStats& st, int emitEvery) {
    if (m_cancel.load()) return false;
    if (row == pz.n) return true;

    for (int c = 0; c < pz.n; c++) {
        if (m_cancel.load()) return false;

        st.casesChecked++;

        if (canPlace(pz, row, c)) {
            pz.queen[row][c] = true;

            if (emitEvery > 0 && (st.casesChecked % emitEvery == 0)) {
                emit progress(pz, st.casesChecked);
                QThread::yieldCurrentThread();
            }

            if (dfsOptimized(pz, row + 1, st, emitEvery)) 
                return true;

            pz.queen[row][c] = false;
        }
    }
    return false;
}

bool Solver::isCompleteValid(const Puzzle& pz) const {
    const int n = pz.n;
    std::vector<int> colCount(n, 0);
    std::vector<int> regCount(n, 0);

    for (int r = 0; r < n; r++) {
        int rowQueens = 0;
        int placedCol = -1;

        for (int c = 0; c < n; c++) {
            if (pz.queen[r][c]) {
                rowQueens++;
                placedCol = c;
            }
        }

        if (rowQueens != 1) return false;

        colCount[placedCol]++;
        if (colCount[placedCol] > 1) return false;

        const int reg = pz.regionId[r][placedCol];
        regCount[reg]++;
        if (regCount[reg] > 1) return false;

        // cek apakah ada queen tetangga (8 arah)
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue;
                const int nr = r + dr;
                const int nc = placedCol + dc;
                if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
                    if (pz.queen[nr][nc]) return false;
                }
            }
        }
    }

    return true;
}

bool Solver::canPlace(const Puzzle& pz, int r, int c) const {
    // cek kolom
    for (int rr = 0; rr < pz.n; rr++) {
        if (pz.queen[rr][c]) return false;
    }

    // cek tetangga 8 arah
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < pz.n && nc >= 0 && nc < pz.n) {
                if (pz.queen[nr][nc]) return false;
            }
        }
    }

    // cek region
    int myRegion = pz.regionId[r][c];
    for (int rr = 0; rr < pz.n; rr++) {
        for (int cc = 0; cc < pz.n; cc++) {
            if (pz.regionId[rr][cc] == myRegion && pz.queen[rr][cc]) 
                return false;
        }
    }

    return true;
}