#include <tstring.h>
#include <tag.h>
#include <tfile.h>
#include <audioproperties.h>
#include <tstringlist.h>
#include <fileref.h>
#include <id3v1genres.h>
#include <id3v1tag.h>
#include <id3v2tag.h>
#include <id3v2header.h>
#include <id3v2extendedheader.h>
#include <id3v2footer.h>
#include <mpegfile.h>
#include <apetag.h>
#include <mpcfile.h>
#include <flacfile.h>
#include <xiphcomment.h>
#include <attachedpictureframe.h>
#include <commentsframe.h>
#include <relativevolumeframe.h>
#include <textidentificationframe.h>

#include <boost/python.hpp>
#include <boost/scoped_array.hpp>
#include <stdexcept>




using namespace boost::python;
using namespace TagLib;
using namespace std;




namespace 
{
  struct tlstring_to_unicode
  {
      static PyObject *convert(String const& s)
        {
          const char *cstr = s.toCString(true);
          return PyUnicode_DecodeUTF8(cstr, strlen(cstr), "ignore");
        }
  };

  struct tlbytevector_to_string
  {
      static PyObject *convert(ByteVector const& s)
        {
          return PyString_FromStringAndSize(s.data(), s.size());
        }
  };

  struct ByteVectorIntermediate
  {
      string str;
      ByteVectorIntermediate(const string &_str)
        : str(_str)
        { }
      operator ByteVector() const
        {
          return ByteVector(str.data(), str.size());
        }
  };

  template<typename Iterator>
  object make_list(Iterator first, Iterator last)
  {
    boost::python::list result;
    while (first != last)
      result.append(*first++);
    return result;
  }

  // -------------------------------------------------------------
  // Map
  // -------------------------------------------------------------
  template<typename Key, typename Value>
  Value &Map_getitem(Map<Key, Value> &m, const Key &k)
  {
    if (!m.contains(k))
    {
      PyErr_SetString( PyExc_KeyError, "key not in map");
      throw error_already_set();
    }
    return m[k];
  }

  template<typename Key, typename Value>
  void Map_setitem(Map<Key, Value> &m, const Key &k, const Value &v)
  {
    m[k] = v;
  }

  template<typename Key, typename Value>
  object Map_keys(Map<Key, Value> &m)
  {
    boost::python::list keys;
    
    typedef Map<Key, Value> map;
    typename map::Iterator first = m.begin(), last = m.end();
    while (first != last)
      keys.append((first++)->first);
    return keys;
  }

  template<typename Key, typename Value>
  void exposeMap(const char *name)
  {
    typedef Map<Key, Value> map;
    class_<map>(name)
      .def("__len__", &map::size)
      .def("size", &map::size)
      .def("size", &map::clear)
      .def("isEmpty", &map::isEmpty)
      .def("__getitem__", Map_getitem<Key, Value>, return_internal_reference<>())
      .def("__setitem__", Map_setitem<Key, Value>)
      .def("__contains__", &map::contains)
      .def("keys", Map_keys<Key, Value>)
      ;
  }

