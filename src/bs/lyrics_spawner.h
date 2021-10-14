#pragma once

#include <bs/utils/lyrics.h>

struct LyricsSpawner {
    Lyrics m_lyrics;
    float m_last_song_time = 0.0f;
    std::size_t m_next_subtitle = 0;

public:
    void OnSongStart();
    void OnSongUpdate(float new_song_time);

private:
    static void SpawnText(std::string_view text, float duration);
};
