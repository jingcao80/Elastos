#ifndef  __ELASTOS_DROID_PHONE_CPROCESSOUTGOINGCALLTEST_H__
#define  __ELASTOS_DROID_PHONE_CPROCESSOUTGOINGCALLTEST_H__

#include "_Elastos_Droid_Phone_CProcessOutgoingCallTest.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * ProcessOutgoingCallTest tests {@link OutgoingCallBroadcaster} by performing
 * a couple of simple modifications to outgoing calls, and by printing log
 * messages for each call.
 */
CarClass(CProcessOutgoingCallTest)
    , public BroadcastReceiver
    , public IProcessOutgoingCallTest
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const String AREACODE;

    static const Boolean LOGV;

    static const Boolean REDIRECT_411_TO_GOOG411;
    static const Boolean SEVEN_DIGIT_DIALING;
    static const Boolean POUND_POUND_SEARCH;
    static const Boolean BLOCK_555;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CPROCESSOUTGOINGCALLTEST_H__