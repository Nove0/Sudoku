#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include "gui.hpp"
#include <bitset>

#define MAX_BOARD_SIZE 100 // Maximum board size supported
#define MAX_SOLVE_SIZE 9 // Maximum board size that can be solved by AI

// Represents the Sudoku game logic and management
class Game
{
    friend class Gui; // Allows Gui class to access private members

public:
    Game(int board_size = 9); // Constructor initializes a game with a given board size
    void run(); // Main game loop

private:
    using BitBoard = unsigned int; // Used for bitmasking row/column/subgrid constraints
    using Board = std::vector<std::vector<int>>; // 2D vector representing the Sudoku board
    using BoolBoard = std::vector<std::vector<bool>>; // 2D vector used for checking cells stated

    static const std::unordered_map<int, int> subgrid_size_map; // Maps board sizes to their subgrid sizes

    int subgrid_lut[MAX_BOARD_SIZE][MAX_BOARD_SIZE] = {}; // Lookup table for subgrid indices
    Board board; // The Sudoku board
    Board known_solution; // The solution (if known)
    bool is_solution_known = false;
    BoolBoard from_puzzle; // The part of the board that was already solved
    int subgrid_size; // Size of each subgrid
    bool running = true; // Controls the game loop

    void initialize_subgrid_lut(int size); // Initializes the subgrid lookup table
    bool is_valid_board_size(int size); // Checks if a board size is supported
    void change_board_size_if_valid(int size); // Changes board size if it's valid

    void clear_board(); // Clears the board
    void clear_from_puzzle(); // Sets all cells of from_puzzle to false
    bool in_range(int v); // Checks if a value is within board range
    bool is_solved(); // Checks if the board is solved

    void fit_known_solution_vec();

    void set_from_puzzle_as_state(); // Sets from_puzzle according to the current board

    // Solver function using backtracking with bitmask optimizations
    bool solver(Board& board, int row, int col, BitBoard seen_row[MAX_SOLVE_SIZE], 
                BitBoard seen_col[MAX_SOLVE_SIZE], BitBoard seen_subgrid[MAX_SOLVE_SIZE], int n);

    void solve_board(); // Solves the current board

    Gui gui; // Graphical User Interface for the game


    // Copies board content safely
    template <typename T>
    void copy_board(std::vector<std::vector<T>>& source, std::vector<std::vector<T>>& dest)
    {
        int size = std::min(source.size(), dest.size());

        for (int i = 0; i < size; ++i)
        {
            std::copy_n(source[i].begin(), size, dest[i].begin());
        }
    }
};
