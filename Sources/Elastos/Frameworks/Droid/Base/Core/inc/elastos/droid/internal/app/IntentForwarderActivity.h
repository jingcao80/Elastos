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

#ifndef __ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class IntentForwarderActivity
    : public Activity
    , public IIntentForwarderActivity
{
public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    /**
     * Returns the managed profile for this device or null if there is no managed
     * profile.
     *
     * TODO: Remove the assumption that there is only one managed profile
     * on the device.
     */
    CARAPI_(AutoPtr<IUserHandle>) GetManagedProfile();

private:
    static const String TAG;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_INTENTFORWARDERACTIVITY_H__
