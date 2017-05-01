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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Content.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/ContentResolver.h"
#include "elastos/droid/content/Intent.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Config.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/regex/Pattern.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::ContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Intent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Widget::IILockSettings;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBaseBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::Pattern;


namespace Elastos {
namespace Droid {
namespace Provider {

INIT_PROI_3 const String Settings::JID_RESOURCE_PREFIX("android");
INIT_PROI_3 const String Settings::TAG("Settings");
const Boolean Settings::LOCAL_LOGV = FALSE;
INIT_PROI_3 Object Settings::sLocationSettingsLock;

//================================================================================
//              Settings::NameValueCache
//================================================================================
INIT_PROI_3 Object Settings::NameValueCache::sLock;

static AutoPtr< ArrayOf<String> > InitSelectValue()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = ISettingsNameValueTable::VALUE;
    return array;
}

INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::NameValueCache::SELECT_VALUE = InitSelectValue();

INIT_PROI_3 const String Settings::NameValueCache::NAME_EQ_PLACEHOLDER("name=?");

Boolean Settings::NameValueTable::PutString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* uri,
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    // The database will take care of replacing duplicates.
    //try {
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    assert(values != NULL);

    AutoPtr<ICharSequence> tmp;
    CString::New(name, (ICharSequence**)&tmp);
    values->Put(String("name"), tmp);
    tmp = NULL;
    CString::New(value, (ICharSequence**)&tmp);
    values->Put(String("value"), tmp);
    AutoPtr<IUri> outUri;
    resolver->Insert(uri, values,(IUri**)&outUri);
    return TRUE;
    //} catch (/*SQLException e*/...) {
    //    Slogger::W(CSettings::TAG, StringBuffer("Can't set key ") + name + StringBuffer(" in ") + uri);
    //    return FALSE;
    //  }
}

ECode Settings::NameValueTable::GetUriFor(
    /* [in] */ IUri* uri,
    /* [in] */ const String& name,
    /* [out] */ IUri** value)
{
    return Uri::WithAppendedPath(uri, name, value);
}

Settings::NameValueCache::NameValueCache(
    /* [in] */ const String& versionSystemProperty,
    /* [in] */ IUri* uri,
    /* [in] */ const String& getCommand,
    /* [in] */ const String& setCommand)
    : mVersionSystemProperty(versionSystemProperty)
    , mUri(uri)
    , mValuesVersion(0)
    , mContentProvider(NULL)
    , mCallGetCommand(getCommand)
    , mCallSetCommand(setCommand)
{
    CHashMap::New((IHashMap**)&mValues);
}

ECode Settings::NameValueCache::LazyGetProvider(
    /* [in] */ IContentResolver* cr,
    /* [out] */ IIContentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    *provider = NULL;

    AutoPtr<IIContentProvider> cp;
    {
        AutoLock syncLock(this);
        cp = mContentProvider;
        if (cp == NULL) {
            String authority;
            mUri->GetAuthority(&authority);
            FAIL_RETURN(cr->AcquireProvider(authority, (IIContentProvider**)&mContentProvider));
            cp = mContentProvider;
        }
    }

    *provider = cp;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode Settings::NameValueCache::PutStringForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // try {
    AutoPtr<IBundle> arg, tmp;
    CBundle::New((IBundle**)&arg);
    arg->PutString(String("value"), value);
    arg->PutInt32(ISettings::CALL_METHOD_USER_KEY, userHandle);
    AutoPtr<IIContentProvider> cp;
    ECode ec = LazyGetProvider(cr, (IIContentProvider**)&cp);
    if (FAILED(ec) || cp == NULL) {
        Slogger::W(TAG, "Can't set key %s in %s", name.string(), TO_CSTR(mUri));
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }

    String package;
    cr->GetPackageName(&package);
    ec = cp->Call(package, mCallSetCommand, name, arg, (IBundle**)&tmp);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Can't set key %s in %s", name.string(), TO_CSTR(mUri));
        *result = FALSE;
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    // } catch (RemoteException e) {
    //     Log.w(TAG, "Can't set key " + name + " in " + mUri, e);
    //     return false;
    // }
    *result = TRUE;
    return NOERROR;
}

ECode Settings::NameValueCache::GetStringForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    Boolean isSelf = (userHandle == UserHandle::GetMyUserId());
    if (isSelf) {
        Int64 newValuesVersion;
        SystemProperties::GetInt64(mVersionSystemProperty, 0, &newValuesVersion);

        // Our own user's settings data uses a client-side cache
        {
            AutoLock syncLock(this);
            if (mValuesVersion != newValuesVersion) {
                if (LOCAL_LOGV || FALSE) {
                    String segment;
                    mUri->GetLastPathSegment(&segment);
                    Slogger::V(TAG, "invalidate [%s]: current %d != cached %d",
                        segment.string(), newValuesVersion, mValuesVersion);
                }

                mValues->Clear();
                mValuesVersion = newValuesVersion;
            }

            Boolean flag = FALSE;
            mValues->ContainsKey(CoreUtils::Convert(name), &flag);
            if (flag) {
                AutoPtr<IInterface> interf;
                mValues->Get(CoreUtils::Convert(name), (IInterface**)&interf);
                ICharSequence* cs = ICharSequence::Probe(interf);
                return cs != NULL ? cs->ToString(value) : NOERROR; // Could be null, that's OK -- negative caching
            }
        }
    } else {
        if (LOCAL_LOGV) {
            Slogger::V(TAG, "get setting for user %d by user %d so skipping cache",
                userHandle, UserHandle::GetMyUserId());
        }
    }

    AutoPtr<IIContentProvider> cp;
    ECode ec = LazyGetProvider(cr, (IIContentProvider**)&cp);
    if (FAILED(ec) || cp == NULL) {
        Slogger::W(TAG, "Can't get key %s from %s", name.string(), TO_CSTR(mUri));
        return ec;
    }

    // Try the fast path first, not using query().  If this
    // fails (alternate Settings provider that doesn't support
    // this interface?) then we fall back to the query/table
    // interface.
    if (mCallGetCommand != NULL) {
        // try {
        AutoPtr<IBundle> args;
        if (!isSelf) {
            CBundle::New((IBundle**)&args);
            args->PutInt32(ISettings::CALL_METHOD_USER_KEY, userHandle);
        }
        String package;
        cr->GetPackageName(&package);
        AutoPtr<IBundle> b;
        cp->Call(package, mCallGetCommand, name, args, (IBundle**)&b);
        if (b != NULL) {
            String _value;
            IBaseBundle::Probe(b)->GetPairValue(&_value);
            // Don't update our cache for reads of other users' data
            if (isSelf) {
                AutoLock lock(sLock);
                mValues->Put(CoreUtils::Convert(name), CoreUtils::Convert(_value));
            }
            else {
                if (LOCAL_LOGV)
                    Slogger::I(TAG, "call-query of user %d by %d so not updating cache",
                            userHandle, UserHandle::GetMyUserId());
            }
            *value = _value;
            return NOERROR;
        }
        // If the response Bundle is null, we fall through
        // to the query interface below.
        // } catch (RemoteException e) {
        //     // Not supported by the remote side?  Fall through
        //     // to query().
        // }
    }

    AutoPtr<ICursor> c;
    // try {
    AutoPtr< ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = name;

    String package;
    cr->GetPackageName(&package);
    ec = cp->Query(package, mUri, SELECT_VALUE,
        NAME_EQ_PLACEHOLDER, selectionArgs, String(NULL), NULL, (ICursor**)&c);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Can't get key %s from %s", name.string(), TO_CSTR(mUri));
        if (c != NULL) {
            ICloseable::Probe(c)->Close();
        }
        // Return null, but don't cache it.
        return NOERROR;
    }
    if (c == NULL) {
        Slogger::W(TAG, "Can't get key %s from %s", name.string(), TO_CSTR(mUri));
        return NOERROR;
    }

    String _value;
    Boolean succeeded = FALSE;
    c->MoveToNext(&succeeded);
    if (succeeded) {
        c->GetString(0, &_value);
    }
    {
        AutoLock lock(sLock);
        mValues->Put((CoreUtils::Convert(name)).Get(), (CoreUtils::Convert(_value)).Get());
    }
    if (LOCAL_LOGV) {
        String segment;
        mUri->GetLastPathSegment(&segment);
        Slogger::V(TAG, "cache miss [%s]: %s = %s", segment.string(), name.string(), _value.string());
    }
    if (c != NULL) {
        ICloseable::Probe(c)->Close();
    }

    *value = _value;
    return NOERROR;
}

//================================================================================
//              Settings::System
//===============================================================================

