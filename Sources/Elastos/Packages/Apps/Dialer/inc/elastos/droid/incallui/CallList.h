
#ifndef __ELASTOS_DROID_INCALLUI_CALLLIST_H__
#define __ELASTOS_DROID_INCALLUI_CALLLIST_H__

#include "Elastos.Droid.Telecomm.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/Call.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Telecom::ICall;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class CallList
    : public Object
    , public ICallList
    , public IInCallPhoneListener
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Static singleton accessor method.
     */
    static CARAPI_(AutoPtr<CallList>) GetInstance();

    CARAPI_(void) AddListener(
        /* [in] */ ICallListListener* listener);

    CARAPI_(AutoPtr<Call>) GetOutgoingCall();

    CARAPI_(AutoPtr<Call>) GetDisconnectedCall();

    CARAPI_(AutoPtr<Call>) GetActiveOrBackgroundCall();

    CARAPI_(AutoPtr<Call>) GetFirstCall();

    CARAPI_(AutoPtr<Call>) GetCallByTelecommCall(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::ICall* telecommCall);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALLLIST_H__
