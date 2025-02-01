#include "game.hpp"
#include <algorithm>
#include "generator.hpp"

Game::Game(int board_size):
    board(board_size, std::vector<int>(board_size, 0)),
    from_puzzle(board_size, std::vector<bool>(board_size, false)),
    gui(this)
{
    if(!is_valid_board_size(board_size))
    {
        throw std::runtime_error("Invalid board_size");
    }
    subgrid_size = subgrid_size_map.at(board_size);
    initialize_subgrid_lut(board_size);
}

void Game::run()
{
    while(running)
    {
        gui.update();
    }
}

void Game::initialize_subgrid_lut(int size)
{
    if(size > MAX_BOARD_SIZE || size <= 0) std::cout << "Unsupported size " << size << "\n"; // Debuging
    for (int r = 0; r < size; ++r)
    {
        for (int c = 0; c < size; ++c)
        {
            subgrid_lut[r][c] = (r / subgrid_size) * subgrid_size + c / subgrid_size;
        }
    }
}

bool Game::is_valid_board_size(int size)
{
    return subgrid_size_map.find(size) != subgrid_size_map.end();
}

void Game::change_board_size_if_valid(int size)
{
    if (!is_valid_board_size(size) || size == (int)board.size()) 
    {
        return;
    }

    subgrid_size = subgrid_size_map.at(size);
    initialize_subgrid_lut(size);

    Board new_board(size, std::vector<int>(size, 0));
    BoolBoard new_from_puzzle(size, std::vector<bool>(size, false));

    copy_board(board, new_board);
    copy_board(from_puzzle, new_from_puzzle);

    bool was_erased = false;
    for(int row = 0; row < size; ++row)
    {
        for(int col = 0; col < size; ++col)
        {
            if(new_board[row][col] < 0 || new_board[row][col] > size) 
            {
                new_board[row][col] = 0;
                new_from_puzzle[row][col] = false;
                was_erased = true; 
            }
        }
    }
    if(was_erased) gui.pop_up_message("Some data was removed tho to being out of range", 2000, {0, 0, 144});
    board = std::move(new_board);
    from_puzzle = std::move(new_from_puzzle);
    gui.update_by_board_size();
}


void Game::clear_board()
{
    for (auto& row : board)
    {
        std::fill(row.begin(), row.end(), 0);
    }
    clear_from_puzzle();
    is_solution_known = false; 
}

void Game::clear_from_puzzle()
{
    for (auto& row : from_puzzle)
    {
        std::fill(row.begin(), row.end(), false);
    }
}

bool Game::in_range(int v)
{
    return v >= 0 && v < (int)board.size();
}

bool Game::is_solved()
{
    int n = board.size();
    BoolBoard seen_row, seen_col, seen_subgrid;
    seen_row.resize(n, std::vector<bool>(n, false));
    seen_col.resize(n, std::vector<bool>(n, false));
    seen_subgrid.resize(n, std::vector<bool>(n, false));

    for(int row = 0; row < n; ++row)
    {
        for(int col = 0; col < n; ++col)
        {
            int subgrid = subgrid_lut[row][col];
            int num = board[row][col] - 1;
            if(num == -1 || seen_row[row][num] || seen_col[col][num] || seen_subgrid[subgrid][num]) 
            {
                return false;
            }
            seen_row[row][num] = true;
            seen_col[col][num] = true;
            seen_subgrid[subgrid][num] = true;
        }
    }
    return true;
}

void Game::fit_known_solution_vec()
{
    known_solution.assign(board.size(), std::vector<int>(board.size(), 0));
}

void Game::set_from_puzzle_as_state()
{
    int n = board.size();
    for(int row = 0; row < n; ++row)
    {
        for(int col = 0; col < n; ++col)
        {
            if(board[row][col] != 0) from_puzzle[row][col] = true;
            else from_puzzle[row][col] = false;
        }
    }
}

bool Game::solver(Board &board, int row, int col, 
                   BitBoard seen_row[MAX_SOLVE_SIZE], 
                   BitBoard seen_col[MAX_SOLVE_SIZE], 
                   BitBoard seen_subgrid[MAX_SOLVE_SIZE], int n)
{
    if (row == n)
    {
        return true;
    }

    if (col == n)
    {
        return solver(board, row + 1, 0, seen_row, seen_col, seen_subgrid, n);
    }

    if (board[row][col] != 0)
    {
        return solver(board, row, col + 1, seen_row, seen_col, seen_subgrid, n);
    }

    int subgrid = subgrid_lut[row][col];

    BitBoard seen = seen_row[row] | seen_col[col] | seen_subgrid[subgrid];
    BitBoard options = (~seen) & ((BitBoard(1) << n) - 1); // Mask to ensure only valid numbers (0 to n - 1) are considered.

    while (options)
    {
        int index = __builtin_ctz(options);

        BitBoard index_mask = BitBoard(1) << index;
        board[row][col] = index + 1; // convert index to an option
        seen_row[row]         |= index_mask;
        seen_col[col]         |= index_mask;
        seen_subgrid[subgrid] |= index_mask;

        if (solver(board, row, col + 1, seen_row, seen_col, seen_subgrid, n))
        {
            return true;
        }

        board[row][col] = 0;
        seen_row[row]         &= ~index_mask;
        seen_col[col]         &= ~index_mask;
        seen_subgrid[subgrid] &= ~index_mask;

        options &= (options - 1);
    }

    return false;
}

void Game::solve_board()
{
    int n = (int)board.size();
    if(n > MAX_SOLVE_SIZE)
    {
        gui.pop_up_message("AI solving is limited to a size of " + std::to_string(MAX_SOLVE_SIZE));
        return;
    }

    BitBoard seen_row[MAX_SOLVE_SIZE]     = { 0 };
    BitBoard seen_col[MAX_SOLVE_SIZE]     = { 0 };
    BitBoard seen_subgrid[MAX_SOLVE_SIZE] = { 0 };

    for(int row = 0; row < n; ++row)
    {
        for(int col = 0; col < n; ++col)
        {
            int num = board[row][col];
            if(num == 0) continue;
            
            int index = num -1;
            int index_mask = (1 << index);

            int subgrid = subgrid_lut[row][col];
            BitBoard seen = seen_row[row] | seen_col[col] | seen_subgrid[subgrid];


            if(seen & index_mask)
            {
                gui.pop_up_message("Invalid State", 1500, {144, 0, 0});
                return;
            }

            seen_row[row] |= index_mask;
            seen_col[col] |= index_mask;
            seen_subgrid[subgrid] |= index_mask;
            from_puzzle[row][col] = true;
        }
    }

    if(solver(board, 0, 0, seen_row, seen_col, seen_subgrid, n))
    {
        // is_solution_known = true;
        // fit_known_solution_vec();
        // copy_board(board, known_solution);

        // This is half good half bad (in one case its bad), i just left it
    }
    else 
    {
        gui.pop_up_message("Unsolvable Board", 1500, {144, 0, 0});
    }
}

const std::unordered_map<int, int> Game::subgrid_size_map = 
{
    {4, 2}, {9, 3}, {16, 4}, {25, 5}, {36, 6}, {49, 7}, {64, 8}, {81, 9}, {100, 10}
};
