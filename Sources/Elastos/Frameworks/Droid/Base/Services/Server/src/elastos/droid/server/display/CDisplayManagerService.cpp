#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/display/CDisplayManagerService.h"
#include "elastos/droid/server/display/CDisplayManagerBinderService.h"
#include "elastos/droid/server/display/LocalDisplayAdapter.h"
#include "elastos/droid/server/display/WifiDisplayAdapter.h"
#include "elastos/droid/server/display/OverlayDisplayAdapter.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::CDisplayInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::EIID_IDisplayManagerInternal;
using Elastos::Droid::Hardware::Display::EIID_IIDisplayManager;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::EIID_IDisplayViewport;
using Elastos::Droid::Hardware::Display::CWifiDisplayStatus;
using Elastos::Droid::Hardware::Display::CDisplayViewport;
using Elastos::Core::Thread;
using Elastos::Utility::Arrays;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String CDisplayManagerService::TAG("CDisplayManagerService");
const Boolean CDisplayManagerService::DEBUG = FALSE;
const String CDisplayManagerService::FORCE_WIFI_DISPLAY_ENABLE("persist.debug.wfd.enable");
const String CDisplayManagerService::SYSTEM_HEADLESS("persist.debug.wfd.enable");
const Int64 CDisplayManagerService::WAIT_FOR_DEFAULT_DISPLAY_TIMEOUT;
const Int32 CDisplayManagerService::MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER;
const Int32 CDisplayManagerService::MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS;
const Int32 CDisplayManagerService::MSG_DELIVER_DISPLAY_EVENT;
const Int32 CDisplayManagerService::MSG_REQUEST_TRAVERSAL;
const Int32 CDisplayManagerService::MSG_UPDATE_VIEWPORT;

//==============================================================================
// CDisplayManagerService::BinderService
//==============================================================================

CAR_INTERFACE_IMPL_2(CDisplayManagerService::BinderService, Object, IIDisplayManager, IBinder)

