
#include "elastos/droid/internal/telephony/dataconnection/DcRetryAlarmController.h"
#include <Elastos.Droid.App.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcRetryAlarmController::SubBroadcastReceiver
//=============================================================================
DcRetryAlarmController::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcRetryAlarmController* host)
    : mHost(host)
{}

ECode DcRetryAlarmController::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                String action = intent.getAction();
                if (TextUtils.isEmpty(action)) {
                    // Our mActionXxxx's could be null when disposed this could match an empty action.
                    log("onReceive: ignore empty action='" + action + "'");
                    return;
                }
                if (TextUtils.equals(action, mActionRetry)) {
                    if (!intent.hasExtra(INTENT_RETRY_ALARM_WHAT)) {
                        throw new RuntimeException(mActionRetry + " has no INTENT_RETRY_ALRAM_WHAT");
                    }
                    if (!intent.hasExtra(INTENT_RETRY_ALARM_TAG)) {
                        throw new RuntimeException(mActionRetry + " has no INTENT_RETRY_ALRAM_TAG");
                    }
                    int what = intent.getIntExtra(INTENT_RETRY_ALARM_WHAT, Integer.MAX_VALUE);
                    int tag = intent.getIntExtra(INTENT_RETRY_ALARM_TAG, Integer.MAX_VALUE);
                    if (DBG) {
                        log("onReceive: action=" + action
                                + " sendMessage(what:" + mDc.getWhatToString(what)
                                + ", tag:" + tag + ")");
                    }
                    mDc.sendMessage(mDc.obtainMessage(what, tag, 0));
                } else {
                    if (DBG) log("onReceive: unknown action=" + action);
                }

#endif
}

//=============================================================================
// DcRetryAlarmController
//=============================================================================
CAR_INTERFACE_IMPL(DcRetryAlarmController, Object, IDcRetryAlarmController)

const Boolean DcRetryAlarmController::DBG = true;
const String DcRetryAlarmController::INTENT_RETRY_ALARM_WHAT("what");
const String DcRetryAlarmController::INTENT_RETRY_ALARM_TAG("tag");

DcRetryAlarmController::DcRetryAlarmController()
    : mLogTag("DcRac")
{
    mIntentReceiver = new SubBroadcastReceiver(this);
}

ECode DcRetryAlarmController::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDataConnection* dc)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mLogTag = dc.getName();
        mPhone = phone;
        mDc = dc;
        mAlarmManager = (AlarmManager) mPhone.getContext().getSystemService(Context.ALARM_SERVICE);
        mActionRetry = mDc.getClass().getCanonicalName() + "." + mDc.getName() + ".action_retry";
        IntentFilter filter = new IntentFilter();
        filter.addAction(mActionRetry);
        log("DcRetryAlarmController: register for intent action=" + mActionRetry);
        mPhone.getContext().registerReceiver(mIntentReceiver, filter, null, mDc.getHandler());

#endif
}

ECode DcRetryAlarmController::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) log("dispose");
        mPhone.getContext().unregisterReceiver(mIntentReceiver);
        mPhone = null;
        mDc = null;
        mAlarmManager = null;
        mActionRetry = null;

#endif
}

ECode DcRetryAlarmController::GetSuggestedRetryTime(
    /* [in] */ IDataConnection* dc,
    /* [in] */ IAsyncResult* ar,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int retryDelay;
        DataCallResponse response = (DataCallResponse) ar.result;
        retryDelay = response.suggestedRetryTime;
        if (retryDelay == RILConstants.MAX_INT) {
            if (DBG) log("getSuggestedRetryTime: suggestedRetryTime is MAX_INT, retry NOT needed");
            retryDelay = -1;
        } else if (retryDelay >= 0) {
            if (DBG) log("getSuggestedRetryTime: suggestedRetryTime is >= 0 use it");
        } else if (dc.mRetryManager.isRetryNeeded()) {
            retryDelay = dc.mRetryManager.getRetryTimer();
            if (retryDelay < 0) {
                retryDelay = 0;
            }
            if (DBG) log("getSuggestedRetryTime: retry is needed");
        } else {
            if (DBG) log("getSuggestedRetryTime: retry is NOT needed");
            retryDelay = -1;
        }
        if (DBG) {
            log("getSuggestedRetryTime: " + retryDelay + " response=" + response + " dc=" + dc);
        }
        return retryDelay;

#endif
}

ECode DcRetryAlarmController::StartRetryAlarm(
    /* [in] */ Int32 what,
    /* [in] */ Int32 tag,
    /* [in] */ Int32 delay)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Intent intent = new Intent(mActionRetry);
        intent.putExtra(INTENT_RETRY_ALARM_WHAT, what);
        intent.putExtra(INTENT_RETRY_ALARM_TAG, tag);
        if (DBG) {
            log("startRetryAlarm: next attempt in " + (delay / 1000) + "s" +
                    " what=" + what + " tag=" + tag);
        }
        PendingIntent retryIntent = PendingIntent.getBroadcast (mPhone.getContext(), 0,
                                        intent, PendingIntent.FLAG_UPDATE_CURRENT);
        mAlarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP,
                SystemClock.elapsedRealtime() + delay, retryIntent);

#endif
}

ECode DcRetryAlarmController::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        StringBuilder sb = new StringBuilder();
        sb.append(mLogTag).append(" [dcRac] ");
        sb.append(" mPhone=").append(mPhone);
        sb.append(" mDc=").append(mDc);
        sb.append(" mAlaramManager=").append(mAlarmManager);
        sb.append(" mActionRetry=").append(mActionRetry);
        return sb.toString();

#endif
}

ECode DcRetryAlarmController::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(mLogTag, "[dcRac] " + s);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
