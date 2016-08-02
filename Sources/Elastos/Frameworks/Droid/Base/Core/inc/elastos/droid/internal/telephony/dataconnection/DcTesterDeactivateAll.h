
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__

#include "_Elastos.Droid.Internal.h"
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
 * To bring down all DC's send the following intent:
 *
 * adb shell am broadcast -a com.android.internal.telephony.dataconnection.action_deactivate_all
 */
class DcTesterDeactivateAll
    : public Object
    , public IDcTesterDeactivateAll
{
private:
    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DcTesterDeactivateAll::SubBroadcastReceiver")

        SubBroadcastReceiver(
            /* [in] */ DcTesterDeactivateAll* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTesterDeactivateAll* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcTesterDeactivateAll();

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ IDcController* dcc,
        /* [in] */ IHandler* handler);

    CARAPI Dispose();

public:
    static String sActionDcTesterDeactivateAll;

    // Below code can't be identified.
    // The static intent receiver one for all instances and we assume this
    // is running on the same thread as Dcc.
    AutoPtr<IBroadcastReceiver> sIntentReceiver;

private:
    static CARAPI Log(
        /* [in] */ const char *fmt, ...);

    static const String LOG__TAG;

    static const Boolean DBG;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IDcController> mDcc;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTESTERDEACTIVATEALL_H__
