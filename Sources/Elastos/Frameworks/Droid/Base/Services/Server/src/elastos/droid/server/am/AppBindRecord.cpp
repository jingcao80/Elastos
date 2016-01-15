
#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/CServiceRecord.h"
#include "elastos/droid/server/am/IntentBindRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

AppBindRecord::AppBindRecord(
    /* [in] */ CServiceRecord* service,
    /* [in] */ IntentBindRecord* intent,
    /* [in] */ ProcessRecord* client)
    : mService(service)
    , mIntent(intent)
    , mClient(client)
{
}

void AppBindRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix);
    pw->Print(String("mService="));
    pw->Println(mService->ToString());
    pw->Print(prefix);
    pw->Print(String("mClient="));
    pw->Println(mClient->ToString());
    DumpInIntentBind(pw, prefix);
}

void AppBindRecord::DumpInIntentBind(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    if (!mConnections.IsEmpty()) {
        pw->Print(prefix);
        pw->Println(String("Per-process Connections:"));
        HashSet< AutoPtr<ConnectionRecord> >::Iterator iter;
        for (iter = mConnections.Begin(); iter != mConnections.End(); ++iter) {
            AutoPtr<ConnectionRecord> c = *iter;
            pw->Print(prefix);
            pw->Print(String("  "));
            pw->Println(c->ToString());
        }
    }
}

String AppBindRecord::ToString()
{
    StringBuilder sb(128);
    sb += "AppBindRecord{";
    sb += (StringUtils::ToString(Int32(this), 16));
    sb += " ";
    sb += mService->mShortName;
    sb += ":";
    sb += mClient->mProcessName;
    sb += "}";
    return sb.ToString();
}

ECode AppBindRecord::ToString(
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
