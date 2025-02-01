
#include "gui.hpp"
#include "game.hpp"
#include "generator.hpp"

Gui::Gui(Game* game): 
    window(sf::VideoMode(board_size + left_side_width, board_size), "")
{
    if(!game) std::__throw_runtime_error("Gui must be initialized with a game");
    set_game(game);
    window.setFramerateLimit(28);
    font.loadFromFile("soft.ttf");
    OkButton::font = font;
    initialize_buttons();
}

Gui::~Gui()
{
    window.close();
}

void Gui::update()
{
    input();
    render();
}

void Gui::render()
{
    window.clear();
    render_board();
    render_left_side();
    render_messages();
    window.display();
}

void Gui::input()
{
    sf::Event e;
    while(window.pollEvent(e))
    {
        auto mp = sf::Mouse::getPosition(window);
        mouse_position = { (float)mp.x, (float)mp.y };
        if(e.type == sf::Event::Closed)
        {
            game->running = false;
        }
        else if(e.type == sf::Event::MouseButtonPressed)
        {
            if(e.mouseButton.button == 0)
            {
                handle_left_click();
            }
            else if(e.mouseButton.button == 1)
            {
                handle_right_click();
            }
        }       
        else if(e.type == sf::Event::MouseButtonReleased)
        {
            if(e.mouseButton.button == 0)
            {
                handle_left_release();
            }
            else if(e.mouseButton.button == 1)
            {
                handle_right_release();
            }
        }
        else if(e.type == sf::Event::KeyPressed)
        {
            if(e.key.code == sf::Keyboard::Enter)
            {
                if(current_button)
                {
                    current_button->on_ok(this);
                }
            }
            if(e.key.code == sf::Keyboard::Right || e.key.code == sf::Keyboard::D)
            {
                current_selected_cell.x = (current_selected_cell.x + 1) % game->board.size();
            }
            else if(current_selected_cell.x != -1)
            {
                if(e.key.code == sf::Keyboard::Left || e.key.code == sf::Keyboard::A)
                {
                    current_selected_cell.x = (current_selected_cell.x - 1) % game->board.size();
                }
                else if(e.key.code == sf::Keyboard::Up || e.key.code == sf::Keyboard::W)
                {
                    current_selected_cell.y = (current_selected_cell.y - 1) % game->board.size();
                }
                else if(e.key.code == sf::Keyboard::Down || e.key.code == sf::Keyboard::S)
                {
                    current_selected_cell.y = (current_selected_cell.y + 1) % game->board.size();
                }
                else if(e.key.code == sf::Keyboard::BackSpace || e.key.code == sf::Keyboard::Delete)
                {
                    game->board[current_selected_cell.y][current_selected_cell.x] = 0;
                }
            }
        }
        else if(e.type == sf::Event::TextEntered)
        {
            int n = game->board.size();
            auto c = e.text.unicode;
            int i_c = c - '0';
            std::string i_s = std::to_string(i_c);
            if(c < '0' || c > '9') continue;
            if(current_button)
            {
                if(current_button->id == 1)
                {
                    auto s = current_button->ok_str;
                    auto res = s + i_s;
                    if((!res.empty() && res[0] == '0') || (res != "10" && res != "100" && !game->is_valid_board_size(std::stoi(res)))) 
                    {
                        current_button->set_ok(i_s);
                    }
                    else current_button->set_ok(res);
                }
                else if(current_button->id == 3)
                {
                    if(i_c >= 1 && i_c <= 5)
                    {
                        current_button->set_ok(i_s);
                    }
                }
            }
            else if(i_c <= n && current_selected_cell.x != -1 && !game->from_puzzle[current_selected_cell.y][current_selected_cell.x])
            {
                auto current = game->board[current_selected_cell.y][current_selected_cell.x];
                int results = current * 10 + i_c;
                if(results > n) game->board[current_selected_cell.y][current_selected_cell.x] = i_c;
                else 
                {
                    game->board[current_selected_cell.y][current_selected_cell.x] = results;
                }
            }

        }
    }
}

