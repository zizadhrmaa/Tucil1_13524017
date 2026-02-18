# Tucil1_13524017
Tugas Kecil 1 IF2211 Strategi Algoritma: Penyelesaian Permainan Queens LinkedIn

Aplikasi solver untuk permainan Queens LinkedIn menggunakan algoritma brute force dan backtracking dengan GUI berbasis Qt6.

## Penjelasan Singkat Program

Program ini adalah solver untuk permainan Queens LinkedIn, yaitu varian dari masalah N-Queens dengan aturan tambahan berupa region. Program dapat:
- Membaca puzzle dari file .txt atau gambar (dengan deteksi region otomatis)
- Menyelesaikan puzzle menggunakan dua mode:
  - **Pure Brute Force**: Mencoba semua kemungkinan tanpa pruning
  - **Optimized Backtracking**: Menggunakan pruning untuk efisiensi
- Menampilkan proses solving secara real-time
- Menyimpan solusi dalam format .txt atau .png

Aturan puzzle:
- Setiap baris, kolom, dan region harus memiliki tepat 1 queen
- Queen tidak boleh bertetangga dalam 8 arah (termasuk diagonal)
- Papan berukuran NxN dengan N region (maksimal 26)

## Requirement Program

### Software
- **C++ Compiler**: GCC 9+ atau Clang 10+ (support C++17)
- **CMake**: versi 3.16 atau lebih baru
- **Qt6**: Qt6 Widgets dan Qt6 Gui
- **Ninja**: build system (opsional, bisa diganti Make)

### Instalasi Dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build
sudo apt install qt6-base-dev libqt6widgets6 libqt6gui6
```

## Cara Mengkompilasi Program

### Metode 1: Menggunakan Makefile (Recommended)
```bash
make          # build dan run
make build    # build saja
make clean    # hapus build folder
make rebuild  # clean dan build ulang
```

### Metode 2: Manual dengan CMake
```bash
# Configure
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build -j

# Clean build (jika diperlukan)
cmake --build build --clean-first
```

## Cara Menjalankan dan Menggunakan Program

### Menjalankan Program
Setelah compile, jalankan executable:
```bash
./bin/Tucil1_13524017
```

Atau jika menggunakan Makefile:
```bash
make run
```

### Menggunakan Program

1. **Load Puzzle**:
   - Klik "Open .txt" untuk membuka file puzzle format teks
   - Klik "Open Image" untuk membuka puzzle dari gambar

2. **Solve Puzzle**:
   - Pilih mode: Pure Brute Force atau Optimized Backtracking
   - Klik "Solve (Brute Force)"
   - Lihat progress solving secara real-time

3. **Export Solusi**:
   - Klik "Export Solution TXT" untuk menyimpan sebagai .txt
   - Klik "Export Solution PNG" untuk menyimpan sebagai gambar

### Format Input File .txt
```
AABBC
AABBC
DDEEC
DDFFC
DDFFC
```

- Setiap huruf merepresentasikan region (A-Z)
- Papan harus NxN
- Jumlah region harus tepat N

### Format Input Gambar

- Gambar harus persegi (width = height)
- Setiap region dibedakan dengan warna berbeda
- Garis grid hitam untuk memisahkan cell (opsional)
- Jumlah region/warna harus tepat N

## Author

**Nama**: Aziza Dharma Putri  
**NIM**: 13524017  
**Prodi**: Teknik Informatika  
**Institusi**: Institut Teknologi Bandung

## Struktur File
```
.
├── CMakeLists.txt          # CMake configuration
├── Makefile                # Build shortcuts
├── README.md               # Dokumentasi
├── .gitignore              # Git ignore rules
├── src/
│   ├── main.cpp            # Entry point
│   ├── MainWindow.h/cpp    # Main window GUI
│   ├── BoardWidget.h/cpp   # Board display widget
│   ├── Puzzle.h/cpp        # Puzzle data structure
│   ├── Solver.h/cpp        # Solving algorithms
│   └── ImageIO.h/cpp       # File I/O handler
│   └── txt2img.cpp         # Change txt to png
├── test/                   # Test puzzles
├── doc/                    # Documentation
├── build/                  # Build files (generated)
└── bin/                    # Executable (generated)
```