ECode CDisplayManagerService::BinderService::constructor(
    /* [in] */ ISystemService* displayManagerService)
{
    mService = (CDisplayManagerService*)displayManagerService;
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CDisplayManagerService::BinderService";
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::GetDisplayInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayInfo** info)
{
    VALIDATE_NOT_NULL(info)

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingUid;
    binder->GetCallingUid(&callingUid);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->GetDisplayInfoInternal(displayId, callingUid, info);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::GetDisplayIds(
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingUid;
    binder->GetCallingUid(&callingUid);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->GetDisplayIdsInternal(callingUid, ids);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::RegisterCallback(
    /* [in] */ IIDisplayManagerCallback* callback)
{
    if (callback == NULL) {
        Slogger::E("CDisplayManagerService::BinderService", "listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingPid;
    binder->GetCallingPid(&callingPid);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->RegisterCallbackInternal(callback, callingPid);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::StartWifiDisplayScan()
{
    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to start wifi display scans"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingPid;
    binder->GetCallingPid(&callingPid);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->StartWifiDisplayScanInternal(callingPid);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::StopWifiDisplayScan()
{
    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to stop wifi display scans"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingPid;
    binder->GetCallingPid(&callingPid);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->StopWifiDisplayScanInternal(callingPid);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ConnectWifiDisplay(
    /* [in] */ const String& address)
{
    if (address == NULL) {
        Slogger::E("CDisplayManagerService::BinderService", "address must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to connect to a wifi display"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->ConnectWifiDisplayInternal(address);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::DisconnectWifiDisplay()
{
    // This request does not require special permissions.
    // Any app can request disconnection from the currently active wifi display.
    // This exception should no longer be needed once wifi display control moves
    // to the media router service.

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->DisconnectWifiDisplayInternal();
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::RenameWifiDisplay(
    /* [in] */ const String& address,
    /* [in] */ const String& alias)
{
    if (address == NULL) {
        Slogger::E("CDisplayManagerService::BinderService", "address must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to rename to a wifi display"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->RenameWifiDisplayInternal(address, alias);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ForgetWifiDisplay(
    /* [in] */ const String& address)
{
    if (address == NULL) {
        Slogger::E("CDisplayManagerService::BinderService", "address must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to forget to a wifi display"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->ForgetWifiDisplayInternal(address);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::PauseWifiDisplay()
{
    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to pause a wifi display session"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->PauseWifiDisplayInternal();
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ResumeWifiDisplay()
{
    mService->mContext->EnforceCallingOrSelfPermission(
        Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        String("Permission required to resume a wifi display session"));

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->ResumeWifiDisplayInternal();
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::GetWifiDisplayStatus(
    /* [out] */ IWifiDisplayStatus** status)
{
    VALIDATE_NOT_NULL(status)

    // This request does not require special permissions.
    // Any app can get information about available wifi displays.

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->GetWifiDisplayStatusInternal(status);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::CreateVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ IIMediaProjection* projection,
    /* [in] */ const String& packageName,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 flags,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 callingUid;
    binder->GetCallingUid(&callingUid);
    if (!ValidatePackageName(callingUid, packageName)) {
        Slogger::E("CDisplayManagerService", "packageName must match the calling uid");
        return E_SECURITY_EXCEPTION;
    }
    if (callback == NULL) {
        Slogger::E("CDisplayManagerService", "appToken must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (TextUtils::IsEmpty(name)) {
        Slogger::E("CDisplayManagerService", "name must be non-NULL and non-empty");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (width <= 0 || height <= 0 || densityDpi <= 0) {
        Slogger::E("CDisplayManagerService", "width, height, and densityDpi must be greater than 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((flags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_PUBLIC) != 0) {
        flags |= IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR;
    }
    if ((flags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY) != 0) {
        flags &= ~IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR;
    }

    if (projection != NULL) {
        // try {
        AutoPtr<IIMediaProjectionManager> mpmgr = mService->GetProjectionService();
        ECode ec = NOERROR;
        Boolean bval;
        mpmgr->IsValidMediaProjection(projection, &bval);
        if (!bval) {
            Slogger::E("CDisplayManagerService", "Invalid media projection");
            return E_SECURITY_EXCEPTION;
        }
        ec = projection->ApplyVirtualDisplayFlags(flags, &flags);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E("CDisplayManagerService", "unable to validate media projection or flags");
            return E_SECURITY_EXCEPTION;
        }
    }

    if (callingUid != IProcess::SYSTEM_UID
        && (flags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR) != 0) {
        if (!CanProjectVideo(projection)) {
            Slogger::E("CDisplayManagerService", "Requires CAPTURE_VIDEO_OUTPUT or "
                "CAPTURE_SECURE_VIDEO_OUTPUT permission, or an appropriate "
                "MediaProjection token in order to create a screen sharing virtual display.");
            return E_SECURITY_EXCEPTION;
        }
    }
    if ((flags & IDisplayManager::VIRTUAL_DISPLAY_FLAG_SECURE) != 0) {
        if (!CanProjectSecureVideo(projection)) {
            Slogger::E("CDisplayManagerService", "Requires CAPTURE_SECURE_VIDEO_OUTPUT "
                "or an appropriate MediaProjection token to create a secure virtual display.");
            return E_SECURITY_EXCEPTION;
        }
    }

    Int64 token;
    binder->ClearCallingIdentity(&token);
    *result = mService->CreateVirtualDisplayInternal(callback, projection, callingUid,
        packageName, name, width, height, densityDpi, surface, flags);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ResizeVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->ResizeVirtualDisplayInternal(IBinder::Probe(callback), width, height, densityDpi);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::SetVirtualDisplaySurface(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ ISurface* surface)
{
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->SetVirtualDisplaySurfaceInternal(IBinder::Probe(callback), surface);
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::ReleaseVirtualDisplay(
    /* [in] */ IIVirtualDisplayCallback* callback)
{
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int64 token;
    binder->ClearCallingIdentity(&token);
    mService->ReleaseVirtualDisplayInternal(IBinder::Probe(callback));
    binder->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDisplayManagerService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    assert(0 && "TODO");
    // if (mContext == NULL
    //     || mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP)
    //                 != PackageManager.PERMISSION_GRANTED) {
    //     pw.println("Permission Denial: can't dump DisplayManager from from pid="
    //             + binder->GetCallingPid() + ", uid=" + binder->GetCallingUid());
    //     return;
    // }

    // AutoPtr<IBinderHelper> binder;
    // CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    // Int64 token;
    // binder->ClearCallingIdentity(&token);
    // mService->DumpInternal(pw);
    // binder->RestoreCallingIdentity(token);
    return NOERROR;
}

Boolean CDisplayManagerService::BinderService::ValidatePackageName(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (packageName != NULL) {
        AutoPtr<ArrayOf<String> > packageNames;
        AutoPtr<IPackageManager> pm;
        mService->mContext->GetPackageManager((IPackageManager**)&pm);
        pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packageNames);
        if (packageNames != NULL) {
            for (Int32 i = 0; i < packageNames->GetLength(); ++i) {
                if ((*packageNames)[i].Equals(packageName)) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

Boolean CDisplayManagerService::BinderService::CanProjectVideo(
    /* [in] */ IIMediaProjection* projection)
{
    if (projection != NULL) {
        Boolean bval;
        ECode ec = projection->CanProjectVideo(&bval);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E("CDisplayManagerService", "Unable to query projection service for permissions");
        }
        if (bval) {
            return TRUE;
        }
    }
    Int32 permission;
    mService->mContext->CheckCallingPermission(
        Manifest::permission::CAPTURE_VIDEO_OUTPUT, &permission);
    if (permission == IPackageManager::PERMISSION_GRANTED) {
        return TRUE;
    }
    return CanProjectSecureVideo(projection);
}

Boolean CDisplayManagerService::BinderService::CanProjectSecureVideo(
    /* [in] */ IIMediaProjection* projection)
{
    Boolean bval = FALSE;
    if (projection != NULL) {

        ECode ec = projection->CanProjectSecureVideo(&bval);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E("CDisplayManagerService", "Unable to query projection service for permissions");
        }
        if (bval) {
            return TRUE;
        }
    }

    Int32 permission;
    mService->mContext->CheckCallingPermission(
        Manifest::permission::CAPTURE_SECURE_VIDEO_OUTPUT, &permission);
    bval = permission == IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

//==============================================================================
// CDisplayManagerService::DisplayBlanker
//==============================================================================
CAR_INTERFACE_IMPL(CDisplayManagerService::DisplayBlanker, Object, IDisplayBlanker)

CDisplayManagerService::DisplayBlanker::DisplayBlanker(
    /* [in] */ CDisplayManagerService* host,
    /* [in] */ IDisplayPowerCallbacks* callbacks)
    : mHost(host)
    , mCallbacks(callbacks)
{
}

ECode CDisplayManagerService::DisplayBlanker::RequestDisplayState(
    /* [in] */ Int32 state)
{
    // The order of operations is important for legacy reasons.
    if (state == IDisplay::STATE_OFF) {
        mHost->RequestGlobalDisplayStateInternal(state);
    }

    mCallbacks->OnDisplayStateChange(state);

    if (state != IDisplay::STATE_OFF) {
        mHost->RequestGlobalDisplayStateInternal(state);
    }
    return NOERROR;
}

ECode CDisplayManagerService::DisplayBlanker::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CDisplayManagerService::DisplayBlanker";
    return NOERROR;
}

//==============================================================================
// CDisplayManagerService::LocalService
//==============================================================================

CAR_INTERFACE_IMPL(CDisplayManagerService::LocalService::LocalService, Object, IDisplayManagerInternal)

CDisplayManagerService::LocalService::LocalService(
    /* [in] */ CDisplayManagerService* host)
    : mHost(host)
{}

ECode CDisplayManagerService::LocalService::InitPowerManagement(
    /* [in] */ IDisplayPowerCallbacks* callbacks,
    /* [in] */ IHandler* handler,
    /* [in] */ ISensorManager* sensorManager)
{
    Object* obj = mHost->mSyncRoot;
    synchronized(obj) {
        AutoPtr<IDisplayBlanker> blanker = new DisplayBlanker(mHost, callbacks);
        mHost->mDisplayPowerController = new DisplayPowerController(
            mHost->mContext, callbacks, handler, sensorManager, blanker);
    }
    return NOERROR;
}

ECode CDisplayManagerService::LocalService::RequestPowerState(
    /* [in] */ IDisplayPowerRequest* request,
    /* [in] */ Boolean waitForNegativeProximity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mDisplayPowerController->RequestPowerState(request, waitForNegativeProximity);
    return NOERROR;
}

ECode CDisplayManagerService::LocalService::IsProximitySensorAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->mDisplayPowerController->IsProximitySensorAvailable();
    return NOERROR;
}

ECode CDisplayManagerService::LocalService::GetDisplayInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ IDisplayInfo** result)
{
    return mHost->GetDisplayInfoInternal(displayId, Process::MyUid(), result);
}

ECode CDisplayManagerService::LocalService::RegisterDisplayTransactionListener(
    /* [in] */ IDisplayTransactionListener* listener)
{
    if (listener == NULL) {
        // throw new IllegalArgumentException("listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mHost->RegisterDisplayTransactionListenerInternal(listener);
}

ECode CDisplayManagerService::LocalService::UnregisterDisplayTransactionListener(
    /* [in] */ IDisplayTransactionListener* listener)
{
    if (listener == NULL) {
        // throw new IllegalArgumentException("listener must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mHost->UnregisterDisplayTransactionListenerInternal(listener);
}

ECode CDisplayManagerService::LocalService::SetDisplayInfoOverrideFromWindowManager(
    /* [in] */ Int32 displayId,
    /* [in] */ IDisplayInfo* info)
{
    return mHost->SetDisplayInfoOverrideFromWindowManagerInternal(displayId, info);
}

ECode CDisplayManagerService::LocalService::PerformTraversalInTransactionFromWindowManager()
{
    return mHost->PerformTraversalInTransactionFromWindowManagerInternal();
}

ECode CDisplayManagerService::LocalService::SetDisplayProperties(
    /* [in] */ Int32 displayId,
    /* [in] */ Boolean hasContent,
    /* [in] */ Float requestedRefreshRate,
    /* [in] */ Boolean inTraversal)
{
    mHost->SetDisplayPropertiesInternal(displayId, hasContent, requestedRefreshRate, inTraversal);
    return NOERROR;
}

//==============================================================================
// CDisplayManagerService::DisplayManagerHandler
//==============================================================================

CDisplayManagerService::DisplayManagerHandler::DisplayManagerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CDisplayManagerService* host)
    : Handler(looper, NULL, TRUE /*async*/)
    , mHost(host)
{
}

String CDisplayManagerService::DisplayManagerHandler::CodeToString(
    /* [in] */ Int32 what)
{
    switch (what) {
        case CDisplayManagerService::MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER :
            return String("MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER");
        case CDisplayManagerService::MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS :
            return String("MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS");
        case CDisplayManagerService::MSG_DELIVER_DISPLAY_EVENT :
            return String("MSG_DELIVER_DISPLAY_EVENT");
        case CDisplayManagerService::MSG_REQUEST_TRAVERSAL :
            return String("MSG_REQUEST_TRAVERSAL");
        case CDisplayManagerService::MSG_UPDATE_VIEWPORT :
            return String("MSG_UPDATE_VIEWPORT");
    }
    return String("Unknown message");
}

ECode CDisplayManagerService::DisplayManagerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    Int32 what;
    msg->GetWhat(&what);

    if (CDisplayManagerService::DEBUG) {
        Slogger::D(CDisplayManagerService::TAG, " >>>> HandleMessage %s", CodeToString(what).string());
    }

    switch (what) {
        case CDisplayManagerService::MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER:
            mHost->RegisterDefaultDisplayAdapter();
            break;

        case CDisplayManagerService::MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS:
            mHost->RegisterAdditionalDisplayAdapters();
            break;

        case CDisplayManagerService::MSG_DELIVER_DISPLAY_EVENT: {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            mHost->DeliverDisplayEvent(arg1, arg2);
            break;
        }

        case CDisplayManagerService::MSG_REQUEST_TRAVERSAL:
            if (mHost->mWindowManagerInternal)
                mHost->mWindowManagerInternal->RequestTraversalFromDisplayManager();
            break;

        case MSG_UPDATE_VIEWPORT:
        {
            {
                Object* obj = mHost->mSyncRoot;
                AutoLock lock(obj);
                mHost->mTempDefaultViewport->CopyFrom(mHost->mDefaultViewport);
                mHost->mTempExternalTouchViewport->CopyFrom(mHost->mExternalTouchViewport);
            }
            if (mHost->mInputManagerInternal) {
                mHost->mInputManagerInternal->SetDisplayViewports(
                    (IDisplayViewport*)mHost->mTempDefaultViewport->Probe(EIID_IDisplayViewport),
                    (IDisplayViewport*)mHost->mTempExternalTouchViewport->Probe(EIID_IDisplayViewport));
            }
            break;
        }
    }

    if (CDisplayManagerService::DEBUG) {
        Slogger::D(CDisplayManagerService::TAG, " <<<< HandleMessage %s done", CodeToString(what).string());
    }
    return NOERROR;
}

//==============================================================================
// CDisplayManagerService::DisplayAdapterListener
//==============================================================================
CAR_INTERFACE_IMPL(CDisplayManagerService::DisplayAdapterListener, Object, IDisplayAdapterListener)

CDisplayManagerService::DisplayAdapterListener::DisplayAdapterListener(
    /* [in] */ CDisplayManagerService* owner)
    : mHost(owner)
{
}

ECode CDisplayManagerService::DisplayAdapterListener::OnDisplayDeviceEvent(
    /* [in] */ Handle32 deviceHandle,
    /* [in] */ Int32 event)
{
    AutoPtr<DisplayDevice> device = (DisplayDevice*)deviceHandle;
    switch (event) {
        case DisplayAdapter::DISPLAY_DEVICE_EVENT_ADDED:
            mHost->HandleDisplayDeviceAdded(device);
            break;

        case DisplayAdapter::DISPLAY_DEVICE_EVENT_CHANGED:
            mHost->HandleDisplayDeviceChanged(device);
            break;

        case DisplayAdapter::DISPLAY_DEVICE_EVENT_REMOVED:
            mHost->HandleDisplayDeviceRemoved(device);
            break;
    }

    return NOERROR;
}

ECode CDisplayManagerService::DisplayAdapterListener::OnTraversalRequested()
{
    Object* obj = mHost->mSyncRoot;
    AutoLock lock(obj);
    mHost->ScheduleTraversalLocked(FALSE);
    return NOERROR;
}

//=====================================================================================
// CDisplayManagerService::CallbackRecord
//=====================================================================================
CAR_INTERFACE_IMPL(CDisplayManagerService::CallbackRecord, Object, IProxyDeathRecipient);

CDisplayManagerService::CallbackRecord::CallbackRecord(
    /* [in] */ CDisplayManagerService* owner,
    /* [in] */ Int32 pid,
    /* [in] */ IIDisplayManagerCallback* callback)
    : mPid(pid)
    , mWifiDisplayScanRequested(FALSE)
    , mHost(owner)
    , mCallback(callback)
{
}

CDisplayManagerService::CallbackRecord::~CallbackRecord()
{
}

//@Override
ECode CDisplayManagerService::CallbackRecord::ProxyDied()
{
    if (DEBUG) {
        Slogger::D(TAG, "Display listener for pid %d died.", mPid);
    }
    mHost->OnCallbackDied(this);

    return NOERROR;
}

void CDisplayManagerService::CallbackRecord::NotifyDisplayEventAsync(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    if (FAILED(mCallback->OnDisplayEvent(displayId, event))) {
        Slogger::W(TAG, "Failed to notify process %d that"
            "displays changed, assuming it died.", mPid);
        ProxyDied();
    }
}

//=====================================================================================
// CDisplayManagerService
//=====================================================================================

CAR_OBJECT_IMPL(CDisplayManagerService)

CDisplayManagerService::CDisplayManagerService()
    : mSafeMode(FALSE)
    , mOnlyCore(FALSE)
    , mSingleDisplayDemoMode(FALSE)
    , mNextNonDefaultDisplayId(IDisplay::DEFAULT_DISPLAY + 1)
    , mGlobalDisplayState(IDisplay::STATE_UNKNOWN)
    , mPendingTraversal(FALSE)
    , mWifiDisplayScanRequestCount(0)
{
    CDisplayViewport::New((IDisplayViewport**)&mDefaultViewport);
    CDisplayViewport::New((IDisplayViewport**)&mExternalTouchViewport);

    mPersistentDataStore = new PersistentDataStore();
    CDisplayInfo::New((IDisplayInfo**)&mTempDisplayInfo);

    CDisplayViewport::New((IDisplayViewport**)&mTempDefaultViewport);
    CDisplayViewport::New((IDisplayViewport**)&mTempExternalTouchViewport);

    mSyncRoot = new SyncRoot();

    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mDisplayTransactionListeners);
    CArrayList::New((IArrayList**)&mTempDisplayStateWorkQueue);
}

ECode CDisplayManagerService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    mContext = context;

    AutoPtr<ILooper> looper;
    assert(0 && "TODO");
    // looper = DisplayThread.get().getLooper();

    mHandler = new DisplayManagerHandler(looper, this);
    // mUiHandler = UiThread::GetHandler();
    mDisplayAdapterListener = new DisplayAdapterListener(this);

    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    sysProp->GetBoolean(String("persist.demo.singledisplay"), FALSE, &mSingleDisplayDemoMode);

    return NOERROR;
}

ECode CDisplayManagerService::OnStart()
{
    Boolean bval;
    mHandler->SendEmptyMessage(MSG_REGISTER_DEFAULT_DISPLAY_ADAPTER, &bval);

    AutoPtr<IBinder> bs;
    CDisplayManagerBinderService::New(this, (IBinder**)&bs);
    PublishBinderService(IContext::DISPLAY_SERVICE, bs, TRUE /*allowIsolated*/);

    AutoPtr<LocalService> ls = new LocalService(this);
    PublishLocalService(EIID_IDisplayManagerInternal, TO_IINTERFACE(ls));
    return NOERROR;
}

ECode CDisplayManagerService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == PHASE_WAIT_FOR_DEFAULT_DISPLAY) {
        ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
        synchronized(sync) {
            Int64 timeout = SystemClock::GetUptimeMillis() + WAIT_FOR_DEFAULT_DISPLAY_TIMEOUT;
            HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator it;
            it = mLogicalDisplays.Find(IDisplay::DEFAULT_DISPLAY);
            while (it == mLogicalDisplays.End()) {
                Int64 delay = timeout - SystemClock::GetUptimeMillis();
                if (delay <= 0) {
                    Slogger::E(TAG, "Timeout waiting for default display to be initialized.");
                    return E_RUNTIME_EXCEPTION;
                }
                if (DEBUG) {
                    Slogger::D(TAG, "waitForDefaultDisplay: waiting, timeout=%ld", delay);
                }
                // try {
                    mSyncRoot->Wait(delay);
                // } catch (InterruptedException ex) {
                // }

                it = mLogicalDisplays.Find(IDisplay::DEFAULT_DISPLAY);
            }
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::WindowManagerAndInputReady()
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        assert(0 && "TODO");
        // mWindowManagerInternal = LocalServices.getService(WindowManagerInternal.class);
        // mInputManagerInternal = LocalServices.getService(InputManagerInternal.class);
        ScheduleTraversalLocked(FALSE);
    }
    return NOERROR;
}

ECode CDisplayManagerService::SystemReady(
    /* [in] */ Boolean safeMode,
    /* [in] */ Boolean onlyCore)
{
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);
    mSafeMode = safeMode;
    mOnlyCore = onlyCore;

    Boolean result;
    mHandler->SendEmptyMessage(MSG_REGISTER_ADDITIONAL_DISPLAY_ADAPTERS, &result);

    return NOERROR;
}

ECode CDisplayManagerService::RegisterDisplayTransactionListenerInternal(
    /* [in] */ IDisplayTransactionListener* listener)
{
    // List is self-synchronized copy-on-write.
    IList::Probe(mDisplayTransactionListeners)->Add(listener);
    return NOERROR;
}

ECode CDisplayManagerService::UnregisterDisplayTransactionListenerInternal(
    /* [in] */ IDisplayTransactionListener* listener)
{
    // List is self-synchronized copy-on-write.
    IList::Probe(mDisplayTransactionListeners)->Remove(listener);
    return NOERROR;
}

ECode CDisplayManagerService::SetDisplayInfoOverrideFromWindowManagerInternal(
    /* [in] */ Int32 displayId,
    /* [in] */ IDisplayInfo* info)
{
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);

    HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator find = mLogicalDisplays.Find(displayId);
    if (find != mLogicalDisplays.End()) {
        AutoPtr<LogicalDisplay> display = find->mSecond;
        if (display->SetDisplayInfoOverrideFromWindowManagerLocked(info)) {
            SendDisplayEventLocked(displayId, IDisplayManagerGlobal::EVENT_DISPLAY_CHANGED);
            ScheduleTraversalLocked(FALSE);
        }
    }

    return NOERROR;
}

ECode CDisplayManagerService::PerformTraversalInTransactionFromWindowManagerInternal()
{
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);
    if (!mPendingTraversal) {
        return NOERROR;
    }
    mPendingTraversal = FALSE;

    PerformTraversalInTransactionLocked();

    // List is self-synchronized copy-on-write.
    IDisplayTransactionListener* listener;
    AutoPtr<IIterator> it;
    IList::Probe(mDisplayTransactionListeners)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext)) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        listener = IDisplayTransactionListener::Probe(obj);
        listener->OnDisplayTransaction();
    }

    return NOERROR;
}

ECode CDisplayManagerService::RequestGlobalDisplayStateInternal(
        /* [in] */ Int32 state)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {

        // Update the display state within the lock.
        Object* syncRoot = (Object*)mSyncRoot.Get();
        synchronized(syncRoot) {
            if (mGlobalDisplayState != state) {
                mGlobalDisplayState = state;
                UpdateGlobalDisplayStateLocked(IList::Probe(mTempDisplayStateWorkQueue));
                ScheduleTraversalLocked(FALSE);
            }
        }

        // Setting the display power state can take hundreds of milliseconds
        // to complete so we defer the most expensive part of the work until
        // after we have exited the critical section to avoid blocking other
        // threads for a Int64 time.
        IRunnable* runnable;
        Int32 size;
        mTempDisplayStateWorkQueue->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mTempDisplayStateWorkQueue->Get(i, (IInterface**)&obj);
            runnable = IRunnable::Probe(obj);
            runnable->Run();
        }

        mTempDisplayStateWorkQueue->Clear();
    }
    return NOERROR;
}

ECode CDisplayManagerService::GetDisplayInfoInternal(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 callingUid,
    /* [out] */ IDisplayInfo** displayInfo)
{
    VALIDATE_NOT_NULL(displayInfo)
    *displayInfo = NULL;

    Object* syncRoot = (Object*)mSyncRoot.Get();
    synchronized(syncRoot) {
        Boolean access;
        HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator it = mLogicalDisplays.Find(displayId);
        if (it != mLogicalDisplays.End()) {
            AutoPtr<IDisplayInfo> info = it->mSecond->GetDisplayInfoLocked();
            info->HasAccess(callingUid, &access);
            if (access) {
                *displayInfo = info;
                REFCOUNT_ADD(*displayInfo)
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::GetDisplayIdsInternal(
    /* [in] */ Int32 callingUid,
    /* [out, callee] */ ArrayOf<Int32>** displayIds)
{
    VALIDATE_NOT_NULL(displayIds);
    *displayIds = NULL;

    Object* syncRoot = (Object*)mSyncRoot.Get();
    synchronized(syncRoot) {
        Int32 count = mLogicalDisplays.GetSize();
        AutoPtr<ArrayOf<Int32> > ids = ArrayOf<Int32>::Alloc(count);
        Int32 n = 0;
        Boolean access;
        HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator it;
        for (it = mLogicalDisplays.Begin(); it != mLogicalDisplays.End(); ++it) {
            AutoPtr<IDisplayInfo> info = it->mSecond->GetDisplayInfoLocked();
            info->HasAccess(callingUid, &access);
            if (access) {
                ids->Set(n++, it->mFirst);
            }
        }

        if (n != count) {
            Arrays::CopyOfRange(ids, 0, n, displayIds);
        }
        else {
            *displayIds = ids;
            REFCOUNT_ADD(*displayIds);
        }
    }

    return NOERROR;
}

ECode CDisplayManagerService::RegisterCallbackInternal(
    /* [in] */ IIDisplayManagerCallback* callback,
    /* [in] */ Int32 callingPid)
{
    Object* syncRoot = (Object*)mSyncRoot.Get();
    synchronized(syncRoot) {
        HashMap<Int32, AutoPtr<CallbackRecord> >::Iterator find = mCallbacks.Find(callingPid);
        if (find != mCallbacks.End()) {
            Slogger::E(TAG, "The calling process has already "
                "registered an IIDisplayManagerCallback.");
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<CallbackRecord> record = new CallbackRecord(this, callingPid, callback);

        AutoPtr<IProxy> proxy = (IProxy*)callback->Probe(EIID_IProxy);
        if (proxy != NULL && FAILED(proxy->LinkToDeath(record, 0))) {
            return E_RUNTIME_EXCEPTION;
        }

        mCallbacks[callingPid] = record;
    }

    return NOERROR;
}

void CDisplayManagerService::OnCallbackDied(
    /* [in] */ CallbackRecord* record)
{
    Object* syncRoot = (Object*)mSyncRoot.Get();
    synchronized(syncRoot) {
        HashMap<Int32, AutoPtr<CallbackRecord> >::Iterator find = mCallbacks.Find(record->mPid);
        if (find != mCallbacks.End()) {
            mCallbacks.Erase(find);
        }
        StopWifiDisplayScanLocked(record);
    }
}

ECode CDisplayManagerService::StartWifiDisplayScanInternal(
    /* [in] */ Int32 callingPid)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {

        HashMap<Int32, AutoPtr<CallbackRecord> >::Iterator find = mCallbacks.Find(callingPid);
        if (find != mCallbacks.End()) {
            Slogger::E(TAG, "The calling process has not registered an IIDisplayManagerCallback.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        StartWifiDisplayScanLocked(find->mSecond);
    }
    return NOERROR;
}

ECode CDisplayManagerService::StartWifiDisplayScanLocked(
    /* [in] */ CallbackRecord* record)
{
    if (!record->mWifiDisplayScanRequested) {
        record->mWifiDisplayScanRequested = TRUE;
        if (mWifiDisplayScanRequestCount++ == 0) {
            if (mWifiDisplayAdapter != NULL) {
                mWifiDisplayAdapter->RequestStartScanLocked();
            }
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::StopWifiDisplayScanInternal(
    /* [in] */ Int32 callingPid)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        HashMap<Int32, AutoPtr<CallbackRecord> >::Iterator find = mCallbacks.Find(callingPid);
        if (find == mCallbacks.End()) {
            Slogger::E(TAG, "The calling process has not registered an IIDisplayManagerCallback.");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        StopWifiDisplayScanLocked(find->mSecond);
    }

    return NOERROR;
}

ECode CDisplayManagerService::StopWifiDisplayScanLocked(
    /* [in] */ CallbackRecord* record)
{
    if (record->mWifiDisplayScanRequested) {
        record->mWifiDisplayScanRequested = FALSE;
        if (--mWifiDisplayScanRequestCount == 0) {
            if (mWifiDisplayAdapter != NULL) {
                mWifiDisplayAdapter->RequestStopScanLocked();
            }
        }
        else if (mWifiDisplayScanRequestCount < 0) {
            // Slogger::Wtf(TAG, "mWifiDisplayScanRequestCount became negative: "
            //         + mWifiDisplayScanRequestCount);
            mWifiDisplayScanRequestCount = 0;
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::ConnectWifiDisplayInternal(
    /* [in] */ const String& address)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestConnectLocked(address);
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::PauseWifiDisplayInternal()
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestPauseLocked();
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::ResumeWifiDisplayInternal()
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestResumeLocked();
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::DisconnectWifiDisplayInternal()
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestDisconnectLocked();
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::RenameWifiDisplayInternal(
    /* [in] */ const String& address,
    /* [in] */ const String& alias)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestRenameLocked(address, alias);
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::ForgetWifiDisplayInternal(
    /* [in] */ const String& address)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mWifiDisplayAdapter != NULL) {
            mWifiDisplayAdapter->RequestForgetLocked(address);
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::GetWifiDisplayStatusInternal(
    /* [out] */ IWifiDisplayStatus** status)
{
    VALIDATE_NOT_NULL(status);

    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);
        if (mWifiDisplayAdapter != NULL) {
            AutoPtr<IWifiDisplayStatus> s = mWifiDisplayAdapter->GetWifiDisplayStatusLocked();
            *status = s;
            REFCOUNT_ADD(*status);
        }
        else {
            CWifiDisplayStatus::New(status);
        }
    }

    return NOERROR;
}

Int32 CDisplayManagerService::CreateVirtualDisplayInternal(
    /* [in] */ IIVirtualDisplayCallback* callback,
    /* [in] */ IIMediaProjection* projection,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& packageName,
    /* [in] */ const String& name,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi,
    /* [in] */ ISurface* surface,
    /* [in] */ Int32 flags)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mVirtualDisplayAdapter == NULL) {
            Slogger::W(TAG,
                "Rejecting request to create private virtual display "
                "because the virtual display adapter is not available.");
            return -1;
        }

        AutoPtr<DisplayDevice> device = mVirtualDisplayAdapter->CreateVirtualDisplayLocked(
            callback, projection, callingUid, packageName,
            name, width, height, densityDpi, surface, flags);
        if (device == NULL) {
            return -1;
        }

        HandleDisplayDeviceAddedLocked(device);
        AutoPtr<LogicalDisplay> display = FindLogicalDisplayForDeviceLocked(device);
        if (display != NULL) {
            return display->GetDisplayIdLocked();
        }

        // Something weird happened and the logical display was not created.
        Slogger::W(TAG, "Rejecting request to create virtual display "
            "because the logical display was not created.");
        mVirtualDisplayAdapter->ReleaseVirtualDisplayLocked(IBinder::Probe(callback));
        HandleDisplayDeviceRemovedLocked(device);
    }
    return -1;
}

ECode CDisplayManagerService::ResizeVirtualDisplayInternal(
    /* [in] */ IBinder* appToken,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 densityDpi)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mVirtualDisplayAdapter == NULL) {
            return NOERROR;
        }

        mVirtualDisplayAdapter->ResizeVirtualDisplayLocked(appToken, width, height, densityDpi);
    }
    return NOERROR;
}

ECode CDisplayManagerService::SetVirtualDisplaySurfaceInternal(
    /* [in] */ IBinder* appToken,
    /* [in] */ ISurface* surface)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mVirtualDisplayAdapter == NULL) {
            return NOERROR;
        }

        mVirtualDisplayAdapter->SetVirtualDisplaySurfaceLocked(appToken, surface);
    }
    return NOERROR;
}

ECode CDisplayManagerService::ReleaseVirtualDisplayInternal(
    /* [in] */ IBinder* appToken)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        if (mVirtualDisplayAdapter == NULL) {
            return NOERROR;
        }

        AutoPtr<DisplayDevice> device = mVirtualDisplayAdapter->ReleaseVirtualDisplayLocked(appToken);
        if (device != NULL) {
            HandleDisplayDeviceRemovedLocked(device);
        }
    }
    return NOERROR;
}

ECode CDisplayManagerService::RegisterDefaultDisplayAdapter()
{
    // Register default display adapter.
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);

    AutoPtr<LocalDisplayAdapter> adapter = new LocalDisplayAdapter(
        obj, mContext, mHandler, mDisplayAdapterListener);
    RegisterDisplayAdapterLocked(adapter);
    return NOERROR;
}

ECode CDisplayManagerService::RegisterAdditionalDisplayAdapters()
{
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);
    Boolean result = ShouldRegisterNonEssentialDisplayAdaptersLocked();
    if (result) {
        RegisterOverlayDisplayAdapterLocked();
        RegisterWifiDisplayAdapterLocked();
    }
    return NOERROR;
}

ECode CDisplayManagerService::RegisterOverlayDisplayAdapterLocked()
{
    AutoPtr<OverlayDisplayAdapter> adapter = new OverlayDisplayAdapter(
        (Object*)mSyncRoot.Get(), mContext, mHandler, mDisplayAdapterListener, mUiHandler);
    RegisterDisplayAdapterLocked(adapter);
    return NOERROR;
}

ECode CDisplayManagerService::RegisterWifiDisplayAdapterLocked()
{
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Boolean value;
    resources->GetBoolean(R::bool_::config_enableWifiDisplay, &value);
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    Int32 enable;
    Boolean needRegister = value || (sysProp->GetInt32(FORCE_WIFI_DISPLAY_ENABLE, -1, &enable), enable == 1);
    if (DEBUG) {
        Slogger::D(TAG, "RegisterWifiDisplayAdapterLocked needRegister: %d, value: %d, enable: %d TODO",
            needRegister, value, enable);
    }
    needRegister = TRUE; // TODO
    if (needRegister) {
        assert(mWifiDisplayAdapter == NULL);
        mWifiDisplayAdapter = new WifiDisplayAdapter(
            (Object*)mSyncRoot.Get(), mContext, mHandler, mDisplayAdapterListener,
            mPersistentDataStore);
        RegisterDisplayAdapterLocked(mWifiDisplayAdapter);
    }
    return NOERROR;
}

ECode CDisplayManagerService::RegisterVirtualDisplayAdapterLocked()
{
    mVirtualDisplayAdapter = new VirtualDisplayAdapter(
        (Object*)mSyncRoot.Get(), mContext, mHandler, mDisplayAdapterListener);
    RegisterDisplayAdapterLocked(mVirtualDisplayAdapter);
    return NOERROR;
}

Boolean CDisplayManagerService::ShouldRegisterNonEssentialDisplayAdaptersLocked()
{
    // In safe mode, we disable non-essential display adapters to give the user
    // an opportunity to fix broken settings or other problems that might affect
    // system stability.
    // In only-core mode, we disable non-essential display adapters to minimize
    // the number of dependencies that are started while in this mode and to
    // prevent problems that might occur due to the device being encrypted.
    return !mSafeMode && !mOnlyCore;
}

void CDisplayManagerService::RegisterDisplayAdapterLocked(
    /* [in] */ DisplayAdapter* adapter)
{
    mDisplayAdapters.PushBack(adapter);
    adapter->RegisterLocked();
}

void CDisplayManagerService::HandleDisplayDeviceAdded(
   /* [in] */ DisplayDevice* device)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        HandleDisplayDeviceAddedLocked(device);
    }
}

void CDisplayManagerService::HandleDisplayDeviceAddedLocked(
   /* [in] */ DisplayDevice* device)
{
    AutoPtr<DisplayDevice> dd = device;
    if (Find(mDisplayDevices.Begin(), mDisplayDevices.End(), dd) == mDisplayDevices.End()) {
        Slogger::W(TAG, "Attempted to add already added display device: %s",
            Object::ToString(device->GetDisplayDeviceInfoLocked()).string());
        return;
    }

    Slogger::I(TAG, "Display device added: %s",
        Object::ToString(device->GetDisplayDeviceInfoLocked()).string());

    mDisplayDevices.PushBack(dd);
    AddLogicalDisplayLocked(device);
    AutoPtr<IRunnable> work = UpdateDisplayStateLocked(device);
    if (work != NULL) {
        work->Run();
    }
    ScheduleTraversalLocked(FALSE);
}

void CDisplayManagerService::HandleDisplayDeviceChanged(
    /* [in] */ DisplayDevice* device)
{
    Object* obj = (Object*)mSyncRoot.Get();
    AutoLock lock(obj);
    if (Find(mDisplayDevices.Begin(), mDisplayDevices.End(),
        AutoPtr<DisplayDevice>(device)) == mDisplayDevices.End()) {
        Slogger::W(TAG, "Attempted to change non-existent display device: %s",
            Object::ToString(device->GetDisplayDeviceInfoLocked()).string());
        return;
    }

    Slogger::I(TAG, "Display device changed: %s",
        Object::ToString(device->GetDisplayDeviceInfoLocked()).string());

    device->ApplyPendingDisplayDeviceInfoChangesLocked();
    if (UpdateLogicalDisplaysLocked()) {
        ScheduleTraversalLocked(FALSE);
    }
}

void CDisplayManagerService::HandleDisplayDeviceRemoved(
    /* [in] */ DisplayDevice* device)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        HandleDisplayDeviceRemovedLocked(device);
    }
}

void CDisplayManagerService::HandleDisplayDeviceRemovedLocked(
    /* [in] */ DisplayDevice* device)
{
    AutoPtr<DisplayDevice> dd = device;
    List<AutoPtr<DisplayDevice> >::Iterator find = Find(mDisplayDevices.Begin(), mDisplayDevices.End(),dd);
    if (find == mDisplayDevices.End()) {
        Slogger::W(TAG, "Attempted to remove non-existent display device: %s",
            Object::ToString(device->GetDisplayDeviceInfoLocked()).string());
        return;
    }

    mDisplayDevices.Erase(find);

    Slogger::I(TAG, "Display device removed: %s",
        Object::ToString(device->GetDisplayDeviceInfoLocked()).string());

    UpdateLogicalDisplaysLocked();
    ScheduleTraversalLocked(FALSE);
}

void CDisplayManagerService::UpdateGlobalDisplayStateLocked(
    /* [in] */ IList* workQueue)
{
    List< AutoPtr<DisplayDevice> >::Iterator it;
    for (it = mDisplayDevices.Begin(); it != mDisplayDevices.End(); ++it) {
        AutoPtr<DisplayDevice> device = *it;
        AutoPtr<IRunnable> runnable = UpdateDisplayStateLocked(device);
        if (runnable != NULL) {
            workQueue->Add(TO_IINTERFACE(runnable));
        }
    }
}

AutoPtr<IRunnable> CDisplayManagerService::UpdateDisplayStateLocked(
    /* [in] */ DisplayDevice* device)
{
    // Blank or unblank the display immediately to match the state requested
    // by the display power controller (if known).
    AutoPtr<DisplayDeviceInfo> info = device->GetDisplayDeviceInfoLocked();
    if ((info->mFlags & DisplayDeviceInfo::FLAG_NEVER_BLANK) == 0) {
        return device->RequestDisplayStateLocked(mGlobalDisplayState);
    }
    return NULL;
}

void CDisplayManagerService::AddLogicalDisplayLocked(
    /* [in] */ DisplayDevice* device)
{
    AutoPtr<DisplayDeviceInfo> deviceInfo = device->GetDisplayDeviceInfoLocked();
    Boolean isDefault = (deviceInfo->mFlags
        & DisplayDeviceInfo::FLAG_DEFAULT_DISPLAY) != 0;
    if (isDefault && mLogicalDisplays.Find(IDisplay::DEFAULT_DISPLAY) != mLogicalDisplays.End()) {
        Slogger::W(TAG, "Ignoring attempt to add a second default display: %s"
            , Object::ToString(deviceInfo).string());
        isDefault = FALSE;
    }

    if (!isDefault && mSingleDisplayDemoMode) {
        Slogger::I(TAG, "Not creating a logical display for a secondary display "
            " because single display demo mode is enabled: %s",
            Object::ToString(deviceInfo).string());
        return;
    }

    Int32 displayId = AssignDisplayIdLocked(isDefault);
    Int32 layerStack = AssignLayerStackLocked(displayId);

    AutoPtr<LogicalDisplay> display = new LogicalDisplay(displayId, layerStack, device);
    display->UpdateLocked(mDisplayDevices);
    if (!display->IsValidLocked()) {
        // This should never happen currently.
        Slogger::W(TAG, "Ignoring display device because the logical display "
            "created from it was not considered valid: %s",
            Object::ToString(deviceInfo).string());
        return;
    }

    mLogicalDisplays[displayId] = display;

    // Wake up waitForDefaultDisplay.
    if (isDefault) {
        mSyncRoot->NotifyAll();
    }

    SendDisplayEventLocked(displayId, IDisplayManagerGlobal::EVENT_DISPLAY_ADDED);
}

Int32 CDisplayManagerService::AssignDisplayIdLocked(
    /* [in] */ Boolean isDefault)
{
    return isDefault ? IDisplay::DEFAULT_DISPLAY : mNextNonDefaultDisplayId++;
}

Int32 CDisplayManagerService::AssignLayerStackLocked(
    /* [in] */ Int32 displayId)
{
    // Currently layer stacks and display ids are the same.
    // This need not be the case.
    return displayId;
}

// Updates all existing logical displays given the current set of display devices.
// Removes invalid logical displays.
// Sends notifications if needed.
Boolean CDisplayManagerService::UpdateLogicalDisplaysLocked()
{
    Boolean changed = FALSE;
    // TODO: from last index to 0
    //
    HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator iter = mLogicalDisplays.Begin();
    for (; iter != mLogicalDisplays.End();) {
        Int32 displayId = iter->mFirst;
        AutoPtr<LogicalDisplay> display = iter->mSecond;

        mTempDisplayInfo->CopyFrom(display->GetDisplayInfoLocked());
        display->UpdateLocked(mDisplayDevices);
        if (!display->IsValidLocked()) {
            mLogicalDisplays.Erase(iter++);
            SendDisplayEventLocked(displayId, IDisplayManagerGlobal::EVENT_DISPLAY_REMOVED);
            changed = TRUE;
        }
        else if (!Object::Equals(mTempDisplayInfo, display->GetDisplayInfoLocked())) {
            ++iter;
            SendDisplayEventLocked(displayId, IDisplayManagerGlobal::EVENT_DISPLAY_CHANGED);
            changed = TRUE;
        }
        else {
            ++iter;
        }
    }
    return changed;
}

void CDisplayManagerService::PerformTraversalInTransactionLocked()
{
    // Clear all viewports before configuring displays so that we can keep
    // track of which ones we have configured.
    ClearViewportsLocked();

    // Configure each display device.
    List< AutoPtr<DisplayDevice> >::Iterator iter;
    for (iter = mDisplayDevices.Begin(); iter != mDisplayDevices.End(); ++iter) {

        ConfigureDisplayInTransactionLocked(*iter);
        (*iter)->PerformTraversalInTransactionLocked();
    }

    // Tell the input system about these new viewports.
    if (mInputManagerInternal != NULL) {
        Boolean result;
        mHandler->SendEmptyMessage(MSG_UPDATE_VIEWPORT, &result);
    }
}

void CDisplayManagerService::SetDisplayPropertiesInternal(
    /* [in] */ Int32 displayId,
    /* [in] */ Boolean hasContent,
    /* [in] */ Float requestedRefreshRate,
    /* [in] */ Boolean inTraversal)
{
    ISynchronize* sync = ISynchronize::Probe(mTempDisplayStateWorkQueue);
    synchronized(sync) {
        HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator it;
        it = mLogicalDisplays.Find(displayId);
        if (it == mLogicalDisplays.End()) {
            return;
        }
        AutoPtr<LogicalDisplay> display = it->mSecond;
        if (display == NULL) {
            return;
        }
        if (display->HasContentLocked() != hasContent) {
            if (DEBUG) {
                Slogger::D(TAG, "Display %d hasContent flag changed: hasContent=%d inTraversal=%d",
                    displayId, hasContent, inTraversal);
            }

            display->SetHasContentLocked(hasContent);
            ScheduleTraversalLocked(inTraversal);
        }
        if (display->GetRequestedRefreshRateLocked() != requestedRefreshRate) {
            if (DEBUG) {
                Slogger::D(TAG, "Display %d has requested a new refresh rate: %f fps",
                    displayId, requestedRefreshRate);
            }
            display->SetRequestedRefreshRateLocked(requestedRefreshRate);
            ScheduleTraversalLocked(inTraversal);
        }
    }
}

void CDisplayManagerService::ClearViewportsLocked()
{
    mDefaultViewport->SetValid(FALSE);
    mExternalTouchViewport->SetValid(FALSE);
}

void CDisplayManagerService::ConfigureDisplayInTransactionLocked(
    /* [in] */ DisplayDevice* device)
{
    AutoPtr<DisplayDeviceInfo> info = device->GetDisplayDeviceInfoLocked();
    Boolean ownContent = (info->mFlags & DisplayDeviceInfo::FLAG_OWN_CONTENT_ONLY) != 0;

    // Find the logical display that the display device is showing.
    // Certain displays only ever show their own content.
    AutoPtr<LogicalDisplay> display = FindLogicalDisplayForDeviceLocked(device);
    if (!ownContent) {
        if (display != NULL && !display->HasContentLocked()) {
            // If the display does not have any content of its own, then
            // automatically mirror the default logical display contents.
            display = NULL;
        }
        if (display == NULL) {
            HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator it;
            it = mLogicalDisplays.Find(IDisplay::DEFAULT_DISPLAY);
            if (it != mLogicalDisplays.End()) {
                display = it->mSecond;
            }
        }
    }

    // Apply the logical display configuration to the display device.
    if (display == NULL) {
        // TODO: no logical display for the device, blank it
        Slogger::W(TAG, "Missing logical display to use for physical display device: %s",
            Object::ToString(device->GetDisplayDeviceInfoLocked()).string());
        return;
    }

    display->ConfigureDisplayInTransactionLocked(device, info->mState == IDisplay::STATE_OFF);

    // Update the viewports if needed.
    Boolean bval;
    mDefaultViewport->IsValid(&bval);
    if (!bval
        && (info->mFlags & DisplayDeviceInfo::FLAG_DEFAULT_DISPLAY) != 0) {
        SetViewportLocked(mDefaultViewport, display, device);
    }

    mExternalTouchViewport->IsValid(&bval);
    if (!bval
            && info->mTouch == DisplayDeviceInfo::TOUCH_EXTERNAL) {
        SetViewportLocked(mExternalTouchViewport, display, device);
    }
}

void CDisplayManagerService::SetViewportLocked(
    /* [in] */ IDisplayViewport* viewport,
    /* [in] */ LogicalDisplay* display,
    /* [in] */ DisplayDevice* device)
{
    viewport->SetValid(TRUE);
    assert(0 && "TODO");
    // viewport->SetDisplayId(display->GetDisplayIdLocked());
    device->PopulateViewportLocked(viewport);
}

AutoPtr<LogicalDisplay> CDisplayManagerService::FindLogicalDisplayForDeviceLocked(
    /* [in] */ DisplayDevice* device)
{
    HashMap<Int32, AutoPtr<LogicalDisplay> >::Iterator iter;
    for (iter = mLogicalDisplays.Begin(); iter != mLogicalDisplays.End(); ++iter) {
        AutoPtr<LogicalDisplay> display = iter->mSecond;
        if (display->GetPrimaryDisplayDeviceLocked().Get() == device) {
            return display;
        }
    }
    return NULL;
}

void CDisplayManagerService::SendDisplayEventLocked(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_DELIVER_DISPLAY_EVENT, displayId, event, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

// Requests that performTraversalsInTransactionFromWindowManager be called at a
// later time to apply changes to surfaces and displays.
void CDisplayManagerService::ScheduleTraversalLocked(
    /* [in] */ Boolean inTraversal)
{
    if (DEBUG) {
        Slogger::D(TAG, "ScheduleTraversalLocked inTraversal:%d", inTraversal);
    }

    if (!mPendingTraversal && mWindowManagerInternal != NULL) {
        mPendingTraversal = TRUE;
        if (!inTraversal) {
            Boolean result;
            mHandler->SendEmptyMessage(MSG_REQUEST_TRAVERSAL, &result);
        }
    }
}

// Runs on Handler thread.
// Delivers display event notifications to callbacks.
void CDisplayManagerService::DeliverDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    if (DEBUG) {
        Slogger::D(TAG, "Delivering display event: displayId=%d, event=%d",
            displayId, event);
    }

    // Grab the lock and copy the callbacks.
    {
        Object* obj = (Object*)mSyncRoot.Get();
        AutoLock lock(obj);
        mTempCallbacks.Clear();
        HashMap<Int32, AutoPtr<CallbackRecord> >::Iterator iter;
        for (iter = mCallbacks.Begin(); iter != mCallbacks.End(); ++iter) {
            mTempCallbacks.PushBack(iter->mSecond);
        }
    }

    // After releasing the lock, send the notifications out.
    List<AutoPtr<CallbackRecord> >::Iterator iter = mTempCallbacks.Begin();
    for (; iter != mTempCallbacks.End(); ++iter) {
        (*iter)->NotifyDisplayEventAsync(displayId, event);
    }
    mTempCallbacks.Clear();
}

AutoPtr<IIMediaProjectionManager> CDisplayManagerService::GetProjectionService()
{
    if (mProjectionService == NULL) {
        AutoPtr<IServiceManager> mgr;
        CServiceManager::AcquireSingleton((IServiceManager**)&mgr);
        AutoPtr<IInterface> obj;
        mgr->GetService(IContext::MEDIA_PROJECTION_SERVICE, (IInterface**)&obj);
        mProjectionService = IIMediaProjectionManager::Probe(obj);
    }
    return mProjectionService;
}

ECode CDisplayManagerService::DumpInternal(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // pw.println("DISPLAY MANAGER (dumpsys display)");

    // synchronized(mSyncRoot) {
    //     pw.println("  mOnlyCode=" + mOnlyCore);
    //     pw.println("  mSafeMode=" + mSafeMode);
    //     pw.println("  mPendingTraversal=" + mPendingTraversal);
    //     pw.println("  mGlobalDisplayState=" + Display.stateToString(mGlobalDisplayState));
    //     pw.println("  mNextNonDefaultDisplayId=" + mNextNonDefaultDisplayId);
    //     pw.println("  mDefaultViewport=" + mDefaultViewport);
    //     pw.println("  mExternalTouchViewport=" + mExternalTouchViewport);
    //     pw.println("  mSingleDisplayDemoMode=" + mSingleDisplayDemoMode);
    //     pw.println("  mWifiDisplayScanRequestCount=" + mWifiDisplayScanRequestCount);

    //     IndentingPrintWriter ipw = new IndentingPrintWriter(pw, "    ");
    //     ipw.increaseIndent();

    //     pw.println();
    //     pw.println("Display Adapters: size=" + mDisplayAdapters.size());
    //     for (DisplayAdapter adapter : mDisplayAdapters) {
    //         pw.println("  " + adapter.getName());
    //         adapter.dumpLocked(ipw);
    //     }

    //     pw.println();
    //     pw.println("Display Devices: size=" + mDisplayDevices.size());
    //     for (DisplayDevice device : mDisplayDevices) {
    //         pw.println("  " + device->GetDisplayDeviceInfoLocked());
    //         device.dumpLocked(ipw);
    //     }

    //     Int32 logicalDisplayCount = mLogicalDisplays.size();
    //     pw.println();
    //     pw.println("Logical Displays: size=" + logicalDisplayCount);
    //     for (int i = 0; i < logicalDisplayCount; i++) {
    //         Int32 displayId = mLogicalDisplays.keyAt(i);
    //         LogicalDisplay display = mLogicalDisplays.valueAt(i);
    //         pw.println("  Display " + displayId + ":");
    //         display.dumpLocked(ipw);
    //     }

    //     Int32 callbackCount = mCallbacks.size();
    //     pw.println();
    //     pw.println("Callbacks: size=" + callbackCount);
    //     for (int i = 0; i < callbackCount; i++) {
    //         CallbackRecord callback = mCallbacks.valueAt(i);
    //         pw.println("  " + i + ": mPid=" + callback.mPid
    //                 + ", mWifiDisplayScanRequested=" + callback.mWifiDisplayScanRequested);
    //     }

    //     if (mDisplayPowerController != NULL) {
    //         mDisplayPowerController->dump(pw);
    //     }
    // }
    return NOERROR;
}

} // namespace Display
} // namespace Server
} // namepsace Droid
} // namespace Elastos
