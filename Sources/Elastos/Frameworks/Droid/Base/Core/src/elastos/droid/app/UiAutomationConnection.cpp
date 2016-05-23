
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/UiAutomationConnection.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfo.h"
#include "elastos/droid/hardware/input/CInputManager.h"
#include "elastos/droid/view/CWindowAnimationFrameStats.h"
#include "elastos/droid/view/SurfaceControl.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::CAccessibilityServiceInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::CInputManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::SurfaceControl;
using Elastos::Droid::View::CWindowAnimationFrameStats;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String UiAutomationConnection::TAG("UiAutomationConnection");
const Int32 UiAutomationConnection::INITIAL_FROZEN_ROTATION_UNSPECIFIED = -1;

CAR_INTERFACE_IMPL_2(UiAutomationConnection, Object, IIUiAutomationConnection, IBinder)

UiAutomationConnection::UiAutomationConnection()
    : mInitialFrozenRotation(INITIAL_FROZEN_ROTATION_UNSPECIFIED)
    , mIsShutdown(FALSE)
    , mOwningUid(0)
{
}

UiAutomationConnection::~UiAutomationConnection()
{}

ECode UiAutomationConnection::constructor()
{
    AutoPtr<IInterface> window = ServiceManager::GetService(IContext::WINDOW_SERVICE);
    AutoPtr<IIWindowManager> mWindowManager = IIWindowManager::Probe(window);

    AutoPtr<IInterface> accessibility = ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE);
    AutoPtr<IIAccessibilityManager> mAccessibilityManager = IIAccessibilityManager::Probe(accessibility);

    CBinder::New((IBinder**)&mToken);
    return NOERROR;
}

