
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/settings/location/SettingsInjector.h"
#include "elastos/droid/settings/location/DimmableIconPreference.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Location::ISettingInjectorService;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  SettingsInjector::StatusLoadingHandler
//===============================================================================

SettingsInjector::StatusLoadingHandler::StatusLoadingHandler(
    /* [in] */ SettingsInjector* host)
    : mReloadRequested(FALSE)
    , mHost(host)
{}

ECode SettingsInjector::StatusLoadingHandler::constructor()
{
    CHashSet::New((ISet**)&mSettingsToLoad);
    CHashSet::New((ISet**)&mSettingsBeingLoaded);
    CHashSet::New((ISet**)&mTimedOutSettings);
    return Handler::constructor();
}

ECode SettingsInjector::StatusLoadingHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "handleMessage start: %s, %s", TO_CSTR(msg), TO_CSTR(this));
    }

    // Update state in response to message
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case WHAT_RELOAD:
            mReloadRequested = TRUE;
            break;
        case WHAT_RECEIVED_STATUS: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ISetting* receivedSetting = ISetting::Probe(obj);
            ((Setting*)receivedSetting)->MaybeLogElapsedTime();
            mSettingsBeingLoaded->Remove(receivedSetting);
            mTimedOutSettings->Remove(receivedSetting);
            RemoveMessages(WHAT_TIMEOUT, receivedSetting);
            break;
        }
        case WHAT_TIMEOUT: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ISetting* timedOutSetting = (Setting*)ISetting::Probe(obj);
            mSettingsBeingLoaded->Remove(timedOutSetting);
            mTimedOutSettings->Add(timedOutSetting);
            if (Logger::IsLoggable(TAG, Logger::WARN)) {
                Logger::W(TAG, "Timed out after %d millis trying to get status for: %s",
                        ((Setting*)timedOutSetting)->GetElapsedTime(), TO_CSTR(timedOutSetting));
            }
            break;
        }
        default:
            Logger::W(TAG, "Unexpected what: %s", TO_CSTR(msg));
            // Log->Wtf(TAG, "Unexpected what: " + msg);
    }

    // Decide whether to load additional settings based on the new state. Start by seeing
    // if we have headroom to load another setting.
    Int32 size;
    if ((mSettingsBeingLoaded->GetSize(&size), size) > 0
            || (mTimedOutSettings->GetSize(&size), size) > 1) {
        // Don't load any more settings until one of the pending settings has completed.
        // To reduce memory pressure, we want to be loading at most one setting (plus at
        // most one timed-out setting) at a time. This means we'll be responsible for
        // bringing in at most two services.
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "too many services already live for %s, %s", TO_CSTR(msg), TO_CSTR(this));
        }
        return NOERROR;
    }

    Boolean res;
    if (mReloadRequested && (mSettingsToLoad->IsEmpty(&res), res) && (mSettingsBeingLoaded->IsEmpty(&res), res)
            && (mTimedOutSettings->IsEmpty(&res), res)) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "reloading because idle and reload requesteed %s, %s", TO_CSTR(msg), TO_CSTR(this));
        }
        // Reload requested, so must reload all settings
        mSettingsToLoad->AddAll(ICollection::Probe(mHost->mSettings));
        mReloadRequested = FALSE;
    }

    // Remove the next setting to load from the queue, if any
    AutoPtr<IIterator> iter;
    mSettingsToLoad->GetIterator((IIterator**)&iter);
    if (iter->HasNext(&res), !res) {
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "nothing left to do for %s, %s", TO_CSTR(msg), TO_CSTR(this));
        }
        return NOERROR;
    }
    AutoPtr<IInterface> obj;
    iter->GetNext((IInterface**)&obj);
    ISetting* setting = ISetting::Probe(obj);
    iter->Remove();

    // Request the status value
    ((Setting*)setting)->StartService();
    mSettingsBeingLoaded->Add(setting);

    // Ensure that if receiving the status value takes too Int64, we start loading the
    // next value anyway
    AutoPtr<IMessage> timeoutMsg;
    ObtainMessage(WHAT_TIMEOUT, setting, (IMessage**)&timeoutMsg);
    SendMessageDelayed(timeoutMsg, INJECTED_STATUS_UPDATE_TIMEOUT_MILLIS, &res);

    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "handleMessage end %s, %s, started loading %s",
                TO_CSTR(msg), TO_CSTR(this), TO_CSTR(setting));
    }
    return NOERROR;
}

ECode SettingsInjector::StatusLoadingHandler::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder builder("");
    builder += "StatusLoadingHandler{";
    builder += "mSettingsToLoad=";
    builder += mSettingsToLoad;
    builder += ", mSettingsBeingLoaded=";
    builder += mSettingsBeingLoaded;
    builder += ", mTimedOutSettings=";
    builder += mTimedOutSettings;
    builder += ", mReloadRequested=";
    builder += mReloadRequested;
    builder += '}';

    *str = builder.ToString();
    return NOERROR;
}

//===============================================================================
//                  SettingsInjector::Setting::StartServiceHandler
//===============================================================================

SettingsInjector::Setting::StartServiceHandler::StartServiceHandler(
    /* [in] */ Setting* host,
    /* [in] */ SettingsInjector* settingsInjector)
    : mHost(host)
    , mSettingsInjector(settingsInjector)
{}

ECode SettingsInjector::Setting::StartServiceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IBundle> bundle;
    msg->GetData((IBundle**)&bundle);
    Boolean enabled;
    bundle->GetBoolean(ISettingInjectorService::ENABLED_KEY, TRUE, &enabled);
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "%s: received %s, bundle: %s", TO_CSTR(mHost->mSetting), TO_CSTR(msg), TO_CSTR(bundle));
    }
    mHost->mPreference->SetSummary((ICharSequence*)NULL);
    mHost->mPreference->SetEnabled(enabled);
    AutoPtr<IMessage> message;
    mSettingsInjector->mHandler->ObtainMessage(WHAT_RECEIVED_STATUS, (ISetting*)mHost, (IMessage**)&message);
    Boolean res;
    mSettingsInjector->mHandler->SendMessage(message, &res);
    return NOERROR;
}

//===============================================================================
//                  SettingsInjector::Setting
//===============================================================================

CAR_INTERFACE_IMPL(SettingsInjector::Setting, Object, ISetting)

SettingsInjector::Setting::Setting(
    /* [in] */ InjectedSetting* setting,
    /* [in] */ IPreference* preference,
    /* [in] */ SettingsInjector* host)
    : mSetting(setting)
    , mPreference(preference)
    , mStartMillis(0)
    , mHost(host)
{}

ECode SettingsInjector::Setting::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder builder("");
    builder += "Setting{";
    builder += "setting=";
    builder += mSetting;
    builder += ", preference=";
    builder += mPreference;
    builder += '}';
    *str = builder.ToString();
    return NOERROR;
}

ECode SettingsInjector::Setting::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean res;
    *result = TO_IINTERFACE(this) == IInterface::Probe(o)
            || (ISetting::Probe(o) != NULL
                && (mSetting->Equals(((IInjectedSetting*)((Setting*)ISetting::Probe(o))->mSetting), &res), res));
    return NOERROR;
}

ECode SettingsInjector::Setting::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSetting->GetHashCode(result);
}

ECode SettingsInjector::Setting::StartService()
{
    AutoPtr<StartServiceHandler> handler = new StartServiceHandler(this, mHost);
    handler->constructor();

    AutoPtr<IMessenger> messenger;
    CMessenger::New(handler, (IMessenger**)&messenger);

    AutoPtr<IIntent> intent = mSetting->GetServiceIntent();
    intent->PutExtra(ISettingInjectorService::MESSENGER_KEY, IParcelable::Probe(messenger));

    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "%s: sending update intent: %s, handler: %s",
                TO_CSTR(mSetting), TO_CSTR(intent), TO_CSTR(handler));
        mStartMillis = SystemClock::GetElapsedRealtime();
    }
    else {
        mStartMillis = 0;
    }

    // Start the service, making sure that this is attributed to the current user rather
    // than the system user.
    AutoPtr<IUserHandle> userHandle;
    Process::MyUserHandle((IUserHandle**)&userHandle);
    AutoPtr<IComponentName> comp;
    return mHost->mContext->StartServiceAsUser(intent, userHandle, (IComponentName**)&comp);
}

Int64 SettingsInjector::Setting::GetElapsedTime()
{
    Int64 end = SystemClock::GetElapsedRealtime();
    return end - mStartMillis;
}

