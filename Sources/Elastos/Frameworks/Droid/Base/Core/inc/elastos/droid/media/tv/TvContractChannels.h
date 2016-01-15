#ifndef __ELASTOS_DROID_MEDIA_TV_TVCONTRACTCHANNELS_H__
#define __ELASTOS_DROID_MEDIA_TV_TVCONTRACTCHANNELS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Tv {

/** Column definitions for the TV channels table. */
class TvContractChannels
{
public:
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Returns the video resolution (definition) for a given video format.
     *
     * @param videoFormat The video format defined in {@link Channels}.
     * @return the corresponding video resolution string. {@code null} if the resolution string
     *         is not defined for the given video format.
     * @see #COLUMN_VIDEO_FORMAT
     */
    static CARAPI GetVideoResolution(
        /* [in] */ const String& videoFormat,
        /* [out] */ String * result);

private:
    TvContractChannels();

private:
    static AutoPtr<IHashMap> VIDEO_FORMAT_TO_RESOLUTION_MAP;
};

} // namespace Tv
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_TV_TVCONTRACTCHANNELS_H__
