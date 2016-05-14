#ifndef __ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CTextViewSavedState.h"
#include "elastos/droid/widget/TextView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CTextViewSavedState)
    , public TextView::TextViewSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTEXTVIEWSAVEDSTATE_H__
