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

#ifndef __ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__

#include "_Elastos_Droid_Settings_CConfirmDeviceCredentialActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Launch this when you want to confirm the user is present by asking them to enter their
 * PIN/password/pattern.
 */
CarClass(CConfirmDeviceCredentialActivity)
    , public Activity
{
public:
    CAR_OBJECT_DECL()

    CConfirmDeviceCredentialActivity();

    virtual ~CConfirmDeviceCredentialActivity();

    CARAPI constructor();

    static CARAPI_(AutoPtr<IIntent>) CreateIntent(
        /* [in] */ ICharSequence* title,
        /* [in] */ ICharSequence* details);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

protected:
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

public:
    static const String TAG;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCONFIRMDEVICECREDENTIALACTIVITY_H__