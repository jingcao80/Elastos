#include "elastos/droid/media/DecoderCapabilities.h"
#include "elastos/droid/media/MediaFile.h"
#include "elastos/droid/media/CMediaFileType.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Mtp::IMtpConstants;

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 MediaFile::FIRST_AUDIO_FILE_TYPE = IMediaFile::FILE_TYPE_MP3;
const Int32 MediaFile::LAST_AUDIO_FILE_TYPE = IMediaFile::FILE_TYPE_FLAC;
const Int32 MediaFile::FIRST_MIDI_FILE_TYPE = IMediaFile::FILE_TYPE_MID;
const Int32 MediaFile::LAST_MIDI_FILE_TYPE = IMediaFile::FILE_TYPE_IMY;
const Int32 MediaFile::FIRST_VIDEO_FILE_TYPE = IMediaFile::FILE_TYPE_MP4;
const Int32 MediaFile::LAST_VIDEO_FILE_TYPE = IMediaFile::FILE_TYPE_WEBM;
const Int32 MediaFile::FIRST_VIDEO_FILE_TYPE2 = IMediaFile::FILE_TYPE_MP2PS;
const Int32 MediaFile::LAST_VIDEO_FILE_TYPE2 = IMediaFile::FILE_TYPE_MP2PS;
// More audio file types
const Int32 MediaFile::FIRST_IMAGE_FILE_TYPE = IMediaFile::FILE_TYPE_JPEG;
const Int32 MediaFile::LAST_IMAGE_FILE_TYPE = IMediaFile::FILE_TYPE_WEBP;
const Int32 MediaFile::FIRST_PLAYLIST_FILE_TYPE = IMediaFile::FILE_TYPE_M3U;
const Int32 MediaFile::LAST_PLAYLIST_FILE_TYPE = IMediaFile::FILE_TYPE_HTTPLIVE;
const Int32 MediaFile::FIRST_DRM_FILE_TYPE = IMediaFile::FILE_TYPE_FL;
const Int32 MediaFile::LAST_DRM_FILE_TYPE = IMediaFile::FILE_TYPE_FL;