static AutoPtr< ArrayOf<String> > InitVOLUMESETTINGS()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(7);
    (*array)[0] = ISettingsSystem::VOLUME_VOICE;
    (*array)[1] = ISettingsSystem::VOLUME_SYSTEM;
    (*array)[2] = ISettingsSystem::VOLUME_RING;
    (*array)[3] = ISettingsSystem::VOLUME_MUSIC;
    (*array)[4] = ISettingsSystem::VOLUME_ALARM;
    (*array)[5] = ISettingsSystem::VOLUME_NOTIFICATION;
    (*array)[6] = ISettingsSystem::VOLUME_BLUETOOTH_SCO;
    return array;
}
INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::System::VOLUME_SETTINGS = InitVOLUMESETTINGS();

static AutoPtr<IUri> InitSystemCONTENTURI()
{
    StringBuilder builder;
    builder += "content://";
    builder += ISettings::AUTHORITY;
    builder += "/system";
    String str = builder.ToString();
    AutoPtr<IUri> uri;
    Uri::Parse(str, (IUri**)&uri);
    return uri;
}
INIT_PROI_3 const AutoPtr<IUri> Settings::System::CONTENT_URI = InitSystemCONTENTURI();

INIT_PROI_3 const AutoPtr<Settings::NameValueCache> Settings::System::sNameValueCache
    = new Settings::NameValueCache(
        ISettingsSystem::SYS_PROP_SETTING_VERSION, InitSystemCONTENTURI(),
        ISettings::CALL_METHOD_GET_SYSTEM, ISettings::CALL_METHOD_PUT_SYSTEM);

static AutoPtr<IHashSet> InitSystemMOVED_TO_SECURE()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New(44, (IHashSet**)&hs);

    hs->Add(CoreUtils::Convert(ISettingsSecure::ANDROID_ID));
    hs->Add(CoreUtils::Convert(ISettingsSecure::HTTP_PROXY));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCATION_PROVIDERS_ALLOWED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_BIOMETRIC_WEAK_FLAGS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_VISIBLE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_TACTILE_FEEDBACK_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_SIZE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_DOTS_VISIBLE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_SHOW_ERROR_PATH));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOGGING_ID));
    hs->Add(CoreUtils::Convert(ISettingsSecure::PARENTAL_CONTROL_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::PARENTAL_CONTROL_LAST_UPDATE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::PARENTAL_CONTROL_REDIRECT_URL));
    hs->Add(CoreUtils::Convert(ISettingsSecure::SETTINGS_CLASSNAME));
    hs->Add(CoreUtils::Convert(ISettingsSecure::USE_GOOGLE_MAIL));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_NUM_OPEN_NETWORKS_KEPT));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_ON));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_ACCEPTABLE_PACKET_LOSS_PERCENTAGE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_AP_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_DELAY_MS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_BACKGROUND_CHECK_TIMEOUT_MS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_INITIAL_IGNORED_PING_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_MAX_AP_CHECKS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_ON));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_PING_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_PING_DELAY_MS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::WIFI_WATCHDOG_PING_TIMEOUT_MS));

    // At one time in System, then Global, but now back in Secure
    hs->Add(CoreUtils::Convert(ISettingsSecure::INSTALL_NON_MARKET_APPS));

    /* CM12 CHANGES */
    hs->Add(CoreUtils::Convert(ISettingsSecure::STATS_COLLECTION));
    hs->Add(CoreUtils::Convert(ISettingsSecure::VOLUME_LINK_NOTIFICATION));
    hs->Add(CoreUtils::Convert(ISettingsSecure::QS_TILES));
    hs->Add(CoreUtils::Convert(ISettingsSecure::QS_USE_MAIN_TILES));
    hs->Add(CoreUtils::Convert(ISettingsSecure::QS_SHOW_BRIGHTNESS_SLIDER));

    /* Settings::Secure::NAVIGATION_RING_TARGETS */
    hs->Add(CoreUtils::Convert("navigation_ring_targets_0"));
    hs->Add(CoreUtils::Convert("navigation_ring_targets_1"));
    hs->Add(CoreUtils::Convert("navigation_ring_targets_2"));

    hs->Add(CoreUtils::Convert(ISettingsSecure::DEV_FORCE_SHOW_NAVBAR));
    hs->Add(CoreUtils::Convert(ISettingsSecure::KEYBOARD_BRIGHTNESS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::BUTTON_BRIGHTNESS));
    hs->Add(CoreUtils::Convert(ISettingsSecure::BUTTON_BACKLIGHT_TIMEOUT));

    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::System::MOVED_TO_SECURE = InitSystemMOVED_TO_SECURE();

static AutoPtr<IHashSet> InitSystemMOVED_TO_SECURE_THEN_GLOBAL()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New((IHashSet**)&hs);
    // these were originally in system but migrated to secure in the past,
    // so are duplicated in the Secure.* namespace
    hs->Add(CoreUtils::Convert(ISettingsGlobal::ADB_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::BLUETOOTH_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_ROAMING));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEVICE_PROVISIONED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::USB_MASS_STORAGE_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::HTTP_PROXY));

    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::System::MOVED_TO_SECURE_THEN_GLOBAL =
    InitSystemMOVED_TO_SECURE_THEN_GLOBAL();

static AutoPtr<IHashSet> InitSystemMOVED_TO_GLOBAL()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New((IHashSet**)&hs);
    // these are moving directly from system to global
    hs->Add(CoreUtils::Convert(ISettingsGlobal::AIRPLANE_MODE_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::AIRPLANE_MODE_RADIOS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::AUTO_TIME));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::AUTO_TIME_ZONE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CAR_DOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CAR_UNDOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DESK_DOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DESK_UNDOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DOCK_SOUNDS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::LOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::UNLOCK_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::LOW_BATTERY_SOUND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::POWER_SOUNDS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_SLEEP_POLICY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::MODE_RINGER));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WINDOW_ANIMATION_SCALE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TRANSITION_ANIMATION_SCALE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::ANIMATOR_DURATION_SCALE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::FANCY_IME_ANIMATIONS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::COMPATIBILITY_MODE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::EMERGENCY_TONE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CALL_AUTO_RETRY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEBUG_APP));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WAIT_FOR_DEBUGGER));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SHOW_PROCESSES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TZINFO_UPDATE_CONTENT_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TZINFO_UPDATE_METADATA_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SELINUX_UPDATE_CONTENT_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SELINUX_UPDATE_METADATA_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SMS_SHORT_CODES_UPDATE_CONTENT_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SMS_SHORT_CODES_UPDATE_METADATA_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CERT_PIN_UPDATE_CONTENT_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CERT_PIN_UPDATE_METADATA_URL));

    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::System::MOVED_TO_GLOBAL = InitSystemMOVED_TO_GLOBAL();

static AutoPtr<IUri> InitSystemDefaultUri(
    /* [in] */ const String& type)
{
    AutoPtr<IUri> uri;
    Settings::System::GetUriFor(type, (IUri**)&uri);
    assert(uri != NULL);
    return uri;
}

INIT_PROI_3 const AutoPtr<IUri> Settings::System::DEFAULT_RINGTONE_URI
    = InitSystemDefaultUri(ISettingsSystem::RINGTONE);
INIT_PROI_3 const AutoPtr<IUri> Settings::System::DEFAULT_RINGTONE_URI_2
    = InitSystemDefaultUri(ISettingsSystem::RINGTONE_2);
INIT_PROI_3 const AutoPtr<IUri> Settings::System::DEFAULT_RINGTONE_URI_3
    = InitSystemDefaultUri(ISettingsSystem::RINGTONE_3);
INIT_PROI_3 const AutoPtr<IUri> Settings::System::DEFAULT_NOTIFICATION_URI
    = InitSystemDefaultUri(ISettingsSystem::NOTIFICATION_SOUND);
INIT_PROI_3 const AutoPtr<IUri> Settings::System::DEFAULT_ALARM_ALERT_URI
    = InitSystemDefaultUri(ISettingsSystem::ALARM_ALERT);

