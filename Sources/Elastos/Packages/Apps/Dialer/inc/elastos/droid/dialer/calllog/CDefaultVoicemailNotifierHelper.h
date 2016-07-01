
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CDEFAULTVOICEMAILNOTIFICATIONHELPER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CDEFAULTVOICEMAILNOTIFICATIONHELPER_H__

#include "_Elastos_Droid_Dialer_CallLog_CDefaultVoicemailNotifierHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CDefaultVoicemailNotifierHelper)
    , public Singleton
    , public IDefaultVoicemailNotifierHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /** Returns the singleton instance of the {@link DefaultVoicemailNotifier}. */
    CARAPI GetInstance(
        /* [in] */ IContext* context,
        /* [out] */ IDefaultVoicemailNotifier** notifier);

    /** Create a new instance of {@link NewCallsQuery}. */
    CARAPI CreateNewCallsQuery(
        /* [in] */ IContentResolver* contentResolver,
        /* [out] */ IDefaultVoicemailNotifierNewCallsQuery** query);

    /**
     * Create a new PhoneNumberHelper.
     * <p>
     * This will cause some Disk I/O, at least the first time it is created, so it should not be
     * called from the main thread.
     */
    CARAPI CreatePhoneNumberHelper(
        /* [in] */ IContext* context,
        /* [out] */ IPhoneNumberDisplayHelper** helper);
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CDEFAULTVOICEMAILNOTIFICATIONHELPER_H__
