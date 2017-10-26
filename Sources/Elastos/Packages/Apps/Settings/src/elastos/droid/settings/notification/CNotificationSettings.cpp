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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/notification/CNotificationSettings.h"
#include "elastos/droid/settings/notification/CNotificationAccessSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Looper.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Media::IRingtoneManagerHelper;
using Elastos::Droid::Media::CRingtoneManagerHelper;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ISeekBarPreference;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IOpenableColumns;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Core::CoreUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

//===============================================================================
//                  CNotificationSettings::MyBaseSearchIndexProvider
//===============================================================================

CNotificationSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CNotificationSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode CNotificationSettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::notification_settings);
    AutoPtr< ArrayOf<ISearchIndexableResource*> > args = ArrayOf<ISearchIndexableResource*>::Alloc(1);
    args->Set(0, sir);

    return Arrays::AsList(args, result);
}

ECode CNotificationSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IArrayList> rt;
    CArrayList::New((IArrayList**)&rt);
    if (Utils::IsVoiceCapable(context)) {
        rt->Add(CoreUtils::Convert(KEY_NOTIFICATION_VOLUME));
    }
    else {
        rt->Add(CoreUtils::Convert(KEY_RING_VOLUME));
        rt->Add(CoreUtils::Convert(KEY_PHONE_RINGTONE));
        rt->Add(CoreUtils::Convert(KEY_VIBRATE_WHEN_RINGING));
    }
    *result = IList::Probe(rt);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                  CNotificationSettings::VolumePreferenceCallback
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationSettings::VolumePreferenceCallback, Object, IVolumeSeekBarPreferenceCallback)

CNotificationSettings::VolumePreferenceCallback::VolumePreferenceCallback(
    /* [in] */ CNotificationSettings* host)
    : mHost(host)
{}

ECode CNotificationSettings::VolumePreferenceCallback::OnSampleStarting(
    /* [in] */ ISeekBarVolumizer* sbv)
{
    if (mCurrent != NULL && mCurrent.Get() != sbv) {
        mCurrent->StopSample();
    }
    mCurrent = sbv;
    if (mCurrent != NULL) {
        mHost->mHandler->RemoveMessages(H::STOP_SAMPLE);
        Boolean res;
        mHost->mHandler->SendEmptyMessageDelayed(H::STOP_SAMPLE, SAMPLE_CUTOFF, &res);
    }
    return NOERROR;
}

ECode CNotificationSettings::VolumePreferenceCallback::OnStreamValueChanged(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 progress)
{
    if (stream == IAudioManager::STREAM_RING) {
        mHost->mHandler->RemoveMessages(H::UPDATE_RINGER_ICON);
        AutoPtr<IMessage> message;
        mHost->mHandler->ObtainMessage(H::UPDATE_RINGER_ICON, progress, 0, (IMessage**)&message);
        message->SendToTarget();
    }
    return NOERROR;
}