void Gui::render_board()
{
    static sf::RectangleShape rect({1, 1});
    static sf::Color light(244, 244, 244);
    static sf::Color dark(233, 233, 233);
    
    static sf::RectangleShape subgrid_h_line({1, 3}); 
    static sf::RectangleShape subgrid_v_line({3, 1});

    rect.setOutlineColor({192, 192, 192});
    rect.setOutlineThickness(1);
    subgrid_h_line.setFillColor({64, 64, 64}); 
    subgrid_v_line.setFillColor({64, 64, 64});

    static sf::Text text("0", font);

    if ((int)rect.getGlobalBounds().getSize().x != (int)square_size)
    {
        rect.setSize({square_size, square_size});
        subgrid_h_line.setSize({square_size * game->board.size(), 2});
        subgrid_v_line.setSize({2, square_size * game->board.size()});
        text.setCharacterSize(square_size * 0.5f);
    }

    int n = game->board.size();
    int subgrid = game->subgrid_size;
    bool subgrid_toggle = false;

    for (int row = 0; row < n; ++row)
    {
        bool row_subgrid_toggle = subgrid_toggle;

        for (int col = 0; col < n; ++col)
        {
            int c = game->board[row][col];
            float x = col * square_size;
            float y = row * square_size;
            text.setString(c > 0 ? std::to_string(c) : "");
            if(game->from_puzzle[row][col]) text.setFillColor({0, 0, 255});
            else text.setFillColor({0, 0, 0});
            center_text(text, {x + square_size / 2.0f, y + square_size / 2.0f});

            rect.setPosition(x, y);
            rect.setFillColor(row_subgrid_toggle ? light : dark);
            window.draw(rect);

            if (c)window.draw(text);

            row_subgrid_toggle = !row_subgrid_toggle;
        }

        subgrid_toggle = !subgrid_toggle;
    }

    for (int i = 1; i < n / subgrid; ++i)
    {
        float pos = i * subgrid * square_size;

        subgrid_v_line.setPosition(pos, 0);
        window.draw(subgrid_v_line);

        subgrid_h_line.setPosition(0, pos);
        window.draw(subgrid_h_line);
    }
    
    if(current_selected_cell.x != -1)
    {
        rect.setFillColor({10, 10, 200, 40});
        rect.setOutlineThickness(0);
        rect.setPosition(current_selected_cell.x * square_size, current_selected_cell.y * square_size);
        window.draw(rect);
    }

    sf::Vector2i highlight_square = {(int)mouse_position.x / (int)square_size, (int)mouse_position.y / (int)square_size};
    if(highlight_square != current_selected_cell && game->in_range(highlight_square.x) && game->in_range(highlight_square.y))
    {
        rect.setFillColor({10, 10, 200, 20});
        rect.setOutlineThickness(0);
        rect.setPosition(highlight_square.x * square_size, highlight_square.y * square_size);
        window.draw(rect);
    }
}

void Gui::render_left_side()
{
    if(current_button) current_button->high_light = true;
    for(auto& button: buttons)
    {
        button.draw(window);
    }
    if(current_button) current_button->high_light = false;
}

void Gui::render_messages()
{
    sf::Text text("", font);
    filter_messages();
    for(auto& m: messages)
    {
        // auto time_left = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m.time_start).count();
        // float progress = std::clamp(static_cast<float>(time_left) / m.milliseconds, 0.0f, 1.0f);

        // int a = static_cast<int>((1.0f - progress) * 255);
        // m.color.a = a;

        text.setFillColor(m.color);
        text.setOutlineThickness(4);
        text.setOutlineColor({255, 255, 255});
        text.setString(m.str);
        center_text(text, m.center);
        window.draw(text);
    }
}

void Gui::handle_button_click()
{
    bool seen = false;
    for(auto& b: buttons)
    {
        if(b.contains(mouse_position))
        {
            current_button = &b;
            seen = true;
            b.on_ok(this);
        }
        else if(b.ok_contains(mouse_position))
        {
            current_button = &b;
            seen = true;
            if(b.id != 3 && b.id != 1) b.on_ok(this);
        }
    }
    if(!seen)
    {
        if(current_button)
        {
            if(current_button->id == 1) current_button->set_ok(std::to_string(game->board.size()));
            current_button->high_light = false;
        }
        current_button = nullptr;
    }
}

void Gui::handle_left_release()
{
    handle_button_click();
}

void Gui::handle_left_click()
{
    refresh();
    sf::Vector2i clicked_square = {(int)mouse_position.x / (int)square_size, (int)mouse_position.y / (int)square_size};
    if(game->in_range(clicked_square.x) && game->in_range(clicked_square.y))
    {
        current_selected_cell = clicked_square;
    }
}

void Gui::handle_right_click()
{
    refresh();
}

void Gui::handle_right_release()
{

}

void Gui::set_game(Game *game)
{
    this->game = game;
    update_by_board_size();
}

void Gui::update_by_board_size()
{
    square_size = board_size / (float)game->board.size();
}

void Gui::pop_up_message(const std::string &str, int milliseconds, const sf::Color& color)
{
    float y_offset = board_size - board_size / 8.0f - (messages.size() * 30.0f);
    sf::Vector2f text_position(board_size / 2.0f, y_offset);
    messages.push_back({str, text_position, color, std::chrono::steady_clock::now(), milliseconds});
}

void Gui::filter_messages()
{
    auto now = std::chrono::steady_clock::now();

    messages.erase(
        std::remove_if(messages.begin(), messages.end(),
            [now](const Massage &message)
            {
                return std::chrono::duration_cast<std::chrono::milliseconds>(
                           now - message.time_start)
                           .count() > message.milliseconds;
            }),
        messages.end());

    float y_offset = board_size - board_size / 8.0f;
    for (auto &message : messages)
    {
        y_offset -= 30.0f;
        message.center = {board_size / 2.0f, y_offset};
    }
}

OkButton& Gui::add_button(const std::string &str, const std::string &ok_str, int type)
{
    static float y = 0;
    static float x = (float)board_size + 60;
    y += 80;
    OkButton b({x, y});
    b.set_str(str);
    b.set_ok(ok_str);
    b.id = type;
    buttons.push_back(b);
    return buttons[buttons.size() - 1];
}

void Gui::refresh()
{
    current_button = nullptr;
    current_selected_cell.x = -1;
}

int number_of_clues(int level, int board_size)  
{  
    int total_cells = board_size * board_size;  
    int min_clues = total_cells / 4 + 1;  

    level = std::max(1, std::min(level, 5));

    double clue_percentages[5] = { 0.85, 0.75, 0.60, 0.45, 0.25 };

    return (int)std::max(min_clues, static_cast<int>(total_cells * clue_percentages[level - 1]));
}

