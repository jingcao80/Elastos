//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <Elastos.Droid.App.h>
#include "elastos/droid/server/wm/KeyguardDisableHandler.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Internal::Os::SomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IBinder;

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
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_YES;
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_NO;
const Int32 KeyguardDisableHandler::ALLOW_DISABLE_UNKNOWN;

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
    Handler::constructor();
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
            SomeArgs* args = (SomeArgs*)(ISomeArgs*)obj.Get();

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

} // Wm
} // Server
} // Droid
} // Elastos
