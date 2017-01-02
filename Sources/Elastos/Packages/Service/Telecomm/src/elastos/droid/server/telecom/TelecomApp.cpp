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

#include "elastos/droid/server/telecom/TelecomApp.h"
#include "elastos/droid/server/telecom/CTelecomServiceImpl.h"
#include "elastos/droid/server/telecom/BlacklistCallNotifier.h"
#include "elastos/droid/server/telecom/BluetoothPhoneService.h"
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/droid/os/UserHandle.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::UserHandle;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

ECode TelecomApp::OnCreate()
{
    Application::OnCreate();
    if (UserHandle::GetMyUserId() == IUserHandle::USER_OWNER) {
        // Note: This style of initialization mimics what will be performed once Telecom is
        // moved
        // to run in the system service. The emphasis is on ensuring that initialization of all
        // telecom classes happens in one place without relying on Singleton initialization.
        mMissedCallNotifier = new MissedCallNotifier();
        mMissedCallNotifier->constructor(this);
        mBlacklistCallNotifier = new BlacklistCallNotifier();
        mBlacklistCallNotifier->constructor(this);
        mPhoneAccountRegistrar = new PhoneAccountRegistrar();
        mPhoneAccountRegistrar->constructor(this);
        mCallsManager = new CallsManager();
        mCallsManager->constructor(this, mMissedCallNotifier,
                mBlacklistCallNotifier, mPhoneAccountRegistrar);
        CallsManager::Initialize(mCallsManager);

        CTelecomServiceImpl::New(TO_IINTERFACE(mMissedCallNotifier), TO_IINTERFACE(mPhoneAccountRegistrar),
                TO_IINTERFACE(mCallsManager), this, (IITelecomService**)&mTelecomService);
        ServiceManager::AddService(IContext::TELECOM_SERVICE, TO_IINTERFACE(mTelecomService));
        // Start the BluetoothPhoneService
        BluetoothPhoneService::Start(this);
    }
    return NOERROR;
}

ECode TelecomApp::GetMissedCallNotifier(
    /* [out] */ MissedCallNotifier** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMissedCallNotifier;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TelecomApp::GetPhoneAccountRegistrar(
    /* [out] */ PhoneAccountRegistrar** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneAccountRegistrar;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
