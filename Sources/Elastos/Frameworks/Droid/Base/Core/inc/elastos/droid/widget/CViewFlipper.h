
#ifndef __ELASTOS_DROID_WIDGET_CVIEWFLIPPER_H__
#define __ELASTOS_DROID_WIDGET_CVIEWFLIPPER_H__

#include "_Elastos_Droid_Widget_CViewFlipper.h"
#include "elastos/droid/widget/ViewFlipper.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * Simple {@link ViewAnimator} that will animate between two or more views
 * that have been added to it.  Only one child is shown at a time.  If
 * requested, can automatically flip between each child at a regular interval.
 *
 * @attr ref android.R.styleable#ViewFlipper_flipInterval
 * @attr ref android.R.styleable#ViewFlipper_autoStart
 */
CarClass(CViewFlipper), public ViewFlipper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CVIEWFLIPPER_H__