  // -------------------------------------------------------------
  // List
  // -------------------------------------------------------------
  template<typename Value>
  Value &List_getitem(List<Value> &l, TagLib::uint i)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    return l[i];
  }

  template<typename Value>
  void List_setitem(List<Value> &l, TagLib::uint i, Value v)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    l[i] = v;
  }

  template<typename Value>
  void List_append(List<Value> &l, Value v)
  {
    l.append(v);
  }

  template<typename Value>
  void exposeList(const char *name)
  {
    typedef List<Value *> list;
    class_<list>(name)
      .def("__len__", &list::size)
      .def("size", &list::size)
      .def("clear", &list::clear)
      .def("isEmpty", &list::isEmpty)
      .def("__getitem__", List_getitem<Value>, return_internal_reference<>())
      .def("__setitem__", List_setitem<Value>)
      .def("append", List_append<Value>)
      // MISSING: iterators, insert, find, contains, erase,
      // assignment, comparison
      ;
  }

  // -------------------------------------------------------------
  // PointerList
  // -------------------------------------------------------------
  template<typename Value>
  Value *&PointerList_getitem(List<Value *> &l, TagLib::uint i)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    return l[i];
  }

  template<typename Value>
  void PointerList_setitem(List<Value *> &l, TagLib::uint i, auto_ptr<Value> v)
  {
    if (i >= l.size())
    {
      PyErr_SetString( PyExc_IndexError, "index out of bounds");
      throw error_already_set();
    }
    l[i] = v.release();
  }

  template<typename Value>
  void PointerList_append(List<Value *> &l, auto_ptr<Value> v)
  {
    l.append(v.release());
  }

  template<typename Value>
  void exposePointerList(const char *name)
  {
    typedef List<Value *> list;
    class_<list>(name)
      .def("__len__", &list::size)
      .def("size", &list::size)
      .def("clear", &list::clear)
      .def("isEmpty", &list::isEmpty)
      .def("__getitem__", PointerList_getitem<Value>, return_internal_reference<>())
      .def("__setitem__", PointerList_setitem<Value>)
      .def("append", PointerList_append<Value>)
      // MISSING: iterators, insert, find, contains, erase,
      // assignment, comparison
      ;
  }




  // -------------------------------------------------------------
  // Basics
  // -------------------------------------------------------------
  struct TagWrap : Tag, wrapper<Tag>
  {
      String title() const { return this->get_override("title")(); }
      String artist() const { return this->get_override("artist")(); }
      String album() const { return this->get_override("album")(); }
      String comment() const { return this->get_override("comment")(); }
      String genre() const { return this->get_override("genre")(); }
      TagLib::uint year() const { return this->get_override("year")(); }
      TagLib::uint track() const { return this->get_override("track")(); }
      void setTitle(const String &v) const { this->get_override("setTitle")(v); }
      void setArtist(const String &v) const { this->get_override("setArtist")(v); }
      void setAlbum(const String &v) const { this->get_override("setAlbum")(v); }
      void setComment(const String &v) const { this->get_override("setComment")(v); }
      void setGenre(const String &v) const { this->get_override("setGenre")(v); }
      void setYear(TagLib::uint i) const { this->get_override("setYear")(i); }
      void setTrack(TagLib::uint i) const { this->get_override("setTrack")(i); }
  };




  struct AudioPropertiesWrap : AudioProperties, wrapper<AudioProperties>
  {
      int length() const { return this->get_override("length")(); }
      int bitrate() const { return this->get_override("bitrate")(); }
      int sampleRate() const { return this->get_override("sampleRate")(); }
      int channels() const { return this->get_override("channels")(); }
  };




  struct FileWrap : File, wrapper<File>
  {
      Tag *tag() const { return this->get_override("tag")(); }
      AudioProperties *audioProperties() const { return this->get_override("audioProperties")(); }
      bool save() { return this->get_override("save")(); }
  };




  // -------------------------------------------------------------
  // ID3v2
  // -------------------------------------------------------------
  struct id3v2_FrameWrap : ID3v2::Frame, wrapper<ID3v2::Frame>
  {
      String toString() const { return this->get_override("toString")(); }
  };
  

  void id3v2_Tag_removeFrame(ID3v2::Tag &t, ID3v2::Frame *f) { t.removeFrame(f); }
  
  object id3v2_rvf_channels(const ID3v2::RelativeVolumeFrame &rvf)
  {
    List<ID3v2::RelativeVolumeFrame::ChannelType> l = rvf.channels();
    return make_list(l.begin(), l.end());
  }

  #define MF_OL(MF, MIN, MAX) \
  BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(MF##_overloads, MF, MIN, MAX);

  MF_OL(createFrame, 1, 2);
  MF_OL(volumeAdjustmentIndex, 0, 1);
  MF_OL(volumeAdjustment, 0, 1);
  MF_OL(peakVolume, 0, 1);
  MF_OL(setVolumeAdjustmentIndex, 1, 2);
  MF_OL(setVolumeAdjustment, 1, 2);
  MF_OL(setPeakVolume, 1, 2);

  // -------------------------------------------------------------
  // MPEG
  // -------------------------------------------------------------
  MF_OL(save, 0, 2)
  MF_OL(ID3v1Tag, 0, 1)
  MF_OL(ID3v2Tag, 0, 1)
  MF_OL(APETag, 0, 1)
  MF_OL(strip, 0, 1)

  // -------------------------------------------------------------
  // MPC
  // -------------------------------------------------------------
  MF_OL(remove, 0, 1)

  // -------------------------------------------------------------
  // FLAC
  // -------------------------------------------------------------
  MF_OL(xiphComment, 0, 1)
}




