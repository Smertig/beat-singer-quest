import zipfile
import json
import sys
from pathlib import Path

if len(sys.argv) < 5:
    print('Usage: {} version lib_path out_qmod_path lib_paths...'.format(sys.argv[0]))
    exit(1)

mod_version, lib_path, out_zip_path, *lib_paths = sys.argv[1:]
lib_names = [Path(lib_path).name for lib_path in lib_paths]

MOD_ID = 'beat-singer'
MOD_NAME = 'BeatSinger'
MOD_AUTHOR = 'Smertig'
MOD_DESCRIPTION = 'BeatSinger for OculusQuest'
MOD_GAME_VERSION = '1.17.1'
MOD_VERSION = mod_version

LIB_NAME = 'lib{}.so'.format(MOD_ID)

mod_info = {
    '_QPVersion': '0.1.1',
    'name': MOD_NAME,
    'id': MOD_ID,
    'author': MOD_AUTHOR,
    'packer': MOD_AUTHOR,
    'version': MOD_VERSION,
    'packageId': 'com.beatgames.beatsaber',
    'packageVersion': MOD_GAME_VERSION,
    'description': MOD_DESCRIPTION,
    'modFiles': [
        LIB_NAME
    ],
    'libraryFiles': lib_names,
    'dependencies': []
}


with zipfile.ZipFile(out_zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
    zipf.writestr('mod.json', json.dumps(mod_info, indent=4))
    zipf.write(lib_path, LIB_NAME)
    for lib_name, lib_path in zip(lib_names, lib_paths):
        zipf.write(lib_path, lib_name)
    print(f'> packed to {out_zip_path}')