ECode UiAutomationConnection::Connect(
    /* [in] */ IIAccessibilityServiceClient* client)
{
    if (client == NULL) {
        Logger::E(TAG, "IllegalArgumentException: Client cannot be NULL!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfShutdownLocked())
        if (IsConnectedLocked()) {
            Logger::E(TAG, "IllegalArgumentException: Already connected.!");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        mOwningUid = Binder::GetCallingUid();
        RegisterUiTestAutomationServiceLocked(client);
        StoreRotationStateLocked();
    }
    return NOERROR;
}

ECode UiAutomationConnection::Disconnect()
{
    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        if (!IsConnectedLocked()) {
            Logger::E(TAG, "Already disconnected.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mOwningUid = -1;
        UnregisterUiTestAutomationServiceLocked();
        RestoreRotationStateLocked();
    }
    return NOERROR;
}

ECode UiAutomationConnection::InjectInputEvent(
    /* [in] */ IInputEvent* event,
    /* [in] */ Boolean sync,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int32 mode = (sync) ? IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH
            : IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC;
    Int64 identity = Binder::ClearCallingIdentity();
    ECode ec = CInputManager::GetInstance()->InjectInputEvent(event, mode, result);
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode UiAutomationConnection::SetRotation(
    /* [in] */ Int32 rotation,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    ECode ec = NOERROR;
    if (rotation == IUiAutomation::ROTATION_UNFREEZE) {
        ec = mWindowManager->ThawRotation();
    } else {
        ec = mWindowManager->FreezeRotation(rotation);
    }
    if (SUCCEEDED(ec)) {
        *result = TRUE;
    }
    // } catch (RemoteException re) {
        /* ignore */
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode UiAutomationConnection::TakeScreenshot(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bmp)
{
    VALIDATE_NOT_NULL(bmp)
    *bmp = NULL;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IBitmap> tmp = SurfaceControl::Screenshot(width, height);
    *bmp = tmp;
    REFCOUNT_ADD(*bmp)
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode UiAutomationConnection::ClearWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
        AutoPtr<IBinder> token;
        mAccessibilityManager->GetWindowToken(windowId, (IBinder**)&token);
        if (token == NULL) {
            return NOERROR;
        }
        ECode ec = mWindowManager->ClearWindowContentFrameStats(token, result);
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode UiAutomationConnection::GetWindowContentFrameStats(
    /* [in] */ Int32 windowId,
    /* [out] */ IWindowContentFrameStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = NULL;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
        AutoPtr<IBinder> token;
        mAccessibilityManager->GetWindowToken(windowId, (IBinder**)&token);
        if (token == NULL) {
            return NOERROR;
        }
        ECode ec = mWindowManager->GetWindowContentFrameStats(token, stats);
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return ec;
}

ECode UiAutomationConnection::ClearWindowAnimationFrameStats()
{
    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
        Boolean bval;
        SurfaceControl::ClearAnimationFrameStats(&bval);
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode UiAutomationConnection::GetWindowAnimationFrameStats(
    /* [out] */ IWindowAnimationFrameStats** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<IWindowAnimationFrameStats> stats;
    CWindowAnimationFrameStats::New((IWindowAnimationFrameStats**)&stats);
    Boolean bval;
    SurfaceControl::GetAnimationFrameStats(stats, &bval);
    *result = stats;
    REFCOUNT_ADD(*result)
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode UiAutomationConnection::ExecuteShellCommand(
    /* [in] */ const String& command,
    /* [in] */ IParcelFileDescriptor* sink)
{
    {    AutoLock syncLock(mLock);
        FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        FAIL_RETURN(ThrowIfShutdownLocked())
        FAIL_RETURN(ThrowIfNotConnectedLocked())
    }

    AutoPtr<IInputStream> in;
    AutoPtr<IOutputStream> out;

    assert(0 && "TODO");
    // java.lang.Process process = Runtime.getRuntime()->Exec(command);

    // in = process.getInputStream();
    AutoPtr<IFileDescriptor> fd;
    sink->GetFileDescriptor((IFileDescriptor**)&fd);
    CFileOutputStream::New(fd, (IOutputStream**)&out);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(8192);
    Int32 readByteCount;
    ECode ec = NOERROR;
    while (TRUE) {
        ec = in->Read(buffer, &readByteCount);
        FAIL_GOTO(ec, _EXIT_)
        if (readByteCount < 0) {
            break;
        }
        ec = out->Write(buffer, 0, readByteCount);
        FAIL_GOTO(ec, _EXIT_)
    }

_EXIT_:
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    ioUtils->CloseQuietly(ICloseable::Probe(in));
    ioUtils->CloseQuietly(ICloseable::Probe(out));
    ioUtils->CloseQuietly(ICloseable::Probe(sink));

    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E(TAG, "Error running shell command");
        return E_RUNTIME_EXCEPTION;
    }
    return ec;
}

ECode UiAutomationConnection::Shutdown()
{
    {    AutoLock syncLock(mLock);
        if (IsConnectedLocked()) {
            FAIL_RETURN(ThrowIfCalledByNotTrustedUidLocked())
        }
        FAIL_RETURN(ThrowIfShutdownLocked())
        mIsShutdown = TRUE;
        if (IsConnectedLocked()) {
            Disconnect();
        }
    }
    return NOERROR;
}

ECode UiAutomationConnection::RegisterUiTestAutomationServiceLocked(
    /* [in] */ IIAccessibilityServiceClient* client)
{
    AutoPtr<IInterface> accessibility = ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE);
    AutoPtr<IIAccessibilityManager> manager = IIAccessibilityManager::Probe(accessibility);

    AutoPtr<IAccessibilityServiceInfo> info;
    CAccessibilityServiceInfo::New((IAccessibilityServiceInfo**)&info);

    info->SetEventTypes(IAccessibilityEvent::TYPES_ALL_MASK);
    info->SetFeedbackType(IAccessibilityServiceInfo::FEEDBACK_GENERIC);
    Int32 flags;
    info->GetFlags(&flags);
    flags |= IAccessibilityServiceInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS
            | IAccessibilityServiceInfo::FLAG_REPORT_VIEW_IDS;
    info->SetFlags(flags);
    info->SetCapabilities(IAccessibilityServiceInfo::CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT
            | IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_TOUCH_EXPLORATION
            | IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_ENHANCED_WEB_ACCESSIBILITY
            | IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_FILTER_KEY_EVENTS);
    // try {
        // Calling out with a lock held is fine since if the system
        // process is gone the client calling in will be killed.
        ECode ec = manager->RegisterUiTestAutomationService(mToken, client, info);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Logger::E(TAG, "Error while registering UiTestAutomationService.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        FAIL_RETURN(ec)
        mClient = client;
        return NOERROR;
    // } catch (RemoteException re) {
    //     throw new IllegalStateException("Error while registering UiTestAutomationService.", re);
    // }
}

ECode UiAutomationConnection::UnregisterUiTestAutomationServiceLocked()
{
    AutoPtr<IInterface> accessibility = ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE);
    AutoPtr<IIAccessibilityManager> manager = IIAccessibilityManager::Probe(accessibility);

    // try {
    // Calling out with a lock held is fine since if the system
    // process is gone the client calling in will be killed.
    ECode ec = manager->UnregisterUiTestAutomationService(mClient);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::E(TAG, "Error while unregistering UiTestAutomationService");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(ec)
    mClient = NULL;
    // } catch (RemoteException re) {
    //     throw new IllegalStateException("Error while unregistering UiTestAutomationService",
    //             re);
    // }
    return NOERROR;
}

ECode UiAutomationConnection::StoreRotationStateLocked()
{
    // try {
    Boolean bval;
    mWindowManager->IsRotationFrozen(&bval);
    if (bval) {
        // Calling out with a lock held is fine since if the system
        // process is gone the client calling in will be killed.
        return mWindowManager->GetRotation(&mInitialFrozenRotation);
    }
    return NOERROR;
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
}

ECode UiAutomationConnection::RestoreRotationStateLocked()
{
    // try {
        if (mInitialFrozenRotation != INITIAL_FROZEN_ROTATION_UNSPECIFIED) {
            // Calling out with a lock held is fine since if the system
            // process is gone the client calling in will be killed.
            return mWindowManager->FreezeRotation(mInitialFrozenRotation);
        } else {
            // Calling out with a lock held is fine since if the system
            // process is gone the client calling in will be killed.
            return mWindowManager->ThawRotation();
        }
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
}

Boolean UiAutomationConnection::IsConnectedLocked()
{
    return mClient != NULL;
}

ECode UiAutomationConnection::ThrowIfShutdownLocked()
{
    if (mIsShutdown) {
        Logger::E(TAG, "IllegalStateException: Connection shutdown!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode UiAutomationConnection::ThrowIfNotConnectedLocked()
{
    if (!IsConnectedLocked()) {
        Logger::E(TAG, "IllegalStateException: Not connected!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode UiAutomationConnection::ThrowIfCalledByNotTrustedUidLocked()
{
    Int32 callingUid = Binder::GetCallingUid();
    if (callingUid != mOwningUid && mOwningUid != IProcess::SYSTEM_UID
            && callingUid != 0 /*root*/) {
        Logger::E(TAG, "SecurityException: Calling from not trusted UID!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode UiAutomationConnection::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
