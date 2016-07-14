
#ifndef __ELASTOS_DROID_IMS_CIMSSSINFO_H__
#define __ELASTOS_DROID_IMS_CIMSSSINFO_H__

#include "_Elastos_Droid_Ims_CImsSsInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Ims {

CarClass(CImsSsInfo)
    , public Object
    , public IImsSsInfo
    , public IParcelable
{
public:
    CImsSsInfo();

    virtual ~CImsSsInfo();

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
    // 0: disabled, 1: enabled
    Int32 mStatus;
    String mIcbNum;
};

} // namespace Ims
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_IMS_CIMSSSINFO_H__
