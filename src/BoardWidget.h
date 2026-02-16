#pragma once
#include "Puzzle.h"
#include <QWidget>

class BoardWidget : public QWidget {
    Q_OBJECT
public:
    explicit BoardWidget(QWidget* parent = nullptr);

    void setPuzzle(const Puzzle& pz);
    const Puzzle& puzzle() const { return m_pz; }

    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* e) override;

private:
    Puzzle m_pz;
};