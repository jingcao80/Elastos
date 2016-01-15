#ifndef __ELASTOS_DROID_VIEW_CEMPTYABSSAVEDSTATE_H__
#define __ELASTOS_DROID_VIEW_CEMPTYABSSAVEDSTATE_H__

#include "_Elastos_Droid_View_CEmptyAbsSavedState.h"
#include "elastos/droid/view/AbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CEmptyAbsSavedState) , public AbsSavedState
{
public:
    CEmptyAbsSavedState();

    constructor();

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif
