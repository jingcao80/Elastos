
#include "elastos/droid/internal/telephony/dataconnection/DcTesterFailBringUpAll.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTesterFailBringUpAll::SubBroadcastReceiver
//=============================================================================
DcTesterFailBringUpAll::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcTesterFailBringUpAll* host)
    : mHost(host)
{}

ECode DcTesterFailBringUpAll::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                String action = intent.getAction();
                if (DBG) log("sIntentReceiver.onReceive: action=" + action);
                if (action.equals(mActionFailBringUp)) {
                    mFailBringUp.saveParameters(intent, "sFailBringUp");
                } else if (action.equals(mPhone.getActionDetached())) {
                    // Counter is MAX, bringUp/retry will always fail
                    log("simulate detaching");
                    mFailBringUp.saveParameters(Integer.MAX_VALUE,
                            DcFailCause.LOST_CONNECTION.getErrorCode(),
                            DcFailBringUp.DEFAULT_SUGGESTED_RETRY_TIME);
                } else if (action.equals(mPhone.getActionAttached())) {
                    // Counter is 0 next bringUp/retry will succeed
                    log("simulate attaching");
                    mFailBringUp.saveParameters(0, DcFailCause.NONE.getErrorCode(),
                            DcFailBringUp.DEFAULT_SUGGESTED_RETRY_TIME);
                } else {
                    if (DBG) log("onReceive: unknown action=" + action);
                }

#endif
}

//=============================================================================
// DcTesterFailBringUpAll
//=============================================================================
CAR_INTERFACE_IMPL(DcTesterFailBringUpAll, Object, IDcTesterFailBringUpAll)

const String DcTesterFailBringUpAll::LOG__TAG("DcTesterFailBrinupAll");
const Boolean DcTesterFailBringUpAll::DBG = true;

DcTesterFailBringUpAll::DcTesterFailBringUpAll()
{
    mActionFailBringUp = DcFailBringUp::INTENT_BASE + "." + DcFailBringUp::ACTION_FAIL_BRINGUP;
    mFailBringUp = new DcFailBringUp();
    mIntentReceiver = new SubBroadcastReceiver(this);
}

ECode DcTesterFailBringUpAll::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IHandler* handler)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPhone = phone;
        if (Build.IS_DEBUGGABLE) {
            IntentFilter filter = new IntentFilter();
            filter.addAction(mActionFailBringUp);
            log("register for intent action=" + mActionFailBringUp);
            filter.addAction(mPhone.getActionDetached());
            log("register for intent action=" + mPhone.getActionDetached());
            filter.addAction(mPhone.getActionAttached());
            log("register for intent action=" + mPhone.getActionAttached());
            phone.getContext().registerReceiver(mIntentReceiver, filter, null, handler);
        }

#endif
}

ECode DcTesterFailBringUpAll::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Build.IS_DEBUGGABLE) {
            mPhone.getContext().unregisterReceiver(mIntentReceiver);
        }

#endif
}

ECode DcTesterFailBringUpAll::GetDcFailBringUp(
    /* [out] */ DcFailBringUp** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mFailBringUp;

#endif
}

ECode DcTesterFailBringUpAll::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, s);

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
