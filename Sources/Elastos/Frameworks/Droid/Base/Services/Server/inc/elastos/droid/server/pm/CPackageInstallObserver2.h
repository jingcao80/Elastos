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

#ifndef __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLOBSERVER2_H__
#define __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLOBSERVER2_H__

#include "_Elastos_Droid_Server_Pm_CPackageInstallObserver2.h"
#include "elastos/droid/server/pm/CPackageManagerService.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver2;
using Elastos::Droid::Content::Pm::IIPackageMoveObserver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

CarClass(CPackageInstallObserver2)
    , public Object
    , public IIPackageInstallObserver2
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IIPackageManager* host,
        /* [in] */ IIPackageMoveObserver* observer,
        /* [in] */ const String& packageName);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageInstalled(
        /* [in] */ const String& basePackageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg,
        /* [in] */ IBundle* extras);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CPackageManagerService* mHost;
    AutoPtr<IIPackageMoveObserver> mObserver;
    String mPackageName;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_CPACKAGEINSTALLOBSERVER2_H__
