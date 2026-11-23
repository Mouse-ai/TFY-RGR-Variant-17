#pragma once
#include <cstddef>
#include <filesystem>
#include <iosfwd>

namespace tfy {
void runTask(int n, std::ostream& out, std::size_t limit,
             const std::filesystem::path& dotDir = {});
}
