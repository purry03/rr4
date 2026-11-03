#pragma once
// The "key = value" text format of rr4save.txt and rr4settings.txt: one
// entry per line, '#' comments, unknown keys ignored by the readers.

#include <filesystem>
#include <map>
#include <string>

namespace rr4 {

using KeyValues = std::map<std::string, std::string>;

// False when the file cannot be opened.
bool readKeyValues(const std::filesystem::path &path, KeyValues &out);
// Writes `text` to a temporary file and renames it over `path`, so a crash
// never leaves a half-written file.
bool writeFileAtomic(const std::filesystem::path &path, const std::string &text);
// Where the save and settings files live: next to the executable on the
// desktop, ux0:data/RedlineRumble4 on the PS Vita.
std::filesystem::path userDataDir();

} // namespace rr4
