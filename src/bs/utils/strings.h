#pragma once

#include <string>
#include <string_view>

struct Il2CppString;

namespace utils {

std::string to_utf8(Il2CppString* str);

Il2CppString* to_il2cpp(std::string_view str);

} // namespace utils
