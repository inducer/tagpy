import tagpy

f = tagpy.FileRef("la.ogg")
t = f.tag()

print t.artist()
print t.title()
print t.album()
print t.year()

t.setArtist("Andreas")
t.setTitle("Laaa-ahh")
t.setAlbum("Shake what'cha got")
t.setYear(2006)
f.save()
