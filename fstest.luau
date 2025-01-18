local tempdir = fs.directory.create("fstestdir")
local otherdir = fs.directory.create("otherdir")
otherdir.parent = tempdir
local nextdir = fs.directory.create("nextdir")
nextdir.parent = tempdir
local nextdir2 = fs.directory.create("nextdir")
nextdir2.parent = nextdir

for e in tempdir:recursive_iterator() do
    print(e)
end
