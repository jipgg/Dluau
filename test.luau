local fs = require("@minluau/filesystem")
local myPath = fs.path(fs.current_path()):absolute()
print(myPath)
print("yooooo")
