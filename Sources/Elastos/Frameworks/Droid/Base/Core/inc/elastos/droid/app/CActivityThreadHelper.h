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

#ifndef __ELASTOS_DROID_APP_CACTIVITYTHREADHELPER_H__
#define __ELASTOS_DROID_APP_CACTIVITYTHREADHELPER_H__

#include "_Elastos_Droid_App_CActivityThreadHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActivityThreadHelper)
    , public Singleton
    , public IActivityThreadHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCurrentActivityThread(
        /* [out] */ IActivityThread** thread);

    CARAPI GetCurrentPackageName(
        /* [out] */ String* name);

    CARAPI GetCurrentProcessName(
        /* [out] */ String* name);

    CARAPI GetCurrentApplication(
        /* [out] */ IApplication** app);

    CARAPI GetPackageManager(
        /* [out] */ IIPackageManager** pkgManager);

    /**
     * Return the Intent that's currently being handled by a
     * BroadcastReceiver on this thread, or null if none.
     * @hide
     */
    CARAPI GetIntentBeingBroadcast(
        /* [out] */ IIntent** intent);

    CARAPI GetSystemMain(
        /* [out] */ IActivityThread** thread);

    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CACTIVITYTHREADHELPER_H__