#define DEF_SIMPLE_METHOD(NAME) \
  def(#NAME, &cl::NAME)
#define DEF_VIRTUAL_METHOD(NAME) \
  def(#NAME, pure_virtual(&cl::NAME))
#define DEF_OVERLOADED_METHOD(NAME, CAST) \
  def(#NAME, (CAST) &cl::NAME, NAME##_overloads())
#define ENUM_VALUE(NAME) \
  value(#NAME, scope::NAME)




BOOST_PYTHON_MODULE(_tagpy)
{
  // -------------------------------------------------------------
  // Infrastructure
  // -------------------------------------------------------------
  to_python_converter<String, tlstring_to_unicode>();
  to_python_converter<ByteVector, tlbytevector_to_string>();
  implicitly_convertible<std::wstring, String>();
  implicitly_convertible<std::string, ByteVectorIntermediate>();
  implicitly_convertible<ByteVectorIntermediate, ByteVector>();

  // -------------------------------------------------------------
  // Basics
  // -------------------------------------------------------------
  exposeList<StringList>("StringList");

  {
    typedef Tag cl;
    class_<TagWrap, boost::noncopyable>("Tag", no_init)
      .DEF_VIRTUAL_METHOD(title)
      .DEF_VIRTUAL_METHOD(artist)
      .DEF_VIRTUAL_METHOD(album)
      .DEF_VIRTUAL_METHOD(comment)
      .DEF_VIRTUAL_METHOD(genre)
      .DEF_VIRTUAL_METHOD(year)
      .DEF_VIRTUAL_METHOD(track)
      
      .DEF_VIRTUAL_METHOD(setTitle)
      .DEF_VIRTUAL_METHOD(setArtist)
      .DEF_VIRTUAL_METHOD(setAlbum)
      .DEF_VIRTUAL_METHOD(setComment)
      .DEF_VIRTUAL_METHOD(setGenre)
      .DEF_VIRTUAL_METHOD(setYear)
      .DEF_VIRTUAL_METHOD(setTrack)
      
      .DEF_VIRTUAL_METHOD(isEmpty)
      ;
  }

  {
    typedef AudioProperties cl;
    class_<AudioPropertiesWrap, boost::noncopyable>("AudioProperties", no_init)
      .def("length", pure_virtual(&AudioProperties::length))
      .def("bitrate", pure_virtual(&AudioProperties::bitrate))
      .def("sampleRate", pure_virtual(&AudioProperties::sampleRate))
      .def("channels", pure_virtual(&AudioProperties::channels))
      ;
  }

  enum_<AudioProperties::ReadStyle>("ReadStyle")
    .value("Fast", AudioProperties::Fast)
    .value("Average", AudioProperties::Average)
    .value("Accurate", AudioProperties::Accurate)
    ;

  {
    typedef File cl;

    class_<FileWrap, boost::noncopyable>("File", no_init)
      .def("name", &File::name)
      .def("audioProperties", pure_virtual(&File::audioProperties), 
           return_internal_reference<>())
      .def("tag", pure_virtual(&File::tag),
           return_internal_reference<>())
      .DEF_VIRTUAL_METHOD(save)
      .DEF_SIMPLE_METHOD(readOnly)
      .DEF_SIMPLE_METHOD(isOpen)
      .DEF_SIMPLE_METHOD(isValid)
      .DEF_SIMPLE_METHOD(clear)
      .DEF_SIMPLE_METHOD(length)
      ;
  }

  enum_<String::Type>("StringType")
    .value("Latin1", String::Latin1)
    .value("UTF16", String::UTF16)
    .value("UTF16BE", String::UTF16BE)
    .value("UTF8", String::UTF8)
    .value("UTF16LE", String::UTF16LE)
    ;

  class_<FileRef>(
    "FileRef", 
    init<const char *, optional<bool, AudioProperties::ReadStyle> >())
    .def("tag", &FileRef::tag,
         return_internal_reference<>())
    .def("audioProperties", &FileRef::audioProperties, 
         return_internal_reference<>())
    .def("file", &FileRef::file,
         return_internal_reference<>())
    .def("save", &FileRef::save)
    .def("isNull", &FileRef::isNull)
    ;

  // -------------------------------------------------------------
  // ID3v1
  // -------------------------------------------------------------
  def("id3v1_genre", ID3v1::genre);

  class_<ID3v1::Tag, boost::noncopyable, bases<Tag> >
    ("id3v1_Tag")
    .def("render", &ID3v1::Tag::render)
    ;

  // -------------------------------------------------------------
  // ID3v2
  // -------------------------------------------------------------
  ID3v2::Frame *(ID3v2::FrameFactory::*cf1)(const ByteVector &, bool) const
    = &ID3v2::FrameFactory::createFrame;
  ID3v2::Frame *(ID3v2::FrameFactory::*cf2)(const ByteVector &, TagLib::uint) const
    = &ID3v2::FrameFactory::createFrame;
  class_<ID3v2::FrameFactory, boost::noncopyable>
    ("id3v2_FrameFactory", no_init)
    .def("createFrame", cf1, return_value_policy<manage_new_object>())
    .def("createFrame", cf2, createFrame_overloads()[return_value_policy<manage_new_object>()])
    .def("instance", &ID3v2::FrameFactory::instance,
         return_value_policy<reference_existing_object>())
    .staticmethod("instance")
    // MISSING: text encoding
    ;

  class_<id3v2_FrameWrap, boost::noncopyable>("id3v2_Frame", no_init)
    .def("frameID", &ID3v2::Frame::frameID)
    .def("size", &ID3v2::Frame::size)
    .def("setData", &ID3v2::Frame::setData)
    .def("setText", &ID3v2::Frame::setText)
    .def("toString", pure_virtual(&ID3v2::Frame::toString))
    .def("render", &ID3v2::Frame::render)

    .def("headerSize", 
         (TagLib::uint (*)()) 
         &ID3v2::Frame::headerSize)
    .def("headerSize", 
         (TagLib::uint (*)(TagLib::uint)) 
         &ID3v2::Frame::headerSize)
    // MISSING: textDelimiter
    ;

  {
    typedef ID3v2::Header cl;
    class_<cl, boost::noncopyable>
      ("id3v2_Header") 
    // MISSING: second constructor
      .DEF_SIMPLE_METHOD(majorVersion)
      .DEF_SIMPLE_METHOD(revisionNumber)
      .DEF_SIMPLE_METHOD(extendedHeader)
      .DEF_SIMPLE_METHOD(experimentalIndicator)
      .DEF_SIMPLE_METHOD(footerPresent)
      .DEF_SIMPLE_METHOD(tagSize)
      .DEF_SIMPLE_METHOD(completeTagSize)
      .DEF_SIMPLE_METHOD(setTagSize)
      .DEF_SIMPLE_METHOD(setData)
      .DEF_SIMPLE_METHOD(render)
      .DEF_SIMPLE_METHOD(size)
      .staticmethod("size")
      .DEF_SIMPLE_METHOD(fileIdentifier)
      .staticmethod("fileIdentifier")
      ;
  }

  {
    typedef ID3v2::ExtendedHeader cl;
    class_<cl, boost::noncopyable>
      ("id3v2_ExtendedHeader", no_init) 
      .DEF_SIMPLE_METHOD(size)
      .DEF_SIMPLE_METHOD(setData)
      ;
  }

  {
    typedef ID3v2::Footer cl;
    class_<cl, boost::noncopyable>
      ("id3v2_Footer", no_init) 
      .DEF_SIMPLE_METHOD(render)
      .DEF_SIMPLE_METHOD(size)
      .staticmethod("size")
      ;
  }

  const ID3v2::FrameList &(ID3v2::Tag::*fl1)(const ByteVector &) const =
    &ID3v2::Tag::frameList;
  const ID3v2::FrameList &(ID3v2::Tag::*fl2)() const =
    &ID3v2::Tag::frameList;
  class_<ID3v2::Tag, boost::noncopyable, bases<Tag> >("id3v2_Tag")
    .def("header", &ID3v2::Tag::header, return_internal_reference<>())
    .def("extendedHeader", &ID3v2::Tag::extendedHeader, return_internal_reference<>())
    .def("footer", &ID3v2::Tag::footer, return_internal_reference<>())

    .def("frameListMap", &ID3v2::Tag::frameListMap, return_internal_reference<>())
    .def("frameList", fl1, return_internal_reference<>())
    .def("frameList", fl2, return_internal_reference<>())

    .def("addFrame", &ID3v2::Tag::addFrame)
    .def("removeFrame", id3v2_Tag_removeFrame)
    .def("removeFrames", &ID3v2::Tag::removeFrames)

    .def("render", &ID3v1::Tag::render)
    ;
  exposeMap<ByteVector, ID3v2::FrameList>("id3v2_FrameListMap");
  exposePointerList<ID3v2::Frame>("id3v2_FrameList");

 
  // -------------------------------------------------------------
  // ID3v2 frame types
  // -------------------------------------------------------------
  {
    typedef TagLib::ID3v2::AttachedPictureFrame scope;
    enum_<ID3v2::AttachedPictureFrame::Type>("id3v2_AttachedPictureFrame_Type")
      .ENUM_VALUE(Other)
      .ENUM_VALUE(FileIcon)
      .ENUM_VALUE(OtherFileIcon)
      .ENUM_VALUE(FrontCover)
      .ENUM_VALUE(BackCover)
      .ENUM_VALUE(LeafletPage)
      .ENUM_VALUE(Media)
      .ENUM_VALUE(LeadArtist)
      .ENUM_VALUE(Artist)
      .ENUM_VALUE(Conductor)
      .ENUM_VALUE(Band)
      .ENUM_VALUE(Composer)
      .ENUM_VALUE(Lyricist)
      .ENUM_VALUE(RecordingLocation)
      .ENUM_VALUE(DuringRecording)
      .ENUM_VALUE(DuringPerformance)
      .ENUM_VALUE(MovieScreenCapture)
      .ENUM_VALUE(ColouredFish)
      .ENUM_VALUE(Illustration)
      .ENUM_VALUE(BandLogo)
      .ENUM_VALUE(PublisherLogo)
      ;
  }

  {
    typedef ID3v2::AttachedPictureFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_AttachedPictureFrame", init<optional<const ByteVector &> >())
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      .DEF_SIMPLE_METHOD(mimeType)
      .DEF_SIMPLE_METHOD(setMimeType)
      .DEF_SIMPLE_METHOD(type)
      .DEF_SIMPLE_METHOD(setType)
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(picture)
      .DEF_SIMPLE_METHOD(setPicture)
      ;
  }

  {
    typedef ID3v2::CommentsFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_CommentsFrame", init<optional<const ByteVector &> >())
      .def(init<String::Type>())
      .DEF_SIMPLE_METHOD(language)
      .DEF_SIMPLE_METHOD(setLanguage)
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      ;
  }

  {
    typedef ID3v2::RelativeVolumeFrame::PeakVolume cl;
    class_<cl>
      ("id3v2_PeakVolume")
      .def_readwrite("bitsRepresentingPeak", &cl::bitsRepresentingPeak)
      .def_readwrite("peakVolume", &cl::peakVolume)
      ;
  }

  {
    typedef TagLib::ID3v2::RelativeVolumeFrame scope;
    enum_<ID3v2::RelativeVolumeFrame::ChannelType>("id3v2_RelativeVolumeFrame_ChannelType")
      .ENUM_VALUE(Other)
      .ENUM_VALUE(MasterVolume)
      .ENUM_VALUE(FrontRight)
      .ENUM_VALUE(FrontLeft)
      .ENUM_VALUE(BackRight)
      .ENUM_VALUE(BackLeft)
      .ENUM_VALUE(FrontCentre)
      .ENUM_VALUE(BackCentre)
      .ENUM_VALUE(Subwoofer)
      ;
  }

  {
    typedef ID3v2::RelativeVolumeFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_RelativeVolumeFrame", init<optional<const ByteVector &> >())
      .def("channels", id3v2_rvf_channels)
      .DEF_SIMPLE_METHOD(setChannelType)
      .DEF_OVERLOADED_METHOD(volumeAdjustmentIndex, short (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setVolumeAdjustmentIndex, void (cl::*)(short, cl::ChannelType))
      .DEF_OVERLOADED_METHOD(volumeAdjustment, float (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setVolumeAdjustment, void (cl::*)(float, cl::ChannelType))
      .DEF_OVERLOADED_METHOD(peakVolume, cl::PeakVolume (cl::*)(cl::ChannelType) const)
      .DEF_OVERLOADED_METHOD(setPeakVolume, void (cl::*)(const cl::PeakVolume &, cl::ChannelType))
      ;
  }

  {
    typedef ID3v2::TextIdentificationFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_TextIdentificationFrame", init<const ByteVector &, optional<String::Type> >())
      .def("setText", (void (cl::*)(const String &)) &cl::setText)
      .def("setText", (void (cl::*)(const StringList &)) &cl::setText)
      .DEF_SIMPLE_METHOD(textEncoding)
      .DEF_SIMPLE_METHOD(setTextEncoding)
      .DEF_SIMPLE_METHOD(fieldList)
      ;
  }

  {
    typedef ID3v2::UserTextIdentificationFrame cl;
    class_<cl, bases<ID3v2::TextIdentificationFrame>, boost::noncopyable>
      ("id3v2_UserTextIdentificationFrame", init<const ByteVector &, optional<String::Type> >())
      .DEF_SIMPLE_METHOD(description)
      .DEF_SIMPLE_METHOD(setDescription)
      .DEF_SIMPLE_METHOD(fieldList)
      ;
  }

  {
    typedef ID3v2::UniqueFileIdentifierFrame cl;
    class_<cl, bases<ID3v2::Frame>, boost::noncopyable>
      ("id3v2_UniqueFileIdentifierFrame", init<const ByteVector &>())
      .def(init<const String &, const ByteVector &>())
      .DEF_SIMPLE_METHOD(owner)
      .DEF_SIMPLE_METHOD(setOwner)
      .DEF_SIMPLE_METHOD(identifier)
      .DEF_SIMPLE_METHOD(setIdentifier)
      ;
  }
  // -------------------------------------------------------------
  // MPEG
  // -------------------------------------------------------------
  enum_<MPEG::File::TagTypes>("mpeg_TagTypes")
    .value("NoTags", MPEG::File::NoTags)
    .value("ID3v1", MPEG::File::ID3v1)
    .value("ID3v2", MPEG::File::ID3v2)
    .value("APE", MPEG::File::APE)
    .value("AllTags", MPEG::File::AllTags)
    ;
  class_<MPEG::File, boost::noncopyable>("mpeg_File",
                     init<const char *, optional<bool, AudioProperties::ReadStyle> >())
    .def(init<const char *, ID3v2::FrameFactory *, optional<bool, AudioProperties::ReadStyle> >())
    .def("save", 
         (bool (MPEG::File::*)(int, bool))
         &MPEG::File::save,
         save_overloads())
    .def("ID3v1Tag", 
         (ID3v1::Tag *(MPEG::File::*)(bool))
         &MPEG::File::ID3v1Tag,
         ID3v1Tag_overloads()[return_internal_reference<>()])
    .def("ID3v2Tag", 
         (ID3v2::Tag *(MPEG::File::*)(bool))
         &MPEG::File::ID3v2Tag,
         ID3v2Tag_overloads()[return_internal_reference<>()])
    .def("APETag", 
         (APE::Tag *(MPEG::File::*)(bool))
         &MPEG::File::APETag,
         APETag_overloads()[return_internal_reference<>()])
    .def("strip", 
         (bool (MPEG::File::*)(int))
         &MPEG::File::strip,
         strip_overloads())
    .def("setID3v2FrameFactory", &MPEG::File::setID3v2FrameFactory)
    .def("firstFrameOffset", &MPEG::File::firstFrameOffset)
    .def("nextFrameOffset", &MPEG::File::nextFrameOffset)
    .def("previousFrameOffset", &MPEG::File::previousFrameOffset)
    .def("lastFrameOffset", &MPEG::File::lastFrameOffset)
    ;

  // -------------------------------------------------------------
  // FLAC
  // -------------------------------------------------------------
  class_<FLAC::File, boost::noncopyable>("flac_File",
                     init<const char *, optional<bool, AudioProperties::ReadStyle> >())
    .def(init<const char *, ID3v2::FrameFactory *, optional<bool, AudioProperties::ReadStyle> >())
    .def("save", &FLAC::File::save)
    .def("ID3v1Tag", 
         (ID3v1::Tag *(FLAC::File::*)(bool))
         &FLAC::File::ID3v1Tag,
         ID3v1Tag_overloads()[return_internal_reference<>()])
    .def("ID3v2Tag", 
         (ID3v2::Tag *(FLAC::File::*)(bool))
         &FLAC::File::ID3v2Tag,
         ID3v2Tag_overloads()[return_internal_reference<>()])
    .def("xiphComment", 
         (Ogg::XiphComment *(FLAC::File::*)(bool))
         &FLAC::File::xiphComment,
         xiphComment_overloads()[return_internal_reference<>()])
    .def("setID3v2FrameFactory", &FLAC::File::setID3v2FrameFactory)
    .def("streamInfoData", &FLAC::File::streamInfoData)
    .def("streamLength", &FLAC::File::streamLength)
    ;

  // -------------------------------------------------------------
  // MPC
  // -------------------------------------------------------------
  enum_<MPC::File::TagTypes>("mpc_TagTypes")
    .value("NoTags", MPC::File::NoTags)
    .value("ID3v1", MPC::File::ID3v1)
    .value("ID3v2", MPC::File::ID3v2)
    .value("APE", MPC::File::APE)
    .value("AllTags", MPC::File::AllTags)
    ;
  class_<MPC::File, boost::noncopyable>("mpc_File",
                                        init<const char *, optional<bool, AudioProperties::ReadStyle> >())
    .def("save", &MPC::File::save)
    .def("ID3v1Tag", 
         (ID3v1::Tag *(MPEG::File::*)(bool))
         &MPEG::File::ID3v1Tag,
         ID3v1Tag_overloads()[return_internal_reference<>()])
    .def("APETag", 
         (APE::Tag *(MPEG::File::*)(bool))
         &MPEG::File::APETag,
         APETag_overloads()[return_internal_reference<>()])
    .def("remove", 
         (bool (MPEG::File::*)(int))
         &MPC::File::remove,
         strip_overloads())
    ;
}




// EMACS-FORMAT-TAG
//
// Local Variables:
// mode: C++
// eval: (c-set-style "stroustrup")
// eval: (c-set-offset 'access-label -2)
// eval: (c-set-offset 'inclass '++)
// c-basic-offset: 2
// tab-width: 8
// End:
