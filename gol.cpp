#include <cairomm/context.h>
#include <glibmm/main.h>
#include <gtkmm/application.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/window.h>

#include <cmath>
#include <ctime>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <utility>

constexpr int WINDOW_WIDTH = 600;
constexpr int WINDOW_HEIGHT = 400;

using Cell = std::pair<int, int>;

class GameOfLife {
 public:
  GameOfLife(int width, int height);

  void set_cell(int x, int y, bool val);
  bool get_cell(int x, int y);

  int width() { return m_width; }
  int height() { return m_height; }
  void step();

 private:
  int m_width;
  int m_height;
  std::set<Cell> m_cells;
};

GameOfLife::GameOfLife(int width, int height)
    : m_width(width), m_height(height) {}

void GameOfLife::set_cell(int x, int y, bool val) {
  if (val) {
    m_cells.insert(std::make_pair(x, y));
  } else {
    m_cells.erase(std::make_pair(x, y));
  }
}

bool GameOfLife::get_cell(int x, int y) {
  return m_cells.find(std::make_pair(x, y)) != m_cells.end();
}

void GameOfLife::step() {
  std::map<Cell, int> neighbor_count;

  for (const auto& [x, y] : m_cells) {
    for (int xd = -1; xd <= 1; xd++) {
      for (int yd = -1; yd <= 1; yd++) {
        if (xd == 0 && yd == 0) continue;
        neighbor_count[std::make_pair(x + xd, y + yd)] += 1;
      }
    }
  }

  std::set<Cell> new_cells;
  for (const auto& [cell, count] : neighbor_count) {
    const bool alive = m_cells.find(cell) != m_cells.end();
    if (alive && (count == 2 || count == 3)) {
      new_cells.insert(cell);
    }
    if (!alive && count == 3) {
      new_cells.insert(cell);
    }
  }
  m_cells = new_cells;
}

class GameOfLifeVisual : public Gtk::DrawingArea {
 public:
  GameOfLifeVisual(GameOfLife game_of_life);
  virtual ~GameOfLifeVisual() = default;

 protected:
  bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
  bool on_timeout();

  GameOfLife m_game_of_life;
};

GameOfLifeVisual::GameOfLifeVisual(GameOfLife game_of_life)
    : m_game_of_life(game_of_life) {
  Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &GameOfLifeVisual::on_timeout), 30);
}

bool GameOfLifeVisual::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
  // Make each sequare 1x1
  cr->scale(WINDOW_WIDTH / m_game_of_life.width(),
            WINDOW_HEIGHT / m_game_of_life.height());

  cr->save();
  cr->set_source_rgba(1.0, 1.0, 1.0, 0.7);
  cr->paint();
  cr->restore();

  cr->save();
  cr->set_source_rgba(0.0, 0.5, 0.0, 1.0);
  for (int x = 0; x < m_game_of_life.width(); x++) {
    for (int y = 0; y < m_game_of_life.height(); y++) {
      if (m_game_of_life.get_cell(x, y)) {
        cr->rectangle(x, y, 1.0, 1.0);
        cr->fill();
      }
    }
  }
  cr->restore();

  return true;
}

bool GameOfLifeVisual::on_timeout() {
  m_game_of_life.step();
  queue_draw();
  return true;
}

class MyWindow : public Gtk::Window {
 public:
  MyWindow(GameOfLife game_of_life);
  virtual ~MyWindow() = default;

 protected:
  GameOfLifeVisual m_game_of_life_vis;
};

MyWindow::MyWindow(GameOfLife game_of_life) : m_game_of_life_vis(game_of_life) {
  set_title("Conway's Game of Life");
  set_default_size(WINDOW_WIDTH, WINDOW_HEIGHT);

  add(m_game_of_life_vis);
  m_game_of_life_vis.show();
}

int main(int argc, char** argv) {
  auto app = Gtk::Application::create("org.gtkmm.examples.base");

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 1);

  const int game_width = 60;
  const int game_height = 40;

  GameOfLife game_of_life(game_width, game_height);
  for (int x = 0; x < game_width; x++) {
    for (int y = 0; y < game_height; y++) {
      if (distrib(gen)) {
        game_of_life.set_cell(x, y, true);
      }
    }
  }

  MyWindow my_window(game_of_life);

  return app->run(my_window);
}
