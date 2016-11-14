
#ifndef __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYADAPTER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYADAPTER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>

using Elastos::IO::IPrintWriter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

class DisplayDevice;

/**
 * A display adapter makes zero or more display devices available to the system
 * and provides facilities for discovering when displays are connected or disconnected.
 * <p>
 * For now, all display adapters are registered in the system server but
 * in principle it could be done from other processes.
 * </p><p>
 * Display adapters are guarded by the {@link DisplayManagerService.SyncRoot} lock.
 * </p>
 */
class DisplayAdapter
    : public Object
{
private:
    class SendDisplayDeviceEventRunnable
        : public Runnable
    {
    public:
        SendDisplayDeviceEventRunnable(
            /* [in] */ DisplayDevice* device,
            /* [in] */ Int32 event,
            /* [in] */ DisplayAdapter* host);

        CARAPI Run();

    private:
        AutoPtr<DisplayDevice> mDevice;
        Int32 mEvent;
        DisplayAdapter* mHost;
    };

    class SendTraversalRequestRunnable
        : public Runnable
    {
    public:
        SendTraversalRequestRunnable(
            /* [in] */ DisplayAdapter* host);

        CARAPI Run();

    private:
        DisplayAdapter* mHost;
    };

public:
    static const Int32 DISPLAY_DEVICE_EVENT_ADDED = 1;
    static const Int32 DISPLAY_DEVICE_EVENT_CHANGED = 2;
    static const Int32 DISPLAY_DEVICE_EVENT_REMOVED = 3;

public:
    // Called with SyncRoot lock held.
    DisplayAdapter(
        /* [in] */ Object* syncRoot,
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IDisplayAdapterListener* listener,
        /* [in] */ const String& name);

    virtual ~DisplayAdapter();

    /**
     * Gets the object that the display adapter should synchronize on when handling
     * calls that come in from outside of the display manager service.
     */
    CARAPI_(Object*) GetSyncRoot();

    /**
     * Gets the display adapter's context.
     */
    CARAPI_(AutoPtr<IContext>) GetContext();

    /**
     * Gets a handler that the display adapter may use to post asynchronous messages.
     */
    CARAPI_(AutoPtr<IHandler>) GetHandler();

    /**
     * Gets the display adapter name for debugging purposes.
     */
    CARAPI_(String) GetName();

    /**
     * Registers the display adapter with the display manager.
     *
     * The display adapter should register any built-in display devices as soon as possible.
     * The boot process will wait for the default display to be registered.
     * Other display devices can be registered dynamically later.
     */
    virtual CARAPI_(void) RegisterLocked();

    /**
     * Dumps the local state of the display adapter.
     */
    virtual CARAPI_(void) DumpLocked(
        /* [in] */ IPrintWriter* pw);

protected:
    /**
     * Sends a display device event to the display adapter listener asynchronously.
     */
    CARAPI_(void) SendDisplayDeviceEventLocked(
        /* [in] */ DisplayDevice* device,
        /* [in] */ Int32 event);

    /**
     * Sends a request to perform traversals.
     */
    CARAPI_(void) SendTraversalRequestLocked();

private:
    Object* mSyncRoot;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IDisplayAdapterListener> mListener;
    String mName;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_DISPLAYADAPTER_H__
