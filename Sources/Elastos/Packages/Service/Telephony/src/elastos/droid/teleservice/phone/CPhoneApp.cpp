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

#include "elastos/droid/teleservice/phone/CPhoneApp.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserHandleHelper;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

CAR_OBJECT_IMPL(CPhoneApp)

ECode CPhoneApp::constructor()
{
    return Application::constructor();
}

ECode CPhoneApp::OnCreate()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 myUserId = 0;
    helper->GetMyUserId(&myUserId);

    if (myUserId == 0) {
        // We are running as the primary user, so should bring up the
        // global phone state.
        mPhoneGlobals = new PhoneGlobals((IContext*)this);
        mPhoneGlobals->OnCreate();

        mTelephonyGlobals = new TelephonyGlobals((IContext*)this);
        return mTelephonyGlobals->OnCreate();
    }
    return NOERROR;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos