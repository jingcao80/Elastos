
#ifndef __ELASTOS_DROID_WIDGET_GRIDVIEW_H__
#define __ELASTOS_DROID_WIDGET_GRIDVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/AbsListView.h"
#include "elastos/droid/R.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A view that shows items in two-dimensional scrolling grid. The items in the
 * grid come from the {@link ListAdapter} associated with this view.
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-gridview.html">Grid
 * View tutorial</a>.</p>
 */
class GridView
    : public AbsListView
    , public IGridView
{
public:
    CAR_INTERFACE_DECL()

    GridView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::gridViewStyle,
        /* [in] */ Int32 defStyleRes = 0);

    virtual CARAPI SetRemoteViewsAdapter(
        /* [in] */ IIntent* intent);

    virtual CARAPI GetAdapter(
        /* [out] */ IAdapter** adapter);

    virtual CARAPI SetAdapter(
        /* [in] */ IAdapter* adapter);

    virtual CARAPI_(Int32) LookForSelectablePosition(
        /* [in] */ Int32 position,
        /* [in] */ Boolean lookDown);

    virtual CARAPI FillGap(
        /* [in] */ Boolean down);

    virtual CARAPI_(Int32) FindMotionRow(
        /* [in] */ Int32 y);

    virtual CARAPI SmoothScrollToPosition(
        /* [in] */ Int32 position);

    using AbsListView::SmoothScrollToPosition;

    virtual CARAPI SmoothScrollByOffset(
        /* [in] */ Int32 offset);

    virtual CARAPI_(Int32) GetGravity();

    CARAPI GetGravity(
        /* [out] */ Int32* gravity);

    virtual CARAPI_(Int32) GetHorizontalSpacing();

    CARAPI GetHorizontalSpacing(
        /* [out] */ Int32* spacing);

    virtual CARAPI_(Int32) GetRequestedHorizontalSpacing();

    CARAPI GetRequestedHorizontalSpacing(
        /* [out] */ Int32* spacing);

    CARAPI_(Int32) GetVerticalSpacing();

    CARAPI GetVerticalSpacing(
        /* [out] */ Int32* spacing);

    CARAPI_(Int32) GetColumnWidth();

    CARAPI GetColumnWidth(
        /* [out] */ Int32* width);

    CARAPI_(Int32) GetRequestedColumnWidth();

    CARAPI GetRequestedColumnWidth(
        /* [out] */ Int32* width);

    CARAPI_(Int32) GetNumColumns();

    CARAPI GetNumColumns(
        /* [out] */ Int32* columns);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI SetSelection(
        /* [in] */ Int32 position);

    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 repeatCount,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    virtual CARAPI SetHorizontalSpacing(
        /* [in] */ Int32 horizontalSpacing);

    virtual CARAPI SetVerticalSpacing(
        /* [in] */ Int32 verticalSpacing);

    virtual CARAPI SetStretchMode(
        /* [in] */ Int32 stretchMode);

    virtual CARAPI_(Int32) GetStretchMode();

    CARAPI GetStretchMode(
        /* [out] */ Int32* mode);

    virtual CARAPI SetColumnWidth(
        /* [in] */ Int32 columnWidth);

    virtual CARAPI SetNumColumns(
        /* [in] */ Int32 numColumns);

    CARAPI OnInitializeAccessibilityNodeInfoForItem(
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ IAccessibilityNodeInfo* info);


