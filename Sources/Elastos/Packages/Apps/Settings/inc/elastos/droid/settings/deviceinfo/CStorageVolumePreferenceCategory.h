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

#ifndef __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEVOLUMEPREFERENCECATEGORY_H__
#define __ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEVOLUMEPREFERENCECATEGORY_H__

#include "_Elastos_Droid_Settings_Deviceinfo_CStorageVolumePreferenceCategory.h"
#include "elastos/droid/preference/PreferenceCategory.h"
#include "elastos/droid/settings/deviceinfo/StorageMeasurement.h"

using Elastos::Droid::Settings::Deviceinfo::IMeasurementDetails;
using Elastos::Droid::Settings::Deviceinfo::IStorageMeasurementReceiver;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::PreferenceCategory;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

CarClass(CStorageVolumePreferenceCategory)
    , public PreferenceCategory
{
public:
    class PreferenceHeader
        : public Preference
    {
    public:
        TO_STRING_IMPL("CStorageVolumePreferenceCategory::PreferenceHeader")

        PreferenceHeader();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 titleRes);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ ICharSequence* title);

        //@Override
        CARAPI IsEnabled(
            /* [out] */ Boolean* result);
    };

private:
    class InitUpdateHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CStorageVolumePreferenceCategory::InitUpdateHandler")

        InitUpdateHandler(
            /* [in] */ CStorageVolumePreferenceCategory* host);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CStorageVolumePreferenceCategory* mHost;
    };

    class InitMeasurementReceiver
        : public Object
        , public IStorageMeasurementReceiver
    {
    public:
        TO_STRING_IMPL("CStorageVolumePreferenceCategory::InitMeasurementReceiver")

        CAR_INTERFACE_DECL()

        InitMeasurementReceiver(
            /* [in] */ CStorageVolumePreferenceCategory* host);

        //@Override
        CARAPI UpdateApproximate(
            /* [in] */ IStorageMeasurement* meas,
            /* [in] */ Int64 totalSize,
            /* [in] */ Int64 availSize);

        //@Override
        CARAPI UpdateDetails(
            /* [in] */ IStorageMeasurement* meas,
            /* [in] */ IMeasurementDetails* details);

    private:
        CStorageVolumePreferenceCategory* mHost;
    };

public:
    TO_STRING_IMPL("CStorageVolumePreferenceCategory")

    CAR_OBJECT_DECL()

    /**
     * Build category to summarize internal storage, including any emulated
     * {@link StorageVolume}.
     */
    static CARAPI_(AutoPtr<IPreferenceCategory>)/*StorageVolumePreferenceCategory*/ BuildForInternal(
        /* [in] */ IContext* context);

    /**
     * Build category to summarize specific physical {@link StorageVolume}.
     */
    static CARAPI_(AutoPtr<IPreferenceCategory>)/*StorageVolumePreferenceCategory*/ BuildForPhysical(
        /* [in] */ IContext* context,
        /* [in] */ IStorageVolume* volume);

    CStorageVolumePreferenceCategory();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IStorageVolume* volume);

    CARAPI Init();

    CARAPI_(AutoPtr<IStorageVolume>) GetStorageVolume();

    CARAPI UpdateApproximate(
        /* [in] */ Int64 totalSize,
        /* [in] */ Int64 availSize);

    CARAPI UpdateDetails(
        /* [in] */ IMeasurementDetails* _details);

    CARAPI OnResume();

    CARAPI OnStorageStateChanged();

    CARAPI OnUsbStateChanged(
        /* [in] */ Boolean isUsbConnected,
        /* [in] */ const String& usbFunction);

    CARAPI OnMediaScannerFinished();

    CARAPI OnCacheCleared();

    CARAPI OnPause();

    CARAPI_(Boolean) MountToggleClicked(
        /* [in] */ IPreference* preference);

    CARAPI_(AutoPtr<IIntent>) IntentForClick(
        /* [in] */ IPreference* pref);

private:
    CARAPI_(void) UpdatePreferencesFromState();

    CARAPI_(AutoPtr<IPreference>) /*CStorageItemPreference*/ BuildItem(
        /* [in] */ Int32 titleRes,
        /* [in] */ Int32 colorRes);

    static CARAPI_(Int64) TotalValues(
        /* [in] */ IHashMap* map,// HashMap<String, Long> ,
        /* [in] */ ArrayOf<String>* keys);

    CARAPI_(void) UpdatePreference(
        /* [in] */ IPreference* pref, //StorageItemPreference
        /* [in] */ Int64 size);

    CARAPI_(void) Measure();

    CARAPI_(String) FormatSize(
        /* [in] */ Int64 size);

    /**
     * Return list of other users, excluding the current user.
     */
    CARAPI_(AutoPtr<IList>) /*List<UserInfo>*/ GetUsersExcluding(
        /* [in] */ IUserInfo* excluding);

public:
    static const String KEY_CACHE;

private:
    static const Int32 ORDER_USAGE_BAR;
    static const Int32 ORDER_STORAGE_LOW;

    /** Physical volume being measured, or {@code NULL} for internal. */
    AutoPtr<IStorageVolume> mVolume;
    AutoPtr<StorageMeasurement> mMeasure;

    AutoPtr<IResources> mResources;
    AutoPtr<IStorageManager> mStorageManager;
    AutoPtr<IUserManager> mUserManager;

    AutoPtr<IPreference> /*CUsageBarPreference*/ mUsageBarPreference;
    AutoPtr<IPreference> mMountTogglePreference;
    AutoPtr<IPreference> mFormatPreference;
    AutoPtr<IPreference> mStorageLow;

    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemTotal;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemAvailable;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemApps;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemDcim;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemMusic;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemDownloads;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemCache;
    AutoPtr<IPreference> /*CStorageItemPreference*/ mItemMisc;
    AutoPtr<IList> mItemUsers; //List<StorageItemPreference>

    Boolean mUsbConnected;
    String mUsbFunction;

    Int64 mTotalSize;

    static const Int32 MSG_UI_UPDATE_APPROXIMATE = 1;
    static const Int32 MSG_UI_UPDATE_DETAILS = 2;

    AutoPtr<InitUpdateHandler> mUpdateHandler;

    AutoPtr<IStorageMeasurementReceiver> mReceiver;
};

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DEVICEINFO_CSTORAGEVOLUMEPREFERENCECATEGORY_H__