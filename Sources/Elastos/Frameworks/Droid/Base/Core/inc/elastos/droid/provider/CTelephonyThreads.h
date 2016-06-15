#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYTHREADS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYTHREADS_H__

#include "_Elastos_Droid_Provider_CTelephonyThreads.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonyThreads)
    , public Singleton
    , public ITelephonyThreads
    , public ITelephonyThreadsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
      * This is a single-recipient version of {@code getOrCreateThreadId}.
      * It's convenient for use with SMS messages.
      * @param context the context object to use.
      * @param recipient the recipient to send to.
      * @hide
      */
    CARAPI GetOrCreateThreadId(
        /* [in] */ IContext* context,
        /* [in] */ const String& recipient,
        /* [out] */ Int64* threadId);

    /**
      * Given the recipients list and subject of an unsaved message,
      * return its thread ID.  If the message starts a new thread,
      * allocate a new thread ID.  Otherwise, use the appropriate
      * existing thread ID.
      *
      * <p>Find the thread ID of the same set of Recipients (in any order,
      * without any additions). If one is found, return it. Otherwise,
      * return a unique thread ID.</p>
      * @hide
      */
    CARAPI GetOrCreateThreadId(
        /* [in] */ IContext* context,
        /* [in] */ ISet* recipients,//String
        /* [out] */ Int64* threadId);


    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    CARAPI GetOBSOLETE_THREADS_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYTHREADS_H__
