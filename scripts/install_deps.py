import json
import os
import stat
import shutil
import requests
import contextlib
import zipfile
from tqdm import tqdm
from pathlib import Path


OUTPUT_PATH = "./extern_mods"
API_URL = "https://qpackages.com"
DEPENDENCIES = [
    {
        'name': 'beatsaber-hook',
        'version': '2.3.1'
    },
    {
        'name': 'modloader',
        'version': '1.2.3'
    },
    {
        'name': 'codegen',
        'version': '0.14.0'
    },
    {
        'name': 'libil2cpp',
        'version': '0.2.2'
    },
    {
        'name': 'config-utils',
        'version': '0.6.0'
    },
    {
        'name': 'questui',
        'version': '0.11.1'
    },
    #{
    #    'name': 'questui_components',
    #    'version': '0.1.16'
    #},
    {
        'name': 'custom-types',
        'version': '0.12.7'
    }
]


# From https://stackoverflow.com/questions/41742317/how-can-i-change-directory-with-python-pathlib
@contextlib.contextmanager
def change_dir(path):
    """Changes working directory and returns to previous on exit."""
    prev_cwd = Path.cwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)


def do_request(path: str) -> dict:
    result = requests.get(API_URL + path)
    if result.text == 'Not Found':
        raise Exception('unknown API path: ' + path)

    return result.json()


def remove_directory(path: str):
    if not os.path.exists(path):
        return

    def on_rm_error(func, path, exc_info):
        os.chmod(path, stat.S_IWRITE)
        os.unlink(path)
    shutil.rmtree(path, onerror=on_rm_error)


def download_file(output_path, url):
    response = requests.get(url, stream=True)

    with tqdm.wrapattr(open(output_path, 'wb'), "write",
                       miniters=1, desc=url.split('/')[-1],
                       total=int(response.headers.get('content-length', 0))) as f:
        for data in response.iter_content(256 * 1024):
            f.write(data)


def download_dependency(output_path: str, config: dict):
    info = config['info']
    id_name: str = info['id']
    url: str = info['url']
    ex_info = info['additionalData']

    include_dir = output_path + '/includes/' + config['dependenciesDir']
    os.makedirs(include_dir, exist_ok=True)
    with change_dir(include_dir):
        if url.startswith('https://github.com'):
            repo_branch = ex_info['branchName']

            print(f'Cloning repo {url}')
            # TODO: use subprocess
            os.system(f'git clone --recurse-submodules --depth=1 --branch={repo_branch} {url} {id_name}')
        else:
            zip_path = id_name + '.zip'
            download_file(zip_path, url)
            with zipfile.ZipFile(zip_path, 'r') as zip_ref:
                zip_ref.extractall(id_name)
            os.remove(zip_path)

    if ex_info.get('headersOnly', False):
        return

    with change_dir(output_path + '/libs'):
        so_link = ex_info['soLink']
        if 'overrideSoName' in ex_info:
            so_name = ex_info['overrideSoName']
        else:
            so_name = Path(so_link).name
            if so_name.endswith('.so.so'):
                so_name = so_name[:-3]

        print(f'Loading library {so_name} from {so_link}')
        download_file(so_name, so_link)


def install_dependencies(output_path: str):
    remove_directory(output_path)
    os.makedirs(output_path + '/includes')
    os.makedirs(output_path + '/libs')

    for dependency in DEPENDENCIES:
        name = dependency['name']
        version = dependency['version']
        print(f'Looking for {name}..')

        dep_data = do_request(f'/{name}/{version}')
        #print(json.dumps(dep_data, indent=2))

        download_dependency(output_path, dep_data['config'])


if __name__ == "__main__":
    install_dependencies(OUTPUT_PATH)
