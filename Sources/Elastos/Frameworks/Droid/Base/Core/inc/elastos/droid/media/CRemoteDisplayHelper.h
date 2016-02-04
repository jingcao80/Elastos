
#ifndef __ELASTOS_DROID_MEDIA_CREMOTEDISPLAYHELPER_H__
#define __ELASTOS_DROID_MEDIA_CREMOTEDISPLAYHELPER_H__

#include "_Elastos_Droid_Media_CRemoteDisplayHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRemoteDisplayHelper)
    , public Singleton
    , public IRemoteDisplayHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Starts listening for displays to be connected on the specified interface.
     *
     * @param iface The interface address and port in the form "x.x.x.x:y".
     * @param listener The listener to invoke when displays are connected or disconnected.
     * @param handler The handler on which to invoke the listener.
     */
    CARAPI Listen(
        /* [in] */ const String& iface,
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler,
        /* [out] */ IRemoteDisplay** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CREMOTEDISPLAYHELPER_H__
