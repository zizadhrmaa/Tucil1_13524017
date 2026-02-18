#include "MainWindow.h"
#include "ImageIO.h"
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QStatusBar>
#include <QWidget>
#include <QMetaObject>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Queens Solver - Tucil1_13524017");
    setMinimumSize(1000, 660);

    auto* central = new QWidget(this);
    central->setObjectName("centralBg");

    auto* rootV = new QVBoxLayout(central);
    rootV->setContentsMargins(24, 16, 24, 16);
    rootV->setSpacing(12);

    auto* header = new QLabel("Queens Solver", central);
    header->setObjectName("headerTitle");
    header->setFixedHeight(48);
    rootV->addWidget(header);

    auto* card = new QFrame(central);
    card->setObjectName("mainCard");
    auto* bodyH = new QHBoxLayout(card);
    bodyH->setContentsMargins(20, 20, 20, 20);
    bodyH->setSpacing(24);

    m_board = new BoardWidget(card);
    m_board->setMinimumSize(480, 480);
    bodyH->addWidget(m_board, 1);

    auto* panel = new QWidget(card);
    panel->setFixedWidth(340);
    auto* pv = new QVBoxLayout(panel);
    pv->setContentsMargins(8, 8, 8, 8);
    pv->setSpacing(20);

    auto* lblUpload = new QLabel("Unggah file untuk dicarikan solusi", panel);
    lblUpload->setObjectName("sectionTitle");
    pv->addWidget(lblUpload);

    auto* hUpload = new QHBoxLayout();
    hUpload->setSpacing(10);
    m_btnOpenTxt = new QPushButton("Buka file .txt", panel);
    m_btnOpenTxt->setObjectName("outlineBtn");
    m_btnOpenImg = new QPushButton("Buka gambar", panel);
    m_btnOpenImg->setObjectName("outlineBtn");
    hUpload->addWidget(m_btnOpenTxt);
    hUpload->addWidget(m_btnOpenImg);
    pv->addLayout(hUpload);

    auto* sep1 = new QFrame(panel);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setObjectName("separator");
    pv->addWidget(sep1);

    auto* lblSolve = new QLabel("Cari solusi permasalahan", panel);
    lblSolve->setObjectName("sectionTitle");
    pv->addWidget(lblSolve);

    m_chkOptimized = new QCheckBox("Optimasi", panel);
    m_chkOptimized->setChecked(false);
    pv->addWidget(m_chkOptimized);

    m_btnSolve = new QPushButton("Selesaikan (Metode Brute Force)", panel);
    m_btnSolve->setObjectName("solveBtn");
    m_btnSolve->setFixedHeight(44);
    m_btnSolve->setEnabled(false);
    pv->addWidget(m_btnSolve);

    auto* cardStats = new QFrame(panel);
    cardStats->setObjectName("statsCard");
    auto* statsV = new QVBoxLayout(cardStats);
    statsV->setContentsMargins(14, 16, 14, 16);
    statsV->setSpacing(10);

    auto* lblHasil = new QLabel("Hasil pencarian solusi", cardStats);
    lblHasil->setObjectName("statsTitle");
    statsV->addWidget(lblHasil);

    m_statsCases = new QLabel("• Banyak percobaan : -", cardStats);
    m_statsCases->setObjectName("statsItem");
    statsV->addWidget(m_statsCases);

    m_statsTime = new QLabel("• Waktu pencarian : -", cardStats);
    m_statsTime->setObjectName("statsItem");
    statsV->addWidget(m_statsTime);

    pv->addWidget(cardStats);

    auto* sep2 = new QFrame(panel);
    sep2->setFrameShape(QFrame::HLine);
    sep2->setObjectName("separator");
    pv->addWidget(sep2);

    auto* lblSave = new QLabel("Simpan hasil pencarian", panel);
    lblSave->setObjectName("sectionTitle");
    pv->addWidget(lblSave);

    auto* hSave = new QHBoxLayout();
    hSave->setSpacing(10);
    m_btnExportTxt = new QPushButton("Simpan file .txt", panel);
    m_btnExportTxt->setObjectName("outlineBtn");
    m_btnExportImg = new QPushButton("Simpan gambar", panel);
    m_btnExportImg->setObjectName("outlineBtn");
    hSave->addWidget(m_btnExportTxt);
    hSave->addWidget(m_btnExportImg);
    pv->addLayout(hSave);

    pv->addStretch(1);

    m_status = new QLabel("Load puzzle dulu.", panel);
    m_status->setObjectName("statusLabel");
    m_status->setWordWrap(true);
    pv->addWidget(m_status);

    bodyH->addWidget(panel);
    rootV->addWidget(card, 1);
    setCentralWidget(central);

    connect(m_btnOpenTxt, &QPushButton::clicked, this, &MainWindow::onOpenTxt);
    connect(m_btnOpenImg, &QPushButton::clicked, this, &MainWindow::onOpenImage);
    connect(m_btnSolve,   &QPushButton::clicked, this, &MainWindow::onSolve);
    connect(m_btnExportTxt, &QPushButton::clicked, this, &MainWindow::onExportTxt);
    connect(m_btnExportImg, &QPushButton::clicked, this, &MainWindow::onExportImage);
    connect(m_chkOptimized, &QCheckBox::toggled,  this, &MainWindow::onOptimizedToggled);

    m_solver = new Solver();
    m_solver->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, m_solver, &QObject::deleteLater);
    connect(m_solver, &Solver::progress, this, &MainWindow::onProgress, Qt::QueuedConnection);
    connect(m_solver, &Solver::finished, this, &MainWindow::onFinished, Qt::QueuedConnection);

    m_workerThread.start();

    onOptimizedToggled(m_chkOptimized->isChecked());

    setStyleSheet(R"(
        QWidget#centralBg {
            background-color: #FFB3C6;
        }
        QFrame#mainCard {
            background-color: white;
            border-radius: 16px;
        }
        QLabel#headerTitle {
            font-size: 26px;
            font-weight: bold;
            color: #1a1a1a;
            background-color: transparent;
        }
        QLabel#sectionTitle {
            font-size: 16px;
            font-weight: bold;
            color: #C2415C;
            background-color: transparent;
        }
        QFrame#statsCard {
            background-color: #FFF0F3;
            border-radius: 10px;
            border: 1px solid #FFB3C6;
        }
        QLabel#statsTitle {
            font-size: 15px;
            font-weight: bold;
            color: #6D2B3D;
            background-color: transparent;
        }
        QLabel#statsItem {
            font-size: 14px;
            color: #6D2B3D;
            background-color: transparent;
        }
        QLabel#statusLabel {
            font-size: 13px;
            color: #6D2B3D;
            background-color: #FFF0F3;
            border-radius: 8px;
            padding: 10px;
        }
        QPushButton#outlineBtn {
            background-color: white;
            color: #C2415C;
            border: 1.5px solid #FFB3C6;
            border-radius: 20px;
            padding: 8px 16px;
            font-size: 14px;
        }
        QPushButton#outlineBtn:hover {
            background-color: #FFF0F3;
            border-color: #FF85A1;
        }
        QPushButton#outlineBtn:pressed {
            background-color: #FFD6E0;
        }
        QPushButton#solveBtn {
            background-color: #FF85A1;
            color: white;
            border: none;
            border-radius: 22px;
            font-size: 15px;
            font-weight: bold;
        }
        QPushButton#solveBtn:hover {
            background-color: #E05C7A;
        }
        QPushButton#solveBtn:pressed {
            background-color: #C2415C;
        }
        QPushButton#solveBtn:disabled {
            background-color: #FFCCD5;
            color: #D4A0AC;
        }
        QCheckBox {
            color: #6D2B3D;
            font-size: 14px;
            spacing: 8px;
            background-color: transparent;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 4px;
            border: 2px solid #FFB3C6;
            background-color: white;
        }
        QCheckBox::indicator:checked {
            background-color: #FF85A1;
            border-color: #FF85A1;
        }
        QFrame#separator {
            color: #FFD6E0;
            background-color: #FFD6E0;
            max-height: 1px;
        }
        QStatusBar {
            background-color: #FFB3C6;
            color: #6D2B3D;
            font-size: 12px;
        }
    )");
}

