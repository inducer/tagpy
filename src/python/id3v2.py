import _tagpy
FrameFactory = _tagpy.id3v2_FrameFactory
Frame = _tagpy.id3v2_Frame
FrameListMap = _tagpy.id3v2_FrameListMap
FrameList = _tagpy.id3v2_FrameList

Header = _tagpy.id3v2_Header
ExtendedHeader = _tagpy.id3v2_ExtendedHeader
Footer = _tagpy.id3v2_Footer

Tag = _tagpy.id3v2_Tag


AttachedPictureFrameType = _tagpy.id3v2_AttachedPictureFrame_Type
AttachedPictureFrame = _tagpy.id3v2_AttachedPictureFrame
CommentsFrame = _tagpy.id3v2_CommentsFrame
PeakVolume = _tagpy.id3v2_PeakVolume
ChannelType = _tagpy.id3v2_RelativeVolumeFrame_ChannelType
RelativeVolumeFrame = _tagpy.id3v2_RelativeVolumeFrame
TextIdentificationFrame = _tagpy.id3v2_TextIdentificationFrame
UserTextIdentificationFrame = _tagpy.id3v2_UserTextIdentificationFrame
UniqueFileIdentifierFrame = _tagpy.id3v2_UniqueFileIdentifierFrame
UnkownFrame = _tagpy.id3v2_UnknownFrame




# monkey-patch Tag ------------------------------------------------------------
def _tag_get_text_frame(self, id):
    flm_id = self.frameListMap[id]

    if flm_id:
        return flm_id[0].toString()
    else:
        return None

def _tag_set_text_frame(self, id, value):
    if not value:
        self.removeFrames(id)
        return

    from tagpy import StringType

    print "YAAA"
    flm_id = self.frameListMap[id]
    if flm_id:
        if isinstance(value, unicode):
            flm_id[0].setTextEncoding(StringType.UTF8)
        flm_id[0].setText(value)
    else:
        tif = TextIdentificationFrame(id, StringType.UTF8)
        self.addFrame(tif)
        tif.setText(value)


def _augment_Tag():
    Tag.getTextFrame = _tag_get_text_frame
    Tag.setTextFrame = _tag_set_text_frame

    for name, frame in [
            ("title", "TIT2"),
            ("artist", "TPE1"),
            ("album", "TALB"),
            ("comment", "TALB"),
            ]:
        setattr(Tag, name, property(
                lambda tag: tag.getTextFrame(frame), 
                lambda tag, value: tag.setTextFrame(frame)))
