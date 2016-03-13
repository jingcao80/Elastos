
#ifndef __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__
#define __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__

#include "_Elastos_Droid_Telephony_CPreciseCallState.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CPreciseCallState)
    , public Object
    , public IPreciseCallState
    , public IParcelable
{
public:
    CPreciseCallState();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 ringingCall,
        /* [in] */ Int32 foregroundCall,
        /* [in] */ Int32 backgroundCall,
        /* [in] */ Int32 disconnectCause,
        /* [in] */ Int32 preciseDisconnectCause);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetRingingCallState(
        /* [out] */ Int32* ringingCallState);

    CARAPI GetForegroundCallState(
        /* [out] */ Int32* foregroundCallState);

    CARAPI GetBackgroundCallState(
        /* [out] */ Int32* backgroundCallState);

    CARAPI GetDisconnectCause(
        /* [out] */ Int32* disconnectCause);

    CARAPI GetPreciseDisconnectCause(
        /* [out] */ Int32* preciseDisconnectCause);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* res);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Int32 mRingingCallState;
    Int32 mForegroundCallState;
    Int32 mBackgroundCallState;
    Int32 mDisconnectCause;
    Int32 mPreciseDisconnectCause;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CPRECISECALLSTATE_H__
