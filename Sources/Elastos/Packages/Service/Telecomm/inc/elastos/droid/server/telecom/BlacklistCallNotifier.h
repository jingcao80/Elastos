
#ifndef __ELASTOS_DROID_SERVER_TELECOM_BLACKLISTCALLNOTIFIER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_BLACKLISTCALLNOTIFIER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::Style::IRelativeSizeSpan;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Handles notifying the user of any blacklisted calls or messages.
 */
class BlacklistCallNotifier
    : public CallsManagerListenerBase
{
private:
    // used to track blacklisted calls and messages
    class BlacklistedItemInfo
        : public Object
    {
    public:
        BlacklistedItemInfo(
            /* [in] */ const String& number,
            /* [in] */ Int64 date,
            /* [in] */ Int32 matchType);

        String mNumber;

        Int64 mDate;

        Int32 mMatchType;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    /** {@inheritDoc} */
    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    /* package */
    CARAPI NotifyBlacklistedCall(
        /* [in] */ const String& number,
        /* [in] */ Int64 date,
        /* [in] */ Int32 matchType);

    /* package */
    CARAPI NotifyBlacklistedMessage(
        /* [in] */ const String& number,
        /* [in] */ Int64 date,
        /* [in] */ Int32 matchType);

    CARAPI CancelBlacklistedNotification(
        /* [in] */ Int32 type);

    /* package */
    static CARAPI GetUnblockNumberFromNotificationPendingIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 type,
        /* [out] */ IPendingIntent** result);

private:
    CARAPI NotifyBlacklistedItem(
        /* [in] */ const String& number,
        /* [in] */ Int64 date,
        /* [in] */ Int32 matchType,
        /* [in] */ Int32 notificationId);

    CARAPI CreateClearBlacklistedCallsIntent(
        /* [out] */ IPendingIntent** result);

    CARAPI CreateClearBlacklistedMessagesIntent(
        /* [out] */ IPendingIntent** result);

    CARAPI FormatSingleCallLine(
        /* [in] */ const String& caller,
        /* [in] */ Int64 date,
        /* [out] */ ICharSequence** result);

    static CARAPI_(AutoPtr<IRelativeSizeSpan>) InitTIME_SPAN();

public:
    static const Int32 BLACKLISTED_CALL_NOTIFICATION;

    static const Int32 BLACKLISTED_MESSAGE_NOTIFICATION;

private:
    static AutoPtr<IRelativeSizeSpan> TIME_SPAN;

    static const Boolean DEBUG;

    AutoPtr<IContext> mContext;

    AutoPtr<INotificationManager> mNotificationManager;

    AutoPtr<IArrayList> mBlacklistedCalls;

    AutoPtr<IArrayList> mBlacklistedMessages;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_BLACKLISTCALLNOTIFIER_H__
