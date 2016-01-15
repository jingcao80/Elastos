#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONHELPER_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONHELPER_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

CarClass(CMediaSessionHelper)
    , public Singleton
    , public IMediaSessionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Return true if this is considered an active playback state.
     *
     * @hide
     */
    CARAPI IsActiveState(
        /* [in] */ Int32 state,
        /* [out] */ Boolean * result);
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONHELPER_H__
