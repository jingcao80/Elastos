#ifndef __ELASTOS_DROID_TEXT_METHOD_TOUCH_H__
#define __ELASTOS_DROID_TEXT_METHOD_TOUCH_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::Method::ITouchDragState;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

//public
class Touch
{
private:
    //static
    class DragState
        : public Object
        , public ITouchDragState
        , public INoCopySpan
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("Touch::DragState")

        DragState();

        CARAPI constructor(
            /* [in] */ Float x,
            /* [in] */ Float y,
            /* [in] */ Int32 scrollX,
            /* [in] */ Int32 scrollY);
    public:
        Float mX;
        Float mY;
        Int32 mScrollX;
        Int32 mScrollY;
        Boolean mFarEnough;
        Boolean mUsed;
        Boolean mIsActivelySelecting;
        Boolean mIsSelectionStarted;
    };

public:
    /**
     * Scrolls the specified widget to the specified coordinates, except
     * constrains the X scrolling position to the horizontal regions of
     * the text that will be visible after scrolling to the specified
     * Y position.
     */
     static CARAPI ScrollTo(
        /* [in] */ ITextView* widget,
        /* [in] */ ILayout* layout,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Handles touch events for dragging.  You may want to do other actions
     * like moving the cursor on touch as well.
     */
    static CARAPI OnTouchEvent(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* ret);

    /**
     * @param widget The text view.
     * @param buffer The text buffer.
     */
    static CARAPI GetInitialScrollX(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [out] */ Int32* ret);

    /**
     * @param widget The text view.
     * @param buffer The text buffer.
     */
    static CARAPI GetInitialScrollY(
        /* [in] */ ITextView* widget,
        /* [in] */ ISpannable* buffer,
        /* [out] */ Int32* ret);

    static CARAPI IsActivelySelecting(
        /* [in] */ ISpannable* buffer,
        /* [out] */ Boolean* ret);

    static CARAPI IsSelectionStarted(
        /* [in] */ ISpannable* buffer,
        /* [out] */ Boolean* ret);

private:
    Touch();
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_TOUCH_H__
