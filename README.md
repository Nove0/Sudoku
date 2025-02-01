# Sudoku Solver & GUI

A simple and interactive Sudoku solver with a graphical UI powered by SFML. This project lets you generate puzzles, solve them using AI, and manually play around with different board sizes.

## Features

- **Intuitive GUI:** Click and type to interact with the board.
- **AI Solver:** Automatically solves puzzles up to **9x9** (possibly more but too slow).
- **Custom Board Sizes:** Supports sizes from **4x4 to 100x100** (possibly more but too small).
- **Puzzle Generator:** Create Sudoku puzzles with adjustable difficulty.
- **Keyboard Shortcuts:**
  - **Arrow Keys / WASD**: Move between cells.
  - **Enter**: Confirm input.
  - **Backspace/Delete**: Erase a number.
  - **Mouse Click**: Select cells and interact with buttons.

## Installation

### Prerequisites
- **SFML** (Simple and Fast Multimedia Library)
  - On Linux: `sudo apt install libsfml-dev`
  - On Windows: [Download from SFML website](https://www.sfml-dev.org/download.php) and set up manually.

### Build & Run

1. Clone the repository:
   ```sh
   git clone https://github.com/Nove0/Sudoku.git
   ```
2. Build using **Make**:
   ```sh
   make
   ```
3. Run the program:
   ```sh
   make run
   ```