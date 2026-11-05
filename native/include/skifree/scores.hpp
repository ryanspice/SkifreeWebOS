#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace ski::scores {
inline constexpr std::size_t Limit = 10;

struct Placement {
  int value = 0;
  bool timed = false;
  // One-based rank, or zero when the current result did not enter the top ten.
  std::size_t rank = 0;
  bool qualified() const { return rank != 0; }
  bool isCurrentRow(std::size_t row) const { return qualified() && row == rank - 1; }
};

// Follows SkiFree 1.04's strict insertion comparison: existing ties stay ahead
// of the new result. The app stores positive elapsed times, while the original
// INI negates them; comparing times in ascending order preserves that ranking.
// Input is the stored, already-ranked course table (at most ten entries).
inline Placement submit(std::vector<int>& table, int value, bool timed) {
  if (table.size() > Limit) throw std::invalid_argument("Score table exceeds ten entries");
  std::size_t at = 0;
  while (at < table.size() && !(timed ? value < table[at] : value > table[at])) ++at;
  Placement result{value, timed, 0};
  if (at == Limit) return result;
  table.insert(table.begin() + static_cast<std::ptrdiff_t>(at), value);
  if (table.size() > Limit) table.resize(Limit);
  result.rank = at + 1;
  return result;
}
}
