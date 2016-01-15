
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"
#include "elastos/droid/hardware/display/WifiDisplayStatus.h"
#include "elastos/droid/hardware/display/CDisplayManagerCallback.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/text/TextUtils.h"
//#include "elastos/droid/view/Display.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Media::Projection::IIMediaProjection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

const Int32 DisplayManagerGlobal::VirtualDisplayCallbackDelegate::MSG_DISPLAY_PAUSED = 0;
const Int32 DisplayManagerGlobal::VirtualDisplayCallbackDelegate::MSG_DISPLAY_RESUMED = 1;
const Int32 DisplayManagerGlobal::VirtualDisplayCallbackDelegate::MSG_DISPLAY_STOPPED = 2;

const String DisplayManagerGlobal::TAG("DisplayManagerGlobal");
const Boolean DisplayManagerGlobal::DEBUG;
const Boolean DisplayManagerGlobal::USE_CACHE;

AutoPtr<IDisplayManagerGlobal> DisplayManagerGlobal::sInstance;
Object DisplayManagerGlobal::sInstanceLock;

DisplayManagerGlobal::DisplayListenerDelegate::DisplayListenerDelegate(
    /* [in] */ IDisplayListener* listener,
    /* [in] */ ILooper* looper)
    : Handler(looper)
    , mListener(listener)
{
}

void DisplayManagerGlobal::DisplayListenerDelegate::SendDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(event, displayId, 0, (IMessage**)&msg);
    Boolean result;
    SendMessage(msg, &result);
}

void DisplayManagerGlobal::DisplayListenerDelegate::ClearEvents()
{
    RemoveCallbacksAndMessages(NULL);
}

ECode DisplayManagerGlobal::DisplayListenerDelegate::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, displayId;
    msg->GetWhat(&what);
    msg->GetArg1(&displayId);

    switch (what) {
        case DisplayManagerGlobal::EVENT_DISPLAY_ADDED:
            mListener->OnDisplayAdded(displayId);
            break;
        case DisplayManagerGlobal::EVENT_DISPLAY_CHANGED:
            mListener->OnDisplayChanged(displayId);
            break;
        case DisplayManagerGlobal::EVENT_DISPLAY_REMOVED:
            mListener->OnDisplayRemoved(displayId);
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DisplayManagerGlobal::VirtualDisplayCallback, Object, IIVirtualDisplayCallback);

DisplayManagerGlobal::VirtualDisplayCallback::VirtualDisplayCallback(
    /* [in] */ IVirtualDisplayCallback* callback,
    /* [in] */ IHandler* handler)
{
    if (callback != NULL) {
        mDelegate = new VirtualDisplayCallbackDelegate(callback, handler);
    }
}

