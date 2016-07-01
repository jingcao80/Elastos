#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CSmartDialMatchPositionHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CSmartDialMatchPositionHelper)
    , public Singleton
    , public ISmartDialMatchPositionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI AdvanceMatchPositions(
        /* [in] */ IArrayList* inList,
        /* [in] */ Int32 toAdvance);

    CARAPI Print(
        /* [in] */ IArrayList* list);
};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__