private:
    CARAPI_(AutoPtr<IView>) FillDown(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 nextTop);

    CARAPI_(AutoPtr<IView>) MakeRow(
        /* [in] */ Int32 startPos,
        /* [in] */ Int32 y,
        /* [in] */ Boolean flow);

    CARAPI_(AutoPtr<IView>) FillUp(
        /* [in] */ Int32 pos,
        /* [in] */ Int32 nextBottom);

    CARAPI_(AutoPtr<IView>) FillFromTop(
        /* [in] */ Int32 nextTop);

    CARAPI_(AutoPtr<IView>) FillFromBottom(
        /* [in] */ Int32 lastPosition,
        /* [in] */ Int32 nextBottom);

    CARAPI_(AutoPtr<IView>) FillSelection(
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    CARAPI_(void) PinToTop(
        /* [in] */ Int32 childrenTop);

    CARAPI_(void) PinToBottom(
        /* [in] */ Int32 childrenBottom);

    CARAPI_(AutoPtr<IView>) FillSpecific(
        /* [in] */ Int32 position,
        /* [in] */ Int32 top);

    CARAPI_(void) CorrectTooHigh(
        /* [in] */ Int32 numColumns,
        /* [in] */ Int32 verticalSpacing,
        /* [in] */ Int32 childCount);

    CARAPI_(void) CorrectTooLow(
        /* [in] */ Int32 numColumns,
        /* [in] */ Int32 verticalSpacing,
        /* [in] */ Int32 childCount);

    CARAPI_(AutoPtr<IView>) FillFromSelection(
        /* [in] */ Int32 selectedTop,
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    CARAPI_(Int32) GetBottomSelectionPixel(
        /* [in] */ Int32 childrenBottom,
        /* [in] */ Int32 fadingEdgeLength,
        /* [in] */ Int32 numColumns,
        /* [in] */ Int32 rowStart);

    CARAPI_(Int32) GetTopSelectionPixel(
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 fadingEdgeLength,
        /* [in] */ Int32 rowStart);

    CARAPI_(void) AdjustForBottomFadingEdge(
        /* [in] */ IView* childInSelectedRow,
        /* [in] */ Int32 topSelectionPixel,
        /* [in] */ Int32 bottomSelectionPixel);

    CARAPI_(void) AdjustForTopFadingEdge(
        /* [in] */ IView* childInSelectedRow,
        /* [in] */ Int32 topSelectionPixel,
        /* [in] */ Int32 bottomSelectionPixel);

    CARAPI_(AutoPtr<IView>) MoveSelection(
        /* [in] */ Int32 delta,
        /* [in] */ Int32 childrenTop,
        /* [in] */ Int32 childrenBottom);

    CARAPI_(Boolean) DetermineColumns(
        /* [in] */ Int32 availableSpace);

    CARAPI_(AutoPtr<IView>) MakeAndAddView(
        /* [in] */ Int32 position,
        /* [in] */ Int32 y,
        /* [in] */ Boolean flow,
        /* [in] */ Int32 childrenLeft,
        /* [in] */ Boolean selected,
        /* [in] */ Int32 where);

    CARAPI_(void) SetupChild(
        /* [in] */ IView* child,
        /* [in] */ Int32 position,
        /* [in] */ Int32 y,
        /* [in] */ Boolean flow,
        /* [in] */ Int32 childrenLeft,
        /* [in] */ Boolean selected,
        /* [in] */ Boolean recycled,
        /* [in] */ Int32 where);

    CARAPI_(Boolean) SequenceScroll(
        /* [in] */ Int32 direction);

    CARAPI_(Boolean) CommonKey(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event);

    CARAPI_(Boolean) IsCandidateSelection(
        /* [in] */ Int32 childIndex,
        /* [in] */ Int32 direction);

    CARAPI AdjustViewsUpOrDown();

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI AttachLayoutAnimationParameters(
        /* [in] */ IView* child,
        /* [in] */ IViewGroupLayoutParams* params,
        /* [in] */ Int32 index,
        /* [in] */ Int32 count);

    CARAPI LayoutChildren();

    CARAPI SetSelectionInt(
        /* [in] */ Int32 position);

    CARAPI_(Boolean) PageScroll(
        /* [in] */ Int32 direction);

    CARAPI_(Boolean) FullScroll(
        /* [in] */ Int32 direction);

    CARAPI_(Boolean) ArrowScroll(
        /* [in] */ Int32 direction);

    CARAPI_(void) OnFocusChanged(
        /* [in] */ Boolean gainFocus,
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    CARAPI_(Int32) ComputeVerticalScrollExtent();

    CARAPI_(Int32) ComputeVerticalScrollOffset();

    CARAPI_(Int32) ComputeVerticalScrollRange();

    CARAPI_(Int32) ListPaddingLeft();

    CARAPI_(Int32) ListPaddingTop();

    CARAPI_(Int32) ListPaddingRight();

    CARAPI_(Int32) ListPaddingBottom();

public:
    static const String GRIDVIEW_NAME;
    static const Int32 NO_STRETCH = 0;
    static const Int32 STRETCH_SPACING = 1;
    static const Int32 STRETCH_COLUMN_WIDTH = 2;
    static const Int32 STRETCH_SPACING_UNIFORM = 3;

    static const Int32 AUTO_FIT = -1;

private:
    Int32 mNumColumns;

    Int32 mHorizontalSpacing;
    Int32 mRequestedHorizontalSpacing;
    Int32 mVerticalSpacing;
    Int32 mStretchMode;
    Int32 mColumnWidth;
    Int32 mRequestedColumnWidth;
    Int32 mRequestedNumColumns;

    AutoPtr<IView> mReferenceView;
    AutoPtr<IView> mReferenceViewInSelectedRow;

    Int32 mGravity;

    AutoPtr<IRect> mTempRect;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
