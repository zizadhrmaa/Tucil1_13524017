#include "MainWindow.h"
#include "ImageIO.h"
#include <QToolBar>
#include <QAction>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QWidget>
#include <QMetaObject>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Queens Solver - Tucil1_13524017");

    auto* tb = addToolBar("Main");
    auto* actOpenTxt = tb->addAction("Open .txt");
    auto* actOpenImg = tb->addAction("Open Image");
    auto* actExportTxt = tb->addAction("Export Solution TXT");
    auto* actExport = tb->addAction("Export Solution PNG");
    
    connect(actOpenTxt, &QAction::triggered, this, &MainWindow::onOpenTxt);
    connect(actOpenImg, &QAction::triggered, this, &MainWindow::onOpenImage);
    connect(actExportTxt, &QAction::triggered, this, &MainWindow::onExportTxt);
    connect(actExport, &QAction::triggered, this, &MainWindow::onExportImage);

    auto* central = new QWidget(this);
    auto* v = new QVBoxLayout(central);

    m_board = new BoardWidget(this);
    v->addWidget(m_board, 1);

    auto* h = new QHBoxLayout();
    m_btnSolve = new QPushButton("Solve (Brute Force)", this);
    m_btnCancel = new QPushButton("Cancel", this);
    m_btnCancel->setEnabled(false);

    m_chkOptimized = new QCheckBox("Enable Optimasi (Backtracking)", this);
    m_chkOptimized->setChecked(false);

    h->addWidget(m_btnSolve);
    h->addWidget(m_btnCancel);
    h->addSpacing(16);
    h->addWidget(m_chkOptimized);
    h->addStretch(1);
    v->addLayout(h);

    m_stats = new QLabel("Cases: 0 | Time: 0 ms", this);
    v->addWidget(m_stats);

    setCentralWidget(central);

    m_status = new QLabel("Load puzzle dulu (.txt atau image).", this);
    statusBar()->addWidget(m_status, 1);

    connect(m_btnSolve, &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(m_btnCancel, &QPushButton::clicked, this, &MainWindow::onCancel);
    connect(m_chkOptimized, &QCheckBox::toggled, this, &MainWindow::onOptimizedToggled);

    m_solver = new Solver();
    m_solver->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_solver, &QObject::deleteLater);
    connect(m_solver, &Solver::progress, this, &MainWindow::onProgress, Qt::QueuedConnection);
    connect(m_solver, &Solver::finished, this, &MainWindow::onFinished, Qt::QueuedConnection);

    m_workerThread.start();

    onOptimizedToggled(m_chkOptimized->isChecked());
}

MainWindow::~MainWindow() {
    if (m_solver) {
        QMetaObject::invokeMethod(m_solver, "cancel", Qt::QueuedConnection);
    }
    m_workerThread.quit();
    m_workerThread.wait();
}

void MainWindow::setUiEnabled(bool enabled) {
    m_btnSolve->setEnabled(enabled);
    m_btnCancel->setEnabled(!enabled);
    if (m_chkOptimized) m_chkOptimized->setEnabled(enabled);
}

void MainWindow::onOptimizedToggled(bool checked) {
    if (!m_solver) return;
    QMetaObject::invokeMethod(m_solver, "setOptimized", Qt::QueuedConnection,
                              Q_ARG(bool, checked));

    if (checked) 
        m_status->setText("Mode: Optimized (Backtracking)");
    else 
        m_status->setText("Mode: Pure Brute Force (Exhaustive)");
}

void MainWindow::onOpenTxt() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Open Puzzle (.txt)", "", "Text files (*.txt)");
    if (path.isEmpty()) return;

    QString err;
    Puzzle pz;
    if (!ImageIO::loadFromTxt(path, pz, &err)) {
        QMessageBox::warning(this, "Error", err);
        return;
    }

    m_current = pz;
    m_board->setPuzzle(m_current);
    m_status->setText("Loaded: " + path);
    m_stats->setText("Cases: 0 | Time: 0 ms");
}

void MainWindow::onOpenImage() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Open Puzzle Image", "", "Images (*.png *.jpg *.jpeg)");
    if (path.isEmpty()) return;

    QString err;
    Puzzle pz;
    if (!ImageIO::loadFromImage(path, pz, &err)) {
        QMessageBox::information(this, "Info", err);
        return;
    }

    m_current = pz;
    m_board->setPuzzle(m_current);
    m_status->setText("Loaded image: " + path);
    m_stats->setText("Cases: 0 | Time: 0 ms");
}

void MainWindow::onSolve() {
    if (!m_current.isValid()) {
        QMessageBox::information(this, "Info", "Puzzle belum valid. Load .txt dulu.");
        return;
    }

    setUiEnabled(false);
    m_status->setText("Solving... (live update aktif)");
    m_stats->setText("Cases: 0 | Time: ...");

    QMetaObject::invokeMethod(m_solver, "setOptimized", Qt::QueuedConnection, Q_ARG(bool, m_chkOptimized->isChecked()));
    QMetaObject::invokeMethod(m_solver, "solve", Qt::QueuedConnection, Q_ARG(Puzzle, m_current));
}

void MainWindow::onCancel() {
    m_status->setText("Cancelling...");
    m_btnCancel->setEnabled(false);

    if (m_solver) {
        QMetaObject::invokeMethod(m_solver, "cancel", Qt::QueuedConnection);
    }
}

void MainWindow::onExportImage() {
    if (!m_current.isValid()) {
        QMessageBox::information(this, "Info", "Belum ada puzzle.");
        return;
    }

    const QString path = QFileDialog::getSaveFileName(this, "Save Solution Image", "solution.png", "PNG (*.png)");
    if (path.isEmpty()) return;

    QString err;
    if (!ImageIO::saveSolutionImage(path, m_board->puzzle(), &err)) {
        QMessageBox::warning(this, "Error", err);
        return;
    }
    m_status->setText("Saved image: " + path);
}

void MainWindow::onExportTxt() {
    if (!m_current.isValid()) {
        QMessageBox::information(this, "Info", "Belum ada solusi.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Save Solution TXT", "solution.txt", "Text files (*.txt)");
    if (path.isEmpty()) return;

    QString err;
    if (!ImageIO::saveSolutionTxt(path, m_board->puzzle(), m_lastStats, &err)) {
        QMessageBox::warning(this, "Error", err);
        return;
    }

    m_status->setText("Saved TXT: " + path);
}

void MainWindow::onProgress(Puzzle snap, long long cases) {
    m_board->setPuzzle(std::move(snap));
    m_stats->setText(QString("Cases: %1 | Time: ...").arg(cases));
}

void MainWindow::onFinished(Puzzle result, SolveStats stats, QString msg) {
    setUiEnabled(true);
    m_board->setPuzzle(result);
    m_current = result;
    m_lastStats = stats;
    m_stats->setText(QString("Cases: %1 | Time: %2 ms")
                     .arg(stats.casesChecked)
                     .arg(stats.elapsedMs));
    m_status->setText(msg);
}