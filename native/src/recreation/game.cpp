// Recreation gameplay. Its gentler pursuit is independent of the Original core.
#include "skifree/recreation.hpp"
#include <algorithm>
#include <cmath>

namespace ski::recreation {
void Game::start(std::uint32_t seed, bool chase) {
  seed_ = seed;
  random_.seed(seed);
  player_ = {};
  yeti_ = {};
  terrain_.clear();
  accumulator_ = elapsed_ = 0;
  nextRow_ = 360;
  score_ = crashes_ = jumps_ = 0;
  yetiAt_ = chase ? 0 : 2000;
  yetiDelay_ = chase ? 3 : 0;
  recoveryTime_ = 0;
  mode_ = Mode::Playing;
  populate();
}
void Game::pause() { if (mode_ == Mode::Playing) { mode_ = Mode::Paused; accumulator_ = 0; } }
void Game::resume() { if (mode_ == Mode::Paused) { mode_ = Mode::Playing; accumulator_ = 0; } }
void Game::title() { mode_ = Mode::Title; accumulator_ = 0; }
void Game::turn(int direction) {
  if (mode_ != Mode::Playing || player_.pose == Pose::Crashed || player_.pose == Pose::Eaten) return;
  if (std::abs(player_.heading) == 3 && player_.heading * direction > 0) player_.x += direction * 18;
  else player_.heading = std::clamp(player_.heading + direction, -3, 3);
}
void Game::downhill() { if (mode_ == Mode::Playing && player_.pose == Pose::Skiing) player_.heading = 0; }
void Game::brake() {
  if (mode_ == Mode::Playing && player_.pose == Pose::Skiing) player_.heading = player_.heading < 0 ? -3 : 3;
}
void Game::point(double horizontal, double vertical) {
  if (mode_ != Mode::Playing || player_.pose != Pose::Skiing) return;
  if (vertical < -25) { brake(); return; }
  player_.heading = horizontal < -150 ? -2 : horizontal < -35 ? -1 : horizontal > 150 ? 2 : horizontal > 35 ? 1 : 0;
}
void Game::action() {
  if (mode_ != Mode::Playing) return;
  if (player_.pose == Pose::Jumping) { player_.trick = true; return; }
  if (player_.pose == Pose::Skiing && player_.cooldown <= 0 && std::abs(player_.heading) != 3) {
    player_.boostTime = 2;
    player_.cooldown = 12;
  }
}
void Game::place(Obstacle object) {
  if (terrain_.size() < 256 && std::isfinite(object.x) && std::isfinite(object.y)) terrain_.push_back(object);
}
void Game::advance(double seconds) {
  if (mode_ != Mode::Playing || !std::isfinite(seconds) || seconds <= 0) return;
  accumulator_ += std::min(seconds, 0.2);
  while (accumulator_ + 1e-9 >= Step && mode_ == Mode::Playing) {
    accumulator_ -= Step;
    tick();
  }
}
void Game::populate() {
  while (nextRow_ < player_.y + 850) {
    for (int i = 0; i < 4; ++i) {
      const unsigned r = random_();
      const unsigned type = r % 16;
      Kind kind = type < 6 ? Kind::SmallTree : type < 9 ? Kind::LargeTree : type < 11 ? Kind::Rock : type < 14 ? Kind::Ramp : type == 14 ? Kind::Snow : Kind::Boarder;
      const double x = player_.x - 670 + static_cast<double>(random_() % 1340);
      const double y = nextRow_ + static_cast<double>(random_() % 90);
      place({kind, x, y, false});
    }
    nextRow_ += 150;
  }
}
void Game::collide(Obstacle& o) {
  if (o.hit || player_.pose == Pose::Crashed || player_.pose == Pose::Eaten) return;
  const double radius = o.kind == Kind::Ramp ? 28 : o.kind == Kind::LargeTree ? 18 : 14;
  if (std::abs(o.x - player_.x) > radius + 8 || std::abs(o.y - player_.y) > 12) return;
  if (player_.pose == Pose::Jumping && o.kind != Kind::LargeTree) return;
  o.hit = true;
  if (o.kind == Kind::Snow) return;
  if (o.kind == Kind::Ramp) {
    if (player_.speed <= 0) return;
    player_.pose = Pose::Jumping;
    player_.poseTime = 1;
    player_.trick = false;
    ++jumps_;
  } else {
    player_.pose = Pose::Crashed;
    player_.poseTime = 1.3;
    // Give the player time to get moving after a fall. The yeti waits nearby.
    recoveryTime_ = player_.poseTime + 1.5;
    player_.boostTime = 0;
    player_.trick = false;
    ++crashes_;
    score_ = std::max(0, score_ - 25);
  }
}
void Game::tick() {
  elapsed_ += Step;
  recoveryTime_ = std::max(0.0, recoveryTime_ - Step);
  player_.boostTime = std::max(0.0, player_.boostTime - Step);
  player_.cooldown = std::max(0.0, player_.cooldown - Step);
  if (player_.pose != Pose::Skiing) {
    player_.poseTime -= Step;
    if (player_.poseTime <= 0) {
      if (player_.pose == Pose::Eaten) { mode_ = Mode::GameOver; return; }
      if (player_.pose == Pose::Jumping) score_ += player_.trick ? 200 : 50;
      player_.pose = Pose::Skiing;
      player_.trick = false;
    }
  }
  player_.speed = 0;
  if (player_.pose == Pose::Skiing || player_.pose == Pose::Jumping) {
    const double angle = player_.heading * 0.40;
    player_.speed = (player_.boostTime > 0 ? 460 : 230) + (player_.pose == Pose::Jumping ? 50 : 0);
    if (std::abs(player_.heading) == 3) player_.speed = 0;
    player_.x += std::sin(angle) * player_.speed * Step;
    player_.y += std::cos(angle) * player_.speed * Step;
  }
  for (auto& o : terrain_) {
    if (o.kind == Kind::Boarder) { o.x += std::sin(elapsed_ + o.y * 0.01) * 1.2; o.y += 1.7; }
    collide(o);
  }
  terrain_.erase(std::remove_if(terrain_.begin(), terrain_.end(), [&](const auto& o) {
    return o.y < player_.y - 450 || o.y > player_.y + 1300 || std::abs(o.x - player_.x) > 1800;
  }), terrain_.end());
  populate();
  if (!yeti_.active && elapsed_ >= yetiDelay_ && distance() >= yetiAt_) {
    yeti_ = {true, player_.x - 140, player_.y - 360, 0};
  }
  if (yeti_.active && player_.pose != Pose::Eaten) {
    yeti_.time += Step;
    const double dx = player_.x - yeti_.x, dy = player_.y - yeti_.y;
    const double length = std::hypot(dx, dy);
    const bool recovering = recoveryTime_ > 0;
    if (length < 18 && !recovering && player_.pose != Pose::Jumping) {
      player_.pose = Pose::Eaten;
      player_.poseTime = 1.5;
      player_.boostTime = 0;
    } else if (length > 0) {
      const double available = recovering ? std::max(0.0, length - 150) : length;
      const double move = std::min(available, (245 + std::min(15.0, yeti_.time * 0.5)) * Step);
      yeti_.x += dx / length * move;
      yeti_.y += dy / length * move;
    }
  }
}
}
