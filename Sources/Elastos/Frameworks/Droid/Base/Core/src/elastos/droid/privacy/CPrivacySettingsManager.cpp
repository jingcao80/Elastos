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

#include "elastos/droid/privacy/CPrivacySettingsManager.h"

namespace Elastos {
namespace Droid {
namespace Privacy {

const String CPrivacySettingsManager::TAG("PrivacySettingsManager");
const String CPrivacySettingsManager::ACTION_PRIVACY_NOTIFICATION("com.privacy.pdroid.PRIVACY_NOTIFICATION");
const String CPrivacySettingsManager::ACTION_PRIVACY_NOTIFICATION_ADDON("com.privacy.pdroid.PRIVACY_NOTIFICATION_ADDON");
const Double CPrivacySettingsManager::API_VERSION = 1.51;
const Double CPrivacySettingsManager::MOD_VERSION = 1.0;
const String CPrivacySettingsManager::MOD_DETAILS("OpenPDroid 1.0 by FFU5y, Mateor, wbedard; forked from PDroid 2.0\n"
        "PDroid 2.0 by CollegeDev; forked from PDroid\n"
        "PDroid by Syvat's\n"
        "Additional contributions by Pastime1971");

ECode CPrivacySettingsManager::GetSettings(
    /* [in] */ const String& packageName,
    /* [out] */ Elastos::Droid::Privacy::IPrivacySettings** privactySettings)
{
//    try {
    *privactySettings = NULL;
    if (mService != NULL) {
        return mService->GetSettings(packageName, privactySettings);
    } else {
//        Log.e(TAG, "getSettings - PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        e.printStackTrace();
//        return null;
//    }
}

ECode CPrivacySettingsManager::GetSettings(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out] */ Elastos::Droid::Privacy::IPrivacySettings** privactySettings)
{
    // TODO: Add your code here
    return GetSettings(packageName, privactySettings);
}

ECode CPrivacySettingsManager::SaveSettings(
    /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* settings,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
//    try {
//            Log.d(TAG, "saveSettings - " + settings);
    *result = FALSE;
    if (mService != NULL) {
        return mService->SaveSettings(settings, result);
    } else {
//        Log.e(TAG, "saveSettings - PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in saveSettings: ", e);
//        return false;
//    }
}

ECode CPrivacySettingsManager::DeleteSettings(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
//    try {
    *result = FALSE;
    if (mService != NULL) {
        return mService->DeleteSettings(packageName, result);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:deleteSettings: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in deleteSettings: ", e);
//        return false;
//    }
}

ECode CPrivacySettingsManager::DeleteSettings(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
    return DeleteSettings(packageName, result);
}

ECode CPrivacySettingsManager::IsServiceAvailable(
    /* [out] */ Boolean* isServiceAvailable)
{
    if (mService != NULL){
        *isServiceAvailable = TRUE;
    }else {
        *isServiceAvailable = FALSE;
    }
    return NOERROR;
}

ECode CPrivacySettingsManager::Notification(
    /* [in] */ const String& packageName,
    /* [in] */ Byte accessMode,
    /* [in] */ const String& dataType,
    /* [in] */ const String& output)
{
    // TODO: Add your code here
//    try {
    if (mService != NULL) {
        return mService->Notification(packageName, accessMode, dataType, output);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:notification: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in notification: ", e);
//    }
}

ECode CPrivacySettingsManager::Notification(
    /* [in] */ const String& packageName,
    /* [in] */ Byte accessMode,
    /* [in] */ const String& dataType,
    /* [in] */ const String& output,
    /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* set)
{
    // TODO: Add your code here
    return Notification(packageName, accessMode, dataType, output);
}

ECode CPrivacySettingsManager::Notification(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Byte accessMode,
    /* [in] */ const String& dataType,
    /* [in] */ const String& output,
    /* [in] */ Elastos::Droid::Privacy::IPrivacySettings* set)
{
    // TODO: Add your code here
    return Notification(packageName, accessMode, dataType, output);
}

ECode CPrivacySettingsManager::RegisterObservers()
{
    // TODO: Add your code here
//    try {
    if (mService != NULL) {
        return mService->RegisterObservers();
    } else {
//        Log.e(TAG, "PrivacySettingsManager:registerObservers: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in registerObservers: ", e);
//    }
}

ECode CPrivacySettingsManager::AddObserver(
    /* [in] */ const String& packageName)
{
    // TODO: Add your code here
    //    try {
    if (mService != NULL) {
        return  mService->AddObserver(packageName);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:registerObservers: PrivacySettingsManagerService is null");
        return NOERROR;
    }
    //    } catch (RemoteException e) {
    //        Log.e(TAG, "RemoteException in registerObservers: ", e);
    //    }
}

ECode CPrivacySettingsManager::PurgeSettings(
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
    *result = FALSE;
//    try {
    if (mService != NULL) {
        return mService->PurgeSettings(result);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:purgeSettings: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in purgeSettings: ", e);
//    }
}

ECode CPrivacySettingsManager::GetVersion(
    /* [out] */ Double* result)
{
    // TODO: Add your code here
    *result = API_VERSION;
    return NOERROR;
}

ECode CPrivacySettingsManager::GetApiVersion(
    /* [out] */ Double* apiVersion)
{
    // TODO: Add your code here
    *apiVersion = API_VERSION;
    return NOERROR;
}

ECode CPrivacySettingsManager::GetModVersion(
    /* [out] */ Double* modVersion)
{
    // TODO: Add your code here
    *modVersion = MOD_VERSION;
    return NOERROR;
}

ECode CPrivacySettingsManager::GetModDetails(
    /* [out] */ String* modDetails)
{
    // TODO: Add your code here
    *modDetails = MOD_DETAILS;
    return NOERROR;
}

ECode CPrivacySettingsManager::SetEnabled(
    /* [in] */ Boolean isEnable,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
//    try {
    *result = FALSE;
    if (mService != NULL) {
        return mService->SetEnabled(isEnable, result);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:setEnabled: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in setEnabled: ", e);
//    }
//    return false;

}

ECode CPrivacySettingsManager::SetNotificationsEnabled(
    /* [in] */ Boolean isEnable,
    /* [out] */ Boolean* result)
{
    // TODO: Add your code here
//    try {
    *result = FALSE;
    if (mService != NULL) {
        return mService->SetNotificationsEnabled(isEnable, result);
    } else {
//        Log.e(TAG, "PrivacySettingsManager:setNotificationsEnabled: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in setNotificationsEnabled: ", e);
//    }
//    return false;

}

ECode CPrivacySettingsManager::SetBootCompleted()
{
    // TODO: Add your code here
//    try {
    if (mService != NULL) {
        return mService->SetBootCompleted();
    } else {
//        Log.e(TAG, "PrivacySettingsManager:setBootCompleted: PrivacySettingsManagerService is null");
        return NOERROR;
    }
//    } catch (RemoteException e) {
//        Log.e(TAG, "RemoteException in setBootCompleted: ", e);
//    }
}

ECode CPrivacySettingsManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIPrivacySettingsManager* service)
{
    // TODO: Add your code here
    mService = service;
    return NOERROR;
}

}
}
}

