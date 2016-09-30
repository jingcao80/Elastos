
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/CDisplaySettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/CDreamSettings.h"
#include "elastos/droid/settings/notification/CDropDownPreference.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Notification::CDropDownPreference;
using Elastos::Droid::Settings::Notification::IDropDownPreference;
using Elastos::Droid::Settings::Notification::EIID_IDropDownPreferenceCallback;
using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Internal::View::CRotationPolicy;
using Elastos::Droid::Internal::View::IRotationPolicy;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDisplaySettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_2(CDisplaySettings::InnerListener, Object, IPreferenceOnPreferenceChangeListener, IPreferenceOnPreferenceClickListener)

CDisplaySettings::InnerListener::InnerListener(
    /* [in] */ CDisplaySettings* host)
    : mHost(host)
{}

ECode CDisplaySettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, newValue, result);
}

ECode CDisplaySettings::InnerListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceClick(preference, result);
}

//===============================================================================
//                  CDisplaySettings::MyBaseSearchIndexProvider
//===============================================================================

CDisplaySettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CDisplaySettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CDisplaySettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::display_settings);
    result->Add(sir);

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode CDisplaySettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = NULL;

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(Elastos::Droid::R::bool_::config_dreamsSupported, &res);
    if (!res) {
        result->Add(CoreUtils::Convert(KEY_SCREEN_SAVER));
    }
    if (!IsAutomaticBrightnessAvailable(resources)) {
        result->Add(CoreUtils::Convert(KEY_AUTO_BRIGHTNESS));
    }
    if (!IsLiftToWakeAvailable(context)) {
        result->Add(CoreUtils::Convert(KEY_LIFT_TO_WAKE));
    }
    if (!IsDozeAvailable(context)) {
        result->Add(CoreUtils::Convert(KEY_DOZE));
    }

    AutoPtr<IRotationPolicy> rp;
    CRotationPolicy::AcquireSingleton((IRotationPolicy**)&rp);
    rp->IsRotationLockToggleVisible(context, &res);
    if (!res) {
        result->Add(CoreUtils::Convert(KEY_AUTO_ROTATE));
    }

    *list = result;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

//===============================================================================
//                  CDisplaySettings::OnCreateCallback
//===============================================================================

CAR_INTERFACE_IMPL(CDisplaySettings::OnCreateCallback, Object, IDropDownPreferenceCallback)

CDisplaySettings::OnCreateCallback::OnCreateCallback(
    /* [in] */ CDisplaySettings* host,
    /* [in] */ IContext* context)
    : mHost(host)
    , mContext(context)
{}

