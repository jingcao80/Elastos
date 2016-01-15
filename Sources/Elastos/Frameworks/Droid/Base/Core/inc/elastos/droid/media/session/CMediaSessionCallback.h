#ifndef __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACK_H__
#define __ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACK_H__

#include "_Elastos_Droid_Media_Session_CMediaSessionCallback.h"
#include "elastos/droid/media/session/MediaSessionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Session {

/**
 * Receives media buttons, transport controls, and commands from controllers
 * and the system. A callback may be set using {@link #setCallback}.
 */
CarClass(CMediaSessionCallback)
    , public MediaSessionCallback
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Session
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_SESSION_CMEDIASESSIONCALLBACK_H__
