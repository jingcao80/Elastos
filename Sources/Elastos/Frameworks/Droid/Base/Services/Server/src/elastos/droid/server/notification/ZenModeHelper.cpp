
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Telecom.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.External.h>
#include "elastos/droid/server/notification/ZenModeHelper.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include "elastos/droid/server/notification/ValidateNotificationPeople.h"
#include "elastos/droid/server/notification/ZenLog.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Media::IAudioAttributes;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Service::Notification::CZenModeConfig;
using Elastos::Droid::Service::Notification::CZenModeConfigHelper;
using Elastos::Droid::Service::Notification::IZenModeConfigHelper;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Utility::ILogHelper;
using Elastos::Droid::R;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Objects;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String ZenModeHelper::TAG("ZenModeHelper");
const Boolean ZenModeHelper::DEBUG = Logger::IsLoggable("ZenModeHelper", ILogHelper::DEBUG);

//===============================================================================
//                  ZenModeHelper::Callback
//===============================================================================

ZenModeHelper::Callback::Callback()
{}

ZenModeHelper::Callback::~Callback()
{}

void ZenModeHelper::Callback::OnConfigChanged()
{}

void ZenModeHelper::Callback::OnZenModeChanged()
{}

//===============================================================================
//                  ZenModeHelper::SettingsObserver
//===============================================================================

ECode ZenModeHelper::SettingsObserver::constructor(
    /* [in] */ IHandler* handle,
    /* [in] */ ZenModeHelper* host)
{
    mHost = host;
    ContentObserver::constructor(handle);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    return global->GetUriFor(ISettingsGlobal::ZEN_MODE, (IUri**)&ZEN_MODE);
}

ZenModeHelper::SettingsObserver::~SettingsObserver()
{}

void ZenModeHelper::SettingsObserver::Observe()
{
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(ZEN_MODE, FALSE /*notifyForDescendents*/, this);
    Update(NULL);
}

ECode ZenModeHelper::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Update(uri);
    return NOERROR;
}

void ZenModeHelper::SettingsObserver::Update(
    /* [in] */ IUri* uri)
{
    Boolean res;
    if (IObject::Probe(ZEN_MODE)->Equals(uri, &res), res) {
        mHost->UpdateZenMode();
    }
}

//===============================================================================
//                  ZenModeHelper::MyRunnable
//===============================================================================

ZenModeHelper::MyRunnable::MyRunnable(
    /* [in] */ ZenModeHelper* host)
    : mHost(host)
{}

ZenModeHelper::MyRunnable::~MyRunnable()
{}

ECode ZenModeHelper::MyRunnable::Run()
{
    mHost->HandleRingerModeChanged();
    return NOERROR;
}

//===============================================================================
//                  ZenModeHelper::MyBroadcastReceiver
//===============================================================================

ZenModeHelper::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ ZenModeHelper* host)
    : mHost(host)
{}

ZenModeHelper::MyBroadcastReceiver::~MyBroadcastReceiver()
{}

ECode ZenModeHelper::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Boolean res;
    mHost->mHandler->Post(mHost->mRingerModeChanged, &res);
    return NOERROR;
}

//===============================================================================
//                  ZenModeHelper
//===============================================================================

ZenModeHelper::ZenModeHelper(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler)
    : mZenMode(0)
    , mPreviousRingerMode(-1)
{
    mContext = context;
    mHandler = handler;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&obj);
    mAppOps = IAppOpsManager::Probe(obj);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    ReadDefaultConfig(resources, (IZenModeConfig**)&mDefaultConfig);
    mConfig = mDefaultConfig;
    mSettingsObserver = new SettingsObserver();
    mSettingsObserver->constructor(mHandler, this);
    mSettingsObserver->Observe();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);

    filter->AddAction(IAudioManager::RINGER_MODE_CHANGED_ACTION);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);

    CArrayList::New((IArrayList**)&mCallbacks);

    mRingerModeChanged = (IRunnable*)new MyRunnable(this);
    mReceiver = (IBroadcastReceiver*)new MyBroadcastReceiver(this);
}

ZenModeHelper::~ZenModeHelper()
{}

