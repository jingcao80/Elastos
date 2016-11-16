#ifndef __ELASTOS_DROID_SETTINGS_CDISPLAYSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDISPLAYSETTINGS_H__

#include "_Elastos_Droid_Settings_CDisplaySettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/CWarnedListPreference.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::CWarnedListPreference;
using Elastos::Droid::Settings::Notification::IDropDownPreferenceCallback;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CDisplaySettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class InnerListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CDisplaySettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CDisplaySettings* host);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CDisplaySettings* mHost;
    };

    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CDisplaySettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list);

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** list);
    };

    class OnCreateCallback
        : public Object
        , public IDropDownPreferenceCallback
    {
    public:
        TO_STRING_IMPL("CDisplaySettings::OnCreateCallback")

        CAR_INTERFACE_DECL()

        OnCreateCallback(
            /* [in] */ CDisplaySettings* host,
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ Int32 pos,
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

    private:
        CDisplaySettings* mHost;
        AutoPtr<IContext> mContext;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CDisplaySettings::MyRunnable")

        MyRunnable(
            /* [in] */ CDisplaySettings* host);

        CARAPI Run();

    private:
        CDisplaySettings* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDisplaySettings();

    ~CDisplaySettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI ReadFontSizePreference(
        /* [in] */ IListPreference* pref);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 dialogId,
        /* [out] */ IDialog** dialog);

    CARAPI WriteFontSizePreference(
        /* [in] */ IInterface* objValue);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* objValue,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceClick(
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

protected:
    CARAPI_(Int32) FloatToIndex(
        /* [in] */ Float val);

private:
    static CARAPI_(Boolean) AllowAllRotations(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsLiftToWakeAvailable(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsDozeAvailable(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsAutomaticBrightnessAvailable(
        /* [in] */ IResources* res);

    CARAPI_(void) UpdateTimeoutPreferenceDescription(
        /* [in] */ Int64 currentTimeout);

    CARAPI_(void) DisableUnusableTimeouts(
        /* [in] */ IListPreference* screenTimeoutPreference);

    CARAPI_(void) UpdateState();

    CARAPI_(void) UpdateScreenSaverSummary();

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TAG;

    /** If there is no setting in the provider, use this. */
    static const Int32 FALLBACK_SCREEN_TIMEOUT_VALUE;

    static const String KEY_SCREEN_TIMEOUT;
    static const String KEY_FONT_SIZE;
    static const String KEY_SCREEN_SAVER;
    static const String KEY_LIFT_TO_WAKE;
    static const String KEY_DOZE;
    static const String KEY_AUTO_BRIGHTNESS;
    static const String KEY_AUTO_ROTATE;

    static const Int32 DLG_GLOBAL_CHANGE_WARNING;

    AutoPtr<CWarnedListPreference> mFontSizePref;

    AutoPtr<IConfiguration> mCurConfig;

    AutoPtr<IListPreference> mScreenTimeoutPreference;
    AutoPtr<IPreference> mScreenSaverPreference;
    AutoPtr<ISwitchPreference> mLiftToWakePreference;
    AutoPtr<ISwitchPreference> mDozePreference;
    AutoPtr<ISwitchPreference> mAutoBrightnessPreference;
    AutoPtr<InnerListener> mListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDISPLAYSETTINGS_H__