/*static*/ Boolean MediaFile::InitStatic()
{
    AddFileType(String("MP3"), IMediaFile::FILE_TYPE_MP3, String("audio/mpeg"), IMtpConstants::FORMAT_MP3);
    AddFileType(String("MPGA"), IMediaFile::FILE_TYPE_MP3, String("audio/mpeg"), IMtpConstants::FORMAT_MP3);
    AddFileType(String("M4A"), IMediaFile::FILE_TYPE_M4A, String("audio/mp4"), IMtpConstants::FORMAT_MPEG);
    AddFileType(String("WAV"), IMediaFile::FILE_TYPE_WAV, String("audio/x-wav"), IMtpConstants::FORMAT_WAV);
    AddFileType(String("AMR"), IMediaFile::FILE_TYPE_AMR, String("audio/amr"));
    AddFileType(String("AWB"), IMediaFile::FILE_TYPE_AWB, String("audio/amr-wb"));
    if (IsWMAEnabled()) {
        AddFileType(String("WMA"), IMediaFile::FILE_TYPE_WMA, String("audio/x-ms-wma"), IMtpConstants::FORMAT_WMA);
    }
    AddFileType(String("OGG"), IMediaFile::FILE_TYPE_OGG, String("audio/ogg"), IMtpConstants::FORMAT_OGG);
    AddFileType(String("OGG"), IMediaFile::FILE_TYPE_OGG, String("application/ogg"), IMtpConstants::FORMAT_OGG);
    AddFileType(String("OGA"), IMediaFile::FILE_TYPE_OGG, String("application/ogg"), IMtpConstants::FORMAT_OGG);
    AddFileType(String("AAC"), IMediaFile::FILE_TYPE_AAC, String("audio/aac"), IMtpConstants::FORMAT_AAC);
    AddFileType(String("AAC"), IMediaFile::FILE_TYPE_AAC, String("audio/aac-adts"), IMtpConstants::FORMAT_AAC);
    AddFileType(String("MKA"), IMediaFile::FILE_TYPE_MKA, String("audio/x-matroska"));

    AddFileType(String("MID"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("MIDI"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("XMF"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("RTTTL"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("SMF"), IMediaFile::FILE_TYPE_SMF, String("audio/sp-midi"));
    AddFileType(String("IMY"), IMediaFile::FILE_TYPE_IMY, String("audio/imelody"));
    AddFileType(String("RTX"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("OTA"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));
    AddFileType(String("MXMF"), IMediaFile::FILE_TYPE_MID, String("audio/midi"));

    AddFileType(String("MPEG"), IMediaFile::FILE_TYPE_MP4, String("video/mpeg"), IMtpConstants::FORMAT_MPEG);
    AddFileType(String("MPG"), IMediaFile::FILE_TYPE_MP4, String("video/mpeg"), IMtpConstants::FORMAT_MPEG);
    AddFileType(String("MP4"), IMediaFile::FILE_TYPE_MP4, String("video/mp4"), IMtpConstants::FORMAT_MPEG);
    AddFileType(String("M4V"), IMediaFile::FILE_TYPE_M4V, String("video/mp4"), IMtpConstants::FORMAT_MPEG);
    AddFileType(String("3GP"), IMediaFile::FILE_TYPE_3GPP, String("video/3gpp"),  IMtpConstants::FORMAT_3GP_CONTAINER);
    AddFileType(String("3GPP"), IMediaFile::FILE_TYPE_3GPP, String("video/3gpp"), IMtpConstants::FORMAT_3GP_CONTAINER);
    AddFileType(String("3G2"), IMediaFile::FILE_TYPE_3GPP2, String("video/3gpp2"), IMtpConstants::FORMAT_3GP_CONTAINER);
    AddFileType(String("3GPP2"), IMediaFile::FILE_TYPE_3GPP2, String("video/3gpp2"), IMtpConstants::FORMAT_3GP_CONTAINER);
    AddFileType(String("MKV"), IMediaFile::FILE_TYPE_MKV, String("video/x-matroska"));
    AddFileType(String("WEBM"), IMediaFile::FILE_TYPE_WEBM, String("video/webm"));
    AddFileType(String("TS"), IMediaFile::FILE_TYPE_MP2TS, String("video/mp2ts"));
    AddFileType(String("AVI"), IMediaFile::FILE_TYPE_AVI, String("video/avi"));

    if (IsWMVEnabled()) {
        AddFileType(String("WMV"), IMediaFile::FILE_TYPE_WMV, String("video/x-ms-wmv"), IMtpConstants::FORMAT_WMV);
        AddFileType(String("ASF"), IMediaFile::FILE_TYPE_ASF, String("video/x-ms-asf"));
    }

    AddFileType(String("JPG"), IMediaFile::FILE_TYPE_JPEG, String("image/jpeg"), IMtpConstants::FORMAT_EXIF_JPEG);
    AddFileType(String("JPEG"), IMediaFile::FILE_TYPE_JPEG, String("image/jpeg"), IMtpConstants::FORMAT_EXIF_JPEG);
    AddFileType(String("GIF"), IMediaFile::FILE_TYPE_GIF, String("image/gif"), IMtpConstants::FORMAT_GIF);
    AddFileType(String("PNG"), IMediaFile::FILE_TYPE_PNG, String("image/png"), IMtpConstants::FORMAT_PNG);
    AddFileType(String("BMP"), IMediaFile::FILE_TYPE_BMP, String("image/x-ms-bmp"), IMtpConstants::FORMAT_BMP);
    AddFileType(String("WBMP"), IMediaFile::FILE_TYPE_WBMP, String("image/vnd.wap.wbmp"));
    AddFileType(String("WEBP"), IMediaFile::FILE_TYPE_WEBP, String("image/webp"));

    AddFileType(String("M3U"), IMediaFile::FILE_TYPE_M3U, String("audio/x-mpegurl"), IMtpConstants::FORMAT_M3U_PLAYLIST);
    AddFileType(String("M3U"), IMediaFile::FILE_TYPE_M3U, String("application/x-mpegurl"), IMtpConstants::FORMAT_M3U_PLAYLIST);
    AddFileType(String("PLS"), IMediaFile::FILE_TYPE_PLS, String("audio/x-scpls"), IMtpConstants::FORMAT_PLS_PLAYLIST);
    AddFileType(String("WPL"), IMediaFile::FILE_TYPE_WPL, String("application/vnd.ms-wpl"), IMtpConstants::FORMAT_WPL_PLAYLIST);
    AddFileType(String("M3U8"), IMediaFile::FILE_TYPE_HTTPLIVE, String("application/vnd.apple.mpegurl"));
    AddFileType(String("M3U8"), IMediaFile::FILE_TYPE_HTTPLIVE, String("audio/mpegurl"));
    AddFileType(String("M3U8"), IMediaFile::FILE_TYPE_HTTPLIVE, String("audio/x-mpegurl"));

    AddFileType(String("FL"), IMediaFile::FILE_TYPE_FL, String("application/x-android-drm-fl"));

    AddFileType(String("TXT"), IMediaFile::FILE_TYPE_TEXT, String("text/plain"), IMtpConstants::FORMAT_TEXT);
    AddFileType(String("HTM"), IMediaFile::FILE_TYPE_HTML, String("text/html"), IMtpConstants::FORMAT_HTML);
    AddFileType(String("HTML"), IMediaFile::FILE_TYPE_HTML, String("text/html"), IMtpConstants::FORMAT_HTML);
    AddFileType(String("PDF"), IMediaFile::FILE_TYPE_PDF, String("application/pdf"));
    AddFileType(String("DOC"), IMediaFile::FILE_TYPE_MS_WORD, String("application/msword"), IMtpConstants::FORMAT_MS_WORD_DOCUMENT);
    AddFileType(String("XLS"), IMediaFile::FILE_TYPE_MS_EXCEL, String("application/vnd.ms-excel"), IMtpConstants::FORMAT_MS_EXCEL_SPREADSHEET);
    AddFileType(String("PPT"), IMediaFile::FILE_TYPE_MS_POWERPOINT, String("application/mspowerpoint"), IMtpConstants::FORMAT_MS_POWERPOINT_PRESENTATION);
    AddFileType(String("FLAC"), IMediaFile::FILE_TYPE_FLAC, String("audio/flac"), IMtpConstants::FORMAT_FLAC);
    AddFileType(String("ZIP"), IMediaFile::FILE_TYPE_ZIP, String("application/zip"));
    AddFileType(String("MPG"), IMediaFile::FILE_TYPE_MP2PS, String("video/mp2p"));
    AddFileType(String("MPEG"), IMediaFile::FILE_TYPE_MP2PS, String("video/mp2p"));

    return TRUE;
}

HashMap< String, AutoPtr<IMediaFileType> > MediaFile::sFileTypeMap;
HashMap< String, Int32 > MediaFile::sMimeTypeMap;
HashMap< String, Int32 > MediaFile::sFileTypeToFormatMap;
HashMap< String, Int32 > MediaFile::sMimeTypeToFormatMap;
HashMap< Int32, String > MediaFile::sFormatToMimeTypeMap;
Boolean MediaFile::mInit= MediaFile::InitStatic();

ECode MediaFile::AddFileType(
    /* [in] */ const String& extension,
    /* [in] */ Int32 fileType,
    /* [in] */ const String& mimeType)
{
    AutoPtr<IMediaFileType> mediaFileType;
    CMediaFileType::New(fileType, mimeType, (IMediaFileType**)&mediaFileType);
    sFileTypeMap[extension] = mediaFileType.Get();
    sMimeTypeMap[mimeType] = fileType;
    return NOERROR;
}

ECode MediaFile::AddFileType(
    /* [in] */ const String& extension,
    /* [in] */ Int32 fileType,
    /* [in] */ const String& mimeType,
    /* [in] */ Int32 mtpFormatCode)
{
    AddFileType(extension, fileType, mimeType);
    sFileTypeToFormatMap[extension] = mtpFormatCode;
    sMimeTypeToFormatMap[mimeType] = mtpFormatCode;
    sFormatToMimeTypeMap[mtpFormatCode] = mimeType;
    return NOERROR;
}

ECode MediaFile::IsAudioFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((fileType >= FIRST_AUDIO_FILE_TYPE &&
               fileType <= LAST_AUDIO_FILE_TYPE) ||
               (fileType >= FIRST_MIDI_FILE_TYPE &&
               fileType <= LAST_MIDI_FILE_TYPE));
    return NOERROR;
}

ECode MediaFile::IsVideoFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (fileType >= FIRST_VIDEO_FILE_TYPE &&
               fileType <= LAST_VIDEO_FILE_TYPE) ||
               (fileType >= FIRST_VIDEO_FILE_TYPE2 &&
               fileType <= LAST_VIDEO_FILE_TYPE2);
    return NOERROR;
}

ECode MediaFile::IsImageFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (fileType >= FIRST_IMAGE_FILE_TYPE &&
               fileType <= LAST_IMAGE_FILE_TYPE);
    return NOERROR;
}

ECode MediaFile::IsPlayListFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (fileType >= FIRST_PLAYLIST_FILE_TYPE &&
               fileType <= LAST_PLAYLIST_FILE_TYPE);
    return NOERROR;
}

ECode MediaFile::IsDrmFileType(
    /* [in] */ Int32 fileType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (fileType >= FIRST_DRM_FILE_TYPE &&
               fileType <= LAST_DRM_FILE_TYPE);
    return NOERROR;
}

ECode MediaFile::GetFileType(
    /* [in] */ const String& path,
    /* [out] */ IMediaFileType** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 lastDot = path.LastIndexOf('.');
    if (lastDot < 0) {
        return NOERROR;
    }

    String tempText = path.Substring(lastDot + 1).ToUpperCase();
    HashMap< String, AutoPtr<IMediaFileType> >::Iterator it = sFileTypeMap.Find(tempText);
    if (it != sFileTypeMap.End()) {
        *result = it->mSecond;
        REFCOUNT_ADD(*result);
    }

    return NOERROR;
}

ECode MediaFile::IsMimeTypeMedia(
    /* [in] */ const String& mimeType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 fileType;
    GetFileTypeForMimeType(mimeType, &fileType);
    Boolean tempState1, tempState2, tempState3, tempState4;
    *result = (IsAudioFileType(fileType, &tempState1), tempState1)
        || (IsVideoFileType(fileType, &tempState2), tempState2)
        || (IsImageFileType(fileType, &tempState3), tempState3)
        || (IsPlayListFileType(fileType, &tempState4), tempState4);
    return NOERROR;
}

ECode MediaFile::GetFileTitle(
    /* [in] */ const String& path,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    // extract file name after last slash
    Int32 lastSlash = path.LastIndexOf('/');
    if (lastSlash >= 0) {
        lastSlash++;
        if (lastSlash < path.GetLength()) {
            *result = path.Substring(lastSlash);
        }
    }
    // truncate the file extension (if any)
    Int32 lastDot = path.LastIndexOf('.');
    if (lastDot > 0) {
        *result = path.Substring(0, lastDot);
    }
    return NOERROR;
}

ECode MediaFile::GetFileTypeForMimeType(
    /* [in] */ const String& mimeType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    HashMap< String, Int32 >::Iterator it = sMimeTypeMap.Find(mimeType);
    if (it != sMimeTypeMap.End()) {
        *result = it->mSecond;
    }
    return NOERROR;
}

ECode MediaFile::GetMimeTypeForFile(
    /* [in] */ const String& path,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMediaFileType> mediaFileType;
    GetFileType(path, (IMediaFileType**)&mediaFileType);
    if (mediaFileType != NULL) {
        mediaFileType->GetMimeType(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode MediaFile::GetFormatCode(
    /* [in] */ const String& fileName,
    /* [in] */ const String& mimeType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (mimeType != NULL) {
        HashMap< String, Int32 >::Iterator it = sMimeTypeToFormatMap.Find(mimeType);
        if (it != sMimeTypeToFormatMap.End()) {
            if (it->mSecond != 0) {
                *result = it->mSecond;
                return NOERROR;
            }
        }
    }

    Int32 lastDot = fileName.LastIndexOf('.');
    if (lastDot > 0) {
        String extension = fileName.Substring(lastDot + 1).ToUpperCase();
        HashMap< String, Int32 >::Iterator it= sFileTypeToFormatMap.Find(extension);
        if (it != sFileTypeToFormatMap.End()) {
            if (it->mSecond != 0) {
                *result = it->mSecond;
                return NOERROR;
            }
        }
    }

    *result = IMtpConstants::FORMAT_UNDEFINED;
    return NOERROR;
}

ECode MediaFile::GetMimeTypeForFormatCode(
    /* [in] */ Int32 formatCode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    HashMap< Int32, String >::Iterator it = sFormatToMimeTypeMap.Find(formatCode);
    if (it != sFormatToMimeTypeMap.End()) {
        *result = it->mSecond;
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

Boolean MediaFile::IsWMAEnabled()
{
    AutoPtr<ArrayOf<AudioDecoder> > ads;
    DecoderCapabilities::GetAudioDecoders((ArrayOf<AudioDecoder>**)&ads);

    if (ads != NULL) {
        for (Int32 i = 0; i < ads->GetLength(); ++i) {
            if ((*ads)[i] == AUDIO_DECODER_WMA)
                return TRUE;
        }
    }
    return FALSE;
}

Boolean MediaFile::IsWMVEnabled()
{
    AutoPtr<ArrayOf<VideoDecoder> > vds;
    DecoderCapabilities::GetVideoDecoders((ArrayOf<AudioDecoder>**)&vds);

    if (vds != NULL) {
        for (Int32 i = 0; i < vds->GetLength(); ++i) {
            if ((*vds)[i] == VIDEO_DECODER_WMV)
                return TRUE;
        }
    }
    return FALSE;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
