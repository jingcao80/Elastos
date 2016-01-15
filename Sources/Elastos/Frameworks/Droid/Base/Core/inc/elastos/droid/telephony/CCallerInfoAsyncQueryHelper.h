
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_CCallerInfoAsyncQueryHelper.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::Telephony::ICallerInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoAsyncQueryHelper)
{
public:
    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef,
        /* [in] */ IOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

    CARAPI StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ IOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie,
        /* [out] */ ICallerInfoAsyncQuery** cia);

private:
    static const String LOG_TAG;
    static const Boolean DBG;
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOASYNCQUERYHELPER_H__
