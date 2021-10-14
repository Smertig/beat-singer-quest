#pragma once

#include <string>

namespace UnityEngine { struct Color; }

namespace config {

void OnSetup();
void OnLoad();

bool isEnabled();
float getTextSpawnStartHeight();
float getTextSpawnEndHeight();
float getTextSpawnEndDepth();
float getTextSpawnFontSize();
UnityEngine::Color getTextSpawnColor();
float getTextDurationMultiplier();
std::string getMusicMatchAPIToken();

} // namespace config
