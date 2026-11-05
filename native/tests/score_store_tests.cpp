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
    check(read(file) == "skifree-initials-v1\n0 10 ---\n0 10 ---\n0 20 ---\n1 30 ---\n1 40 ---\n2 100 ---\n2 0 ---\n2 -5 ---\n", "named format retains unassigned legacy initials");
    Tables loaded; Store reload(file); check(reload.load(loaded) == StoreStatus::Ok && loaded == t, "round trip and ties");
    t[0].push_back(21); check(reload.save(t) == StoreStatus::Ok, "replace existing file");
    check(read(old) == "legacy data\n", "legacy record untouched");
    t[0][0].initials="RYN";check(reload.save(t)==StoreStatus::Ok,"save named score");
    Store named(file);check(named.load(loaded)==StoreStatus::Ok&&loaded==t,"initials remain attached through round trip");
    const auto legacyFile=dir/"legacy-v104.txt", namedFile=dir/"scores-initials-v1.txt";
    const std::string legacyText="skifree-v104\n0 1000\n2 -10\n";
    put(legacyFile,legacyText);Store migrate(namedFile,legacyFile);Tables migrated;
    check(migrate.load(migrated)==StoreStatus::Ok&&migrated[0][0]==Entry{1000,"---"},"legacy table imports without fabricated initials");
    migrated[0][0].initials="ABC";check(migrate.save(migrated)==StoreStatus::Ok&&read(legacyFile)==legacyText,"migration writes new file and preserves old bytes");
    put(legacyFile,"invalid old file\n");Store preferred(namedFile,legacyFile);
    check(preferred.load(loaded)==StoreStatus::Ok&&loaded==migrated,"existing named file wins over legacy fallback");
    put(namedFile,"skifree-initials-v1\n0 1000 abC\n");Store badPrimary(namedFile,legacyFile);
    check(badPrimary.load(loaded)==StoreStatus::Invalid,"invalid initials do not fall back or silently reset");
    const std::string badNames[]={"AA","AAAA","A1B","A-A","abc"};
    for(const auto& name:badNames){put(namedFile,"skifree-initials-v1\n0 1000 "+name+"\n");Store badName(namedFile);check(badName.load(loaded)==StoreStatus::Invalid,"malformed initials rejected");}
    fs::remove(namedFile);put(legacyFile,legacyText);put(legacyFile.string()+".tmp","recovery");Store interrupted(namedFile,legacyFile);
    check(interrupted.load(loaded)==StoreStatus::OrphanTemp&&interrupted.save(migrated)==StoreStatus::Disabled&&!fs::exists(namedFile),"legacy interrupted save blocks migration without losing recovery material");
    fs::remove(legacyFile.string()+".tmp");
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