ECode ZenModeHelper::ReadDefaultConfig(
    /* [in] */ IResources* resources,
    /* [out] */ IZenModeConfig** _config)
{
    VALIDATE_NOT_NULL(_config);
    *_config = NULL;

    AutoPtr<IIoUtils> utils;
    CIoUtils::AcquireSingleton((IIoUtils**)&utils);

    AutoPtr<IXmlResourceParser> parser;
    // try {
    ECode ec = resources->GetXml(R::xml::default_zen_mode_config, (IXmlResourceParser**)&parser);
    if (SUCCEEDED(ec)) {
        Int32 result;
        while ((IXmlPullParser::Probe(parser)->Next(&result), result) != IXmlPullParser::END_DOCUMENT) {
            AutoPtr<IZenModeConfigHelper> helper;
            CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
            AutoPtr<IZenModeConfig> config;
            helper->ReadXml(IXmlPullParser::Probe(parser), (IZenModeConfig**)&config);
            utils->CloseQuietly(ICloseable::Probe(parser));
            if (config != NULL) {
                *_config = config;
                REFCOUNT_ADD(*_config);
                utils->CloseQuietly(ICloseable::Probe(parser));
                return NOERROR;;
            }
        }
    }
    else {
        Slogger::W(TAG, "Error reading default zen mode config from resource");
    }
    // } catch (Exception e) {
    //     Slog.w(TAG, "Error reading default zen mode config from resource", e);
    // } finally {
    //     IoUtils.closeQuietly(parser);
    // }
    AutoPtr<IZenModeConfig> fig;
    CZenModeConfig::New((IZenModeConfig**)&fig);
    *_config = fig;
    REFCOUNT_ADD(*_config);
    utils->CloseQuietly(ICloseable::Probe(parser));
    return NOERROR;
}

void ZenModeHelper::AddCallback(
    /* [in] */ Callback* callback)
{
    mCallbacks->Add(TO_IINTERFACE(callback));
}

void ZenModeHelper::SetAudioManager(
    /* [in] */ IAudioManager* audioManager)
{
    mAudioManager = audioManager;
}

Int32 ZenModeHelper::GetZenModeListenerInterruptionFilter()
{
    switch (mZenMode) {
        case ISettingsGlobal::ZEN_MODE_OFF:
            return INotificationListenerService::INTERRUPTION_FILTER_ALL;
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS:
            return INotificationListenerService::INTERRUPTION_FILTER_PRIORITY;
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS:
            return INotificationListenerService::INTERRUPTION_FILTER_NONE;
        default:
            return 0;
    }
}

Int32 ZenModeHelper::ZenModeFromListenerInterruptionFilter(
    /* [in] */ Int32 listenerInterruptionFilter,
    /* [in] */ Int32 defValue)
{
    switch (listenerInterruptionFilter) {
        case INotificationListenerService::INTERRUPTION_FILTER_ALL:
            return ISettingsGlobal::ZEN_MODE_OFF;
        case INotificationListenerService::INTERRUPTION_FILTER_PRIORITY:
            return ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS;
        case INotificationListenerService::INTERRUPTION_FILTER_NONE:
            return ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS;
        default:
            return defValue;
    }
}

void ZenModeHelper::RequestFromListener(
    /* [in] */ Int32 interruptionFilter)
{
    const Int32 newZen = ZenModeFromListenerInterruptionFilter(interruptionFilter, -1);
    if (newZen != -1) {
        SetZenMode(newZen, String("listener"));
    }
}

Boolean ZenModeHelper::ShouldIntercept(
    /* [in] */ NotificationRecord* record)
{
    if (IsSystem(record)) {
        return FALSE;
    }
    switch (mZenMode) {
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS:
            // #notevenalarms
            ZenLog::TraceIntercepted(record, String("none"));
            return TRUE;
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS:
            if (IsAlarm(record)) {
                // Alarms are always priority
                return FALSE;
            }
            // allow user-prioritized packages through in priority mode
            if (record->GetPackagePriority() == INotification::PRIORITY_MAX) {
                ZenLog::TraceNotIntercepted(record, String("priorityApp"));
                return FALSE;
            }
            if (IsCall(record)) {
                Boolean allowCalls;
                mConfig->GetAllowCalls(&allowCalls);
                if (!allowCalls) {
                    ZenLog::TraceIntercepted(record, String("!allowCalls"));
                    return TRUE;
                }
                return ShouldInterceptAudience(record);
            }
            if (IsMessage(record)) {
                Boolean allowMessages;
                mConfig->GetAllowMessages(&allowMessages);
                if (!allowMessages) {
                    ZenLog::TraceIntercepted(record, String("!allowMessages"));
                    return TRUE;
                }
                return ShouldInterceptAudience(record);
            }
            if (IsEvent(record)) {
                Boolean allowEvents;
                mConfig->GetAllowEvents(&allowEvents);
                if (!allowEvents) {
                    ZenLog::TraceIntercepted(record, String("!allowEvents"));
                    return TRUE;
                }
                return FALSE;
            }
            ZenLog::TraceIntercepted(record, String("!priority"));
            return TRUE;
        default:
            return FALSE;
    }
}

