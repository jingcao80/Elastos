
#include "elastos/droid/view/IWindowManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"

using Elastos::Droid::Os::AndroidParcelUtils;

namespace Elastos {
namespace Droid {
namespace View {

const String IWindowManagerProxy::TAG("IWindowManagerProxy");
const String IWindowManagerProxy::DESCRIPTOR("android.view.IWindowManager");
const Int32 IWindowManagerProxy::TRANSACTION_openSession = android::IBinder::FIRST_CALL_TRANSACTION + 3;
const Int32 IWindowManagerProxy::TRANSACTION_getCurrentAnimatorScale = android::IBinder::FIRST_CALL_TRANSACTION + 48;

CAR_INTERFACE_IMPL(IWindowManagerProxy, Object, IIWindowManager);

IWindowManagerProxy::IWindowManagerProxy(
    /* [in] */ android::sp<android::IBinder>& wm)
    : mWm(wm)
{}

ECode IWindowManagerProxy::StartViewServer(
    /* [in] */ Int32 port,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StopViewServer(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::IsViewServerRunning(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::OpenSession(
    /* [in] */ IIWindowSessionCallback* cb,
    /* [in] */ IInputMethodClient* client,
    /* [in] */ IIInputContext* inputContext,
    /* [out] */ IWindowSession** session)
{
    VALIDATE_NOT_NULL(session);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIIWindowSessionCallback(data, cb);
    AndroidParcelUtils::WriteIInputMethodClient(data, client);
    AndroidParcelUtils::WriteIIInputContext(data, inputContext);
    mWm->transact(TRANSACTION_openSession, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    AutoPtr<IWindowSession> result = AndroidParcelUtils::ReadWindowSession(reply);
    *session = result;
    REFCOUNT_ADD(*session);
    return ec;
}

ECode IWindowManagerProxy::InputMethodClientHasFocus(
    /* [in] */ IInputMethodClient* client,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetInitialDisplaySize(
    /* [in] */ Int32 displayId,
    /* [in] */ IPoint* inSize,
    /* [out] */ IPoint** outSize)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetBaseDisplaySize(
    /* [in] */ Int32 displayId,
    /* [in] */ IPoint* inSize,
    /* [out] */ IPoint** outSize)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetForcedDisplaySize(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ClearForcedDisplaySize(
    /* [in] */ Int32 displayId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetInitialDisplayDensity(
    /* [in] */ Int32 displayId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetBaseDisplayDensity(
    /* [in] */ Int32 displayId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetForcedDisplayDensity(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 density)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ClearForcedDisplayDensity(
    /* [in] */ Int32 displayId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetOverscan(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::PauseKeyDispatching(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ResumeKeyDispatching(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetEventDispatching(
    /* [in] */ Boolean enabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::AddWindowToken(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 type)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::RemoveWindowToken(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::AddAppToken(
    /* [in] */ Int32 addPos,
    /* [in] */ IApplicationToken* token,
    /* [in] */ Int32 groupId,
    /* [in] */ Int32 stackId,
    /* [in] */ Int32 requestedOrientation,
    /* [in] */ Boolean fullscreen,
    /* [in] */ Boolean showWhenLocked,
    /* [in] */ Int32 userId,
    /* [in] */ Int32 configChanges,
    /* [in] */ Boolean voiceInteraction,
    /* [in] */ Boolean launchTaskBehind)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAppGroupId(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 groupId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAppOrientation(
    /* [in] */ IApplicationToken* token,
    /* [in] */ Int32 requestedOrientation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetAppOrientation(
    /* [in] */ IApplicationToken* token,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetFocusedApp(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean moveFocusNow)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::PrepareAppTransition(
    /* [in] */ Int32 transit,
    /* [in] */ Boolean alwaysKeepCurrent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetPendingAppTransition(
    /* [in] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::OverridePendingAppTransition(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 enterAnim,
    /* [in] */ Int32 exitAnim,
    /* [in] */ IIRemoteCallback* startedCallback)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::OverridePendingAppTransitionScaleUp(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 startWidth,
    /* [in] */ Int32 startHeight)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::OverridePendingAppTransitionThumb(
    /* [in] */ IBitmap* srcThumb,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IIRemoteCallback* startedCallback,
    /* [in] */ Boolean scaleUp)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::OverridePendingAppTransitionAspectScaledThumb(
    /* [in] */ IBitmap* srcThumb,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IIRemoteCallback* startedCallback,
    /* [in] */ Boolean scaleUp)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ExecuteAppTransition()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAppStartingWindow(
    /* [in] */ IBinder* token,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 theme,
    /* [in] */ ICompatibilityInfo* compatInfo,
    /* [in] */ ICharSequence* nonLocalizedLabel,
    /* [in] */ Int32 labelRes,
    /* [in] */ Int32 icon,
    /* [in] */ Int32 logo,
    /* [in] */ Int32 windowFlags,
    /* [in] */ IBinder* transferFrom,
    /* [in] */ Boolean createIfNeeded)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAppWillBeHidden(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAppVisibility(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean visible)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StartAppFreezingScreen(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 configChanges)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StopAppFreezingScreen(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean force)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::RemoveAppToken(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::UpdateOrientationFromAppTokens(
    /* [in] */ IConfiguration* currentConfig,
    /* [in] */ IBinder* freezeThisOneIfNeeded,
    /* [out] */ IConfiguration** cfg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetNewConfiguration(
    /* [in] */ IConfiguration* config)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StartFreezingScreen(
    /* [in] */ Int32 exitAnim,
    /* [in] */ Int32 enterAnim)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StopFreezingScreen()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::DisableKeyguard(
    /* [in] */ IBinder* token,
    /* [in] */ const String& tag)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ReenableKeyguard(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ExitKeyguardSecurely(
    /* [in] */ IOnKeyguardExitResult* cb)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::IsKeyguardLocked(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::IsKeyguardSecure(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::InKeyguardRestrictedInputMode(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::DismissKeyguard()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::KeyguardGoingAway(
    /* [in] */ Boolean disableWindowAnimations,
    /* [in] */ Boolean keyguardGoingToNotificationShade,
    /* [in] */ Boolean keyguardShowingMedia)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// These can only be called with the SET_ANIMATON_SCALE permission.
ECode IWindowManagerProxy::GetAnimationScale(
    /* [in] */ Int32 which,
    /* [out] */ Float* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetAnimationScales(
    /* [out, callee] */ ArrayOf<Float>** scales)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAnimationScale(
    /* [in] */ Int32 which,
    /* [in] */ Float scale)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetAnimationScales(
    /* [in] */ ArrayOf<Float>* scales)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetCurrentAnimatorScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    mWm->transact(TRANSACTION_getCurrentAnimatorScale, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadFloat(reply);
    return ec;
}

ECode IWindowManagerProxy::SetInTouchMode(
    /* [in] */ Boolean showFocus)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ShowStrictModeViolation(
    /* [in] */ Boolean on)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetStrictModeVisualIndicatorPreference(
    /* [in] */ const String& enabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::SetScreenCaptureDisabled(
    /* [in] */ Int32 userId,
    /* [in] */ Boolean disabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::UpdateRotation(
    /* [in] */ Boolean alwaysSendConfiguration,
    /* [in] */ Boolean forceRelayout)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetRotation(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::WatchRotation(
    /* [in] */ IRotationWatcher* watcher,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::RemoveRotationWatcher(
    /* [in] */ IRotationWatcher* watcher)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetPreferredOptionsPanelGravity(
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::FreezeRotation(
    /* [in] */ Int32 rotation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ThawRotation()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::IsRotationFrozen(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ScreenshotApplications(
    /* [in] */ IBinder* appToken,
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 maxWidth,
    /* [in] */ Int32 maxHeight,
    /* [in] */ Boolean force565,
    /* [out] */ IBitmap** bitmap)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetLastWallpaperX(
    /* [out] */ Int32* x)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetLastWallpaperY(
    /* [out] */ Int32* y)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::StatusBarVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::HasNavigationBar(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::HasPermanentMenuKey(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode IWindowManagerProxy::NeedsNavigationBar(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::LockNow(
    /* [in] */ IBundle* options)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::IsSafeModeEnabled(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::EnableScreenIfNeeded()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::ClearWindowContentFrameStats(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IWindowManagerProxy::GetWindowContentFrameStats(
    /* [in] */ IBinder* token,
    /* [out] */ IWindowContentFrameStats** stats)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
