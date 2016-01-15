
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Hardware.h>
#include "elastos/droid/server/display/VirtualDisplayAdapter.h"
#include "elastos/droid/server/display/CMediaProjectionCallback.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Media::Projection::EIID_IIMediaProjectionCallback;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceControl;
using Elastos::Droid::View::ISurfaceControlHelper;
using Elastos::Droid::View::CSurfaceControlHelper;

using Elastos::IO::IPrintWriter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

//===================================================================
// VirtualDisplayAdapter::MediaProjectionCallback
//===================================================================

CAR_INTERFACE_IMPL_2(VirtualDisplayAdapter::MediaProjectionCallback, Object, IIMediaProjectionCallback, IBinder)

VirtualDisplayAdapter::MediaProjectionCallback::MediaProjectionCallback()
{}

VirtualDisplayAdapter::MediaProjectionCallback::~MediaProjectionCallback()
{}

ECode VirtualDisplayAdapter::MediaProjectionCallback::constructor(
    /* [in] */ IBinder* appToken,
    /* [out] */ IObject* displayAdapter)
{
    mAppToken = appToken;
    mDisplayAdapter = (VirtualDisplayAdapter*)displayAdapter;
    return NOERROR;
}

ECode VirtualDisplayAdapter::MediaProjectionCallback::OnStop()
{
    Object* obj = mDisplayAdapter->GetSyncRoot();
    synchronized(obj) {
        mDisplayAdapter->HandleMediaProjectionStoppedLocked(mAppToken);
    }
    return NOERROR;
}

ECode VirtualDisplayAdapter::MediaProjectionCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "VirtualDisplayAdapter::MediaProjectionCallback";
    return NOERROR;
}

//===================================================================
// VirtualDisplayAdapter::Callback
//===================================================================
const Int32 VirtualDisplayAdapter::Callback::MSG_ON_DISPLAY_PAUSED = 0;
const Int32 VirtualDisplayAdapter::Callback::MSG_ON_DISPLAY_RESUMED = 1;
const Int32 VirtualDisplayAdapter::Callback::MSG_ON_DISPLAY_STOPPED = 2;

VirtualDisplayAdapter::Callback::Callback(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ IHandler* handler,
    /* [in] */ VirtualDisplayAdapter* host)
{
    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    Handler::constructor(looper);
    mCallback = callback;
    mHost = host;
}

ECode VirtualDisplayAdapter::Callback::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case MSG_ON_DISPLAY_PAUSED:
            mCallback->OnPaused();
            break;
        case MSG_ON_DISPLAY_RESUMED:
            mCallback->OnResumed();
            break;
        case MSG_ON_DISPLAY_STOPPED:
            mCallback->OnStopped();
            break;
    }

    return NOERROR;
}

ECode VirtualDisplayAdapter::Callback::DispatchDisplayPaused()
{
    Boolean bval;
    return SendEmptyMessage(MSG_ON_DISPLAY_PAUSED, &bval);
}

ECode VirtualDisplayAdapter::Callback::DispatchDisplayResumed()
{
    Boolean bval;
    return SendEmptyMessage(MSG_ON_DISPLAY_RESUMED, &bval);
}

ECode VirtualDisplayAdapter::Callback::DispatchDisplayStopped()
{
    Boolean bval;
    return SendEmptyMessage(MSG_ON_DISPLAY_STOPPED, &bval);
}

//===================================================================
// VirtualDisplayAdapter::VirtualDisplayDevice
//===================================================================
const Int32 VirtualDisplayAdapter::VirtualDisplayDevice::PENDING_SURFACE_CHANGE = 0x01;
const Int32 VirtualDisplayAdapter::VirtualDisplayDevice::PENDING_RESIZE = 0x02;

CAR_INTERFACE_IMPL(VirtualDisplayAdapter::VirtualDisplayDevice, DisplayDevice, IProxyDeathRecipient)

VirtualDisplayAdapter::VirtualDisplayDevice::VirtualDisplayDevice(
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
    /* [in] */ VirtualDisplayAdapter* host)
    : DisplayDevice((DisplayAdapter*)host, displayToken)
{
    mAppToken = appToken;
    mOwnerUid = ownerUid;
    mOwnerPackageName = ownerPackageName;
    mName = name;
    mWidth = width;
    mHeight = height;
    mDensityDpi = densityDpi;
    mSurface = surface;
    mFlags = flags;
    mCallback = callback;
    mDisplayState = IDisplay::STATE_UNKNOWN;
    mPendingChanges |= PENDING_SURFACE_CHANGE;
    mStopped = FALSE;
}

ECode VirtualDisplayAdapter::VirtualDisplayDevice::ProxyDied()
{
    Object* obj = mHost->GetSyncRoot();
    synchronized(obj) {
        if (mSurface.Get() != NULL) {
            mHost->HandleProxyDiedLocked(mAppToken);
        }
    }
    return NOERROR;
}

ECode VirtualDisplayAdapter::VirtualDisplayDevice::DestroyLocked()
{
    if (mSurface != NULL) {
        mSurface->ReleaseSurface();
        mSurface = NULL;
    }

    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
    helper->DestroyDisplay(GetDisplayTokenLocked());
    mCallback->DispatchDisplayStopped();
    return NOERROR;
}

