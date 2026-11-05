#pragma once
#include <array>
#include <filesystem>
#include <string>

namespace ski {
// Exact filenames used by the renderer and the bundled asset inventory.
inline constexpr std::array<const char*, 89> RequiredSprites{
  "ski32_001",
  "ski32_002",
  "ski32_003",
  "ski32_004",
  "ski32_005",
  "ski32_006",
  "ski32_007",
  "ski32_008",
  "ski32_009",
  "ski32_010",
  "ski32_011",
  "ski32_012",
  "ski32_013",
  "ski32_014",
  "ski32_015",
  "ski32_016",
  "ski32_017",
  "ski32_018",
  "ski32_019",
  "ski32_020",
  "ski32_021",
  "ski32_022",
  "ski32_023",
  "ski32_024",
  "ski32_025",
  "ski32_026",
  "ski32_027",
  "ski32_028",
  "ski32_029",
  "ski32_030",
  "ski32_031",
  "ski32_032",
  "ski32_033",
  "ski32_034",
  "ski32_035",
  "ski32_036",
  "ski32_037",
  "ski32_038",
  "ski32_039",
  "ski32_040",
  "ski32_041",
  "ski32_042",
  "ski32_043",
  "ski32_044",
  "ski32_045",
  "ski32_046",
  "ski32_047",
  "ski32_048",
  "ski32_049",
  "ski32_050",
  "ski32_051",
  "ski32_052",
  "ski32_053",
  "ski32_054",
  "ski32_055",
  "ski32_056",
  "ski32_057",
  "ski32_058",
  "ski32_059",
  "ski32_060",
  "ski32_061",
  "ski32_062",
  "ski32_063",
  "ski32_064",
  "ski32_065",
  "ski32_066",
  "ski32_067",
  "ski32_068",
  "ski32_069",
  "ski32_070",
  "ski32_071",
  "ski32_072",
  "ski32_073",
  "ski32_074",
  "ski32_075",
  "ski32_076",
  "ski32_077",
  "ski32_078",
  "ski32_079",
  "ski32_080",
  "ski32_081",
  "ski32_082",
  "ski32_083",
  "ski32_084",
  "ski32_085",
  "ski32_086",
  "ski32_087",
  "ski32_088",
  "ski32_089"
};
inline std::string checkAssetFiles(const std::filesystem::path& directory) {
  if (directory.empty()) return "No game asset directory is available.";
  std::error_code error;
  if (!std::filesystem::is_directory(directory, error)) return "Game files are missing. Reinstall the application.";
  for (const auto* name : RequiredSprites) {
    const auto file = directory / (std::string(name) + ".png");
    error.clear();
    if (!std::filesystem::is_regular_file(file, error)) return "Game files are incomplete. Missing " + std::string(name) + ".png";
    const auto size = std::filesystem::file_size(file, error);
    if (error || size == 0 || size > 1024 * 1024) return "Game files contain an invalid sprite file.";
  }
  return {};
}
}
