#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <functional>
#include <chrono>

class Game;
class Gui;

// Represents an interactive button with an "OK" confirmation feature
class OkButton
{
public:
    OkButton(sf::Vector2f position);
    
    static sf::Font font; // Shared font for all buttons
    static sf::Vector2f rect_size; // Default button size
    
    void (*on_click_function)(OkButton& self, Gui* game) = nullptr; // Callback function for button click
    
    sf::Vector2f position; // Position of the button
    std::string str = "0"; // Main button label
    std::string ok_str = "ok"; // Confirmation button label
    
    int id = -1; // Identifier for the button
    bool high_light = false; // Highlight state
    
    void set_str(const std::string& text);
    void set_ok(const std::string& text);
    void draw(sf::RenderWindow& window);
    
    bool contains(sf::Vector2f point); // Checks if a point is inside the button
    bool ok_contains(sf::Vector2f point); // Checks if a point is inside the confirmation button

    void on_ok(Gui* gui); // Executes when the "OK" button is clicked
    void set_on_click(void (*func)(OkButton& self, Gui* gui)); // Sets the callback function
};

// Manages the graphical user interface (GUI) of the game
class Gui
{
    typedef struct Message
    {
        std::string str; // Message text
        sf::Vector2f center; // Position of the message
        sf::Color color; // Message color
        std::chrono::steady_clock::time_point time_start; // Time when the message was created
        int milliseconds; // Duration the message is displayed
    } Massage;

    friend class Game;
    friend class OkButton;

    Gui(Game* game);
    ~Gui();

    Game* game; // Reference to the game instance

    const int board_size = 900; // Size of the board in pixels
    const int left_side_width = 300; // Width of the left UI panel
    float square_size; // Size of a single square on the board

    sf::Vector2i current_selected_cell = {-1, 0}; // Currently selected cell
    sf::RenderWindow window; // Main game window
    sf::Vector2f mouse_position; // Mouse position in the window
    sf::Font font; // Font used in the GUI

    std::vector<OkButton> buttons; // List of buttons in the UI
    std::vector<Massage> messages; // Displayed messages

    OkButton* current_button = nullptr; // Currently selected button

    void update(); // Updates the GUI state
    void render(); // Renders the GUI elements
    void input(); // Handles user input

    void render_board(); // Renders the game board
    void render_left_side(); // Renders the left UI panel
    void render_messages(); // Renders floating messages

    void handle_button_click(); // Handles button click events

    void handle_left_release();
    void handle_left_click();

    void handle_right_click();
    void handle_right_release();

    void set_game(Game* game); // Sets the associated game instance
    void update_by_board_size(); // Updates UI elements based on board size

    void pop_up_message(const std::string& str, int milliseconds = 1500, const sf::Color& color = {0, 0, 0}); // Displays a pop-up message
    void filter_messages(); // Removes expired messages

    OkButton& add_button(const std::string& str, const std::string& ok_str, int type = 0); // Adds a button to the UI

    void refresh(); // Resets UI state
    void initialize_buttons(); // Initializes all UI buttons

    static void center_text(sf::Text& text, sf::Vector2f center); // Centers text at a given position
};
