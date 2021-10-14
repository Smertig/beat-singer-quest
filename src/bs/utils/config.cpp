// HACK: the most dirty hack ever - config-utils uses ObjectEmpty() on non-object rapidjson values, but it still works!
// TODO: make PR and remove hack
#define RAPIDJSON_ASSERT(...)

#include "config.h"

#include <bs/mod_info.h>

#include <extern/beatsaber-hook/shared/utils/typedefs.h>
#include <extern/config-utils/shared/config-utils.hpp>

const char* BUNDLED_MUSIC_MATCH_API_TOKEN = "180220daeb2405592f296c4aea0f6d15e90e08222b559182bacf92";

DECLARE_CONFIG(BeatSingerConfig,
    CONFIG_VALUE(enableMod, bool, "Enable Lyrics", true);
    CONFIG_VALUE(textSpawnStartHeight, float, "Lyrics Spawn Height", 2.1f);
    CONFIG_VALUE(textSpawnEndHeight, float, "Lyrics Target Height", 2.7f);
    CONFIG_VALUE(textSpawnEndDepth, float, "Lyrics Target Depth", 12.0f);
    CONFIG_VALUE(textSpawnColor, UnityEngine::Color, "Lyrics Color", UnityEngine::Color(0.2f, 1.0f, 0.1f, 1.0f));
    CONFIG_VALUE(textSpawnFontSize, float, "Lyrics Font Size", 4.0f);
    CONFIG_VALUE(textDurationMultiplier, float, "Lyrics Duration Multiplier", 1.6f);
    CONFIG_VALUE(musicMatchAPIToken, std::string, "MusicMatch API token", BUNDLED_MUSIC_MATCH_API_TOKEN);

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(enableMod);
        CONFIG_INIT_VALUE(textSpawnStartHeight);
        CONFIG_INIT_VALUE(textSpawnEndHeight);
        CONFIG_INIT_VALUE(textSpawnEndDepth);
        CONFIG_INIT_VALUE(textSpawnColor);
        CONFIG_INIT_VALUE(textSpawnFontSize);
        CONFIG_INIT_VALUE(textDurationMultiplier);
        CONFIG_INIT_VALUE(musicMatchAPIToken);
    )
)
DEFINE_CONFIG(BeatSingerConfig)

void FixToken() {
    auto& token = getBeatSingerConfig().musicMatchAPIToken;
    if (token.GetValue().empty()) {
        token.SetValue(BUNDLED_MUSIC_MATCH_API_TOKEN);
    }
}

// UI implementation

#include <questui/shared/QuestUI.hpp>
#include <HMUI/Touchable.hpp>

void OnUIDidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    using namespace UnityEngine;

    static_cast<void>(addedToHierarchy);
    static_cast<void>(screenSystemEnabling);

    FixToken();

    if (firstActivation) {
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();
        GameObject* container = QuestUI::BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());

        AddConfigValueToggle(container->get_transform(), getBeatSingerConfig().enableMod);
        AddConfigValueIncrementFloat(container->get_transform(), getBeatSingerConfig().textSpawnStartHeight, 1, 0.1f, -3.0f, 8.0f);
        AddConfigValueIncrementFloat(container->get_transform(), getBeatSingerConfig().textSpawnEndHeight, 1, 0.1f, -3.0f, 8.0f);
        AddConfigValueIncrementFloat(container->get_transform(), getBeatSingerConfig().textSpawnEndDepth, 1, 0.5f, 0.0f, 30.0f);
        AddConfigValueColorPicker(container->get_transform(), getBeatSingerConfig().textSpawnColor);
        AddConfigValueIncrementFloat(container->get_transform(), getBeatSingerConfig().textSpawnFontSize, 1, 0.5f, 0.5f, 20.0f);
        AddConfigValueIncrementFloat(container->get_transform(), getBeatSingerConfig().textDurationMultiplier, 1, 0.1f, 0.1f, 5.0f);
        AddConfigValueStringSetting(container->get_transform(), getBeatSingerConfig().musicMatchAPIToken);

        // TODO: add "Reset to defaults" button (or reset token)
    }
}

void config::OnSetup() {
    getBeatSingerConfig().Init(getModInfo());
}

void config::OnLoad() {
    QuestUI::Init();
    QuestUI::Register::RegisterModSettingsViewController(getModInfo(), OnUIDidActivate);
}

bool config::isEnabled() {
    return getBeatSingerConfig().enableMod.GetValue();
}

float config::getTextSpawnStartHeight() {
    return getBeatSingerConfig().textSpawnStartHeight.GetValue();
}

float config::getTextSpawnEndHeight() {
    return getBeatSingerConfig().textSpawnEndHeight.GetValue();
}

float config::getTextSpawnEndDepth() {
    return getBeatSingerConfig().textSpawnEndDepth.GetValue();
}

float config::getTextSpawnFontSize() {
    return getBeatSingerConfig().textSpawnFontSize.GetValue();
}

UnityEngine::Color config::getTextSpawnColor() {
    return getBeatSingerConfig().textSpawnColor.GetValue();
}

float config::getTextDurationMultiplier() {
    return getBeatSingerConfig().textDurationMultiplier.GetValue();
}

std::string config::getMusicMatchAPIToken() {
    FixToken();

    return getBeatSingerConfig().musicMatchAPIToken.GetValue();
}
