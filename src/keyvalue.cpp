#include "keyvalue.hpp"

#include <cstdio>
#include <fstream>

namespace rr4 {

namespace {
std::string trim(const std::string &s) {
    size_t a = s.find_first_not_of(" \t\r");
    size_t b = s.find_last_not_of(" \t\r");
    return a == std::string::npos ? std::string() : s.substr(a, b - a + 1);
}
} // namespace

bool readKeyValues(const std::filesystem::path &path, KeyValues &out) {
    std::ifstream in(path);
    if (!in) return false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        out[trim(line.substr(0, eq))] = trim(line.substr(eq + 1));
    }
    return true;
}

bool writeFileAtomic(const std::filesystem::path &path, const std::string &text) {
    std::filesystem::path tmp = path;
    tmp += ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out || !(out << text) || !out.flush()) return false;
    }
    std::error_code ec;
    std::filesystem::rename(tmp, path, ec);
    if (ec) {
        std::fprintf(stderr, "cannot write %s: %s\n", path.string().c_str(), ec.message().c_str());
        return false;
    }
    return true;
}

std::filesystem::path userDataDir() {
#ifdef __vita__
    // app0: (the installed package) is read-only.
    const std::filesystem::path dir = "ux0:data/RedlineRumble4";
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return dir;
#else
    std::error_code ec;
    std::filesystem::path exe = std::filesystem::read_symlink("/proc/self/exe", ec);
    return ec ? std::filesystem::current_path() : exe.parent_path();
#endif
}

} // namespace rr4
