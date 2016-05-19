
#ifndef __ELASTOS_APPS_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
#define __ELASTOS_APPS_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__

#include "_Elastos_Apps_Dialer_Interactions_CUndemoteOutgoingCallReceiver.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Interactions {

/**
 * This broadcast receiver is used to listen to outgoing calls and undemote formerly demoted
 * contacts if a phone call is made to a phone number belonging to that contact.
 */
CarClass(CUndemoteOutgoingCallReceiver)
    , public BroadcastReceiver
    , public ICUndemoteOutgoingCallReceiver
{
private:
    class MyThread
        : public Thread
    {
    public:
        MyThread(
            /* [in] */ CUndemoteOutgoingCallReceiver* host);

        // @Override
        CARAPI Run();

    private:
        CUndemoteOutgoingCallReceiver* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_INTERACTIONS_CUNDEMOTEOUTGOINGCALLRECEIVER_H__
