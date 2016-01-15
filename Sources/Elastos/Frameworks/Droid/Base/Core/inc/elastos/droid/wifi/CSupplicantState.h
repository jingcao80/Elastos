
#ifndef __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATE_H__
#define __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATE_H__

#include "_Elastos_Droid_Wifi_CSupplicantState.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CSupplicantState)
    , public Object
    , public ISupplicantState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ SupplicantState state);

    CARAPI Set(
        /* [in] */ SupplicantState state);

    CARAPI Get(
        /* [out] */ SupplicantState* state);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
private:
    SupplicantState mState;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CSUPPLICANTSTATE_H__
