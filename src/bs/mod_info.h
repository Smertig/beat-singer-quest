#pragma once

#include <extern/modloader/shared/modloader.hpp>

// Holds the ModInfo object for this mod. This object is set in void setup()
inline const ModInfo& getModInfo() {
    static ModInfo modInfo {
        .id      = "beat-singer",
        .version = MOD_VERSION
    };

    return modInfo;
}