ECode CDisplaySettings::OnCreateCallback::OnItemSelected(
    /* [in] */ Int32 pos,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<IRotationPolicy> rp;
    CRotationPolicy::AcquireSingleton((IRotationPolicy**)&rp);
    Boolean res;
    IBoolean::Probe(value)->GetValue(&res);
    rp->SetRotationLock(mContext, res);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CDisplaySettings::MyRunnable
//===============================================================================

CDisplaySettings::MyRunnable::MyRunnable(
    /* [in] */ CDisplaySettings* host)
    : mHost(host)
{}

ECode CDisplaySettings::MyRunnable::Run()
{
    mHost->mFontSizePref->Click();
    return NOERROR;
}

//===============================================================================
//                  CDisplaySettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CDisplaySettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> CDisplaySettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CDisplaySettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

const String CDisplaySettings::TAG("CDisplaySettings");

const Int32 CDisplaySettings::FALLBACK_SCREEN_TIMEOUT_VALUE = 30000;

const String CDisplaySettings::KEY_SCREEN_TIMEOUT("screen_timeout");
const String CDisplaySettings::KEY_FONT_SIZE("font_size");
const String CDisplaySettings::KEY_SCREEN_SAVER("screensaver");
const String CDisplaySettings::KEY_LIFT_TO_WAKE("lift_to_wake");
const String CDisplaySettings::KEY_DOZE("doze");
const String CDisplaySettings::KEY_AUTO_BRIGHTNESS("auto_brightness");
const String CDisplaySettings::KEY_AUTO_ROTATE("auto_rotate");

const Int32 CDisplaySettings::DLG_GLOBAL_CHANGE_WARNING = 1;

CAR_INTERFACE_IMPL(CDisplaySettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CDisplaySettings)

CDisplaySettings::CDisplaySettings()
{}

CDisplaySettings::~CDisplaySettings()
{}

ECode CDisplaySettings::constructor()
{
    CConfiguration::New((IConfiguration**)&mCurConfig);
    mListener = new InnerListener(this);
    return SettingsPreferenceFragment::constructor();
}

ECode CDisplaySettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);

    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);

    AddPreferencesFromResource(R::xml::display_settings);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    FindPreference(CoreUtils::Convert(KEY_SCREEN_SAVER), (IPreference**)&mScreenSaverPreference);
    Boolean res;
    if (mScreenSaverPreference != NULL
            && (resources->GetBoolean(Elastos::Droid::R::bool_::config_dreamsSupported, &res), res) == FALSE) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(mScreenSaverPreference, &res);
    }

    AutoPtr<IPreference> screenTimeoutPreferenceTmp;
    FindPreference(CoreUtils::Convert(KEY_SCREEN_TIMEOUT), (IPreference**)&screenTimeoutPreferenceTmp);
    mScreenTimeoutPreference = IListPreference::Probe(screenTimeoutPreferenceTmp);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int64 currentTimeout;
    ss->GetInt64(resolver, ISettingsSystem::SCREEN_OFF_TIMEOUT, FALLBACK_SCREEN_TIMEOUT_VALUE, &currentTimeout);
    mScreenTimeoutPreference->SetValue(StringUtils::ToString(currentTimeout));
    screenTimeoutPreferenceTmp->SetOnPreferenceChangeListener(mListener);
    DisableUnusableTimeouts(mScreenTimeoutPreference);
    UpdateTimeoutPreferenceDescription(currentTimeout);

    AutoPtr<IPreference> fontSizePrefTmp;
    FindPreference(CoreUtils::Convert(KEY_FONT_SIZE), (IPreference**)&fontSizePrefTmp);
    mFontSizePref = (CWarnedListPreference*) IListPreference::Probe(fontSizePrefTmp);
    mFontSizePref->SetOnPreferenceChangeListener(mListener);
    mFontSizePref->SetOnPreferenceClickListener(mListener);

    if (IsAutomaticBrightnessAvailable(resources)) {
        AutoPtr<IPreference> autoBrightnessPreferenceTmp;
        FindPreference(CoreUtils::Convert(KEY_AUTO_BRIGHTNESS), (IPreference**)&autoBrightnessPreferenceTmp);
        mAutoBrightnessPreference = ISwitchPreference::Probe(autoBrightnessPreferenceTmp);
        autoBrightnessPreferenceTmp->SetOnPreferenceChangeListener(mListener);
    }
    else {
        RemovePreference(KEY_AUTO_BRIGHTNESS);
    }

    if (IsLiftToWakeAvailable(activity)) {
        AutoPtr<IPreference> liftToWakePreferenceTmp;
        FindPreference(CoreUtils::Convert(KEY_LIFT_TO_WAKE), (IPreference**)&liftToWakePreferenceTmp);
        mLiftToWakePreference = ISwitchPreference::Probe(liftToWakePreferenceTmp);
        liftToWakePreferenceTmp->SetOnPreferenceChangeListener(mListener);
    }
    else {
        RemovePreference(KEY_LIFT_TO_WAKE);
    }

    if (IsDozeAvailable(activity)) {
        AutoPtr<IPreference> dozePreferenceTmp;
        FindPreference(CoreUtils::Convert(KEY_DOZE), (IPreference**)&dozePreferenceTmp);
        mDozePreference = ISwitchPreference::Probe(dozePreferenceTmp);
        dozePreferenceTmp->SetOnPreferenceChangeListener(mListener);
    }
    else {
        RemovePreference(KEY_DOZE);
    }

    AutoPtr<IRotationPolicy> rp;
    CRotationPolicy::AcquireSingleton((IRotationPolicy**)&rp);
    if (rp->IsRotationLockToggleVisible(activity, &res), res) {
        AutoPtr<IPreference> rotatePreferenceTmp;
        FindPreference(CoreUtils::Convert(KEY_AUTO_ROTATE), (IPreference**)&rotatePreferenceTmp);
        CDropDownPreference* rotatePreference = (CDropDownPreference*) IDropDownPreference::Probe(rotatePreferenceTmp);
        String str;
        activity->GetString(R::string::display_auto_rotate_rotate, &str);
        rotatePreference->AddItem(str, CoreUtils::Convert(FALSE));
        Int32 rotateLockedResourceId;
        // The following block sets the string used when rotation is locked.
        // If the device locks specifically to portrait or landscape (rather than current
        // rotation), then we use a different string to include this information.
        if (AllowAllRotations(activity)) {
            rotateLockedResourceId = R::string::display_auto_rotate_stay_in_current;
        }
        else {
            Int32 data;
            if ((rp->GetRotationLockOrientation(activity, &data), data)
                    == IConfiguration::ORIENTATION_PORTRAIT) {
                rotateLockedResourceId = R::string::display_auto_rotate_stay_in_portrait;
            }
            else {
                rotateLockedResourceId = R::string::display_auto_rotate_stay_in_landscape;
            }
        }

        activity->GetString(rotateLockedResourceId, &str);
        rotatePreference->AddItem(str, CoreUtils::Convert(TRUE));
        rotatePreference->SetSelectedItem((rp->IsRotationLocked(activity, &res), res) ? 1 : 0);
        AutoPtr<OnCreateCallback> callback = new OnCreateCallback(this, activity);
        rotatePreference->SetCallback(callback);
    }
    else {
        RemovePreference(KEY_AUTO_ROTATE);
    }
    return NOERROR;
}

Boolean CDisplaySettings::AllowAllRotations(
    /* [in] */ IContext* context)
{
    AutoPtr<IResourcesHelper> helper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&helper);

    AutoPtr<IResources> resources;
    helper->GetSystem((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(Elastos::Droid::R::bool_::config_allowAllRotations, &res);
    return res;
}

Boolean CDisplaySettings::IsLiftToWakeAvailable(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&obj);
    ISensorManager* sensors = ISensorManager::Probe(obj);
    AutoPtr<ISensor> ss;
    return sensors != NULL && (sensors->GetDefaultSensor(ISensor::TYPE_WAKE_GESTURE, (ISensor**)&ss), ss != NULL);
}

Boolean CDisplaySettings::IsDozeAvailable(
    /* [in] */ IContext* context)
{
    String name;
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        sp->Get(String("debug.doze.component"), &name);
    }
    if (TextUtils::IsEmpty(name)) {
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        resources->GetString(Elastos::Droid::R::string::config_dozeComponent, &name);
    }
    return !TextUtils::IsEmpty(name);
}

Boolean CDisplaySettings::IsAutomaticBrightnessAvailable(
    /* [in] */ IResources* res)
{
    Boolean result;
    res->GetBoolean(Elastos::Droid::R::bool_::config_automatic_brightness_available, &result);
    return result;
}

void CDisplaySettings::UpdateTimeoutPreferenceDescription(
    /* [in] */ Int64 currentTimeout)
{
    AutoPtr<IListPreference> preference = mScreenTimeoutPreference;
    String summary;
    if (currentTimeout < 0) {
        // Unsupported value
        summary = "";
    }
    else {
        AutoPtr< ArrayOf<ICharSequence*> > entries;
        preference->GetEntries((ArrayOf<ICharSequence*>**)&entries);
        AutoPtr< ArrayOf<ICharSequence*> > values;
        preference->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
        if (entries == NULL || entries->GetLength() == 0) {
            summary = "";
        }
        else {
            Int32 best = 0;
            for (Int32 i = 0; i < values->GetLength(); i++) {
                Int64 timeout = StringUtils::ParseInt64(Object::ToString((*values)[i]));
                if (currentTimeout >= timeout) {
                    best = i;
                }
            }
            AutoPtr<IContext> context;
            IPreference::Probe(preference)->GetContext((IContext**)&context);
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, (*entries)[best]);

            context->GetString(R::string::screen_timeout_summary, args, &summary);
        }
    }
    IPreference::Probe(preference)->SetSummary(CoreUtils::Convert(summary));
}

void CDisplaySettings::DisableUnusableTimeouts(
    /* [in] */ IListPreference* screenTimeoutPreference)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IInterface> obj;
    IContext::Probe(activity)->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    IDevicePolicyManager* dpm = IDevicePolicyManager::Probe(obj);
    Int64 maxTimeout = 0;
    if (dpm != NULL) {
        dpm->GetMaximumTimeToLock(NULL, &maxTimeout);
    }
    if (maxTimeout == 0) {
        return; // policy not enforced
    }

    AutoPtr< ArrayOf<ICharSequence*> > entries;
    screenTimeoutPreference->GetEntries((ArrayOf<ICharSequence*>**)&entries);
    AutoPtr< ArrayOf<ICharSequence*> > values;
    screenTimeoutPreference->GetEntryValues((ArrayOf<ICharSequence*>**)&values);

    AutoPtr<IArrayList> revisedEntries;//ArrayList<CharSequence>
    CArrayList::New((IArrayList**)&revisedEntries);
    AutoPtr<IArrayList> revisedValues;//ArrayList<CharSequence>
    CArrayList::New((IArrayList**)&revisedValues);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Int64 timeout = StringUtils::ParseInt64(Object::ToString((*values)[i]));
        if (timeout <= maxTimeout) {
            revisedEntries->Add((*entries)[i]);
            revisedValues->Add((*values)[i]);
        }
    }
    Int32 size;
    AutoPtr<IInterface> tmp;
    if ((revisedEntries->GetSize(&size), size) != entries->GetLength() ||
            (revisedValues->GetSize(&size), size) != values->GetLength()) {
        String str;
        screenTimeoutPreference->GetValue(&str);
        const Int32 userPreference = StringUtils::ParseInt32(str);

        AutoPtr< ArrayOf<IInterface*> > args1 = ArrayOf<IInterface*>::Alloc((revisedEntries->GetSize(&size), size));
        AutoPtr< ArrayOf<IInterface*> > result1;
        revisedEntries->ToArray(args1, (ArrayOf<IInterface*>**)&result1);

        AutoPtr< ArrayOf<ICharSequence*> > object1 = ArrayOf<ICharSequence*>::Alloc(result1->GetLength());
        for (Int32 i = 0; i < result1->GetLength(); ++i) {
            object1->Set(i, ICharSequence::Probe((*result1)[i]));
        }
        screenTimeoutPreference->SetEntries(object1);

        AutoPtr< ArrayOf<IInterface*> > args2 = ArrayOf<IInterface*>::Alloc((revisedValues->GetSize(&size), size));
        AutoPtr< ArrayOf<IInterface*> > result2;
        revisedValues->ToArray(args2, (ArrayOf<IInterface*>**)&result2);

        AutoPtr< ArrayOf<ICharSequence*> > object2 = ArrayOf<ICharSequence*>::Alloc(result2->GetLength());
        for (Int32 i = 0; i < result2->GetLength(); ++i) {
            object2->Set(i, ICharSequence::Probe((*result2)[i]));
        }
        screenTimeoutPreference->SetEntryValues(object2);

        if (userPreference <= maxTimeout) {
            screenTimeoutPreference->SetValue(StringUtils::ToString(userPreference));
        }
        else if ((revisedValues->GetSize(&size), size) > 0
                && StringUtils::ParseInt64(Object::ToString((revisedValues->Get((revisedValues->GetSize(&size), size - 1), (IInterface**)&tmp), tmp)))
                == maxTimeout) {
            // If the last one happens to be the same as the max timeout, select that
            screenTimeoutPreference->SetValue(StringUtils::ToString(maxTimeout));
        }
        else {
            // There will be no highlighted selection since nothing in the list matches
            // maxTimeout. The user can still select anything less than maxTimeout.
            // TODO: maybe append maxTimeout to the list and mark selected.
        }
    }
    IPreference::Probe(screenTimeoutPreference)->SetEnabled((revisedEntries->GetSize(&size), size > 0));
}

Int32 CDisplaySettings::FloatToIndex(
    /* [in] */ Float val)
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr< ArrayOf<String> > indices;
    resources->GetStringArray(R::array::entryvalues_font_size, (ArrayOf<String>**)&indices);
    Float lastVal = StringUtils::ParseFloat((*indices)[0]);
    for (Int32 i = 1; i < indices->GetLength(); i++) {
        Float thisVal = StringUtils::ParseFloat((*indices)[i]);
        if (val < (lastVal + (thisVal - lastVal)*.5f)) {
            return i-1;
        }
        lastVal = thisVal;
    }
    return indices->GetLength() - 1;
}

ECode CDisplaySettings::ReadFontSizePreference(
    /* [in] */ IListPreference* pref)
{
    AutoPtr<IConfiguration> config;
    ECode ec = ActivityManagerNative::GetDefault()->GetConfiguration((IConfiguration**)&config);
    if (SUCCEEDED(ec)) {
        Int32 data;
        mCurConfig->UpdateFrom(config, &data);
    }
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Unable to retrieve font size");
    }

    // mark the appropriate item in the preferences list
    Float fontScale;
    mCurConfig->GetFontScale(&fontScale);
    Int32 index = FloatToIndex(fontScale);
    pref->SetValueIndex(index);

    // report the current size in the summary text
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr< ArrayOf<String> > fontSizeNames;
    res->GetStringArray(R::array::entries_font_size, (ArrayOf<String>**)&fontSizeNames);
    String str;
    res->GetString(R::string::summary_font_size, &str);

    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert((*fontSizeNames)[index]));
    IPreference::Probe(pref)->SetSummary(CoreUtils::Convert(StringUtils::Format(str, args)));
    return NOERROR;
}

ECode CDisplaySettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    UpdateState();
    return NOERROR;
}

ECode CDisplaySettings::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog);
    *dialog = NULL;

    if (dialogId == DLG_GLOBAL_CHANGE_WARNING) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<MyRunnable> runnable = new MyRunnable(this);
        AutoPtr<IDialog> result = Utils::BuildGlobalChangeWarningDialog(
                IContext::Probe(activity), R::string::global_font_change_title, runnable);

        *dialog = result;
        REFCOUNT_ADD(*dialog)
        return NOERROR;
    }
    return NOERROR;
}

void CDisplaySettings::UpdateState()
{
    ReadFontSizePreference(mFontSizePref);
    UpdateScreenSaverSummary();

    // Update auto brightness if it is available.
    if (mAutoBrightnessPreference != NULL) {
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        Int32 brightnessMode;
        ss->GetInt32(GetContentResolver(), ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL, &brightnessMode);
        ITwoStatePreference::Probe(mAutoBrightnessPreference)->SetChecked(brightnessMode != ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL);
    }

    // Update lift-to-wake if it is available.
    if (mLiftToWakePreference != NULL) {
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Int32 value;
        secure->GetInt32(GetContentResolver(), ISettingsSecure::WAKE_GESTURE_ENABLED, 0, &value);
        ITwoStatePreference::Probe(mLiftToWakePreference)->SetChecked(value != 0);
    }

    // Update doze if it is available.
    if (mDozePreference != NULL) {
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Int32 value;
        secure->GetInt32(GetContentResolver(), ISettingsSecure::DOZE_ENABLED, 1, &value);
        ITwoStatePreference::Probe(mDozePreference)->SetChecked(value != 0);
    }
}

void CDisplaySettings::UpdateScreenSaverSummary()
{
    if (mScreenSaverPreference != NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        mScreenSaverPreference->SetSummary(
                CDreamSettings::GetSummaryTextWithDreamName(IContext::Probe(activity)));
    }
}

ECode CDisplaySettings::WriteFontSizePreference(
    /* [in] */ IInterface* objValue)
{
    mCurConfig->SetFontScale(StringUtils::ParseFloat(Object::ToString(objValue)));
    ECode ec = ActivityManagerNative::GetDefault()->UpdatePersistentConfiguration(mCurConfig);
    if (ec == (ECode) E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Unable to save font size");
    }
    return NOERROR;
}

ECode CDisplaySettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
}

ECode CDisplaySettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* objValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean res;

    String key;
    preference->GetKey(&key);
    if (KEY_SCREEN_TIMEOUT.Equals(key)) {
        Int32 value;
        ECode ec = StringUtils::Parse(Object::ToString(objValue), &value);
        if (SUCCEEDED(ec)) {
            AutoPtr<ISettingsSystem> ss;
            CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
            ss->PutInt32(GetContentResolver(), ISettingsSystem::SCREEN_OFF_TIMEOUT, value, &res);
            UpdateTimeoutPreferenceDescription(value);
        }
        else if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
            Logger::E(TAG, "could not persist screen timeout setting 0x%08x", ec);
        }
    }
    if (KEY_FONT_SIZE.Equals(key)) {
        WriteFontSizePreference(objValue);
    }
    if (preference == IPreference::Probe(mAutoBrightnessPreference)) {
        Boolean value;
        IBoolean::Probe(objValue)->GetValue(&value);
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        ss->PutInt32(GetContentResolver(), ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                value ? ISettingsSystem::SCREEN_BRIGHTNESS_MODE_AUTOMATIC : ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL, &res);
    }
    if (preference == IPreference::Probe(mLiftToWakePreference)) {
        Boolean value;
        IBoolean::Probe(objValue)->GetValue(&value);
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        secure->PutInt32(GetContentResolver(), ISettingsSecure::WAKE_GESTURE_ENABLED, value ? 1 : 0, &res);
    }
    if (preference == IPreference::Probe(mDozePreference)) {
        Boolean value;
        IBoolean::Probe(objValue)->GetValue(&value);
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        secure->PutInt32(GetContentResolver(), ISettingsSecure::DOZE_ENABLED, value ? 1 : 0, &res);
    }
    *result = TRUE;
    return NOERROR;
}

ECode CDisplaySettings::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (preference == (IPreference*)mFontSizePref.Get()) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        if (Utils::HasMultipleUsers(IContext::Probe(activity))) {
            ShowDialog(DLG_GLOBAL_CHANGE_WARNING);
            *result = TRUE;
            return NOERROR;
        }
        else {
            mFontSizePref->Click();
        }
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos