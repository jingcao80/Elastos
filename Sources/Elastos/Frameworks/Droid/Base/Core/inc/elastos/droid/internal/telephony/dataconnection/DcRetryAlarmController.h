
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCRETRYALARMCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCRETRYALARMCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.Droid.App.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Os::IAsyncResult;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * The Data Connection Retry Alarm Controller.
 */
class DcRetryAlarmController
    : public Object
    , public IDcRetryAlarmController
{
private:
    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        SubBroadcastReceiver(
            /* [in] */ DcRetryAlarmController* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcRetryAlarmController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcRetryAlarmController();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IDataConnection* dc);

    /**
     * Dispose of resources when shutting down
     */
    CARAPI Dispose();

    /**
     * Using dc.mRetryManager and the result of the SETUP_DATA_CALL determine
     * the retry delay.
     *
     * @param dc is the DataConnection
     * @param ar is the result from SETUP_DATA_CALL
     * @return < 0 if no retry is needed otherwise the delay to the next SETUP_DATA_CALL
     */
    CARAPI GetSuggestedRetryTime(
        /* [in] */ IDataConnection* dc,
        /* [in] */ IAsyncResult* ar,
        /* [out] */ Int32* result);

    CARAPI StartRetryAlarm(
        /* [in] */ Int32 what,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 delay);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI Log(
        /* [in] */ const char *fmt, ...);

private:
    String mLogTag;

    static const Boolean DBG;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IDataConnection> mDc;

    AutoPtr<IAlarmManager> mAlarmManager;

    // The Intent action for retrying and its two extra's
    String mActionRetry;

    static const String INTENT_RETRY_ALARM_WHAT;

    static const String INTENT_RETRY_ALARM_TAG;

    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCRETRYALARMCONTROLLER_H__
