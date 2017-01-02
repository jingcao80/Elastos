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

#ifndef __ELASTOS_DROID_SERVER_CPRIVACYSETTINGSMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CPRIVACYSETTINGSMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CPrivacySettingsManagerService.h"

using namespace Elastos::Droid::Privacy;
using namespace Elastos::Droid::Content;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CPrivacySettingsManagerService)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPrivacySettingsManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI GetSettings(
        /* [in] */ const String& packageName,
        /* [out] */ IPrivacySettings** privacySettings);

    CARAPI SaveSettings(
        /* [in] */ IPrivacySettings* settings,
        /* [out] */ Boolean* result);

    CARAPI DeleteSettings(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI GetVersion(
        /* [out] */ Double* version);

    CARAPI Notification(
        /* [in] */ const String& packageName,
        // /* [in] */ Int32 uid,
        /* [in] */ Byte accessMode,
        /* [in] */ const String& dataType,
        /* [in] */ const String& output);

    CARAPI RegisterObservers();

    CARAPI AddObserver(
        /* [in] */ const String& packageName);

    CARAPI PurgeSettings(
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in]  */ Boolean isEnable,
        /* [out] */ Boolean* rst);

    CARAPI SetNotificationsEnabled(
        /* [in] */  Boolean isEnable,
        /* [out] */ Boolean* rst);

    CARAPI SetBootCompleted();

private:
    static Boolean sSendNotifications;
    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr<IPrivacyPersistenceAdapter> mPersistence;
    Boolean mEnabled;
    Boolean mNotificationEnabled;
    Boolean mBootCompleted;
};

}
}
}
#endif // __ELASTOS_DROID_SERVER_CPRIVACYSETTINGSMANAGERSERVICE_H__
