#include "common.hpp"

std::optional<std::string> loadTextFile(const std::string& filename)
{
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();

    return text.str();
}
