    #include <iostream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <chrono> // For better sleep
#include <thread> // For std::this_thread::sleep_for
#include <limits> // Required for numeric_limits

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h> // For _getch()
#else
    #include <unistd.h>
    #include <termios.h> // For non-buffered input on Linux/macOS
    #include <sys/ioctl.h> // For terminal size
#endif

// ANSI Escape Codes for colors and text styles
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLD    "\033[1m"       /* Bold */
#define UNDERLINE "\033[4m"     /* Underline */
#define BG_BLACK   "\033[40m"   /* Background Black */
#define BG_RED     "\033[41m"   /* Background Red */
#define BG_GREEN   "\033[42m"   /* Background Green */
#define BG_YELLOW  "\033[43m"   /* Background Yellow */
#define BG_BLUE    "\033[44m"   /* Background Blue */
#define BG_MAGENTA "\033[45m"   /* Background Magenta */
#define BG_CYAN    "\033[46m"   /* Background Cyan */
#define BG_WHITE   "\033[47m"   /* Background White */

using namespace std;

// Function to enable ANSI escape codes on Windows
void enableAnsiColors() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// Custom sleep function for cross-platform compatibility
void customSleep(int milliseconds) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

// Non-buffered input for "Press any key to continue"
char getch_custom() {
#ifdef _WIN32
    return _getch();
#else
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return (char)ch;
#endif
}

// Player statistics structure
struct PlayerStats {
    string name;
    int wins;
    int losses;
    int ties;
    int totalGames;
    double winRate;
    
    PlayerStats() : wins(0), losses(0), ties(0), totalGames(0), winRate(0.0) {}
    
    void updateStats(int result) { // 1=win, 0=tie, -1=loss
        totalGames++;
        if (result == 1) wins++;
        else if (result == 0) ties++;
        else losses++;
        winRate = (totalGames > 0) ? (double)wins / totalGames * 100 : 0.0;
    }
};

// Game class untuk mengelola semua aspek permainan
class TicTacToeGame {
private:
    char board[9];
    PlayerStats player1Stats, player2Stats, computerStats;
    vector<string> gameHistory;
    bool soundEnabled;
    int totalGamesPlayed;
    
public:
    TicTacToeGame() : soundEnabled(true), totalGamesPlayed(0) {
        clearBoard();
        enableAnsiColors(); // Enable ANSI colors at startup
    }
    
    void clearBoard() {
        for(int i = 0; i < 9; i++) board[i] = ' ';
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            // Clear screen and move cursor to top-left
            cout << "\033[2J\033[H";
        #endif
    }
    
    void playSound(string sound) {
        if (!soundEnabled) return;
        #ifdef _WIN32
            if (sound == "move") Beep(800, 100);
            else if (sound == "win") Beep(1000, 200);
            else if (sound == "lose") Beep(400, 300);
            else if (sound == "tie") Beep(600, 150);
            else if (sound == "select") Beep(700, 50); // New sound for menu selection
        #endif
    }

    // Typing effect for messages
    void typeMessage(const string& message, int delay_ms = 30) {
        for (char c : message) {
            cout << c << flush;
            customSleep(delay_ms);
        }
        cout << RESET; // Ensure color reset after message
    }
    
    void printHeader() {
        clearScreen();
        cout << BOLD << BG_BLUE << WHITE << "============================================================\n";
        cout << "             BERN'S TIC TAC TOE PROJECT                  \n";
        cout << "              " << YELLOW << "Created by: Group Bern" << WHITE << "                 \n";
        cout << "============================================================" << RESET << "\n\n";
    }
    
    void drawAdvancedBoard() {
        cout << BOLD << CYAN << "\n     * GAME BOARD *\n\n" << RESET;
        cout << BLUE << "     |     |     \n";
        cout << "  " << (board[0] == 'X' ? RED : (board[0] == 'O' ? GREEN : WHITE)) << board[0] << BLUE << "  |  "
             << (board[1] == 'X' ? RED : (board[1] == 'O' ? GREEN : WHITE)) << board[1] << BLUE << "  |  "
             << (board[2] == 'X' ? RED : (board[2] == 'O' ? GREEN : WHITE)) << board[2] << BLUE << "  \n";
        cout << "_____|_____|_____\n";
        cout << "     |     |     \n";
        cout << "  " << (board[3] == 'X' ? RED : (board[3] == 'O' ? GREEN : WHITE)) << board[3] << BLUE << "  |  "
             << (board[4] == 'X' ? RED : (board[4] == 'O' ? GREEN : WHITE)) << board[4] << BLUE << "  |  "
             << (board[5] == 'X' ? RED : (board[5] == 'O' ? GREEN : WHITE)) << board[5] << BLUE << "  \n";
        cout << "_____|_____|_____\n";
        cout << "     |     |     \n";
        cout << "  " << (board[6] == 'X' ? RED : (board[6] == 'O' ? GREEN : WHITE)) << board[6] << BLUE << "  |  "
             << (board[7] == 'X' ? RED : (board[7] == 'O' ? GREEN : WHITE)) << board[7] << BLUE << "  |  "
             << (board[8] == 'X' ? RED : (board[8] == 'O' ? GREEN : WHITE)) << board[8] << BLUE << "  \n";
        cout << "     |     |     \n" << RESET;
        cout << '\n';
    }
    
