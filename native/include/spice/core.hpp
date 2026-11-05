#pragma once
// Viewport/geometry subset of Spice Home C++, Copyright (c) 2026 Spice Home contributors.
// Distributed under the MIT license; see the bundled source provenance notice.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace spice {
constexpr float DesignWidth = 960.0f;
constexpr float DesignHeight = 540.0f;
struct Point { float x = 0, y = 0; };
struct Rect {
  float x = 0, y = 0, w = 0, h = 0;
  bool contains(Point p) const {
    return p.x >= x && p.y >= y && p.x < x + w && p.y < y + h;
  }
  Point center() const { return {x + w * .5f, y + h * .5f}; }
};
struct Viewport {
  int window_w = 960, window_h = 540, drawable_w = 960, drawable_h = 540;
  float scale = 1, left = 0, top = 0;
  static Viewport make(int ww, int wh, int dw, int dh);
  std::optional<Point> pointer(float x, float y) const;
};
}
