#include "common.hpp"

std::optional<std::string> LoadTextFile(const std::string &filename) {
  std::ifstream fin(filename);
  if (!fin.is_open()) {
    SPDLOG_ERROR("failed to open file: {}", filename);
    return {};
  }
  std::stringstream text;
  text << fin.rdbuf();

  return text.str();
}

std::vector<std::string> Split(const std::string &s,
                               const std::string &sep = " ") {
  size_t pos = 0;
  size_t npos = 0;
  std::vector<std::string> ret;

  if (sep.empty()) {
    return ret;
  }
  while (pos < s.size()) {
    if ((npos = s.find(sep, pos)) == std::string::npos) {
      npos = s.size();
    }
    if (npos - pos > 0) {
      ret.push_back(s.substr(pos, npos - pos));
    }
    pos = npos + sep.size();
  }

  return ret;
}