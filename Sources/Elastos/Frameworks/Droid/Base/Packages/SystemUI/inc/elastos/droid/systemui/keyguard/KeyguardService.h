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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/systemui/keyguard/CKeyguardViewMediator.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class CIKeyguardService;

class KeyguardService
    : public Elastos::Droid::App::Service
    , public IKeyguardService
{
public:
    CAR_INTERFACE_DECL()

    KeyguardService();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

    using Service::CheckPermission;

    CARAPI CheckPermission();

public:
    const static String TAG;
    const static String PERMISSION;

private:
    AutoPtr<CKeyguardViewMediator> mKeyguardViewMediator;

    AutoPtr<IBinder> mBinder;
    friend class CIKeyguardService;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_KEYGUARDSERVICE_H__