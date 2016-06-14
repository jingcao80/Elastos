#include "Elastos.Droid.Google.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/google/mms/ContentType.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

//=====================================================================
//                             ContentType
//=====================================================================
const String ContentType::MMS_MESSAGE("application/vnd.wap.mms-message");
const String ContentType::MMS_GENERIC("application/vnd.wap.mms-generic");
const String ContentType::MULTIPART_MIXED("application/vnd.wap.multipart.mixed");
const String ContentType::MULTIPART_RELATED("application/vnd.wap.multipart.related");
const String ContentType::MULTIPART_ALTERNATIVE("application/vnd.wap.multipart.alternative");
const String ContentType::TEXT_PLAIN("text/plain");
const String ContentType::TEXT_HTML("text/html");
const String ContentType::TEXT_VCALENDAR("text/x-vCalendar");
const String ContentType::TEXT_VCARD("text/x-vCard");
const String ContentType::IMAGE_UNSPECIFIED("image/*");
const String ContentType::IMAGE_JPEG("image/jpeg");
const String ContentType::IMAGE_JPG("image/jpg");
const String ContentType::IMAGE_GIF("image/gif");
const String ContentType::IMAGE_WBMP("image/vnd.wap.wbmp");
const String ContentType::IMAGE_PNG("image/png");
const String ContentType::IMAGE_X_MS_BMP("image/x-ms-bmp");
const String ContentType::AUDIO_UNSPECIFIED("audio/*");
const String ContentType::AUDIO_AAC("audio/aac");
const String ContentType::AUDIO_AAC_MP4("audio/aac_mp4");
const String ContentType::AUDIO_QCELP("audio/qcelp");
const String ContentType::AUDIO_EVRC("audio/evrc");
const String ContentType::AUDIO_AMR_WB("audio/amr-wb");
const String ContentType::AUDIO_AMR("audio/amr");
const String ContentType::AUDIO_IMELODY("audio/imelody");
const String ContentType::AUDIO_MID("audio/mid");
const String ContentType::AUDIO_MIDI("audio/midi");
const String ContentType::AUDIO_MP3("audio/mp3");
const String ContentType::AUDIO_MPEG3("audio/mpeg3");
const String ContentType::AUDIO_MPEG("audio/mpeg");
const String ContentType::AUDIO_MPG("audio/mpg");
const String ContentType::AUDIO_MP4("audio/mp4");
const String ContentType::AUDIO_X_MID("audio/x-mid");
const String ContentType::AUDIO_X_MIDI("audio/x-midi");
const String ContentType::AUDIO_X_MP3("audio/x-mp3");
const String ContentType::AUDIO_X_MPEG3("audio/x-mpeg3");
const String ContentType::AUDIO_X_MPEG("audio/x-mpeg");
const String ContentType::AUDIO_X_MPG("audio/x-mpg");
const String ContentType::AUDIO_3GPP("audio/3gpp");
const String ContentType::AUDIO_X_WAV("audio/x-wav");
const String ContentType::AUDIO_OGG("application/ogg");
const String ContentType::VIDEO_UNSPECIFIED("video/*");
const String ContentType::VIDEO_3GPP("video/3gpp");
const String ContentType::VIDEO_3G2("video/3gpp2");
const String ContentType::VIDEO_H263("video/h263");
const String ContentType::VIDEO_MP4("video/mp4");
const String ContentType::APP_SMIL("application/smil");
const String ContentType::APP_WAP_XHTML("application/vnd.wap.xhtml+xml");
const String ContentType::APP_XHTML("application/xhtml+xml");
const String ContentType::APP_DRM_CONTENT("application/vnd.oma.drm.content");
const String ContentType::APP_DRM_MESSAGE("application/vnd.oma.drm.message");

static AutoPtr<IArrayList> InitArrayList()
{
    AutoPtr<IArrayList> array;
    CArrayList::New((IArrayList**)&array);
    return array;
}

AutoPtr<IArrayList> ContentType::sSupportedContentTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedImageTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedAudioTypes = InitArrayList();
AutoPtr<IArrayList> ContentType::sSupportedVideoTypes = InitArrayList();

Boolean ContentType::IsSupportedType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && sSupportedContentTypes.contains(contentType);
    assert(0);
    return FALSE;
}

Boolean ContentType::IsSupportedImageType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return isImageType(contentType) && isSupportedType(contentType);
    assert(0);
    return FALSE;
}

Boolean ContentType::IsSupportedAudioType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return isAudioType(contentType) && isSupportedType(contentType);
    assert(0);
    return FALSE;
}

Boolean ContentType::IsSupportedVideoType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return isVideoType(contentType) && isSupportedType(contentType);
    assert(0);
    return FALSE;
}

Boolean ContentType::IsTextType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && contentType.startsWith("text/");
    assert(0);
    return FALSE;
}

Boolean ContentType::IsImageType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && contentType.startsWith("image/");
    assert(0);
    return FALSE;
}

Boolean ContentType::IsAudioType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && contentType.startsWith("audio/");
    assert(0);
    return FALSE;
}

Boolean ContentType::IsVideoType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && contentType.startsWith("video/");
    assert(0);
    return FALSE;
}

Boolean ContentType::IsDrmType(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType)
    //         && (contentType.equals(APP_DRM_CONTENT)
    //                 || contentType.equals(APP_DRM_MESSAGE));
    assert(0);
    return FALSE;
}

Boolean ContentType::IsUnspecified(
    /* [in] */ const String& contentType)
{
    // ==================before translated======================
    // return (null != contentType) && contentType.endsWith("*");
    assert(0);
    return FALSE;
}

AutoPtr<IArrayList> ContentType::GetImageTypes()
{
    // ==================before translated======================
    // return (ArrayList<String>) sSupportedImageTypes.clone();
    assert(0);
    AutoPtr<IArrayList> empty;
    return empty;
}

AutoPtr<IArrayList> ContentType::GetAudioTypes()
{
    // ==================before translated======================
    // return (ArrayList<String>) sSupportedAudioTypes.clone();
    assert(0);
    AutoPtr< IArrayList> empty;
    return empty;
}

AutoPtr<IArrayList> ContentType::GetVideoTypes()
{
    // ==================before translated======================
    // return (ArrayList<String>) sSupportedVideoTypes.clone();
    assert(0);
    AutoPtr< IArrayList> empty;
    return empty;
}

AutoPtr<IArrayList> ContentType::GetSupportedTypes()
{
    // ==================before translated======================
    // return (ArrayList<String>) sSupportedContentTypes.clone();
    assert(0);
    AutoPtr< IArrayList> empty;
    return empty;
}

ContentType::ContentType()
{
}

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
