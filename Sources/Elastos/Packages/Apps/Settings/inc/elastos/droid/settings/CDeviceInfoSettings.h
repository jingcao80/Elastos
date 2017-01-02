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

#ifndef __ELASTOS_DROID_SETTINGS_CDEVICEINFOSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDEVICEINFOSETTINGS_H__

#include "_Elastos_Droid_Settings_CDeviceInfoSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Widget::IToast;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDeviceInfoSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    /**
     * For search.
     */
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CDeviceInfoSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** result);

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** result);

    private:
        CARAPI_(Boolean) IsPropertyMissing(
            /* [in] */ const String& property);

        CARAPI_(Boolean) CheckIntentAction(
            /* [in] */ IContext* context,
            /* [in] */ const String& action);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDeviceInfoSettings();

    ~CDeviceInfoSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    static CARAPI_(String) GetFormattedKernelVersion();

    static CARAPI_(String) FormatKernelVersion(
        /* [in] */ const String& rawKernelVersion);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    CARAPI_(void) RemovePreferenceIfPropertyMissing(
        /* [in] */ IPreferenceGroup* preferenceGroup,
        /* [in] */ const String& preference,
        /* [in] */ const String& property);

    CARAPI_(void) RemovePreferenceIfBoolFalse(
        /* [in] */ const String& preference,
        /* [in] */ Int32 resId);

    CARAPI_(void) SetStringSummary(
        /* [in] */ const String& preference,
        /* [in] */ const String& value);

    CARAPI_(void) SetValueSummary(
        /* [in] */ const String& preference,
        /* [in] */ const String& property);

    CARAPI_(void) SendFeedback();

    /**
     * Reads a line from the specified file.
     * @param filename the file to read from
     * @return the first line, if any.
     * @throws IOException if the file couldn't be read
     */
    static CARAPI ReadLine(
        /* [in] */ const String& filename,
        /* [out] */ String* result);// throws IOException

    /**
     * Returns " (ENGINEERING)" if the msv file has a zero value, else returns "".
     * @return a string to append to the model number description.
     */
    CARAPI_(String) GetMsvSuffix();

    static CARAPI_(String) GetFeedbackReporterPackage(
        /* [in] */ IContext* context);

protected:
    static const Int32 TAPS_TO_BE_A_DEVELOPER;

    AutoPtr< ArrayOf<Int64> > mHits;
    Int32 mDevHitCountdown;
    AutoPtr<IToast> mDevHitToast;

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TAG;
    static const String FILENAME_PROC_VERSION;
    static const String FILENAME_MSV;

    static const String KEY_CONTAINER;
    static const String KEY_REGULATORY_INFO;
    static const String KEY_TERMS;
    static const String KEY_LICENSE;
    static const String KEY_COPYRIGHT;
    static const String KEY_SYSTEM_UPDATE_SETTINGS;
    static const String PROPERTY_URL_SAFETYLEGAL;
    static const String PROPERTY_SELINUX_STATUS;
    static const String KEY_KERNEL_VERSION;
    static const String KEY_BUILD_NUMBER;
    static const String KEY_DEVICE_MODEL;
    static const String KEY_SELINUX_STATUS;
    static const String KEY_BASEBAND_VERSION;
    static const String KEY_FIRMWARE_VERSION;
    static const String KEY_UPDATE_SETTING;
    static const String KEY_EQUIPMENT_ID;
    static const String PROPERTY_EQUIPMENT_ID;
    static const String KEY_DEVICE_FEEDBACK;
    static const String KEY_SAFETY_LEGAL;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDEVICEINFOSETTINGS_H__