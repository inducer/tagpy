import _tagpy




def _genreList():
    result = []
    i = 0
    next = _tagpy.id3v1_genre(i)
    while next:
        result.append(next)
        i += 1
        next = _tagpy.id3v1_genre(i)
    return result

_GenreList = _genreList()
_GenreMap = dict([(v, k) for k, v in enumerate(_GenreList)])

genre = _tagpy.id3v1_genre
def genreIndex(genre): return _GenreMap[genre]
def genreList(): return _GenreList
def genreMap(): return _GenreMap
