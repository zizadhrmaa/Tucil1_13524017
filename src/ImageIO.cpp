#include "ImageIO.h"
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QPainter>
#include <unordered_map>

static QColor niceColor(int i) {
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

bool ImageIO::loadFromTxt(const QString& path, Puzzle& out, QString* err) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (err) *err = "Gagal membuka file.";
        return false;
    }

    QTextStream in(&f);
    std::vector<QString> lines;
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (!line.isEmpty()) lines.push_back(line);
    }

    if (lines.empty()) {
        if (err) *err = "File kosong.";
        return false;
    }

    const int n = (int)lines.size();

    if (n > 26) {
        if (err) *err = "Ukuran papan terlalu besar. Maksimum 26x26 (A-Z).";
        return false;
    }

    for (const auto& ln : lines) {
        if (ln.size() != n) {
            if (err) *err = "Input harus papan NxN (jumlah kolom = jumlah baris).";
            return false;
        }
    }

    std::unordered_map<char, int> mp;
    mp.reserve((size_t)n);
    int nextId = 0;

    out.n = n;
    out.regionId.assign(n, std::vector<int>(n, 0));
    out.queen.assign(n, std::vector<bool>(n, false));

    for (int r = 0; r < n; r++) {
        for (int c = 0; c < n; c++) {
            const QChar qc = lines[r].at(c);
            const char ch = qc.toLatin1();

            if (ch < 'A' || ch > 'Z') {
                if (err) {
                    *err = QString("Karakter tidak valid di baris %1 kolom %2. "
                                   "Hanya boleh huruf A-Z.")
                               .arg(r + 1).arg(c + 1);
                }
                return false;
            }

            auto it = mp.find(ch);
            if (it == mp.end()) {
                mp.emplace(ch, nextId);
                out.regionId[r][c] = nextId;
                nextId++;
            } else {
                out.regionId[r][c] = it->second;
            }
        }
    }

    if (nextId != n) {
        if (err) {
            *err = QString("Jumlah region tidak valid. Ditemukan %1 region unik, "
                           "harus tepat %2 (sesuai ukuran papan).")
                       .arg(nextId).arg(n);
        }
        return false;
    }

    out.regionColors.resize(n);
    for (int i = 0; i < n; i++) 
        out.regionColors[i] = niceColor(i);

    if (!out.isValid()) {
        if (err) *err = "Puzzle invalid setelah parsing (cek format input).";
        return false;
    }
    return true;
}

