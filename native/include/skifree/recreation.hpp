#pragma once
#include <cstdint>
#include <random>
#include <vector>

namespace ski::recreation {
constexpr double Step = 0.02;
enum class Mode { Title, Playing, Paused, GameOver };
enum class Pose { Skiing, Crashed, Jumping, Eaten };
enum class Kind { SmallTree, LargeTree, Rock, Ramp, Snow, Boarder };
struct Obstacle {
  Kind kind = Kind::SmallTree;
  double x = 0, y = 0;
  bool hit = false;
};
struct Player {
  double x = 0, y = 0, speed = 0;
  int heading = 0;
  Pose pose = Pose::Skiing;
  double poseTime = 0, boostTime = 0, cooldown = 0;
  bool trick = false;
};
struct Yeti {
  bool active = false;
  double x = 0, y = 0, time = 0;
};
class Game {
 public:
  void start(std::uint32_t seed, bool chase = false);
  void advance(double seconds);
  void pause();
  void resume();
  void title();
  void turn(int direction);
  void downhill();
  void brake();
  void action();
  void point(double horizontal, double vertical);
  void place(Obstacle obstacle); // Also used by deterministic course fixtures.
  Mode mode() const { return mode_; }
  const Player& player() const { return player_; }
  const Yeti& yeti() const { return yeti_; }
  const std::vector<Obstacle>& terrain() const { return terrain_; }
  double distance() const { return player_.y / 10.0; }
  double elapsed() const { return elapsed_; }
  int score() const { return score_; }
  int crashes() const { return crashes_; }
  int jumps() const { return jumps_; }
  std::uint32_t seed() const { return seed_; }
 private:
#ifdef SKIFREE_RECREATION_TESTING
  friend struct GameFixture;
#endif
  void tick();
  void populate();
  void collide(Obstacle& object);
  Mode mode_ = Mode::Title;
  Player player_;
  Yeti yeti_;
  std::vector<Obstacle> terrain_;
  std::mt19937 random_{1};
  std::uint32_t seed_ = 1;
  double accumulator_ = 0, elapsed_ = 0, nextRow_ = 360, yetiAt_ = 2000;
  double yetiDelay_ = 0, recoveryTime_ = 0;
  int score_ = 0, crashes_ = 0, jumps_ = 0;
};
}

namespace spice { class Renderer; }
namespace ski::recreation { void landscape(spice::Renderer&,const Game&); }
