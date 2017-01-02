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

#include "elastos/droid/ext/frameworkdef.h"
#include "CPrivacySettingsManagerService.h"
#include <elastos/utility/logging/Slogger.h>
// #include <stdio.h>
// #include "utils/log.h"
//#include "privacy/CPrivacyPersistenceAdapter.h"
using namespace Elastos::Droid::Privacy;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

Boolean CPrivacySettingsManagerService::sSendNotifications = TRUE;
const String CPrivacySettingsManagerService::TAG("CPrivacySettingsManagerService");

CPrivacySettingsManagerService::CPrivacySettingsManagerService()
    : mEnabled(FALSE)
    , mNotificationEnabled(FALSE)
    , mBootCompleted(FALSE)
{}

ECode CPrivacySettingsManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    CPrivacyPersistenceAdapter::New(context, (IPrivacyPersistenceAdapter**)&mPersistence);
    String value;
    mPersistence->GetValue(IPrivacyPersistenceAdapter::SETTING_ENABLED, &value);
    if (!value.Equals(IPrivacyPersistenceAdapter::VALUE_TRUE)) {
        mEnabled = TRUE;
    }
    else {
        mEnabled = FALSE;
    }

    mPersistence->GetValue(IPrivacyPersistenceAdapter::SETTING_NOTIFICATIONS_ENABLED, &value);
    if (!value.Equals(IPrivacyPersistenceAdapter::VALUE_TRUE)) {
        mNotificationEnabled = TRUE;
    }
    else {
        mNotificationEnabled = FALSE;
    }
    mBootCompleted = TRUE;
    return NOERROR;
}

ECode CPrivacySettingsManagerService::GetSettings(
    /* [in] */ const String& packageName,
    /* [out] */ IPrivacySettings** privacySettings)
{
    VALIDATE_NOT_NULL(privacySettings);
    return mPersistence->GetSettings(packageName, privacySettings);
}

ECode CPrivacySettingsManagerService::SaveSettings(
    /* [in] */ IPrivacySettings* settings,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPersistence->SaveSettings(settings, result);
}

ECode CPrivacySettingsManagerService::DeleteSettings(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPersistence->DeleteSettings(packageName, result);
}

ECode CPrivacySettingsManagerService::GetVersion(
    /* [out] */ Double* version)
{
    VALIDATE_NOT_NULL(version);
    // *version = CPrivacyPersistenceAdapter::sDbVersion;
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacySettingsManagerService::Notification(
    /* [in] */ const String& packageName,
    ///* [in] */ Int32 uid,
    /* [in] */ Byte accessMode,
    /* [in] */ const String& dataType,
    /* [in] */ const String& output)
{
    if (mBootCompleted && mNotificationEnabled && sSendNotifications) {
        AutoPtr<IIntent> intent;
        CIntent::New((IIntent**)&intent);
        intent->SetAction(IPrivacySettingsManager::ACTION_PRIVACY_NOTIFICATION);
        intent->PutExtra(String("packageName"), packageName);
        intent->PutExtra(String("uid"), IPrivacyPersistenceAdapter::DUMMY_UID);
        intent->PutByteExtra(String("accessMode"), accessMode);
        intent->PutExtra(String("dataType"), dataType);
        intent->PutExtra(String("output"), output);
        mContext->SendBroadcast(intent);
        return NOERROR;
    }
    else {
        // Slogger::D(TAG, "INFO:No broadcast are sent");
        return NOERROR;
    }
}

ECode CPrivacySettingsManagerService::RegisterObservers()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacySettingsManagerService::AddObserver(
    /* [in] */ const String& packageName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacySettingsManagerService::PurgeSettings(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPersistence->PurgeSettings(result);
}

ECode CPrivacySettingsManagerService::SetEnabled(
        /* [in]  */ Boolean   isEnable,
        /* [out] */ Boolean*  rst)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacySettingsManagerService::SetNotificationsEnabled(
        /* [in] */  Boolean   isEnable,
        /* [out] */ Boolean*  rst)
{
    VALIDATE_NOT_NULL(rst);
    *rst = FALSE;
    // checkCallerCanWriteOrThrow();
    String value = isEnable ? IPrivacyPersistenceAdapter::VALUE_TRUE : IPrivacyPersistenceAdapter::VALUE_FALSE;
    Boolean result = FALSE;
    mPersistence->SetValue(IPrivacyPersistenceAdapter::SETTING_NOTIFICATIONS_ENABLED, value, &result);
    if (result) {
        if (isEnable) {
            mNotificationEnabled = TRUE;
        }
        else {
            mNotificationEnabled = FALSE;
        }
        mBootCompleted = TRUE;
        *rst = TRUE;
    }
    else {
        *rst = FALSE;
    }

    return NOERROR;
}

ECode CPrivacySettingsManagerService::SetBootCompleted()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
