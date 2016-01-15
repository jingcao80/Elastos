
#include "elastos/droid/server/am/IntentBindRecord.h"
// #include "elastos/droid/server/am/CServiceRecord.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

IntentBindRecord::IntentBindRecord(
    /* [in] */ CServiceRecord* service,
    /* [in] */ IIntentFilterComparison* intent)
    : mService(service)
    , mIntent(intent)
    , mRequested(FALSE)
    , mReceived(FALSE)
    , mHasBound(FALSE)
    , mDoRebind(FALSE)
{
}

IntentBindRecord::~IntentBindRecord()
{
}


void IntentBindRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("service="));
    assert(0);
    // pw->Println(mService->ToString());
    DumpInService(pw, prefix);
}

void IntentBindRecord::DumpInService(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("intent={"));
    AutoPtr<IIntent> intent;
    mIntent->GetIntent((IIntent**)&intent);
    String str;
    intent->ToShortString(FALSE ,TRUE, FALSE, FALSE, &str);
    pw->Print(str);
    pw->PrintCharln('}');
    pw->Print(prefix);
    pw->Print(String("binder="));
    pw->Println(mBinder);
    pw->Print(prefix);
    pw->Print(String("requested="));
    pw->Print(mRequested);
    pw->Print(String(" received="));
    pw->Print(mReceived);
    pw->Print(String(" hasBound="));
    pw->Print(mHasBound);
    pw->Print(String(" doRebind="));
    pw->Println(mDoRebind);
    HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> >::Iterator it;
    for (it = mApps.Begin(); it != mApps.End(); ++it) {
        AutoPtr<AppBindRecord> a = it->mSecond;
        pw->Print(prefix);
        pw->Print(String("* Client AppBindRecord{"));
        pw->Print(StringUtils::ToString((Int32)a.Get(), 16));
        pw->PrintChar(' ');
        assert(0);
        // pw->Print(a->mClient->ToString());
        pw->Println('}');
        a->DumpInIntentBind(pw, prefix + "  ");
    }
}

Int32 IntentBindRecord::CollectFlags()
{
    Int32 flags = 0;
    HashMap< AutoPtr<ProcessRecord>, AutoPtr<AppBindRecord> >::Iterator it;
    for (it = mApps.Begin(); it != mApps.End(); ++it) {
        AutoPtr<AppBindRecord> app = it->mSecond;
        HashSet< AutoPtr<ConnectionRecord> >::Iterator cit;
        for (cit = app->mConnections.Begin(); cit != app->mConnections.End(); ++cit) {
            AutoPtr<ConnectionRecord> conn = *cit;
            flags |= conn->mFlags;
        }
    }
    return flags;
}

String IntentBindRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }

    StringBuilder sb(128);
    sb += "IntentBindRecord{";
    sb += StringUtils::ToString(Int32(this), 16);
    sb += ' ';
    if ((CollectFlags()& IContext::BIND_AUTO_CREATE) != 0) {
        sb += "CR ";
    }
    assert(0);
    // sb += mService->mShortName;
    sb += ':';
    if (mIntent != NULL) {
        AutoPtr<IIntent> intent;
        mIntent->GetIntent((IIntent**)&intent);
        intent->ToShortString(&sb, FALSE, FALSE, FALSE, FALSE);
    }
    sb += '}';
    return mStringName = sb.ToString();
}

ECode IntentBindRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