ECode CNotificationSettings::VolumePreferenceCallback::StopSample()
{
    if (mCurrent != NULL) {
        mCurrent->StopSample();
    }
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings::SettingsObserver
//===============================================================================

CNotificationSettings::SettingsObserver::SettingsObserver(
    /* [in] */ CNotificationSettings* host)
    : mHost(host)
{}

ECode CNotificationSettings::SettingsObserver::constructor()
{
    ContentObserver::constructor(mHost->mHandler);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    sys->GetUriFor(ISettingsSystem::VIBRATE_WHEN_RINGING,
            (IUri**)&VIBRATE_WHEN_RINGING_URI);
    sys->GetUriFor(ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
            (IUri**)&NOTIFICATION_LIGHT_PULSE_URI);

    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    ss->GetUriFor(ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS,
            (IUri**)&LOCK_SCREEN_PRIVATE_URI);
    ss->GetUriFor(ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS,
            (IUri**)&LOCK_SCREEN_SHOW_URI);
    return NOERROR;
}

ECode CNotificationSettings::SettingsObserver::Register(
    /* [in] */ Boolean _register)
{
    AutoPtr<IContentResolver> cr = mHost->GetContentResolver();
    if (_register) {
        cr->RegisterContentObserver(VIBRATE_WHEN_RINGING_URI, FALSE, this);
        cr->RegisterContentObserver(NOTIFICATION_LIGHT_PULSE_URI, FALSE, this);
        cr->RegisterContentObserver(LOCK_SCREEN_PRIVATE_URI, FALSE, this);
        cr->RegisterContentObserver(LOCK_SCREEN_SHOW_URI, FALSE, this);
    }
    else {
        cr->UnregisterContentObserver(this);
    }
    return NOERROR;
}

ECode CNotificationSettings::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    ContentObserver::OnChange(selfChange, uri);
    Boolean res;
    if (IObject::Probe(VIBRATE_WHEN_RINGING_URI)->Equals(uri, &res), res) {
        mHost->UpdateVibrateWhenRinging();
    }
    if (IObject::Probe(NOTIFICATION_LIGHT_PULSE_URI)->Equals(uri, &res), res) {
        mHost->UpdatePulse();
    }
    if ((IObject::Probe(LOCK_SCREEN_PRIVATE_URI)->Equals(uri, &res), res) ||
            (IObject::Probe(LOCK_SCREEN_SHOW_URI)->Equals(uri, &res), res)) {
        mHost->UpdateLockscreenNotifications();
    }
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings::H
//===============================================================================

const Int32 CNotificationSettings::H::UPDATE_PHONE_RINGTONE;
const Int32 CNotificationSettings::H::UPDATE_NOTIFICATION_RINGTONE;
const Int32 CNotificationSettings::H::STOP_SAMPLE;
const Int32 CNotificationSettings::H::UPDATE_RINGER_ICON;

ECode CNotificationSettings::H::constructor(
    /* [in] */ CNotificationSettings* host)
{
    mHost = host;
    return Handler::constructor(Looper::GetMainLooper());
}

ECode CNotificationSettings::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case UPDATE_PHONE_RINGTONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mPhoneRingtonePreference->SetSummary(ICharSequence::Probe(obj));
            break;
        }
        case UPDATE_NOTIFICATION_RINGTONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->mNotificationRingtonePreference->SetSummary(ICharSequence::Probe(obj));
            break;
        }
        case STOP_SAMPLE:
            mHost->mVolumeCallback->StopSample();
            break;
        case UPDATE_RINGER_ICON: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->UpdateRingOrNotificationIcon(arg1);
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings::MyRunnable
//===============================================================================

CNotificationSettings::MyRunnable::MyRunnable(
    /* [in] */ CNotificationSettings* host)
    : mHost(host)
{}

ECode CNotificationSettings::MyRunnable::Run()
{
    if (mHost->mPhoneRingtonePreference != NULL) {
        AutoPtr<ICharSequence> summary = mHost->UpdateRingtoneName(
                mHost->mContext, IRingtoneManager::TYPE_RINGTONE);
        if (summary != NULL) {
            AutoPtr<IMessage> message;
            mHost->mHandler->ObtainMessage(H::UPDATE_PHONE_RINGTONE, summary, (IMessage**)&message);
            message->SendToTarget();
        }
    }
    if (mHost->mNotificationRingtonePreference != NULL) {
        AutoPtr<ICharSequence> summary = mHost->UpdateRingtoneName(
                mHost->mContext, IRingtoneManager::TYPE_NOTIFICATION);
        if (summary != NULL) {
            AutoPtr<IMessage> message;
            mHost->mHandler->ObtainMessage(H::UPDATE_NOTIFICATION_RINGTONE, summary, (IMessage**)&message);
            message->SendToTarget();
        }
    }
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings::MyOnPreferenceChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationSettings::MyOnPreferenceChangeListener, Object, IPreferenceOnPreferenceChangeListener)

CNotificationSettings::MyOnPreferenceChangeListener::MyOnPreferenceChangeListener(
    /* [in] */ CNotificationSettings* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

ECode CNotificationSettings::MyOnPreferenceChangeListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean val;
    IBoolean::Probe(newValue)->GetValue(&val);
    AutoPtr<IContentResolver> resolver = mHost->GetContentResolver();
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);

    switch (mId) {
        case 0:
            return ss->PutInt32(resolver, ISettingsSystem::VIBRATE_WHEN_RINGING,
                val ? 1 : 0, result);
        case 1:
            return ss->PutInt32(resolver, ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
                    val ? 1 : 0, result);
    }
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings::DropDownPreferenceCallback
//===============================================================================