static AutoPtr<ArrayOf<String> > InitSystemSettingsToBackup()
{
    String sSystemSettingsToBackup[] = {
        ISettingsSystem::STAY_ON_WHILE_PLUGGED_IN,   // moved to global
        ISettingsSystem::WIFI_USE_STATIC_IP,
        ISettingsSystem::WIFI_STATIC_IP,
        ISettingsSystem::WIFI_STATIC_GATEWAY,
        ISettingsSystem::WIFI_STATIC_NETMASK,
        ISettingsSystem::WIFI_STATIC_DNS1,
        ISettingsSystem::WIFI_STATIC_DNS2,
        ISettingsSystem::BLUETOOTH_DISCOVERABILITY,
        ISettingsSystem::BLUETOOTH_DISCOVERABILITY_TIMEOUT,
        ISettingsSystem::BLUETOOTH_ACCEPT_ALL_FILES,
        ISettingsSystem::DIM_SCREEN,
        ISettingsSystem::SCREEN_OFF_TIMEOUT,
        ISettingsSystem::SCREEN_BRIGHTNESS,
        ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
        ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ,
        ISettingsSystem::VIBRATE_INPUT_DEVICES,
        ISettingsSystem::MODE_RINGER_STREAMS_AFFECTED,
        ISettingsSystem::VOLUME_VOICE,
        ISettingsSystem::VOLUME_SYSTEM,
        ISettingsSystem::VOLUME_RING,
        ISettingsSystem::VOLUME_MUSIC,
        ISettingsSystem::VOLUME_ALARM,
        ISettingsSystem::VOLUME_NOTIFICATION,
        ISettingsSystem::VOLUME_BLUETOOTH_SCO,
        ISettingsSystem::VOLUME_VOICE + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_SYSTEM + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_RING + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_MUSIC + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_ALARM + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_NOTIFICATION + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::VOLUME_BLUETOOTH_SCO + ISettingsSystem::APPEND_FOR_LAST_AUDIBLE,
        ISettingsSystem::TEXT_AUTO_REPLACE,
        ISettingsSystem::TEXT_AUTO_CAPS,
        ISettingsSystem::TEXT_AUTO_PUNCTUATE,
        ISettingsSystem::TEXT_SHOW_PASSWORD,
        ISettingsSystem::AUTO_TIME,                  // moved to global
        ISettingsSystem::AUTO_TIME_ZONE,             // moved to global
        ISettingsSystem::TIME_12_24,
        ISettingsSystem::DATE_FORMAT,
        ISettingsSystem::DTMF_TONE_WHEN_DIALING,
        ISettingsSystem::DTMF_TONE_TYPE_WHEN_DIALING,
        ISettingsSystem::HEARING_AID,
        ISettingsSystem::TTY_MODE,
        ISettingsSystem::SOUND_EFFECTS_ENABLED,
        ISettingsSystem::HAPTIC_FEEDBACK_ENABLED,
        ISettingsSystem::POWER_SOUNDS_ENABLED,       // moved to global
        ISettingsSystem::DOCK_SOUNDS_ENABLED,        // moved to global
        ISettingsSystem::LOCKSCREEN_SOUNDS_ENABLED,
        ISettingsSystem::SHOW_WEB_SUGGESTIONS,
        ISettingsSystem::NOTIFICATION_LIGHT_PULSE,
        ISettingsSystem::SIP_CALL_OPTIONS,
        ISettingsSystem::SIP_RECEIVE_CALLS,
        ISettingsSystem::POINTER_SPEED,
        ISettingsSystem::VIBRATE_WHEN_RINGING,
        ISettingsSystem::RINGTONE,
        ISettingsSystem::NOTIFICATION_SOUND,
        ISettingsSystem::SYSTEM_PROFILES_ENABLED,
        ISettingsSystem::PHONE_BLACKLIST_ENABLED,
        ISettingsSystem::PHONE_BLACKLIST_NOTIFY_ENABLED,
        ISettingsSystem::PHONE_BLACKLIST_PRIVATE_NUMBER_MODE,
        ISettingsSystem::PHONE_BLACKLIST_UNKNOWN_NUMBER_MODE,
        ISettingsSystem::PHONE_BLACKLIST_REGEX_ENABLED,
        ISettingsSystem::STATUS_BAR_BATTERY_STYLE,
        ISettingsSystem::STATUS_BAR_SHOW_BATTERY_PERCENT,
        ISettingsSystem::INCREASING_RING,
        ISettingsSystem::INCREASING_RING_START_VOLUME,
        ISettingsSystem::INCREASING_RING_RAMP_UP_TIME,
        ISettingsSystem::DISPLAY_TEMPERATURE_NIGHT,
        ISettingsSystem::DISPLAY_TEMPERATURE_DAY,
        ISettingsSystem::DISPLAY_TEMPERATURE_MODE,
        ISettingsSystem::DISPLAY_AUTO_OUTDOOR_MODE,
        ISettingsSystem::LIVE_DISPLAY_HINTED
    };

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(ArraySize(sSystemSettingsToBackup));
    array->Copy(sSystemSettingsToBackup, ArraySize(sSystemSettingsToBackup));
    return array;
}

INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::System::SETTINGS_TO_BACKUP
    = InitSystemSettingsToBackup();

static AutoPtr<ArrayOf<String> > initSystemCLONE_TO_MANAGED_PROFILE()
{
    String sSystemCLONE_TO_MANAGED_PROFILE[] = {
        ISettingsSystem::DATE_FORMAT,
        ISettingsSystem::HAPTIC_FEEDBACK_ENABLED,
        ISettingsSystem::SOUND_EFFECTS_ENABLED,
        ISettingsSystem::TEXT_SHOW_PASSWORD,
        ISettingsSystem::TIME_12_24
    };

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(ArraySize(sSystemCLONE_TO_MANAGED_PROFILE));
    array->Copy(sSystemCLONE_TO_MANAGED_PROFILE, ArraySize(sSystemCLONE_TO_MANAGED_PROFILE));
    return array;
}

INIT_PROI_3 const AutoPtr<ArrayOf<String> > Settings::System::CLONE_TO_MANAGED_PROFILE
    = initSystemCLONE_TO_MANAGED_PROFILE();

ECode Settings::System::PutListAsDelimitedString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [in] */ IList* list)
{
    AutoPtr<ArrayOf<IInterface*> > tokens;
    list->ToArray((ArrayOf<IInterface*>**)&tokens);
    String store = TextUtils::Join(CoreUtils::Convert(delimiter), tokens);
    Boolean result;
    return PutString(resolver, name, store, &result);
}

ECode Settings::System::GetDelimitedStringAsList(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    *list = NULL;

    String baseString;
    GetString(resolver, name, &baseString);

    AutoPtr<IList> alist;
    FAIL_RETURN(CArrayList::New((IList**)&alist));

    if (!TextUtils::IsEmpty(baseString)) {
        AutoPtr<ArrayOf<String> > array = TextUtils::Split(
            baseString, Pattern::Quote(delimiter));
        for (Int32 i = 0; i < array->GetLength(); i++) {
            if (TextUtils::IsEmpty((*array)[i])) {
                continue;
            }
            alist->Add(CoreUtils::Convert((*array)[i]));
        }
    }

    *list = alist;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Settings::System::GetMovedKeys(
    /* [in] */ IHashSet* outKeySet)
{
    VALIDATE_NOT_NULL(outKeySet);
    return outKeySet->AddAll(ICollection::Probe(MOVED_TO_GLOBAL));
}

ECode Settings::System::GetNonLegacyMovedKeys(
    /* [in] */ IHashSet* outKeySet)
{
    VALIDATE_NOT_NULL(outKeySet);
    return outKeySet->AddAll(ICollection::Probe(MOVED_TO_GLOBAL));
}

ECode Settings::System::GetString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    return GetStringForUser(resolver, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
    Boolean flag = FALSE;
    MOVED_TO_SECURE->Contains(csq, &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Secure, returning read-only value.", name.string());
        return Secure::GetStringForUser(resolver, name, userHandle, value);
    }

    MOVED_TO_GLOBAL->Contains(csq, &flag);
    if (!flag) {
        MOVED_TO_SECURE_THEN_GLOBAL->Contains(csq, &flag);
    }
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Global, returning read-only value.", name.string());
        String str;
        FAIL_RETURN(Global::GetStringForUser(resolver, name, userHandle, &str))
        *value = str;
        return NOERROR;
    }
    return sNameValueCache->GetStringForUser(resolver, name, userHandle, value);
}

ECode Settings::System::PutString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(resolver, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::PutStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
    Boolean flag = FALSE;
    MOVED_TO_SECURE->Contains(csq, &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Secure, value is unchanged.", name.string());
        *result = FALSE;
        return NOERROR;
    }

    MOVED_TO_GLOBAL->Contains(csq, &flag);
    if (!flag) {
        MOVED_TO_SECURE_THEN_GLOBAL->Contains(csq, &flag);
    }
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Global, value is unchanged.", name.string());
        *result = FALSE;
        return NOERROR;
    }
    return sNameValueCache->PutStringForUser(resolver, name, value, userHandle, result);
}

