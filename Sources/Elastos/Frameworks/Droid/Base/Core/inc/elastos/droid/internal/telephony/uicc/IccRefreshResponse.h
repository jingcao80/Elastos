#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * See also RIL_SimRefresh in include/telephony/ril.h
  *
  * {@hide}
  */
class IccRefreshResponse
    : public Object
    , public IIccRefreshResponse
{
public:
    CAR_INTERFACE_DECL();
    /* null terminated string, e.g.,
                                                   from 0xA0, 0x00 -> 0x41,
                                                   0x30, 0x30, 0x30 */
    /* Example: a0000000871002f310ffff89080000ff */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    Int32 mRefreshResult;
    /* Sim Refresh result */
    Int32 mEfId;
    /* EFID */
    String mAid;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__