bool ImageIO::loadFromImage(const QString& path, Puzzle& out, QString* err) {
    QImage img(path);
    if (img.isNull()) {
        if (err) *err = "Gagal membaca gambar.";
        return false;
    }

    img = img.convertToFormat(QImage::Format_ARGB32);

    const int W = img.width();
    const int H = img.height();
    if (W <= 0 || H <= 0) {
        if (err) *err = "Ukuran gambar tidak valid.";
        return false;
    }

    if (W != H) {
        if (err) *err = "Gambar harus persegi (width == height) untuk di-parse sebagai papan NxN.";
        return false;
    }

    auto isDark = [](QRgb px) -> bool {
        const int r = qRed(px), g = qGreen(px), b = qBlue(px);
        return (r + g + b) / 3 < 40;
    };

    std::vector<int> hLines;
    hLines.reserve(H);

    for (int y = 0; y < H; y++) {
        int darkCount = 0;
        for (int x = 0; x < W; x++) {
            if (isDark(img.pixel(x, y))) darkCount++;
        }
        const double frac = (double)darkCount / (double)W;
        if (frac > 0.90) hLines.push_back(y);
    }

    std::vector<int> vLines;
    vLines.reserve(W);

    for (int x = 0; x < W; x++) {
        int darkCount = 0;
        for (int y = 0; y < H; y++) {
            if (isDark(img.pixel(x, y))) darkCount++;
        }
        const double frac = (double)darkCount / (double)H;
        if (frac > 0.90) vLines.push_back(x);
    }

    auto compressLines = [](const std::vector<int>& lines) {
        std::vector<int> reps;
        for (int i = 0; i < (int)lines.size(); ) {
            int j = i;
            while (j + 1 < (int)lines.size() && lines[j + 1] == lines[j] + 1) j++;
            reps.push_back(lines[(i + j) / 2]);
            i = j + 1;
        }
        return reps;
    };

    const std::vector<int> h = compressLines(hLines);
    const std::vector<int> v = compressLines(vLines);

    std::vector<int> yCuts;
    std::vector<int> xCuts;

    if (!h.empty() && !v.empty()) {
        yCuts.clear();
        xCuts.clear();

        yCuts.push_back(-1);
        for (int yy : h) yCuts.push_back(yy);
        yCuts.push_back(H);

        xCuts.push_back(-1);
        for (int xx : v) xCuts.push_back(xx);
        xCuts.push_back(W);
    }

    int n = 0;

    if (!yCuts.empty() && !xCuts.empty()) {
        n = (int)yCuts.size() - 1;
        const int nx = (int)xCuts.size() - 1;
        if (n != nx) {
            if (err) *err = "Deteksi grid tidak konsisten (jumlah cell baris != kolom).";
            return false;
        }

        int validRows = 0;
        for (int i = 0; i + 1 < (int)yCuts.size(); i++) {
            int top = yCuts[i] + 1;
            int bot = yCuts[i + 1] - 1;
            if (top <= bot) validRows++;
        }
        int validCols = 0;
        for (int i = 0; i + 1 < (int)xCuts.size(); i++) {
            int left = xCuts[i] + 1;
            int right = xCuts[i + 1] - 1;
            if (left <= right) validCols++;
        }
        if (validRows != validCols) {
            if (err) *err = "Deteksi cell gagal (area cell terlalu tipis / garis terlalu rapat).";
            return false;
        }
        n = validRows;
    } else {
        n = W;
    }

    if (n <= 0) {
        if (err) *err = "Gagal menentukan ukuran papan dari gambar.";
        return false;
    }
    if (n > 26) {
        if (err) *err = "Ukuran papan dari gambar terlalu besar. Maksimum 26x26 (A-Z).";
        return false;
    }

    out.n = n;
    out.regionId.assign(n, std::vector<int>(n, 0));
    out.queen.assign(n, std::vector<bool>(n, false));

    std::unordered_map<QRgb, int> colorToId;
    colorToId.reserve((size_t)n);

    std::vector<QColor> regionColorList;
    regionColorList.reserve((size_t)n);

    auto addColor = [&](QRgb key) -> int {
        auto it = colorToId.find(key);
        if (it != colorToId.end()) return it->second;
        const int id = (int)colorToId.size();
        colorToId.emplace(key, id);
        regionColorList.push_back(QColor(key));
        return id;
    };

    if (!yCuts.empty() && !xCuts.empty()) {
        int rr = 0;
        for (int i = 0; i + 1 < (int)yCuts.size(); i++) {
            const int top = yCuts[i] + 1;
            const int bot = yCuts[i + 1] - 1;
            if (top > bot) continue;

            int cc = 0;
            for (int j = 0; j + 1 < (int)xCuts.size(); j++) {
                const int left = xCuts[j] + 1;
                const int right = xCuts[j + 1] - 1;
                if (left > right) continue;

                const int sy = (top + bot) / 2;
                const int sx = (left + right) / 2;
                const QRgb px = img.pixel(sx, sy);

                if (isDark(px)) {
                    if (err) *err = "Sampling warna cell kena garis grid (terlalu gelap). Pastikan cell berisi warna solid.";
                    return false;
                }

                out.regionId[rr][cc] = addColor(px);
                cc++;
            }
            rr++;
        }
    } else {
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                const QRgb px = img.pixel(c, r);
                if (isDark(px)) {
                    if (err) *err = "Terdeteksi pixel gelap (mirip garis grid) pada mode fallback. "
                                    "Gunakan gambar dengan grid lines yang jelas, atau gambar NxN pixel murni.";
                    return false;
                }
                out.regionId[r][c] = addColor(px);
            }
        }
    }

    const int regionCount = (int)colorToId.size();
    if (regionCount != n) {
        if (err) {
            *err = QString("Jumlah region dari gambar tidak valid. Ditemukan %1 warna/region unik, "
                           "harus tepat %2 (sesuai ukuran papan).")
                       .arg(regionCount).arg(n);
        }
        return false;
    }

    out.regionColors = regionColorList;

    if (!out.isValid()) {
        if (err) *err = "Puzzle invalid setelah parsing dari gambar.";
        return false;
    }
    return true;
}

bool ImageIO::saveSolutionTxt(const QString& path, const Puzzle& pz, const SolveStats& st, QString* err) {
    if (!pz.isValid()) {
        if (err) *err = "Puzzle tidak valid.";
        return false;
    }

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if (err) *err = "Gagal menyimpan file txt.";
        return false;
    }

    QTextStream out(&f);

    for (int r = 0; r < pz.n; r++) {
        for (int c = 0; c < pz.n; c++) {
            if (pz.queen[r][c]) {
                out << '#';
            } else {
                int id = pz.regionId[r][c];
                QChar ch = (id >= 0 && id < 26) ? QChar('A' + id) : QChar('?');
                out << ch;
            }
        }
        out << "\n";
    }

    out << "\n";
    out << "Waktu pencarian: " << st.elapsedMs << " ms\n";
    out << "Banyak kasus yang ditinjau: " << st.casesChecked << " kasus\n";

    return true;
}

bool ImageIO::saveSolutionImage(const QString& path, const Puzzle& pz, QString* err) {
    if (!pz.isValid()) {
        if (err) *err = "Puzzle tidak valid.";
        return false;
    }

    const int cell = 60;
    const int pad = 10;
    const int W = pad*2 + pz.n * cell;
    const int H = pad*2 + pz.n * cell;

    QImage outImg(W, H, QImage::Format_ARGB32);
    outImg.fill(Qt::white);

    QPainter p(&outImg);
    p.setRenderHint(QPainter::Antialiasing, true);

    for (int r = 0; r < pz.n; r++) {
        for (int c = 0; c < pz.n; c++) {
            int id = pz.regionId[r][c];
            QColor col = (id >= 0 && id < (int)pz.regionColors.size()) 
                ? pz.regionColors[id] : QColor("#E5E7EB");
            QRect rect(pad + c*cell, pad + r*cell, cell, cell);
            p.fillRect(rect, col);
            p.setPen(QPen(Qt::black, 1));
            p.drawRect(rect);
            if (pz.queen[r][c]) {
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

    if (!outImg.save(path)) {
        if (err) *err = "Gagal menyimpan image.";
        return false;
    }
    return true;
}