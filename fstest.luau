local tempdir = fs.makedir("fstestdir")
local otherdir = fs.makedir("otherdir")
otherdir.parent = tempdir
local nextdir = fs.makedir("nextdir")
nextdir.parent = tempdir
local nextdir2 = fs.makedir("nextdir")
nextdir2.parent = nextdir

for e in tempdir:recursive_iterator() do
    print(e)
end
