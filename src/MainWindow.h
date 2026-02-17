#pragma once
#include "BoardWidget.h"
#include "Solver.h"
#include "Puzzle.h"
#include <QMainWindow>
#include <QThread>

class QLabel;
class QPushButton;
class QCheckBox;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onOpenTxt();
    void onOpenImage();
    void onSolve();
    void onCancel();
    void onExportImage();
    void onExportTxt();
    void onProgress(Puzzle snap, long long cases);
    void onFinished(Puzzle result, SolveStats stats, QString msg);
    void onOptimizedToggled(bool checked);

private:
    void setUiEnabled(bool enabled);

    BoardWidget* m_board = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_statsCases = nullptr;
    QLabel* m_statsTime = nullptr;
    QPushButton* m_btnSolve = nullptr;
    QPushButton* m_btnOpenTxt = nullptr;
    QPushButton* m_btnOpenImg = nullptr;
    QPushButton* m_btnExportTxt = nullptr;
    QPushButton* m_btnExportImg = nullptr;
    QCheckBox* m_chkOptimized = nullptr;

    Puzzle m_current;
    SolveStats m_lastStats;
    QThread m_workerThread;
    Solver* m_solver = nullptr;
    bool m_solving = false;
};