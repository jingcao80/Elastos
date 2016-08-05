#ifndef __ELASTOS_DROID_GOOGLE_MMS_CONTENTTYPE_H__
#define __ELASTOS_DROID_GOOGLE_MMS_CONTENTTYPE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

class ContentType
    : public Object
    , public IContentType
{
public:
    static CARAPI_(Boolean) IsSupportedType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsSupportedImageType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsSupportedAudioType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsSupportedVideoType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsTextType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsImageType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsAudioType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsVideoType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsDrmType(
        /* [in] */ const String& contentType);

    static CARAPI_(Boolean) IsUnspecified(
        /* [in] */ const String& contentType);

    // @SuppressWarnings("unchecked") String
    static CARAPI_(AutoPtr<IArrayList>) GetImageTypes();

    // @SuppressWarnings("unchecked") String
    static CARAPI_(AutoPtr<IArrayList>) GetAudioTypes();

    // @SuppressWarnings("unchecked") String
    static CARAPI_(AutoPtr<IArrayList>) GetVideoTypes();

    // @SuppressWarnings("unchecked") String
    static CARAPI_(AutoPtr<IArrayList>) GetSupportedTypes();

private:
    // This class should never be instantiated.
    ContentType();

//public:
//    static const String MMS_MESSAGE;
//    // The phony content type for generic PDUs (e.g. ReadOrig.ind,
//    // Notification.ind, Delivery.ind).
//    static const String MMS_GENERIC;
//    static const String MULTIPART_MIXED;
//    static const String MULTIPART_RELATED;
//    static const String MULTIPART_ALTERNATIVE;
//    static const String TEXT_PLAIN;
//    static const String TEXT_HTML;
//    static const String TEXT_VCALENDAR;
//    static const String TEXT_VCARD;
//    static const String IMAGE_UNSPECIFIED;
//    static const String IMAGE_JPEG;
//    static const String IMAGE_JPG;
//    static const String IMAGE_GIF;
//    static const String IMAGE_WBMP;
//    static const String IMAGE_PNG;
//    static const String IMAGE_X_MS_BMP;
//    static const String AUDIO_UNSPECIFIED;
//    static const String AUDIO_AAC;
//    static const String AUDIO_AAC_MP4;
//    static const String AUDIO_QCELP;
//    static const String AUDIO_EVRC;
//    static const String AUDIO_AMR_WB;
//    static const String AUDIO_AMR;
//    static const String AUDIO_IMELODY;
//    static const String AUDIO_MID;
//    static const String AUDIO_MIDI;
//    static const String AUDIO_MP3;
//    static const String AUDIO_MPEG3;
//    static const String AUDIO_MPEG;
//    static const String AUDIO_MPG;
//    static const String AUDIO_MP4;
//    static const String AUDIO_X_MID;
//    static const String AUDIO_X_MIDI;
//    static const String AUDIO_X_MP3;
//    static const String AUDIO_X_MPEG3;
//    static const String AUDIO_X_MPEG;
//    static const String AUDIO_X_MPG;
//    static const String AUDIO_3GPP;
//    static const String AUDIO_X_WAV;
//    static const String AUDIO_OGG;
//    static const String VIDEO_UNSPECIFIED;
//    static const String VIDEO_3GPP;
//    static const String VIDEO_3G2;
//    static const String VIDEO_H263;
//    static const String VIDEO_MP4;
//    static const String APP_SMIL;
//    static const String APP_WAP_XHTML;
//    static const String APP_XHTML;
//    static const String APP_DRM_CONTENT;
//    static const String APP_DRM_MESSAGE;

private:
    static AutoPtr<IArrayList> sSupportedContentTypes; // String
    static AutoPtr<IArrayList> sSupportedImageTypes; // String
    static AutoPtr<IArrayList> sSupportedAudioTypes; // String
    static AutoPtr<IArrayList> sSupportedVideoTypes; // String
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_CONTENTTYPE_H__
