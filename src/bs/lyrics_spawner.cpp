#include "lyrics_spawner.h"
#include "lyrics_loader.h"
#include "lyrics_parser.h"

#include <bs/utils/logger.h>
#include <bs/utils/config.h>
#include <bs/utils/strings.h>
#include <bs/utils/scope_value.h>

#include <UnityEngine/Object.hpp>
#include <UnityEngine/Transform.hpp>
#include <GlobalNamespace/FlyingTextSpawner.hpp>
#include <GlobalNamespace/BeatmapObjectsInstaller.hpp>
#include <GlobalNamespace/GameplayCoreSceneSetupData.hpp>
#include <GlobalNamespace/IDifficultyBeatmap.hpp>
#include <GlobalNamespace/IBeatmapLevel.hpp>
#include <Zenject/DiContainer.hpp>

void LyricsSpawner::SpawnText(std::string_view text, float duration) {
    getLogger().debug("SpawnText: text='%*s', duration=%f", static_cast<int>(text.size()), text.data(), duration);

    auto text_spawner = UnityEngine::Object::FindObjectOfType<GlobalNamespace::FlyingTextSpawner*>();
    if (!text_spawner) {
        getLogger().warning("Missing FlyingTextSpawner on scene");
        return;
    }

    auto pos = UnityEngine::Vector3{ 0.0f, config::getTextSpawnStartHeight(), 0.0f };
    auto identity = UnityEngine::Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };

    [[maybe_unused]] auto guard1 = utils::scope_exchange(text_spawner->targetYPos, config::getTextSpawnEndHeight());
    [[maybe_unused]] auto guard2 = utils::scope_exchange(text_spawner->targetZPos, config::getTextSpawnEndDepth());
    [[maybe_unused]] auto guard3 = utils::scope_exchange(text_spawner->duration, duration);
    [[maybe_unused]] auto guard4 = utils::scope_exchange(text_spawner->fontSize, config::getTextSpawnFontSize());
    [[maybe_unused]] auto guard5 = utils::scope_exchange(text_spawner->color, config::getTextSpawnColor());

    text_spawner->SpawnText(pos, identity, identity, utils::to_il2cpp(text));

    getLogger().debug("FlyingTextSpawner: duration=%f, xSpread=%f, targetPos=%f;%f, fontSize=%f, shake=%s",
                      text_spawner->duration,
                      text_spawner->xSpread,
                      text_spawner->targetYPos,
                      text_spawner->targetZPos,
                      text_spawner->fontSize,
                      text_spawner->shake ? "true" : "false"
    );
}

void LyricsSpawner::OnSongStart() {
    m_last_song_time = 0.0f;
    m_lyrics = {};
    m_next_subtitle = 0;

    if (!config::isEnabled()) {
        return;
    }

    getLogger().info("Getting scene_setup..");
    auto scene_setup = UnityEngine::Object::FindObjectOfType<GlobalNamespace::BeatmapObjectsInstaller*>();
    CRASH_UNLESS(scene_setup);
    getLogger().info("Got! scene_setup..");

    getLogger().info("Instantiating FlyingTextSpawner..");
    auto text_spawner = scene_setup->Container->InstantiateComponentOnNewGameObject<GlobalNamespace::FlyingTextSpawner*>();
    getLogger().info("FlyingTextSpawner is ready: %p", text_spawner);

    getLogger().info("Getting scene_setup_data..");
    auto scene_setup_data = scene_setup->sceneSetupData;
    CRASH_UNLESS(scene_setup_data);
    getLogger().info("Got scene_setup_data! %p", scene_setup_data);

    getLogger().info("Getting beatmap..");
    auto beatmap = scene_setup_data->difficultyBeatmap;
    getLogger().info("Got beatmap! %p", beatmap);

    getLogger().debug("Getting level..");
    auto level = beatmap->get_level();
    getLogger().debug("Got level! %p", level);

    auto preview_level = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(level);

    getLogger().debug("Getting song_name..");
    auto song_name = preview_level->get_songName();
    getLogger().debug("Got song_name! %p", song_name);

    getLogger().debug("Getting song_author_name..");
    auto song_author_name = preview_level->get_songAuthorName();
    getLogger().debug("Got song_author_name! %p", song_author_name);

    if (song_name && song_author_name) {
        const auto song_name_utf8 = utils::to_utf8(song_name);
        const auto song_author_name_utf8 = utils::to_utf8(song_author_name);
        getLogger().info("song: '%s', author: '%s'", song_name_utf8.c_str(), song_author_name_utf8.c_str());
        getLogger().info("timeOffset: %f", preview_level->get_songTimeOffset());

        loadLyrics(song_name_utf8, song_author_name_utf8, [this](std::optional<std::string> result) {
            if (!result) {
                SpawnText("Lyrics not found..", 2.0f);
                return;
            }

            auto lyrics = parseLyrics(*result);
            if (!lyrics) {
                SpawnText("Unable to parse lyrics..", 2.0f);
                return;
            }

            SpawnText("Lyrics found online!", 2.0f);
            m_lyrics = std::move(*lyrics);
        });
    }
    else {
        getLogger().warning("song or author is invalid");
    }
}

void LyricsSpawner::OnSongUpdate(float new_song_time) {
    if (m_last_song_time == new_song_time) {
        return;
    }

    if (m_next_subtitle < m_lyrics.subtitles.size()) {
        const float DELAY = 0.0f;
        const float DEFAULT_DURATION = 1.0f;

        float subtitle_time = m_lyrics.subtitles[m_next_subtitle].time;

        if (new_song_time >= subtitle_time + DELAY) {
            const float duration = [&]{
                if (m_next_subtitle + 1 < m_lyrics.subtitles.size()) {
                    return m_lyrics.subtitles[m_next_subtitle + 1].time - subtitle_time;
                }
                else {
                    return DEFAULT_DURATION;
                }
            }();

            SpawnText(m_lyrics.subtitles[m_next_subtitle].text, duration * config::getTextDurationMultiplier());
            ++m_next_subtitle;
        }
    }

    m_last_song_time = new_song_time;
}
