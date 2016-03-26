
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__

#include "_Elastos.Droid.Server.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IRemoteDisplayListener;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

class ExtendedRemoteDisplayHelper
{
public:
    /**
     * Starts listening for displays to be connected on the specified interface.
     *
     * @param iface The interface address and port in the form "x.x.x.x:y".
     * @param listener The listener to invoke
     *         when displays are connected or disconnected.
     * @param handler The handler on which to invoke the listener.
     * @param context The current service context
     *  */
    static CARAPI_(AutoPtr<IInterface>) Listen(
        /* [in] */ const String& iface,
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler,
        /* [in] */ IContext* context);

    /**
     * Disconnects the remote display and stops listening for new connections.
     */
    static CARAPI_(void) Dispose(
        /* [in] */ IInterface* extRemoteDisplay);

    /**
     * Checks if ExtendedRemoteDisplay is available
     */
    static CARAPI_(Boolean) IsAvailable();
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__
