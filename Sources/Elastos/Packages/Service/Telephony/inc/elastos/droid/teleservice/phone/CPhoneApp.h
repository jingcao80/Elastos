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

#ifndef  __ELASTOS_DROID_PHONE_CPHONEAPP_H__
#define  __ELASTOS_DROID_PHONE_CPHONEAPP_H__

#include "_Elastos_Droid_TeleService_Phone_CPhoneApp.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/teleservice/services/telephony/TelephonyGlobals.h"
#include "elastos/droid/app/Application.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::App::Application;
using Elastos::Droid::TeleService::Services::Telephony::TelephonyGlobals;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Top-level Application class for the Phone app.
 */
CarClass(CPhoneApp)
    , public Application
{
public:
    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

public:
    AutoPtr<PhoneGlobals> mPhoneGlobals;
    AutoPtr<TelephonyGlobals> mTelephonyGlobals;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CPHONEAPP_H__