#include "skifree/recreation.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace ski::recreation {
// Compiled only in the dedicated test target; no fixture access in the app.
struct GameFixture {
  static void clearCourse(Game& game) {
    game.terrain_.clear();
    game.nextRow_ = 1e9;
  }
  static void atDistance(Game& game, double metres) { game.player_.y = metres * 10; }
  static void closePursuit(Game& game) {
    game.yeti_ = {true, game.player_.x, game.player_.y - 40, 40};
  }
};
}

namespace {
using namespace ski::recreation;
int checks = 0;
void check(bool condition, const std::string& description) {
  if (!condition) throw std::runtime_error(description);
  ++checks;
  std::cout << "PASS: " << description << '\n';
}
void run(Game& game, int ticks, bool boost = false) {
  for (int i = 0; i < ticks; ++i) {
    if (boost && game.player().cooldown <= 0) game.action();
    game.advance(Step);
  }
}
Game course(bool chase) {
  Game game;
  game.start(7, chase);
  GameFixture::clearCourse(game);
  return game;
}
bool alive(const Game& game) {
  return game.mode() == Mode::Playing && game.player().pose != Pose::Eaten;
}
}

int main() {
  try {
    auto opening = course(true);
    run(opening, 149);
    check(!opening.yeti().active && opening.distance() > 60,
          "Chase gives the skier an opening before pursuit starts");
    run(opening, 2);
    check(opening.yeti().active && opening.player().y - opening.yeti().y > 350,
          "Pursuit starts with a visible opportunity to escape");

    auto downhill = course(true);
    run(downhill, 1000);
    check(alive(downhill), "Unboosted clear downhill run survives the first 20 seconds");
    run(downhill, 3000);
    check(downhill.mode() == Mode::GameOver,
          "Pursuit still catches a skier who never uses an escape action");

    auto stopped = course(true);
    stopped.brake();
    run(stopped, 750);
    check(stopped.mode() == Mode::GameOver, "Stopping indefinitely still ends the chase");

    auto escaped = course(true);
    run(escaped, 6000, true);
    check(alive(escaped) && escaped.distance() > 2500,
          "Timely boosts can sustain a two-minute clear downhill escape");

    auto crash = course(true);
    GameFixture::closePursuit(crash);
    crash.place({Kind::Rock, 0, 5, false});
    run(crash, 1);
    check(crash.player().pose == Pose::Crashed && crash.crashes() == 1,
          "Close pursuit fixture produces an actual terrain crash");
    run(crash, 50);
    check(crash.player().pose == Pose::Crashed,
          "Nearby yeti does not end the run while the skier is down");
    run(crash, 25);
    check(alive(crash) && crash.player().pose == Pose::Skiing,
          "Skier can stand up and move before pursuit can catch again");
    crash.action();
    run(crash, 250);
    check(alive(crash) && crash.distance() > 100,
          "Boost after recovery creates a real escape from close pursuit");

    auto paused = course(true);
    run(paused, 100);
    const double before = paused.elapsed();
    paused.pause();
    run(paused, 1000);
    check(paused.elapsed() == before && !paused.yeti().active,
          "Pause does not consume the opening or advance pursuit");
    paused.resume();
    run(paused, 51);
    check(paused.yeti().active, "Pursuit timing continues normally after resume");

    auto slope = course(false);
    GameFixture::atDistance(slope, 1999);
    run(slope, 2);
    check(!slope.yeti().active, "Recreation slope keeps its ordinary 2000-metre threshold");
    run(slope, 1);
    check(slope.yeti().active && alive(slope),
          "Recreation slope also gets the forgiving pursuit when threshold is crossed");

    auto restarted = course(true);
    GameFixture::closePursuit(restarted);
    restarted.place({Kind::Rock, 0, 5, false});
    run(restarted, 1);
    restarted.start(8, true);
    GameFixture::clearCourse(restarted);
    restarted.brake();
    run(restarted, 750);
    check(restarted.mode() == Mode::GameOver,
          "Restart clears prior crash protection and starts a normal chase");
    std::cout << checks << " recreation behavior checks passed\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