CAR_INTERFACE_IMPL(CNotificationSettings::DropDownPreferenceCallback, Object, IDropDownPreferenceCallback)

CNotificationSettings::DropDownPreferenceCallback::DropDownPreferenceCallback(
    /* [in] */ CNotificationSettings* host)
    : mHost(host)
{}

ECode CNotificationSettings::DropDownPreferenceCallback::OnItemSelected(
    /* [in] */ Int32 pos,
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 val;
    IInteger32::Probe(value)->GetValue(&val);
    if (val == mHost->mLockscreenSelectedValue) {
        *result = TRUE;
        return NOERROR;
    }
    const Boolean enabled = val != R::string::lock_screen_notifications_summary_disable;
    const Boolean show = val == R::string::lock_screen_notifications_summary_show;
    AutoPtr<IContentResolver> resolver = mHost->GetContentResolver();
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(resolver,
            ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, show ? 1 : 0, &res);
    secure->PutInt32(resolver,
            ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, enabled ? 1 : 0, &res);
    mHost->mLockscreenSelectedValue = val;
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                 CNotificationSettings
//===============================================================================

AutoPtr<IIndexableSearchIndexProvider> CNotificationSettings::SEARCH_INDEX_DATA_PROVIDER;

const String CNotificationSettings::TAG("NotificationSettings");

const String CNotificationSettings::KEY_SOUND("sound");
const String CNotificationSettings::KEY_MEDIA_VOLUME("media_volume");
const String CNotificationSettings::KEY_ALARM_VOLUME("alarm_volume");
const String CNotificationSettings::KEY_RING_VOLUME("ring_volume");
const String CNotificationSettings::KEY_NOTIFICATION_VOLUME("notification_volume");
const String CNotificationSettings::KEY_PHONE_RINGTONE("ringtone");
const String CNotificationSettings::KEY_NOTIFICATION_RINGTONE("notification_ringtone");
const String CNotificationSettings::KEY_VIBRATE_WHEN_RINGING("vibrate_when_ringing");
const String CNotificationSettings::KEY_NOTIFICATION("notification");
const String CNotificationSettings::KEY_NOTIFICATION_PULSE("notification_pulse");
const String CNotificationSettings::KEY_LOCK_SCREEN_NOTIFICATIONS("lock_screen_notifications");
const String CNotificationSettings::KEY_NOTIFICATION_ACCESS("manage_notification_access");

const Int32 CNotificationSettings::SAMPLE_CUTOFF = 2000;

AutoPtr<IIndexableSearchIndexProvider> CNotificationSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CNotificationSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL(CNotificationSettings, SettingsPreferenceFragment, IIndexable)

CAR_OBJECT_IMPL(CNotificationSettings);

CNotificationSettings::CNotificationSettings()
    : mVoiceCapable(FALSE)
    , mSecure(FALSE)
    , mLockscreenSelectedValue(0)
{
    mVolumeCallback = new VolumePreferenceCallback(this);
    mHandler = new H();
    mHandler->constructor(this);
    mSettingsObserver = new SettingsObserver(this);
    mSettingsObserver->constructor();
    mLookupRingtoneNames = new MyRunnable(this);
}

CNotificationSettings::~CNotificationSettings()
{}

ECode CNotificationSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode CNotificationSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    mContext = IContext::Probe(activity);
    mContext->GetPackageManager((IPackageManager**)&mPM);
    mVoiceCapable = Utils::IsVoiceCapable(mContext);

    AutoPtr<ILockPatternUtils> lpUtils;
    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&lpUtils);
    lpUtils->IsSecure(&mSecure);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
    mVibrator = IVibrator::Probe(obj);
    Boolean res;
    if (mVibrator != NULL && (mVibrator->HasVibrator(&res), !res)) {
        mVibrator = NULL;
    }

    AddPreferencesFromResource(R::xml::notification_settings);
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(KEY_SOUND), (IPreference**)&pref);
    AutoPtr<IPreferenceCategory> sound = IPreferenceCategory::Probe(pref);
    InitVolumePreference(KEY_MEDIA_VOLUME, IAudioManager::STREAM_MUSIC);
    InitVolumePreference(KEY_ALARM_VOLUME, IAudioManager::STREAM_ALARM);
    AutoPtr<IPreferenceGroup> _sound = IPreferenceGroup::Probe(pref);
    if (mVoiceCapable) {
        mRingOrNotificationPreference =
                InitVolumePreference(KEY_RING_VOLUME, IAudioManager::STREAM_RING);
        AutoPtr<IPreference> preference;
        _sound->FindPreference(CoreUtils::Convert(KEY_NOTIFICATION_VOLUME), (IPreference**)&preference);
        _sound->RemovePreference(preference, &res);
    }
    else {
        mRingOrNotificationPreference =
                InitVolumePreference(KEY_NOTIFICATION_VOLUME, IAudioManager::STREAM_NOTIFICATION);
        AutoPtr<IPreference> preference;
        _sound->FindPreference(CoreUtils::Convert(KEY_RING_VOLUME), (IPreference**)&preference);
        _sound->RemovePreference(preference, &res);
    }
    InitRingtones(sound);
    InitVibrateWhenRinging(sound);

    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_NOTIFICATION), (IPreference**)&pref);
    AutoPtr<IPreferenceCategory> notification = IPreferenceCategory::Probe(pref);
    InitPulse(notification);
    InitLockscreenNotifications(notification);

    FindPreference(CoreUtils::Convert(KEY_NOTIFICATION_ACCESS), (IPreference**)&mNotificationAccess);
    RefreshNotificationListeners();
    return NOERROR;
}

