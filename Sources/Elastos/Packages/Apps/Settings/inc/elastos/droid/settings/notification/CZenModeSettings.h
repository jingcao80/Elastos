#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CZenModeSettings.h"
#include "elastos/droid/settings/notification/SettingPref.h"
#include "elastos/droid/settings/notification/ZenModeDowntimeDaysSelection.h"
#include "elastos/droid/settings/notification/CDropDownPreference.h"
#include "elastos/droid/settings/notification/CZenModeConditionSelection.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/app/DialogFragment.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/preference/Preference.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Notification::IDropDownPreferenceCallback;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::ITimePickerDialogOnTimeSetListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::CScrollView;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CZenModeSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CNotificationSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetRawDataToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** list); //  List<SearchIndexableRaw>

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** list);
    };

    class TimePickerPreference
        : public Elastos::Droid::Preference::Preference
    {
        friend class CZenModeSettings;
    public:
        TO_STRING_IMPL("CZenModeSettings::TimePickerPreference")

        TimePickerPreference();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IFragmentManager* mgr);

        CARAPI SetCallback(
            /* [in] */ ITimePickerPreferenceCallback* callback);

        CARAPI SetSummaryFormat(
            /* [in] */ Int32 resId);

        CARAPI SetTime(
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute);

    private:
        CARAPI_(void) UpdateSummary();

    private:
        AutoPtr<IContext> mContext;

        Int32 mSummaryFormat;
        Int32 mHourOfDay;
        Int32 mMinute;
        AutoPtr<ITimePickerPreferenceCallback> mCallback;
    };

    class TimePickerFragment
        : public DialogFragment
        , public ITimePickerDialogOnTimeSetListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::TimePickerFragment")

        CAR_INTERFACE_DECL()

        TimePickerFragment();

        virtual ~TimePickerFragment();

        CARAPI constructor();

        //@Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** result);

        CARAPI OnTimeSet(
            /* [in] */ ITimePicker* view,
            /* [in] */ Int32 hourOfDay,
            /* [in] */ Int32 minute);

    public:
        AutoPtr<IPreference> /*TimePickerPreference*/ mPref;
    };

    class OnCreateZenModeDowntimeDaysSelection
        : public ZenModeDowntimeDaysSelection
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateZenModeDowntimeDaysSelection")

        OnCreateZenModeDowntimeDaysSelection();

        CARAPI constructor(
            /* [in] */ ISettingsPreferenceFragment* frag,
            /* [in] */ IContext* context,
            /* [in] */ const String& mode);

    protected:
        //@Override
        CARAPI OnChanged(
            /* [in] */ const String& mode);

    private:
        CZenModeSettings* mHost;
    };

private:
    class SettingPrefWithCallback
        : public SettingPref
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::SettingPrefWithCallback")

        SettingPrefWithCallback(
            /* [in] */ Int32 type,
            /* [in] */ const String& key,
            /* [in] */ const String& setting,
            /* [in] */ Int32 def,
            /* [in] */ ArrayOf<Int32>* values);

        virtual CARAPI SetCallback(
            /* [in] */ ISettingPrefWithCallback* callback);

        //@Override
        CARAPI Update(
            /* [in] */ IContext* context);

        //@Override
        CARAPI_(AutoPtr<IPreference>) Init(
            /* [in] */ SettingsPreferenceFragment* settings);

        virtual CARAPI_(Boolean) SetValueWithoutCallback(
            /* [in] */ IContext* context,
            /* [in] */ Int32 value);

        virtual CARAPI_(Int32) GetValue(
            /* [in] */ IContext* context);

    protected:
        //@Override
        CARAPI SetSetting(
            /* [in] */ IContext* context,
            /* [in] */ Int32 value,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<ISettingPrefWithCallback> mCallback;
        Int32 mValue;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::SettingsObserver")

        CARAPI constructor(
            /* [in] */ CZenModeSettings* host);

        virtual CARAPI Register();

        virtual CARAPI Unregister();

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IUri> ZEN_MODE_URI;
        AutoPtr<IUri> ZEN_MODE_CONFIG_ETAG_URI;
        CZenModeSettings* mHost;
    };

    class TimePickerOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::TimePickerOnPreferenceClickListener")

        CAR_INTERFACE_DECL()

        TimePickerOnPreferenceClickListener(
            /* [in] */ TimePickerPreference* host,
            /* [in] */ IFragmentManager* mgr);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        TimePickerPreference* mHost;
        AutoPtr<IFragmentManager> mMgr;
    };

    class InitSettingPrefWithCallback
        : public SettingPrefWithCallback
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::InitSettingPrefWithCallback")

        InitSettingPrefWithCallback(
            /* [in] */ Int32 type,
            /* [in] */ const String& key,
            /* [in] */ const String& setting,
            /* [in] */ Int32 def,
            /* [in] */ ArrayOf<Int32>* values);

    protected:
        virtual CARAPI GetCaption(
            /* [in] */ IResources* res,
            /* [in] */ Int32 value,
            /* [out] */ String* result);
    };

    class OnCreateSettingPrefWithCallback
        : public Object
        , public ISettingPrefWithCallback
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateSettingPrefWithCallback")

        CAR_INTERFACE_DECL()

        OnCreateSettingPrefWithCallback(
            /* [in] */ CZenModeSettings* host);

        //@Override
        CARAPI OnSettingSelected(
            /* [in] */ Int32 value);

    private:
        CZenModeSettings* mHost;
    };

    class OnCreateOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateOnPreferenceChangeListener")

        CAR_INTERFACE_DECL()

        OnCreateOnPreferenceChangeListener(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CZenModeSettings* mHost;
        Int32 mId;
    };

    class OnCreateDropDownPreferenceCallback
        : public Object
        , public IDropDownPreferenceCallback
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateDropDownPreferenceCallback")

        CAR_INTERFACE_DECL()

        OnCreateDropDownPreferenceCallback(
            /* [in] */ CZenModeSettings* host);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ Int32 pos,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CZenModeSettings* mHost;
    };

    class OnCreateOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateOnPreferenceClickListener")

        CAR_INTERFACE_DECL()

        OnCreateOnPreferenceClickListener(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CZenModeSettings* mHost;
        Int32 mId;
    };

    class OnCreateOnDismissListener
        : public Object
        , public IDialogInterfaceOnDismissListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateOnDismissListener")

        CAR_INTERFACE_DECL()

        OnCreateOnDismissListener(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

    private:
        CZenModeSettings* mHost;
        Int32 mId;
    };

    class OnCreateTimePickerPreferenceCallback
        : public Object
        , public ITimePickerPreferenceCallback
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::OnCreateTimePickerPreferenceCallback")

        CAR_INTERFACE_DECL()

        OnCreateTimePickerPreferenceCallback(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnSetTime(
            /* [in] */ Int32 hour,
            /* [in] */ Int32 minute,
            /* [out] */ Boolean* result);

    private:
        CZenModeSettings* mHost;
        Int32 mId;
    };

    class ShowConditionSelectionOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::ShowConditionSelectionOnClickListener")

        CAR_INTERFACE_DECL()

        ShowConditionSelectionOnClickListener(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 id,
            /* [in] */ CZenModeConditionSelection* zmcs,
            /* [in] */ Int32 value);

        //@Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CZenModeSettings* mHost;
        Int32 mId;
        AutoPtr<CZenModeConditionSelection> mZmcs;
        Int32 mValue;
    };

    class ShowConditionSelectionOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        TO_STRING_IMPL("CZenModeSettings::ShowConditionSelectionOnCancelListener")

        CAR_INTERFACE_DECL()

        ShowConditionSelectionOnCancelListener(
            /* [in] */ CZenModeSettings* host,
            /* [in] */ Int32 value);

        //@Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CZenModeSettings* mHost;
        Int32 mValue;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CZenModeSettings();

    ~CZenModeSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

