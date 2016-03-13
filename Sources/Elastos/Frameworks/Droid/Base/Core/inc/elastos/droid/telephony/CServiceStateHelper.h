
#ifndef __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__
#define __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Telephony_CServiceStateHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CServiceStateHelper)
    , public Singleton
    , public IServiceStateHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewFromBundle(
        /* [in] */ IBundle* m,
        /* [out] */ IServiceState** res);

    CARAPI RilRadioTechnologyToString(
        /* [in] */ Int32 rt,
        /* [out] */ String* res);

    CARAPI IsGsm(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Boolean* res);

    CARAPI IsCdma(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ Boolean* res);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_CSERVICESTATEHELPER_H__
