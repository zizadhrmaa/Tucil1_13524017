#pragma once
#include "Puzzle.h"
#include <QObject>
#include <atomic>
#include <chrono>
#include <QMetaType>
#include <QString>

struct SolveStats {
    long long casesChecked = 0;
    int elapsedMs = 0;
    bool found = false;
};

Q_DECLARE_METATYPE(SolveStats)

class Solver : public QObject {
    Q_OBJECT
public:
    explicit Solver(QObject* parent = nullptr);

public slots:
    void solve(Puzzle pz);
    void cancel();
    void setOptimized(bool on);   

signals:
    void progress(Puzzle snapshot, long long casesChecked);            
    void finished(Puzzle result, SolveStats stats, QString message);   

private:
    std::atomic<bool> m_cancel{false};
    std::atomic<bool> m_optimized{false};

    bool dfsPure(Puzzle& pz, int row, SolveStats& st, int emitEvery);
    bool dfsOptimized(Puzzle& pz, int row, SolveStats& st, int emitEvery);

    bool isCompleteValid(const Puzzle& pz) const;
    bool canPlace(const Puzzle& pz, int r, int c) const;
};
