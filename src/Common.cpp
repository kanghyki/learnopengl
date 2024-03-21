#include "Common.hpp"

std::optional<std::string> loadTextFile(const std::string& fileName)
{
    std::ifstream fin(fileName);
    if (!fin.is_open())
    {
        SPDLOG_ERROR("failed to open file: {}", fileName);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();

    return text.str();
}