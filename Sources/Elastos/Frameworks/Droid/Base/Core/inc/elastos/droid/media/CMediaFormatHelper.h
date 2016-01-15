
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__

#include "_Elastos_Droid_Media_CMediaFormatHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaFormatHelper)
    , public Singleton
    , public IMediaFormatHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateAudioFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelCount,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal subtitle format.
     * @param mime The mime type of the content.
     * @param language The language of the content, using either ISO 639-1 or 639-2/T
     *        codes.  Specify null or "und" if language information is only included
     *        in the content.  (This will also work if there are multiple language
     *        tracks in the content.)
     */
    CARAPI CreateSubtitleFormat(
        /* [in] */ const String& mime,
        /* [in] */ const String& language,
        /* [out] */ IMediaFormat** result);

    /**
     * Creates a minimal video format.
     * @param mime The mime type of the content.
     * @param width The width of the content (in pixels)
     * @param height The height of the content (in pixels)
     */
    CARAPI CreateVideoFormat(
        /* [in] */ const String& mime,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ IMediaFormat** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAFORMATHELPER_H__
