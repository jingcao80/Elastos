#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCTLVDATAHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCTLVDATAHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CUiccTlvDataHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUiccTlvDataHelper)
    , public Singleton
    , public IUiccTlvDataHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI IsUiccTlvData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ Boolean* result);

    CARAPI Parse(
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IUiccTlvData** result);

};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCTLVDATAHELPER_H__
