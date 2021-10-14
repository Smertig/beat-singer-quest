#pragma once

#include <string>
#include <vector>

struct Subtitle {
    float time;
    std::string text;
};

struct Lyrics {
    std::vector<Subtitle> subtitles;
};
