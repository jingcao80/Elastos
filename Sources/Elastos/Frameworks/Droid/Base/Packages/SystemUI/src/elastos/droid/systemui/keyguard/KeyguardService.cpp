#include "elastos/droid/systemui/keyguard/KeyguardService.h"
#include "elastos/droid/systemui/keyguard/CIKeyguardService.h"
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
// CIKeyguardService
//--------------------------------------------------------------------------------------
CAR_OBJECT_IMPL(CIKeyguardService)
CAR_INTERFACE_IMPL_2(CIKeyguardService, Object, IIKeyguardService, IBinder);
CIKeyguardService::CIKeyguardService()
    : mIsOccluded(FALSE)
{}

ECode CIKeyguardService::constructor(
    /* [in] */ IKeyguardService* host)
{
    mHost = (KeyguardService*)host;
    return NOERROR;
}

ECode CIKeyguardService::IsShowing(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsShowing(result);
}

ECode CIKeyguardService::IsSecure(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsSecure(result);
}

ECode CIKeyguardService::IsShowingAndNotOccluded(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsShowingAndNotOccluded(result);
}

ECode CIKeyguardService::IsInputRestricted(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsInputRestricted(result);
}

ECode CIKeyguardService::VerifyUnlock(
    /* [in] */ IIKeyguardExitCallback* callback)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->VerifyUnlock(callback);
}

ECode CIKeyguardService::KeyguardDone(
    /* [in] */ Boolean authenticated,
    /* [in] */ Boolean wakeup)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->KeyguardDone(authenticated, wakeup);
}

ECode CIKeyguardService::SetOccluded(
    /* [in] */ Boolean isOccluded,
    /* [out] */ Int32* result)
{
    FAIL_RETURN(mHost->CheckPermission())
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

ECode CIKeyguardService::Dismiss()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->Dismiss();
}

ECode CIKeyguardService::OnDreamingStarted()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnDreamingStarted();
}

ECode CIKeyguardService::OnDreamingStopped()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnDreamingStopped();
}

ECode CIKeyguardService::OnScreenTurnedOff(
    /* [in] */ Int32 reason)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnScreenTurnedOff(reason);
}

ECode CIKeyguardService::OnScreenTurnedOn(
    /* [in] */ IIKeyguardShowCallback* callback)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnScreenTurnedOn(callback);
}

ECode CIKeyguardService::SetKeyguardEnabled(
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->SetKeyguardEnabled(enabled);
}

ECode CIKeyguardService::IsDismissable(
    /* [out] */ Boolean* result)
{
    return mHost->mKeyguardViewMediator->IsDismissable(result);
}

ECode CIKeyguardService::OnSystemReady()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnSystemReady();
}

ECode CIKeyguardService::DoKeyguardTimeout(
    /* [in] */ IBundle* options)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->DoKeyguardTimeout(options);
}

ECode CIKeyguardService::SetCurrentUser(
    /* [in] */ Int32 userId)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->SetCurrentUser(userId);
}

ECode CIKeyguardService::ShowAssistant()
{
    return mHost->CheckPermission();
}

ECode CIKeyguardService::Dispatch(
    /* [in] */ IMotionEvent* event)
{
    return mHost->CheckPermission();
}

ECode CIKeyguardService::LaunchCamera()
{
    return mHost->CheckPermission();
}

ECode CIKeyguardService::OnBootCompleted()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnBootCompleted();
}

ECode CIKeyguardService::StartKeyguardExitAnimation(
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 fadeoutDuration)
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->StartKeyguardExitAnimation(startTime, fadeoutDuration);
}

ECode CIKeyguardService::OnActivityDrawn()
{
    FAIL_RETURN(mHost->CheckPermission())
    return mHost->mKeyguardViewMediator->OnActivityDrawn();
}

ECode CIKeyguardService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//--------------------------------------------------------------------------------------
// KeyguardService
//--------------------------------------------------------------------------------------

const String KeyguardService::TAG("KeyguardService");
const String KeyguardService::PERMISSION(Manifest::permission::CONTROL_KEYGUARD);

CAR_INTERFACE_IMPL(KeyguardService, Elastos::Droid::App::Service, IKeyguardService)

KeyguardService::KeyguardService()
{
}

ECode KeyguardService::constructor()
{
    CIKeyguardService::New(this, (IBinder**)&mBinder);
    return Service::constructor();
}

ECode KeyguardService::OnCreate()
{
    AutoPtr<IApplication> application;
    Service::GetApplication((IApplication**)&application);
    AutoPtr<ISystemUIApplication> sa = ISystemUIApplication::Probe(application);
    sa->StartServicesIfNeeded();
    AutoPtr<IInterface> obj;
    sa->GetComponent(String("EIID_IKeyguardViewMediator"), (IInterface**)&obj);

    mKeyguardViewMediator = (CKeyguardViewMediator*)(IKeyguardViewMediator::Probe(obj));
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