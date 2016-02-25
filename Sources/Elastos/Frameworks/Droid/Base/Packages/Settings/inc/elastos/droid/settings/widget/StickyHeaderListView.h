


package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::IListView;

/**
 * This class provides sticky header functionality in a list view, to use with
 * SetupWizardIllustration. To use this, add a header tagged with "sticky", or a header tagged with
 * "stickyContainer" and one of its child tagged as "sticky". The sticky container will be drawn
 * when the sticky element hits the top of the view.
 *
 * There are a few things to note:
 * 1. The two supported scenarios are StickyHeaderListView -> Header (stickyContainer) -> sticky,
 *    and StickyHeaderListView -> Header (sticky). The arrow (->) represents parent/child
 *    relationship and must be immediate child.
 * 2. The view does not work well with padding. b/16190933
 * 3. If fitsSystemWindows is TRUE, then this will offset the sticking position by the height of
 *    the system decorations at the top of the screen.
 *
 * @see SetupWizardIllustration
 * @see com.google.android.setupwizard.util.StickyHeaderScrollView
 *
 * Copied from com.google.android.setupwizard.util.StickyHeaderListView
 */
public class StickyHeaderListView extends ListView {

    private View mSticky;
    private View mStickyContainer;
    private Boolean mDrawScrollBar;
    private Int32 mStatusBarInset = 0;
    private RectF mStickyRect = new RectF();

    public StickyHeaderListView(Context context) {
        Super(context);
    }

    public StickyHeaderListView(Context context, AttributeSet attrs) {
        Super(context, attrs);
    }

    public StickyHeaderListView(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        Super(context, attrs, defStyleAttr);
    }

    public StickyHeaderListView(Context context, AttributeSet attrs, Int32 defStyleAttr,
                                Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
    }

    //@Override
    protected void OnLayout(Boolean changed, Int32 l, Int32 t, Int32 r, Int32 b) {
        super->OnLayout(changed, l, t, r, b);
        if (mSticky == NULL) {
            UpdateStickyView();
        }
    }

    CARAPI UpdateStickyView() {
        mSticky = FindViewWithTag("sticky");
        mStickyContainer = FindViewWithTag("stickyContainer");
    }

    //@Override
    public Boolean DispatchTouchEvent(MotionEvent ev) {
        if (mStickyRect->Contains(ev->GetX(), ev->GetY())) {
            ev->OffsetLocation(-mStickyRect.left, -mStickyRect.top);
            return mStickyContainer->DispatchTouchEvent(ev);
        } else {
            return super->DispatchTouchEvent(ev);
        }
    }

    //@Override
    CARAPI Draw(Canvas canvas) {
        mDrawScrollBar = FALSE;
        super->Draw(canvas);
        if (mSticky != NULL) {
            final Int32 saveCount = canvas->Save();
            // The view to draw when sticking to the top
            final View drawTarget = mStickyContainer != NULL ? mStickyContainer : mSticky;
            // The offset to draw the view at when sticky
            final Int32 drawOffset = mStickyContainer != NULL ? mSticky->GetTop() : 0;
            // Position of the draw target, relative to the outside of the scrollView
            final Int32 drawTop = drawTarget->GetTop();
            if (drawTop + drawOffset < mStatusBarInset || !drawTarget->IsShown()) {
                // ListView does not translate the canvas, so we can simply draw at the top
                canvas->Translate(0, -drawOffset + mStatusBarInset);
                canvas->ClipRect(0, 0, drawTarget->GetWidth(), drawTarget->GetHeight());
                drawTarget->Draw(canvas);
                mStickyRect->Set(0, -drawOffset + mStatusBarInset, drawTarget->GetWidth(),
                        drawTarget->GetHeight() - drawOffset + mStatusBarInset);
            } else {
                mStickyRect->SetEmpty();
            }
            canvas->RestoreToCount(saveCount);
        }
        // Draw the scrollbars last so they are on top of the header
        mDrawScrollBar = TRUE;
        OnDrawScrollBars(canvas);
    }

    //@Override
    protected Boolean IsVerticalScrollBarHidden() {
        return super->IsVerticalScrollBarHidden() || !mDrawScrollBar;
    }

    //@Override
    public WindowInsets OnApplyWindowInsets(WindowInsets insets) {
        if (GetFitsSystemWindows()) {
            mStatusBarInset = insets->GetSystemWindowInsetTop();
            insets->ConsumeSystemWindowInsets(FALSE, TRUE, FALSE, FALSE);
        }
        return insets;
    }
}
