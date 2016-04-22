#ifndef  __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/PagedView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/* Class that does most of the work of enabling dragging items out of a PagedView by performing a
 * vertical drag. Used by both CustomizePagedView and AllAppsPagedView.
 * Subclasses must do the following:
 *   * call setDragSlopeThreshold after making an instance of the PagedViewWithDraggableItems
 *   * call child.setOnLongClickListener(this) and child.setOnTouchListener(this) on all children
 *       (good place to do it is in syncPageItems)
 *   * override beginDragging(View) (but be careful to call super.beginDragging(View)
 *
 */
class PagedViewWithDraggableItems
    : public PagedView
    , public IViewOnLongClickListener
    , public IViewOnTouchListener
{
public:
    CAR_INTERFACE_DECL();

    PagedViewWithDraggableItems();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI SetDragSlopeThreshold(
        /* [in] */ Float dragSlopeThreshold);


protected:
    CARAPI_(Boolean) BeginDragging(
        /* [in] */ IView* v);

    CARAPI_(void) CancelDragging();

    /*
     * Determines if we should change the touch state to start scrolling after the
     * user moves their touch point too far.
     */
    CARAPI DetermineScrollingStart(
        /* [in] */ IMotionEvent* ev);

    /*
     * Determines if we should change the touch state to start dragging after the
     * user moves their touch point far enough.
     */
    CARAPI_(void) DetermineDraggingStart(
        /* [in] */ IMotionEvent* ev);

    //@Override
    CARAPI OnDetachedFromWindow();

    /** Show the scrolling indicators when we move the page */
    CARAPI OnPageBeginMoving();

    CARAPI OnPageEndMoving();

private:
    CARAPI_(void) HandleTouchEvent(
        /* [in] */ IMotionEvent* ev);

private:
    AutoPtr<IView> mLastTouchedItem;
    Boolean mIsDragging;
    Boolean mIsDragEnabled;
    Float mDragSlopeThreshold;
    AutoPtr<ILauncher> mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PACKAGECHANGEDRECEIVER_H__