ECode Settings::System::GetUriFor(
    /* [in] */ const String& name,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = NULL;

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
    Boolean flag = FALSE;
    MOVED_TO_SECURE->Contains(csq, &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Secure, returning Secure URI.", name.string());
        return Secure::GetUriFor(Settings::Secure::CONTENT_URI, name, uri);
    }
    MOVED_TO_GLOBAL->Contains(csq, &flag);
    if (!flag) {
        MOVED_TO_SECURE_THEN_GLOBAL->Contains(csq, &flag);
    }
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System to"
            " android.provider.Settings.Global, returning read-only global URI.", name.string());
        return Global::GetUriFor(Settings::Global::CONTENT_URI, name, uri);
    }
    return GetUriFor(CONTENT_URI, name, uri);
}

ECode Settings::System::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    return GetInt32ForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = def;
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    if (v.IsNull()) {
        *value = def;
        return NOERROR;
    } else {
        Int32 _value = StringUtils::ParseInt32(v);
        if (value == 0) {
            *value = def;
            return NOERROR;
        }
        *value = _value;
        return NOERROR;
    }
    // } catch (NumberFormatException e) {
    //     return def;
    // }
}

ECode Settings::System::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int32* value)
{
    return GetInt32ForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    Int32 _value = StringUtils::ParseInt32(v);
    if (_value == 0){
        *value = 0;
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::System::PutInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    return PutInt32ForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::PutInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::System::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    return GetInt64ForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = def;
    String valString;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &valString))
    Int64 _value;
    // try {
    if (valString.IsNull()) {
        _value = def;
    }
    else {
        _value = StringUtils::ParseInt64(valString);
        if (_value == 0) {
            _value = def;
        }
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     value = def;
    // }
}

ECode Settings::System::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int64* value)
{
    return GetInt64ForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String valString;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &valString))
    // try {
    Int64 _value = StringUtils::ParseInt64(valString);

    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::System::PutInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [out] */ Boolean* result)
{
    return PutInt64ForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::PutInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::System::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [out] */ Float* value)
{
    return GetFloatForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = def;

    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    if (v.IsNull()) {
        *value = def;
        return NOERROR;
    }
    else {
        Float _value = StringUtils::ParseFloat(v);
        if (_value == 0) {
            *value = def;
            return NOERROR;
        }
        *value = _value;
        return NOERROR;
    }
    // } catch (NumberFormatException e) {
    //     return def;
    // }
}

ECode Settings::System::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Float* value)
{
    return GetFloatForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::System::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    if (v.IsNull()) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    // try {
    Float _value = StringUtils::ParseFloat(v);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::System::PutFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [out] */ Boolean* result)
{
    return PutFloatForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::PutFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::System::GetConfiguration(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* outConfig)
{
    return GetConfigurationForUser(cr, outConfig, UserHandle::GetMyUserId());
}

ECode Settings::System::GetConfigurationForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* outConfig,
    /* [in] */ Int32 userHandle)
{
    Float fontScale;
    outConfig->GetFontScale(&fontScale);
    Float value;
    FAIL_RETURN(GetFloatForUser(cr, ISettingsSystem::FONT_SCALE, fontScale, userHandle, &value))
    outConfig->SetFontScale(value);
    if (outConfig->GetFontScale(&fontScale), fontScale < 0) {
        outConfig->SetFontScale(1);
    }
    return NOERROR;
}

void Settings::System::ClearConfiguration(
    /* [in] */ IConfiguration* inoutConfig)
{
    inoutConfig->SetFontScale(0);
}

ECode Settings::System::PutConfiguration(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* config,
    /* [out] */ Boolean* result)
{
    return PutConfigurationForUser(cr, config, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::PutConfigurationForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IConfiguration* config,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Float fontScale;
    config->GetFontScale(&fontScale);
    return Settings::System::PutFloatForUser(cr, ISettingsSystem::FONT_SCALE, fontScale, userHandle, result);
}

Boolean Settings::System::HasInterestingConfigurationChanges(
    /* [in] */ Int32 changes)
{
     return (changes & IActivityInfo::CONFIG_FONT_SCALE) != 0;
}

ECode Settings::System::GetShowGTalkServiceStatus(
    /* [in] */ IContentResolver* cr,
    /* [out] */ Boolean* result)
{
     return GetShowGTalkServiceStatusForUser(cr, UserHandle::GetMyUserId(), result);
}

ECode Settings::System::GetShowGTalkServiceStatusForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    Int32 value;
    FAIL_RETURN(GetInt32ForUser(cr, ISettingsSystem::SHOW_GTALK_SERVICE_STATUS, 0, userHandle, &value))
    *result = value != 0;
    return NOERROR;
}

ECode Settings::System::SetShowGTalkServiceStatus(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Boolean flag)
{
    return SetShowGTalkServiceStatusForUser(cr, flag, UserHandle::GetMyUserId());
}

ECode Settings::System::SetShowGTalkServiceStatusForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Boolean flag,
    /* [in] */ Int32 userHandle)
{
    Boolean result;
    return PutInt32ForUser(cr, ISettingsSystem::SHOW_GTALK_SERVICE_STATUS, flag ? 1 : 0, userHandle, &result);
}

//================================================================================
//              Settings::Secure
//================================================================================
//
static AutoPtr<IUri> InitSecureCONTENTURI()
{
    AutoPtr<IUri> uri;
    StringBuilder builder;
    builder += "content://";
    builder += ISettings::AUTHORITY;
    builder += "/secure";
    String str = builder.ToString();
    Uri::Parse(str, (IUri**)&uri);
    return uri;
}
INIT_PROI_3 const AutoPtr<IUri> Settings::Secure::CONTENT_URI = InitSecureCONTENTURI();


static AutoPtr<ArrayOf<String> > InitSecureSettingsToBackup()
{
    String sSecureSettingsToBackup[] = {
        ISettingsSecure::BUGREPORT_IN_POWER_MENU,                            // moved to global
        ISettingsSecure::ALLOW_MOCK_LOCATION,
        ISettingsSecure::PARENTAL_CONTROL_ENABLED,
        ISettingsSecure::PARENTAL_CONTROL_REDIRECT_URL,
        ISettingsSecure::USB_MASS_STORAGE_ENABLED,                           // moved to global
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_SCALE,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_AUTO_UPDATE,
        ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
        ISettingsSecure::BACKUP_AUTO_RESTORE,
        ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
        ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
        ISettingsSecure::ACCESSIBILITY_ENABLED,
        ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD,
        ISettingsSecure::ACCESSIBILITY_HIGH_TEXT_CONTRAST_ENABLED,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_ENABLED,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_LOCALE,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_BACKGROUND_COLOR,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_FOREGROUND_COLOR,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_EDGE_TYPE,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_EDGE_COLOR,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_TYPEFACE,
        ISettingsSecure::ACCESSIBILITY_CAPTIONING_FONT_SCALE,
        ISettingsSecure::TTS_USE_DEFAULTS,
        ISettingsSecure::TTS_DEFAULT_RATE,
        ISettingsSecure::TTS_DEFAULT_PITCH,
        ISettingsSecure::TTS_DEFAULT_SYNTH,
        ISettingsSecure::TTS_DEFAULT_LANG,
        ISettingsSecure::TTS_DEFAULT_COUNTRY,
        ISettingsSecure::TTS_ENABLED_PLUGINS,
        ISettingsSecure::TTS_DEFAULT_LOCALE,
        ISettingsSecure::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,            // moved to global
        ISettingsSecure::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY,               // moved to global
        ISettingsSecure::WIFI_NUM_OPEN_NETWORKS_KEPT,                        // moved to global
        ISettingsSecure::MOUNT_PLAY_NOTIFICATION_SND,
        ISettingsSecure::MOUNT_UMS_AUTOSTART,
        ISettingsSecure::MOUNT_UMS_PROMPT,
        ISettingsSecure::MOUNT_UMS_NOTIFY_ENABLED,
        ISettingsSecure::UI_NIGHT_MODE,
        ISettingsSecure::SLEEP_TIMEOUT,
        ISettingsSecure::PRIVACY_GUARD_DEFAULT,
        ISettingsSecure::ADVANCED_REBOOT,
        ISettingsSecure::DEVELOPMENT_SHORTCUT
    };

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(ArraySize(sSecureSettingsToBackup));
    array->Copy(sSecureSettingsToBackup, ArraySize(sSecureSettingsToBackup));
    return array;
}
INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::Secure::SETTINGS_TO_BACKUP = InitSecureSettingsToBackup();

INIT_PROI_3 const AutoPtr<Settings::NameValueCache> Settings::Secure::sNameValueCache
    = new Settings::NameValueCache(
        ISettingsSecure::SYS_PROP_SETTING_VERSION, InitSecureCONTENTURI(),
        ISettings::CALL_METHOD_GET_SECURE, ISettings::CALL_METHOD_PUT_SECURE);

