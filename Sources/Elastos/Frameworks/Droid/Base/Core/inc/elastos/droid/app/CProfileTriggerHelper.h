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

#ifndef __ELASTOS_DROID_APP_CPROFILE_TRIGGER_HELPER_H__
#define __ELASTOS_DROID_APP_CPROFILE_TRIGGER_HELPER_H__

#include "_Elastos_Droid_App_CProfileTriggerHelper.h"
#include "elastos/droid/app/Profile.h"
#include <elastos/core/Singleton.h>


namespace Elastos {
namespace Droid {
namespace App {

CarClass(CProfileTriggerHelper)
    , public Singleton
    , public IProfileTriggerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI FromXml(
        /* [in] */ IXmlPullParser* xpp,
        /* [in] */ IContext* context,
        /* [out] */ IProfileTrigger** result);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CPROFILE_TRIGGER_HELPER_H__