MainWindow::~MainWindow() {
    if (m_solver) {
        QMetaObject::invokeMethod(m_solver, "cancel", Qt::QueuedConnection);
    }
    m_workerThread.quit();
    m_workerThread.wait();
}

void MainWindow::setUiEnabled(bool enabled) {
    m_btnOpenTxt->setEnabled(enabled);
    m_btnOpenImg->setEnabled(enabled);
    m_chkOptimized->setEnabled(enabled);
    m_btnExportTxt->setEnabled(enabled);
    m_btnExportImg->setEnabled(enabled);

    m_solving = !enabled;
    if (!enabled) {
        m_btnSolve->setText("Batalkan pencarian");
        m_btnSolve->setObjectName("cancelBtn");
        m_btnSolve->setStyleSheet(
            "QPushButton { background-color: #6D2B3D; color: white; border: none;"
            "border-radius: 22px; font-size: 14px; font-weight: bold; }"
            "QPushButton:hover { background-color: #8B3A50; }"
        );
    } else {
        m_btnSolve->setText("Selesaikan (Metode Brute Force)");
        m_btnSolve->setObjectName("solveBtn");
        m_btnSolve->setStyleSheet("");
        m_btnSolve->setEnabled(m_current.isValid());
    }
}

void MainWindow::onOptimizedToggled(bool checked) {
    if (!m_solver) return;
    QMetaObject::invokeMethod(m_solver, "setOptimized", Qt::QueuedConnection,
                              Q_ARG(bool, checked));
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
    m_status->setText("File dimuat: " + path);
    m_statsCases->setText("• Banyak percobaan : -");
    m_statsTime->setText("• Waktu pencarian : -");
    m_btnSolve->setEnabled(true);
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
    m_status->setText("File dimuat: " + path);
    m_statsCases->setText("• Banyak percobaan : -");
    m_statsTime->setText("• Waktu pencarian : -");
    m_btnSolve->setEnabled(true);
}

