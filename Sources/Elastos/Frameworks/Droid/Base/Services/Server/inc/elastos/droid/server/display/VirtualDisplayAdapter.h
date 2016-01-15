#ifndef __ELASTOS_DROID_SERVER_DISPLAY_VIRTAUL_DISPLAYADAPTER_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_VIRTAUL_DISPLAYADAPTER_H__

#include <Elastos.Droid.Media.h>
#include "elastos/droid/server/display/DisplayAdapter.h"
#include "elastos/droid/server/display/DisplayDevice.h"
#include "elastos/droid/server/display/DisplayDeviceInfo.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::Display::IIVirtualDisplayCallback;
using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Droid::Media::Projection::IIMediaProjectionCallback;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::View::ISurface;

using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * A display adapter that provides virtual displays on behalf of applications.
 * <p>
 * Display adapters are guarded by the {@link DisplayManagerService.SyncRoot} lock.
 * </p>
 */
class VirtualDisplayAdapter
    : public DisplayAdapter
{
public:
    class MediaProjectionCallback
        : public Object
        , public IIMediaProjectionCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        MediaProjectionCallback();

        virtual ~MediaProjectionCallback();

        CARAPI constructor(
            /* [in] */ IBinder* appToken,
            /* [out] */ IObject* displayAdapter);

        //@Override
        CARAPI OnStop();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<IBinder> mAppToken;
        AutoPtr<VirtualDisplayAdapter> mDisplayAdapter;
    };

private:

    class Callback
        : public Handler
    {
    public:
        Callback(
            /* [in] */ IIVirtualDisplayCallback* callback,
            /* [in] */ IHandler* handler,
            /* [in] */ VirtualDisplayAdapter* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI DispatchDisplayPaused();

        CARAPI DispatchDisplayResumed();

        CARAPI DispatchDisplayStopped();

    private:
        static const Int32 MSG_ON_DISPLAY_PAUSED;
        static const Int32 MSG_ON_DISPLAY_RESUMED;
        static const Int32 MSG_ON_DISPLAY_STOPPED;

        AutoPtr<IIVirtualDisplayCallback> mCallback;
        VirtualDisplayAdapter* mHost;
    };

    class VirtualDisplayDevice
        : public DisplayDevice
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        VirtualDisplayDevice(
            /* [in] */ IBinder* displayToken,
            /* [in] */ IBinder* appToken,
            /* [in] */ Int32 ownerUid,
            /* [in] */ const String& ownerPackageName,
            /* [in] */ const String& name,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 densityDpi,
            /* [in] */ ISurface* surface,
            /* [in] */ Int32 flags,
            /* [in] */ Callback* callback,
            /* [in] */ VirtualDisplayAdapter* host);

        //@Override
        CARAPI ProxyDied();

        CARAPI DestroyLocked();

        //@Override
        AutoPtr<IRunnable> RequestDisplayStateLocked(
            /* [in] */ Int32 state);

        //@Override
        CARAPI_(void) PerformTraversalInTransactionLocked();

        CARAPI SetSurfaceLocked(
            /* [in] */ ISurface* surface);

        CARAPI ResizeLocked(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 densityDpi);

        CARAPI StopLocked();

        //@Override
        CARAPI_(void) DumpLocked(
            /* [in ]*/ IPrintWriter* pw);

        //@Override
        CARAPI_(AutoPtr<DisplayDeviceInfo>) GetDisplayDeviceInfoLocked();

    private:
        friend class VirtualDisplayAdapter;
        static const Int32 PENDING_SURFACE_CHANGE;
        static const Int32 PENDING_RESIZE;

        AutoPtr<IBinder> mAppToken;
        Int32 mOwnerUid;
        String mOwnerPackageName;
        String mName;
        Int32 mFlags;
        AutoPtr<Callback> mCallback;

        Int32 mWidth;
        Int32 mHeight;
        Int32 mDensityDpi;
        AutoPtr<ISurface> mSurface;
        AutoPtr<DisplayDeviceInfo> mInfo;
        Int32 mDisplayState;
        Boolean mStopped;
        Int32 mPendingChanges;
        VirtualDisplayAdapter* mHost;
    };

public:

    // Called with SyncRoot lock held.
    VirtualDisplayAdapter(
        /* [in] */ Object* syncRoot,
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ IDisplayAdapterListener* listener);

    AutoPtr<DisplayDevice> CreateVirtualDisplayLocked(
        /* [in] */ IIVirtualDisplayCallback* callback,
        /* [in] */ IIMediaProjection* projection,
        /* [in] */ Int32 ownerUid,
        /* [in] */ const String& ownerPackageName,
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags);

    CARAPI ResizeVirtualDisplayLocked(
        /* [in] */ IBinder* appToken,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi);

    CARAPI SetVirtualDisplaySurfaceLocked(
        /* [in] */ IBinder* appToken,
        /* [in] */ ISurface* surface);

    AutoPtr<DisplayDevice> ReleaseVirtualDisplayLocked(
        /* [in] */ IBinder* appToken);

private:
    void HandleProxyDiedLocked(
        /* [in] */ IBinder* appToken);

    void HandleMediaProjectionStoppedLocked(
        /* [in] */ IBinder* appToken);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayMap> mVirtualDisplayDevices; //new ArrayMap<IBinder, VirtualDisplayDevice>();
    AutoPtr<IHandler> mHandler;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_VIRTAUL_DISPLAYADAPTER_H__