#include "BoardWidget.h"
#include <QPainter>

BoardWidget::BoardWidget(QWidget* parent) : QWidget(parent) {
    setAutoFillBackground(true);
}

void BoardWidget::setPuzzle(const Puzzle& pz) {
    m_pz = pz;
    update();
}

QSize BoardWidget::minimumSizeHint() const {
    return QSize(520, 520);
}

void BoardWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    if (!m_pz.isValid()) {
        p.setPen(Qt::black);
        p.drawText(rect(), Qt::AlignCenter, "Belum ada puzzle / puzzle invalid");
        return;
    }

    int n = m_pz.n;
    int pad = 10;
    int availW = width() - 2*pad;
    int availH = height() - 2*pad;
    int cell = std::max(10, std::min(availW, availH) / n);

    int boardW = n * cell;
    int boardH = n * cell;
    int ox = (width() - boardW) / 2;
    int oy = (height() - boardH) / 2;

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            int id = m_pz.regionId[r][c];
            QColor col = (id >= 0 && id < (int)m_pz.regionColors.size()) 
                ? m_pz.regionColors[id] : QColor("#E5E7EB");
            
            QRect rect(ox + c*cell, oy + r*cell, cell, cell);
            p.fillRect(rect, col);
            p.setPen(QPen(Qt::black, 1));
            p.drawRect(rect);
            if (m_pz.queen[r][c]) {
                p.setPen(QPen(QColor("#FFD700"), 1));
                p.setBrush(QColor("#FFD700"));
                QFont f = p.font();
                f.setPixelSize(cell * 0.6);
                f.setBold(true);
                p.setFont(f);
                p.drawText(rect, Qt::AlignCenter, "♛");
                p.setBrush(Qt::NoBrush);
            }
        }
    }
}