ECode CNotificationSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    RefreshNotificationListeners();
    LookupRingtoneNames();
    mSettingsObserver->Register(TRUE);
    return NOERROR;
}

ECode CNotificationSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();
    mVolumeCallback->StopSample();
    mSettingsObserver->Register(FALSE);
    return NOERROR;
}

AutoPtr<CVolumeSeekBarPreference> CNotificationSettings::InitVolumePreference(
    /* [in] */ const String& key,
    /* [in] */ Int32 stream)
{
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(key), (IPreference**)&pref);
    AutoPtr<CVolumeSeekBarPreference> volumePref = (CVolumeSeekBarPreference*)ISeekBarPreference::Probe(pref);
    volumePref->SetCallback(mVolumeCallback);
    volumePref->SetStream(stream);
    return volumePref;
}

void CNotificationSettings::UpdateRingOrNotificationIcon(
    /* [in] */ Int32 progress)
{
    mRingOrNotificationPreference->ShowIcon(progress > 0
                ? R::drawable::ring_notif
                : (mVibrator == NULL
                        ? R::drawable::ring_notif_mute
                        : R::drawable::ring_notif_vibrate));
}

void CNotificationSettings::InitRingtones(
    /* [in] */ IPreferenceCategory* root)
{
    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    _root->FindPreference(CoreUtils::Convert(KEY_PHONE_RINGTONE), (IPreference**)&mPhoneRingtonePreference);
    if (mPhoneRingtonePreference != NULL && !mVoiceCapable) {
        Boolean res;
        _root->RemovePreference(mPhoneRingtonePreference, &res);
        mPhoneRingtonePreference = NULL;
    }
    _root->FindPreference(CoreUtils::Convert(KEY_NOTIFICATION_RINGTONE), (IPreference**)&mNotificationRingtonePreference);
}

void CNotificationSettings::LookupRingtoneNames()
{
    AsyncTask::Execute(mLookupRingtoneNames);
}

