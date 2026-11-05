#pragma once
#include "skifree/scores.hpp"
#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <vector>
#include <utility>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace ski::scores {
using Tables = std::array<std::vector<Entry>, 3>;
enum class StoreStatus { Ok, Missing, Disabled, Invalid, IoError, OrphanTemp, WriteError, ReplaceError };

// A store is writable only after a successful load or a confirmed missing file.
// Named scores use a new file. An optional legacy file is read only if the
// primary file is absent; neither legacy scores nor record.txt are overwritten.
class Store {
 public:
  explicit Store(std::filesystem::path path, std::filesystem::path legacy = {},
                 std::filesystem::path olderLegacy = {})
      : path_(std::move(path)), legacy_(std::move(legacy)), olderLegacy_(std::move(olderLegacy)) {}
  StoreStatus load(Tables& tables) {
    writable_ = false;
    if (path_.empty()) return StoreStatus::Disabled;
    std::error_code ec;
    const bool orphan = std::filesystem::exists(temp(), ec);
    if (ec) return StoreStatus::IoError;
    if (orphan) return StoreStatus::OrphanTemp;
    const bool exists = std::filesystem::exists(path_, ec);
    if (ec) return StoreStatus::IoError;
    if (!exists) {
      for (const auto& source : {legacy_, olderLegacy_}) {
        if (source.empty()) continue;
        Store legacy(source);
        Tables imported;
        const auto status = legacy.load(imported);
        if (status == StoreStatus::Missing) continue;
        if (status != StoreStatus::Ok) return status;
        tables = std::move(imported); writable_ = true; return StoreStatus::Ok;
      }
      tables = {}; writable_ = true; return StoreStatus::Missing;
    }
    std::ifstream in(path_);
    if (!in) return StoreStatus::IoError;
    Tables parsed;
    std::string tag;
    if (!(in >> tag) || (tag != "skifree-v104" && tag != "skifree-initials-v1")) return StoreStatus::Invalid;
    for (;;) {
      in >> std::ws;
      if (in.bad()) return StoreStatus::IoError;
      if (in.eof()) break;
      int course = 0, value = 0;
      if (!(in >> course >> value)) return in.bad() ? StoreStatus::IoError : StoreStatus::Invalid;
      std::string initials = "---";
      if (tag == "skifree-initials-v1" && !(in >> initials)) return in.bad() ? StoreStatus::IoError : StoreStatus::Invalid;
      if (!validInitials(initials)) return StoreStatus::Invalid;
      if (course < 0 || course > 2 || parsed[static_cast<std::size_t>(course)].size() >= Limit)
        return StoreStatus::Invalid;
      parsed[static_cast<std::size_t>(course)].emplace_back(value, initials);
    }
    if (!valid(parsed)) return StoreStatus::Invalid;
    tables = std::move(parsed);
    writable_ = true;
    return StoreStatus::Ok;
  }
  StoreStatus save(const Tables& tables) {
    if (!writable_) return StoreStatus::Disabled;
    if (!valid(tables)) { writable_ = false; return StoreStatus::Invalid; }
    std::error_code ec;
    if (std::filesystem::exists(temp(), ec)) { writable_ = false; return StoreStatus::OrphanTemp; }
    if (ec) { writable_ = false; return StoreStatus::IoError; }
    if (!path_.parent_path().empty()) std::filesystem::create_directories(path_.parent_path(), ec);
    if (ec) { writable_ = false; return StoreStatus::WriteError; }
    {
      std::ofstream out(temp(), std::ios::trunc);
      if (out) {
        out << "skifree-initials-v1\n";
        for (std::size_t c = 0; c < tables.size(); ++c)
          for (const auto& entry : tables[c]) out << c << ' ' << entry.value << ' ' << entry.initials << '\n';
        out.close();
      }
      if (!out) { writable_ = false; return StoreStatus::WriteError; }
    }
#ifdef _WIN32
    if (!MoveFileExW(temp().c_str(), path_.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
      writable_ = false; return StoreStatus::ReplaceError;
    }
#else
    std::filesystem::rename(temp(), path_, ec);
    if (ec) { writable_ = false; return StoreStatus::ReplaceError; }
#endif
    return StoreStatus::Ok;
  }
 private:
  static bool valid(const Tables& tables) {
    for (std::size_t c = 0; c < tables.size(); ++c) {
      if (tables[c].size() > Limit) return false;
      for (const auto& entry : tables[c]) if (!validInitials(entry.initials)) return false;
      if (!std::is_sorted(tables[c].begin(), tables[c].end(), [c](const Entry& a, const Entry& b) {
            return c < 2 ? a.value < b.value : a.value > b.value;
          })) return false;
    }
    return true;
  }
  std::filesystem::path temp() const { auto result = path_; result += ".tmp"; return result; }
  std::filesystem::path path_;
  std::filesystem::path legacy_;
  std::filesystem::path olderLegacy_;
  bool writable_ = false;
};
}
