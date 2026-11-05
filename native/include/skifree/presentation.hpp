#pragma once
#include "skifree/classic.hpp"

namespace ski::presentation {
// The opening scene is an intentional presentation exception. Once its credit
// scrolls away, lift sprites use their original world positions again. Only
// drawing is offset; the simulation and collisions are never moved.
constexpr int IntroLiftOffset = -64;
inline bool liftSprite(int sprite) { return sprite >= 64 && sprite <= 67; }
inline int liftOffset(bool intro) { return intro ? IntroLiftOffset : 0; }
inline classic::Draw originalSprite(classic::Draw draw, bool intro) {
    if (liftSprite(draw.sprite)) draw.x += liftOffset(intro);
    return draw;
}
}
