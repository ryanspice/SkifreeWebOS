#include "skifree/score_store.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
using namespace ski::scores;
namespace fs = std::filesystem;
void check(bool b, const char* s) { if (!b) throw std::runtime_error(s); std::cout << "PASS: " << s << '\n'; }
void put(const fs::path& p, const std::string& s) { std::ofstream o(p); o << s; if (!o) throw std::runtime_error("fixture write"); }
std::string read(const fs::path& p) { std::ifstream i(p); return {std::istreambuf_iterator<char>(i), {}}; }
int main() {
  const auto dir = fs::temp_directory_path() / ("skifree-store-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
  try {
    fs::create_directories(dir); const auto file = dir / "scores-v104.txt"; const auto old = dir / "record.txt";
    put(old, "legacy data\n"); Tables t; Store s(file);
    check(s.load(t) == StoreStatus::Missing, "missing file permits new table");
    t = {{{10, 10, 20}, {30, 40}, {100, 0, -5}}};
    check(s.save(t) == StoreStatus::Ok, "initial save");
    check(read(file) == "skifree-v104\n0 10\n0 10\n0 20\n1 30\n1 40\n2 100\n2 0\n2 -5\n", "exact format");
    Tables loaded; Store reload(file); check(reload.load(loaded) == StoreStatus::Ok && loaded == t, "round trip and ties");
    t[0].push_back(21); check(reload.save(t) == StoreStatus::Ok, "replace existing file");
    check(read(old) == "legacy data\n", "legacy record untouched");
    const std::string invalid[] = {"bad\n", "skifree-v104\n0", "skifree-v104\n3 1\n", "skifree-v104\n0 2\n0 1\n", "skifree-v104\n2 1\n2 2\n", "skifree-v104\n0 x\n", "skifree-v104\n0 1\n0 2\n0 3\n0 4\n0 5\n0 6\n0 7\n0 8\n0 9\n0 10\n0 11\n"};
    for (const auto& fixture : invalid) {
      put(file, fixture); Store bad(file); Tables unchanged = t;
      check(bad.load(unchanged) == StoreStatus::Invalid && unchanged == t && bad.save(t) == StoreStatus::Disabled && read(file) == fixture, "invalid data preserved and writes blocked");
    }
    put(file, "skifree-v104\n"); put(file.string()+".tmp", "recovery"); Store orphan(file);
    check(orphan.load(loaded) == StoreStatus::OrphanTemp && orphan.save(t) == StoreStatus::Disabled && read(file.string()+".tmp") == "recovery", "orphan temp preserved");
    fs::remove(file.string()+".tmp"); Store replacement(file); check(replacement.load(loaded) == StoreStatus::Ok, "load before replace failure");
    fs::remove(file); fs::create_directory(file);
    check(replacement.save(t) == StoreStatus::ReplaceError && fs::is_directory(file) && fs::exists(file.string()+".tmp"), "replace failure preserves target and temp");
    const auto blocker = dir / "blocker"; Store writefail(blocker / "scores-v104.txt");
    check(writefail.load(loaded) == StoreStatus::Missing, "missing parent permits initial load");
    put(blocker, "file");
    check(writefail.save(t) == StoreStatus::WriteError && read(blocker) == "file", "write failure preserves blocking file");
    fs::remove_all(dir); return 0;
  } catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; fs::remove_all(dir); return 1; }
}
