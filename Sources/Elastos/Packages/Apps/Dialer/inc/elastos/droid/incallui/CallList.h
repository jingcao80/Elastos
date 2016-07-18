
#ifndef __ELASTOS_DROID_INCALLUI_CALLLIST_H__
#define __ELASTOS_DROID_INCALLUI_CALLLIST_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

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

    CARAPI_(AutoPtr<Call>) GetFirstCall();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALLLIST_H__
