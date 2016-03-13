#include "elastos/droid/internal/telephony/CCallerInfoAsyncQuery.h"
#include "elastos/droid/internal/telephony/CCallerInfoAsyncQueryHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL(CCallerInfoAsyncQueryHelper, Singleton, ICallerInfoAsyncQueryHelper)

CAR_SINGLETON_IMPL(CCallerInfoAsyncQueryHelper)

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ IUri* contactRef,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, contactRef, listener, cookie, cia);
}

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, number, listener, cookie, cia);
}

ECode CCallerInfoAsyncQueryHelper::StartQuery(
    /* [in] */ Int32 token,
    /* [in] */ IContext* ctx,
    /* [in] */ const String& number,
    /* [in] */ ICallerInfoAsyncQueryOnQueryCompleteListener* listener,
    /* [in] */ IInterface* cookie,
    /* [in] */ Int64 subId,
    /* [out] */ ICallerInfoAsyncQuery** cia)
{
    return CCallerInfoAsyncQuery::StartQuery(token, ctx, number, listener, cookie, subId, cia);
}

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony