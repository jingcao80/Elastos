
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Internal_Telephony_CCallerInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfoHelper)
    , public Singleton
    , public ICallerInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * getCallerInfo given a Cursor.
     * @param context the context used to retrieve string constants
     * @param contactRef the URI to attach to this CallerInfo object
     * @param cursor the first object in the cursor is used to build the CallerInfo object.
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a URI, look up in the call-log database
     * for the uri unique key.
     * @param context the context used to get the ContentResolver
     * @param contactRef the URI used to lookup caller id
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a phone number, look up in the call-log database
     * for the matching caller id info.
     * @param context the context used to get the ContentResolver
     * @param number the phone number used to lookup caller id
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied. If
     * a matching number is not found, then a generic caller info is returned,
     * with all relevant fields empty or null.
     */
    CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a phone number and subscription, look up in the call-log database
     * for the matching caller id info.
     * @param context the context used to get the ContentResolver
     * @param number the phone number used to lookup caller id
     * @param subId the subscription for checking for if voice mail number or not
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied. If
     * a matching number is not found, then a generic caller info is returned,
     * with all relevant fields empty or null.
     */
    CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ Int64 subId,
        /* [out] */ ICallerInfo** callerInfo);
};

} //namespace Elastos
} //namespace Droid
} //namespace Internal
} //namespace Telephony

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFOHELPER_H__