AutoPtr<ICharSequence> CNotificationSettings::UpdateRingtoneName(
    /* [in] */ IContext* context,
    /* [in] */ Int32 type)
{
    if (context == NULL) {
        Logger::E(TAG, "Unable to update ringtone name, no context provided");
        return NULL;
    }
    AutoPtr<IRingtoneManagerHelper> helper;
    CRingtoneManagerHelper::AcquireSingleton((IRingtoneManagerHelper**)&helper);
    AutoPtr<IUri> ringtoneUri;
    helper->GetActualDefaultRingtoneUri(context, type, (IUri**)&ringtoneUri);
    String str;
    context->GetString(Elastos::Droid::R::string::ringtone_unknown, &str);
    AutoPtr<ICharSequence> summary = CoreUtils::Convert(str);
    // Is it a silent ringtone?
    if (ringtoneUri == NULL) {
        context->GetString(Elastos::Droid::R::string::ringtone_silent, &str);
        summary = CoreUtils::Convert(str);
    }
    else {
        AutoPtr<ICursor> cursor;
        // try {
            String authority;
            ringtoneUri->GetAuthority(&authority);
            AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);

            String scheme;
            ECode ec = NOERROR;
            if (IMediaStore::AUTHORITY.Equals(authority)) {
                // Fetch the ringtone title from the media provider
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);

                (*args)[0] = IMediaStoreMediaColumns::TITLE;
                ec = resolver->Query(ringtoneUri, args, String(NULL), NULL,
                        String(NULL), (ICursor**)&cursor);
                if (FAILED(ec)) {
                    if (cursor != NULL) {
                        ICloseable::Probe(cursor)->Close();
                    }
                }
            }
            else if (IContentResolver::SCHEME_CONTENT.Equals((ringtoneUri->GetScheme(&scheme), scheme))) {
                AutoPtr<IContentResolver> resolver;
                context->GetContentResolver((IContentResolver**)&resolver);

                (*args)[0] = IOpenableColumns::DISPLAY_NAME;
                ec = resolver->Query(ringtoneUri, args, String(NULL), NULL,
                        String(NULL), (ICursor**)&cursor);
                if (FAILED(ec)) {
                    if (cursor != NULL) {
                        ICloseable::Probe(cursor)->Close();
                    }
                }
            }
            if (cursor != NULL) {
                Boolean res;
                if (cursor->MoveToFirst(&res), res) {
                    cursor->GetString(0, &str);
                    summary = CoreUtils::Convert(str);
                }
            }
        // } catch (SQLiteException sqle) {
        //     // Unknown title for the ringtone
        // } catch (IllegalArgumentException iae) {
        //     // Some other error retrieving the column from the provider
        // } finally {
            if (cursor != NULL) {
                ICloseable::Probe(cursor)->Close();
            }
        // }
    }
    return summary;
}

void CNotificationSettings::InitVibrateWhenRinging(
    /* [in] */ IPreferenceCategory* root)
{
    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    AutoPtr<IPreference> preference;
    _root->FindPreference(CoreUtils::Convert(KEY_VIBRATE_WHEN_RINGING), (IPreference**)&preference);
    mVibrateWhenRinging = ITwoStatePreference::Probe(preference);
    if (mVibrateWhenRinging == NULL) {
        Logger::I(TAG, "Preference not found: %s", KEY_VIBRATE_WHEN_RINGING.string());
        return;
    }
    Boolean res;
    if (!mVoiceCapable) {
        _root->RemovePreference(preference, &res);
        mVibrateWhenRinging = NULL;
        return;
    }
    preference->SetPersistent(FALSE);
    UpdateVibrateWhenRinging();
    AutoPtr<MyOnPreferenceChangeListener> listener = new MyOnPreferenceChangeListener(this, 0);
    preference->SetOnPreferenceChangeListener(listener);
}

void CNotificationSettings::UpdateVibrateWhenRinging()
{
    if (mVibrateWhenRinging == NULL) return;
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    Int32 data;
    ss->GetInt32(GetContentResolver(),
            ISettingsSystem::VIBRATE_WHEN_RINGING, 0, &data);
    mVibrateWhenRinging->SetChecked(data != 0);
}

void CNotificationSettings::InitPulse(
    /* [in] */ IPreferenceCategory* parent)
{
    IPreferenceGroup* _parent = IPreferenceGroup::Probe(parent);
    AutoPtr<IPreference> preference;
    _parent->FindPreference(CoreUtils::Convert(KEY_NOTIFICATION_PULSE), (IPreference**)&preference);
    mNotificationPulse = ITwoStatePreference::Probe(preference);
    if (mNotificationPulse == NULL) {
        Logger::I(TAG, "Preference not found: %s", KEY_NOTIFICATION_PULSE.string());
        return;
    }

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Boolean res;
    if (resources->GetBoolean(Elastos::Droid::R::bool_::config_intrusiveNotificationLed, &res), !res) {
        _parent->RemovePreference(preference, &res);
    }
    else {
        UpdatePulse();
        AutoPtr<MyOnPreferenceChangeListener> listener = new MyOnPreferenceChangeListener(this, 1);
        preference->SetOnPreferenceChangeListener(listener);
    }
}

