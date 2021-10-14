#include "lyrics_parser.h"

#include <bs/utils/logger.h>

#include <extern/beatsaber-hook/shared/utils/utils.h> // CRASH_UNLESS
#include <nlohmann/json.hpp>

#include <vector>

std::optional<Lyrics> parseLyrics(std::string_view json) {
    try {
        std::vector<nlohmann::json> subtitle_list = nlohmann::json::parse(json).at("message").at("body").at("macro_calls").at("track.subtitles.get").at("message").at("body").at("subtitle_list");
        CRASH_UNLESS(!subtitle_list.empty()); // TODO: return error?

        const std::string body = subtitle_list.at(0).at("subtitle").at("subtitle_body");
        const std::vector<nlohmann::json> subtitles = nlohmann::json::parse(body);

        Lyrics result;
        result.subtitles.reserve(subtitles.size());

        for (auto&& subtitle : subtitles) {
            result.subtitles.emplace_back(Subtitle{
                .time = subtitle.at("time").at("total"),
                .text = subtitle.at("text")
            });
        }

        return result;
    }
    catch (std::exception& e) {
        getLogger().warning("unable to parse JSON: %s", e.what());

        return std::nullopt;
    }
}
