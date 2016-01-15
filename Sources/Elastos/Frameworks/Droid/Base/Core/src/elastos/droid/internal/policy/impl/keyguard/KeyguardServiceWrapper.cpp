#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/internal/policy/impl/keyguard/KeyguardServiceWrapper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

CAR_INTERFACE_IMPL(KeyguardServiceWrapper, Object, IIKeyguardService)
String KeyguardServiceWrapper::TAG("KeyguardServiceWrapper");

KeyguardServiceWrapper::KeyguardServiceWrapper()
{
}

ECode KeyguardServiceWrapper::constructor(
    /* [in] */ IIKeyguardService* service)
{
    mService = service;
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsShowing(
    /* [out] */ Boolean* showing)
{
    /* Remove_Java
    try {
        return mService.isShowing();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    return false;
    */
    VALIDATE_NOT_NULL(showing);
    *showing = FALSE;
    ECode ecode = mService->IsShowing(showing);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsSecure(
    /* [out] */ Boolean* secure)
{
    /* Remove_Java
    try {
        return mService.isSecure();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    return false; // TODO cache state
    */
    VALIDATE_NOT_NULL(secure);
    *secure = FALSE;
    ECode ecode = mService->IsSecure(secure);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsShowingAndNotOccluded(
    /* [out] */ Boolean* showingAndNotOcclude)
{
    /* Remove_Java
    try {
        return mService.isShowingAndNotOccluded();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    return false; // TODO cache state
    */
    VALIDATE_NOT_NULL(showingAndNotOcclude);
    *showingAndNotOcclude = FALSE;
    ECode ecode = mService->IsShowingAndNotOccluded(showingAndNotOcclude);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsInputRestricted(
    /* [out] */ Boolean* inputRestricted)
{
    /* Remove_Java
    try {
        return mService.isInputRestricted();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    return false; // TODO cache state
    */
    VALIDATE_NOT_NULL(inputRestricted);
    *inputRestricted = FALSE;
    ECode ecode = mService->IsInputRestricted(inputRestricted);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::IsDismissable(
    /* [out] */ Boolean* dismissable)
{
    /* Remove_Java
    try {
        return mService.isDismissable();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    return true; // TODO cache state
    */
    VALIDATE_NOT_NULL(dismissable);
    *dismissable = FALSE;
    ECode ecode = mService->IsDismissable(dismissable);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::VerifyUnlock(
    /* [in] */ IIKeyguardExitCallback* cb)
{
    /* Remove_Java
    try {
        mService.verifyUnlock(callback);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->VerifyUnlock(cb);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    /* Remove_Java
    try {
        mService.keyguardDone(authenticated, wakeup);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->KeyguardDone(authenticated, wakeup);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetOccluded(
    /* [in] */ Boolean isOccluded,
    /* [out] */ Int32* result)

{
    /* Remove_Java
    try {
        return mService.setOccluded(isOccluded);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
        return IKeyguardServiceConstants.KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_NONE;
    }
    */
    ECode ecode = mService->SetOccluded(isOccluded, result);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        *result = IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_NONE;
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::Dismiss()
{
    /* Remove_Java
    try {
        mService.dismiss();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->Dismiss();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnDreamingStarted()
{
    /* Remove_Java
    try {
        mService.onDreamingStarted();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnDreamingStarted();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnDreamingStopped()
{
    /* Remove_Java
    try {
        mService.onDreamingStopped();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnDreamingStopped();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnScreenTurnedOff(
    /* [in] */ Int32 reason)
{
    /* Remove_Java
    try {
        mService.onScreenTurnedOff(reason);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnScreenTurnedOff(reason);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* cb)
{
    /* Remove_Java
    try {
        mService.onScreenTurnedOn(result);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnScreenTurnedOn(cb);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    /* Remove_Java
    try {
        mService.setKeyguardEnabled(enabled);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->SetKeyguardEnabled(enabled);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnSystemReady()
{
    /* Remove_Java
    try {
        mService.onSystemReady();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnSystemReady();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::DoKeyguardTimeout(
    /* [in] */ IBundle* options)
{
    /* Remove_Java
    try {
        mService.doKeyguardTimeout(options);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->DoKeyguardTimeout(options);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    /* Remove_Java
    try {
        mService.setCurrentUser(userId);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->SetCurrentUser(userId);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnBootCompleted()
{
    /* Remove_Java
    try {
        mService.onBootCompleted();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnBootCompleted();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)

{
    /* Remove_Java
    try {
        mService.startKeyguardExitAnimation(startTime, fadeoutDuration);
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->StartKeyguardExitAnimation(startTime, fadeoutDuration);
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::OnActivityDrawn()
{
    /* Remove_Java
    try {
        mService.onActivityDrawn();
    } catch (RemoteException e) {
        Slog.w(TAG , "Remote Exception", e);
    }
    */
    ECode ecode = mService->OnActivityDrawn();
    if (FAILED(ecode))
    {
        //Slog.w(TAG , "Remote Exception", e);
        Logger::W(TAG, "Remote Exception, ecode: 0x%x", ecode);
        return ecode;
    }
    return NOERROR;
}

ECode KeyguardServiceWrapper::ShowAssistant()
{
    // Not used by PhoneWindowManager
    return NOERROR;
}

ECode KeyguardServiceWrapper::Dispatch(
    /* [in] */ IMotionEvent* event)
{
    // Not used by PhoneWindowManager.  See code in {@link NavigationBarView}
    return NOERROR;
}

ECode KeyguardServiceWrapper::LaunchCamera()
{
    // Not used by PhoneWindowManager.  See code in {@link NavigationBarView}
    return NOERROR;
}

/*
//@Override
public IBinder asBinder() {
    return mService.asBinder();
}
*/

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos
