
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/settings/notification/CZenModeSettings.h"
#include "elastos/droid/settings/notification/CZenModeSettingsTimePickerFragment.h"
#include "elastos/droid/settings/notification/CZenModeSettingsTimePickerPreference.h"
#include "elastos/droid/settings/notification/CZenModeSettingsDowntimeDaysSelection.h"
#include "elastos/droid/settings/notification/CConditionProviderSettings.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::CTimePickerDialog;
using Elastos::Droid::App::EIID_ITimePickerDialogOnTimeSetListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Service::Notification::CZenModeConfig;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::Objects;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CZenModeSettings::MyBaseSearchIndexProvider
//===============================================================================

CZenModeSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CZenModeSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CZenModeSettings::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<ISparseArray> keyTitles = AllKeyTitles(context);
    Int32 N;
    keyTitles->GetSize(&N);
    AutoPtr<IList> result; //List<SearchIndexableRaw>
    CArrayList::New(N, (IList**)&result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
        data->constructor(context);

        AutoPtr<IInterface> obj;
        keyTitles->ValueAt(i, (IInterface**)&obj);
        data->mKey = Object::ToString(obj);
        Int32 key;
        keyTitles->KeyAt(i, &key);
        res->GetString(key, &data->mTitle);
        res->GetString(R::string::zen_mode_settings_title, &data->mScreenTitle);
        result->Add((ISearchIndexableRaw*)data);
    }
    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode CZenModeSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IArrayList> rt;
    CArrayList::New((IArrayList**)&rt);
    if (!Utils::IsVoiceCapable(context)) {
        rt->Add(CoreUtils::Convert(KEY_CALLS));
    }
    *list = IList::Probe(rt);
    REFCOUNT_ADD(*list);
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::TimePickerPreference
//===============================================================================

CZenModeSettings::TimePickerPreference::TimePickerPreference()
    : mSummaryFormat(0)
    , mHourOfDay(0)
    , mMinute(0)
{}

ECode CZenModeSettings::TimePickerPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IFragmentManager* mgr)
{
    Preference::constructor(context);
    mContext = context;
    SetPersistent(FALSE);
    AutoPtr<TimePickerOnPreferenceClickListener> listener = new TimePickerOnPreferenceClickListener(this, mgr);
    SetOnPreferenceClickListener(listener);
    return NOERROR;
}

ECode CZenModeSettings::TimePickerPreference::SetCallback(
    /* [in] */ ITimePickerPreferenceCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CZenModeSettings::TimePickerPreference::SetSummaryFormat(
    /* [in] */ Int32 resId)
{
    mSummaryFormat = resId;
    UpdateSummary();
    return NOERROR;
}

ECode CZenModeSettings::TimePickerPreference::SetTime(
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    Boolean res;
    if (mCallback != NULL && (mCallback->OnSetTime(hourOfDay, minute, &res), !res)) return NOERROR;
    mHourOfDay = hourOfDay;
    mMinute = minute;
    UpdateSummary();
    return NOERROR;
}

void CZenModeSettings::TimePickerPreference::UpdateSummary()
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance((ICalendar**)&c);
    c->Set(ICalendar::HOUR_OF_DAY, mHourOfDay);
    c->Set(ICalendar::MINUTE, mMinute);
    AutoPtr<IDate> date;
    c->GetTime((IDate**)&date);
    String time;
    DateFormat::GetTimeFormat(mContext)->Format(date, &time);
    if (mSummaryFormat != 0) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(time));
        resources->GetString(mSummaryFormat, args, &time);
    }
    SetSummary(CoreUtils::Convert(time));
}

//===============================================================================
//                  CZenModeSettings::TimePickerFragment
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::TimePickerFragment, DialogFragment, ITimePickerDialogOnTimeSetListener)

CZenModeSettings::TimePickerFragment::TimePickerFragment()
{}

CZenModeSettings::TimePickerFragment::~TimePickerFragment()
{}

ECode CZenModeSettings::TimePickerFragment::constructor()
{
    return DialogFragment::constructor();
}

ECode CZenModeSettings::TimePickerFragment::OnCreateDialog(
    /* [in] */ IBundle* savedInstanceState,
    /* [out] */ IDialog** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<CZenModeSettingsTimePickerPreference> pref = (CZenModeSettingsTimePickerPreference*)mPref.Get();
    const Boolean usePref = pref != NULL && pref->mHourOfDay >= 0 && pref->mMinute >= 0;
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance((ICalendar**)&c);

    Int32 hour = pref->mHourOfDay;
    Int32 minute = pref->mMinute;
    if (!usePref) {
        c->Get(ICalendar::HOUR_OF_DAY, &hour);
        c->Get(ICalendar::MINUTE, &minute);
    }

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    return CTimePickerDialog::New(activity, this, hour, minute,
            DateFormat::Is24HourFormat(activity), result);
}

ECode CZenModeSettings::TimePickerFragment::OnTimeSet(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    if (mPref != NULL) {
        ((CZenModeSettingsTimePickerPreference*)mPref.Get())->SetTime(hourOfDay, minute);
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateZenModeDowntimeDaysSelection
//===============================================================================

CZenModeSettings::OnCreateZenModeDowntimeDaysSelection::OnCreateZenModeDowntimeDaysSelection()
{}

ECode CZenModeSettings::OnCreateZenModeDowntimeDaysSelection::constructor(
    /* [in] */ ISettingsPreferenceFragment* frag,
    /* [in] */ IContext* context,
    /* [in] */ const String& mode)
{
    mHost = (CZenModeSettings*)frag;
    return ZenModeDowntimeDaysSelection::constructor(context, mode);
}

ECode CZenModeSettings::OnCreateZenModeDowntimeDaysSelection::OnChanged(
    /* [in] */ const String& mode)
{
    if (mHost->mDisableListeners) return NOERROR;
    String sleepMode;
    mHost->mConfig->GetSleepMode(&sleepMode);
    if (Objects::Equals(CoreUtils::Convert(mode), CoreUtils::Convert(sleepMode))) return NOERROR;
    if (DEBUG) Logger::D(TAG, "days.onChanged sleepMode=%s", mode.string());
    AutoPtr<IZenModeConfig> newConfig;
    mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
    newConfig->SetSleepMode(mode);
    mHost->SetZenModeConfig(newConfig);
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::SettingPrefWithCallback
//===============================================================================

CZenModeSettings::SettingPrefWithCallback::SettingPrefWithCallback(
    /* [in] */ Int32 type,
    /* [in] */ const String& key,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [in] */ ArrayOf<Int32>* values)
    : SettingPref(type, key, setting, def, values)
    , mValue(0)
{}

ECode CZenModeSettings::SettingPrefWithCallback::SetCallback(
    /* [in] */ ISettingPrefWithCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CZenModeSettings::SettingPrefWithCallback::Update(
    /* [in] */ IContext* context)
{
    // Avoid callbacks from non-user changes.
    mValue = GetValue(context);
    return SettingPref::Update(context);
}

ECode CZenModeSettings::SettingPrefWithCallback::SetSetting(
    /* [in] */ IContext* context,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (value == mValue) {
        *result = TRUE;
        return NOERROR;
    }
    mValue = value;
    if (mCallback != NULL) {
        mCallback->OnSettingSelected(value);
    }
    return SettingPref::SetSetting(context, value, result);
}

AutoPtr<IPreference> CZenModeSettings::SettingPrefWithCallback::Init(
    /* [in] */ SettingsPreferenceFragment* settings)
{
    AutoPtr<IPreference> ret = SettingPref::Init(settings);
    AutoPtr<IActivity> activity;
    settings->GetActivity((IActivity**)&activity);
    mValue = GetValue(IContext::Probe(activity));

    return ret;
}

Boolean CZenModeSettings::SettingPrefWithCallback::SetValueWithoutCallback(
    /* [in] */ IContext* context,
    /* [in] */ Int32 value)
{
    // Set the current value ahead of time, this way we won't trigger a callback.
    mValue = value;

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    PutInt32(mType, resolver, mSetting, value, &result);
    return result;
}

Int32 CZenModeSettings::SettingPrefWithCallback::GetValue(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 result;
    GetInt32(mType, resolver, mSetting, mDefault, &result);
    return result;
}

//===============================================================================
//                  CZenModeSettings::SettingsObserver
//===============================================================================

ECode CZenModeSettings::SettingsObserver::constructor(
    /* [in] */ CZenModeSettings* host)
{
    mHost = host;
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->GetUriFor(ISettingsGlobal::ZEN_MODE, (IUri**)&ZEN_MODE_URI);
    global->GetUriFor(ISettingsGlobal::ZEN_MODE_CONFIG_ETAG, (IUri**)&ZEN_MODE_CONFIG_ETAG_URI);
    return ContentObserver::constructor(mHost->mHandler);
}

ECode CZenModeSettings::SettingsObserver::Register()
{
    AutoPtr<IContentResolver> resolver = mHost->GetContentResolver();
    resolver->RegisterContentObserver(ZEN_MODE_URI, FALSE, this);
    resolver->RegisterContentObserver(ZEN_MODE_CONFIG_ETAG_URI, FALSE, this);
    return NOERROR;
}

ECode CZenModeSettings::SettingsObserver::Unregister()
{
    mHost->GetContentResolver()->UnregisterContentObserver(this);
    return NOERROR;
}

ECode CZenModeSettings::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    ContentObserver::OnChange(selfChange, uri);
    Boolean res;
    IObject::Probe(ZEN_MODE_URI)->Equals(uri, &res);
    if (res) {
        PREF_ZEN_MODE->Update(mHost->mContext);
    }
    IObject::Probe(ZEN_MODE_CONFIG_ETAG_URI)->Equals(uri, &res);
    if (res) {
        mHost->UpdateZenModeConfig();
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::TimePickerOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::TimePickerOnPreferenceClickListener, Object, IPreferenceOnPreferenceClickListener)

CZenModeSettings::TimePickerOnPreferenceClickListener::TimePickerOnPreferenceClickListener(
    /* [in] */ TimePickerPreference* host,
    /* [in] */ IFragmentManager* mgr)
    : mHost(host)
    , mMgr(mgr)
{}

ECode CZenModeSettings::TimePickerOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<CZenModeSettingsTimePickerFragment> frag;
    CZenModeSettingsTimePickerFragment::NewByFriend((CZenModeSettingsTimePickerFragment**)&frag);
    frag->mPref = mHost;
    frag->Show(mMgr,
            String("Elastos::Droid::Settings::Notification::CZenModeSettingsTimePickerPreference"));
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::InitSettingPrefWithCallback
//===============================================================================

CZenModeSettings::InitSettingPrefWithCallback::InitSettingPrefWithCallback(
    /* [in] */ Int32 type,
    /* [in] */ const String& key,
    /* [in] */ const String& setting,
    /* [in] */ Int32 def,
    /* [in] */ ArrayOf<Int32>* values)
    : SettingPrefWithCallback(type, key, setting, def, values)
{}

ECode CZenModeSettings::InitSettingPrefWithCallback::GetCaption(
    /* [in] */ IResources* res,
    /* [in] */ Int32 value,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    switch (value) {
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS:
            return res->GetString(R::string::zen_mode_option_no_interruptions, result);
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS:
            return res->GetString(R::string::zen_mode_option_important_interruptions, result);
        default:
            return res->GetString(R::string::zen_mode_option_off, result);
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateSettingPrefWithCallback
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateSettingPrefWithCallback, Object, ISettingPrefWithCallback)

CZenModeSettings::OnCreateSettingPrefWithCallback::OnCreateSettingPrefWithCallback(
    /* [in] */ CZenModeSettings* host)
    : mHost(host)
{}

ECode CZenModeSettings::OnCreateSettingPrefWithCallback::OnSettingSelected(
    /* [in] */ Int32 value)
{
    if (value != ISettingsGlobal::ZEN_MODE_OFF) {
        mHost->ShowConditionSelection(value);
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateOnPreferenceChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateOnPreferenceChangeListener, Object, IPreferenceOnPreferenceChangeListener)

CZenModeSettings::OnCreateOnPreferenceChangeListener::OnCreateOnPreferenceChangeListener(
    /* [in] */ CZenModeSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CZenModeSettings::OnCreateOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (mId) {
        case 0: {
            if (mHost->mDisableListeners) {
                *result = TRUE;
                return NOERROR;
            }
            Boolean val;
            IBoolean::Probe(newValue)->GetValue(&val);
            Boolean allowCalls;
            mHost->mConfig->GetAllowCalls(&allowCalls);
            if (val == allowCalls) {
                *result = TRUE;
                return NOERROR;
            }
            if (DEBUG) Logger::D(TAG, "onPrefChange allowCalls=%d", val);
            AutoPtr<IZenModeConfig> newConfig;
            mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
            newConfig->SetAllowCalls(val);
            *result = mHost->SetZenModeConfig(newConfig);
            return NOERROR;
        }
        case 1: {
            if (mHost->mDisableListeners) {
                *result = TRUE;
                return NOERROR;
            }
            Boolean val;
            IBoolean::Probe(newValue)->GetValue(&val);
            Boolean allowMessages;
            mHost->mConfig->GetAllowMessages(&allowMessages);
            if (val == allowMessages) {
                *result = TRUE;
                return NOERROR;
            }
            if (DEBUG) Logger::D(TAG, "onPrefChange allowMessages=%d", val);
            AutoPtr<IZenModeConfig> newConfig;
            mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
            newConfig->SetAllowMessages(val);
            *result = mHost->SetZenModeConfig(newConfig);
            return NOERROR;
        }
        case 2: {
            if (mHost->mDisableListeners) {
                *result = TRUE;
                return NOERROR;
            }
            Boolean val;
            IBoolean::Probe(newValue)->GetValue(&val);
            Boolean allowEvents;
            mHost->mConfig->GetAllowEvents(&allowEvents);
            if (val == allowEvents) {
                *result = TRUE;
                return NOERROR;
            }
            if (DEBUG) Logger::D(TAG, "onPrefChange allowEvents=%d", val);
            AutoPtr<IZenModeConfig> newConfig;
            mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
            newConfig->SetAllowEvents(val);
            *result = mHost->SetZenModeConfig(newConfig);
            return NOERROR;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateDropDownPreferenceCallback
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateDropDownPreferenceCallback, Object, IDropDownPreferenceCallback)

CZenModeSettings::OnCreateDropDownPreferenceCallback::OnCreateDropDownPreferenceCallback(
    /* [in] */ CZenModeSettings* host)
    : mHost(host)
{}

ECode CZenModeSettings::OnCreateDropDownPreferenceCallback::OnItemSelected(
    /* [in] */ Int32 pos,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (mHost->mDisableListeners) {
        *result = TRUE;
        return NOERROR;
    }
    Int32 val;
    IInteger32::Probe(newValue)->GetValue(&val);
    Int32 allowFrom;
    mHost->mConfig->GetAllowFrom(&allowFrom);
    if (val == allowFrom) {
        *result = TRUE;
        return NOERROR;
    }
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    String str;
    helper->SourceToString(val, &str);
    if (DEBUG) Logger::D(TAG, "onPrefChange allowFrom=%s", str.string());
    AutoPtr<IZenModeConfig> newConfig;
    mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
    newConfig->SetAllowFrom(val);
    *result = mHost->SetZenModeConfig(newConfig);
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::
//===============================================================================


//===============================================================================
//                  CZenModeSettings::OnCreateTimePickerPreferenceCallback
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateTimePickerPreferenceCallback, Object, ITimePickerPreferenceCallback)

CZenModeSettings::OnCreateTimePickerPreferenceCallback::OnCreateTimePickerPreferenceCallback(
    /* [in] */ CZenModeSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CZenModeSettings::OnCreateTimePickerPreferenceCallback::OnSetTime(
    /* [in] */ Int32 hour,
    /* [in] */ Int32 minute,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);

    Boolean res;
    switch (mId) {
        case 0: {
            if (mHost->mDisableListeners) {
                *result = TRUE;
                return NOERROR;
            }
            if (helper->IsValidHour(hour, &res), !res) {
                *result = FALSE;
                return NOERROR;
            }
            if (helper->IsValidMinute(minute, &res), !res) {
                *result = FALSE;
                return NOERROR;
            }
            Int32 sleepStartHour, sleepStartMinute;
            mHost->mConfig->GetSleepStartHour(&sleepStartHour);
            if (hour == sleepStartHour &&
                    minute == (mHost->mConfig->GetSleepStartMinute(&sleepStartMinute), sleepStartMinute)) {
                *result = TRUE;
                return NOERROR;
            }
            if (DEBUG) Logger::D(TAG, "onPrefChange sleepStart h=%d m=%d", hour, minute);
            AutoPtr<IZenModeConfig> newConfig;
            mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
            newConfig->SetSleepStartHour(hour);
            newConfig->SetSleepStartMinute(minute);
            *result = mHost->SetZenModeConfig(newConfig);
            return NOERROR;
        }

        case 1: {
            if (mHost->mDisableListeners) {
                *result = TRUE;
                return NOERROR;
            }
            if (helper->IsValidHour(hour, &res), !res) {
                *result = FALSE;
                return NOERROR;
            }
            if (helper->IsValidMinute(minute, &res), !res) {
                *result = FALSE;
                return NOERROR;
            }
            Int32 sleepEndHour, sleepEndMinute;
            mHost->mConfig->GetSleepEndHour(&sleepEndHour);
            if (hour == sleepEndHour &&
                    minute == (mHost->mConfig->GetSleepEndMinute(&sleepEndMinute), sleepEndMinute)) {
                *result = TRUE;
                return NOERROR;
            }
            if (DEBUG) Logger::D(TAG, "onPrefChange sleepEnd h=%d m=%d", hour, minute);
            AutoPtr<IZenModeConfig> newConfig;
            mHost->mConfig->Copy((IZenModeConfig**)&newConfig);
            newConfig->SetSleepEndHour(hour);
            newConfig->SetSleepEndMinute(minute);
            *result = mHost->SetZenModeConfig(newConfig);
            return NOERROR;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateOnPreferenceClickListener, Object, IPreferenceOnPreferenceClickListener)

CZenModeSettings::OnCreateOnPreferenceClickListener::OnCreateOnPreferenceClickListener(
    /* [in] */ CZenModeSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CZenModeSettings::OnCreateOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    switch (mId) {
        case 0: {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::zen_mode_downtime_days);
            String sleepMode;
            mHost->mConfig->GetSleepMode(&sleepMode);
            AutoPtr<IView> selection;
            CZenModeSettingsDowntimeDaysSelection::New((ISettingsPreferenceFragment*)mHost,
                    mHost->mContext, sleepMode, (IView**)&selection);
            builder->SetView(selection);
            AutoPtr<OnCreateOnDismissListener> listener = new OnCreateOnDismissListener(mHost , 0);
            builder->SetOnDismissListener(listener);
            builder->SetPositiveButton(R::string::done_button, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Show((IAlertDialog**)&dialog);

            *result = TRUE;
            return NOERROR;
        }

        case 1: {
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(mHost->mContext, (IAlertDialogBuilder**)&builder);
            builder->SetTitle(R::string::zen_mode_entry_conditions_title);
            AutoPtr<IView> selection;
            assert(0 && "TODO");
            // CZenModeAutomaticConditionSelection::New(mHost->mContext, (IView**)&selection);
            builder->SetView(selection);
            AutoPtr<OnCreateOnDismissListener> listener = new OnCreateOnDismissListener(mHost , 1);
            builder->SetOnDismissListener(listener);
            builder->SetPositiveButton(R::string::dlg_ok, NULL);
            AutoPtr<IAlertDialog> dialog;
            builder->Show((IAlertDialog**)&dialog);

            *result = TRUE;
            return TRUE;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings::OnCreateOnDismissListener
//===============================================================================

CAR_INTERFACE_IMPL(CZenModeSettings::OnCreateOnDismissListener, Object, IDialogInterfaceOnDismissListener)

CZenModeSettings::OnCreateOnDismissListener::OnCreateOnDismissListener(
    /* [in] */ CZenModeSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CZenModeSettings::OnCreateOnDismissListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    switch (mId) {
        case 0:
            mHost->UpdateDays();
            break;
        case 1:
            mHost->RefreshAutomationSection();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  CZenModeSettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CZenModeSettings::SEARCH_INDEX_DATA_PROVIDER;

const String CZenModeSettings::TAG("CZenModeSettings");
const Boolean CZenModeSettings::DEBUG = TRUE;

const String CZenModeSettings::KEY_ZEN_MODE("zen_mode");
const String CZenModeSettings::KEY_IMPORTANT("important");
const String CZenModeSettings::KEY_CALLS("phone_calls");
const String CZenModeSettings::KEY_MESSAGES("messages");
const String CZenModeSettings::KEY_STARRED("starred");
const String CZenModeSettings::KEY_EVENTS("events");
const String CZenModeSettings::KEY_ALARM_INFO("alarm_info");

const String CZenModeSettings::KEY_DOWNTIME("downtime");
const String CZenModeSettings::KEY_DAYS("days");
const String CZenModeSettings::KEY_START_TIME("start_time");
const String CZenModeSettings::KEY_END_TIME("end_time");

const String CZenModeSettings::KEY_AUTOMATION("automation");
const String CZenModeSettings::KEY_ENTRY("entry");
const String CZenModeSettings::KEY_CONDITION_PROVIDERS("manage_condition_providers");

AutoPtr<CZenModeSettings::SettingPrefWithCallback> CZenModeSettings::PREF_ZEN_MODE;

const AutoPtr<ISimpleDateFormat> CZenModeSettings::DAY_FORMAT;

Boolean CZenModeSettings::InitStatic()
{
    if (PREF_ZEN_MODE == NULL) {
        AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(3);
        (*args)[0] = ISettingsGlobal::ZEN_MODE_OFF;
        (*args)[1] = ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS;
        (*args)[2] = ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS;

        PREF_ZEN_MODE = new CZenModeSettings::InitSettingPrefWithCallback(
                SettingPref::TYPE_GLOBAL, KEY_ZEN_MODE, ISettingsGlobal::ZEN_MODE,
                ISettingsGlobal::ZEN_MODE_OFF, args);
    }

    if (DAY_FORMAT == NULL) {
        CSimpleDateFormat::New(String("EEE"), (ISimpleDateFormat**)&DAY_FORMAT);
    }

    return TRUE;
}

const Boolean CZenModeSettings::sInitStatic = InitStatic();

AutoPtr<IIndexableSearchIndexProvider> CZenModeSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CZenModeSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CZenModeSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CZenModeSettings)

CZenModeSettings::CZenModeSettings()
    : mDisableListeners(FALSE)
{}

CZenModeSettings::~CZenModeSettings()
{}

ECode CZenModeSettings::constructor()
{
    CHandler::New((IHandler**)&mHandler);
    mSettingsObserver = new SettingsObserver();
    mSettingsObserver->constructor(this);

    return SettingsPreferenceFragment::constructor();
}

AutoPtr<ISparseArray> CZenModeSettings::AllKeyTitles(
    /* [in] */ IContext* context)
{
    AutoPtr<ISparseArray> rt;//SparseArray<String>
    CSparseArray::New((ISparseArray**)&rt);
    rt->Put(R::string::zen_mode_important_category, CoreUtils::Convert(KEY_IMPORTANT));
    if (Utils::IsVoiceCapable(context)) {
        rt->Put(R::string::zen_mode_phone_calls, CoreUtils::Convert(KEY_CALLS));
        rt->Put(R::string::zen_mode_option_title, CoreUtils::Convert(KEY_ZEN_MODE));
    }
    else {
        rt->Put(R::string::zen_mode_option_title_novoice, CoreUtils::Convert(KEY_ZEN_MODE));
    }
    rt->Put(R::string::zen_mode_messages, CoreUtils::Convert(KEY_MESSAGES));
    rt->Put(R::string::zen_mode_from_starred, CoreUtils::Convert(KEY_STARRED));
    rt->Put(R::string::zen_mode_events, CoreUtils::Convert(KEY_EVENTS));
    rt->Put(R::string::zen_mode_alarm_info, CoreUtils::Convert(KEY_ALARM_INFO));
    rt->Put(R::string::zen_mode_downtime_category, CoreUtils::Convert(KEY_DOWNTIME));
    rt->Put(R::string::zen_mode_downtime_days, CoreUtils::Convert(KEY_DAYS));
    rt->Put(R::string::zen_mode_start_time, CoreUtils::Convert(KEY_START_TIME));
    rt->Put(R::string::zen_mode_end_time, CoreUtils::Convert(KEY_END_TIME));
    rt->Put(R::string::zen_mode_automation_category, CoreUtils::Convert(KEY_AUTOMATION));
    rt->Put(R::string::manage_condition_providers, CoreUtils::Convert(KEY_CONDITION_PROVIDERS));
    return rt;
}

ECode CZenModeSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    mContext->GetPackageManager((IPackageManager**)&mPM);

    AddPreferencesFromResource(R::xml::zen_mode_settings);
    AutoPtr<IPreferenceScreen> root;
    GetPreferenceScreen((IPreferenceScreen**)&root);

    mConfig = GetZenModeConfig();
    if (DEBUG) Logger::D(TAG, "Loaded mConfig=%s", TO_CSTR(mConfig));

    AutoPtr<IPreference> zenMode = PREF_ZEN_MODE->Init(this);
    AutoPtr<OnCreateSettingPrefWithCallback> callback = new OnCreateSettingPrefWithCallback(this);
    PREF_ZEN_MODE->SetCallback(callback);

    if (!Utils::IsVoiceCapable(mContext)) {
        zenMode->SetTitle(R::string::zen_mode_option_title_novoice);
    }

    AutoPtr<IPreference> importantPreference;
    IPreferenceGroup::Probe(root)->FindPreference(CoreUtils::Convert(KEY_IMPORTANT), (IPreference**)&importantPreference);
    IPreferenceCategory* important = IPreferenceCategory::Probe(importantPreference);

    IPreferenceGroup* importantGroup = IPreferenceGroup::Probe(important);
    AutoPtr<IPreference> preferenceCalls;
    importantGroup->FindPreference(CoreUtils::Convert(KEY_CALLS), (IPreference**)&preferenceCalls);
    mCalls = ISwitchPreference::Probe(preferenceCalls);
    Boolean res;
    if (Utils::IsVoiceCapable(mContext)) {
        AutoPtr<OnCreateOnPreferenceChangeListener> listener0 = new OnCreateOnPreferenceChangeListener(this, 0);
        preferenceCalls->SetOnPreferenceChangeListener(listener0);
    }
    else {
        importantGroup->RemovePreference(preferenceCalls, &res);
        mCalls = NULL;
    }

    AutoPtr<IPreference> preferenceMessages;
    importantGroup->FindPreference(CoreUtils::Convert(KEY_MESSAGES), (IPreference**)&preferenceMessages);
    mMessages = ISwitchPreference::Probe(preferenceMessages);
    AutoPtr<OnCreateOnPreferenceChangeListener> listener1 = new OnCreateOnPreferenceChangeListener(this, 1);
    preferenceMessages->SetOnPreferenceChangeListener(listener1);

    AutoPtr<IPreference> preferenceStarred;
    importantGroup->FindPreference(CoreUtils::Convert(KEY_STARRED), (IPreference**)&preferenceStarred);
    mStarred = (CDropDownPreference*)IDropDownPreference::Probe(preferenceStarred);
    mStarred->AddItem(R::string::zen_mode_from_anyone, CoreUtils::Convert(IZenModeConfig::SOURCE_ANYONE));
    mStarred->AddItem(R::string::zen_mode_from_starred, CoreUtils::Convert(IZenModeConfig::SOURCE_STAR));
    mStarred->AddItem(R::string::zen_mode_from_contacts, CoreUtils::Convert(IZenModeConfig::SOURCE_CONTACT));
    AutoPtr<OnCreateDropDownPreferenceCallback> ddpCallback = new OnCreateDropDownPreferenceCallback(this);
    mStarred->SetCallback(ddpCallback);
    importantGroup->AddPreference(preferenceStarred, &res);

    AutoPtr<IPreference> preferenceEvents;
    importantGroup->FindPreference(CoreUtils::Convert(KEY_EVENTS), (IPreference**)&preferenceEvents);
    mEvents = ISwitchPreference::Probe(preferenceEvents);
    AutoPtr<OnCreateOnPreferenceChangeListener> listener2 = new OnCreateOnPreferenceChangeListener(this, 2);
    preferenceEvents->SetOnPreferenceChangeListener(listener2);

    AutoPtr<IPreference> downtimePreference;
    IPreferenceGroup::Probe(root)->FindPreference(CoreUtils::Convert(KEY_DOWNTIME), (IPreference**)&downtimePreference);
    IPreferenceGroup* /*IPreferenceCategory*/ downtime = IPreferenceGroup::Probe(downtimePreference);

    downtime->FindPreference(CoreUtils::Convert(KEY_DAYS), (IPreference**)&mDays);
    AutoPtr<OnCreateOnPreferenceClickListener> daysListener = new OnCreateOnPreferenceClickListener(this, 0);
    mDays->SetOnPreferenceClickListener(daysListener);

    AutoPtr<IFragmentManager> mgr;
    GetFragmentManager((IFragmentManager**)&mgr);

    CZenModeSettingsTimePickerPreference::New(mContext, mgr, (IPreference**)&mStart);
    mStart->SetKey(KEY_START_TIME);
    mStart->SetTitle(R::string::zen_mode_start_time);
    AutoPtr<OnCreateTimePickerPreferenceCallback> tppCallback0 = new OnCreateTimePickerPreferenceCallback(this, 0);
    ((CZenModeSettingsTimePickerPreference*)mStart.Get())->SetCallback(tppCallback0);
    downtime->AddPreference(mStart, &res);
    String key;
    mDays->GetKey(&key);
    mStart->SetDependency(key);

    CZenModeSettingsTimePickerPreference::New(mContext, mgr, (IPreference**)&mEnd);
    mEnd->SetKey(KEY_END_TIME);
    mEnd->SetTitle(R::string::zen_mode_end_time);
    AutoPtr<OnCreateTimePickerPreferenceCallback> tppCallback1 = new OnCreateTimePickerPreferenceCallback(this, 1);
    ((CZenModeSettingsTimePickerPreference*)mEnd.Get())->SetCallback(tppCallback1);
    downtime->AddPreference(mEnd, &res);
    mEnd->SetDependency(key);

    AutoPtr<IPreference> preference;
    FindPreference(CoreUtils::Convert(KEY_AUTOMATION), (IPreference**)&preference);
    mAutomationCategory = IPreferenceCategory::Probe(preference);
    FindPreference(CoreUtils::Convert(KEY_ENTRY), (IPreference**)&mEntry);
    AutoPtr<OnCreateOnPreferenceClickListener> entryListener = new OnCreateOnPreferenceClickListener(this, 1);
    mEntry->SetOnPreferenceClickListener(entryListener);
    FindPreference(CoreUtils::Convert(KEY_CONDITION_PROVIDERS), (IPreference**)&mConditionProviders);

    UpdateControls();
    return NOERROR;
}

void CZenModeSettings::UpdateDays()
{
    if (mConfig != NULL) {
        String sleepMode;
        mConfig->GetSleepMode(&sleepMode);

        AutoPtr<IZenModeConfigHelper> helper;
        CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
        AutoPtr< ArrayOf<Int32> > days;
        helper->TryParseDays(sleepMode, (ArrayOf<Int32>**)&days);
        if (days != NULL && days->GetLength() != 0) {
            AutoPtr<StringBuilder> sb = new StringBuilder();
            AutoPtr<ICalendarHelper> cHelper;
            CCalendarHelper::AcquireSingleton((ICalendarHelper**)&cHelper);
            AutoPtr<ICalendar> c;
            cHelper->GetInstance((ICalendar**)&c);

            for (Int32 i = 0; i < ZenModeDowntimeDaysSelection::DAYS->GetLength(); i++) {
                const Int32 day = (*ZenModeDowntimeDaysSelection::DAYS)[i];
                for (Int32 j = 0; j < days->GetLength(); j++) {
                    if (day == (*days)[j]) {
                        c->Set(ICalendar::DAY_OF_WEEK, day);
                        if (sb->GetLength() > 0) {
                            String str;
                            mContext->GetString(R::string::summary_divider_text, &str);
                            sb->Append(str);
                        }
                        AutoPtr<IDate> date;
                        c->GetTime((IDate**)&date);
                        String dateStr;
                        Elastos::Text::IDateFormat::Probe(DAY_FORMAT)->Format(date, &dateStr);
                        sb->Append(dateStr);
                        break;
                    }
                }
            }
            if (sb->GetLength() > 0) {
                mDays->SetSummary(sb);
                mDays->NotifyDependencyChange(FALSE);
                return;
            }
        }
    }
    mDays->SetSummary(R::string::zen_mode_downtime_days_none);
    mDays->NotifyDependencyChange(TRUE);
}

void CZenModeSettings::UpdateEndSummary()
{
    Int32 sleepStartHour, sleepStartMinute, sleepEndHour, sleepEndMinute;
    mConfig->GetSleepStartHour(&sleepStartHour);
    mConfig->GetSleepStartMinute(&sleepStartMinute);
    mConfig->GetSleepEndHour(&sleepEndHour);
    mConfig->GetSleepEndMinute(&sleepEndMinute);
    const Int32 startMin = 60 * sleepStartHour + sleepStartMinute;
    const Int32 endMin = 60 * sleepEndHour + sleepEndMinute;
    const Boolean nextDay = startMin >= endMin;
    ((CZenModeSettingsTimePickerPreference*)mEnd.Get())->SetSummaryFormat(nextDay ? R::string::zen_mode_end_time_summary_format : 0);
}

void CZenModeSettings::UpdateControls()
{
    mDisableListeners = TRUE;
    if (mCalls != NULL) {
        Boolean allowCalls;
        mConfig->GetAllowCalls(&allowCalls);
        ITwoStatePreference::Probe(mCalls)->SetChecked(allowCalls);
    }
    Boolean allowMessages;
    mConfig->GetAllowMessages(&allowMessages);
    ITwoStatePreference::Probe(mMessages)->SetChecked(allowMessages);
    Int32 allowFrom;
    mConfig->GetAllowFrom(&allowFrom);
    mStarred->SetSelectedValue(CoreUtils::Convert(allowFrom));
    Boolean allowEvents;
    mConfig->GetAllowEvents(&allowEvents);
    ITwoStatePreference::Probe(mEvents)->SetChecked(allowEvents);
    UpdateStarredEnabled();
    UpdateDays();

    Int32 sleepStartHour, sleepStartMinute, sleepEndHour, sleepEndMinute;
    mConfig->GetSleepStartHour(&sleepStartHour);
    mConfig->GetSleepStartMinute(&sleepStartMinute);
    mConfig->GetSleepEndHour(&sleepEndHour);
    mConfig->GetSleepEndMinute(&sleepEndMinute);
    ((CZenModeSettingsTimePickerPreference*)mStart.Get())->SetTime(sleepStartHour, sleepStartMinute);
    ((CZenModeSettingsTimePickerPreference*)mEnd.Get())->SetTime(sleepEndHour, sleepEndMinute);
    mDisableListeners = FALSE;
    RefreshAutomationSection();
    UpdateEndSummary();
}

void CZenModeSettings::UpdateStarredEnabled()
{
    Boolean allowCalls;
    mConfig->GetAllowCalls(&allowCalls);
    Boolean allowMessages;
    mConfig->GetAllowMessages(&allowMessages);
    mStarred->SetEnabled(allowCalls || allowMessages);
}

void CZenModeSettings::RefreshAutomationSection()
{
    if (mConditionProviders != NULL) {
        const Int32 total = CConditionProviderSettings::GetProviderCount(mPM);
        if (total == 0) {
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            Boolean res;
            IPreferenceGroup::Probe(screen)->RemovePreference(IPreference::Probe(mAutomationCategory), &res);
        }
        else {
            const Int32 n = CConditionProviderSettings::GetEnabledProviderCount(mContext);
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            if (n == 0) {
                String str;
                resources->GetString(
                        R::string::manage_condition_providers_summary_zero, &str);
                mConditionProviders->SetSummary(CoreUtils::Convert(str));
            }
            else {
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(n));
                String str;
                resources->GetQuantityString(
                        R::plurals::manage_condition_providers_summary_nonzero,
                        n, args, &str);
                String format("");
                format.AppendFormat(str);
                mConditionProviders->SetSummary(CoreUtils::Convert(format));
            }
            String entrySummary = GetEntryConditionSummary();
            if (n == 0 || entrySummary.IsNull()) {
                mEntry->SetSummary(R::string::zen_mode_entry_conditions_summary_none);
            }
            else {
                mEntry->SetSummary(CoreUtils::Convert(entrySummary));
            }
        }
    }
}

String CZenModeSettings::GetEntryConditionSummary()
{
    AutoPtr<IINotificationManager> nm = IINotificationManager::Probe(ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));

    AutoPtr< ArrayOf<ICondition*> > automatic;
    ECode ec = nm->GetAutomaticZenModeConditions((ArrayOf<ICondition*>**)&automatic);
    if (SUCCEEDED(ec)) {
        if (automatic == NULL || automatic->GetLength() == 0) {
            return String(NULL);
        }
        String divider;
        GetString(R::string::summary_divider_text, &divider);
        AutoPtr<StringBuilder> sb = new StringBuilder();
        for (Int32 i = 0; i < automatic->GetLength(); i++) {
            if (i > 0) sb->Append(divider);
            String summary;
            (*automatic)[i]->GetSummary(&summary);
            sb->Append(summary);
        }
        return sb->ToString();
    }
    else {
        Logger::W(TAG, "Error calling getAutomaticZenModeConditions 0x%08x", ec);
        return String(NULL);
    }
}

ECode CZenModeSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    UpdateControls();
    mSettingsObserver->Register();
    return NOERROR;
}

ECode CZenModeSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    mSettingsObserver->Unregister();
    return NOERROR;
}

void CZenModeSettings::UpdateZenModeConfig()
{
    AutoPtr<IZenModeConfig> config = GetZenModeConfig();
    if (Objects::Equals(config, mConfig)) return;
    mConfig = config;
    if (DEBUG) Logger::D(TAG, "updateZenModeConfig mConfig= %s", TO_CSTR(mConfig));
    UpdateControls();
}

AutoPtr<IZenModeConfig> CZenModeSettings::GetZenModeConfig()
{
    AutoPtr<IINotificationManager> nm = IINotificationManager::Probe(ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));

    AutoPtr<IZenModeConfig> config;
    ECode ec = nm->GetZenModeConfig((IZenModeConfig**)&config);
    if (SUCCEEDED(ec)) {
        return config;
    }
    else {
       Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
       config = NULL;
       CZenModeConfig::New((IZenModeConfig**)&config);
       return config;
    }
}

Boolean CZenModeSettings::SetZenModeConfig(
    /* [in] */ IZenModeConfig* config)
{
    AutoPtr<IINotificationManager> nm = IINotificationManager::Probe(ServiceManager::GetService(IContext::NOTIFICATION_SERVICE));

    Boolean success;
    ECode ec = nm->SetZenModeConfig(config, &success);
    if (SUCCEEDED(ec)) {
        if (success) {
            mConfig = config;
            if (DEBUG) Logger::D(TAG, "Saved mConfig= %s", TO_CSTR(mConfig));
            UpdateEndSummary();
            UpdateStarredEnabled();
        }
        return success;
    }
    else {
       Logger::W(TAG, "Error calling NoMan 0x%08x", ec);
       return FALSE;
    }
}

void CZenModeSettings::PutZenModeSetting(
    /* [in] */ Int32 value)
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutInt32(GetContentResolver(), ISettingsGlobal::ZEN_MODE, value, &res);
}

void CZenModeSettings::ShowConditionSelection(
    /* [in] */ Int32 newSettingsValue)
{
    assert(0 && "TODO");
    // if (mDialog != NULL) return;

    // final ZenModeConditionSelection zenModeConditionSelection =
    //         new ZenModeConditionSelection(mContext);
    // DialogInterface.OnClickListener positiveListener = new DialogInterface->OnClickListener() {
    //     //@Override
    //     CARAPI OnClick(
    //         /* [in] */ IDialogInterface* dialog,
    //         /* [in] */ Int32 which)
    //     {
    //         zenModeConditionSelection->ConfirmCondition();
    //         mDialog = NULL;
    //     }
    // };
    // final Int32 oldSettingsValue = PREF_ZEN_MODE->GetValue(mContext);
    // ScrollView scrollView = new ScrollView(mContext);
    // scrollView->AddView(zenModeConditionSelection);
    // mDialog = new AlertDialog->Builder(GetActivity())
    //         .SetTitle(PREF_ZEN_MODE->GetCaption(GetResources(), newSettingsValue))
    //         .SetView(scrollView)
    //         .SetPositiveButton(R::string::okay, positiveListener)
    //         .SetNegativeButton(R::string::cancel_all_caps, new DialogInterface->OnClickListener() {
    //             //@Override
    //             CARAPI OnClick(
    //                 /* [in] */ IDialogInterface* dialog,
    //                 /* [in] */ Int32 which)
    //             {
    //                 CancelDialog(oldSettingsValue);
    //             }
    //         })
    //         .SetOnCancelListener(new DialogInterface->OnCancelListener() {
    //             //@Override
    //             CARAPI OnCancel(
    //                 /* [in] */ IDialogInterface* dialog)
    //             {
    //                 CancelDialog(oldSettingsValue);
    //             }
    //         }).Create();
    // mDialog->Show();
}

void CZenModeSettings::CancelDialog(
    /* [in] */ Int32 oldSettingsValue)
{
    // If not making a decision, reset drop down to current setting.
    PREF_ZEN_MODE->SetValueWithoutCallback(mContext, oldSettingsValue);
    mDialog = NULL;
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos