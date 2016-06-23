
#include "elastos/droid/internal/telephony/dataconnection/DcTesterDeactivateAll.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DcController.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Build;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

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
    String action;
    intent->GetAction(&action);
    if (DBG) Log("sIntentReceiver.onReceive: action=%s", action.string());
    String actionDetached;
    mHost->mPhone->GetActionDetached(&actionDetached);
    if (action.Equals(sActionDcTesterDeactivateAll)
            || action.Equals(actionDetached)) {
        mHost->Log("Send DEACTIVATE to all Dcc's");
        if (mHost->mDcc != NULL) {
            AutoPtr<IIterator> it;
            ((DcController*) mHost->mDcc.Get())->mDcListAll->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> obj;
                it->GetNext((IInterface**)&obj);
                AutoPtr<IDataConnection> dc = IDataConnection::Probe(obj);
                ((DataConnection*) dc.Get())->TearDownNow();
            }
        } else {
            if (DBG) Log("onReceive: mDcc is null, ignoring");
        }
    } else {
        if (DBG) Log("onReceive: unknown action=%s", action.string());
    }
    return NOERROR;
}

//=============================================================================
// DcTesterDeactivateAll
//=============================================================================
CAR_INTERFACE_IMPL(DcTesterDeactivateAll, Object, IDcTesterDeactivateAll)

const String DcTesterDeactivateAll::LOG__TAG("DcTesterDeacativeAll");
const Boolean DcTesterDeactivateAll::DBG = TRUE;
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
    mPhone = phone;
    mDcc = dcc;
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(sActionDcTesterDeactivateAll);
        Log("register for intent action=%s", sActionDcTesterDeactivateAll.string());
        String actionDetached;
        mPhone->GetActionDetached(&actionDetached);
        filter->AddAction(actionDetached);
        Log("register for intent action=%s", actionDetached.string());
        AutoPtr<IContext> context;
        IPhone::Probe(phone)->GetContext((IContext**)&context);
        AutoPtr<IIntent> intent;
        context->RegisterReceiver(sIntentReceiver, filter, String(NULL), handler, (IIntent**)&intent);
    }
    return NOERROR;
}

ECode DcTesterDeactivateAll::Dispose()
{
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        context->UnregisterReceiver(sIntentReceiver);
    }
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcTesterDeactivateAll::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(LOG__TAG, msgBuf);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
