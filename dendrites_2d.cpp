#include <stdint.h>
#include <iostream>
#include <bitset>
#include <Windows.h>
#include <math.h>

#define BOARDWIDTH 120
#define BOARDHEIGHT 40
#define PI 3.1415926535
#define MAXPRINTBUFFER 2048


void printBoard(std::bitset<BOARDWIDTH> board[BOARDHEIGHT]) {
    /// Prints the board into the command line, using the specified characters for the filled/unfilled cells
    char line[MAXPRINTBUFFER];

    std::cout.flush();

    size_t textIndex = 0;
    for (size_t y = 0; y < BOARDHEIGHT; y++) {
        for (size_t x = 0; x < BOARDWIDTH; x++) {

            line[textIndex++] = board[y][x] ? 'X' : ' '; //(char)254u : ' ';

            if (textIndex == MAXPRINTBUFFER - 1) {
                line[textIndex] = '\0';
                std::cout << line;
                textIndex = 0;
            }

        }

        line[textIndex++] = '\n';

        if (textIndex == MAXPRINTBUFFER - 1) {
            line[textIndex] = '\0';
            std::cout << line;
            textIndex = 0;
        }
    }

    std::cout << std::endl;
}


double weightingFunc(uint32_t x, uint32_t y) {
    /// Returns a weighting value indicating the probability of plating occuring at a given location

    // This weighting function is a simple linear concentration gradient, increasing from left to right
    return (double) y / BOARDHEIGHT;
}


uint8_t countNeighbours(std::bitset<BOARDWIDTH> board[BOARDHEIGHT], uint32_t x, uint32_t y) {
    /// Returns the number of cells surrounding a central cell at (x, y)
    return board[y - 1][x - 1] + board[y - 1][x] + board[y - 1][x + 1] + board[y][x - 1] + board[y][x + 1] + board[y + 1][x - 1] + board[y + 1][x] + board[y + 1][x + 1];
}

uint8_t countTrueNeighbours(std::bitset<BOARDWIDTH> board[BOARDHEIGHT], uint32_t x, uint32_t y) {
    /// Returns the number of cells surrounding a central cell at (x, y)
    return board[y - 1][x] + board[y][x - 1] + board[y][x + 1] + board[y + 1][x];
}


void updateCell(uint32_t x, uint32_t y, std::bitset<BOARDWIDTH> board[BOARDHEIGHT], std::bitset<BOARDWIDTH> tempBoard[BOARDHEIGHT]) {
    /// Evaluates the game rules for a single cell on the board at (x, y), setting the outcome on the tempBoard
    ///

    // Probabilistically generate new automata in the direction of lowest existing density
    double prob;
    uint8_t numNeighbours = countNeighbours(tempBoard, x, y);

    if (board[y][x]) { // For an alive cell, find the probability of killing it

        switch(numNeighbours) {
            case 0: prob = 1; break;
            case 1: 
            case 2: 
            case 3: prob = 0; break;
            case 4: 
            case 5: 
            case 6: 
            case 7: 
            case 8: prob = 0; break;
        }

        // Probabilistically kill the cell at (x, y)
        if (rand() < prob * RAND_MAX) {
            board[y].reset(x);
        }

    } else { // For a dead cell, find the probability of generating a cell

        prob = (double) numNeighbours / 8.0;

        // Probabilistically generate a cell at (x, y)
        if (rand() < prob * weightingFunc(x, y) * RAND_MAX) {
            board[y].set(x);
        }

    }
    
}


void updateBoard(std::bitset<BOARDWIDTH> board[BOARDHEIGHT], std::bitset<BOARDWIDTH> tempBoard[BOARDHEIGHT]) {
    /// Applies the rules to update the game board, excluding the boundary, which remains fixed throughout

    // Copy the current board to a temp board where we will evaluate each cell
    memmove(tempBoard, board, sizeof(*tempBoard) * BOARDHEIGHT);

    // Evaluate each cell in turn, updating the original board where necessary
    for (size_t y = 1; y < BOARDHEIGHT - 1; y++) {
        for (size_t x = 1; x < BOARDWIDTH - 1; x++) {
            updateCell(x, y, board, tempBoard);
        }
    }

}


int main () {

    // Setup initial array
    std::bitset<BOARDWIDTH> board[BOARDHEIGHT], tempBoard[BOARDHEIGHT];
    
    for (int x = 0; x < BOARDWIDTH; x++) {
        int yLim = (int) round(4 * sin((double) x / BOARDWIDTH * PI));
        for (int y = 0; y < yLim; y++) {
            board[y].set(x);
        }
    }
    
    board[4].set(BOARDWIDTH / 2);
    board[5].set(BOARDWIDTH / 2);
    board[6].set(BOARDWIDTH / 2);

    printBoard(board);
    char debugStr[128];
    std::cin >> debugStr;

    for (size_t time = 0; time < 1000; time++) {
        printBoard(board);
        updateBoard(board, tempBoard);
        Sleep(100);
    }

}