Boolean ZenModeHelper::ShouldInterceptAudience(
    /* [in] */ NotificationRecord* record)
{
    if (!AudienceMatches(record->GetContactAffinity())) {
        ZenLog::TraceIntercepted(record, String("!audienceMatches"));
        return TRUE;
    }
    return FALSE;
}

Int32 ZenModeHelper::GetZenMode()
{
    return mZenMode;
}

void ZenModeHelper::SetZenMode(
    /* [in] */ Int32 zenModeValue,
    /* [in] */ const String& reason)
{
    ZenLog::TraceSetZenMode(zenModeValue, reason);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean res;
    global->PutInt32(resolver, ISettingsGlobal::ZEN_MODE, zenModeValue, &res);
}

void ZenModeHelper::UpdateZenMode()
{
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Int32 mode;
    global->GetInt32(resolver,
            ISettingsGlobal::ZEN_MODE, ISettingsGlobal::ZEN_MODE_OFF, &mode);
    if (mode != mZenMode) {
        ZenLog::TraceUpdateZenMode(mZenMode, mode);
    }
    mZenMode = mode;
    Boolean zen = mZenMode != ISettingsGlobal::ZEN_MODE_OFF;
    AutoPtr< ArrayOf<String> > exceptionPackages; // none (for now)

    // call restrictions
    Boolean allowCalls;
    mConfig->GetAllowCalls(&allowCalls);
    const Boolean muteCalls = zen && !allowCalls;
    mAppOps->SetRestriction(IAppOpsManager::OP_VIBRATE, IAudioAttributes::USAGE_NOTIFICATION_RINGTONE,
            muteCalls ? IAppOpsManager::MODE_IGNORED : IAppOpsManager::MODE_ALLOWED,
            exceptionPackages);
    mAppOps->SetRestriction(IAppOpsManager::OP_PLAY_AUDIO, IAudioAttributes::USAGE_NOTIFICATION_RINGTONE,
            muteCalls ? IAppOpsManager::MODE_IGNORED : IAppOpsManager::MODE_ALLOWED,
            exceptionPackages);

    // alarm restrictions
    const Boolean muteAlarms = mZenMode == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS;
    mAppOps->SetRestriction(IAppOpsManager::OP_VIBRATE, IAudioAttributes::USAGE_ALARM,
            muteAlarms ? IAppOpsManager::MODE_IGNORED : IAppOpsManager::MODE_ALLOWED,
            exceptionPackages);
    mAppOps->SetRestriction(IAppOpsManager::OP_PLAY_AUDIO, IAudioAttributes::USAGE_ALARM,
            muteAlarms ? IAppOpsManager::MODE_IGNORED : IAppOpsManager::MODE_ALLOWED,
            exceptionPackages);

    // force ringer mode into compliance
    if (mAudioManager != NULL) {
        Int32 ringerMode;
        mAudioManager->GetRingerMode(&ringerMode);
        Int32 forcedRingerMode = -1;
        if (mZenMode == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) {
            if (ringerMode != IAudioManager::RINGER_MODE_SILENT) {
                mPreviousRingerMode = ringerMode;
                if (DEBUG) Slogger::D(TAG, "Silencing ringer");
                forcedRingerMode = IAudioManager::RINGER_MODE_SILENT;
            }
        }
        else {
            if (ringerMode == IAudioManager::RINGER_MODE_SILENT) {
                if (DEBUG) Slogger::D(TAG, "Unsilencing ringer");
                forcedRingerMode = mPreviousRingerMode != -1 ? mPreviousRingerMode
                        : IAudioManager::RINGER_MODE_NORMAL;
                mPreviousRingerMode = -1;
            }
        }
        if (forcedRingerMode != -1) {
            mAudioManager->SetRingerMode(forcedRingerMode, FALSE /*checkZen*/);
            ZenLog::TraceSetRingerMode(forcedRingerMode);
        }
    }
    DispatchOnZenModeChanged();
}