ECode DisplayManagerGlobal::VirtualDisplayCallback::OnPaused()
{
    Boolean result;
    if (mDelegate != NULL) {
        FAIL_RETURN(mDelegate->SendEmptyMessage(VirtualDisplayCallbackDelegate::MSG_DISPLAY_PAUSED, &result))
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::VirtualDisplayCallback::OnResumed()
{
    Boolean result;
    if (mDelegate != NULL) {
        FAIL_RETURN(mDelegate->SendEmptyMessage(VirtualDisplayCallbackDelegate::MSG_DISPLAY_RESUMED, &result))
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::VirtualDisplayCallback::OnStopped()
{
    Boolean result;
    if (mDelegate != NULL) {
        FAIL_RETURN(mDelegate->SendEmptyMessage(VirtualDisplayCallbackDelegate::MSG_DISPLAY_STOPPED, &result))
    }
    return NOERROR;
}

DisplayManagerGlobal::VirtualDisplayCallbackDelegate::VirtualDisplayCallbackDelegate(
    /* [in] */ IVirtualDisplayCallback* callback,
    /* [in] */ IHandler* handler)
    : mCallback(callback)
{
    AutoPtr<ILooper> looper;
    if (handler != NULL) {
        handler->GetLooper((ILooper**)&looper);
    }
    else {
        looper = Looper::GetMyLooper();
    }

    Handler::constructor(looper, NULL, TRUE /*async*/);
}

ECode DisplayManagerGlobal::VirtualDisplayCallbackDelegate::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case MSG_DISPLAY_PAUSED:
            mCallback->OnPaused();
            break;
        case MSG_DISPLAY_RESUMED:
            mCallback->OnResumed();
            break;
        case MSG_DISPLAY_STOPPED:
            mCallback->OnStopped();
            break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DisplayManagerGlobal, Object, IDisplayManagerGlobal);

DisplayManagerGlobal::DisplayManagerGlobal(
    /* [in] */ IIDisplayManager* dm)
    : mDm(dm)
{}

DisplayManagerGlobal::~DisplayManagerGlobal()
{
    mDisplayListeners.Clear();
}

AutoPtr<IDisplayManagerGlobal> DisplayManagerGlobal::GetInstance()
{
    synchronized(sInstanceLock) {
        if (sInstance == NULL) {
            AutoPtr<IInterface> service = ServiceManager::GetService(IContext::DISPLAY_SERVICE);
            if (service != NULL) {
                AutoPtr<IIDisplayManager> idm = IIDisplayManager::Probe(service);
                sInstance = (IDisplayManagerGlobal*)new DisplayManagerGlobal(idm);
            }
            else {
                Logger::E(TAG, "GetInstance() failed to get service %s", IContext::DISPLAY_SERVICE.string());
            }
        }
    }

    return sInstance;
}

ECode DisplayManagerGlobal::GetDisplayInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayInfo** displayInfo)
{
    VALIDATE_NOT_NULL(displayInfo);

    //try {
    synchronized(mLock) {
        AutoPtr<IDisplayInfo> info;
        if (USE_CACHE) {
            HashMap<Int32, AutoPtr<IDisplayInfo> >::Iterator find
                = mDisplayInfoCache.Find(displayId);
            if (find != mDisplayInfoCache.End()) {
                *displayInfo = find->mSecond;
                REFCOUNT_ADD(*displayInfo);
                return NOERROR;
            }
        }

        mDm->GetDisplayInfo(displayId, displayInfo);
        if (*displayInfo == NULL) {
            return NOERROR;
        }

        if (USE_CACHE) {
            mDisplayInfoCache[displayId] = *displayInfo;
        }

        RegisterCallbackIfNeededLocked();

        if (DEBUG) {
            Logger::D(TAG, "getDisplayInfo: displayId=%d, info", displayId/*, info*/);
        }
    }

    return NOERROR;

    // } catch (RemoteException ex) {
    //     Log.e(TAG, "Could not get display information from display manager.", ex);
    //     return NULL;
    // }
}

ECode DisplayManagerGlobal::GetDisplayIds(
    /* [out, calee] */ ArrayOf<Int32>** displayIds)
{
    VALIDATE_NOT_NULL(displayIds);

    //try {
    synchronized(mLock) {
        if (USE_CACHE) {
            if (mDisplayIdCache != NULL) {
                *displayIds = mDisplayIdCache;
                REFCOUNT_ADD(*displayIds);
                return NOERROR;
            }
        }

        if (FAILED(mDm->GetDisplayIds(displayIds))) {
            *displayIds = ArrayOf<Int32>::Alloc(1);
            REFCOUNT_ADD(*displayIds);
            (**displayIds)[0] = IDisplay::DEFAULT_DISPLAY;
            return NOERROR;
        }

        if (USE_CACHE) {
            mDisplayIdCache = *displayIds;
        }
        RegisterCallbackIfNeededLocked();
    }

    return NOERROR;

    // } catch (RemoteException ex) {
    //     Log.e(TAG, "Could not get display ids from display manager.", ex);
    //     return new Int32[] { Display.DEFAULT_DISPLAY };
    // }
}

ECode DisplayManagerGlobal::GetCompatibleDisplay(
    /* [in] */ Int32 displayId,
    /* [in] */ IDisplayAdjustments* cih,
    /* [out] */ IDisplay** display)
{
    VALIDATE_NOT_NULL(display);

    AutoPtr<IDisplayInfo> displayInfo;
    GetDisplayInfo(displayId, (IDisplayInfo**)&displayInfo);
    if (displayInfo == NULL) {
        *display = NULL;
        return NOERROR;
    }

    assert(0 && "TODO View::Display");
    //*display = new View::Display(this, displayId, displayInfo, cih);
    if (*display) {
        REFCOUNT_ADD(*display);
        return NOERROR;
    }
    else {
        return E_OUT_OF_MEMORY_ERROR;
    }
}

ECode DisplayManagerGlobal::GetRealDisplay(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplay** display)
{
    VALIDATE_NOT_NULL(display);

    AutoPtr<IDisplayAdjustments> da;
    assert(0 && "TODO GetDEFAULT_DISPLAY_ADJUSTMENTS");
    //GetDEFAULT_DISPLAY_ADJUSTMENTS((IDisplayAdjustments**)&da);
    return GetCompatibleDisplay(displayId, da, display);
}

ECode DisplayManagerGlobal::GetRealDisplay(
    /* [in] */ Int32 displayId,
    /* [in] */ IBinder* token,
    /* [out] */ IDisplay** display)
{
    VALIDATE_NOT_NULL(display);

    assert(0 && "TODO new DisplayAdjustments");
    AutoPtr<IDisplayAdjustments> adjustment;// = new DisplayAdjustments(token);
    return GetCompatibleDisplay(displayId, adjustment, display);
}

ECode DisplayManagerGlobal::RegisterDisplayListener(
    /* [in] */ IDisplayListener* listener,
    /* [in] */ IHandler* handler)
{
    if (listener == NULL) {
        Logger::E(TAG, "listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ILooper> looper;
    if (NULL == handler) {
        looper = Looper::GetMyLooper();
    }
    else {
        handler->GetLooper((ILooper**)&looper);
    }

    synchronized(mLock) {
        List<AutoPtr<DisplayListenerDelegate> >::Iterator iter = FindDisplayListenerLocked(listener);
        if (iter == mDisplayListeners.End()) {
            AutoPtr<DisplayListenerDelegate> delegate = new DisplayListenerDelegate(listener, looper);
            mDisplayListeners.PushBack(delegate);
            RegisterCallbackIfNeededLocked();
        }
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::UnregisterDisplayListener(
    /* [in] */ IDisplayListener* listener)
{
    if (listener == NULL) {
        Logger::E(TAG, "listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    synchronized(mLock) {
        List<AutoPtr<DisplayListenerDelegate> >::Iterator iter = FindDisplayListenerLocked(listener);
        if (iter != mDisplayListeners.End()) {
            AutoPtr<DisplayListenerDelegate> d = *iter;
            d->ClearEvents();
            mDisplayListeners.Erase(iter);
        }
    }

    return NOERROR;
}

List<AutoPtr<DisplayManagerGlobal::DisplayListenerDelegate> >::Iterator
DisplayManagerGlobal::FindDisplayListenerLocked(
    /* [in] */ IDisplayListener* listener)
{
    List<AutoPtr<DisplayManagerGlobal::DisplayListenerDelegate> >::Iterator iter = mDisplayListeners.Begin();
    for (; iter != mDisplayListeners.End(); ++iter) {
        if ((*iter)->mListener.Get() == listener)
            return iter;
    }

    return mDisplayListeners.End();
}

void DisplayManagerGlobal::RegisterCallbackIfNeededLocked()
{
    if (mCallback == NULL) {
        CDisplayManagerCallback::New((Handle32)this, (IIDisplayManagerCallback**)&mCallback);
        if (FAILED(mDm->RegisterCallback(mCallback))) {
            Logger::E(TAG, "Failed to register callback with display manager service.");
            mCallback = NULL;
        }
    }
}

void DisplayManagerGlobal::HandleDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    synchronized(mLock) {
        if (USE_CACHE) {
            mDisplayInfoCache.Erase(displayId);

            if (event == EVENT_DISPLAY_ADDED || event == EVENT_DISPLAY_REMOVED) {
                mDisplayIdCache = NULL;
            }
        }

        List<AutoPtr<DisplayListenerDelegate> >::Iterator iter = mDisplayListeners.Begin();
        for (; iter != mDisplayListeners.End(); ++iter) {
            (*iter)->SendDisplayEvent(displayId, event);
        }
    }
}

ECode DisplayManagerGlobal::StartWifiDisplayScan()
{
    synchronized(mLock) {
        if (mWifiDisplayScanNestCount++ == 0) {
            RegisterCallbackIfNeededLocked();
            if (FAILED(mDm->StartWifiDisplayScan())) {
                Logger::E(TAG, "Failed to scan for Wifi displays.");
                return E_REMOTE_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::StopWifiDisplayScan()
{
    synchronized(mLock) {
        if (--mWifiDisplayScanNestCount == 0) {
            if (FAILED(mDm->StopWifiDisplayScan())) {
                Logger::E(TAG, "Failed to scan for Wifi displays.");
                return E_REMOTE_EXCEPTION;
            }
        } else if (mWifiDisplayScanNestCount < 0) {
            //Log.wtf(TAG, "Wifi display scan nest count became negative: "
            //        + mWifiDisplayScanNestCount);
            mWifiDisplayScanNestCount = 0;
        }
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::ConnectWifiDisplay(
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress.IsNull()) {
        Logger::E(TAG, "deviceAddress must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (FAILED(mDm->ConnectWifiDisplay(deviceAddress))) {
        Logger::E(TAG, "Failed to connect to Wifi display.%s", deviceAddress.string());
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::PauseWifiDisplay()
{
    if (FAILED(mDm->PauseWifiDisplay())) {
        Logger::E(TAG, "Failed to pause Wifi display.");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::ResumeWifiDisplay()
{
    if (FAILED(mDm->ResumeWifiDisplay())) {
        Logger::E(TAG, "Failed to resume Wifi display.");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::DisconnectWifiDisplay()
{
    if (FAILED(mDm->DisconnectWifiDisplay())) {
        Logger::E(TAG, "Failed to disconnect from Wifi display.");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::RenameWifiDisplay(
    /* [in] */ const String& deviceAddress,
    /* [in] */ const String& alias)
{
    if (deviceAddress.IsNull()) {
        Logger::E(TAG, "deviceAddress must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (FAILED(mDm->RenameWifiDisplay(deviceAddress, alias))) {
        Logger::E(TAG, "Failed to rename Wifi display %s with alias %s.",
            deviceAddress.string(), alias.string());
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::ForgetWifiDisplay(
    /* [in] */ const String& deviceAddress)
{
    if (deviceAddress.IsNull()) {
        Logger::E(TAG, "deviceAddress must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (FAILED(mDm->ForgetWifiDisplay(deviceAddress))) {
        Logger::E(TAG, "Failed to forget Wifi display.");
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::GetWifiDisplayStatus(
    /* [out] */ IWifiDisplayStatus** status)
{
    VALIDATE_NOT_NULL(status);
    *status = NULL;

    if (FAILED(mDm->GetWifiDisplayStatus(status))) {
        Logger::E(TAG, "Failed to get Wifi display status.");
        AutoPtr<IWifiDisplayStatus> tmpStatus = new WifiDisplayStatus();
        *status = tmpStatus;
        REFCOUNT_ADD(*status);
        return E_REMOTE_EXCEPTION;
    }

    return NOERROR;
}

ECode DisplayManagerGlobal::CreateVirtualDisplay(
    /* [in] */ IContext* context,
    /* [in] */ IMediaProjection* projection,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 flags,
    /* [in] */ IVirtualDisplayCallback* cb,
    /* [in] */ IHandler* handler,
    /* [out] */ IVirtualDisplay** outvd)
{
    assert(0 && "TODO TextUtils::IsEmpty");
    // if (TextUtils::IsEmpty(name)) {
    //     //throw new IllegalArgumentException("name must be non-null and non-empty");
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }
    if (width <= 0 || height <= 0 || densityDpi <= 0) {
        //throw new IllegalArgumentException("width, height, and densityDpi must be "
        //        + "greater than 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<VirtualDisplayCallback> callbackWrapper = new VirtualDisplayCallback(cb, handler);
    AutoPtr<IMediaProjection> projec;
    assert(0 && "TODO projection->GetProjection");
    AutoPtr<IIMediaProjection> projectionToken;// = projection != NULL ?
            //(projection->GetProjection((IMediaProjection**)&projec), projec) : NULL;
    Int32 displayId;
    //try {
    String pName;
    context->GetPackageName(&pName);
    if(FAILED(mDm->CreateVirtualDisplay(callbackWrapper, projectionToken,
            pName, name, width, height, densityDpi, surface, flags, &displayId))) {
        *outvd = NULL;
        return E_REMOTE_EXCEPTION;
    }
    //} catch (RemoteException ex) {
    //    Log.e(TAG, "Could not create virtual display: " + name, ex);
    //    return null;
    //}
    if (displayId < 0) {
        //Log.e(TAG, "Could not create virtual display: " + name);
        *outvd = NULL;
        return NOERROR;
    }

    AutoPtr<IDisplay> display;
    FAIL_RETURN(GetRealDisplay(displayId, (IDisplay**)&display))
    if (display == NULL) {
        //Log.wtf(TAG, "Could not obtain display info for newly created "
        //         + "virtual display: " + name);
        if (FAILED(mDm->ReleaseVirtualDisplay(callbackWrapper))) {
            return E_REMOTE_EXCEPTION;
        }
        *outvd = NULL;
        return NOERROR;
    }
    assert(0 && "TODO View::Display");
    // return CVirtualDisplay::New(this, display, callbackWrapper, surface, outvd);
    return NOERROR;
}

ECode DisplayManagerGlobal::SetVirtualDisplaySurface(
    /* [in] */ IIVirtualDisplayCallback* token,
    /* [in] */ ISurface* surface)
{
    if (FAILED(mDm->SetVirtualDisplaySurface(token, surface))) {
        Logger::E(TAG, "Failed to set virtual display surface.");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::ResizeVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* token,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    if (FAILED(mDm->ResizeVirtualDisplay(token, width, height, densityDpi))) {
        Logger::E(TAG, "Failed to resize virtual display.");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode DisplayManagerGlobal::ReleaseVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* token)
{
    if (FAILED(mDm->ReleaseVirtualDisplay(token))) {
        Logger::E(TAG, "Failed to release virtual display.");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
