
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERFAILBRINGUPALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERFAILBRINGUPALL_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/dataconnection/DcFailBringUp.h"
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * A package level call that causes all DataConnection bringUp calls to fail a specific
 * number of times. Here is an example that sets counter to 2 and cause to -3 for all instances:
 *    adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_fail_bringup \
 *     --ei counter 2 --ei fail_cause -3
 *
 * Also you can add a suggested retry time if desired:
 *     --ei suggested_retry_time 5000
 *
 * The fail_cause is one of {@link DcFailCause}
 */
class DcTesterFailBringUpAll
    : public Object
    , public IDcTesterFailBringUpAll
{
private:
    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        SubBroadcastReceiver(
            /* [in] */ DcTesterFailBringUpAll* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTesterFailBringUpAll* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcTesterFailBringUpAll();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IHandler* handler);

    CARAPI Dispose();

    CARAPI GetDcFailBringUp(
        /* [out] */ DcFailBringUp** result);

private:
    CARAPI Log(
        /* [in] */ const String& s);

private:
    static const String LOG__TAG;

    static const Boolean DBG;

    AutoPtr<IPhoneBase> mPhone;

    String mActionFailBringUp;

    // The saved FailBringUp data from the intent
    AutoPtr<DcFailBringUp> mFailBringUp;

    // The static intent receiver one for all instances.
    AutoPtr<IBroadcastReceiver> mIntentReceiver;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERFAILBRINGUPALL_H__
