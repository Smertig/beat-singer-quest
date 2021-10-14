#include <bs/mod_info.h>
#include <bs/utils/config.h>
#include <bs/lyrics_spawner.h>
#include <bs/utils/logger.h>
#include <bs/utils/hook_utils.h>

#include <GlobalNamespace/AudioTimeSyncController.hpp>
#include <GlobalNamespace/GameSongController.hpp>

// This function is called immediately after the mod is loaded and is used to determine information about the mod.
// This function is useful for setting up other things, such as the logger and configuration objects.
extern "C" void setup(ModInfo& info) {
    info = getModInfo();

    // Log some information
    getLogger().info("Mod name: %s", Modloader::getInfo().name.c_str());

    // Create and load config here, config can be modified using rapidjson and getConfig().config for the rapidjson::Document
    config::OnSetup();

    getLogger().info("Completed setup!");
}

// This function is called once il2cpp_init has been called, so this is the first time in the execution that il2cpp_utils and
// il2cpp_functions can be safely used, and OFFSETLESS hooks installed.
extern "C" void load() {
    il2cpp_functions::Init();

    static LyricsSpawner g_lyrics_spawner;

    config::OnLoad();

    getLogger().debug("Installing hooks...");

    hookMethod<&GlobalNamespace::AudioTimeSyncController::Update>(
        [](auto original, GlobalNamespace::AudioTimeSyncController* self) {
            g_lyrics_spawner.OnSongUpdate(self->songTime);

            return original(self);
        }
    );

    hookMethod<&GlobalNamespace::GameSongController::StartSong>(
        [](auto original, auto self, auto offset) {
            g_lyrics_spawner.OnSongStart();

            return original(self, offset);
        }
    );

    getLogger().debug("Installed all hooks!");
}
