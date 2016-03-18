
#ifndef __ELASTOS_DROID_IMS_CIMSREASONINFO_H__
#define __ELASTOS_DROID_IMS_CIMSREASONINFO_H__

#include "_Elastos_Droid_Ims_CImsReasonInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsReasonInfo)
    , public Object
    , public IImsReasonInfo
    , public IParcelable
{
public:
    CImsReasonInfo();

    virtual ~CImsReasonInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ Int32 extraCode);

    CARAPI constructor(
        /* [in] */ Int32 code,
        /* [in] */ Int32 extraCode,
        /* [in] */ const String& extraMessage);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetCode(
        /* [out] */ Int32* result);

    CARAPI GetExtraCode(
        /* [out] */ Int32* result);

    CARAPI GetExtraMessage(
        /* [out] */ String* result);

    CARAPI GetReasonType(
        /* [out] */ Int32* result);

public:
    // For reason type
    Int32 mReasonType;
    // For main reason code
    Int32 mCode;
    // For the extra code value; it depends on the code value.
    Int32 mExtraCode;
    // For the additional message of the reason info.
    String mExtraMessage;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSREASONINFO_H__
