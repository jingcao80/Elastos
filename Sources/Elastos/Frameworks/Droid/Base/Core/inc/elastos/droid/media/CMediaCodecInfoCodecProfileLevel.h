
#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoCodecProfileLevel.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoCodecProfileLevel)
    , public Object
    , public IMediaCodecInfoCodecProfileLevel
{
public:
    CMediaCodecInfoCodecProfileLevel();

    virtual ~CMediaCodecInfoCodecProfileLevel();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

public:
    /**
     * Defined in the OpenMAX IL specs, depending on the type of media
     * this can be OMX_VIDEO_AVCPROFILETYPE, OMX_VIDEO_H263PROFILETYPE,
     * OMX_VIDEO_MPEG4PROFILETYPE or OMX_VIDEO_VP8PROFILETYPE.
     */
    Int32 mProfile;

    /**
     * Defined in the OpenMAX IL specs, depending on the type of media
     * this can be OMX_VIDEO_AVCLEVELTYPE, OMX_VIDEO_H263LEVELTYPE
     * OMX_VIDEO_MPEG4LEVELTYPE or OMX_VIDEO_VP8LEVELTYPE.
     */
    Int32 mLevel;
};


} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECPROFILELEVEL_H__
