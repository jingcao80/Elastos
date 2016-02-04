
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Provider.h>
#include "elastos/droid/server/notification/ZenLog.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String ZenLog::TAG("ZenLog");

const Int32 ZenLog::SIZE = Build::IS_DEBUGGABLE ? 100 : 20;

const AutoPtr< ArrayOf<Int64> > ZenLog::TIMES = ArrayOf<Int64>::Alloc(SIZE);
const AutoPtr< ArrayOf<Int32> > ZenLog::TYPES = ArrayOf<Int32>::Alloc(SIZE);
const AutoPtr< ArrayOf<String> > ZenLog::MSGS = ArrayOf<String>::Alloc(SIZE);

static AutoPtr<ISimpleDateFormat> InitFORMAT()
{
    AutoPtr<ISimpleDateFormat> format;
    CSimpleDateFormat::New(String("MM-dd HH:mm:ss.SSS"), (ISimpleDateFormat**)&format);
    return format;
}

const AutoPtr<ISimpleDateFormat> ZenLog::FORMAT = InitFORMAT();

const Int32 ZenLog::TYPE_INTERCEPTED;
const Int32 ZenLog::TYPE_ALLOW_DISABLE;
const Int32 ZenLog::TYPE_SET_RINGER_MODE;
const Int32 ZenLog::TYPE_DOWNTIME;
const Int32 ZenLog::TYPE_SET_ZEN_MODE;
const Int32 ZenLog::TYPE_UPDATE_ZEN_MODE;
const Int32 ZenLog::TYPE_EXIT_CONDITION;
const Int32 ZenLog::TYPE_SUBSCRIBE;
const Int32 ZenLog::TYPE_UNSUBSCRIBE;
const Int32 ZenLog::TYPE_CONFIG;
const Int32 ZenLog::TYPE_FOLLOW_RINGER_MODE;
const Int32 ZenLog::TYPE_NOT_INTERCEPTED;
const Int32 ZenLog::TYPE_DISABLE_EFFECTS;

Int32 ZenLog::sNext;
Int32 ZenLog::sSize;
Object ZenLog::lock;

void ZenLog::TraceIntercepted(
    /* [in] */ NotificationRecord* record,
    /* [in] */ const String& reason)
{
    if (record != NULL && record->IsIntercepted()) return;  // already logged
    StringBuilder buider;
    buider += record->GetKey();
    buider += ",";
    buider += reason;
    Append(TYPE_INTERCEPTED, buider.ToString());
}

void ZenLog::TraceNotIntercepted(
    /* [in] */ NotificationRecord* record,
    /* [in] */ const String& reason)
{
    if (record != NULL && record->mIsUpdate) return;  // already logged
    StringBuilder buider;
    buider += record->GetKey();
    buider += ",";
    buider += reason;
    Append(TYPE_NOT_INTERCEPTED, buider.ToString());
}

void ZenLog::TraceSetRingerMode(
    /* [in] */ Int32 ringerMode)
{
    Append(TYPE_SET_RINGER_MODE, RingerModeToString(ringerMode));
}

void ZenLog::TraceDowntime(
    /* [in] */ Boolean inDowntime,
    /* [in] */ Int32 day,
    /* [in] */ IArraySet* days)
{
    StringBuilder buider;
    buider += inDowntime;
    buider += ",day=";
    buider += day;
    buider += ",days=";
    buider += days;
    Append(TYPE_DOWNTIME, buider.ToString());
}

void ZenLog::TraceSetZenMode(
    /* [in] */ Int32 mode,
    /* [in] */ const String& reason)
{
    StringBuilder buider;
    buider += ZenModeToString(mode);
    buider += ",";
    buider += reason;
    Append(TYPE_SET_ZEN_MODE, buider.ToString());
}

void ZenLog::TraceUpdateZenMode(
    /* [in] */ Int32 fromMode,
    /* [in] */ Int32 toMode)
{
    StringBuilder buider;
    buider += ZenModeToString(fromMode);
    buider += " -> ";
    buider += ZenModeToString(toMode);
    Append(TYPE_UPDATE_ZEN_MODE, buider.ToString());
}

void ZenLog::TraceExitCondition(
    /* [in] */ ICondition* c,
    /* [in] */ IComponentName* component,
    /* [in] */ const String& reason)
{
    StringBuilder buider;
    buider += c;
    buider += ",";
    buider += ComponentToString(component);
    buider += ",";
    buider += reason;
    Append(TYPE_EXIT_CONDITION, buider.ToString());
}

void ZenLog::TraceSubscribe(
    /* [in] */ IUri* uri,
    /* [in] */ IIConditionProvider* provider,
    /* [in] */ ECode e)
{
    StringBuilder buider;
    buider += uri;
    buider += ",";
    buider += SubscribeResult(provider, e);
    Append(TYPE_SUBSCRIBE, buider.ToString());
}