AutoPtr<IILockSettings> Settings::Secure::sLockSettings;
Boolean Settings::Secure::sIsSystemProcess = FALSE;

static AutoPtr<IHashSet> InitSecureMOVED_TO_LOCK_SETTINGS()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New(6, (IHashSet**)&hs);

    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_VISIBLE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_TACTILE_FEEDBACK_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_PATTERN_SIZE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_DOTS_VISIBLE));
    hs->Add(CoreUtils::Convert(ISettingsSecure::LOCK_SHOW_ERROR_PATH));

    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::Secure::MOVED_TO_LOCK_SETTINGS = InitSecureMOVED_TO_LOCK_SETTINGS();

static AutoPtr<IHashSet> initSecureMOVED_TO_GLOBAL()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New((IHashSet**)&hs);

    hs->Add(CoreUtils::Convert(ISettingsGlobal::ADB_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::ASSISTED_GPS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::BLUETOOTH_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::BUGREPORT_IN_POWER_MENU));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CDMA_CELL_BROADCAST_SMS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CDMA_ROAMING_MODE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CDMA_SUBSCRIPTION_MODE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_ACTIVITY_TIMEOUT_MOBILE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_ACTIVITY_TIMEOUT_WIFI));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_ROAMING));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEVICE_PROVISIONED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DISPLAY_DENSITY_FORCED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DISPLAY_SIZE_FORCED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DOWNLOAD_MAX_BYTES_OVER_MOBILE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DOWNLOAD_RECOMMENDED_MAX_BYTES_OVER_MOBILE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::MOBILE_DATA));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_DEV_BUCKET_DURATION));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_DEV_DELETE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_DEV_PERSIST_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_DEV_ROTATE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_GLOBAL_ALERT_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_POLL_INTERVAL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_REPORT_XT_OVER_DEV));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_SAMPLE_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_TIME_CACHE_MAX_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_BUCKET_DURATION));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_DELETE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_PERSIST_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_ROTATE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_TAG_BUCKET_DURATION));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_TAG_DELETE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_TAG_PERSIST_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETSTATS_UID_TAG_ROTATE_AGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NETWORK_PREFERENCE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NITZ_UPDATE_DIFF));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NITZ_UPDATE_SPACING));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NTP_SERVER));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NTP_TIMEOUT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PDP_WATCHDOG_ERROR_POLL_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PDP_WATCHDOG_LONG_POLL_INTERVAL_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PDP_WATCHDOG_MAX_PDP_RESET_FAIL_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PDP_WATCHDOG_POLL_INTERVAL_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PDP_WATCHDOG_TRIGGER_PACKET_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SAMPLING_PROFILER_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SETUP_PREPAID_DATA_SERVICE_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SETUP_PREPAID_DETECTION_REDIR_HOST));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SETUP_PREPAID_DETECTION_TARGET_URL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TETHER_DUN_APN));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TETHER_DUN_REQUIRED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::TETHER_SUPPORTED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::USB_MASS_STORAGE_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::USE_GOOGLE_MAIL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_COUNTRY_CODE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_FRAMEWORK_SCAN_INTERVAL_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_FREQUENCY_BAND));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_IDLE_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_MAX_DHCP_RETRY_COUNT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_MOBILE_DATA_TRANSITION_WAKELOCK_TIMEOUT_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_NUM_OPEN_NETWORKS_KEPT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_P2P_DEVICE_NAME));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_SAVED_STATE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_SUPPLICANT_SCAN_INTERVAL_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_SUPPLICANT_SCAN_INTERVAL_WFD_CONNECTED_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_SUSPEND_OPTIMIZATIONS_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_ENHANCED_AUTO_JOIN));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_NETWORK_SHOW_RSSI));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_WATCHDOG_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIFI_WATCHDOG_POOR_NETWORK_TEST_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WIMAX_NETWORKS_AVAILABLE_NOTIFICATION_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PACKAGE_VERIFIER_ENABLE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PACKAGE_VERIFIER_TIMEOUT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PACKAGE_VERIFIER_DEFAULT_RESPONSE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::GPRS_REGISTER_CHECK_PERIOD_MS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WTF_IS_FATAL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::BATTERY_DISCHARGE_DURATION_THRESHOLD));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::BATTERY_DISCHARGE_THRESHOLD));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SEND_ACTION_APP_ERROR));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_AGE_SECONDS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_MAX_FILES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_QUOTA_KB));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_QUOTA_PERCENT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_RESERVE_PERCENT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DROPBOX_TAG_PREFIX));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::ERROR_LOGCAT_PREFIX));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SYS_FREE_STORAGE_LOG_INTERVAL));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DISK_FREE_CHANGE_REPORTING_THRESHOLD));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SYS_STORAGE_THRESHOLD_PERCENTAGE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SYS_STORAGE_THRESHOLD_MAX_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SYS_STORAGE_FULL_THRESHOLD_BYTES));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SYNC_MAX_RETRY_DELAY_IN_SECONDS));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CONNECTIVITY_CHANGE_DELAY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CAPTIVE_PORTAL_DETECTION_ENABLED));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::CAPTIVE_PORTAL_SERVER));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::NSD_ON));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SET_INSTALL_LOCATION));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEFAULT_INSTALL_LOCATION));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::INET_CONDITION_DEBOUNCE_UP_DELAY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::INET_CONDITION_DEBOUNCE_DOWN_DELAY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::READ_EXTERNAL_STORAGE_ENFORCED_DEFAULT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::HTTP_PROXY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::GLOBAL_HTTP_PROXY_HOST));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::GLOBAL_HTTP_PROXY_PORT));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::GLOBAL_HTTP_PROXY_EXCLUSION_LIST));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::SET_GLOBAL_HTTP_PROXY));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::DEFAULT_DNS_SERVER));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::PREFERRED_NETWORK_MODE));
    hs->Add(CoreUtils::Convert(ISettingsGlobal::WEBVIEW_DATA_REDUCTION_PROXY_KEY));

    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::Secure::MOVED_TO_GLOBAL = initSecureMOVED_TO_GLOBAL();

static AutoPtr<ArrayOf<String> > initSecureCLONE_TO_MANAGED_PROFILE()
{
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(12);

    (*args)[0] = ISettingsSecure::ACCESSIBILITY_ENABLED;
    (*args)[1] = ISettingsSecure::ALLOW_MOCK_LOCATION;
    (*args)[2] = ISettingsSecure::ALLOWED_GEOLOCATION_ORIGINS;
    (*args)[3] = ISettingsSecure::DEFAULT_INPUT_METHOD;
    (*args)[4] = ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES;
    (*args)[5] = ISettingsSecure::ENABLED_INPUT_METHODS;
    (*args)[6] = ISettingsSecure::LOCATION_MODE;
    (*args)[7] = ISettingsSecure::LOCATION_PROVIDERS_ALLOWED;
    (*args)[8] = ISettingsSecure::LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS;
    (*args)[9] = ISettingsSecure::SELECTED_INPUT_METHOD_SUBTYPE;
    (*args)[10] = ISettingsSecure::SELECTED_SPELL_CHECKER;
    (*args)[11] = ISettingsSecure::SELECTED_SPELL_CHECKER_SUBTYPE;

    return args;
}

INIT_PROI_3 const AutoPtr<ArrayOf<String> > Settings::Secure::CLONE_TO_MANAGED_PROFILE
    = initSecureCLONE_TO_MANAGED_PROFILE();

static AutoPtr<ArrayOf<String> > initNAVIGATION_RING_TARGETS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, String("navigation_ring_targets_0"));
    array->Set(1, String("navigation_ring_targets_1"));
    array->Set(2, String("navigation_ring_targets_2"));
    return array;
}

INIT_PROI_3 const AutoPtr<ArrayOf<String> > Settings::Secure::NAVIGATION_RING_TARGETS
    = initNAVIGATION_RING_TARGETS();

INIT_PROI_3 Object Settings::Secure::sSecureLock;

ECode Settings::Secure::PutListAsDelimitedString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [in] */ IList* list)
{
    AutoPtr<ArrayOf<IInterface*> > tokens;
    list->ToArray((ArrayOf<IInterface*>**)&tokens);
    String store = TextUtils::Join(CoreUtils::Convert(delimiter), tokens);
    Boolean result;
    return PutString(resolver, name, store, &result);
}

