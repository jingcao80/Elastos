
#include "elastos/droid/server/am/ReceiverList.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL_LIGHT(ReceiverList, Object, IProxyDeathRecipient)

ReceiverList::ReceiverList(
    /* [in] */ CActivityManagerService* owner,
    /* [in] */ ProcessRecord* callerApp,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 userId,
    /* [in] */ IIntentReceiver* receiver)
    : mOwner(owner)
    , mReceiver(receiver)
    , mApp(callerApp)
    , mPid(pid)
    , mUid(uid)
    , mUserId(userId)
    , mLinkedToDeath(FALSE)
{
}

ReceiverList::~ReceiverList()
{
    for (Iterator it = Begin(); it !=  End(); ++it) {
        AutoPtr<BroadcastFilter> filter = *it;
        filter->mReceiverList = NULL;
    }
    Clear();
}

// Want object identity, not the array identity we are inheriting.
ECode ReceiverList::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TO_IINTERFACE(this) == TO_IINTERFACE(o);
    return NOERROR;
}

ECode ReceiverList::HashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode ReceiverList::ProxyDied()
{
    Slogger::W("ReceiverList", " >> %p ProxyDied()", (IProxyDeathRecipient*)this);
    mLinkedToDeath = FALSE;
    assert(0);
    return NOERROR;
    // return mOwner->UnregisterReceiver(mReceiver);
}

void ReceiverList::DumpLocal(
    /* [in] */ IPrintWriter* pw,
    /* [out] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("app="));
    pw->Print(mApp != NULL ? mApp->ToShortString() : String(NULL));
    pw->Print(String(" pid="));
    pw->Print(mPid);
    pw->Print(String(" uid="));
    pw->Print(mUid);
    pw->Print(String(" user="));
    pw->Println(mUserId);
    if (mCurBroadcast != NULL || mLinkedToDeath) {
        pw->Print(prefix);
        pw->Print(String("curBroadcast="));
        pw->Print(mCurBroadcast->ToString());
        pw->Print(String(" linkedToDeath="));
        pw->Println(mLinkedToDeath);
    }
}

void ReceiverList::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [out] */ const String& prefix)
{
    AutoPtr<IPrinter> pr;
    // CPrintWriterPrinter::New(pw, (IPrinter**)&pr);
    DumpLocal(pw, prefix);
    String p2 = prefix + "  ";
    Int32 i = 0;
    for (Iterator it = Begin(); it !=  End(); ++it, ++i) {
        AutoPtr<BroadcastFilter> bf = *it;
        pw->Print(prefix);
        pw->Print(String("Filter #"));
        pw->Print(i);
        pw->Print(String(": BroadcastFilter{"));
        pw->Print(StringUtils::ToString((Int32)bf.Get(), 16));
        pw->Println('}');
        bf->DumpInReceiverList(pw, pr, p2);
    }
}

String ReceiverList::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }
    StringBuilder sb(128);
    sb += "ReceiverList{";
    sb += StringUtils::ToString(Int32(this), 16);
    sb += ", pid=";
    sb += mPid;
    sb += ", processName=";
    sb += (mApp != NULL ? mApp->mProcessName : "(unknown name)");
    sb += ", uid=";
    sb += mUid;
    sb += ", userId=";
    sb += mUserId;
    sb += "receiver=";
    sb += !IProxy::Probe(mReceiver) ? " local:" : " remote:";
    sb += StringUtils::ToString((Int32)IBinder::Probe(mReceiver), 16);
    sb += "}";
    return mStringName = sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
