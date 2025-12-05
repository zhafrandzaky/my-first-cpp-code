# WhatsApp Simulator - Queue Implementation

Simple Code C++ untuk simulasi WhatsApp menggunakan Queue dengan Doubly Linked List.

## 📝 Deskripsi

Program ini mensimulasikan sistem pesan WhatsApp dengan fitur:
- ✉️ Tambah pesan baru (dengan spam filter)
- 🗑️ Hapus pesan pertama (FIFO - First In First Out)
- ❌ Batalkan pesan terakhir (LIFO - Last In First Out)
- 📋 Lihat semua pesan
- ✓ Tandai pesan sebagai dibaca
- 🎨 Interface dengan warna dan mouse support
- ⌨️ Text editor dengan selection, copy, paste

## 🐧 Cara Menjalankan di Linux

Program ini dibuat untuk Linux, jadi bisa langsung compile dan run tanpa konfigurasi tambahan.

### Langkah 1: Pastikan g++ Terinstall

Cek apakah g++ sudah ada:

```bash
g++ --version
```

Jika belum ada, install dengan:

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install g++ -y

# Fedora/RHEL
sudo dnf install gcc-c++ -y

# Arch Linux
sudo pacman -S gcc
```

### Langkah 2: Masuk ke Folder Project

```bash
cd /path/to/my-first-cpp-code
```

### Langkah 3: Compile Program

```bash
g++ wa.cpp -o wa
```

### Langkah 4: Jalankan Program

```bash
./wa
```

🎉 **Program berjalan sempurna dengan semua fitur (mouse, warna, UI)!**

---

## 🚀 Cara Menjalankan di Windows

### ⚠️ Catatan Penting
Program ini dibuat untuk Linux dan menggunakan library POSIX (`termios.h`, `unistd.h`, `sys/ioctl.h`).

## 📊 Perbandingan Metode

| Metode | Kompabilitas | Mouse/UI | Kemudahan | File Tambahan | Rekomendasi |
|--------|-------------|----------|-----------|---------------|-------------|
| Docker | ✅ 100% | ⚠️ Terbatas | ⭐⭐⭐⭐⭐ | ❌ Tidak perlu | **TERMUDAH** |
| WSL | ✅ 100% | ✅ Sempurna | ⭐⭐⭐⭐ | ❌ Tidak perlu | **TERBAIK** |
| MinGW | ❌ Tidak jalan | ❌ Error | ⭐⭐ | ❌ Tidak perlu | Tidak disarankan |

---

### Opsi 1: Menggunakan Docker 🐳 (TERMUDAH - Tanpa File Tambahan)

**✅ Kelebihan:**
- Setup paling cepat (hanya butuh Docker Desktop)
- **Tidak perlu file script tambahan** - langsung dari README
- 100% kompatibel untuk compile
- Portable dan isolated

**⚠️ Kekurangan:**
- Terminal UI terbatas
- File size installer besar (~500MB)
- Mouse tracking kadang tidak berfungsi

#### Langkah 1: Install Docker Desktop

1. Buka browser, kunjungi: **https://www.docker.com/products/docker-desktop/**
2. Klik **Download for Windows**
3. Tunggu download selesai (~500MB)

#### Langkah 2: Install Docker Desktop

1. Jalankan installer **Docker Desktop Installer.exe**
2. Klik **OK** untuk menggunakan WSL 2 (recommended)
3. Tunggu instalasi selesai (~5-10 menit)
4. Klik **Close and restart** 
5. **RESTART komputer**

#### Langkah 3: Jalankan Docker Desktop

1. Setelah restart, buka **Docker Desktop** dari Start Menu
2. Tunggu sampai muncul status **"Engine running"** (pojok kiri bawah)
3. Bisa minimize, Docker berjalan di background

#### Langkah 4: Buka Terminal (PowerShell/CMD)

1. Tekan **Windows + R**
2. Ketik `powershell` → Enter
3. Pindah ke folder project:

```powershell
cd C:\struda\my-first-cpp-code
```

⚠️ **Sesuaikan path** dengan lokasi folder Anda!

#### Langkah 5: Jalankan Docker Container

Di PowerShell, jalankan:

```powershell
docker run -it --rm -v ${PWD}:/app gcc bash
```

**Penjelasan command:**
- `docker run` = Jalankan container
- `-it` = Interactive terminal
- `--rm` = Hapus container setelah selesai
- `-v ${PWD}:/app` = Mount folder sekarang ke `/app` di container
- `gcc` = Gunakan image GCC (compiler C++)
- `bash` = Buka bash shell

**Pertama kali** akan download image GCC (~400MB), tunggu sampai selesai (~5-10 menit).

#### Langkah 6: Compile Program

Setelah masuk ke container (prompt berubah jadi `root@xxxxx:/`), jalankan:

```bash
cd /app
g++ wa.cpp -o wa
```

Jika **TIDAK muncul error**, berarti compile berhasil!

#### Langkah 7: Jalankan Program

```bash
./wa
```

⚠️ **Catatan:** Mouse tracking dan beberapa fitur UI mungkin tidak bekerja sempurna.

#### Langkah 8: Keluar dari Container

Setelah selesai, ketik:

```bash
exit
```

Container akan otomatis terhapus (karena flag `--rm`).

---

### Opsi 2: Menggunakan WSL ⭐⭐⭐⭐ (TERBAIK - Full Features)

**✅ Kelebihan:**
- 100% kompatibel dengan code Linux
- Semua fitur berjalan sempurna (mouse, warna, UI)
- Setup mudah dan cepat
- Ringan (tidak perlu virtual machine penuh)

#### Langkah 1: Install WSL
1. Buka **PowerShell** sebagai Administrator:
   - Tekan **Windows + X**
   - Pilih **Terminal (Admin)** atau **Windows PowerShell (Admin)**
   
2. Jalankan command:
```powershell
wsl --install
```

3. Tunggu proses instalasi selesai (~5-15 menit tergantung koneksi internet)
4. **RESTART komputer** setelah instalasi selesai

#### Langkah 2: Setup Ubuntu (Pertama Kali)
Setelah restart, Ubuntu akan otomatis terbuka untuk setup awal:

1. Tunggu proses instalasi Ubuntu selesai (~5 menit)
2. Buat username baru (misalnya: `zhafran`)
   - Ketik username → Enter
   - **Catatan:** Username harus huruf kecil, tanpa spasi
3. Buat password
   - Ketik password → Enter
   - **Catatan:** Saat mengetik password, TIDAK MUNCUL apa-apa di layar (normal!)
4. Konfirmasi password (ketik ulang password yang sama)
5. Tunggu sampai muncul prompt seperti: `zhafran@DESKTOP-XXX:~$`

✅ **WSL berhasil terinstall!**

#### Langkah 3: Update Package Manager
Di terminal Ubuntu/WSL yang sudah terbuka, jalankan:

```bash
sudo apt update
```

- Ketik password yang tadi dibuat → Enter
- Tunggu update selesai (~1-2 menit)

#### Langkah 4: Install Compiler C++
Jalankan:

```bash
sudo apt install g++ -y
```

- Tunggu instalasi selesai (~2-3 menit)
- **Tidak perlu ketik password lagi** (karena baru saja pakai sudo)

#### Langkah 5: Verifikasi Instalasi
Cek apakah g++ berhasil terinstall:

```bash
g++ --version
```

Jika muncul versi g++ (contoh: `g++ (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0`), berarti **BERHASIL!**

#### Langkah 6: Akses File Windows dari WSL

File Windows bisa diakses di folder `/mnt/`:
- Drive `C:\` → `/mnt/c/`
- Drive `D:\` → `/mnt/d/`

Pindah ke folder project:

```bash
cd /mnt/c/struda/my-first-cpp-code
```

⚠️ **Sesuaikan path** dengan lokasi folder Anda!

**Tips:** Gunakan **Tab** untuk auto-complete path, contoh:
- Ketik: `cd /mnt/c/str` lalu tekan **Tab**

#### Langkah 7: Verifikasi Lokasi File
Cek apakah sudah di folder yang benar:

```bash
pwd
```

Output harusnya: `/mnt/c/struda/my-first-cpp-code`

Lihat daftar file:

```bash
ls
```

Harusnya muncul: `wa.cpp`, `README.md`, dll.

#### Langkah 8: Compile Program

```bash
g++ wa.cpp -o wa
```

Jika **TIDAK muncul error**, berarti compile berhasil!

#### Langkah 9: Jalankan Program

```bash
./wa
```

🎉 **Program berjalan sempurna dengan semua fitur!**

---

#### Tips Menggunakan WSL:

**Cara buka WSL lagi setelah ditutup:**
- Cara 1: Ketik `wsl` di PowerShell/CMD
- Cara 2: Ketik `ubuntu` di Start Menu
- Cara 3: Buka aplikasi **Ubuntu** dari Start Menu

**Edit code:**
- Tetap edit di VS Code Windows seperti biasa
- File otomatis sinkron dengan WSL
- Compile & run di WSL terminal

**Copy paste di WSL:**
- Copy: Blok text → Klik kanan
- Paste: Klik kanan di terminal

---

### Opsi 3: Menggunakan MinGW-w64 via MSYS2 ⭐⭐ (TIDAK Recommended)

**⚠️ PERINGATAN:** Metode ini **TIDAK AKAN JALAN** 100% karena code menggunakan library Linux yang tidak tersedia di Windows native.

**❌ Masalah yang akan muncul:**
- Error: `termios.h: No such file or directory`
- Error: `unistd.h: No such file or directory`
- Mouse tracking tidak berfungsi
- Raw mode terminal tidak support
- ANSI color codes mungkin tidak muncul

**🎯 Rekomendasi:** Gunakan **Docker** (Opsi 1) atau **WSL** (Opsi 2) untuk hasil terbaik!

---

Jika tetap ingin mencoba MinGW:

#### Langkah 1: Download MSYS2

1. Buka browser, kunjungi: **https://www.msys2.org/**
2. Klik tombol download **msys2-x86_64-xxxxxxxx.exe**
3. Tunggu download selesai (~100MB)

#### Langkah 2: Install MSYS2

1. Jalankan file installer yang sudah didownload
2. Klik **Next** → **Next** → **Next**
3. Install di lokasi default: `C:\msys64`
4. Klik **Finish**
5. ✅ **Centang** "Run MSYS2 now" (atau biarkan ter-centang)

#### Langkah 3: Update Package Database

Setelah install, terminal MSYS2 akan otomatis terbuka. Di terminal tersebut:

```bash
pacman -Syu
```

- Ketik `Y` lalu tekan **Enter** untuk konfirmasi
- Tunggu proses update selesai
- ⚠️ **Terminal akan tertutup otomatis** setelah update

#### Langkah 4: Buka Terminal MSYS2 MINGW64 (PENTING!)

**JANGAN buka MSYS2 MSYS biasa!** Buka yang ini:

1. Tekan tombol **Windows** di keyboard
2. Ketik: `MINGW64`
3. Klik **MSYS2 MINGW64** (icon biru/ungu dengan tulisan "MINGW64")

Atau cari di:
```
Start Menu → MSYS2 → MSYS2 MINGW64
```

#### Langkah 5: Install Compiler C++

Di terminal **MSYS2 MINGW64** yang baru dibuka, jalankan:

```bash
pacman -S mingw-w64-x86_64-gcc
```

- Ketik `Y` lalu tekan **Enter**
- Tunggu instalasi selesai (ukuran ~50-100 MB)

#### Langkah 6: Verifikasi Instalasi

Cek apakah g++ sudah terinstall:

```bash
g++ --version
```

Jika muncul output seperti ini, berarti **BERHASIL**:
```
g++ (Rev6, Built by MSYS2 project) 13.2.0
Copyright (C) 2023 Free Software Foundation, Inc.
...
```

#### Langkah 7: Masuk ke Direktori Project

Di terminal MSYS2 MINGW64, pindah ke folder project:

⚠️ **Penting:** Format path di MSYS2 berbeda dengan Windows!
- Windows: `C:\struda\my-first-cpp-code`
- MSYS2: `/c/struda/my-first-cpp-code`

Jalankan:
```bash
cd /c/struda/my-first-cpp-code
```

**Tips:** Sesuaikan path di atas dengan lokasi folder Anda!
- Drive `C:\` menjadi `/c/`
- Drive `D:\` menjadi `/d/`
- Backslash `\` menjadi forward slash `/`

Cek apakah sudah benar dengan:
```bash
pwd
ls
```

Harusnya muncul file `wa.cpp`, `README.md`, dll.

#### Langkah 8: Compile Program

Jalankan perintah compile:

```bash
g++ wa.cpp -o wa.exe
```

**⚠️ AKAN MUNCUL ERROR** karena library Linux tidak tersedia!

#### Langkah 9: Jalankan Program (Jika Berhasil Compile)

```bash
./wa.exe
```

**❌ Kemungkinan besar TIDAK AKAN JALAN** karena:
- Error: `termios.h: No such file or directory` → Library Linux tidak ada
- Error: `unistd.h: No such file or directory` → Library Linux tidak ada
- Error: `sys/ioctl.h: No such file or directory` → Library Linux tidak ada
- Mouse tracking tidak berfungsi
- Raw mode terminal tidak support
- ANSI color codes mungkin tidak muncul

**🎯 SOLUSI: Gunakan Docker (Opsi 1) atau WSL (Opsi 2) - 100% Kompatibel!**

---

## 📚 Struktur Data

Program ini menggunakan:
- **Doubly Linked List**: Untuk implementasi Queue
- **Queue (FIFO)**: Untuk sistem antrian pesan
- **Struct**: Untuk menyimpan data pesan dan node

## 🎮 Fitur Interface

- **Mouse Support**: Klik 2x untuk memilih menu dan konfirmasi
- **Keyboard Navigation**: Arrow keys (↑↓←→) untuk navigasi
- **Text Selection**: Shift+Arrow, Ctrl+Arrow, Alt+Arrow untuk seleksi teks
- **Color Coding**: Pesan belum dibaca (hijau), sudah dibaca (abu-abu)

## 👨‍💻 Developer

Dibuat untuk pembelajaran Data Structure & Algorithm - Queue Implementation.