void CNotificationSettings::UpdatePulse()
{
    if (mNotificationPulse == NULL) {
        return;
    }
    // try {
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        Int32 data;
        ECode ec = ss->GetInt32(GetContentResolver(), ISettingsSystem::NOTIFICATION_LIGHT_PULSE, &data);
        if (FAILED(ec)) {
        // } catch (Settings.SettingNotFoundException snfe) {
            Logger::E(TAG, "%s not found", ISettingsSystem::NOTIFICATION_LIGHT_PULSE.string());
        // }
        }
        else {
            mNotificationPulse->SetChecked(data == 1);
        }
}

void CNotificationSettings::InitLockscreenNotifications(
    /* [in] */ IPreferenceCategory* parent)
{
    AutoPtr<IPreference> preference;
    IPreferenceGroup::Probe(parent)->FindPreference(CoreUtils::Convert(KEY_LOCK_SCREEN_NOTIFICATIONS), (IPreference**)&preference);
    mLockscreen = (CDropDownPreference*) IDropDownPreference::Probe(preference);
    if (mLockscreen == NULL) {
        Logger::I(TAG, "Preference not found: %s", KEY_LOCK_SCREEN_NOTIFICATIONS.string());
        return;
    }

    mLockscreen->AddItem(R::string::lock_screen_notifications_summary_show,
            CoreUtils::Convert(R::string::lock_screen_notifications_summary_show));
    if (mSecure) {
        mLockscreen->AddItem(R::string::lock_screen_notifications_summary_hide,
                CoreUtils::Convert(R::string::lock_screen_notifications_summary_hide));
    }
    mLockscreen->AddItem(R::string::lock_screen_notifications_summary_disable,
            CoreUtils::Convert(R::string::lock_screen_notifications_summary_disable));
    UpdateLockscreenNotifications();
    AutoPtr<DropDownPreferenceCallback> callback = new DropDownPreferenceCallback(this);
    mLockscreen->SetCallback(callback);
}

void CNotificationSettings::UpdateLockscreenNotifications()
{
    if (mLockscreen == NULL) {
        return;
    }
    const Boolean enabled = GetLockscreenNotificationsEnabled();
    const Boolean allowPrivate = !mSecure || GetLockscreenAllowPrivateNotifications();
    mLockscreenSelectedValue = !enabled ? R::string::lock_screen_notifications_summary_disable :
            allowPrivate ? R::string::lock_screen_notifications_summary_show :
            R::string::lock_screen_notifications_summary_hide;
    mLockscreen->SetSelectedValue(CoreUtils::Convert(mLockscreenSelectedValue));
}

Boolean CNotificationSettings::GetLockscreenNotificationsEnabled()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_SHOW_NOTIFICATIONS, 0, &data);
    return data != 0;
}

Boolean CNotificationSettings::GetLockscreenAllowPrivateNotifications()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 0, &data);
    return data != 0;
}

void CNotificationSettings::RefreshNotificationListeners()
{
    if (mNotificationAccess != NULL) {
        Int32 total = CNotificationAccessSettings::GetListenersCount(mPM);
        if (total == 0) {
            AutoPtr<IPreferenceScreen> screen;
            GetPreferenceScreen((IPreferenceScreen**)&screen);
            Boolean res;
            IPreferenceGroup::Probe(screen)->RemovePreference(mNotificationAccess, &res);
        }
        else {
            Int32 n = CNotificationAccessSettings::GetEnabledListenersCount(mContext);
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            if (n == 0) {
                String str;
                resources->GetString(
                        R::string::manage_notification_access_summary_zero, &str);
                mNotificationAccess->SetSummary(CoreUtils::Convert(str));
            }
            else {
                AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
                args->Set(0, CoreUtils::Convert(n));
                String result;
                resources->GetQuantityString(
                        R::plurals::manage_notification_access_summary_nonzero,
                        n, args, &result);
                String str("");
                str.AppendFormat(result);
                mNotificationAccess->SetSummary(CoreUtils::Convert(str));
            }
        }
    }
}

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos