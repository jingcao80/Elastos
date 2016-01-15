
#include "wm/KeyguardDisableHandler.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/SomeArgs.h"
#include <elastos/utility/logging/Logger.h>


using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::SomeArgs;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

KeyguardDisableHandler::KeyguardTokenWatcher::KeyguardTokenWatcher(
    /* [in] */ KeyguardDisableHandler* handler)
    : TokenWatcher(handler, KeyguardDisableHandler::TAG)
    , mHost(handler)
{}

ECode KeyguardDisableHandler::KeyguardTokenWatcher::Acquired()
{
    // We fail safe and prevent disabling keyguard in the unlikely event this gets
    // called before DevicePolicyManagerService has started.
    if (mHost->mAllowDisableKeyguard == ALLOW_DISABLE_UNKNOWN) {
        AutoPtr<IInterface> service;
        mHost->mContext->GetSystemService(
                IContext::DEVICE_POLICY_SERVICE, (IInterface**)&service);
        AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(service);
        if (dpm != NULL) {
            // try {
            AutoPtr<IUserInfo> uInfo;
            ActivityManagerNative::GetDefault()->GetCurrentUser((IUserInfo**)&uInfo);
            Int32 id;
            uInfo->GetId(&id);
            Int32 pwd;
            dpm->GetPasswordQuality(NULL, id, &pwd);
            mHost->mAllowDisableKeyguard = pwd == IDevicePolicyManager::PASSWORD_QUALITY_UNSPECIFIED ?
                            ALLOW_DISABLE_YES : ALLOW_DISABLE_NO;
            // } catch (RemoteException re) {
            //     // Nothing much we can do
            // }
        }
    }
    if (mHost->mAllowDisableKeyguard == ALLOW_DISABLE_YES) {
        mHost->mPolicy->EnableKeyguard(FALSE);
    } else {
        Logger::V(TAG, "Not disabling keyguard since device policy is enforced");
    }
    return NOERROR;
}

ECode KeyguardDisableHandler::KeyguardTokenWatcher::Released()
{
    mHost->mPolicy->EnableKeyguard(true);
    return NOERROR;
}

const String KeyguardDisableHandler::TAG("KeyguardDisableHandler");
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_YES = 1;
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_NO = 0;
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_UNKNOWN = -1;

const Int32 KeyguardDisableHandler::KEYGUARD_DISABLE;
const Int32 KeyguardDisableHandler::KEYGUARD_REENABLE;
const Int32 KeyguardDisableHandler::KEYGUARD_POLICY_CHANGED;

KeyguardDisableHandler::KeyguardDisableHandler(
    /* [in] */ IContext* context,
    /* [in] */ IWindowManagerPolicy* policy)
    : mContext(context)
    , mPolicy(policy)
    , mAllowDisableKeyguard(ALLOW_DISABLE_UNKNOWN)
{
}

ECode KeyguardDisableHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (mKeyguardTokenWatcher == NULL) {
        mKeyguardTokenWatcher = new KeyguardTokenWatcher(this);
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case KEYGUARD_DISABLE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            SomeArgs* args = (SomeArgs*)obj.Get();

            IBinder* token = IBinder::Probe(args->mArg1);
            ICharSequence* seq= ICharSequence::Probe(args->mArg2);
            String s;
            seq->ToString(&s);
            mKeyguardTokenWatcher->AcquireToken(token, s);
            args->Recycle();
            break;
        }

        case KEYGUARD_REENABLE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IBinder* token = IBinder::Probe(obj);
            mKeyguardTokenWatcher->ReleaseToken(token);
            break;
        }
        case KEYGUARD_POLICY_CHANGED:
            mPolicy->EnableKeyguard(TRUE);
            // lazily evaluate this next time we're asked to disable keyguard
            mAllowDisableKeyguard = ALLOW_DISABLE_UNKNOWN;
            break;
    }

    return NOERROR;
}


// PInterface KeyguardDisableHandler::KeyguardTokenWatcher::Probe(
//     /* [in] */ REIID riid)
// {
//     if (riid == EIID_IInterface) {
//         return (IInterface*)(ITokenWatcher*)this;
//     }
//     else if (riid == EIID_ITokenWatcher) {
//         return (Elastos::Droid::Os::ITokenWatcher*)this;
//     }
//     return NULL;
// }

// UInt32 KeyguardDisableHandler::KeyguardTokenWatcher::AddRef()
// {
//     return ElRefBase::AddRef();
// }

// UInt32 KeyguardDisableHandler::KeyguardTokenWatcher::Release()
// {
//     return ElRefBase::Release();
// }

// ECode KeyguardDisableHandler::KeyguardTokenWatcher::GetInterfaceID(
//     /* [in] */ IInterface *pObject,
//     /* [out] */ InterfaceID *pIID)
// {
//     return E_NOT_IMPLEMENTED;
// }


} // Wm
} // Server
} // Droid
} // Elastos
