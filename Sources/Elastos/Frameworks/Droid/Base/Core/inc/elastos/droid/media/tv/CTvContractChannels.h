#ifndef __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__
#define __ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__

#include "_Elastos_Droid_Media_Tv_CTvContractChannels.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Column definitions for the TV channels table. */
CarClass(CTvContractChannels)
    , public Singleton
    , public ITvContractChannels
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns the video resolution (definition) for a given video format.
     *
     * @param videoFormat The video format defined in {@link Channels}.
     * @return the corresponding video resolution string. {@code null} if the resolution string
     *         is not defined for the given video format.
     * @see #COLUMN_VIDEO_FORMAT
     */
    CARAPI GetVideoResolution(
        /* [in] */ const String& videoFormat,
        /* [out] */ String* result);
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_CTVCONTRACTCHANNELS_H__
