#pragma once
#include <vector>
#include <array>
#include <random>
#include <algorithm>
#include <iostream>

namespace generator
{
    using Board = std::vector<std::vector<int>>;

    class SudokuGenerator
    {
    public:
        SudokuGenerator(int size) :
            size(size),
            box_size(static_cast<int>(std::sqrt(size))),
            board(size, std::vector<int>(size, 0))
        {
            if (!is_valid_size(size))
            {
                throw std::invalid_argument("Invalid Sudoku size.");
            }
            initialize_lookup_tables();
        }

        Board generate_solved()
        {
            board.assign(size, std::vector<int>(size, 0));
            fill_board();
            return board;
        }

        Board generate_puzzle(int clues, Board& solution)
        {
            if (clues < size * size / 4 || clues > size * size)
            {
                throw std::invalid_argument("Invalid number of clues.");
            }

            Board solved_board = generate_solved();
            solution = solved_board;
            Board puzzle = solved_board;
            remove_numbers(puzzle, clues);
            return puzzle;
        }

    private:
        int size;
        int box_size;
        Board board;
        std::vector<std::vector<bool>> row_lookup;
        std::vector<std::vector<bool>> col_lookup;
        std::vector<std::vector<bool>> box_lookup;
        std::mt19937 rng{std::random_device{}()};

        bool is_valid_size(int n)
        {
            int root = static_cast<int>(std::sqrt(n));
            return root * root == n;
        }

        void initialize_lookup_tables()
        {
            row_lookup.assign(size, std::vector<bool>(size + 1, false));
            col_lookup.assign(size, std::vector<bool>(size + 1, false));
            box_lookup.assign(size, std::vector<bool>(size + 1, false));
        }

        bool fill_board(int row = 0, int col = 0)
        {
            if (row == size)
            {
                return true;
            }

            int next_row = (col == size - 1) ? row + 1 : row;
            int next_col = (col + 1) % size;

            std::vector<int> numbers(size);
            std::iota(numbers.begin(), numbers.end(), 1);
            std::shuffle(numbers.begin(), numbers.end(), rng);

            for (int num : numbers)
            {
                if (is_safe(row, col, num))
                {
                    place_number(row, col, num);
                    if (fill_board(next_row, next_col))
                    {
                        return true;
                    }
                    remove_number(row, col, num);
                }
            }

            return false;
        }

        bool is_safe(int row, int col, int num)
        {
            int box_index = (row / box_size) * box_size + (col / box_size);
            return !row_lookup[row][num] &&
                   !col_lookup[col][num] &&
                   !box_lookup[box_index][num];
        }

        void place_number(int row, int col, int num)
        {
            int box_index = (row / box_size) * box_size + (col / box_size);
            board[row][col] = num;
            row_lookup[row][num] = true;
            col_lookup[col][num] = true;
            box_lookup[box_index][num] = true;
        }

        void remove_number(int row, int col, int num)
        {
            int box_index = (row / box_size) * box_size + (col / box_size);
            board[row][col] = 0;
            row_lookup[row][num] = false;
            col_lookup[col][num] = false;
            box_lookup[box_index][num] = false;
        }

        void remove_numbers(Board &puzzle, int clues)
        {
            int remaining = size * size;
            std::vector<int> positions(remaining);
            std::iota(positions.begin(), positions.end(), 0);
            std::shuffle(positions.begin(), positions.end(), rng);

            for (int pos : positions)
            {
                if (remaining <= clues)
                {
                    break;
                }

                int row = pos / size;
                int col = pos % size;
                int temp = puzzle[row][col];
                puzzle[row][col] = 0;

                if (!has_unique_solution(puzzle))
                {
                    puzzle[row][col] = temp;
                }
                else
                {
                    --remaining;
                }
            }
        }

        bool has_unique_solution(Board puzzle)
        {
            int solutions = 0;
            solve(puzzle, 0, 0, solutions);
            return solutions == 1;
        }

        void  
        
        solve(Board &puzzle, int row, int col, int &solutions)
        {
            if (row == size)
            {
                ++solutions;
                return;
            }

            int next_row = (col == size - 1) ? row + 1 : row;
            int next_col = (col + 1) % size;

            if (puzzle[row][col] != 0)
            {
                solve(puzzle, next_row, next_col, solutions);
                return;
            }

            for (int num = 1; num <= size; ++num)
            {
                if (is_safe_solve(puzzle, row, col, num))
                {
                    puzzle[row][col] = num;
                    solve(puzzle, next_row, next_col, solutions);
                    puzzle[row][col] = 0;

                    if (solutions > 1)
                    {
                        return;
                    }
                }
            }
        }

        bool is_safe_solve(const Board &puzzle, int row, int col, int num)
        {
            int box_start_row = (row / box_size) * box_size;
            int box_start_col = (col / box_size) * box_size;

            for (int i = 0; i < size; ++i)
            {
                if (puzzle[row][i] == num || puzzle[i][col] == num)
                {
                    return false;
                }
            }

            for (int i = 0; i < box_size; ++i)
            {
                for (int j = 0; j < box_size; ++j)
                {
                    if (puzzle[box_start_row + i][box_start_col + j] == num)
                    {
                        return false;
                    }
                }
            }

            return true;
        }
    };
}