void Gui::initialize_buttons()
{
    auto& clear = add_button("Clear", "-->", 0);
    clear.set_on_click([](OkButton& self, Gui* gui) 
    { 
        gui->game->clear_board(); 
        gui->refresh(); 
    });

    auto& resize = add_button("Resize", std::to_string(game->board.size()), 1);
    resize.set_on_click([](OkButton& self, Gui* gui)
    {
        for(auto c: self.ok_str) 
        {
            if(c < '0' || c > '9')
            {
                self.set_str(std::to_string(gui->game->board.size()));
                return;
            }
        }
        int new_size = std::stoi(self.ok_str);
        if(new_size == (int)gui->game->board.size()) return;
        gui->game->change_board_size_if_valid(new_size);
        self.set_ok(std::to_string(new_size));
        gui->refresh();
    }
    );

    auto& solver = add_button("Solve", "-->", 2);
    solver.set_on_click([](OkButton& self, Gui* gui)
    {
        gui->refresh();
        Game* game = gui->game;
        if(game->is_solution_known)
        {
            game->copy_board(game->known_solution, game->board);
        }
        else gui->game->solve_board();
    });

    auto& puzzle = add_button("Puzzle", "1-5", 3);
    puzzle.set_on_click([](OkButton& self, Gui* gui)
    {
        int n = gui->game->board.size();
        if(n > MAX_SOLVE_SIZE)
        {
            gui->pop_up_message("AI support end at size " + std::to_string(MAX_SOLVE_SIZE));
            return;
        }
        if(self.ok_str.empty()) return;
        for(auto c: self.ok_str) 
        {
            if(c < '0' || c > '9')
            {
                self.set_ok("");
                return;
            }
        }
        gui->pop_up_message("Generating Puzzle...", 5);
        gui->render();
        int l = std::stoi(self.ok_str);
        gui->game->board = generator::SudokuGenerator(n).generate_puzzle(number_of_clues(l, n), gui->game->known_solution);
        gui->game->is_solution_known = true;
        gui->game->set_from_puzzle_as_state();
        self.set_ok("1-5");
        gui->refresh();
    });

    auto& reset = add_button("Reset", "-->", 4);
    reset.set_on_click([](OkButton& self, Gui* gui)
    {
        gui->refresh();
        int n = (int)gui->game->board.size();
        for(int row = 0; row < n; ++row)
        {
            for(int col = 0; col < n; ++col)
            {
                if(!gui->game->from_puzzle[row][col])
                {
                    gui->game->board[row][col] = 0;
                }
            }
        }
    });

    auto& submit = add_button("Submit", "-->", 5);
    submit.set_on_click([](OkButton& self, Gui* gui)
    {
        gui->refresh();
        if(gui->game->is_solved())
        {
            gui->pop_up_message("Board is solved!", 2000, {0, 144, 0});
        }
        else
        {
            gui->pop_up_message("Board is not solved yet!", 2000, {255, 0, 0});
        }
    });
}

void Gui::center_text(sf::Text& text, sf::Vector2f center)
{
    auto text_bounds = text.getLocalBounds();
    text.setOrigin(text_bounds.left + text_bounds.width / 2.0f, text_bounds.top + text_bounds.height / 2.0f);
    text.setPosition(center.x, center.y);
}

OkButton::OkButton(sf::Vector2f position): 
    position(position)
{
}

void OkButton::set_str(const std::string &text)
{
    str = text;
}

void OkButton::set_ok(const std::string &text)
{
    ok_str = text;
}

void OkButton::draw(sf::RenderWindow &window)
{
    static sf::Text text("0", font, 20);
    static sf::RectangleShape rect(rect_size);
    rect.setFillColor({0, 0, 192, 40});
    text.setFillColor({255, 255, 255});

    rect.setOutlineColor({255, 255, 255, 100});
    rect.setOutlineThickness(high_light ? 2 : 0);

    text.setString(ok_str);
    Gui::center_text(text, {position.x +  rect_size.x / 2.0f, position.y + rect_size.y / 2.0f});
    rect.setPosition(position);

    window.draw(rect);
    window.draw(text);

    text.setString(str);

    sf::Vector2f next_position = {position.x + rect_size.x * 1.5f, position.y};
    Gui::center_text(text, {next_position.x + rect_size.x / 2.0f, next_position.y + rect_size.y / 2.0f});
    rect.setPosition(next_position);


    window.draw(rect);
    window.draw(text);
}

bool OkButton::contains(sf::Vector2f point)
{
    sf::Vector2f next_position = {position.x + rect_size.x * 1.5f, position.y};
    return point.x >= next_position.x && point.x < next_position.x + rect_size.x &&
           point.y >= next_position.y && point.y < next_position.y + rect_size.y;
    
}

bool OkButton::ok_contains(sf::Vector2f point)
{
    return point.x >= position.x && point.x < position.x + rect_size.x &&
           point.y >= position.y && point.y < position.y + rect_size.y;
}

void OkButton::on_ok(Gui* gui)
{
    if(on_click_function) on_click_function(*this, gui);
}

void OkButton::set_on_click(void (*func)(OkButton& self, Gui* gui))
{
    on_click_function = func;
}

sf::Font OkButton::font; 
sf::Vector2f OkButton::rect_size = {70, 40};