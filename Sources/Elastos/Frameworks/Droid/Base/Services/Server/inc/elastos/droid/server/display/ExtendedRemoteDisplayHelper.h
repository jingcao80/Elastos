
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
    static CARAPI Listen(
        /* [in] */ const String& iface,
        /* [in] */ IRemoteDisplayListener* listener,
        /* [in] */ IHandler* handler,
        /* [in] */ IContext* context,
        /* [out] */ IInterface** result);

    /**
     * Disconnects the remote display and stops listening for new connections.
     */
    static CARAPI Dispose(
        /* [in] */ IInterface* extRemoteDisplay);

    /**
     * Checks if ExtendedRemoteDisplay is available
     */
    static CARAPI_(Boolean) IsAvailable();

private:
    static CARAPI_(Boolean) InitExtRemoteDisplay();

private:
    static const String TAG;

    // ExtendedRemoteDisplay class
    // ExtendedRemoteDisplay is an enhanced RemoteDisplay. It has
    // similar interface as RemoteDisplay class
    static AutoPtr<IClassInfo> sExtRemoteDisplayClass;

    // Method object for the API ExtendedRemoteDisplay.Listen
    // ExtendedRemoteDisplay.Listen has the same API signature as
    // RemoteDisplay.Listen except for an additional argument to pass the
    // Context
    static AutoPtr<IMethodInfo> sExtRemoteDisplayListen;

    // Method Object for the API ExtendedRemoteDisplay.Dispose
    // ExtendedRemoteDisplay.Dispose follows the same API signature as
    // RemoteDisplay.Dispose
    static AutoPtr<IMethodInfo> sExtRemoteDisplayDispose;
    static Boolean sInit;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_EXTENDEDREMOTEDISPLAYHELPER_H__