ECode SettingsInjector::Setting::MaybeLogElapsedTime()
{
    if (Logger::IsLoggable(TAG, Logger::___DEBUG) && mStartMillis != 0) {
        Int64 elapsed = GetElapsedTime();
        Logger::D(TAG, "%s update took %ld millis", TO_CSTR(this), elapsed);
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsInjector
//===============================================================================

const String SettingsInjector::TAG("SettingsInjector");
const Int64 SettingsInjector::INJECTED_STATUS_UPDATE_TIMEOUT_MILLIS = 1000;
const Int32 SettingsInjector::WHAT_RELOAD;
const Int32 SettingsInjector::WHAT_RECEIVED_STATUS;
const Int32 SettingsInjector::WHAT_TIMEOUT;

SettingsInjector::SettingsInjector(
    /* [in] */ IContext* context)
{
    mContext = context;
    CHashSet::New((ISet**)&mSettings);
    mHandler = new StatusLoadingHandler(this);
    mHandler->constructor();
}

AutoPtr<IList> SettingsInjector::GetSettings()
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ISettingInjectorService::ACTION_SERVICE_INTENT, (IIntent**)&intent);

    AutoPtr<IList> resolveInfos; //List<ResolveInfo>
    pm->QueryIntentServices(intent, IPackageManager::GET_META_DATA, (IList**)&resolveInfos);
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "Found services: %s", TO_CSTR(resolveInfos));
    }

    Int32 size;
    resolveInfos->GetSize(&size);
    AutoPtr<IList> settings; //List<InjectedSetting>
    CArrayList::New(size, (IList**)&settings);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        resolveInfos->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        // try {
        AutoPtr<InjectedSetting> setting;
        ECode ec = ParseServiceInfo(resolveInfo, pm, (InjectedSetting**)&setting);
        if (FAILED(ec)) {
            Logger::W(TAG, "Unable to load service info %s, ec: 0x%08x", TO_CSTR(resolveInfo), ec);
        }
        else if (setting == NULL) {
            Logger::W(TAG, "Unable to load service info %s", TO_CSTR(resolveInfo));
        }
        else {
            settings->Add((IInjectedSetting*)setting);
        }
        // } catch (XmlPullParserException e) {
        //     Logger::W(TAG, "Unable to load service info " + resolveInfo, e);
        // } catch (IOException e) {
        //     Logger::W(TAG, "Unable to load service info " + resolveInfo, e);
        // }
    }
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "Loaded settings: %s", TO_CSTR(settings));
    }

    return settings;
}

ECode SettingsInjector::ParseServiceInfo(
    /* [in] */ IResolveInfo* service,
    /* [in] */ IPackageManager* pm,
    /* [out] */ InjectedSetting** result) //throws XmlPullParserException, IOException
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IServiceInfo> si;
    service->GetServiceInfo((IServiceInfo**)&si);
    AutoPtr<IApplicationInfo> ai;
    IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&ai);

    Int32 flags;
    ai->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
        if (Logger::IsLoggable(TAG, Logger::WARN)) {
            Logger::W(TAG, "Ignoring attempt to inject setting from app not in system image: %s", TO_CSTR(service));
            return NOERROR;
        }
    }

    AutoPtr<IXmlResourceParser> parser;
    // try {
    IPackageItemInfo* _si = IPackageItemInfo::Probe(si);
    ECode ec = _si->LoadXmlMetaData(pm, ISettingInjectorService::META_DATA_NAME, (IXmlResourceParser**)&parser);
    String packageName;
    _si->GetPackageName(&packageName);
    if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Unable to load resources for package %s", packageName.string());
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    if (parser == NULL) {
        Logger::E(TAG, "No %s meta-data for %s: %s", ISettingInjectorService::META_DATA_NAME.string(),
                TO_CSTR(service), TO_CSTR(si));
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    IXmlPullParser* _parser = IXmlPullParser::Probe(parser);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(_parser);

    Int32 type;
    while ((_parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }

    String nodeName;
    _parser->GetName(&nodeName);
    if (!ISettingInjectorService::ATTRIBUTES_NAME.Equals(nodeName)) {
        Logger::E(TAG, "Meta-data does not start with %s tag", ISettingInjectorService::ATTRIBUTES_NAME.string());
        if (parser != NULL) {
            parser->Close();
        }
        return E_XML_PULL_PARSER_EXCEPTION;
    }

    AutoPtr<IResources> res;
    ec = pm->GetResourcesForApplication(ai, (IResources**)&res);
    if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "Unable to load resources for package %s", packageName.string());
        if (parser != NULL) {
            parser->Close();
        }
        return E_XML_PULL_PARSER_EXCEPTION;
    }
    String name;
    _si->GetName(&name);
    AutoPtr<InjectedSetting> is = ParseAttributes(packageName, name, res, attrs);
    *result = is;
    REFCOUNT_ADD(*result)
    if (parser != NULL) {
        parser->Close();
    }
    return NOERROR;
    // } catch (PackageManager.NameNotFoundException e) {
    //     throw new XmlPullParserException(
    //             "Unable to load resources for package " + si.packageName);
    // } finally {
    //     if (parser != NULL) {
    //         parser->Close();
    //     }
    // }
}

