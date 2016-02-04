
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_ZENLOG_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_ZENLOG_H__

#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/server/notification/NotificationRecord.h"

using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IIConditionProvider;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Text::ISimpleDateFormat;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class ZenLog
    : public Object
{
public:
    static CARAPI_(void) TraceIntercepted(
        /* [in] */ NotificationRecord* record,
        /* [in] */ const String& reason);

    static CARAPI_(void) TraceNotIntercepted(
        /* [in] */ NotificationRecord* record,
        /* [in] */ const String& reason);

    static CARAPI_(void) TraceSetRingerMode(
        /* [in] */ Int32 ringerMode);

    static CARAPI_(void) TraceDowntime(
        /* [in] */ Boolean inDowntime,
        /* [in] */ Int32 day,
        /* [in] */ IArraySet* days);

    static CARAPI_(void) TraceSetZenMode(
        /* [in] */ Int32 mode,
        /* [in] */ const String& reason);

    static CARAPI_(void) TraceUpdateZenMode(
        /* [in] */ Int32 fromMode,
        /* [in] */ Int32 toMode);

    static CARAPI_(void) TraceExitCondition(
        /* [in] */ ICondition* c,
        /* [in] */ IComponentName* component,
        /* [in] */ const String& reason);

    static CARAPI_(void) TraceSubscribe(
        /* [in] */ IUri* uri,
        /* [in] */ IIConditionProvider* provider,
        /* [in] */ ECode e);

    static CARAPI_(void) TraceUnsubscribe(
        /* [in] */ IUri* uri,
        /* [in] */ IIConditionProvider* provider,
        /* [in] */ ECode e);

    static CARAPI_(void) TraceConfig(
        /* [in] */ IZenModeConfig* oldConfig,
        /* [in] */ IZenModeConfig* newConfig);

    static CARAPI_(void) TraceFollowRingerMode(
        /* [in] */ int ringerMode,
        /* [in] */ int oldZen,
        /* [in] */ int newZen);

    static CARAPI_(void) TraceDisableEffects(
        /* [in] */ NotificationRecord* record,
        /* [in] */ const String& reason);

    static CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

private:
    static CARAPI_(String) SubscribeResult(
        /* [in] */ IIConditionProvider* provider,
        /* [in] */ ECode e);

    static CARAPI_(String) TypeToString(
        /* [in] */ Int32 type);

    static CARAPI_(String) RingerModeToString(
        /* [in] */ Int32 ringerMode);

    static CARAPI_(String) ZenModeToString(
        /* [in] */ Int32 zenMode);

    static CARAPI_(String) ComponentToString(
        /* [in] */ IComponentName* component);

    static CARAPI_(void) Append(
        /* [in] */ Int32 type,
        /* [in] */ const String& msg);

private:
    static const String TAG;

    static const Int32 SIZE;

    static const AutoPtr< ArrayOf<Int64> > TIMES;
    static const AutoPtr< ArrayOf<Int32> > TYPES;
    static const AutoPtr< ArrayOf<String> > MSGS;

    static const AutoPtr<ISimpleDateFormat> FORMAT;

    static const Int32 TYPE_INTERCEPTED = 1;
    static const Int32 TYPE_ALLOW_DISABLE = 2;
    static const Int32 TYPE_SET_RINGER_MODE = 3;
    static const Int32 TYPE_DOWNTIME = 4;
    static const Int32 TYPE_SET_ZEN_MODE = 5;
    static const Int32 TYPE_UPDATE_ZEN_MODE = 6;
    static const Int32 TYPE_EXIT_CONDITION = 7;
    static const Int32 TYPE_SUBSCRIBE = 8;
    static const Int32 TYPE_UNSUBSCRIBE = 9;
    static const Int32 TYPE_CONFIG = 10;
    static const Int32 TYPE_FOLLOW_RINGER_MODE = 11;
    static const Int32 TYPE_NOT_INTERCEPTED = 12;
    static const Int32 TYPE_DISABLE_EFFECTS = 13;

    static Int32 sNext;
    static Int32 sSize;
    static Object lock;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_ZENLOG_H__
