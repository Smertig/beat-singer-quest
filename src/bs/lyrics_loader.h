#pragma once

#include <string_view>
#include <functional>
#include <optional>

void loadLyrics(std::string_view song, std::string_view artist, std::function<void(std::optional<std::string>)> callback);
