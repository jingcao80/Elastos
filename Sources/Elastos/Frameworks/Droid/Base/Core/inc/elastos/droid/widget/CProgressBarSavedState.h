#ifndef __ELASTOS_DROID_WIDGET_CPROGRESSBARSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CPROGRESSBARSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CProgressBarSavedState.h"
#include "elastos/droid/widget/ProgressBar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CProgressBarSavedState)
    , public ProgressBar::ProgressBarSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CPROGRESSBARSAVEDSTATE_H__
