#ifndef __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__

#include "_Elastos_Droid_Media_CMediaCodecInfoCodecCapabilitiesHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaCodecInfoCodecCapabilitiesHelper)
    , public Singleton
    , public IMediaCodecInfoCodecCapabilitiesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Retrieve the codec capabilities for a certain {@code mime type}, {@code
     * profile} and {@code level}.  If the type, or profile-level combination
     * is not understood by the framework, it returns null.
     */
    CARAPI CreateFromProfileLevel(
        /* [in] */ const String& mime,
        /* [in] */ Int32 profile,
        /* [in] */ Int32 level,
        /* [out] */ IMediaCodecInfoCodecCapabilities** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIACODECINFOCODECCAPABILITIESHELPER_H__
