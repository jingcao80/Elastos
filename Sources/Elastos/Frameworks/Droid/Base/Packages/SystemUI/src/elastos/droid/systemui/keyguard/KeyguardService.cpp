#include "elastos/droid/systemui/keyguard/KeyguardService.h"
#include "elastos/droid/content/ContextWrapper.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::ContextWrapper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::Policy::EIID_IIKeyguardService;
using Elastos::Droid::Internal::Policy::IIKeyguardServiceConstants;
using Elastos::Droid::Manifest;
using Elastos::Droid::SystemUI::ISystemUIApplication;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardViewMediator;
using Elastos::Droid::SystemUI::Keyguard::EIID_IKeyguardService;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

//--------------------------------------------------------------------------------------
// KeyguardService::KeyguardServiceBinder
//--------------------------------------------------------------------------------------

CAR_INTERFACE_IMPL_2(KeyguardService::KeyguardServiceBinder, Object, IIKeyguardService, IBinder)

KeyguardService::KeyguardServiceBinder::KeyguardServiceBinder()
{}

KeyguardService::KeyguardServiceBinder::KeyguardServiceBinder(
    /* [in] */ KeyguardService* host)
    : mIsOccluded(FALSE)
    , mHost(host)
{}

ECode KeyguardService::KeyguardServiceBinder::constructor(
    /* [in] */ Handle32 host)
{
    mIsOccluded = FALSE;
    mHost = (KeyguardService*)host;
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::IsShowing(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsShowing(result);
}

ECode KeyguardService::KeyguardServiceBinder::IsSecure(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsSecure(result);
}

ECode KeyguardService::KeyguardServiceBinder::IsShowingAndNotOccluded(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsShowingAndNotOccluded(result);
}

ECode KeyguardService::KeyguardServiceBinder::IsInputRestricted(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsInputRestricted(result);
}

ECode KeyguardService::KeyguardServiceBinder::VerifyUnlock(
    /* [in] */ IIKeyguardExitCallback* callback)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->VerifyUnlock(callback);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->KeyguardDone(authenticated, wakeup);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::SetOccluded(
    /* [in] */ Boolean isOccluded,
    /* [out] */ Int32* result)
{
    mHost->CheckPermission();
    synchronized(this) {
        Boolean b;
        mHost->mKeyguardViewMediator->IsShowing(&b);
        if (isOccluded && b && !mIsOccluded) {
            *result = IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_UNSET_FLAGS;
        }
        else if (!isOccluded && b && mIsOccluded) {
            *result = IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_SET_FLAGS;
        }
        else {
            *result = IIKeyguardServiceConstants::KEYGUARD_SERVICE_SET_OCCLUDED_RESULT_NONE;
        }
        if (mIsOccluded != isOccluded) {
            mHost->mKeyguardViewMediator->SetOccluded(isOccluded);

            // Cache the value so we always have a fresh view in whether Keyguard is occluded.
            // If we would just call mKeyguardViewMediator.isOccluded(), this might be stale
            // because that value gets updated in another thread.
            mIsOccluded = isOccluded;
        }
    }
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::Dismiss()
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->Dismiss();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::OnDreamingStarted() {
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnDreamingStarted();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::OnDreamingStopped() {
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnDreamingStopped();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::OnScreenTurnedOff(
    /* [in] */ Int32 reason)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnScreenTurnedOff(reason);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnScreenTurnedOn(callback);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->SetKeyguardEnabled(enabled);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::IsDismissable(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsDismissable(result);
}

ECode KeyguardService::KeyguardServiceBinder::OnSystemReady()
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnSystemReady();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::DoKeyguardTimeout(
    /* [in] */ IBundle* options)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->DoKeyguardTimeout(options);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->SetCurrentUser(userId);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::ShowAssistant()
{
    return mHost->CheckPermission();
}

ECode KeyguardService::KeyguardServiceBinder::Dispatch(
    /* [in] */ IMotionEvent* event)
{
    return mHost->CheckPermission();
}

ECode KeyguardService::KeyguardServiceBinder::LaunchCamera()
{
    return mHost->CheckPermission();
}

ECode KeyguardService::KeyguardServiceBinder::OnBootCompleted()
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnBootCompleted();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->StartKeyguardExitAnimation(startTime, fadeoutDuration);
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::OnActivityDrawn()
{
    mHost->CheckPermission();
    mHost->mKeyguardViewMediator->OnActivityDrawn();
    return NOERROR;
}

ECode KeyguardService::KeyguardServiceBinder::ToString(
    /* [out] */ String* str)
{
    return NOERROR;
}

//--------------------------------------------------------------------------------------
// KeyguardService
//--------------------------------------------------------------------------------------

const String KeyguardService::TAG("KeyguardService");
const String KeyguardService::PERMISSION(Manifest::permission::CONTROL_KEYGUARD);

CAR_INTERFACE_IMPL(KeyguardService, Elastos::Droid::App::Service, IKeyguardService)

KeyguardService::KeyguardService()
{
    mBinder = new KeyguardServiceBinder(this);
}

ECode KeyguardService::constructor()
{
    return NOERROR;
}

ECode KeyguardService::OnCreate()
{
    AutoPtr<IApplication> application;
    Service::GetApplication((IApplication**)&application);
    AutoPtr<ISystemUIApplication> sa = ISystemUIApplication::Probe(application);
    sa->StartServicesIfNeeded();
    AutoPtr<IInterface> obj;
    sa->GetComponent(EIID_IKeyguardViewMediator, (IInterface**)&obj);

    mKeyguardViewMediator = (KeyguardViewMediator*)(IKeyguardViewMediator::Probe(obj));
    return NOERROR;
}

ECode KeyguardService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** b)
{
    VALIDATE_NOT_NULL(b)
    *b = mBinder;
    REFCOUNT_ADD(*b)
    return NOERROR;
}

ECode KeyguardService::CheckPermission()
{
    AutoPtr<IContext> ctx;
    ContextWrapper::GetBaseContext((IContext**)&ctx);
    Int32 result;
    ctx->CheckCallingOrSelfPermission(PERMISSION, &result);
    if (result != IPackageManager::PERMISSION_GRANTED) {
        Logger::W(TAG, "Caller needs permission '%s' to call ", PERMISSION.string()/*, Debug.getCaller()*/);
        AutoPtr<IBinderHelper> bh;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&bh);
        Int32 pid;
        bh->GetCallingPid(&pid);
        Logger::E(TAG, "Access denied to process: %d, must have permission %s", pid, PERMISSION.string());
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos