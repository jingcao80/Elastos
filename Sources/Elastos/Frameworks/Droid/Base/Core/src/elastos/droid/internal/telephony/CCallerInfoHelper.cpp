#include "elastos/droid/internal/telephony/CCallerInfo.h"
#include "elastos/droid/internal/telephony/CCallerInfoHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallerInfoHelper, Singleton, ICallerInfoHelper)

CAR_SINGLETON_IMPL(CCallerInfoHelper)

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICursor* cursor,
    /* [out] */ ICallerInfo** callerInfo)
{
    return CCallerInfo::GetCallerInfo(ctx, contactRef, cursor, callerInfo);
}

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* contactRef,
    /* [out] */ ICallerInfo** callerInfo)
{
    return CCallerInfo::GetCallerInfo(ctx, contactRef, callerInfo);
}

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [out] */ ICallerInfo** callerInfo)
{
    return CCallerInfo::GetCallerInfo(ctx, number, callerInfo);
}

ECode CCallerInfoHelper::GetCallerInfo(
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfo** callerInfo)
{
    return CCallerInfo::GetCallerInfo(ctx, number, subId, callerInfo);
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony
