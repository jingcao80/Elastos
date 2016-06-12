
#include "elastos/droid/internal/telephony/dataconnection/DcTesterDeactivateAll.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTesterDeactivateAll::SubBroadcastReceiver
//=============================================================================
DcTesterDeactivateAll::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcTesterDeactivateAll* host)
    : mHost(host)
{}

ECode DcTesterDeactivateAll::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                String action = intent.getAction();
                if (DBG) log("sIntentReceiver.onReceive: action=" + action);
                if (action.equals(sActionDcTesterDeactivateAll)
                        || action.equals(mPhone.getActionDetached())) {
                    log("Send DEACTIVATE to all Dcc's");
                    if (mDcc != null) {
                        for (DataConnection dc : mDcc.mDcListAll) {
                            dc.tearDownNow();
                        }
                    } else {
                        if (DBG) log("onReceive: mDcc is null, ignoring");
                    }
                } else {
                    if (DBG) log("onReceive: unknown action=" + action);
                }

#endif
}

//=============================================================================
// DcTesterDeactivateAll
//=============================================================================
CAR_INTERFACE_IMPL(DcTesterDeactivateAll, Object, IDcTesterDeactivateAll)

const String DcTesterDeactivateAll::LOG__TAG("DcTesterDeacativeAll");
const Boolean DcTesterDeactivateAll::DBG = true;
String DcTesterDeactivateAll::sActionDcTesterDeactivateAll("com.android.internal.telephony.dataconnection.action_deactivate_all");

DcTesterDeactivateAll::DcTesterDeactivateAll()
{
    sIntentReceiver = new SubBroadcastReceiver(this);
}

ECode DcTesterDeactivateAll::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IDcController* dcc,
    /* [in] */ IHandler* handler)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mPhone = phone;
        mDcc = dcc;
        if (Build.IS_DEBUGGABLE) {
            IntentFilter filter = new IntentFilter();
            filter.addAction(sActionDcTesterDeactivateAll);
            log("register for intent action=" + sActionDcTesterDeactivateAll);
            filter.addAction(mPhone.getActionDetached());
            log("register for intent action=" + mPhone.getActionDetached());
            phone.getContext().registerReceiver(sIntentReceiver, filter, null, handler);
        }

#endif
}

ECode DcTesterDeactivateAll::Dispose()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (Build.IS_DEBUGGABLE) {
            mPhone.getContext().unregisterReceiver(sIntentReceiver);
        }

#endif
}

ECode DcTesterDeactivateAll::Log(
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
