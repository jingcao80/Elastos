#ifndef __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__
#define __ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__

#include "_Elastos_Apps_Dialer_Dialpad_CSmartDialMatchPositionHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Apps {
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
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_DIALPAD_CSMARTDIALMATCHPOSITIONHELPER_H__