ECode Settings::Secure::GetDelimitedStringAsList(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    *list = NULL;

    String baseString;
    GetString(resolver, name, &baseString);

    AutoPtr<IList> alist;
    FAIL_RETURN(CArrayList::New((IList**)&alist));

    if (!TextUtils::IsEmpty(baseString)) {
        AutoPtr<ArrayOf<String> > array = TextUtils::Split(
            baseString, Pattern::Quote(delimiter));
        for (Int32 i = 0; i < array->GetLength(); i++) {
            if (TextUtils::IsEmpty((*array)[i])) {
                continue;
            }
            alist->Add(CoreUtils::Convert((*array)[i]));
        }
    }

    *list = alist;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Settings::Secure::GetMovedKeys(
    /* [in] */ IHashSet* outKeySet)
{
    VALIDATE_NOT_NULL(outKeySet);
    return outKeySet->AddAll(ICollection::Probe(Settings::Secure::MOVED_TO_GLOBAL));
}

ECode Settings::Secure::GetString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    return GetStringForUser(resolver, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    AutoPtr<ICharSequence> csq = CoreUtils::Convert(name);
    Boolean flag = FALSE;
    Settings::Secure::MOVED_TO_GLOBAL->Contains(csq, &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.Secure"
            " to android.provider.Settings.Global.", name.string());
        return Global::GetStringForUser(resolver, name, userHandle, value);
    }

    Settings::Secure::MOVED_TO_LOCK_SETTINGS->Contains(csq, &flag);
    if (flag) {
        {
            AutoLock syncLock(sSecureLock);
            if (sLockSettings == NULL) {
                sLockSettings = IILockSettings::Probe(ServiceManager::GetService(String("lock_settings")));
                sIsSystemProcess = Process::MyUid() == IProcess::SYSTEM_UID;
            }
        }
        if (sLockSettings != NULL && !sIsSystemProcess) {
            String str;
            ECode ec = sLockSettings->GetString(name, String("0"), userHandle, &str);
            if (SUCCEEDED(ec)) {
                *value = str;
                return NOERROR;
            }
        }
    }

    return sNameValueCache->GetStringForUser(resolver, name, userHandle, value);
}

ECode Settings::Secure::PutString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(resolver, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::Secure::PutStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (ISettingsSecure::LOCATION_MODE.Equals(name)) {
        // HACK ALERT: temporary hack to work around b/10491283.
        // TODO: once b/10491283 fixed, remove this hack
        Int32 iValue;
        StringUtils::Parse(value, &iValue);
        *result = SetLocationModeForUser(resolver, iValue, userHandle);
        return NOERROR;
    }

    Boolean flag = FALSE;
    MOVED_TO_GLOBAL->Contains(CoreUtils::Convert(name), &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.System"
            " to android.provider.Settings.Global", name.string());
        return Global::PutStringForUser(resolver, name, value, userHandle, result);
    }
    return sNameValueCache->PutStringForUser(resolver, name, value, userHandle, result);
}

ECode Settings::Secure::GetUriFor(
    /* [in] */ const String& name,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    Boolean flag = FALSE;
    MOVED_TO_GLOBAL->Contains(CoreUtils::Convert(name).Get(), &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.Secure"
            " to android.provider.Settings.Global, returning global URI.", name.string());
        return Global::GetUriFor(Settings::Global::CONTENT_URI, name, uri);
    }

    return GetUriFor(CONTENT_URI, name, uri);
}

ECode Settings::Secure::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    return GetInt32ForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (ISettingsSecure::LOCATION_MODE.Equals(name)) {
        // HACK ALERT: temporary hack to work around b/10491283.
        // TODO: once b/10491283 fixed, remove this hack
        *value = GetLocationModeForUser(cr, userHandle);
        return NOERROR;
    }
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    if (!v.IsNull()) {
        Int32 _value;
        ECode ec = StringUtils::Parse(v, &_value);
        if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION){
            *value = def;
            return NOERROR;
        }
        *value = _value;
        return NOERROR;
    }
    *value = def;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return def;
    // }
}

ECode Settings::Secure::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int32* value)
{
    return GetInt32ForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    if (ISettingsSecure::LOCATION_MODE.Equals(name)) {
        // HACK ALERT: temporary hack to work around b/10491283.
        // TODO: once b/10491283 fixed, remove this hack
        *value = GetLocationModeForUser(cr, userHandle);
        return NOERROR;
    }
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    Int32 _value;
    ECode ec = StringUtils::Parse(v, &_value);
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Secure::PutInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    return PutInt32ForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::Secure::PutInt32ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::Secure::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    return GetInt64ForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = def;
    String valString;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &valString))
    Int64 _value;
    // try {
    if (valString.IsNull()) {
        _value = def;
    }
    else {
        _value = StringUtils::ParseInt64(valString);
        if (_value == 0) {
            _value = def;
            return NOERROR;
        }
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     value = def;
    // }
}

ECode Settings::Secure::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int64* value)
{
    return GetInt64ForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String valString;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &valString))
    // try {
    Int64 _value = StringUtils::ParseInt64(valString);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Secure::PutInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [out] */ Boolean* result)
{
    return PutInt64ForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::Secure::PutInt64ForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::Secure::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [out] */ Float* value)
{
    return GetFloatForUser(cr, name, def, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = def;
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    // try {
    Float _value;
    if (v.IsNull()) {
        _value = def;
    }
    else {
        _value = StringUtils::ParseFloat(v);
        if (_value == 0) {
            _value = def;
        }
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return def;
    // }
}

ECode Settings::Secure::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Float* value)
{
    return GetFloatForUser(cr, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::Secure::GetFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String v;
    FAIL_RETURN(GetStringForUser(cr, name, userHandle, &v))
    if (v.IsNull()) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
        // throw new SettingNotFoundException(name);
    }
    // try {
    Float _value = StringUtils::ParseFloat(v);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Secure::PutFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [out] */ Boolean* result)
{
    return PutFloatForUser(cr, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::Secure::PutFloatForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    return PutStringForUser(cr, name, StringUtils::ToString(value), userHandle, result);
}

ECode Settings::Secure::IsLocationProviderEnabled(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& provider,
    /* [out] */ Boolean* result)
{
    return IsLocationProviderEnabledForUser(cr, provider, UserHandle::GetMyUserId(), result);
}

ECode Settings::Secure::IsLocationProviderEnabledForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& provider,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String allowedProviders;
    FAIL_RETURN(GetStringForUser(cr, ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, userId, &allowedProviders))
    *result = TextUtils::DelimitedStringContains(allowedProviders, ',', provider);
    return NOERROR;
}

ECode Settings::Secure::SetLocationProviderEnabled(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    Boolean ret = FALSE;
    return SetLocationProviderEnabledForUser(cr, provider, enabled, UserHandle::GetMyUserId(), &ret);
}

ECode Settings::Secure::SetLocationProviderEnabledForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& _provider,
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // to ensure thread safety, we write the provider name with a '+' or '-'
    // and let the SettingsProvider handle it rather than reading and modifying
    // the list of enabled providers.
    String provider;
    {
        AutoLock syncLock(sLocationSettingsLock);
        if (enabled) {
            provider = String("+") + _provider;
        } else {
            provider = String("-") + _provider;
        }
    }

    return PutStringForUser(cr, ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, provider, userId, result);
}

Boolean Settings::Secure::SetLocationModeForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId)
{
    Boolean result = FALSE;
    {    AutoLock syncLock(sLocationSettingsLock);
        Boolean gps = FALSE;
        Boolean network = FALSE;
        switch (mode) {
            case ISettingsSecure::LOCATION_MODE_OFF:
                break;
            case ISettingsSecure::LOCATION_MODE_SENSORS_ONLY:
                gps = TRUE;
                break;
            case ISettingsSecure::LOCATION_MODE_BATTERY_SAVING:
                network = TRUE;
                break;
            case ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY:
                gps = TRUE;
                network = TRUE;
                break;
            default:
                Slogger::E("Settings::Secure", "Invalid location mode: %d" + mode);
                // return E_ILLEGAL_ARGUMENT_EXCEPTION;
                return FALSE;
        }

        Settings::Secure::SetLocationProviderEnabledForUser(
            cr, ILocationManager::GPS_PROVIDER, gps, userId, &result);
        if (result) {
            Settings::Secure::SetLocationProviderEnabledForUser(
                cr, ILocationManager::NETWORK_PROVIDER, network, userId, &result);
        }
    }
    return result;
}

Int32 Settings::Secure::GetLocationModeForUser(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(sLocationSettingsLock);
        Boolean gpsEnabled = FALSE;
        Settings::Secure::IsLocationProviderEnabledForUser(
            cr, ILocationManager::GPS_PROVIDER, userId, &gpsEnabled);
        Boolean networkEnabled = FALSE;
        Settings::Secure::IsLocationProviderEnabledForUser(
            cr, ILocationManager::NETWORK_PROVIDER, userId, &networkEnabled);
        if (gpsEnabled && networkEnabled) {
            return ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY;
        }
        else if (gpsEnabled) {
            return ISettingsSecure::LOCATION_MODE_SENSORS_ONLY;
        }
        else if (networkEnabled) {
            return ISettingsSecure::LOCATION_MODE_BATTERY_SAVING;
        }
        else {
            return ISettingsSecure::LOCATION_MODE_OFF;
        }
    }
    return ISettingsSecure::LOCATION_MODE_OFF;
}

//================================================================================
//              Settings::Global
//================================================================================
static AutoPtr<IUri> InitGlobalCONTENTURI()
{
    StringBuilder sb("content://");
    sb += ISettings::AUTHORITY;
    sb += "/global";
    AutoPtr<IUri> uri;
    Uri::Parse(sb.ToString(), (IUri**)&uri);
    return uri;
}
INIT_PROI_3 const AutoPtr<IUri> Settings::Global::CONTENT_URI = InitGlobalCONTENTURI();

static AutoPtr<ArrayOf<String> > InitGlobalSettingsToBackup()
{
    String sGlobalSettingsToBackup[] = {
        ISettingsGlobal::BUGREPORT_IN_POWER_MENU,
        ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN,
        ISettingsGlobal::WAKE_WHEN_PLUGGED_OR_UNPLUGGED,
        ISettingsGlobal::AUTO_TIME,
        ISettingsGlobal::AUTO_TIME_ZONE,
        ISettingsGlobal::POWER_SOUNDS_ENABLED,
        ISettingsGlobal::DOCK_SOUNDS_ENABLED,
        ISettingsGlobal::USB_MASS_STORAGE_ENABLED,
        ISettingsGlobal::ENABLE_ACCESSIBILITY_GLOBAL_GESTURE_ENABLED,
        ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_NOTIFICATION_ON,
        ISettingsGlobal::WIFI_NETWORKS_AVAILABLE_REPEAT_DELAY,
        ISettingsGlobal::WIFI_WATCHDOG_POOR_NETWORK_TEST_ENABLED,
        ISettingsGlobal::WIFI_NUM_OPEN_NETWORKS_KEPT,
        ISettingsGlobal::EMERGENCY_TONE,
        ISettingsGlobal::CALL_AUTO_RETRY,
        ISettingsGlobal::DOCK_AUDIO_MEDIA_ENABLED
    };

    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(ArraySize(sGlobalSettingsToBackup));
    array->Copy(sGlobalSettingsToBackup, ArraySize(sGlobalSettingsToBackup));
    return array;
}

INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::Global::SETTINGS_TO_BACKUP = InitGlobalSettingsToBackup();

INIT_PROI_3 const AutoPtr<Settings::NameValueCache> Settings::Global::sNameValueCache = new Settings::NameValueCache(
        ISettingsGlobal::SYS_PROP_SETTING_VERSION, InitGlobalCONTENTURI(),
        ISettings::CALL_METHOD_GET_GLOBAL, ISettings::CALL_METHOD_PUT_GLOBAL);

static AutoPtr<IHashSet> InitGlobalMOVED_TO_SECURE()
{
    AutoPtr<IHashSet> hs;
    CHashSet::New(2, (IHashSet**)&hs);
    hs->Add(CoreUtils::Convert(ISettingsGlobal::INSTALL_NON_MARKET_APPS).Get());
    hs->Add(CoreUtils::Convert(ISettingsSecure::POWER_MENU_ACTIONS).Get());
    return hs;
}

INIT_PROI_3 const AutoPtr<IHashSet> Settings::Global::MOVED_TO_SECURE = InitGlobalMOVED_TO_SECURE();

static AutoPtr<ArrayOf<String> > InitGloabalMULTI_SIM_USER_PREFERRED_SUBS()
{
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(3);
    (*str)[0] = String("user_preferred_sub1");
    (*str)[1] = String("user_preferred_sub2");
    (*str)[2] = String("user_preferred_sub3");
    return str;
}

INIT_PROI_3 const AutoPtr<ArrayOf<String> > Settings::Global::MULTI_SIM_USER_PREFERRED_SUBS =
    InitGloabalMULTI_SIM_USER_PREFERRED_SUBS();

ECode Settings::Global::ZenModeToString(
    /* [in] */ Int32 mode,
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    if (mode == ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS) {
        *key = String("ZEN_MODE_IMPORTANT_INTERRUPTIONS");
        return NOERROR;
    }

    if (mode == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) {
        *key = String("ZEN_MODE_NO_INTERRUPTIONS");
        return NOERROR;
    }
    *key = String("ZEN_MODE_OFF");
    return NOERROR;
}

ECode Settings::Global::GetBluetoothMapPriorityKey(
    /* [in] */ const String& address,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ISettingsGlobal::BLUETOOTH_INPUT_DEVICE_PRIORITY_PREFIX + address.ToUpperCase(/*Locale.ROOT*/);
    return NOERROR;
}

String Settings::Global::GetBluetoothHeadsetPriorityKey(
    /* [in] */ const String& address)
{
    return ISettingsGlobal::BLUETOOTH_HEADSET_PRIORITY_PREFIX + address.ToUpperCase(/*Locale.ROOT*/);
}

String Settings::Global::GetBluetoothA2dpSrcPriorityKey(
    /* [in] */ const String& address)
{
    return ISettingsGlobal::BLUETOOTH_A2DP_SRC_PRIORITY_PREFIX + address.ToUpperCase(/*Locale.ROOT*/);
}

String Settings::Global::GetBluetoothA2dpSinkPriorityKey(
    /* [in] */ const String& address)
{
    return ISettingsGlobal::BLUETOOTH_A2DP_SINK_PRIORITY_PREFIX + address.ToUpperCase(/*Locale.ROOT*/);
}

String Settings::Global::GetBluetoothInputDevicePriorityKey(
    /* [in] */ const String& address)
{
    return ISettingsGlobal::BLUETOOTH_INPUT_DEVICE_PRIORITY_PREFIX + address.ToUpperCase(/*Locale.ROOT*/);
}

ECode Settings::Global::PutListAsDelimitedString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [in] */ IList* list)
{
    AutoPtr<ArrayOf<IInterface*> > tokens;
    list->ToArray((ArrayOf<IInterface*>**)&tokens);
    String store = TextUtils::Join(CoreUtils::Convert(delimiter), tokens);
    Boolean result;
    return PutString(resolver, name, store, &result);
}

ECode Settings::Global::GetDelimitedStringAsList(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& delimiter,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);

    *list = NULL;

    String baseString;
    GetString(resolver, name, &baseString);

    AutoPtr<IList> alist;
    FAIL_RETURN(CArrayList::New((IList**)&alist));

    if (!TextUtils::IsEmpty(baseString)) {
        AutoPtr<ArrayOf<String> > array = TextUtils::Split(
            baseString, Pattern::Quote(delimiter));
        for (Int32 i = 0; i < array->GetLength(); i++) {
            if (TextUtils::IsEmpty((*array)[i])) {
                continue;
            }
            alist->Add(CoreUtils::Convert((*array)[i]));
        }
    }

    *list = alist;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Settings::Global::GetString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    return GetStringForUser(resolver, name, UserHandle::GetMyUserId(), value);
}

ECode Settings::Global::GetStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ Int32 userHandle,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = String(NULL);

    Boolean flag = FALSE;
    MOVED_TO_SECURE->Contains(CoreUtils::Convert(name).Get(), &flag);
    if (flag) {
        Slogger::W(TAG, "Setting %s has moved from android.provider.Settings.Global"
            " to android.provider.Settings.Secure, returning read-only value.", name.string());
        return Settings::Secure::GetStringForUser(resolver, name, userHandle, value);
    }
    return sNameValueCache->GetStringForUser(resolver, name, userHandle, value);
}

ECode Settings::Global::PutString(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return PutStringForUser(resolver, name, value, UserHandle::GetMyUserId(), result);
}

ECode Settings::Global::PutStringForUser(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ const String& name,
    /* [in] */ const String& value,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (LOCAL_LOGV) {
        Slogger::V(TAG, "Global.putString(name=%s, value=%s for %d", name.string(), value.string(), userHandle);
    }
    return sNameValueCache->PutStringForUser(resolver, name, value, userHandle, result);
}

ECode Settings::Global::GetUriFor(
    /* [in] */ const String& name,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    Boolean flag = FALSE;
    if (MOVED_TO_SECURE->Contains(CoreUtils::Convert(name).Get(), &flag), flag) {
        Slogger::V(TAG, "Setting %s has moved from android.provider.Settings.Global"
            " to android.provider.Settings.Secure, returning Secure URI.", name.string());
        return Secure::GetUriFor(Settings::Secure::CONTENT_URI, name, uri);
    }
    return GetUriFor(CONTENT_URI, name, uri);
}

