#include "elastos/droid/provider/CTelephonyThreads.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyThreads, Singleton, ITelephonyThreads, ITelephonyThreadsColumns);
CAR_SINGLETON_IMPL(CTelephonyThreads);

ECode CTelephonyThreads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ const String& recipient,
    /* [out] */ Int64* threadId)
{
    return Telephony::Threads::GetOrCreateThreadId(context, recipient, threadId);
}

ECode CTelephonyThreads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ ISet* recipients,//String
    /* [out] */ Int64* threadId)
{
    return Telephony::Threads::GetOrCreateThreadId(context, recipients, threadId);
}

ECode CTelephonyThreads::GetOBSOLETE_THREADS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Threads::OBSOLETE_THREADS_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyThreads::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Threads::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
