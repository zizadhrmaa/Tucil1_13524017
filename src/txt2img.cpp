#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <vector>
#include <unordered_map>

static QColor regionColor(int i) {
    static std::vector<QColor> palette = {
        QColor("#94A3B8"), QColor("#6EE7B7"), QColor("#93C5FD"), QColor("#F9A8D4"),
        QColor("#C4B5FD"), QColor("#FCA5A5"), QColor("#67E8F9"), QColor("#86EFAC"),
        QColor("#FF6B6B"), QColor("#4ECDC4"), QColor("#45B7D1"), QColor("#BB8FCE"),
        QColor("#F97316"), QColor("#98D8C8"), QColor("#A78BFA"), QColor("#34D399"),
        QColor("#FB7185"), QColor("#38BDF8"), QColor("#A3E635"), QColor("#E879F9"),
        QColor("#2DD4BF"), QColor("#457B9D"), QColor("#1D3557"), QColor("#166534"),
        QColor("#7C3AED"), QColor("#9F1239")
    };
    return palette[i % (int)palette.size()];
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    if (argc < 3) {
        qWarning("Usage: txt2img <input.txt> <output.png>");
        return 1;
    }

    const QString inputPath  = argv[1];
    const QString outputPath = argv[2];

    QFile f(inputPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Gagal membuka file: %s", qPrintable(inputPath));
        return 1;
    }

    QTextStream in(&f);
    std::vector<QString> lines;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) lines.push_back(line);
    }

    if (lines.empty()) {
        qWarning("File kosong.");
        return 1;
    }

    const int n = (int)lines.size();
    for (const auto& ln : lines) {
        if (ln.size() != n) {
            qWarning("Input harus NxN.");
            return 1;
        }
    }

    std::unordered_map<char, int> mp;
    int nextId = 0;
    std::vector<std::vector<int>> regionId(n, std::vector<int>(n, 0));

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            char ch = lines[r].at(c).toLatin1();
            if (ch < 'A' || ch > 'Z') {
                qWarning("Karakter tidak valid: %c", ch);
                return 1;
            }
            auto it = mp.find(ch);
            if (it == mp.end()) {
                mp.emplace(ch, nextId);
                regionId[r][c] = nextId++;
            } else {
                regionId[r][c] = it->second;
            }
        }
    }

    if (nextId != n) {
        qWarning("Jumlah region harus tepat %d, ditemukan %d.", n, nextId);
        return 1;
    }

    const int cell     = 60;
    const int gridLine = 2;
    const int size     = n * cell + (n + 1) * gridLine;

    QImage img(size, size, QImage::Format_ARGB32);
    img.fill(Qt::black);

    QPainter p(&img);

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            const int x = gridLine + c * (cell + gridLine);
            const int y = gridLine + r * (cell + gridLine);
            p.fillRect(x, y, cell, cell, regionColor(regionId[r][c]));
        }
    }

    p.end();

    if (!img.save(outputPath)) {
        qWarning("Gagal menyimpan gambar: %s", qPrintable(outputPath));
        return 1;
    }

    qInfo("Berhasil disimpan: %s (%dx%d px)", qPrintable(outputPath), size, size);
    return 0;
}