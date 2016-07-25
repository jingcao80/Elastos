
#ifndef __ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__
#define __ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__

#include "Elastos.Droid.Telecomm.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class TelecomAdapter
    : public Object
    , public IInCallPhoneListener
{
public:
    CAR_INTERFACE_DECL();

    static CARAPI_(AutoPtr<TelecomAdapter>) GetInstance();

    CARAPI_(void) AnswerCall(
        /* [in] */ const String& callId,
        /* [in] */ Int32 videoState);

    CARAPI_(void) RejectCall(
        /* [in] */ const String& callId,
        /* [in] */ Boolean rejectWithMessage,
        /* [in] */ const String& message);

    CARAPI_(void) DisconnectCall(
        /* [in] */ const String& callId);

    CARAPI_(void) UnholdCall(
        /* [in] */ const String& callId);

    CARAPI_(void) Merge(
        /* [in] */ const String& callId);

    CARAPI_(void) Swap(
        /* [in] */ const String& callId);

    CARAPI_(void) PhoneAccountSelected(
        /* [in] */ const String& callId,
        /* [in] */ IPhoneAccountHandle* accountHandle);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_TELECOMADAPTER_H__