void ZenModeHelper::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("mZenMode="));
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    String str;
    global->ZenModeToString(mZenMode, &str);
    pw->Println(str);
    pw->Print(prefix);
    pw->Print(String("mConfig="));
    pw->Println(mConfig);
    pw->Print(prefix);
    pw->Print(String("mDefaultConfig="));
    pw->Println(mDefaultConfig);
    pw->Print(prefix);
    pw->Print(String("mPreviousRingerMode="));
    pw->Println(mPreviousRingerMode);
    pw->Print(prefix);
    pw->Print(String("mDefaultPhoneApp="));
    pw->Println(mDefaultPhoneApp);
}

ECode ZenModeHelper::ReadXml(
    /* [in] */ IXmlPullParser* parser)
{
    AutoPtr<IZenModeConfigHelper> helper;
    CZenModeConfigHelper::AcquireSingleton((IZenModeConfigHelper**)&helper);
    AutoPtr<IZenModeConfig> config;
    helper->ReadXml(parser, (IZenModeConfig**)&config);
    if (config != NULL) {
        SetConfig(config);
    }
    return NOERROR;
}

ECode ZenModeHelper::WriteXml(
    /* [in] */ IXmlSerializer* out)
{
    return mConfig->WriteXml(out);
}

AutoPtr<IZenModeConfig> ZenModeHelper::GetConfig()
{
    return mConfig;
}

Boolean ZenModeHelper::SetConfig(
    /* [in] */ IZenModeConfig* config)
{
    Boolean res;
    if (config == NULL || (config->IsValid(&res), !res)) return FALSE;
    if (IObject::Probe(config)->Equals(mConfig, &res), res) return TRUE;
    ZenLog::TraceConfig(mConfig, config);
    mConfig = config;
    DispatchOnConfigChanged();
    Int32 hashCode;
    IObject::Probe(mConfig)->GetHashCode(&hashCode);
    String val = StringUtils::ToString(hashCode);
    AutoPtr<IContentResolver> resolver;
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->PutString(resolver, ISettingsGlobal::ZEN_MODE_CONFIG_ETAG, val, &res);
    UpdateZenMode();
    return TRUE;
}

void ZenModeHelper::HandleRingerModeChanged()
{
    if (mAudioManager != NULL) {
        // follow ringer mode if necessary
        Int32 ringerMode;
        mAudioManager->GetRingerMode(&ringerMode);
        Int32 newZen = -1;
        if (ringerMode == IAudioManager::RINGER_MODE_SILENT) {
            AutoPtr<IResources> resources;
            mContext->GetResources((IResources**)&resources);
            Boolean b = FALSE;
            resources->GetBoolean(
                        R::bool_::config_setZenModeWhenSilentModeOn, &b);
            if (mZenMode == ISettingsGlobal::ZEN_MODE_OFF && !b) {
                newZen = ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS;
            }
            else if (mZenMode != ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) {
                newZen = ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS;
            }
        }
        else if ((ringerMode == IAudioManager::RINGER_MODE_NORMAL
                || ringerMode == IAudioManager::RINGER_MODE_VIBRATE)
                && mZenMode == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) {
            newZen = ISettingsGlobal::ZEN_MODE_OFF;
        }
        if (newZen != -1) {
            ZenLog::TraceFollowRingerMode(ringerMode, mZenMode, newZen);
            SetZenMode(newZen, String("ringerMode"));
        }
    }
}

void ZenModeHelper::DispatchOnConfigChanged()
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<Callback> callback = (Callback*)IObject::Probe(obj);
        callback->OnConfigChanged();
    }
}

void ZenModeHelper::DispatchOnZenModeChanged()
{
    Int32 size;
    mCallbacks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mCallbacks->Get(i, (IInterface**)&obj);
        AutoPtr<Callback> callback = (Callback*)IObject::Probe(obj);
        callback->OnZenModeChanged();
    }
}