void ZenLog::TraceUnsubscribe(
    /* [in] */ IUri* uri,
    /* [in] */ IIConditionProvider* provider,
    /* [in] */ ECode e)
{
    StringBuilder buider;
    buider += uri;
    buider += ",";
    buider += SubscribeResult(provider, e);
    Append(TYPE_UNSUBSCRIBE, buider.ToString());
}

void ZenLog::TraceConfig(
    /* [in] */ IZenModeConfig* oldConfig,
    /* [in] */ IZenModeConfig* newConfig)
{
    String str = String(NULL);
    if (newConfig != NULL ) {
        IObject::Probe(newConfig)->ToString(&str);
    }
    Append(TYPE_CONFIG, str);
}

void ZenLog::TraceFollowRingerMode(
    /* [in] */ int ringerMode,
    /* [in] */ int oldZen,
    /* [in] */ int newZen)
{
    StringBuilder buider;
    buider += RingerModeToString(ringerMode);
    buider += ", ";
    buider += ZenModeToString(oldZen);
    buider += " -> ";
    buider += ZenModeToString(newZen);
    Append(TYPE_FOLLOW_RINGER_MODE, buider.ToString());
}

void ZenLog::TraceDisableEffects(
    /* [in] */ NotificationRecord* record,
    /* [in] */ const String& reason)
{
    StringBuilder buider;
    buider += record->GetKey();
    buider += ",";
    buider += reason;
    Append(TYPE_DISABLE_EFFECTS, buider.ToString());
}

String ZenLog::SubscribeResult(
    /* [in] */ IIConditionProvider* provider,
    /* [in] */ ECode e)
{
    return provider == NULL ? String("no provider") : e != NOERROR ? StringUtils::ToHexString(e) : String("ok");
}

String ZenLog::TypeToString(
    /* [in] */ Int32 type)
{
    switch (type) {
        case TYPE_INTERCEPTED: return String("intercepted");
        case TYPE_ALLOW_DISABLE: return String("allow_disable");
        case TYPE_SET_RINGER_MODE: return String("set_ringer_mode");
        case TYPE_DOWNTIME: return String("downtime");
        case TYPE_SET_ZEN_MODE: return String("set_zen_mode");
        case TYPE_UPDATE_ZEN_MODE: return String("update_zen_mode");
        case TYPE_EXIT_CONDITION: return String("exit_condition");
        case TYPE_SUBSCRIBE: return String("subscribe");
        case TYPE_UNSUBSCRIBE: return String("unsubscribe");
        case TYPE_CONFIG: return String("config");
        case TYPE_FOLLOW_RINGER_MODE: return String("follow_ringer_mode");
        case TYPE_NOT_INTERCEPTED: return String("not_intercepted");
        case TYPE_DISABLE_EFFECTS: return String("disable_effects");
        default: return String("unknown");
    }
}

String ZenLog::RingerModeToString(
    /* [in] */ Int32 ringerMode)
{
    switch (ringerMode) {
        case IAudioManager::RINGER_MODE_SILENT: return String("silent");
        case IAudioManager::RINGER_MODE_VIBRATE: return String("vibrate");
        case IAudioManager::RINGER_MODE_NORMAL: return String("normal");
        default: return String("unknown");
    }
}

String ZenLog::ZenModeToString(
    /* [in] */ Int32 zenMode)
{
    switch (zenMode) {
        case ISettingsGlobal::ZEN_MODE_OFF: return String("off");
        case ISettingsGlobal::ZEN_MODE_IMPORTANT_INTERRUPTIONS: return String("important_interruptions");
        case ISettingsGlobal::ZEN_MODE_NO_INTERRUPTIONS: return String("no_interruptions");
        default: return String("unknown");
    }
}

String ZenLog::ComponentToString(
    /* [in] */ IComponentName* component)
{
    String str;
    if (component != NULL) {
        component->ToShortString(&str);
        return str;
    }
    return String(NULL);
}

void ZenLog::Append(
    /* [in] */ Int32 type,
    /* [in] */ const String& msg)
{
    synchronized(lock) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        sys->GetCurrentTimeMillis(&(*TIMES)[sNext]);
        (*TYPES)[sNext] = type;
        (*MSGS)[sNext] = msg;
        sNext = (sNext + 1) % SIZE;
        if (sSize < SIZE) {
            sSize++;
        }
    }
    StringBuilder buider;
    buider += TypeToString(type);
    buider += ": ";
    buider += msg;
    Slogger::D(TAG, buider.ToString());
}

void ZenLog::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    synchronized(lock) {
        const Int32 start = (sNext - sSize + SIZE) % SIZE;
        for (Int32 i = 0; i < sSize; i++) {
            const Int32 j = (start + i) % SIZE;
            pw->Print(prefix);
            AutoPtr<IDate> date;
            CDate::New((*TIMES)[j], (IDate**)&date);
            String str;
            IDateFormat::Probe(FORMAT)->Format(date, &str);
            pw->Print(str);
            pw->PrintChar(' ');
            pw->Print(TypeToString((*TYPES)[j]));
            pw->Print(String(": "));
            pw->Println((*MSGS)[j]);
        }
    }
}

} // Notification
} // Server
} // Droid
} // Elastos
