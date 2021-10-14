cmake_minimum_required(VERSION 3.16)
project(beat_singer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(MOD_VERSION "0.2.1")

set(SCRIPT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# TODO: target_*
add_compile_options(-frtti -fPIE -fPIC)
add_compile_definitions(UNITY_2019)
add_compile_definitions(MOD_VERSION=\"${MOD_VERSION}\")

# extern mods libraries
set(EXTERN_MODS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern_mods)
set(EXTERN_MODS_INCLUDE_DIR ${EXTERN_MODS_DIR}/includes)
set(EXTERN_MODS_LIBS_DIR ${EXTERN_MODS_DIR}/libs)

# extern C++ libraries
add_subdirectory(${EXTERN_DIR}/rcmp)
include_directories(${EXTERN_DIR}/nlohmann-json/include)

include_directories(${EXTERN_MODS_INCLUDE_DIR}) # required by beat-singer
include_directories(${EXTERN_MODS_INCLUDE_DIR}/extern) # required by beatsaber-hook (uses #include "modloader/shared/modloader.hpp" in logging.hpp)
#include_directories(${EXTERN_MODS_INCLUDE_DIR}/extern/questui_components) # required by questui_components
include_directories(${EXTERN_MODS_INCLUDE_DIR}/extern/libil2cpp/il2cpp/libil2cpp) # required by codegen
include_directories(${EXTERN_MODS_INCLUDE_DIR}/extern/codegen/include) # required to use codegen
link_directories(${EXTERN_MODS_LIBS_DIR})

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs")

# beat-singer
add_library(beat-singer SHARED
        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/bs/lyrics_loader.cpp
        ${SOURCE_DIR}/bs/lyrics_spawner.cpp
        ${SOURCE_DIR}/bs/lyrics_parser.cpp
        ${SOURCE_DIR}/bs/utils/strings.cpp
        ${SOURCE_DIR}/bs/utils/rcmp.cpp
        ${SOURCE_DIR}/bs/utils/logger.cpp
        ${SOURCE_DIR}/bs/utils/config.cpp
)
target_include_directories(beat-singer PRIVATE ${SOURCE_DIR})
target_link_libraries(beat-singer PRIVATE rcmp)

# various modding stuff
target_link_libraries(beat-singer PRIVATE
        log
        beatsaber-hook_2_3_1
        modloader
        codegen
        custom-types
        questui
        #questui_components
)

# automatically create qmod after build
add_custom_command(TARGET beat-singer POST_BUILD
    COMMAND python
        ${SCRIPT_DIR}/packer.py
        ${MOD_VERSION}
        $<TARGET_FILE:beat-singer>
        ${CMAKE_CURRENT_SOURCE_DIR}/beat-singer.qmod
        ${EXTERN_MODS_LIBS_DIR}/libbeatsaber-hook_2_3_1.so
        ${EXTERN_MODS_LIBS_DIR}/libcustom-types.so
        ${EXTERN_MODS_LIBS_DIR}/libquestui.so
)
