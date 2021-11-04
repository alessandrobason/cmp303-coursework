import json, sys

if(len(sys.argv) != 3):
    print("usage: python3 convert.py <input> <output>")
    exit(1)

map_in = sys.argv[1]
map_out = sys.argv[2]

data = json.load(open(map_in))
tileset = json.load(open(data["tilesets"][0]["source"]))

COLLISION_RECT   = 0
COLLISION_CIRCLE = 1

out = {
    "tileset": {
        "texture": "res/magiscarf.png",
    },
    "tilemap": {
        "width": 0,
        "height": 0,
        "layer_count": 0,
        "layers_under": 0,
        "tiles": [],
        "collisions": [],
        "animations": [],
    }
}

out["tilemap"]["width"] = data["width"]
out["tilemap"]["height"] = data["height"]
out["tilemap"]["layer_count"] = len(data["layers"]) - 1

for layer in data["layers"]:
    if layer["name"] == "collisions":
        for obj in layer["objects"]:
            if "ellipse" in obj and obj["ellipse"] == True:
                radius = int(obj["width"] / 2)
                coll = {
                    "type": COLLISION_CIRCLE,
                    "x": obj["x"] + radius,
                    "y": obj["y"] + radius,
                    "radius": radius
                }
            else:
                coll = {
                    "type": COLLISION_RECT,
                    "x": obj["x"],
                    "y": obj["y"],
                    "w": obj["width"],
                    "h": obj["height"],
                }
            out["tilemap"]["collisions"].append(coll)
        continue
    elif layer["name"] == "base":
        out["tilemap"]["layers_under"] = data["layers"].index(layer) + 1
    
    for tile in layer["data"]:
        out["tilemap"]["tiles"].append(tile - 1)

for animation in tileset["tiles"]:
    if "animation" not in animation:
        continue
    
    first_frame = animation["animation"][0]
    index = first_frame["tileid"]

    anim = {
        "duration": first_frame["duration"] / 1000.0,
        "animation": [],
        "indices": [],
    }
    
    for frame in animation["animation"]:
        anim["animation"].append(frame["tileid"])

    tiles = out["tilemap"]["tiles"]
    for i in range(len(tiles)):
        if tiles[i] == index:
            anim["indices"].append(i)

    if len(anim["indices"]) > 0:
        out["tilemap"]["animations"].append(anim)

json.dump(out, open(map_out, "w"), indent=4)