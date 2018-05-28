## Requirement

- glfw3

## Compilation

```
make
```

You can also set debug level on compilation

```
make LOG_LEVEL=LOG_DEBUG
```

LOG_LEVEL can be any of:
- LOG_DEBUG
- LOG_INFO
- LOG_WARN
- LOG_ERROR

## Configuration

launch with `./pannellum conf.json` where conf.json is:

```json
{
    "base_path": "path/to/panos/%i",
    "config_path": "/config.json",
    "valid_ids": [14, 16, 20, 23, 26, 27, 31, 37, 5],
    "path": [
        [14, 16],
        [14, 5],
        [5, 16]
    ],
    "start": 14
}

```

- %i will be set to one of the valids ids
- start is the start id
- valid_id is not parsed yet
- config_path is the path under base_path in which is stored the pano's conf:


```json
{
    "type": "multires",
    
    "multiRes": {
        "path": "/%l/%s%y_%x.jpg",
        "tileResolution": 512,
        "maxLevel": 4,
        "cubeResolution": 2896
    }
}
```
- Nearly the same as pannellum conf-file, without extension
