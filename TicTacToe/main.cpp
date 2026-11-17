#include <iostream>
#include <limits>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

const int WINNING_COMBINATIONS[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6}             // Diagonals
};

enum GameState { IN_PROGRESS, PLAYER_1_WON, PLAYER_2_WON, DRAW };


void clearTerminal() {
    std::cout << "\033[2J\033[H" << std::flush;
}

// ANSI Color & Style Codes
const std::string RED    = "\033[1;91m"; // Bright Bold Red (X)
const std::string BLUE   = "\033[1;96m"; // Bright Bold Cyan/Blue (O)
const std::string GREEN  = "\033[1;92m"; // Bright Bold Green (Winning 3-in-a-row)
const std::string YELLOW = "\033[1;93m"; // Bright Bold Yellow (Banners / Draws)
const std::string GRAY   = "\033[90m";   // Dim Gray
const std::string WHITE  = "\033[37m";
const std::string RESET  = "\033[0m";    // Reset to default

void printBanner() {
    std::cout << YELLOW
              << "╔═══════════════════════════╗\n"
              << "║        TIC-TAC-TOE        ║\n"
              << "╚═══════════════════════════╝\n"
              << RESET;
}

// Returns 1 of the 3 horizontal slices (line 0, 1, or 2) for a single cell
std::string getCellLine(const char board[9], int index, int line, int winCombo = -1) {
    bool isWin = (winCombo != -1) &&
                 (WINNING_COMBINATIONS[winCombo][0] == index ||
                  WINNING_COMBINATIONS[winCombo][1] == index ||
                  WINNING_COMBINATIONS[winCombo][2] == index);

    char c = board[index];

    if (c == 'X') {
        std::string color = isWin ? GREEN : RED;
        if (line == 0) return color + "  \\ /  " + RESET;
        if (line == 1) return color + "   ╳   " + RESET;
        if (line == 2) return color + "  / \\  " + RESET;
    } else if (c == 'O') {
        std::string color = isWin ? GREEN : BLUE;
        if (line == 0) return color + "  ╭─╮  " + RESET;
        if (line == 1) return color + "  │ │  " + RESET;
        if (line == 2) return color + "  ╰─╯  " + RESET;
    } else {
        // Empty numbered slot: blank on top/bottom, centered number in the middle
        if (line == 1) return GRAY + "   " + std::string(1, c) + "   " + RESET;
        return "       "; // 7 spaces
    }
    return "       ";
}

// Renders the classic open '#' grid with 3-line-tall cells
void printBoard(const char board[9], int winCombo = -1) {
    // Ordered top-to-bottom to match numpad layout
    const int rows[3][3] = {
        {6, 7, 8},
        {3, 4, 5},
        {0, 1, 2}
    };

    std::cout << "\n";
    for (int r = 0; r < 3; ++r) {
        // Print all 3 vertical slices (0 = top, 1 = middle, 2 = bottom) for this row
        for (int line = 0; line < 3; ++line) {
            std::cout << "    "
                      << getCellLine(board, rows[r][0], line, winCombo) << WHITE << "│" << RESET
                      << getCellLine(board, rows[r][1], line, winCombo) << WHITE << "│" << RESET
                      << getCellLine(board, rows[r][2], line, winCombo) << "\n";
        }
        // Print horizontal divider between board rows
        if (r < 2) {
            std::cout << WHITE << "    ───────┼───────┼───────\n" << RESET;
        }
    }
    std::cout << "\n";
}


// Checks game status and records which combination won via winCombo
GameState check_game_over(const char board[9], int& winCombo) {
    winCombo = -1;

    // 1. Check for a winner
    for (int i = 0; i < 8; ++i) {
        int a = WINNING_COMBINATIONS[i][0];
        int b = WINNING_COMBINATIONS[i][1];
        int c = WINNING_COMBINATIONS[i][2];

        if ((board[a] == 'X' || board[a] == 'O') && board[a] == board[b] && board[a] == board[c]) {
            winCombo = i; // Save the winning line index (0-7)
            return (board[a] == 'X') ? PLAYER_1_WON : PLAYER_2_WON;
        }
    }

    // 2. Check for a draw
    for (int i = 0; i < 9; ++i) {
        if (board[i] != 'X' && board[i] != 'O') {
            return IN_PROGRESS;
        }
    }

    return DRAW;
}


void playBot(char board[9]) {
    std::cout << "\n  [1-Player Bot mode is not implemented yet!]\n";
}

void play2(char board[9]) {
    int turn = 1;
    int winCombo = -1;
    GameState state = IN_PROGRESS;

    while ((state = check_game_over(board, winCombo)) == IN_PROGRESS) {
        clearTerminal();
        printBanner();

        std::string playerTag = (turn == 1) ? (RED + "X" + RESET) : (BLUE + "O" + RESET);
        std::cout << "  Player " << turn << "'s Turn (" << playerTag << ")\n";

        printBoard(board);

        int move;
        std::cout << "  Enter your move (0-8): ";

        while (!(std::cin >> move) || move > 8 || move < 0 || board[move] == 'X' || board[move] == 'O') {
            std::cout << RED << "  Invalid move! " << RESET << "Try again (0-8): ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        board[move] = (turn == 1) ? 'X' : 'O';
        turn = (turn == 1) ? 2 : 1;
    }

    // Final board render with the winning 3-in-a-row highlighted in green
    clearTerminal();
    printBanner();
    printBoard(board, winCombo);

    if (state == DRAW) {
        std::cout << YELLOW << "  === It's a Draw! ===" << RESET << "\n";
    } else if (state == PLAYER_1_WON) {
        std::cout << GREEN << "  🎉 Player 1 (" << RED << "X" << GREEN << ") Wins! 🎉" << RESET << "\n";
    } else if (state == PLAYER_2_WON) {
        std::cout << GREEN << "  🎉 Player 2 (" << BLUE << "O" << GREEN << ") Wins! 🎉" << RESET << "\n";
    }
}


void start() {
    char playAgain = 'n';

    do {
        clearTerminal();
        printBanner();

        int mode;
        std::cout << "\n  [1] 1-Player (vs Bot)\n"
                  << "  [2] 2-Players\n\n"
                  << "  Select mode (1 or 2): ";

        while (!(std::cin >> mode) || (mode != 1 && mode != 2)) {
            std::cout << RED << "  Invalid entry! " << RESET << "Enter 1 or 2: ";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        char board[9] = {
            '0', '1', '2',
            '3', '4', '5',
            '6', '7', '8'
        };

        if (mode == 1) {
            playBot(board);
        } else {
            play2(board);
        }

        std::cout << "\n  Would you like to play again? (y/n): ";
        std::cin >> playAgain;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (playAgain == 'y' || playAgain == 'Y');

    std::cout << "\n  Thanks for playing! Goodbye!\n";
}

int main() {
    #ifdef _WIN32
    // Enable UTF-8 and ANSI color escape sequences on Windows terminals
    SetConsoleOutputCP(CP_UTF8);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    #endif

    start();
    return 0;
}