AutoPtr<IRunnable> VirtualDisplayAdapter::VirtualDisplayDevice::RequestDisplayStateLocked(
    /* [in] */ Int32 state)
{
    if (state != mDisplayState) {
        mDisplayState = state;
        if (state == IDisplay::STATE_OFF) {
            mCallback->DispatchDisplayPaused();
        } else {
            mCallback->DispatchDisplayResumed();
        }
    }
    return NULL;
}

void VirtualDisplayAdapter::VirtualDisplayDevice::PerformTraversalInTransactionLocked()
{
    if ((mPendingChanges & PENDING_RESIZE) != 0) {
        AutoPtr<ISurfaceControlHelper> helper;
        CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);
        helper->SetDisplaySize(GetDisplayTokenLocked(), mWidth, mHeight);
    }
    if ((mPendingChanges & PENDING_SURFACE_CHANGE) != 0) {
        SetSurfaceInTransactionLocked(mSurface);
    }
    mPendingChanges = 0;
}

ECode VirtualDisplayAdapter::VirtualDisplayDevice::SetSurfaceLocked(
    /* [in] */ ISurface* surface)
{
    if (!mStopped && mSurface.Get() != surface) {
        if ((mSurface != NULL) != (surface != NULL)) {
            mHost->SendDisplayDeviceEventLocked(this, DISPLAY_DEVICE_EVENT_CHANGED);
        }
        mHost->SendTraversalRequestLocked();
        mSurface = surface;
        mInfo = NULL;
        mPendingChanges |= PENDING_SURFACE_CHANGE;
    }
    return NOERROR;
}

ECode VirtualDisplayAdapter::VirtualDisplayDevice::ResizeLocked(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    if (mWidth != width || mHeight != height || mDensityDpi != densityDpi) {
        mHost->SendDisplayDeviceEventLocked(this, DISPLAY_DEVICE_EVENT_CHANGED);
        mHost->SendTraversalRequestLocked();
        mWidth = width;
        mHeight = height;
        mDensityDpi = densityDpi;
        mInfo = NULL;
        mPendingChanges |= PENDING_RESIZE;
    }
    return NOERROR;
}

ECode VirtualDisplayAdapter::VirtualDisplayDevice::StopLocked()
{
    SetSurfaceLocked(NULL);
    mStopped = true;
    return NOERROR;
}

void VirtualDisplayAdapter::VirtualDisplayDevice::DumpLocked(
    /* [in ]*/ IPrintWriter* pw)
{
    // super.dumpLocked(pw);
    // pw.println("mFlags=" + mFlags);
    // pw.println("mDisplayState=" + IDisplay::stateToString(mDisplayState));
    // pw.println("mStopped=" + mStopped);
}

AutoPtr<DisplayDeviceInfo> VirtualDisplayAdapter::VirtualDisplayDevice::GetDisplayDeviceInfoLocked()
{
    if (mInfo == NULL) {
        mInfo = new DisplayDeviceInfo();
        mInfo->mName = mName;
        mInfo->mWidth = mWidth;
        mInfo->mHeight = mHeight;
        mInfo->mRefreshRate = 60;
        mInfo->mDensityDpi = mDensityDpi;
        mInfo->mXDpi = mDensityDpi;
        mInfo->mYDpi = mDensityDpi;
        mInfo->mPresentationDeadlineNanos = 1000000000L / (Int32) mInfo->mRefreshRate; // 1 frame
        mInfo->mFlags = 0;
        if ((mFlags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_PUBLIC) == 0) {
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_PRIVATE
                    | DisplayDeviceInfo::FLAG_NEVER_BLANK;
        }
        if ((mFlags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR) != 0) {
            mInfo->mFlags &= ~DisplayDeviceInfo::FLAG_NEVER_BLANK;
        } else {
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_OWN_CONTENT_ONLY;
        }

        if ((mFlags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_SECURE) != 0) {
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_SECURE;
        }
        if ((mFlags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_PRESENTATION) != 0) {
            mInfo->mFlags |= DisplayDeviceInfo::FLAG_PRESENTATION;
        }
        mInfo->mType = IDisplay::TYPE_VIRTUAL;
        mInfo->mTouch = DisplayDeviceInfo::TOUCH_NONE;
        mInfo->mState = mSurface != NULL ? IDisplay::STATE_ON : IDisplay::STATE_OFF;
        mInfo->mOwnerUid = mOwnerUid;
        mInfo->mOwnerPackageName = mOwnerPackageName;
    }
    return mInfo;
}


//===================================================================
// VirtualDisplayAdapter
//===================================================================
const String VirtualDisplayAdapter::TAG("VirtualDisplayAdapter");
const Boolean VirtualDisplayAdapter::DEBUG = FALSE;

VirtualDisplayAdapter::VirtualDisplayAdapter(
    /* [in] */ Object* syncRoot,
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IDisplayAdapterListener* listener)
    : DisplayAdapter(syncRoot, context, handler, listener, TAG)
{
    mHandler = handler;
    CArrayMap::New((IArrayMap**)&mVirtualDisplayDevices);
}

