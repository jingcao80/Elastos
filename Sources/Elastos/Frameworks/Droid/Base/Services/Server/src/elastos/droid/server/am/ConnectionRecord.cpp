
#include "elastos/droid/server/am/ConnectionRecord.h"
#include "elastos/droid/server/am/AppBindRecord.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ConnectionRecord::ConnectionRecord(
    /* [in] */ AppBindRecord* binding,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ IIServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 clientLabel,
    /* [in] */ IPendingIntent* clientIntent)
    : mBinding(binding)
    , mActivity(activity)
    , mConn(conn)
    , mFlags(flags)
    , mClientLabel(clientLabel)
    , mClientIntent(clientIntent)
    , mServiceDead(FALSE)
{
}

void ConnectionRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    StringBuilder sb;
    sb += prefix;
    sb += "binding";
    sb += mBinding->ToString();
    pw->Println(sb.ToString());
    if (mActivity != NULL) {
        sb.SetLength(0);
        sb += prefix;
        sb += "activity=";
        sb += mActivity->ToString();
        pw->Println(sb.ToString());
    }
    sb.SetLength(0);
    sb += prefix;
    sb += "conn=";
    sb += IBinder::Probe(mConn);
    sb += " flags=0x";
    sb += StringUtils::ToString(mFlags, 16);
    pw->Println(sb.ToString());
}

ECode ConnectionRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String ConnectionRecord::ToString()
{
    if (!mStringName.IsNull()) {
        return mStringName;
    }
    StringBuilder sb(128);
    sb += "ConnectionRecord{";
    sb += StringUtils::ToString(Int32(this), 16);
    sb += " u";
    sb += mBinding->mClient->mUserId;
    sb += ' ';
    if ((mFlags & IContext::BIND_AUTO_CREATE) != 0) {
        sb += "CR ";
    }
    if ((mFlags & IContext::BIND_DEBUG_UNBIND) != 0) {
        sb += "DBG ";
    }
    if ((mFlags & IContext::BIND_NOT_FOREGROUND) != 0) {
        sb += "!FG ";
    }
    if ((mFlags & IContext::BIND_ABOVE_CLIENT) != 0) {
        sb += "ABCLT ";
    }
    if ((mFlags & IContext::BIND_ALLOW_OOM_MANAGEMENT) != 0) {
        sb += "OOM ";
    }
    if ((mFlags & IContext::BIND_WAIVE_PRIORITY) != 0) {
        sb += "WPRI ";
    }
    if ((mFlags & IContext::BIND_IMPORTANT) != 0) {
        sb += "IMP ";
    }
    if ((mFlags & IContext::BIND_ADJUST_WITH_ACTIVITY) != 0) {
        sb += "ACT ";
    }
    if ((mFlags & IContext::BIND_VISIBLE) != 0) {
        sb += "VIS ";
    }
    if ((mFlags & IContext::BIND_SHOWING_UI) != 0) {
        sb += "UI ";
    }
    if ((mFlags & IContext::BIND_NOT_VISIBLE) != 0) {
        sb += "!VIS ";
    }
    if (mServiceDead) {
        sb += "DEAD ";
    }
    sb += mBinding->mService->mShortName;
    sb += ":@";
    sb += StringUtils::ToString((Int32)IBinder::Probe(mConn), 16);
    sb += '}';
    return mStringName = sb.ToString();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