Boolean ZenModeHelper::IsSystem(
    /* [in] */ NotificationRecord* record)
{
    return record->IsCategory(INotification::CATEGORY_SYSTEM);
}

Boolean ZenModeHelper::IsAlarm(
    /* [in] */ NotificationRecord* record)
{
    return record->IsCategory(INotification::CATEGORY_ALARM)
            || record->IsAudioStream(IAudioManager::STREAM_ALARM)
            || record->IsAudioAttributesUsage(IAudioAttributes::USAGE_ALARM);
}

Boolean ZenModeHelper::IsEvent(
    /* [in] */ NotificationRecord* record)
{
    return record->IsCategory(INotification::CATEGORY_EVENT);
}

Boolean ZenModeHelper::IsCall(
    /* [in] */ NotificationRecord* record)
{
    String pkg;
    record->mSbn->GetPackageName(&pkg);
    return record != NULL && (IsDefaultPhoneApp(pkg)
            || record->IsCategory(INotification::CATEGORY_CALL));
}

Boolean ZenModeHelper::IsDefaultPhoneApp(
    /* [in] */ const String& pkg)
{
    if (mDefaultPhoneApp == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&obj);
        AutoPtr<ITelecomManager> telecomm = ITelecomManager::Probe(obj);
        if (telecomm != NULL) {
            telecomm->GetDefaultPhoneApp((IComponentName**)&mDefaultPhoneApp);
        }
        else {
            mDefaultPhoneApp = NULL;
        }
        if (DEBUG) Slogger::D(TAG, "Default phone app: %p", mDefaultPhoneApp.Get());
    }

    String name;
    return pkg != NULL && mDefaultPhoneApp != NULL
            && pkg.Equals((mDefaultPhoneApp->GetPackageName(&name), name));
}

Boolean ZenModeHelper::IsDefaultMessagingApp(
    /* [in] */ NotificationRecord* record)
{
    const Int32 userId = record->GetUserId();
    if (userId == IUserHandle::USER_NULL || userId == IUserHandle::USER_ALL) return FALSE;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    String defaultApp;
    secure->GetStringForUser(resolver, ISettingsSecure::SMS_DEFAULT_APPLICATION, userId, &defaultApp);

    String pkg;
    record->mSbn->GetPackageName(&pkg);

    return Objects::Equals(CoreUtils::Convert(defaultApp), CoreUtils::Convert(pkg));
}

Boolean ZenModeHelper::IsMessage(
    /* [in] */ NotificationRecord* record)
{
    return record->IsCategory(INotification::CATEGORY_MESSAGE) || IsDefaultMessagingApp(record);
}

Boolean ZenModeHelper::MatchesCallFilter(
    /* [in] */ IUserHandle* userHandle,
    /* [in] */ IBundle* extras,
    /* [in] */ ValidateNotificationPeople* validator,
    /* [in] */ Int32 contactsTimeoutMs,
    /* [in] */ Float timeoutAffinity)
{
    const Int32 zen = mZenMode;
    if (zen == ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS) return FALSE; // nothing gets through
    if (zen == ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS) {
        Boolean allowCalls;
        mConfig->GetAllowCalls(&allowCalls);
        if (!allowCalls) return FALSE; // no calls get through
        if (validator != NULL) {
            Float contactAffinity;
            validator->GetContactAffinity(userHandle, extras,
                    contactsTimeoutMs, timeoutAffinity, &contactAffinity);
            return AudienceMatches(contactAffinity);
        }
    }
    return TRUE;
}

Boolean ZenModeHelper::AudienceMatches(
    /* [in] */ Float contactAffinity)
{
    Int32 allowFrom;
    mConfig->GetAllowFrom(&allowFrom);
    switch (allowFrom) {
        case IZenModeConfig::SOURCE_ANYONE:
            return TRUE;
        case IZenModeConfig::SOURCE_CONTACT:
            return contactAffinity >= ValidateNotificationPeople::VALID_CONTACT;
        case IZenModeConfig::SOURCE_STAR:
            return contactAffinity >= ValidateNotificationPeople::STARRED_CONTACT;
        default:
            Slogger::W(TAG, "Encountered unknown source: %d", allowFrom);
            return TRUE;
    }
}

} // Notification
} // Server
} // Droid
} // Elastos