    void showMenu() {
        printHeader();
        cout << BOLD << YELLOW << "*** SELECT GAME MODE ***\n\n" << RESET;
        cout << "1. " << GREEN << "VS Computer (AI Challenge)\n" << RESET;
        cout << "2. " << CYAN << "Multiplayer (Local Battle)\n" << RESET;
        cout << "3. " << MAGENTA << "View Statistics\n" << RESET;
        cout << "4. " << WHITE << "Toggle Sound (" << (soundEnabled ? GREEN "ON" : RED "OFF") << WHITE << ")\n" << RESET;
        cout << "5. " << BLUE << "Game History\n" << RESET;
        cout << "6. " << RED << "Exit Game\n\n" << RESET;
        cout << BOLD << "Enter your choice (1-6): " << RESET;
    }
    
    int getDifficultyLevel() {
        int difficulty;
        printHeader(); // Clear and print header again for clean look
        cout << BOLD << YELLOW << "*** SELECT AI DIFFICULTY ***\n\n" << RESET;
        cout << "1. " << GREEN << "Easy Mode (Random moves)\n" << RESET;
        cout << "2. " << CYAN << "Medium Mode (Smart offense)\n" << RESET;
        cout << "3. " << MAGENTA << "Hard Mode (Master AI)\n" << RESET;
        cout << "4. " << RED << "Impossible Mode (Unbeatable)\n\n" << RESET;
        
        do {
            cout << BOLD << "Choose difficulty (1-4): " << RESET;
            cin >> difficulty;
            playSound("select");
            if (cin.fail() || difficulty < 1 || difficulty > 4) {
                cin.clear(); // Clear error flags
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << RED << "Invalid choice! Please select 1-4.\n" << RESET;
                customSleep(500);
            }
        } while (difficulty < 1 || difficulty > 4);
        
        // Membersihkan buffer setelah membaca integer 'difficulty'
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        return difficulty;
    }
    
    void playerMove(char player, string playerName) {
        int choice;
        bool validMove = false;
        
        do {
            cout << BOLD << "Player " << (player == 'X' ? RED : GREEN) << playerName << RESET << " (" << (player == 'X' ? RED : GREEN) << player << RESET << "), choose position (1-9): " << RESET;
            cin >> choice;
            
            if (cin.fail()) {
                cin.clear(); // Clear error flags
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
                cout << RED << "Invalid input! Please enter a number.\n" << RESET;
                customSleep(500);
                continue;
            }

            if (choice < 1 || choice > 9) {
                cout << RED << "Invalid position! Choose 1-9.\n" << RESET;
                customSleep(500);
                continue;
            }
            
            choice--; // Adjust to 0-indexed array
            if (board[choice] == ' ') {
                board[choice] = player;
                playSound("move");
                validMove = true;
            } else {
                cout << RED << "Position occupied! Try another spot.\n" << RESET;
                customSleep(500);
            }
        } while (!validMove);
        
        // Membersihkan buffer setelah membaca integer 'choice'
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    void computerMove(char computer, char player, int difficulty) {
        int choice = -1;
        
        cout << BOLD << YELLOW;
        typeMessage("Computer is thinking...", 50);
        cout << RESET << "\n";
        
        customSleep(1000); // Delay for dramatic effect
        
        switch (difficulty) {
            case 1: // Easy - Random
                do {
                    choice = rand() % 9;
                } while (board[choice] != ' ');
                break;
                
            case 2: // Medium - Win if possible, then block, then take center/corner, then random
                choice = findWinningMove(computer); // Try to win
                if (choice == -1) {
                    choice = findWinningMove(player); // Try to block
                }
                if (choice == -1) { // If no immediate win or block, take center or corner
                    if (board[4] == ' ') choice = 4; // Take center
                    else if (board[0] == ' ') choice = 0;
                    else if (board[2] == ' ') choice = 2;
                    else if (board[6] == ' ') choice = 6;
                    else if (board[8] == ' ') choice = 8;
                }
                if (choice == -1) { // Fallback to random
                    do {
                        choice = rand() % 9;
                    } while (board[choice] != ' ');
                }
                break;
                
            case 3: // Hard - Win or block, then strategic moves
                choice = findWinningMove(computer); // Try to win
                if (choice == -1) choice = findWinningMove(player); // Try to block
                if (choice == -1) { // If no immediate win or block, apply strategy
                    // Take center if available
                    if (board[4] == ' ') choice = 4;
                    // Take opposite corner if player took a corner
                    else if (board[0] == player && board[8] == ' ') choice = 8;
                    else if (board[2] == player && board[6] == ' ') choice = 6;
                    else if (board[6] == player && board[2] == ' ') choice = 2;
                    else if (board[8] == player && board[0] == ' ') choice = 0;
                    // Take any available corner
                    else if (board[0] == ' ') choice = 0;
                    else if (board[2] == ' ') choice = 2;
                    else if (board[6] == ' ') choice = 6;
                    else if (board[8] == ' ') choice = 8;
                    // Take any available side
                    else if (board[1] == ' ') choice = 1;
                    else if (board[3] == ' ') choice = 3;
                    else if (board[5] == ' ') choice = 5;
                    else if (board[7] == ' ') choice = 7;
                }
                if (choice == -1) { // Fallback to random (shouldn't happen often in Hard)
                     do {
                        choice = rand() % 9;
                    } while (board[choice] != ' ');
                }
                break;
                
            case 4: // Impossible - Minimax algorithm
                choice = minimaxMove(computer, player);
                break;
        }
        
        board[choice] = computer;
        cout << BOLD << CYAN << "Computer chose position " << (choice + 1) << "!\n" << RESET;
        playSound("move");
    }
    
    int findWinningMove(char player) {
        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = player;
                if (checkWinner() == player) {
                    board[i] = ' '; // Undo the move
                    return i;
                }
                board[i] = ' '; // Undo the move
            }
        }
        return -1;
    }
    
    int minimaxMove(char computer, char player) {
        int bestMove = -1;
        int bestScore = -1000; // Initialize with a very low score
        
        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') {
                board[i] = computer;
                int score = minimax(false, computer, player); // Opponent's turn
                board[i] = ' '; // Undo the move
                
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = i;
                }
            }
        }
        return bestMove;
    }
    
    int minimax(bool isMaximizing, char computer, char player) {
        char winner = checkWinner();
        if (winner == computer) return 1; // Computer wins
        if (winner == player) return -1;  // Player wins
        if (isTie()) return 0;            // It's a tie
        
        if (isMaximizing) { // Computer's turn (maximizing player)
            int bestScore = -1000;
            for (int i = 0; i < 9; i++) {
                if (board[i] == ' ') {
                    board[i] = computer;
                    int score = minimax(false, computer, player);
                    board[i] = ' ';
                    bestScore = max(score, bestScore);
                }
            }
            return bestScore;
        } else { // Player's turn (minimizing player)
            int bestScore = 1000;
            for (int i = 0; i < 9; i++) {
                if (board[i] == ' ') {
                    board[i] = player;
                    int score = minimax(true, computer, player);
                    board[i] = ' ';
                    bestScore = min(score, bestScore);
                }
            }
            return bestScore;
        }
    }
    
    char checkWinner() {
        // Check rows, columns, diagonals
        int winPatterns[8][3] = {
            {0,1,2}, {3,4,5}, {6,7,8}, // rows
            {0,3,6}, {1,4,7}, {2,5,8}, // columns
            {0,4,8}, {2,4,6}           // diagonals
        };
        
        for (int i = 0; i < 8; i++) {
            if (board[winPatterns[i][0]] != ' ' &&
                board[winPatterns[i][0]] == board[winPatterns[i][1]] &&
                board[winPatterns[i][1]] == board[winPatterns[i][2]]) {
                return board[winPatterns[i][0]];
            }
        }
        return ' ';
    }
    
    bool isTie() {
        for (int i = 0; i < 9; i++) {
            if (board[i] == ' ') return false;
        }
        return true;
    }
    
    void displayWinner(char winner, string player1Name, string player2Name) {
        clearScreen();
        printHeader();
        drawAdvancedBoard();
        
        cout << BOLD << BG_GREEN << WHITE << "========================================\n" << RESET;
        if (winner == 'X') {
            cout << BOLD << GREEN << "*** CONGRATULATIONS! ***\n" << RESET;
            cout << BOLD << RED << "*** " << player1Name << " WINS! ***\n" << RESET;
            playSound("win");
            showCelebration();
        } else if (winner == 'O') {
            if (player2Name == "Computer") {
                cout << BOLD << RED << "*** COMPUTER WINS! ***\n" << RESET;
                playSound("lose");
                showDefeatMessage();
            } else {
                cout << BOLD << GREEN << "*** CONGRATULATIONS! ***\n" << RESET;
                cout << BOLD << GREEN << "*** " << player2Name << " WINS! ***\n" << RESET;
                playSound("win");
                showCelebration();
            }
        }
        cout << BOLD << BG_GREEN << WHITE << "========================================\n" << RESET;
        cout << "\nPress any key to continue...";
        getch_custom();
    }
    
    void displayTie() {
        clearScreen();
        printHeader();
        drawAdvancedBoard();
        cout << BOLD << BG_YELLOW << BLACK << "========================================\n" << RESET;
        cout << BOLD << YELLOW << "*** IT'S A TIE! GREAT GAME! ***\n" << RESET;
        cout << BOLD << BG_YELLOW << BLACK << "========================================\n" << RESET;
        playSound("tie");
        cout << "\nPress any key to continue...";
        getch_custom();
    }
    
    void showCelebration() {
        cout << BOLD << MAGENTA << "    * * * VICTORY ACHIEVED! * * *\n" << RESET;
        cout << BOLD << CYAN << "  Excellent gameplay! Well done!\n" << RESET;
        // Simple animation
        for (int i = 0; i < 3; ++i) {
            cout << YELLOW << "  \\(^_^)/ " << RESET << flush;
            customSleep(200);
            cout << "\r" << "          " << flush; // Clear line
            cout << GREEN << "  \\(^_^)/ " << RESET << flush;
            customSleep(200);
            cout << "\r" << "          " << flush; // Clear line
        }
        cout << "\n";
    }

    void showDefeatMessage() {
        cout << BOLD << RED << "    (>_<)   OH NO! YOU LOST!   (>_<)\n" << RESET;
        cout << BOLD << WHITE << "  Better luck next time!\n" << RESET;
        // Simple animation
        for (int i = 0; i < 3; ++i) {
            cout << RED << "  (x_x) " << RESET << flush;
            customSleep(200);
            cout << "\r" << "        " << flush; // Clear line
            cout << WHITE << "  (o_o) " << RESET << flush;
            customSleep(200);
            cout << "\r" << "        " << flush; // Clear line
        }
        cout << "\n";
    }
    
    void updateGameHistory(string result) {
        time_t now = time(0);
        char* timeStr = ctime(&now);
        string timestamp(timeStr);
        timestamp.pop_back(); // Remove newline
        gameHistory.push_back(timestamp + " - " + result);
        if (gameHistory.size() > 10) { // Keep only last 10 games
            gameHistory.erase(gameHistory.begin());
        }
    }
    
    void showStatistics() {
        clearScreen();
        printHeader();
        cout << BOLD << YELLOW << "*** GAME STATISTICS ***\n\n" << RESET;
        
        cout << BOLD << CYAN << "----------------------------------------\n" << RESET;
        cout << BOLD << WHITE << "          PLAYER STATISTICS            \n" << RESET;
        cout << BOLD << CYAN << "----------------------------------------\n" << RESET;
        
        if (player1Stats.totalGames > 0) {
            cout << BOLD << "Player: " << GREEN << player1Stats.name << RESET << "\n";
            cout << "   Wins: " << player1Stats.wins << " | Losses: " << player1Stats.losses;
            cout << " | Ties: " << player1Stats.ties << "\n";
            cout << "   Win Rate: " << fixed << setprecision(1) << player1Stats.winRate << "%\n\n";
        } else {
            cout << "Player 1: No games played yet.\n\n";
        }
        
        if (player2Stats.totalGames > 0) {
            cout << BOLD << "Player: " << GREEN << player2Stats.name << RESET << "\n";
            cout << "   Wins: " << player2Stats.wins << " | Losses: " << player2Stats.losses;
            cout << " | Ties: " << player2Stats.ties << "\n";
            cout << "   Win Rate: " << fixed << setprecision(1) << player2Stats.winRate << "%\n\n";
        } else {
            cout << "Player 2: No games played yet.\n\n";
        }
        
        if (computerStats.totalGames > 0) {
            cout << BOLD << "Computer:\n" << RESET;
            cout << "   Wins: " << computerStats.wins << " | Losses: " << computerStats.losses;
            cout << " | Ties: " << computerStats.ties << "\n";
            cout << "   Win Rate: " << fixed << setprecision(1) << computerStats.winRate << "%\n\n";
        } else {
            cout << "Computer: No games played yet.\n\n";
        }
        
        cout << BOLD << "Total Games Played: " << totalGamesPlayed << "\n" << RESET;
        
        cout << "\nPress any key to continue...";
        getch_custom();
    }
    
    void showGameHistory() {
        clearScreen();
        printHeader();
        cout << BOLD << YELLOW << "*** GAME HISTORY (Last 10 games) ***\n\n" << RESET;
        
        if (gameHistory.empty()) {
            cout << RED << "No games played yet!\n" << RESET;
        } else {
            for (int i = gameHistory.size() - 1; i >= 0; i--) {
                cout << (gameHistory.size() - i) << ". " << GREEN << gameHistory[i] << RESET << "\n";
            }
        }
        
        cout << "\nPress any key to continue...";
        getch_custom();
    }
    
    bool playAgain() {
        char choice;
        cout << BOLD << "\nPlay another round? (y/n): " << RESET;
        cin >> choice;
        playSound("select");
        // Clear buffer after char input
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return (choice == 'y' || choice == 'Y');
    }
    
    void playVsComputer() {
        string playerName;
        cout << BOLD << "Enter your name: " << RESET;
        getline(cin, playerName);
        
        int difficulty = getDifficultyLevel(); // getDifficultyLevel() membersihkan buffernya sendiri
        string difficultyName[] = {"", "Easy", "Medium", "Hard", "Impossible"};
        cout << BOLD << "\nStarting game against " << CYAN << difficultyName[difficulty] << RESET << " AI...\n" << RESET;
        customSleep(1500);
        
        do {
            clearBoard();
            bool gameRunning = true;
            totalGamesPlayed++;
            
            while (gameRunning) {
                clearScreen();
                printHeader();
                cout << BOLD << "Mode: " << GREEN << "VS Computer (" << difficultyName[difficulty] << ")\n" << RESET;
                cout << BOLD << "Player: " << RED << playerName << RESET << " (X) vs " << GREEN << "Computer" << RESET << " (O)\n" << RESET;
                drawAdvancedBoard();
                
                // Player move
                playerMove('X', playerName); // playerMove() membersihkan buffernya sendiri
                char winner = checkWinner();
                if (winner == 'X') {
                    displayWinner('X', playerName, "Computer");
                    player1Stats.name = playerName;
                    player1Stats.updateStats(1);
                    computerStats.name = "Computer";
                    computerStats.updateStats(-1);
                    updateGameHistory(playerName + " defeated Computer (" + difficultyName[difficulty] + ")");
                    gameRunning = false;
                    break;
                }
                if (isTie()) {
                    displayTie();
                    player1Stats.name = playerName;
                    player1Stats.updateStats(0);
                    computerStats.name = "Computer";
                    computerStats.updateStats(0);
                    updateGameHistory("Tie: " + playerName + " vs Computer (" + difficultyName[difficulty] + ")");
                    gameRunning = false;
                    break;
                }
                
                // Computer move
                clearScreen();
                printHeader();
                cout << BOLD << "Mode: " << GREEN << "VS Computer (" << difficultyName[difficulty] << ")\n" << RESET;
                cout << BOLD << "Player: " << RED << playerName << RESET << " (X) vs " << GREEN << "Computer" << RESET << " (O)\n" << RESET;
                drawAdvancedBoard();
                computerMove('O', 'X', difficulty);
                winner = checkWinner();
                if (winner == 'O') {
                    displayWinner('O', playerName, "Computer");
                    player1Stats.name = playerName;
                    player1Stats.updateStats(-1);
                    computerStats.name = "Computer";
                    computerStats.updateStats(1);
                    updateGameHistory("Computer (" + difficultyName[difficulty] + ") defeated " + playerName);
                    gameRunning = false;
                    break;
                }
                if (isTie()) {
                    displayTie();
                    player1Stats.name = playerName;
                    player1Stats.updateStats(0);
                    computerStats.name = "Computer";
                    computerStats.updateStats(0);
                    updateGameHistory("Tie: " + playerName + " vs Computer (" + difficultyName[difficulty] + ")");
                    gameRunning = false;
                    break;
                }
            }
        } while (playAgain());
    }
    
    void playMultiplayer() {
        string player1Name, player2Name;
        cout << BOLD << "Enter Player 1 name (X): " << RESET;
        getline(cin, player1Name);
        cout << BOLD << "Enter Player 2 name (O): " << RESET;
        getline(cin, player2Name);
        
        do {
            clearBoard();
            bool gameRunning = true;
            totalGamesPlayed++;
            
            while (gameRunning) {
                // Player 1 move
                clearScreen();
                printHeader();
                cout << BOLD << "Mode: " << CYAN << "Multiplayer\n" << RESET;
                cout << BOLD << RED << player1Name << RESET << " (X) vs " << GREEN << player2Name << RESET << " (O)\n" << RESET;
                drawAdvancedBoard();
                playerMove('X', player1Name); // playerMove() membersihkan buffernya sendiri
                char winner = checkWinner();
                if (winner == 'X') {
                    displayWinner('X', player1Name, player2Name);
                    player1Stats.name = player1Name;
                    player1Stats.updateStats(1);
                    player2Stats.name = player2Name;
                    player2Stats.updateStats(-1);
                    updateGameHistory(player1Name + " defeated " + player2Name);
                    gameRunning = false;
                    break;
                }
                if (isTie()) {
                    displayTie();
                    player1Stats.name = player1Name;
                    player1Stats.updateStats(0);
                    player2Stats.name = player2Name;
                    player2Stats.updateStats(0);
                    updateGameHistory("Tie: " + player1Name + " vs " + player2Name);
                    gameRunning = false;
                    break;
                }
                
                // Player 2 move
                clearScreen();
                printHeader();
                cout << BOLD << "Mode: " << CYAN << "Multiplayer\n" << RESET;
                cout << BOLD << RED << player1Name << RESET << " (X) vs " << GREEN << player2Name << RESET << " (O)\n" << RESET;
                drawAdvancedBoard();
                playerMove('O', player2Name); // playerMove() membersihkan buffernya sendiri
                winner = checkWinner();
                if (winner == 'O') {
                    displayWinner('O', player1Name, player2Name);
                    player1Stats.name = player1Name;
                    player1Stats.updateStats(-1);
                    player2Stats.name = player2Name;
                    player2Stats.updateStats(1);
                    updateGameHistory(player2Name + " defeated " + player1Name);
                    gameRunning = false;
                    break;
                }
                if (isTie()) {
                    displayTie();
                    player1Stats.name = player1Name;
                    player1Stats.updateStats(0);
                    player2Stats.name = player2Name;
                    player2Stats.updateStats(0);
                    updateGameHistory("Tie: " + player1Name + " vs " + player2Name);
                    gameRunning = false;
                    break;
                }
            }
        } while (playAgain());
    }
    
    void run() {
        srand(time(0));
        int choice;
        
        cout << BOLD << BG_MAGENTA << WHITE;
        typeMessage("Initializing Bern's Tic Tac Toe Games...", 40);
        cout << RESET << "\n";
        
        customSleep(1500);
        
        do {
            showMenu();
            cin >> choice;
            playSound("select");
            
            // Membersihkan buffer setelah membaca integer 'choice'
            // Ini penting agar `getline` berikutnya tidak membaca newline yang tersisa.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            
            switch (choice) {
                case 1:
                    playVsComputer();
                    break;
                case 2:
                    playMultiplayer();
                    break;
                case 3:
                    showStatistics();
                    break;
                case 4:
                    soundEnabled = !soundEnabled;
                    cout << BOLD << "Sound " << (soundEnabled ? GREEN "enabled" : RED "disabled") << "!\n" << RESET;
                    customSleep(1000);
                    break;
                case 5:
                    showGameHistory();
                    break;
                case 6:
                    clearScreen();
                    cout << BOLD << BG_BLUE << WHITE;
                    typeMessage("Thanks for playing Bern's Tic Tac Toe Game!\n", 40);
                    typeMessage("Come back soon for more epic battles!\n", 40);
                    cout << RESET;
                    customSleep(2000);
                    break;
                default:
                    cout << RED << "Invalid choice! Please select 1-6.\n" << RESET;
                    customSleep(1000);
            }
        } while (choice != 6);
    }
};

int main() {
    TicTacToeGame game;
    game.run();
    return 0;
}