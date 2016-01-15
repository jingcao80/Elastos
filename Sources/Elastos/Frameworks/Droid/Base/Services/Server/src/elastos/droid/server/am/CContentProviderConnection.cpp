
#include "elastos/droid/server/am/CContentProviderConnection.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/ContentProviderRecord.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

CAR_INTERFACE_IMPL(CContentProviderConnection, Object, IBinder)
CAR_OBJECT_IMPL(CContentProviderConnection)

CContentProviderConnection::CContentProviderConnection()
    : mCreateTime(0)
    , mStableCount(0)
    , mUnstableCount(0)
    , mWaiting(FALSE)
    , mDead(FALSE)
    , mNumStableIncs(0)
    , mNumUnstableIncs(0)
{}

CContentProviderConnection::~CContentProviderConnection()
{
}

ECode CContentProviderConnection::constructor(
    /* [in] */ Handle32 provider,
    /* [in] */ Handle32 client)
{
    mProvider = (ContentProviderRecord*)provider;
    mClient = (ProcessRecord*)client;
    mCreateTime = SystemClock::GetElapsedRealtime();
    return NOERROR;
}

ECode CContentProviderConnection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = ToString();
    return NOERROR;
}

String CContentProviderConnection::ToString()
{
    StringBuilder sb(128);
    sb += ("CContentProviderConnection{");
    ToShortString(&sb);
    sb += ('}');
    return sb.ToString();
}

String CContentProviderConnection::ToShortString()
{
    StringBuilder sb(128);
    ToShortString(&sb);
    return sb.ToString();
}

String CContentProviderConnection::ToClientString()
{
    StringBuilder sb(128);
    ToClientString(&sb);
    return sb.ToString();
}

void CContentProviderConnection::ToShortString(
    /* [in] */ StringBuilder* sb)
{
    if (sb == NULL)
        return;

    *sb += mProvider->ToShortString();
    *sb += ("->");
    ToClientString(sb);
}

void CContentProviderConnection::ToClientString(
    /* [in] */ StringBuilder* sb)
{
    if (sb == NULL)
        return;

    if (mClient != NULL) {
        assert(0);
        // *sb += (mClient->ToShortString());
    }
    *sb += (" s");
    *sb += (mStableCount);
    *sb += ("/");
    *sb += (mNumStableIncs);
    *sb += (" u");
    *sb += (mUnstableCount);
    *sb += ("/");
    *sb += (mNumUnstableIncs);
    if (mWaiting) {
        *sb += (" WAITING");
    }
    if (mDead) {
        *sb += (" DEAD");
    }

    Int64 nowReal = SystemClock::GetElapsedRealtime();
    *sb += (" ");
    TimeUtils::FormatDuration(nowReal - mCreateTime, *sb);
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
