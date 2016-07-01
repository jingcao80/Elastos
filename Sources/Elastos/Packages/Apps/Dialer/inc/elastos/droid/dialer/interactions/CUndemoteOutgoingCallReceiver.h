
#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__

#include "_Elastos_Droid_Dialer_Interactions_CUndemoteOutgoingCallReceiver.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/core/Thread.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

/**
 * This broadcast receiver is used to listen to outgoing calls and undemote formerly demoted
 * contacts if a phone call is made to a phone number belonging to that contact.
 */
CarClass(CUndemoteOutgoingCallReceiver)
    , public BroadcastReceiver
    , public IUndemoteOutgoingCallReceiver
{
private:
    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ CUndemoteOutgoingCallReceiver* host,
            /* [in] */ IContext* context,
            /* [in] */ const String& number);

        // @Override
        CARAPI Run();

    private:
        CUndemoteOutgoingCallReceiver* mHost;
        AutoPtr<IContext> mContext;
        String mNumber;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) UndemoteContactWithId(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    CARAPI_(Int64) GetContactIdFromPhoneNumber(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

private:
    static const Int64 NO_CONTACT_FOUND; // = -1;
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
