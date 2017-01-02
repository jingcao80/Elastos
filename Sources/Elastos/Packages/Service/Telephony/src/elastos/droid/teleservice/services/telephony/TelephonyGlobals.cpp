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

#include "elastos/droid/teleservice/services/telephony/TelephonyGlobals.h"
#include "elastos/droid/teleservice/services/telephony/TelecomAccountRegistry.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.Droid.Internal.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

AutoPtr<TelephonyGlobals> TelephonyGlobals::sInstance;

Object TelephonyGlobals::slock;

TelephonyGlobals::TelephonyGlobals(
    /* [in] */ IContext* context)
{
    context->GetApplicationContext((IContext**)&mContext);
}

AutoPtr<TelephonyGlobals> TelephonyGlobals::GetInstance(
    /* [in] */ IContext* context)
{
    {
        AutoLock syncLock(slock);

        if (sInstance == NULL) {
            sInstance = new TelephonyGlobals(context);
        }
        return sInstance;
    }
}

ECode TelephonyGlobals::OnCreate()
{
    // // TODO: Make this work with Multi-SIM devices
    AutoPtr<IPhone> phone;
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&phone);
    if (phone != NULL) {
        mTtyManager = new TtyManager(mContext, phone);
    }

    AutoPtr<TelecomAccountRegistry> registry = TelecomAccountRegistry::GetInstance(mContext);
    return registry->SetupOnBoot();
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos
