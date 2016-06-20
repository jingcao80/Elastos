#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONTROLLERHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONTROLLERHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Uicc_CUiccControllerHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

CarClass(CUiccControllerHelper)
    , public Singleton
    , public IUiccControllerHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI Make(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci,
        /* [out] */ IUiccController** result);

    CARAPI GetInstance(
        /* [out] */ IUiccController** result);

    CARAPI GetFamilyFromRadioTechnology(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Int32* result);

    CARAPI ParseOemSimRefresh(
        /* [in] */ IByteBuffer* payload,
        /* [out] */ IIccRefreshResponse** result);
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_CUICCCONTROLLERHELPER_H__
