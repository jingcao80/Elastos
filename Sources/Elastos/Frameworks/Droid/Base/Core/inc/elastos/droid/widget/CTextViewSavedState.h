#ifndef __ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CTextViewSavedState.h"
#include "elastos/droid/widget/TextViewSavedState.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTextViewSavedState), public TextViewSavedState
{
public:
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI constructor();

};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__