AutoPtr<DisplayDevice> VirtualDisplayAdapter::CreateVirtualDisplayLocked(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ IIMediaProjection* projection,
    /* [in] */ Int32 ownerUid,
     /* [in] */ const String& ownerPackageName,
     /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 flags)
{
    AutoPtr<ISurfaceControlHelper> helper;
    CSurfaceControlHelper::AcquireSingleton((ISurfaceControlHelper**)&helper);

    Boolean secure = (flags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_SECURE) != 0;
    IBinder* appToken = IBinder::Probe(callback);

    AutoPtr<IBinder> displayToken;
    helper->CreateDisplay(name, secure, (IBinder**)&displayToken);
    AutoPtr<Callback> cb = new Callback(callback, mHandler, this);
    AutoPtr<VirtualDisplayDevice> device = new VirtualDisplayDevice(
        displayToken, appToken, ownerUid, ownerPackageName, name,
        width, height, densityDpi, surface, flags, cb, this);

    mVirtualDisplayDevices->Put(TO_IINTERFACE(appToken), TO_IINTERFACE(device));

    // try {
    AutoPtr<IProxy> proxy = (IProxy*)appToken->Probe(EIID_IProxy);
    ECode ec = NOERROR;
    if (projection != NULL) {
        AutoPtr<IIMediaProjectionCallback> mpcb;
        assert(0 && "TODO");
        // CMediaProjectionCallback::New(appToken, IObject::Probe(this),
        //     (IIMediaProjectionCallback**)&mpcb);
        ec = projection->RegisterCallback(mpcb);
        FAIL_GOTO(ec, _EXIT_)
    }

    if (proxy != NULL) {
        ec = proxy->LinkToDeath(device, 0);
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        mVirtualDisplayDevices->Remove(TO_IINTERFACE(appToken));
        device->DestroyLocked();
        return NULL;
    }

    // Return the display device without actually sending the event indicating
    // that it was added.  The caller will handle it.
    return device;
}

ECode VirtualDisplayAdapter::ResizeVirtualDisplayLocked(
    /* [in] */ IBinder* appToken,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    AutoPtr<IInterface> obj;
    mVirtualDisplayDevices->Get(TO_IINTERFACE(appToken), (IInterface**)&obj);

    if (obj != NULL) {
        VirtualDisplayDevice* device = (VirtualDisplayDevice*)IObject::Probe(obj);
        device->ResizeLocked(width, height, densityDpi);
    }
    return NOERROR;
}

ECode VirtualDisplayAdapter::SetVirtualDisplaySurfaceLocked(
    /* [in] */ IBinder* appToken,
    /* [in] */ ISurface* surface)
{
    AutoPtr<IInterface> obj;
    mVirtualDisplayDevices->Get(TO_IINTERFACE(appToken), (IInterface**)&obj);

    if (obj != NULL) {
        VirtualDisplayDevice* device = (VirtualDisplayDevice*)IObject::Probe(obj);
        device->SetSurfaceLocked(surface);
    }
    return NOERROR;
}

AutoPtr<DisplayDevice> VirtualDisplayAdapter::ReleaseVirtualDisplayLocked(
    /* [in] */ IBinder* appToken)
{
    AutoPtr<IInterface> obj;
    mVirtualDisplayDevices->Get(TO_IINTERFACE(appToken), (IInterface**)&obj);
    VirtualDisplayDevice* device = (VirtualDisplayDevice*)IObject::Probe(obj);
    if (device != NULL) {
        device->DestroyLocked();

        AutoPtr<IProxy> proxy = (IProxy*)appToken->Probe(EIID_IProxy);
        if (proxy != NULL) {
            Boolean bval;
            proxy->UnlinkToDeath(device, 0, &bval);
        }
    }

    // Return the display device that was removed without actually sending the
    // event indicating that it was removed.  The caller will handle it.
    return device;
}

void VirtualDisplayAdapter::HandleProxyDiedLocked(
    /* [in] */ IBinder* appToken)
{
    AutoPtr<IInterface> obj;
    mVirtualDisplayDevices->Remove(TO_IINTERFACE(appToken), (IInterface**)&obj);

    if (obj != NULL) {
        VirtualDisplayDevice* device = (VirtualDisplayDevice*)IObject::Probe(obj);
        Slogger::I(TAG, "Virtual display device released because application token died: %s",
            device->mOwnerPackageName.string());
        device->DestroyLocked();
        SendDisplayDeviceEventLocked(device, DISPLAY_DEVICE_EVENT_REMOVED);
    }
}

void VirtualDisplayAdapter::HandleMediaProjectionStoppedLocked(
    /* [in] */ IBinder* appToken)
{
    AutoPtr<IInterface> obj;
    mVirtualDisplayDevices->Remove(TO_IINTERFACE(appToken), (IInterface**)&obj);

    if (obj != NULL) {
        VirtualDisplayDevice* device = (VirtualDisplayDevice*)IObject::Probe(obj);
        Slogger::I(TAG, "Virtual display device released because media projection stopped: %s",
            device->mName.string());
        device->StopLocked();
    }
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
