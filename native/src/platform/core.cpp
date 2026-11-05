#include "spice/core.hpp"

namespace spice {
Viewport Viewport::make(int ww, int wh, int dw, int dh) {
  Viewport v;
  v.window_w = std::max(1, ww);
  v.window_h = std::max(1, wh);
  v.drawable_w = std::max(1, dw);
  v.drawable_h = std::max(1, dh);
  v.scale = std::min(v.drawable_w / DesignWidth, v.drawable_h / DesignHeight);
  v.left = (v.drawable_w - DesignWidth * v.scale) * .5f;
  v.top = (v.drawable_h - DesignHeight * v.scale) * .5f;
  return v;
}
std::optional<Point> Viewport::pointer(float x, float y) const {
  const Point p{(x * drawable_w / window_w - left) / scale,
                (y * drawable_h / window_h - top) / scale};
  if (p.x < 0 || p.y < 0 || p.x >= DesignWidth || p.y >= DesignHeight) return std::nullopt;
  return p;
}
}
