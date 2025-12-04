#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctime>
using namespace std;

// Struct untuk menyimpan data pesan
struct Pesan {
    string text;
    bool isRead;
    
    Pesan() : text(""), isRead(false) {}
    Pesan(string t, bool r = false) : text(t), isRead(r) {}
};

// Node untuk Doubly Linked List
struct Node {
    Pesan data;
    Node* next;
    Node* prev;
    
    Node(Pesan p) : data(p), next(nullptr), prev(nullptr) {}
};

// Class Queue menggunakan Doubly Linked List
class QueuePesan {
private:
    Node* front;  // Depan antrian (untuk dequeue)
    Node* rear;   // Belakang antrian (untuk enqueue)
    int size;
    
public:
    QueuePesan() : front(nullptr), rear(nullptr), size(0) {}
    
    // Destructor untuk membersihkan memory
    ~QueuePesan() {
        while (front != nullptr) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
    }
    
    // Mengecek apakah queue kosong
    bool isEmpty() {
        return front == nullptr;
    }
    
    // Mendapatkan jumlah pesan
    int getSize() {
        return size;
    }
    
    // Enqueue - Menambah pesan baru (dengan spam filter)
    bool enqueue(string text) {
        // Spam Filter: Cek apakah pesan sama dengan pesan yang sudah ada
        Node* current = front;
        while (current != nullptr) {
            if (current->data.text == text) {
                return false; // Tolak pesan (spam)
            }
            current = current->next;
        }
        
        Pesan newPesan(text, false);
        Node* newNode = new Node(newPesan);
        
        if (isEmpty()) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            newNode->prev = rear;
            rear = newNode;
        }
        size++;
        return true;
    }
    
    // Dequeue - Menghapus pesan pertama
    bool dequeue() {
        if (isEmpty()) {
            return false;
        }
        
        Node* temp = front;
        front = front->next;
        
        if (front == nullptr) {
            rear = nullptr;
        } else {
            front->prev = nullptr;
        }
        
        delete temp;
        size--;
        return true;
    }
    
    // Membatalkan pesan terakhir (Rear) - Fitur khusus Doubly Linked List
    bool cancelLast() {
        if (isEmpty()) {
            return false;
        }
        
        Node* temp = rear;
        rear = rear->prev;
        
        if (rear == nullptr) {
            front = nullptr;
        } else {
            rear->next = nullptr;
        }
        
        delete temp;
        size--;
        return true;
    }
    
    // Menandai pesan sebagai dibaca
    bool markAsRead(int index) {
        if (isEmpty() || index < 0 || index >= size) {
            return false;
        }
        
        Node* current = front;
        for (int i = 0; i < index && current != nullptr; i++) {
            current = current->next;
        }
        
        if (current != nullptr) {
            current->data.isRead = true;
            return true;
        }
        return false;
    }
    
    // Mendapatkan pointer ke front (untuk display)
    Node* getFront() {
        return front;
    }
};

// Fungsi gotoxy untuk positioning di terminal
void gotoxy(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

// Fungsi untuk clear screen
void clearScreen() {
    printf("\033[2J");
    printf("\033[H");
    fflush(stdout);
}

// Fungsi untuk set warna text
void setColor(int color) {
    printf("\033[%dm", color);
}

// Reset warna
void resetColor() {
    printf("\033[0m");
}

// Fungsi untuk convert string ke lowercase
string toLowerCase(string str) {
    string result = str;
    for (int i = 0; i < (int)result.length(); i++) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] + ('a' - 'A');
        }
    }
    return result;
}

// Fungsi untuk cek apakah string hanya berisi whitespace/newline
bool isEmptyOrWhitespace(const string& str) {
    for (int i = 0; i < (int)str.length(); i++) {
        if (str[i] != ' ' && str[i] != '\n' && str[i] != '\t' && str[i] != '\r') {
            return false; // Ada karakter selain whitespace
        }
    }
    return true; // Hanya whitespace atau kosong
}

// Fungsi untuk parse input multiple message numbers
// Format: "1,3,5" atau "1-3" atau "1,3-5,7" atau "all"
// Return: vector berisi index yang dipilih (0-based)
vector<int> parseMessageNumbers(const string& input, int maxSize) {
    vector<int> result;
    
    if (toLowerCase(input) == "all") {
        for (int i = 0; i < maxSize; i++) {
            result.push_back(i);
        }
        return result;
    }
    
    string current = "";
    for (int i = 0; i <= (int)input.length(); i++) {
        char ch = (i < (int)input.length()) ? input[i] : ',';
        
        if (ch == ',' || ch == ' ' || i == (int)input.length()) {
            if (!current.empty()) {
                // Check if it's a range (e.g., "1-3")
                int dashPos = -1;
                for (int j = 0; j < (int)current.length(); j++) {
                    if (current[j] == '-') {
                        dashPos = j;
                        break;
                    }
                }
                
                if (dashPos != -1) {
                    // Range format
                    string startStr = current.substr(0, dashPos);
                    string endStr = current.substr(dashPos + 1);
                    int start = atoi(startStr.c_str());
                    int end = atoi(endStr.c_str());
                    
                    if (start > 0 && end > 0 && start <= end && end <= maxSize) {
                        for (int j = start; j <= end; j++) {
                            result.push_back(j - 1); // Convert to 0-based
                        }
                    }
                } else {
                    // Single number
                    int num = atoi(current.c_str());
                    if (num > 0 && num <= maxSize) {
                        result.push_back(num - 1); // Convert to 0-based
                    }
                }
                current = "";
            }
        } else {
            current += ch;
        }
    }
    
    return result;
}

// Fungsi untuk enable mouse tracking
void enableMouseTracking() {
    printf("\033[?1000h"); // Enable mouse button tracking
    printf("\033[?1002h"); // Enable mouse motion tracking
    printf("\033[?1015h"); // Enable urxvt mouse mode
    printf("\033[?1006h"); // Enable SGR mouse mode
}

// Fungsi untuk disable mouse tracking
void disableMouseTracking() {
    printf("\033[?1000l");
    printf("\033[?1002l");
    printf("\033[?1015l");
    printf("\033[?1006l");
}

// Fungsi untuk set terminal ke raw mode
struct termios orig_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    fflush(stdout);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// Fungsi untuk menampilkan header
void displayHeader() {
    setColor(42); // Background hijau
    setColor(30); // Text hitam
    gotoxy(1, 1);
    printf("╔════════════════════════════════════════════════════════════════════════╗");
    gotoxy(1, 2);
    printf("║              WhatsApp Simulator - Queue Implementation                 ║");
    gotoxy(1, 3);
    printf("╚════════════════════════════════════════════════════════════════════════╝");
    resetColor();
    fflush(stdout);
}

// Fungsi untuk menampilkan menu
void displayMenu(int selected) {
    int menuY = 5;
    int menuX = 10;
    
    gotoxy(menuX, menuY);
    printf("╔══════════════════════════════════════════════╗");
    
    string options[] = {
        "1. Tambah Pesan Baru",
        "2. Hapus Pesan Pertama",
        "3. Batalkan Pesan Terakhir",
        "4. Lihat Semua Pesan",
        "5. Tandai Sebagai Dibaca",
        "6. Keluar"
    };
    
    for (int i = 0; i < 6; i++) {
        gotoxy(menuX, menuY + 1 + i);
        if (i == selected) {
            setColor(47); // Background putih
            setColor(30); // Text hitam
            printf("║ > %-43s║", options[i].c_str());
            resetColor();
        } else {
            printf("║   %-43s║", options[i].c_str());
        }
    }
    
    gotoxy(menuX, menuY + 7);
    printf("╚══════════════════════════════════════════════╝");
    
    gotoxy(menuX, menuY + 9);
    setColor(36); // Cyan
    printf("Mouse: Klik 2x | Keyboard: ↑↓ Enter");
    resetColor();
    fflush(stdout);
}

