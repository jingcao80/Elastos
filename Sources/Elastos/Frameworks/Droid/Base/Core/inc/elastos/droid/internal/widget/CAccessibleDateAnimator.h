
#ifndef  __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__
#define  __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__

#include "_Elastos_Droid_Internal_Widget_CAccessibleDateAnimator.h"
#include "elastos/droid/widget/ViewAnimator.h"

using Elastos::Droid::Widget::ViewAnimator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * @hide
 */
CarClass(CAccessibleDateAnimator)
    , public ViewAnimator
    , public IAccessibleDateAnimator
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CAccessibleDateAnimator();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDateMillis(
        /* [in] */ Int64 dateMillis);

    /**
     * Announce the currently-selected date when launched.
     */
    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

private:
    Int64 mDateMillis;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACCESSIBLEDATEANIMATOR_H__
