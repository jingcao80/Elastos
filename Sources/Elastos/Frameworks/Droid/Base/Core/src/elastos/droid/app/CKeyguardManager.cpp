
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/CKeyguardManager.h"
#include "elastos/droid/app/CKeyguardManagerKeyguardLock.h"
#include "elastos/droid/app/COnKeyguardExitResult.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::View::IOnKeyguardExitResult;
using Elastos::Droid::View::CWindowManagerGlobal;

namespace Elastos {
namespace Droid {
namespace App {

//====================================================================
// CKeyguardManager::KeyguardLock
//====================================================================
CAR_INTERFACE_IMPL(CKeyguardManager::KeyguardLock, Object, IKeyguardManagerKeyguardLock)

CKeyguardManager::KeyguardLock::KeyguardLock()
{}

CKeyguardManager::KeyguardLock::~KeyguardLock()
{}

ECode CKeyguardManager::KeyguardLock::constructor(
    /* [in] */ const String& tag,
    /* [in] */ IKeyguardManager* owner)
{
    mTag = tag;
    mOwner = (CKeyguardManager*)owner;
    return NOERROR;
}

ECode CKeyguardManager::KeyguardLock::DisableKeyguard()
{
    // try {
    mOwner->mWM->DisableKeyguard(mToken, mTag);
    return NOERROR;
    // } catch (RemoteException ex) {
    // }
}

ECode CKeyguardManager::KeyguardLock::ReenableKeyguard()
{
    // try {
    //     mWM.reenableKeyguard(mToken);
    mOwner->mWM->ReenableKeyguard(mToken);
    return NOERROR;
    // } catch (RemoteException ex) {
    // }
}

//====================================================================
// CKeyguardManager
//====================================================================
CAR_INTERFACE_IMPL(CKeyguardManager, Object, IKeyguardManager)

CAR_OBJECT_IMPL(CKeyguardManager)

ECode CKeyguardManager::constructor()
{
    mWM = CWindowManagerGlobal::GetWindowManagerService();
    return NOERROR;
}

ECode CKeyguardManager::CreateConfirmDeviceCredentialIntent(
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* description,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    Boolean bval;
    IsKeyguardSecure(&bval);
    if (!bval) return NOERROR;

    AutoPtr<IIntent> intent;
    CIntent::New(IKeyguardManager::ACTION_CONFIRM_DEVICE_CREDENTIAL, (IIntent**)&intent);
    intent->PutExtra(IKeyguardManager::EXTRA_TITLE, title);
    intent->PutExtra(IKeyguardManager::EXTRA_DESCRIPTION, description);
    // For security reasons, only allow this to come from system settings.
    intent->SetPackage(String("com.android.settings"));
    *result = intent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CKeyguardManager::NewKeyguardLock(
    /* [in] */ const String& tag,
    /* [out] */ IKeyguardManagerKeyguardLock** keyguardLock)
{
    VALIDATE_NOT_NULL(keyguardLock);
    return CKeyguardManagerKeyguardLock::New(tag, this, keyguardLock);
}

ECode CKeyguardManager::IsKeyguardLocked(
    /* [out] */ Boolean* isLocked)
{
    VALIDATE_NOT_NULL(isLocked);
    // try {
    ECode ec = mWM->IsKeyguardLocked(isLocked);
    if (FAILED(ec)) *isLocked = FALSE;
    return NOERROR;
    // } catch (RemoteException ex) {
    //     return false;
    // }
}

ECode CKeyguardManager::IsKeyguardSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure);
    // try {
    ECode ec = mWM->IsKeyguardSecure(isSecure);
    if (FAILED(ec)) *isSecure = FALSE;
    return NOERROR;
    // } catch (RemoteException ex) {
    //     return false;
    // }
}

ECode CKeyguardManager::InKeyguardRestrictedInputMode(
    /* [out] */ Boolean* isInMode)
{
    VALIDATE_NOT_NULL(isInMode);
    // try {
    ECode ec = mWM->InKeyguardRestrictedInputMode(isInMode);
    if (FAILED(ec)) *isInMode = FALSE;
    return NOERROR;
    // } catch (RemoteException ex) {
    //     return false;
    // }
}

ECode CKeyguardManager::ExitKeyguardSecurely(
    /* [in] */ IKeyguardManagerOnKeyguardExitResult* cb)
{
    AutoPtr<IOnKeyguardExitResult> callback;
    COnKeyguardExitResult::New(cb, (IOnKeyguardExitResult**)&callback);
    // try {
    mWM->ExitKeyguardSecurely(callback);
    return NOERROR;
    //     mWM.exitKeyguardSecurely(new IOnKeyguardExitResult.Stub() {
    //         public void onKeyguardExitResult(boolean success) throws RemoteException {
    //             callback.onKeyguardExitResult(success);
    //         }
    //     });
    // } catch (RemoteException e) {

    // }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
