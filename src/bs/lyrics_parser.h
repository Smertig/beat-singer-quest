#pragma once

#include <bs/utils/lyrics.h>

#include <optional>
#include <string_view>

std::optional<Lyrics> parseLyrics(std::string_view json);