void MainWindow::onSolve() {
    if (m_solving) {
        onCancel();
        return;
    }

    if (!m_current.isValid()) return;

    setUiEnabled(false);
    m_status->setText("Mencari solusi...");
    m_statsCases->setText("• Banyak percobaan : 0");
    m_statsTime->setText("• Waktu pencarian : ...");

    QMetaObject::invokeMethod(m_solver, "setOptimized", Qt::QueuedConnection,
                              Q_ARG(bool, m_chkOptimized->isChecked()));
    QMetaObject::invokeMethod(m_solver, "solve", Qt::QueuedConnection,
                              Q_ARG(Puzzle, m_current));
}

void MainWindow::onCancel() {
    m_status->setText("Membatalkan...");
    if (m_solver) {
        m_solver->cancel();
    }
}
void MainWindow::onExportTxt() {
    if (!m_current.isValid()) {
        QMessageBox::information(this, "Info", "Belum ada solusi.");
        return;
    }

    QString path = QFileDialog::getSaveFileName(
        this, "Save Solution TXT", "solution.txt", "Text files (*.txt)");
    if (path.isEmpty()) return;

    QString err;
    if (!ImageIO::saveSolutionTxt(path, m_board->puzzle(), m_lastStats, &err)) {
        QMessageBox::warning(this, "Error", err);
        return;
    }
    m_status->setText("Tersimpan: " + path);
}

void MainWindow::onExportImage() {
    if (!m_current.isValid()) {
        QMessageBox::information(this, "Info", "Belum ada puzzle.");
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this, "Save Solution Image", "solution.png", "PNG (*.png)");
    if (path.isEmpty()) return;

    QString err;
    if (!ImageIO::saveSolutionImage(path, m_board->puzzle(), &err)) {
        QMessageBox::warning(this, "Error", err);
        return;
    }
    m_status->setText("Tersimpan: " + path);
}

void MainWindow::onProgress(Puzzle snap, long long cases) {
    m_board->setPuzzle(std::move(snap));
    m_statsCases->setText(QString("• Banyak percobaan : %1").arg(cases));
    m_statsTime->setText("• Waktu pencarian : ...");
}

void MainWindow::onFinished(Puzzle result, SolveStats stats, QString msg) {
    setUiEnabled(true);
    m_board->setPuzzle(result);
    m_current = result;
    m_lastStats = stats;
    m_statsCases->setText(QString("• Banyak percobaan : %1").arg(stats.casesChecked));
    m_statsTime->setText(QString("• Waktu pencarian : %1 ms").arg(stats.elapsedMs));
    m_status->setText(msg);
}