// Fungsi untuk menampilkan tombol kembali
bool displayBackButton(int y, int& lastClickedButton, time_t& lastClickTime) {
    int buttonY = y;
    int buttonX = 10;
    
    gotoxy(buttonX, buttonY);
    setColor(37); // Tanpa highlight karena hanya 1 tombol
    printf("╔════════════════════╗");
    gotoxy(buttonX, buttonY + 1);
    printf("║  [← KEMBALI]       ║");
    gotoxy(buttonX, buttonY + 2);
    printf("╚════════════════════╝");
    resetColor();
    
    gotoxy(buttonX, buttonY + 4);
    setColor(36);
    printf("Enter/Klik 2x untuk kembali");
    resetColor();
    fflush(stdout);
    
    const int DOUBLE_CLICK_THRESHOLD = 500;
    
    // Read input
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\033') { // ESC sequence
            char seq[5];
            if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                if (seq[0] == '[') {
                    if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                        if (seq[1] == '<') { // Mouse input
                            char mouseData[20];
                            int idx = 0;
                            while (read(STDIN_FILENO, &mouseData[idx], 1) == 1 && mouseData[idx] != 'M' && mouseData[idx] != 'm') {
                                idx++;
                            }
                            
                            if (mouseData[idx] == 'M') { // Mouse click
                                int mouseY = 0;
                                sscanf(mouseData, "%*d;%*d;%d", &mouseY);
                                
                                // Check if clicked on back button
                                if (mouseY >= buttonY && mouseY <= buttonY + 2) {
                                    struct timespec ts;
                                    clock_gettime(CLOCK_MONOTONIC, &ts);
                                    time_t currentTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
                                    
                                    if (lastClickedButton == 1 && 
                                        (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                        return true; // Double click - go back
                                    } else {
                                        lastClickedButton = 1;
                                        lastClickTime = currentTime;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (c == '\n' || c == '\r') { // Enter key
            return true; // Confirm - go back
        }
    }
    return false;
}

// Fungsi untuk menampilkan pesan
void displayMessages(QueuePesan& queue) {
    int lastClickedButton = -1;
    time_t lastClickTime = 0;
    bool shouldReturn = false;
    
    while (!shouldReturn) {
        clearScreen();
        displayHeader();
        
        gotoxy(10, 5);
        setColor(33); // Kuning
        printf("═══════════════════ DAFTAR PESAN ═══════════════════");
        resetColor();
        
        int y = 7;
        
        if (queue.isEmpty()) {
            gotoxy(10, y);
            setColor(31); // Merah
            printf("Tidak ada pesan.");
            resetColor();
            y++;
        } else {
            Node* current = queue.getFront();
            int index = 0;
            
            while (current != nullptr) {
                // Render pesan dengan multi-line support
                string pesan = current->data.text;
                bool isRead = current->data.isRead;
                
                // Split pesan berdasarkan newline
                int startPos = 0;
                bool firstLine = true;
                
                for (int i = 0; i <= (int)pesan.length(); i++) {
                    if (i == (int)pesan.length() || pesan[i] == '\n') {
                        gotoxy(10, y);
                        
                        if (firstLine) {
                            // Baris pertama dengan indikator
                            if (isRead) {
                                setColor(37); // Abu-abu (sudah dibaca)
                                printf("[√] Pesan %d: ", index + 1);
                            } else {
                                setColor(32); // Hijau (belum dibaca)
                                printf("[-] Pesan %d: ", index + 1);
                            }
                        } else {
                            // Baris selanjutnya dengan indentasi
                            char indent[50];
                            snprintf(indent, sizeof(indent), "[%c] Pesan %d: ", ' ', index + 1);
                            for (int j = 0; j < (int)strlen(indent); j++) {
                                printf(" ");
                            }
                        }
                        
                        // Print substring
                        string line = pesan.substr(startPos, i - startPos);
                        if (isRead) {
                            setColor(37);
                        } else {
                            setColor(32);
                        }
                        printf("%s", line.c_str());
                        resetColor();
                        
                        y++;
                        startPos = i + 1;
                        firstLine = false;
                    }
                }
                
                current = current->next;
                index++;
            }
        }
        
        gotoxy(10, y + 2);
        setColor(36);
        printf("Total pesan: %d", queue.getSize());
        resetColor();
        
        shouldReturn = displayBackButton(y + 4, lastClickedButton, lastClickTime);
    }
}

// Fungsi untuk input pesan baru
void inputNewMessage(QueuePesan& queue) {
    int lastClickedButton = -1;
    time_t lastClickTime = 0;
    bool shouldReturn = false;
    string message = "";
    bool messageSent = false;
    int selectedButton = 0; // 0 = Kirim, 1 = Kembali
    int focusArea = 0; // 0 = text input, 1 = buttons
    int cursorPos = 0; // Posisi cursor dalam text
    int selectionStart = -1; // -1 berarti tidak ada seleksi
    int selectionEnd = -1;
    int lastCursorPos = 0; // Untuk deteksi Ctrl+Shift alternative
    
    // For mouse double-click word selection
    time_t lastMouseClickTime = 0;
    int lastMouseClickPos = -1;
    bool isMousePressed = false; // Track apakah mouse sedang ditekan
    bool isWordSelectionMode = false; // Mode seleksi per kata dengan drag
    int wordSelectionAnchorStart = -1; // Anchor point saat double-click
    int wordSelectionAnchorEnd = -1;
    const int MOUSE_DOUBLE_CLICK_THRESHOLD = 500; // milliseconds
    
    // For double-click Ctrl+Arrow detection
    time_t lastCtrlArrowTime = 0;
    char lastCtrlArrowDir = 0; // 'C' for right, 'D' for left
    const int CTRL_ARROW_DOUBLE_CLICK_THRESHOLD = 500; // milliseconds
    
    while (!shouldReturn) {
        clearScreen();
        displayHeader();
        
        gotoxy(10, 5);
        setColor(33);
        printf("═══════════════════ TAMBAH PESAN BARU ═══════════════════");
        resetColor();
        
        if (!messageSent) {
            // Tampilkan area input dengan cursor (multi-line)
            const int INPUT_WIDTH = 55; // Lebar area input tetap
            const int INPUT_START_X = 10;
            const int INPUT_START_Y = 7;
            
            gotoxy(INPUT_START_X, INPUT_START_Y);
            printf("Masukkan pesan:");
            
            // Hitung jumlah baris yang diperlukan
            int totalChars = message.length() + 1; // +1 untuk cursor
            int numLines = 1;
            
            // Hitung baris berdasarkan width dan newline
            int charCount = 0;
            for (int i = 0; i < (int)message.length(); i++) {
                if (message[i] == '\n') {
                    numLines++;
                    charCount = 0;
                } else {
                    charCount++;
                    if (charCount >= INPUT_WIDTH) {
                        numLines++;
                        charCount = 0;
                    }
                }
            }
            // Tambah 1 baris untuk cursor jika di akhir
            if (charCount > 0 || message.empty()) {
                // Sudah termasuk di numLines
            }
            
            // Render multi-line text area
            int currentLine = 0;
            int currentCol = 0;
            int charIndex = 0;
            
            for (int line = 0; line < numLines && line < 20; line++) { // Max 20 baris
                gotoxy(INPUT_START_X, INPUT_START_Y + 1 + line);
                
                // Set background color
                if (focusArea == 0) {
                    setColor(47); setColor(30); // Highlight area input
                } else {
                    setColor(37);
                }
                
                // Render karakter per baris
                int renderedChars = 0;
                bool lineBreak = false;
                
                while (renderedChars < INPUT_WIDTH && charIndex <= (int)message.length()) {
                    // Render cursor SEBELUM karakter di posisi ini
                    if (charIndex == cursorPos && focusArea == 0 && 
                        (selectionStart == -1 || selectionEnd == -1 || selectionStart == selectionEnd)) {
                        printf("│");
                        renderedChars++;
                        if (renderedChars >= INPUT_WIDTH) break;
                    }
                    
                    // Cek apakah posisi ini terseleksi
                    bool isSelected = false;
                    if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
                        int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                        int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                        if (charIndex >= selStart && charIndex < selEnd) {
                            isSelected = true;
                        }
                    }
                    
                    // Set warna untuk seleksi
                    if (isSelected) {
                        resetColor();
                        setColor(40); // Background hitam
                        setColor(37); // Text putih
                    }
                    
                    // Render karakter
                    if (charIndex < (int)message.length()) {
                        char ch = message[charIndex];
                        if (ch == '\n') {
                            // Newline - isi sisa baris dengan spasi lalu break
                            while (renderedChars < INPUT_WIDTH) {
                                printf(" ");
                                renderedChars++;
                            }
                            charIndex++;
                            lineBreak = true;
                            break;
                        } else {
                            printf("%c", ch);
                            renderedChars++;
                            charIndex++;
                        }
                    } else {
                        // Sudah di akhir message, padding dengan spasi
                        printf(" ");
                        renderedChars++;
                        charIndex++;
                    }
                    
                    // Kembalikan warna jika habis seleksi
                    if (isSelected) {
                        resetColor();
                        if (focusArea == 0) {
                            setColor(47); setColor(30);
                        } else {
                            setColor(37);
                        }
                    }
                }
                
                // Padding sisa baris jika belum penuh
                while (renderedChars < INPUT_WIDTH) {
                    printf(" ");
                    renderedChars++;
                }
                
                resetColor();
                
                // Jika line break, lanjut ke baris berikutnya
                if (lineBreak) continue;
            }
            
            // Update posisi tombol berdasarkan jumlah baris
            int buttonY = INPUT_START_Y + 1 + numLines + 1;
            
            // Tombol Kirim dan Kembali
            gotoxy(10, buttonY);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("╔════════════════╗");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("╔════════════════════╗");
            resetColor();
            
            gotoxy(10, buttonY + 1);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("║  [✓ KIRIM]     ║");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("║  [← KEMBALI]       ║");
            resetColor();
            
            gotoxy(10, buttonY + 2);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("╚════════════════╝");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("╚════════════════════╝");
            resetColor();
            
            gotoxy(10, buttonY + 4);
            setColor(36);
            if (focusArea == 0) {
                printf("←→ kursor | Ctrl+←→ loncat kata | Shift+←→ blok | Alt+←→ blok kata");
            } else {
                printf("Tombol: ←→ pilih | Enter/Klik 2x konfirmasi | ↑ ke text");
            }
            resetColor();
            fflush(stdout);
            
            // Read input
            char c;
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == '\033') { // ESC sequence
                    char seq[10];
                    memset(seq, 0, sizeof(seq));
                    if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                        if (seq[0] == '[') {
                            if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                                // Check for extended sequences (Shift, Ctrl+Shift)
                                if (seq[1] == '1') {
                                    if (read(STDIN_FILENO, &seq[2], 1) == 1) {
                                        if (seq[2] == ';') {
                                            if (read(STDIN_FILENO, &seq[3], 1) == 1) {
                                                if (read(STDIN_FILENO, &seq[4], 1) == 1) {
                                                    // Modifier values:
                                                    // 2 = Shift
                                                    // 3 = Alt
                                                    // 4 = Shift+Alt
                                                    // 5 = Ctrl
                                                    // 6 = Ctrl+Shift
                                                    // 7 = Ctrl+Alt
                                                    // 8 = Ctrl+Shift+Alt
                                                    bool isShift = (seq[3] == '2');
                                                    bool isCtrl = (seq[3] == '5');
                                                    bool isCtrlShift = (seq[3] == '6');
                                                    bool isAlt = (seq[3] == '3');
                                                    
                                                    // Handle Alt+Arrow (word selection)
                                                    if (focusArea == 0 && isAlt) {
                                                        if (seq[4] == 'C') { // Alt+Right - extend selection word right
                                                            if (selectionStart == -1) {
                                                                // Mulai seleksi dari posisi saat ini
                                                                selectionStart = cursorPos;
                                                                selectionEnd = cursorPos;
                                                            }
                                                            
                                                            // Extend dari ujung seleksi (yang lebih besar)
                                                            int extendFrom = (selectionStart < selectionEnd) ? selectionEnd : selectionStart;
                                                            cursorPos = extendFrom;
                                                            
                                                            // Skip spasi jika ada di posisi saat ini
                                                            while (cursorPos < (int)message.length() && message[cursorPos] == ' ') {
                                                                cursorPos++;
                                                            }
                                                            
                                                            // Blok 1 kata (sampai ketemu spasi atau akhir)
                                                            while (cursorPos < (int)message.length() && message[cursorPos] != ' ') {
                                                                cursorPos++;
                                                            }
                                                            
                                                            // Update selectionEnd
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'D') { // Alt+Left - extend selection word left
                                                            if (selectionStart == -1) {
                                                                // Mulai seleksi dari posisi saat ini
                                                                selectionStart = cursorPos;
                                                                selectionEnd = cursorPos;
                                                            }
                                                            
                                                            // Extend dari ujung seleksi (yang lebih kecil)
                                                            int extendFrom = (selectionStart < selectionEnd) ? selectionStart : selectionEnd;
                                                            cursorPos = extendFrom;
                                                            
                                                            if (cursorPos > 0) {
                                                                cursorPos--;
                                                                
                                                                // Skip spasi jika ada di posisi saat ini
                                                                while (cursorPos > 0 && message[cursorPos] == ' ') {
                                                                    cursorPos--;
                                                                }
                                                                
                                                                // Blok 1 kata ke kiri (sampai ketemu spasi atau awal)
                                                                while (cursorPos > 0 && message[cursorPos - 1] != ' ') {
                                                                    cursorPos--;
                                                                }
                                                            }
                                                            
                                                            // Update selectionStart (untuk extend ke kiri)
                                                            selectionStart = cursorPos;
                                                        }
                                                    }
                                                    
                                                    // Handle Ctrl+Arrow (jump per word without selection)
                                                    if (focusArea == 0 && isCtrl && !isCtrlShift && !isShift) {
                                                        if (seq[4] == 'C') { // Ctrl+Right
                                                            // Jump per kata ke kanan (tanpa seleksi)
                                                            selectionStart = -1;
                                                            selectionEnd = -1;
                                                            
                                                            if (cursorPos < (int)message.length() && message[cursorPos] == ' ') {
                                                                while (cursorPos < (int)message.length() && message[cursorPos] == ' ') {
                                                                    cursorPos++;
                                                                }
                                                            } else {
                                                                while (cursorPos < (int)message.length() && message[cursorPos] != ' ') {
                                                                    cursorPos++;
                                                                }
                                                            }
                                                        } else if (seq[4] == 'D') { // Ctrl+Left
                                                            // Jump per kata ke kiri (tanpa seleksi)
                                                            selectionStart = -1;
                                                            selectionEnd = -1;
                                                            
                                                            if (cursorPos > 0) {
                                                                cursorPos--;
                                                                if (message[cursorPos] == ' ') {
                                                                    while (cursorPos > 0 && message[cursorPos] == ' ') {
                                                                        cursorPos--;
                                                                    }
                                                                    if (cursorPos > 0 || message[0] != ' ') {
                                                                        cursorPos++;
                                                                    }
                                                                } else {
                                                                    while (cursorPos > 0 && message[cursorPos - 1] != ' ') {
                                                                        cursorPos--;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    
                                                    if (focusArea == 0 && (isShift || isCtrlShift)) {
                                                        if (seq[4] == 'C') { // Shift+Right atau Ctrl+Shift+Right
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            
                                                            if (isCtrlShift) {
                                                                // Blok per kata ke kanan
                                                                // Jika di spasi, skip spasi sampai ketemu huruf
                                                                if (cursorPos < (int)message.length() && message[cursorPos] == ' ') {
                                                                    while (cursorPos < (int)message.length() && message[cursorPos] == ' ') {
                                                                        cursorPos++;
                                                                    }
                                                                } else {
                                                                    // Jika di huruf, maju sampai ketemu spasi atau akhir
                                                                    while (cursorPos < (int)message.length() && message[cursorPos] != ' ') {
                                                                        cursorPos++;
                                                                    }
                                                                }
                                                            } else {
                                                                // Geser 1 karakter
                                                                if (cursorPos < (int)message.length()) cursorPos++;
                                                            }
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'D') { // Shift+Left atau Ctrl+Shift+Left
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            
                                                            if (isCtrlShift) {
                                                                // Blok per kata ke kiri
                                                                // Mundur satu langkah dulu
                                                                if (cursorPos > 0) {
                                                                    cursorPos--;
                                                                    
                                                                    // Jika di spasi, skip spasi sampai ketemu huruf
                                                                    if (message[cursorPos] == ' ') {
                                                                        while (cursorPos > 0 && message[cursorPos] == ' ') {
                                                                            cursorPos--;
                                                                        }
                                                                        // Posisikan di akhir kata sebelumnya
                                                                        if (cursorPos > 0 || message[0] != ' ') {
                                                                            cursorPos++;
                                                                        }
                                                                    } else {
                                                                        // Jika di huruf, mundur sampai ketemu spasi atau awal
                                                                        while (cursorPos > 0 && message[cursorPos - 1] != ' ') {
                                                                            cursorPos--;
                                                                        }
                                                                    }
                                                                }
                                                            } else {
                                                                // Geser 1 karakter
                                                                if (cursorPos > 0) cursorPos--;
                                                            }
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'A') { // Shift+Up
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            // Geser ke atas 1 baris (55 karakter)
                                                            const int INPUT_WIDTH = 55;
                                                            if (cursorPos >= INPUT_WIDTH) {
                                                                cursorPos -= INPUT_WIDTH;
                                                            } else {
                                                                cursorPos = 0; // Ke awal text
                                                            }
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'B') { // Shift+Down
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            // Geser ke bawah 1 baris (55 karakter)
                                                            const int INPUT_WIDTH = 55;
                                                            if (cursorPos + INPUT_WIDTH <= (int)message.length()) {
                                                                cursorPos += INPUT_WIDTH;
                                                            } else {
                                                                cursorPos = message.length(); // Ke akhir text
                                                            }
                                                            selectionEnd = cursorPos;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                } else if (seq[1] == 'A') { // Up arrow
                                    if (focusArea == 1) {
                                        focusArea = 0; // Kembali ke text input
                                    } else if (focusArea == 0) {
                                        // Navigasi ke atas 1 baris dalam text
                                        const int INPUT_WIDTH = 55;
                                        
                                        // Hitung posisi baris saat ini berdasarkan newline
                                        int currentLine = 0;
                                        int charCount = 0;
                                        for (int i = 0; i < cursorPos && i < (int)message.length(); i++) {
                                            if (message[i] == '\n') {
                                                currentLine++;
                                                charCount = 0;
                                            } else {
                                                charCount++;
                                                if (charCount >= INPUT_WIDTH) {
                                                    currentLine++;
                                                    charCount = 0;
                                                }
                                            }
                                        }
                                        
                                        if (currentLine > 0) {
                                            // Ada baris di atas, naik
                                            int targetLine = currentLine - 1;
                                            int newPos = 0;
                                            int line = 0;
                                            charCount = 0;
                                            
                                            for (int i = 0; i <= (int)message.length(); i++) {
                                                if (line == targetLine && charCount >= INPUT_WIDTH) {
                                                    break;
                                                }
                                                if (line == targetLine + 1) {
                                                    break;
                                                }
                                                
                                                newPos = i;
                                                
                                                if (i < (int)message.length()) {
                                                    if (message[i] == '\n') {
                                                        line++;
                                                        charCount = 0;
                                                    } else {
                                                        charCount++;
                                                        if (charCount >= INPUT_WIDTH) {
                                                            line++;
                                                            charCount = 0;
                                                        }
                                                    }
                                                }
                                            }
                                            
                                            cursorPos = newPos;
                                        } else {
                                            cursorPos = 0; // Ke awal text
                                        }
                                        // Clear selection
                                        selectionStart = -1;
                                        selectionEnd = -1;
                                    }
                                } else if (seq[1] == 'B') { // Down arrow
                                    if (focusArea == 0) {
                                        // Hitung total baris dalam message
                                        int totalLines = 1; // Minimal 1 baris
                                        int charCount = 0;
                                        for (int i = 0; i < (int)message.length(); i++) {
                                            if (message[i] == '\n') {
                                                totalLines++;
                                                charCount = 0;
                                            } else {
                                                charCount++;
                                                if (charCount >= INPUT_WIDTH) {
                                                    totalLines++;
                                                    charCount = 0;
                                                }
                                            }
                                        }
                                        
                                        // Hitung baris saat ini
                                        int currentLine = 0;
                                        charCount = 0;
                                        for (int i = 0; i < cursorPos && i < (int)message.length(); i++) {
                                            if (message[i] == '\n') {
                                                currentLine++;
                                                charCount = 0;
                                            } else {
                                                charCount++;
                                                if (charCount >= INPUT_WIDTH) {
                                                    currentLine++;
                                                    charCount = 0;
                                                }
                                            }
                                        }
                                        
                                        // Cek apakah cursor di posisi setelah baris terakhir text
                                        bool atLastLine = (currentLine >= totalLines - 1);
                                        
                                        if (!atLastLine) {
                                            // Masih ada baris di bawah, geser ke bawah
                                            int targetLine = currentLine + 1;
                                            int newPos = 0;
                                            int line = 0;
                                            charCount = 0;
                                            
                                            for (int i = 0; i <= (int)message.length(); i++) {
                                                if (line == targetLine) {
                                                    newPos = i;
                                                    break;
                                                }
                                                
                                                if (i < (int)message.length()) {
                                                    if (message[i] == '\n') {
                                                        line++;
                                                        charCount = 0;
                                                    } else {
                                                        charCount++;
                                                        if (charCount >= INPUT_WIDTH) {
                                                            line++;
                                                            charCount = 0;
                                                        }
                                                    }
                                                } else {
                                                    newPos = message.length();
                                                }
                                            }
                                            
                                            cursorPos = newPos;
                                            // Clear selection
                                            selectionStart = -1;
                                            selectionEnd = -1;
                                        } else {
                                            // Sudah di baris terakhir, pindah ke buttons
                                            focusArea = 1;
                                        }
                                    }
                                } else if (seq[1] == 'C') { // Right arrow (tanpa modifier)
                                    if (focusArea == 0) {
                                        // Clear selection
                                        selectionStart = -1;
                                        selectionEnd = -1;
                                        // Geser cursor ke kanan
                                        if (cursorPos < (int)message.length()) cursorPos++;
                                    } else {
                                        // Di area button
                                        selectedButton = (selectedButton + 1) % 2;
                                    }
                                } else if (seq[1] == 'D') { // Left arrow (tanpa modifier)
                                    if (focusArea == 0) {
                                        // Clear selection
                                        selectionStart = -1;
                                        selectionEnd = -1;
                                        // Geser cursor ke kiri
                                        if (cursorPos > 0) cursorPos--;
                                    } else {
                                        // Di area button
                                        selectedButton = (selectedButton - 1 + 2) % 2;
                                    }
                                } else if (seq[1] == '<') { // Mouse input
                                    char mouseData[20];
                                    int idx = 0;
                                    char eventType;
                                    
                                    // Read button number
                                    int buttonNum = 0;
                                    char ch;
                                    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != ';') {
                                        if (ch >= '0' && ch <= '9') {
                                            buttonNum = buttonNum * 10 + (ch - '0');
                                        }
                                    }
                                    
                                    // Read rest of mouse data
                                    idx = 0;
                                    while (read(STDIN_FILENO, &mouseData[idx], 1) == 1 && mouseData[idx] != 'M' && mouseData[idx] != 'm') {
                                        idx++;
                                    }
                                    eventType = mouseData[idx];
                                    
                                    int mouseY = 0, mouseX = 0;
                                    sscanf(mouseData, "%d;%d", &mouseX, &mouseY);
                                    
                                    struct timespec ts;
                                    clock_gettime(CLOCK_MONOTONIC, &ts);
                                    time_t currentTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
                                    
                                    // Check if in text area
                                    if (mouseY >= INPUT_START_Y + 1 && mouseY < INPUT_START_Y + 1 + numLines && 
                                        mouseX >= INPUT_START_X && mouseX < INPUT_START_X + INPUT_WIDTH && focusArea == 0) {
                                        
                                        // Calculate clicked position in text
                                        int lineClicked = mouseY - (INPUT_START_Y + 1);
                                        int colClicked = mouseX - INPUT_START_X;
                                        
                                        // Find actual character position considering newlines
                                        int clickedPos = 0;
                                        int currentLine = 0;
                                        int currentCol = 0;
                                        
                                        for (int i = 0; i <= (int)message.length(); i++) {
                                            if (currentLine == lineClicked && currentCol == colClicked) {
                                                clickedPos = i;
                                                break;
                                            }
                                            
                                            if (i < (int)message.length()) {
                                                if (message[i] == '\n') {
                                                    currentLine++;
                                                    currentCol = 0;
                                                } else {
                                                    currentCol++;
                                                    if (currentCol >= INPUT_WIDTH) {
                                                        currentLine++;
                                                        currentCol = 0;
                                                    }
                                                }
                                            } else {
                                                clickedPos = message.length();
                                                break;
                                            }
                                        }
                                        
                                        if (eventType == 'M' && buttonNum == 0) { // Mouse button press
                                            isMousePressed = true;
                                            
                                            // Double-click detection for word selection
                                            if (lastMouseClickPos == clickedPos && 
                                                (currentTime - lastMouseClickTime) < MOUSE_DOUBLE_CLICK_THRESHOLD) {
                                                // Double click - select word at cursor
                                                isWordSelectionMode = true;
                                                
                                                // Find word boundaries
                                                int wordStart = clickedPos;
                                                int wordEnd = clickedPos;
                                                
                                                // Expand left to word start
                                                while (wordStart > 0 && message[wordStart - 1] != ' ' && message[wordStart - 1] != '\n') {
                                                    wordStart--;
                                                }
                                                
                                                // Expand right to word end
                                                while (wordEnd < (int)message.length() && message[wordEnd] != ' ' && message[wordEnd] != '\n') {
                                                    wordEnd++;
                                                }
                                                
                                                selectionStart = wordStart;
                                                selectionEnd = wordEnd;
                                                wordSelectionAnchorStart = wordStart;
                                                wordSelectionAnchorEnd = wordEnd;
                                                cursorPos = wordEnd;
                                                
                                                lastMouseClickPos = -1; // Reset untuk prevent triple-click
                                            } else {
                                                // Single click - move cursor
                                                isWordSelectionMode = false;
                                                wordSelectionAnchorStart = -1;
                                                wordSelectionAnchorEnd = -1;
                                                cursorPos = clickedPos;
                                                selectionStart = -1;
                                                selectionEnd = -1;
                                                
                                                lastMouseClickPos = clickedPos;
                                                lastMouseClickTime = currentTime;
                                            }
                                        } else if (eventType == 'm' && buttonNum == 0) { // Mouse release
                                            isMousePressed = false;
                                            isWordSelectionMode = false; // Exit word selection mode
                                        } else if (eventType == 'M' && buttonNum == 32) { // Mouse drag (button held)
                                            // Handle mouse drag for word selection mode
                                            if (isMousePressed && isWordSelectionMode && wordSelectionAnchorStart != -1) {
                                                // Extend selection word by word based on drag position
                                                
                                                // Determine direction from anchor
                                                if (clickedPos >= wordSelectionAnchorEnd) {
                                                    // Dragging right from original word - expand to word end
                                                    int newEnd = clickedPos;
                                                    while (newEnd < (int)message.length() && message[newEnd] != ' ' && message[newEnd] != '\n') {
                                                        newEnd++;
                                                    }
                                                    selectionStart = wordSelectionAnchorStart;
                                                    selectionEnd = newEnd;
                                                    cursorPos = newEnd;
                                                } else if (clickedPos <= wordSelectionAnchorStart) {
                                                    // Dragging left from original word - expand to word start
                                                    int newStart = clickedPos;
                                                    while (newStart > 0 && message[newStart - 1] != ' ' && message[newStart - 1] != '\n') {
                                                        newStart--;
                                                    }
                                                    selectionStart = newStart;
                                                    selectionEnd = wordSelectionAnchorEnd;
                                                    cursorPos = newStart;
                                                } else {
                                                    // Within original word - keep original selection
                                                    selectionStart = wordSelectionAnchorStart;
                                                    selectionEnd = wordSelectionAnchorEnd;
                                                    cursorPos = wordSelectionAnchorEnd;
                                                }
                                            }
                                        }
                                    }
                                    // Check if clicked on button area
                                    else if (eventType == 'M' && buttonNum == 0 && mouseY >= buttonY && mouseY <= buttonY + 2) {
                                        const int DOUBLE_CLICK_THRESHOLD = 500;
                                        
                                        // Check if clicked on Kirim button
                                        if (mouseX >= 10 && mouseX <= 28) {
                                            if (lastClickedButton == 1 && 
                                                (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                                // Send message
                                                if (!message.empty() && !isEmptyOrWhitespace(message)) {
                                                    bool success = queue.enqueue(message);
                                                    messageSent = true;
                                                    lastClickedButton = -1;
                                                    if (!success) {
                                                        message = "SPAM_ERROR";
                                                    }
                                                } else if (!message.empty() && isEmptyOrWhitespace(message)) {
                                                    message = "EMPTY_ERROR";
                                                    messageSent = true;
                                                    lastClickedButton = -1;
                                                }
                                            } else {
                                                lastClickedButton = 1;
                                                lastClickTime = currentTime;
                                            }
                                        }
                                        // Check if clicked on Kembali button
                                        else if (mouseX >= 30 && mouseX <= 52) {
                                            if (lastClickedButton == 2 && 
                                                (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                                shouldReturn = true;
                                            } else {
                                                lastClickedButton = 2;
                                                lastClickTime = currentTime;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (c == '\n' || c == '\r') { // Enter key
                    if (focusArea == 1) {
                        // Di area button
                        if (selectedButton == 0) {
                            // Kirim
                            if (!message.empty() && !isEmptyOrWhitespace(message)) {
                                bool success = queue.enqueue(message);
                                messageSent = true;
                                lastClickedButton = -1;
                                if (!success) {
                                    message = "SPAM_ERROR";
                                }
                            } else if (!message.empty() && isEmptyOrWhitespace(message)) {
                                message = "EMPTY_ERROR";
                                messageSent = true;
                                lastClickedButton = -1;
                            }
                        } else {
                            // Kembali
                            shouldReturn = true;
                        }
                    } else if (focusArea == 0) {
                        // Di text area, Enter membuat baris baru
                        if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
                            // Ada seleksi, replace dengan newline
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            message.erase(selStart, selEnd - selStart);
                            message.insert(selStart, 1, '\n');
                            cursorPos = selStart + 1;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Insert newline di posisi cursor
                            message.insert(cursorPos, 1, '\n');
                            cursorPos++;
                        }
                    }
                } else if (c == 127 || c == 8) { // Backspace
                    if (focusArea == 0) {
                        if (selectionStart != -1 && selectionEnd != -1) {
                            // Ada seleksi, hapus text yang diselect
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            message.erase(selStart, selEnd - selStart);
                            cursorPos = selStart;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Tidak ada seleksi, hapus 1 karakter sebelum cursor
                            if (cursorPos > 0) {
                                message.erase(cursorPos - 1, 1);
                                cursorPos--;
                            }
                        }
                    }
                } else if (c == 127 + 1) { // Delete key (extended)
                    if (focusArea == 0) {
                        if (selectionStart != -1 && selectionEnd != -1) {
                            // Ada seleksi, hapus text yang diselect
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            message.erase(selStart, selEnd - selStart);
                            cursorPos = selStart;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Tidak ada seleksi, hapus 1 karakter setelah cursor
                            if (cursorPos < (int)message.length()) {
                                message.erase(cursorPos, 1);
                            }
                        }
                    }
                } else if (c >= 32 && c <= 126) { // Printable characters
                    if (focusArea == 0) {
                        if (selectionStart != -1 && selectionEnd != -1) {
                            // Ada seleksi, replace dengan karakter baru
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            message.erase(selStart, selEnd - selStart);
                            message.insert(selStart, 1, c);
                            cursorPos = selStart + 1;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Insert karakter di posisi cursor
                            message.insert(cursorPos, 1, c);
                            cursorPos++;
                        }
                    }
                }
            }
        } else {
            // Message sent confirmation
            if (message == "SPAM_ERROR") {
                gotoxy(10, 7);
                setColor(31);
                printf("✗ Jangan spam! Pesan sama dengan pesan terakhir.");
                resetColor();
            } else if (message == "EMPTY_ERROR") {
                gotoxy(10, 7);
                setColor(31);
                printf("✗ Pesan tidak boleh kosong atau hanya berisi spasi/enter!");
                resetColor();
            } else {
                // Display multi-line message properly
                int startPos = 0;
                bool firstLine = true;
                int displayY = 7;
                
                for (int i = 0; i <= (int)message.length(); i++) {
                    if (i == (int)message.length() || message[i] == '\n') {
                        gotoxy(10, displayY);
                        
                        if (firstLine) {
                            setColor(32);
                            printf("√ Pesan berhasil dikirim: ");
                        } else {
                            printf("                          "); // Sesuai panjang string di atas
                        }
                        
                        // Print substring
                        string line = message.substr(startPos, i - startPos);
                        setColor(32);
                        printf("%s", line.c_str());
                        resetColor();
                        
                        displayY++;
                        startPos = i + 1;
                        firstLine = false;
                    }
                }
            }
            
            shouldReturn = displayBackButton(9, lastClickedButton, lastClickTime);
        }
    }
}

// Fungsi untuk hapus pesan pertama
void deleteFirstMessage(QueuePesan& queue) {
    int lastClickedButton = -1;
    time_t lastClickTime = 0;
    bool shouldReturn = false;
    bool alreadyDeleted = false;
    bool deleteSuccess = false;
    
    while (!shouldReturn) {
        clearScreen();
        displayHeader();
        
        gotoxy(10, 5);
        setColor(33);
        printf("═══════════════════ HAPUS PESAN PERTAMA ═══════════════════");
        resetColor();
        
        // Hapus pesan hanya sekali
        if (!alreadyDeleted) {
            deleteSuccess = queue.dequeue();
            alreadyDeleted = true;
        }
        
        gotoxy(10, 7);
        if (deleteSuccess) {
            setColor(32);
            printf("✓ Pesan pertama berhasil dihapus!");
        } else {
            setColor(31);
            printf("✗ Tidak ada pesan untuk dihapus!");
        }
        resetColor();
        
        shouldReturn = displayBackButton(9, lastClickedButton, lastClickTime);
    }
}

// Fungsi untuk batalkan pesan terakhir
void cancelLastMessage(QueuePesan& queue) {
    int lastClickedButton = -1;
    time_t lastClickTime = 0;
    bool shouldReturn = false;
    bool alreadyCancelled = false;
    bool cancelSuccess = false;
    
    while (!shouldReturn) {
        clearScreen();
        displayHeader();
        
        gotoxy(10, 5);
        setColor(33);
        printf("═══════════════════ BATALKAN PESAN TERAKHIR ═══════════════════");
        resetColor();
        
        // Batalkan pesan hanya sekali
        if (!alreadyCancelled) {
            cancelSuccess = queue.cancelLast();
            alreadyCancelled = true;
        }
        
        gotoxy(10, 7);
        if (cancelSuccess) {
            setColor(32);
            printf("✓ Pesan terakhir berhasil dibatalkan!");
        } else {
            setColor(31);
            printf("✗ Tidak ada pesan untuk dibatalkan!");
        }
        resetColor();
        
        shouldReturn = displayBackButton(9, lastClickedButton, lastClickTime);
    }
}

// Fungsi untuk tandai pesan sebagai dibaca
void markMessageAsRead(QueuePesan& queue) {
    int lastClickedButton = -1;
    time_t lastClickTime = 0;
    bool shouldReturn = false;
    string input = "";
    bool marked = false;
    int selectedButton = 0; // 0 = Tandai, 1 = Kembali
    int focusArea = 0; // 0 = text input, 1 = buttons
    int cursorPos = 0; // Posisi cursor dalam text
    int selectionStart = -1; // -1 berarti tidak ada seleksi
    int selectionEnd = -1;
    int markedCount = 0; // Jumlah pesan yang ditandai
    
    // For mouse double-click word selection
    time_t lastMouseClickTime = 0;
    int lastMouseClickPos = -1;
    bool isMousePressed = false;
    bool isWordSelectionMode = false;
    int wordSelectionAnchorStart = -1;
    int wordSelectionAnchorEnd = -1;
    const int MOUSE_DOUBLE_CLICK_THRESHOLD = 500;
    
    while (!shouldReturn) {
        clearScreen();
        displayHeader();
        
        gotoxy(10, 5);
        setColor(33);
        printf("═══════════════════ TANDAI PESAN SEBAGAI DIBACA ═══════════════════");
        resetColor();
        
        if (queue.isEmpty()) {
            gotoxy(10, 7);
            setColor(31);
            printf("Tidak ada pesan.");
            resetColor();
            
            shouldReturn = displayBackButton(9, lastClickedButton, lastClickTime);
        } else if (!marked) {
            // Tampilkan daftar pesan
            int y = 7;
            gotoxy(10, y);
            setColor(36);
            printf("── Daftar Pesan ──");
            resetColor();
            y += 2;
            
            Node* current = queue.getFront();
            int index = 0;
            
            while (current != nullptr && index < 10) { // Batasi 10 pesan agar tidak terlalu panjang
                // Render pesan dengan multi-line support
                string pesan = current->data.text;
                bool isRead = current->data.isRead;
                
                // Split pesan berdasarkan newline
                int startPos = 0;
                bool firstLine = true;
                
                for (int i = 0; i <= (int)pesan.length(); i++) {
                    if (i == (int)pesan.length() || pesan[i] == '\n') {
                        gotoxy(10, y);
                        
                        if (firstLine) {
                            // Baris pertama dengan indikator
                            if (isRead) {
                                setColor(37); // Abu-abu (sudah dibaca)
                                printf("[√] %d. ", index + 1);
                            } else {
                                setColor(32); // Hijau (belum dibaca)
                                printf("[-] %d. ", index + 1);
                            }
                        } else {
                            // Baris selanjutnya dengan indentasi  
                            char indent[50];
                            snprintf(indent, sizeof(indent), "[%c] %d. ", ' ', index + 1);
                            for (int j = 0; j < (int)strlen(indent); j++) {
                                printf(" ");
                            }
                        }
                        
                        // Print substring
                        string line = pesan.substr(startPos, i - startPos);
                        if (isRead) {
                            setColor(37);
                        } else {
                            setColor(32);
                        }
                        printf("%s", line.c_str());
                        resetColor();
                        
                        y++;
                        startPos = i + 1;
                        firstLine = false;
                    }
                }
                
                current = current->next;
                index++;
            }
            
            if (queue.getSize() > 10) {
                gotoxy(10, y);
                setColor(37);
                printf("... dan %d pesan lainnya", queue.getSize() - 10);
                resetColor();
                y++;
            }
            
            y += 2;
            
            // Input field dengan text editor
            const int INPUT_WIDTH = 30; // Lebar input
            const int INPUT_START_X = 10;
            const int INPUT_START_Y = y;
            
            gotoxy(INPUT_START_X, INPUT_START_Y);
            printf("Masukkan (1-%d / 1,3 / 1-3 / all): ", queue.getSize());
            
            // Render input area dengan cursor
            gotoxy(INPUT_START_X, INPUT_START_Y + 1);
            
            // Set background color
            if (focusArea == 0) {
                setColor(47); setColor(30); // Highlight area input
            } else {
                setColor(37);
            }
            
            // Render text dengan cursor dan selection
            for (int i = 0; i < INPUT_WIDTH; i++) {
                // Render cursor
                if (i == cursorPos && focusArea == 0 && 
                    (selectionStart == -1 || selectionEnd == -1 || selectionStart == selectionEnd)) {
                    printf("│");
                    if (i < (int)input.length()) i++; // Skip character jika ada cursor
                    if (i >= INPUT_WIDTH) break;
                }
                
                // Cek selection
                bool isSelected = false;
                if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
                    int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                    int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                    if (i >= selStart && i < selEnd) {
                        isSelected = true;
                    }
                }
                
                // Set warna untuk selection
                if (isSelected) {
                    resetColor();
                    setColor(40); setColor(37);
                }
                
                // Render character
                if (i < (int)input.length()) {
                    printf("%c", input[i]);
                } else {
                    printf(" ");
                }
                
                // Reset warna setelah selection
                if (isSelected) {
                    resetColor();
                    if (focusArea == 0) {
                        setColor(47); setColor(30);
                    } else {
                        setColor(37);
                    }
                }
            }
            resetColor();
            
            y += 3;
            
            // Tombol Tandai dan Kembali
            int buttonY = y + 1;
            gotoxy(10, buttonY);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("╔════════════════╗");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("╔════════════════════╗");
            resetColor();
            
            gotoxy(10, buttonY + 1);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("║  [✓ TANDAI]    ║");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("║  [← KEMBALI]       ║");
            resetColor();
            
            gotoxy(10, buttonY + 2);
            if (focusArea == 1 && selectedButton == 0) {
                setColor(47); setColor(30);
            }
            printf("╚════════════════╝");
            resetColor();
            printf("  ");
            if (focusArea == 1 && selectedButton == 1) {
                setColor(47); setColor(30);
            }
            printf("╚════════════════════╝");
            resetColor();
            
            gotoxy(10, buttonY + 4);
            setColor(36);
            if (focusArea == 0) {
                printf("←→ kursor | Ctrl+←→ loncat | Shift+←→ blok | Alt+←→ blok kata | ↓ tombol");
            } else {
                printf("←→ pilih | Enter/Klik 2x | ↑ input");
            }
            resetColor();
            fflush(stdout);
            
            // Read input
            char c;
            if (read(STDIN_FILENO, &c, 1) == 1) {
                if (c == '\033') { // ESC sequence
                    char seq[10];
                    memset(seq, 0, sizeof(seq));
                    if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                        if (seq[0] == '[') {
                            if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                                // Check for extended sequences (Shift, Ctrl+Shift, Alt)
                                if (seq[1] == '1') {
                                    if (read(STDIN_FILENO, &seq[2], 1) == 1) {
                                        if (seq[2] == ';') {
                                            if (read(STDIN_FILENO, &seq[3], 1) == 1) {
                                                if (read(STDIN_FILENO, &seq[4], 1) == 1) {
                                                    bool isShift = (seq[3] == '2');
                                                    bool isCtrl = (seq[3] == '5');
                                                    bool isCtrlShift = (seq[3] == '6');
                                                    bool isAlt = (seq[3] == '3');
                                                    
                                                    // Handle Alt+Arrow (word selection)
                                                    if (focusArea == 0 && isAlt) {
                                                        if (seq[4] == 'C') { // Alt+Right
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                                selectionEnd = cursorPos;
                                                            }
                                                            int extendFrom = (selectionStart < selectionEnd) ? selectionEnd : selectionStart;
                                                            cursorPos = extendFrom;
                                                            while (cursorPos < (int)input.length() && input[cursorPos] == ' ') {
                                                                cursorPos++;
                                                            }
                                                            while (cursorPos < (int)input.length() && input[cursorPos] != ' ') {
                                                                cursorPos++;
                                                            }
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'D') { // Alt+Left
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                                selectionEnd = cursorPos;
                                                            }
                                                            int extendFrom = (selectionStart < selectionEnd) ? selectionStart : selectionEnd;
                                                            cursorPos = extendFrom;
                                                            if (cursorPos > 0) {
                                                                cursorPos--;
                                                                while (cursorPos > 0 && input[cursorPos] == ' ') {
                                                                    cursorPos--;
                                                                }
                                                                while (cursorPos > 0 && input[cursorPos - 1] != ' ') {
                                                                    cursorPos--;
                                                                }
                                                            }
                                                            selectionStart = cursorPos;
                                                        }
                                                    }
                                                    
                                                    // Handle Ctrl+Arrow (jump per word without selection)
                                                    if (focusArea == 0 && isCtrl && !isCtrlShift && !isShift) {
                                                        if (seq[4] == 'C') { // Ctrl+Right
                                                            selectionStart = -1;
                                                            selectionEnd = -1;
                                                            if (cursorPos < (int)input.length() && input[cursorPos] == ' ') {
                                                                while (cursorPos < (int)input.length() && input[cursorPos] == ' ') {
                                                                    cursorPos++;
                                                                }
                                                            } else {
                                                                while (cursorPos < (int)input.length() && input[cursorPos] != ' ') {
                                                                    cursorPos++;
                                                                }
                                                            }
                                                        } else if (seq[4] == 'D') { // Ctrl+Left
                                                            selectionStart = -1;
                                                            selectionEnd = -1;
                                                            if (cursorPos > 0) {
                                                                cursorPos--;
                                                                if (input[cursorPos] == ' ') {
                                                                    while (cursorPos > 0 && input[cursorPos] == ' ') {
                                                                        cursorPos--;
                                                                    }
                                                                    if (cursorPos > 0 || input[0] != ' ') {
                                                                        cursorPos++;
                                                                    }
                                                                } else {
                                                                    while (cursorPos > 0 && input[cursorPos - 1] != ' ') {
                                                                        cursorPos--;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                    }
                                                    
                                                    if (focusArea == 0 && isShift) {
                                                        if (seq[4] == 'C') { // Shift+Right
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            if (cursorPos < (int)input.length()) cursorPos++;
                                                            selectionEnd = cursorPos;
                                                        } else if (seq[4] == 'D') { // Shift+Left
                                                            if (selectionStart == -1) {
                                                                selectionStart = cursorPos;
                                                            }
                                                            if (cursorPos > 0) cursorPos--;
                                                            selectionEnd = cursorPos;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                } else if (seq[1] == 'A') { // Up arrow
                                    if (focusArea == 1) {
                                        focusArea = 0; // Kembali ke text input
                                    }
                                } else if (seq[1] == 'B') { // Down arrow
                                    if (focusArea == 0) {
                                        focusArea = 1; // Ke buttons
                                    }
                                } else if (seq[1] == 'C') { // Right arrow (tanpa modifier)
                                    if (focusArea == 0) {
                                        selectionStart = -1;
                                        selectionEnd = -1;
                                        if (cursorPos < (int)input.length()) cursorPos++;
                                    } else {
                                        selectedButton = (selectedButton + 1) % 2;
                                    }
                                } else if (seq[1] == 'D') { // Left arrow (tanpa modifier)
                                    if (focusArea == 0) {
                                        selectionStart = -1;
                                        selectionEnd = -1;
                                        if (cursorPos > 0) cursorPos--;
                                    } else {
                                        selectedButton = (selectedButton - 1 + 2) % 2;
                                    }
                                } else if (seq[1] == '<') { // Mouse input
                                    char mouseData[20];
                                    int idx = 0;
                                    char eventType;
                                    
                                    // Read button number
                                    int buttonNum = 0;
                                    char ch;
                                    while (read(STDIN_FILENO, &ch, 1) == 1 && ch != ';') {
                                        if (ch >= '0' && ch <= '9') {
                                            buttonNum = buttonNum * 10 + (ch - '0');
                                        }
                                    }
                                    
                                    // Read rest of mouse data
                                    idx = 0;
                                    while (read(STDIN_FILENO, &mouseData[idx], 1) == 1 && mouseData[idx] != 'M' && mouseData[idx] != 'm') {
                                        idx++;
                                    }
                                    eventType = mouseData[idx];
                                    
                                    int mouseY = 0, mouseX = 0;
                                    sscanf(mouseData, "%d;%d", &mouseX, &mouseY);
                                    
                                    struct timespec ts;
                                    clock_gettime(CLOCK_MONOTONIC, &ts);
                                    time_t currentTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
                                    
                                    const int INPUT_START_X = 10;
                                    const int INPUT_START_Y = y - 3;
                                    const int INPUT_WIDTH = 30;
                                    
                                    // Check if in text area
                                    if (mouseY == INPUT_START_Y + 1 && 
                                        mouseX >= INPUT_START_X && mouseX < INPUT_START_X + INPUT_WIDTH && focusArea == 0) {
                                        
                                        int clickedPos = mouseX - INPUT_START_X;
                                        if (clickedPos > (int)input.length()) clickedPos = input.length();
                                        
                                        if (eventType == 'M' && buttonNum == 0) { // Mouse button press
                                            isMousePressed = true;
                                            
                                            // Double-click detection for word selection
                                            if (lastMouseClickPos == clickedPos && 
                                                (currentTime - lastMouseClickTime) < MOUSE_DOUBLE_CLICK_THRESHOLD) {
                                                // Double click - select word at cursor
                                                isWordSelectionMode = true;
                                                
                                                // Find word boundaries
                                                int wordStart = clickedPos;
                                                int wordEnd = clickedPos;
                                                
                                                // Expand left to word start
                                                while (wordStart > 0 && input[wordStart - 1] != ' ') {
                                                    wordStart--;
                                                }
                                                
                                                // Expand right to word end
                                                while (wordEnd < (int)input.length() && input[wordEnd] != ' ') {
                                                    wordEnd++;
                                                }
                                                
                                                selectionStart = wordStart;
                                                selectionEnd = wordEnd;
                                                wordSelectionAnchorStart = wordStart;
                                                wordSelectionAnchorEnd = wordEnd;
                                                cursorPos = wordEnd;
                                                
                                                lastMouseClickPos = -1; // Reset untuk prevent triple-click
                                            } else {
                                                // Single click - move cursor
                                                isWordSelectionMode = false;
                                                wordSelectionAnchorStart = -1;
                                                wordSelectionAnchorEnd = -1;
                                                cursorPos = clickedPos;
                                                selectionStart = -1;
                                                selectionEnd = -1;
                                                
                                                lastMouseClickPos = clickedPos;
                                                lastMouseClickTime = currentTime;
                                            }
                                        } else if (eventType == 'm' && buttonNum == 0) { // Mouse release
                                            isMousePressed = false;
                                            isWordSelectionMode = false;
                                        } else if (eventType == 'M' && buttonNum == 32) { // Mouse drag
                                            if (isMousePressed && isWordSelectionMode && wordSelectionAnchorStart != -1) {
                                                // Extend selection word by word based on drag position
                                                if (clickedPos >= wordSelectionAnchorEnd) {
                                                    // Dragging right from original word - expand to word end
                                                    int newEnd = clickedPos;
                                                    while (newEnd < (int)input.length() && input[newEnd] != ' ') {
                                                        newEnd++;
                                                    }
                                                    selectionStart = wordSelectionAnchorStart;
                                                    selectionEnd = newEnd;
                                                    cursorPos = newEnd;
                                                } else if (clickedPos <= wordSelectionAnchorStart) {
                                                    // Dragging left from original word - expand to word start
                                                    int newStart = clickedPos;
                                                    while (newStart > 0 && input[newStart - 1] != ' ') {
                                                        newStart--;
                                                    }
                                                    selectionStart = newStart;
                                                    selectionEnd = wordSelectionAnchorEnd;
                                                    cursorPos = newStart;
                                                } else {
                                                    // Within original word - keep original selection
                                                    selectionStart = wordSelectionAnchorStart;
                                                    selectionEnd = wordSelectionAnchorEnd;
                                                    cursorPos = wordSelectionAnchorEnd;
                                                }
                                            }
                                        }
                                    }
                                    // Check if clicked on button area
                                    else if (eventType == 'M' && buttonNum == 0 && mouseY >= buttonY && mouseY <= buttonY + 2) {
                                        const int DOUBLE_CLICK_THRESHOLD = 500;
                                        
                                        // Check if clicked on Tandai button
                                        if (mouseY >= buttonY && mouseY <= buttonY + 2 && mouseX >= 10 && mouseX <= 28) {
                                            if (lastClickedButton == 1 && 
                                                (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                                // Mark as read
                                                vector<int> indices = parseMessageNumbers(input, queue.getSize());
                                                if (!indices.empty()) {
                                                    markedCount = indices.size();
                                                    for (int i = 0; i < (int)indices.size(); i++) {
                                                        queue.markAsRead(indices[i]);
                                                    }
                                                    marked = true;
                                                    lastClickedButton = -1;
                                                } else {
                                                    input = "INVALID";
                                                    marked = true;
                                                    lastClickedButton = -1;
                                                }
                                            } else {
                                                lastClickedButton = 1;
                                                lastClickTime = currentTime;
                                            }
                                        }
                                        // Check if clicked on Kembali button
                                        else if (mouseY >= buttonY && mouseY <= buttonY + 2 && mouseX >= 30 && mouseX <= 52) {
                                            if (lastClickedButton == 2 && 
                                                (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                                shouldReturn = true;
                                            } else {
                                                lastClickedButton = 2;
                                                lastClickTime = currentTime;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                } else if (c == '\n' || c == '\r') { // Enter key
                    if (focusArea == 1) {
                        // Di area button
                        if (selectedButton == 0) {
                            // Tandai
                            vector<int> indices = parseMessageNumbers(input, queue.getSize());
                            if (!indices.empty()) {
                                markedCount = indices.size();
                                for (int i = 0; i < (int)indices.size(); i++) {
                                    queue.markAsRead(indices[i]);
                                }
                                marked = true;
                            } else {
                                input = "INVALID";
                                marked = true;
                            }
                        } else {
                            // Kembali
                            shouldReturn = true;
                        }
                    }
                } else if (c == 127 || c == 8) { // Backspace
                    if (focusArea == 0) {
                        if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
                            // Ada seleksi, hapus text yang diselect
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            input.erase(selStart, selEnd - selStart);
                            cursorPos = selStart;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Tidak ada seleksi, hapus 1 karakter sebelum cursor
                            if (cursorPos > 0) {
                                input.erase(cursorPos - 1, 1);
                                cursorPos--;
                            }
                        }
                    }
                } else if (c >= 32 && c <= 126) { // Printable characters
                    if (focusArea == 0) {
                        if (selectionStart != -1 && selectionEnd != -1 && selectionStart != selectionEnd) {
                            // Ada seleksi, replace dengan karakter baru
                            int selStart = selectionStart < selectionEnd ? selectionStart : selectionEnd;
                            int selEnd = selectionStart < selectionEnd ? selectionEnd : selectionStart;
                            input.erase(selStart, selEnd - selStart);
                            input.insert(selStart, 1, c);
                            cursorPos = selStart + 1;
                            selectionStart = -1;
                            selectionEnd = -1;
                        } else {
                            // Insert karakter di posisi cursor
                            input.insert(cursorPos, 1, c);
                            cursorPos++;
                        }
                    }
                }
            }
        } else {
            // Confirmation
            gotoxy(10, 7);
            if (input == "INVALID") {
                setColor(31);
                printf("✗ Input tidak valid! Format: 1 atau 1,3 atau 1-3 atau all");
            } else {
                setColor(32);
                if (markedCount == queue.getSize()) {
                    printf("✓ Semua %d pesan berhasil ditandai sebagai dibaca!", markedCount);
                } else if (markedCount > 1) {
                    printf("✓ %d pesan berhasil ditandai sebagai dibaca!", markedCount);
                } else {
                    printf("✓ 1 pesan berhasil ditandai sebagai dibaca!");
                }
            }
            resetColor();
            
            shouldReturn = displayBackButton(9, lastClickedButton, lastClickTime);
        }
    }
}

// Main function
int main() {
    QueuePesan queue;
    int selectedMenu = 0;
    bool running = true;
    
    // Variables for double-click detection
    int lastClickedMenu = -1;
    time_t lastClickTime = 0;
    const int DOUBLE_CLICK_THRESHOLD = 500; // milliseconds
    
    enableRawMode();
    enableMouseTracking();
    
    while (running) {
        clearScreen();
        displayHeader();
        displayMenu(selectedMenu);
        
        // Read input
        char c;
        if (read(STDIN_FILENO, &c, 1) == 1) {
            if (c == '\033') { // ESC sequence
                char seq[5];
                if (read(STDIN_FILENO, &seq[0], 1) == 1) {
                    if (seq[0] == '[') {
                        if (read(STDIN_FILENO, &seq[1], 1) == 1) {
                            // Arrow keys
                            if (seq[1] == 'A') { // Up arrow
                                selectedMenu = (selectedMenu - 1 + 6) % 6;
                            } else if (seq[1] == 'B') { // Down arrow
                                selectedMenu = (selectedMenu + 1) % 6;
                            } else if (seq[1] == '<') { // Mouse input
                                char mouseData[20];
                                int idx = 0;
                                while (read(STDIN_FILENO, &mouseData[idx], 1) == 1 && mouseData[idx] != 'M' && mouseData[idx] != 'm') {
                                    idx++;
                                }
                                
                                // Parse mouse coordinates (simplified)
                                // Format: <0;x;yM or <0;x;ym
                                if (mouseData[idx] == 'M') { // Mouse click
                                    // Extract y coordinate roughly
                                    int mouseY = 0;
                                    sscanf(mouseData, "%*d;%*d;%d", &mouseY);
                                    
                                    // Map mouse Y to menu item (menu starts at Y=6, each item is 1 line)
                                    if (mouseY >= 6 && mouseY <= 11) {
                                        int clickedMenu = mouseY - 6;
                                        selectedMenu = clickedMenu;
                                        
                                        // Double-click detection
                                        struct timespec ts;
                                        clock_gettime(CLOCK_MONOTONIC, &ts);
                                        time_t currentTime = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
                                        
                                        if (clickedMenu == lastClickedMenu && 
                                            (currentTime - lastClickTime) < DOUBLE_CLICK_THRESHOLD) {
                                            // Double click detected - execute menu action
                                            switch (selectedMenu) {
                                                case 0: // Tambah Pesan
                                                    inputNewMessage(queue);
                                                    break;
                                                case 1: // Hapus Pesan Pertama
                                                    deleteFirstMessage(queue);
                                                    break;
                                                case 2: // Batalkan Pesan Terakhir
                                                    cancelLastMessage(queue);
                                                    break;
                                                case 3: // Lihat Semua Pesan
                                                    displayMessages(queue);
                                                    break;
                                                case 4: // Tandai Sebagai Dibaca
                                                    markMessageAsRead(queue);
                                                    break;
                                                case 5: // Keluar
                                                    running = false;
                                                    break;
                                            }
                                            
                                            lastClickedMenu = -1; // Reset after executing
                                        } else {
                                            // Single click - just update selection
                                            lastClickedMenu = clickedMenu;
                                            lastClickTime = currentTime;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (c == '\n' || c == '\r') { // Enter key
                // Execute selected menu
                switch (selectedMenu) {
                    case 0: // Tambah Pesan
                        inputNewMessage(queue);
                        break;
                    case 1: // Hapus Pesan Pertama
                        deleteFirstMessage(queue);
                        break;
                    case 2: // Batalkan Pesan Terakhir
                        cancelLastMessage(queue);
                        break;
                    case 3: // Lihat Semua Pesan
                        displayMessages(queue);
                        break;
                    case 4: // Tandai Sebagai Dibaca
                        markMessageAsRead(queue);
                        break;
                    case 5: // Keluar
                        running = false;
                        break;
                }
            }
        }
    }
    
    disableMouseTracking();
    disableRawMode();
    
    clearScreen();
    gotoxy(1, 1);
    setColor(32);
    printf("Terima kasih telah menggunakan WhatsApp Simulator!\n");
    resetColor();
    
    return 0;
}
