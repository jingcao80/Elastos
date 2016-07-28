
#ifndef __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__
#define __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__

#include "elastos/droid/incallui/CallerInfo.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Helper class to make it easier to run asynchronous caller-id lookup queries.
 * @see CallerInfo
 *
 */
class CallerInfoAsyncQuery
{
public:
    /**
     * Factory method to start the query based on a CallerInfo object.
     *
     * Note: if the number contains an "@" character we treat it
     * as a SIP address, and look it up directly in the Data table
     * rather than using the PhoneLookup table.
     * TODO: But eventually we should expose two separate methods, one for
     * numbers and one for SIP addresses, and then have
     * PhoneUtils.startGetCallerInfo() decide which one to call based on
     * the phone type of the incoming connection.
     */
    static CARAPI_(void) StartQuery(
        /* [in] */ Int32 token,
        /* [in] */ IContext* context,
        /* [in] */ CallerInfo* info,
        /* [in] */ IOnQueryCompleteListener* listener,
        /* [in] */ IInterface* cookie);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLERINFOASYNCQUERY_H__
