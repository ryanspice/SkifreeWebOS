#pragma once
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
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

struct Entry {
  int value = 0;
  std::string initials = "---";
  Entry() = default;
  Entry(int score, std::string name = "---") : value(score), initials(std::move(name)) {}
  bool operator==(const Entry&) const = default;
};
inline bool validInitials(const std::string& value) {
  if (value == "---") return true; // An imported score with no recorded initials.
  if (value.size() != 3) return false;
  for (char c : value) if (c < 'A' || c > 'Z') return false;
  return true;
}
inline Placement submit(std::vector<Entry>& table, int value, bool timed) {
  std::vector<int> values;
  for (const auto& entry : table) values.push_back(entry.value);
  const auto result = submit(values, value, timed);
  if (result.qualified()) {
    table.insert(table.begin() + static_cast<std::ptrdiff_t>(result.rank - 1), Entry{value, "AAA"});
    if (table.size() > Limit) table.resize(Limit);
  }
  return result;
}
}
