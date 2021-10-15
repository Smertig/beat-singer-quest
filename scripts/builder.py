import os
import shutil
import subprocess

# download NDK from https://developer.android.com/ndk/downloads or
#                   https://github.com/android/ndk/wiki/Unsupported-Downloads
# upload mod: https://ethansaber.github.io/Beat-Saber-Quest-Mod-Tutorial/

BUILD_FOLDER_NAME = '_build'
FULL_REBUILD      = True
IS_DEBUG          = os.getenv('BEAT_SINGER_DEBUG') == '1'
NDK_PATH          = os.getenv('NDK_PATH')
ANDROID_ABI       = 'arm64-v8a'
ANDROID_PLATFORM  = 25

if FULL_REBUILD:
    shutil.rmtree(BUILD_FOLDER_NAME, ignore_errors=True)
    os.mkdir(BUILD_FOLDER_NAME)
os.chdir(BUILD_FOLDER_NAME)

cmake_args = {
    'CMAKE_BUILD_TYPE': 'Debug' if IS_DEBUG else 'Release',
    'CMAKE_TOOLCHAIN_FILE': f'{NDK_PATH}/build/cmake/android.toolchain.cmake',
    'CMAKE_MAKE_PROGRAM': f'{NDK_PATH}/prebuilt/windows-x86_64/bin/make.exe',
    'ANDROID_NDK': NDK_PATH,
    'ANDROID_ABI': ANDROID_ABI,
    'ANDROID_PLATFORM': ANDROID_PLATFORM,
    'ANDROID_STL': 'c++_static'
}

subprocess.run([ 
    'cmake',
    '-G', 'MinGW Makefiles',
    *(f'-D{key}={value}' for key, value in cmake_args.items()),
    '..'
])

subprocess.run([
    'cmake',
    '--build',
    '.'
], check=True)
