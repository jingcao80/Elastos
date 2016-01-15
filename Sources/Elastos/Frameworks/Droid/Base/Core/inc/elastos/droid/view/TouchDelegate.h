
#ifndef __ELASTOS_DROID_VIEW_TOUCHDELEGATE_H__
#define __ELASTOS_DROID_VIEW_TOUCHDELEGATE_H__

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::ITouchDelegate;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Helper class to handle situations where you want a view to have a larger touch area than its
 * actual view bounds. The view whose touch area is changed is called the delegate view. This
 * class should be used by an ancestor of the delegate. To use a TouchDelegate, first create an
 * instance that specifies the bounds that should be mapped to the delegate and the delegate
 * view itself.
 * <p>
 * The ancestor should then forward all of its touch events received in its
 * {@link android.view.View#onTouchEvent(MotionEvent)} to {@link #onTouchEvent(MotionEvent)}.
 * </p>
 */
class TouchDelegate
    : public Object
    , public ITouchDelegate
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Constructor
     *
     * @param bounds Bounds in local coordinates of the containing view that should be mapped to
     *        the delegate view
     * @param delegateView The view that should receive motion events
     */
    CARAPI constructor(
        /* [in] */ IRect* bounds,
        /* [in] */ IView* delegateView);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

private:
    /**
     * View that should receive forwarded touch events
     */
    AutoPtr<IView> mDelegateView;

    /**
     * Bounds in local coordinates of the containing view that should be mapped to the delegate
     * view. This rect is used for initial hit testing.
     */
    AutoPtr<IRect> mBounds;

    /**
     * mBounds inflated to include some slop. This rect is to track whether the motion events
     * should be considered to be be within the delegate view.
     */
    AutoPtr<IRect> mSlopBounds;

    /**
     * True if the delegate had been targeted on a down event (intersected mBounds).
     */
    Boolean mDelegateTargeted;

    Int32 mSlop;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_TOUCHDELEGATE_H__