AutoPtr<InjectedSetting> SettingsInjector::ParseAttributes(
    /* [in] */ const String& packageName,
    /* [in] */ const String& className,
    /* [in] */ IResources* res,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ITypedArray> sa;
    res->ObtainAttributes(attrs, TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::SettingInjectorService), (ITypedArray**)&sa);
    // try {
        // Note that to help guard against malicious string injection, we do not allow dynamic
        // specification of the label (setting title)
        String title;
        sa->GetString(Elastos::Droid::R::styleable::SettingInjectorService_title, &title);
        Int32 iconId;
        sa->GetResourceId(Elastos::Droid::R::styleable::SettingInjectorService_icon, 0, &iconId);
        String settingsActivity;
        sa->GetString(Elastos::Droid::R::styleable::SettingInjectorService_settingsActivity, &settingsActivity);
        if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
            Logger::D(TAG, "parsed title: %s, iconId: %d, settingsActivity: %s",
                    title.string(), iconId, settingsActivity.string());
        }
        sa->Recycle();
        AutoPtr<InjectedSetting> is = InjectedSetting::NewInstance(packageName, className,
                title, iconId, settingsActivity);
        return is;
    // } finally {
    //     sa->Recycle();
    // }
}

AutoPtr<IList> SettingsInjector::GetInjectedSettings()
{
    AutoPtr<IList> settings = GetSettings(); //Iterable<InjectedSetting>
    AutoPtr<IList> prefs;//ArrayList<Preference>
    CArrayList::New((IList**)&prefs);

    Int32 size;
    settings->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        settings->Get(i, (IInterface**)&obj);
        InjectedSetting* setting = (InjectedSetting*)IInjectedSetting::Probe(obj);

        AutoPtr<IPreference> pref = AddServiceSetting(prefs, setting);
        AutoPtr<Setting> set = new Setting(setting, pref, this);
        mSettings->Add((ISetting*)set);
    }

    ReloadStatusMessages();

    return prefs;
}

ECode SettingsInjector::ReloadStatusMessages()
{
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "reloadingStatusMessages: %s", TO_CSTR(mSettings));
    }
    AutoPtr<IMessage> message;
    mHandler->ObtainMessage(WHAT_RELOAD, (IMessage**)&message);
    Boolean res;
    mHandler->SendMessage(message, &res);
    return NOERROR;
}

AutoPtr<IPreference> SettingsInjector::AddServiceSetting(
    /* [in] */ IList* prefs, //List<Preference>
    /* [in] */ InjectedSetting* info)
{
    AutoPtr<DimmableIconPreference> pref = new DimmableIconPreference();
    pref->constructor(mContext);
    pref->SetTitle(CoreUtils::Convert(info->mTitle));
    pref->SetSummary((ICharSequence*)NULL);
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IDrawable> icon;
    pm->GetDrawable(info->mPackageName, info->mIconId, NULL, (IDrawable**)&icon);
    pref->SetIcon(icon);

    // Activity to start if they click on the preference. Must start in new task to ensure
    // that "android.settings.LOCATION_SOURCE_SETTINGS" brings user back to Settings > Location.
    AutoPtr<IIntent> settingIntent;
    CIntent::New((IIntent**)&settingIntent);
    settingIntent->SetClassName(info->mPackageName, info->mSettingsActivity);
    settingIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    pref->SetIntent(settingIntent);

    prefs->Add((IPreference*)pref);
    return (IPreference*)pref;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos