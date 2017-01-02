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

#ifndef __ELASTOS_DROID_SERVER_CKILLSWITCHSERVICE_H__
#define __ELASTOS_DROID_SERVER_CKILLSWITCHSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Server_CKillSwitchService.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Os::IIKillSwitchService;
using Elastos::Droid::Internal::Os::IIKillSwitch;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * @hide
 */
CarClass(CKillSwitchService)
    , public Object
    , public IIKillSwitchService
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKillSwitchService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI HasKillSwitch(
        /* [out] */ Boolean* result);

    CARAPI SetDeviceUuid(
        /* [in] */ const String& uuid);

    CARAPI GetDeviceUuid(
        /* [out] */ String* result);

    CARAPI IsDeviceLocked(
        /* [out] */ Boolean* result);

    CARAPI SetDeviceLocked(
        /* [in] */ Boolean locked);

    CARAPI SetAccountId(
        /* [in] */ const String& value);

    CARAPI GetAccountId(
        /* [out] */ String* result);

    CARAPI_(Boolean) VerifyWritePermission();

private:
    static const String TAG;
    static Boolean DEBUG;

    AutoPtr<IContext> mContext;

    AutoPtr<IIKillSwitch> mKillSwitchImpl;
    AutoPtr<IList> mPackagesAllowedToWrite;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CKILLSWITCHSERVICE_H__