protected:
    virtual CARAPI_(void) PutZenModeSetting(
        /* [in] */ Int32 value);

    virtual CARAPI_(void) ShowConditionSelection(
        /* [in] */ Int32 newSettingsValue);

    virtual CARAPI_(void) CancelDialog(
        /* [in] */ Int32 oldSettingsValue);

private:
    static CARAPI_(AutoPtr<ISparseArray>)/*SparseArray<String>*/ AllKeyTitles(
        /* [in] */ IContext* context);

    CARAPI_(void) UpdateDays();

    CARAPI_(void) UpdateEndSummary();

    CARAPI_(void) UpdateControls();

    CARAPI_(void) UpdateStarredEnabled();

    CARAPI_(void) RefreshAutomationSection();

    CARAPI_(String) GetEntryConditionSummary();

    CARAPI_(void) UpdateZenModeConfig();

    CARAPI_(AutoPtr<IZenModeConfig>) GetZenModeConfig();

    CARAPI_(Boolean) SetZenModeConfig(
        /* [in] */ IZenModeConfig* config);

    static CARAPI_(Boolean) InitStatic();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String KEY_ZEN_MODE;
    static const String KEY_IMPORTANT;
    static const String KEY_CALLS;
    static const String KEY_MESSAGES;
    static const String KEY_STARRED;
    static const String KEY_EVENTS;
    static const String KEY_ALARM_INFO;

    static const String KEY_DOWNTIME;
    static const String KEY_DAYS;
    static const String KEY_START_TIME;
    static const String KEY_END_TIME;

    static const String KEY_AUTOMATION;
    static const String KEY_ENTRY;
    static const String KEY_CONDITION_PROVIDERS;

    static AutoPtr<SettingPrefWithCallback> PREF_ZEN_MODE;

    static const AutoPtr<ISimpleDateFormat> DAY_FORMAT;

    static const Boolean sInitStatic;

    // Enable indexing of searchable data
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    AutoPtr<IHandler> mHandler;
    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPM;
    AutoPtr<IZenModeConfig> mConfig;
    Boolean mDisableListeners;
    AutoPtr<ISwitchPreference> mCalls;
    AutoPtr<ISwitchPreference> mMessages;
    AutoPtr<CDropDownPreference> mStarred;
    AutoPtr<ISwitchPreference> mEvents;
    AutoPtr<IPreference> mDays;
    AutoPtr<IPreference> /*TimePickerPreference*/ mStart;
    AutoPtr<IPreference> /*TimePickerPreference*/ mEnd;
    AutoPtr<IPreferenceCategory> mAutomationCategory;
    AutoPtr<IPreference> mEntry;
    AutoPtr<IPreference> mConditionProviders;
    AutoPtr<IAlertDialog> mDialog;
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CZENMODESETTINGS_H__