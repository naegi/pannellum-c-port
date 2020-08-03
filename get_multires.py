import argparse
import requests
import sys
import json
import os
import pathlib
import math

def downloadimage(url, path):
    print(f"Downloading img {url} to {path}")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    r = requests.get(url, stream=True)
    if r.status_code == 200:
        with open(path, 'wb') as f:
            for chunk in r.iter_content(1024):
                f.write(chunk)
    else:
        print("Fail !")

def downladscene(project, scene_id, scene, base):
    max_level = scene["multiRes"]["maxLevel"]
    base_path = scene["multiRes"]["basePath"]
    path = scene["multiRes"]["path"]
    ext = scene["multiRes"]["extension"]
    size = scene["multiRes"]["cubeResolution"]
    tileSize = scene["multiRes"]["tileResolution"]

    for level in range(max_level, 0, -1):
        tiles = int(math.ceil(float(size) / tileSize))
        for s in ['l', 'u', 'd', 'b', 'r', 'f']:
            for x in range(tiles):
                for y in range(tiles):
                    t = path.replace('%l', str(level)).replace("%s", s).replace("%x", str(x)).replace("%y", str(y))
                    url = base + base_path + t + '.' + ext
                    filepath = project + "/" + str(scene_id) + "/" + t +  '.' + ext
                    downloadimage(url, filepath)
        size = int(size / 2)

def writeconfigscene(project, scene_id, scene):
    with open(project + "/" + str(scene_id) + "/config.json", "w") as f:
        json.dump({
            "type": "multires", "multiRes": {
                "path": scene["multiRes"]["path"] + '.' + scene["multiRes"]["extension"],
                "tileResolution": scene["multiRes"]["tileResolution"],
                "maxLevel": scene["multiRes"]["maxLevel"],
                "cubeResolution": scene["multiRes"]["cubeResolution"],
            }
        }, f)

def writeconfig(project, config, scene_map):
    with open(project + "/config.json", "w") as f:
        json.dump({
            "base_path": "./%i",
            "config_path": "/config.json",
            "valid_ids": list(scene_map.values()),
            "path": [],
            "start": scene_map[config["default"]["firstScene"]]
        }, f)

def main(project, config, base):
    scene_id = 0
    scene_map = dict()
    for scene in config["scenes"].keys():
        scene_map[scene] = scene_id
        downladscene(project, scene_id, config["scenes"][scene], base)
        writeconfigscene(project, scene_id, config["scenes"][scene])
        scene_id += 1
    writeconfig(project, config, scene_map)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('base', type=str, help="Base url to multires")
    parser.add_argument('project', help="Name of the project to import (used for choosing local folder names only)")
    parser.add_argument('configfile', type=argparse.FileType('r'), default=sys.stdin)
    args = parser.parse_args()

    config = json.load(args.configfile)
    main(args.project, config, args.base)
