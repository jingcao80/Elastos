
#ifndef __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__
#define __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__

#include "_Elastos_Droid_Ims_CImsCallForwardInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsCallForwardInfo)
    , public Object
    , public IImsCallForwardInfo
    , public IParcelable
{
public:
    CImsCallForwardInfo();

    virtual ~CImsCallForwardInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    // Refer to ImsUtInterface#CDIV_CF_XXX
    Int32 mCondition;
    // 0: disabled, 1: enabled
    Int32 mStatus;
    // 0x91: International, 0x81: Unknown
    Int32 mToA;
    // Number (it will not include the "sip" or "tel" URI scheme)
    String mNumber;
    // No reply timer for CF
    Int32 mTimeSeconds;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSCALLFORWARDINFO_H__