ECode Settings::Global::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 def,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    FAIL_RETURN(GetString(cr, name, &v))
    // try {
    Int32 _value;
    if (v.IsNull()) {
        _value = def;
    } else {
        _value = StringUtils::ParseInt32(v);
        if (_value == 0) {
            _value = def;
        }
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     return def;
    // }
}

ECode Settings::Global::GetInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String v;
    FAIL_RETURN(GetString(cr, name, &v))
    // try {
    Int32 _value = StringUtils::ParseInt32(v);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Global::PutInt32(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int32 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return PutString(cr, name, StringUtils::ToString(value), result);
}

ECode Settings::Global::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 def,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    String valString;
    FAIL_RETURN(GetString(cr, name, &valString))
    // try {
    Int64 _value;
    if (valString.IsNull()) {
        _value = def;
    }
    else {
        _value = StringUtils::ParseInt64(valString);
        if (_value == 0) {
            _value = def;
        }
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     value = def;
    // }
}

ECode Settings::Global::GetInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String valString;
    FAIL_RETURN(GetString(cr, name, &valString))
    // try {
    Int64 _value = StringUtils::ParseInt64(valString);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Global::PutInt64(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Int64 value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return PutString(cr, name, StringUtils::ToString(value), result);
}

ECode Settings::Global::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float def,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    String v;
    FAIL_RETURN(GetString(cr, name, &v))
    // try {
    Float _value;
    if (v.IsNull()) {
        _value = def;
    }
    else {
        ECode ec = StringUtils::Parse(v, &_value);
        if (FAILED(ec)) _value = def;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     value = def;
    // }
}

ECode Settings::Global::GetFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    String v;
    FAIL_RETURN(GetString(cr, name, &v))
    if (v.IsNull()) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
        // throw new SettingNotFoundException(name);
    }
    // try {
    Float _value = StringUtils::ParseFloat(v);
    if (_value == 0) {
        return E_SETTING_NOT_FOUND_EXCEPTION;
    }
    *value = _value;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     throw new SettingNotFoundException(name);
    // }
}

ECode Settings::Global::PutFloat(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& name,
    /* [in] */ Float value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return PutString(cr, name, StringUtils::ToString(value), result);
}

//================================================================================
//              Settings::Bookmarks
//================================================================================

static AutoPtr<IUri> InitBookmarksContentUri()
{
    StringBuilder sb("content://");
    sb += ISettings::AUTHORITY;
    sb += "/bookmarks";
    AutoPtr<IUri> uri;
    Uri::Parse(sb.ToString(), (IUri**)&uri);
    return uri;
}
INIT_PROI_3 AutoPtr<IUri> Settings::Bookmarks::CONTENT_URI = InitBookmarksContentUri();

INIT_PROI_3 const String Settings::Bookmarks::TAG("Bookmarks");

static AutoPtr< ArrayOf<String> > IntitIntentProjection()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(1);
    (*array)[0] = ISettingsBookmarks::INTENT;
    return array;
}
INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::Bookmarks::sIntentProjection = IntitIntentProjection();

static AutoPtr< ArrayOf<String> > IntitShortcutProjection()
{
    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = ISettingsBookmarks::ID;
    (*array)[1] = ISettingsBookmarks::SHORTCUT;
    return array;
}
INIT_PROI_3 const AutoPtr< ArrayOf<String> > Settings::Bookmarks::sShortcutProjection = IntitShortcutProjection();
INIT_PROI_3 const String Settings::Bookmarks::sShortcutSelection = ISettingsBookmarks::SHORTCUT + String("=?");

ECode Settings::Bookmarks::GetIntentForShortcut(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Char32 shortcut,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    AutoPtr<IIntent> _intent;

    AutoPtr<ICursor> c;
    AutoPtr< ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
    (*selectionArgs)[0] = StringUtils::ToString((Int32)shortcut);
    FAIL_RETURN(cr->Query(CONTENT_URI, sIntentProjection, sShortcutSelection,
            selectionArgs, ISettingsBookmarks::ORDERING, (ICursor**)&c))
    assert(c != NULL);
    // Keep trying until we find a valid shortcut
    // try {
    Boolean hasNext = FALSE;
    while (_intent == NULL && (c->MoveToNext(&hasNext), hasNext)) {
        // try {
        Int32 colIndex;
        if (FAILED(c->GetColumnIndexOrThrow(ISettingsBookmarks::INTENT, &colIndex))) {
            Slogger::W(TAG, "Intent column not found");
            continue;
        }
        String intentURI;
        if (FAILED(c->GetString(colIndex, &intentURI))) {
            continue;
        }
        Intent::ParseUri(intentURI, 0, (IIntent**)&_intent);
        // } catch (java.net.URISyntaxException e) {
        //     // The stored URL is bad...  ignore it.
        // } catch (IllegalArgumentException e) {
        //     // Column not found
        //     Log.w(TAG, "Intent column not found", e);
        // }
    }

    if (c != NULL) {
        ICloseable::Probe(c)->Close();
    }
    *intent = _intent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode Settings::Bookmarks::Add(
    /* [in] */ IContentResolver* cr,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& title,
    /* [in] */ const String& folder,
    /* [in] */ Char32 shortcut,
    /* [in] */ Int32 ordering,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    // If a shortcut is supplied, and it is already defined for
    // another bookmark, then remove the old definition.
    if (shortcut != 0) {
        AutoPtr< ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(1);
        (*selectionArgs)[0] = StringUtils::ToString((Int32)shortcut);
        Int32 rowsAffected;
        FAIL_RETURN(cr->Delete(CONTENT_URI, sShortcutSelection, selectionArgs, &rowsAffected))
    }

    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    if (!title.IsNull()) {
        values->Put(ISettingsBookmarks::TITLE, title);
    }
    if (!folder.IsNull()) {
        values->Put(ISettingsBookmarks::FOLDER, folder);
    }

    String str;
    intent->ToUri(0, &str);
    values->Put(ISettingsBookmarks::INTENT, str);

    if (shortcut != 0) {
        values->Put(ISettingsBookmarks::SHORTCUT, (Int32)shortcut);
    }

    values->Put(ISettingsBookmarks::ORDERING, ordering);
    return cr->Insert(CONTENT_URI, values, uri);
}

AutoPtr<ICharSequence> Settings::Bookmarks::GetLabelForFolder(
    /* [in] */ IResources* r,
    /* [in] */ const String& folder)
{
    AutoPtr<ICharSequence> cs;
    CString::New(folder, (ICharSequence**)&cs);
    return cs;
}

ECode Settings::Bookmarks::GetTitle(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title)
    *title = NULL;

    Int32 titleColumn;
    cursor->GetColumnIndex(ISettingsBookmarks::TITLE, &titleColumn);
    Int32 intentColumn;
    cursor->GetColumnIndex(ISettingsBookmarks::INTENT, &intentColumn);
    if (titleColumn == -1 || intentColumn == -1) {
        Slogger::E(TAG, "The cursor must contain the TITLE and INTENT columns.");
        return E_ILLEGAL_SQLITE_EXCEPTION;
        // throw new IllegalArgumentException(
        //         "The cursor must contain the TITLE and INTENT columns.");
    }

    String str;
    FAIL_RETURN(cursor->GetString(titleColumn, &str))
    if (!TextUtils::IsEmpty(str)) {
        AutoPtr<ICharSequence> cs;
        CString::New(str, (ICharSequence**)&cs);
        *title = cs;
        REFCOUNT_ADD(*title)
        return NOERROR;
    }

    String intentUri;
    FAIL_RETURN(cursor->GetString(intentColumn, &intentUri))
    if (TextUtils::IsEmpty(intentUri)) {
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        *title = cs;
        REFCOUNT_ADD(*title)
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    // try {
    if (FAILED(Intent::ParseUri(intentUri, 0, (IIntent**)&intent))) {
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        *title = cs;
        REFCOUNT_ADD(*title)
        return NOERROR;
    }
    // } catch (URISyntaxException e) {
    //     return "";
    // }

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IResolveInfo> info;
    packageManager->ResolveActivity(intent, 0, (IResolveInfo**)&info);
    if (info) {
        return info->LoadLabel(packageManager, title);
    }
    else {
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        *title = cs;
        REFCOUNT_ADD(*title)
        return NOERROR;
    }
}

String Settings::GetGTalkDeviceId(
    /* [in] */ Int64 androidId)
{
    return String("android-") + StringUtils::ToHexString(androidId);
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
