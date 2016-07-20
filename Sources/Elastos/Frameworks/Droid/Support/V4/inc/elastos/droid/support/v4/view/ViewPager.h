
#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_VIEWPAGER_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_VIEWPAGER_H__

#include "_Elastos.Droid.Support.V4.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/view/ViewGroup.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IVelocityTracker;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::IScroller;
using Elastos::Droid::Widget::IEdgeEffect;
using Elastos::Core::IComparator;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

/**
 * Layout manager that allows the user to flip left and right
 * through pages of data.  You supply an implementation of a
 * {@link PagerAdapter} to generate the pages that the view shows.
 *
 * <p>Note this class is currently under early design and
 * development.  The API will likely change in later updates of
 * the compatibility library, requiring changes to the source code
 * of apps when they are compiled against the newer version.</p>
 *
 * <p>ViewPager is most often used in conjunction with {@link android.app.Fragment},
 * which is a convenient way to supply and manage the lifecycle of each page.
 * There are standard adapters implemented for using fragments with the ViewPager,
 * which cover the most common use cases.  These are
 * {@link android.support.v4.app.FragmentPagerAdapter} and
 * {@link android.support.v4.app.FragmentStatePagerAdapter}; each of these
 * classes have simple code showing how to build a full user interface
 * with them.
 *
 * <p>For more information about how to use ViewPager, read <a
 * href="{@docRoot}training/implementing-navigation/lateral.html">Creating Swipe Views with
 * Tabs</a>.</p>
 *
 * <p>Below is a more complicated example of ViewPager, using it in conjunction
 * with {@link android.app.ActionBar} tabs.  You can find other examples of using
 * ViewPager in the API 4+ Support Demos and API 13+ Support Demos sample code.
 *
 * {@sample development/samples/Support13Demos/src/com/example/android/supportv13/app/ActionBarTabsPager.java
 *      complete}
 */
class ECO_PUBLIC ViewPager
    : public ViewGroup
    , public IViewPager
{
public:
    class ItemInfo : public Object
    {
    public:
        ItemInfo()
            : mPosition(0)
            , mScrolling(FALSE)
            , mWidthFactor(0)
            , mOffset(0)
        {}

    public:
        AutoPtr<IInterface> mObject;
        Int32 mPosition;
        Boolean mScrolling;
        Float mWidthFactor;
        Float mOffset;
    };

    /**
     * Simple implementation of the {@link OnPageChangeListener} interface with stub
     * implementations of each method. Extend this if you do not intend to override
     * every method of {@link OnPageChangeListener}.
     */
    class SimpleOnPageChangeListener
        : public Object
        , public IViewPagerOnPageChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnPageScrolled(
            /* [in] */ Int32 position,
            /* [in] */ Float positionOffset,
            /* [in] */ Int32 positionOffsetPixels);

        // @Override
        CARAPI OnPageSelected(
            /* [in] */ Int32 position);

        // @Override
        CARAPI OnPageScrollStateChanged(
            /* [in] */ Int32 state);
    };

    /**
     * This is the persistent state that is saved by ViewPager.  Only needed
     * if you are creating a sublass of ViewPager that must save its own
     * state, in which case it should implement a subclass of this which
     * contains that state.
     */
    class SavedState
        : public BaseSavedState
        , public IViewPagerSavedState
    {
    public:
        SavedState()
            : mPosition(0)
        {}

        virtual ~SavedState() {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    // protected:
    //     SavedState(Parcel in, ClassLoader loader);

    public:
        Int32 mPosition;
        AutoPtr<IParcelable> mAdapterState;
        AutoPtr<IClassLoader> mLoader;
    };

    class MyAccessibilityDelegate
        : public AccessibilityDelegate
    {
    public:
        MyAccessibilityDelegate(
            /* [in] */ ViewPager* host)
            : mHost(host)
        {}

        CARAPI OnInitializeAccessibilityEvent(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityEvent* event);

        CARAPI OnInitializeAccessibilityNodeInfo(
            /* [in] */ IView* host,
            /* [in] */ IAccessibilityNodeInfo* info);

        CARAPI PerformAccessibilityAction(
            /* [in] */ IView* host,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* args,
            /* [out] */ Boolean* res);

    private:
        CARAPI_(Boolean) CanScroll();

    private:
        ViewPager* mHost;
    };

    /**
     * Layout parameters that should be supplied for views added to a
     * ViewPager.
     */
    class ViewPagerLayoutParams
        : public LayoutParams
        , public IViewPagerLayoutParams
    {
    public:
        ViewPagerLayoutParams()
            : mIsDecor(FALSE)
            , mGravity(0)
            , mWidthFactor(0.0)
            , mNeedsMeasure(FALSE)
            , mPosition(0)
            , mChildIndex(0)
        {}

        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

    public:
        /**
         * true if this view is a decoration on the pager itself and not
         * a view supplied by the adapter.
         */
        Boolean mIsDecor;

        /**
         * Gravity setting for use on decor views only:
         * Where to position the view page within the overall ViewPager
         * container; constants are defined in {@link android.view.Gravity}.
         */
        Int32 mGravity;

        /**
         * Width as a 0-1 multiplier of the measured pager width
         */
        Float mWidthFactor;

        /**
         * true if this view was added during layout and needs to be measured
         * before being positioned.
         */
        Boolean mNeedsMeasure;

        /**
         * Adapter position this view is for if !isDecor
         */
        Int32 mPosition;

        /**
         * Current child index within the ViewPager that this view occupies
         */
        Int32 mChildIndex;
    };

    class ViewPositionComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */  IInterface* a,
            /* [in] */  IInterface* b,
            /* [out] */ Int32* result);
    };

private:
    class ItemInfoComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */  IInterface* a,
            /* [in] */  IInterface* b,
            /* [out] */ Int32* result);
    };

    class MyInterpolator
        : public Object
        , public IInterpolator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* interpolation);
    };

    class EndScrollRunnable : public Runnable
    {
    public:
        EndScrollRunnable(
            /* [in] */ ViewPager* host)
            : mHost(host)
        {}

        // @Override
        CARAPI Run();

    private:
        ViewPager* mHost;
    };

    class PagerObserver : public DataSetObserver
    {
    public:
        PagerObserver(
            /* [in] */ ViewPager* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

    private:
        ViewPager* mHost;
    };

public:
    ViewPager();

    virtual ~ViewPager() {}

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Set a PagerAdapter that will supply views for this pager as needed.
     *
     * @param adapter Adapter to use
     */
    CARAPI SetAdapter(
        /* [in] */ IPagerAdapter* adapter);

    /**
     * Retrieve the current adapter supplying pages.
     *
     * @return The currently registered PagerAdapter
     */
    CARAPI GetAdapter(
        /* [out] */ IPagerAdapter** adapter);

    /**
     * Set the currently selected page. If the ViewPager has already been through its first
     * layout with its current adapter there will be a smooth animated transition between
     * the current item and the specified item.
     *
     * @param item Item index to select
     */
    CARAPI SetCurrentItem(
        /* [in] */ Int32 item);

    /**
     * Set the currently selected page.
     *
     * @param item Item index to select
     * @param smoothScroll True to smoothly scroll to the new item, false to transition immediately
     */
    CARAPI SetCurrentItem(
        /* [in] */ Int32 item,
        /* [in] */ Boolean smoothScroll);

    CARAPI GetCurrentItem(
        /* [out] */ Int32* item);

    /**
     * Set a listener that will be invoked whenever the page changes or is incrementally
     * scrolled. See {@link OnPageChangeListener}.
     *
     * @param listener Listener to set
     */
    CARAPI SetOnPageChangeListener(
        /* [in] */ IViewPagerOnPageChangeListener* listener);

    /**
     * Set a {@link PageTransformer} that will be called for each attached page whenever
     * the scroll position is changed. This allows the application to apply custom property
     * transformations to each page, overriding the default sliding look and feel.
     *
     * <p><em>Note:</em> Prior to Android 3.0 the property animation APIs did not exist.
     * As a result, setting a PageTransformer prior to Android 3.0 (API 11) will have no effect.</p>
     *
     * @param reverseDrawingOrder true if the supplied PageTransformer requires page views
     *                            to be drawn from last to first instead of first to last.
     * @param transformer PageTransformer that will modify each page's animation properties
     */
    CARAPI SetPageTransformer(
        /* [in] */ Boolean reverseDrawingOrder,
        /* [in] */ IViewPagerPageTransformer* transformer);

    /**
     * Returns the number of pages that will be retained to either side of the
     * current page in the view hierarchy in an idle state. Defaults to 1.
     *
     * @return How many pages will be kept offscreen on either side
     * @see #setOffscreenPageLimit(int)
     */
    CARAPI GetOffscreenPageLimit(
        /* [out] */ Int32* value);

    /**
     * Set the number of pages that should be retained to either side of the
     * current page in the view hierarchy in an idle state. Pages beyond this
     * limit will be recreated from the adapter when needed.
     *
     * <p>This is offered as an optimization. If you know in advance the number
     * of pages you will need to support or have lazy-loading mechanisms in place
     * on your pages, tweaking this setting can have benefits in perceived smoothness
     * of paging animations and interaction. If you have a small number of pages (3-4)
     * that you can keep active all at once, less time will be spent in layout for
     * newly created view subtrees as the user pages back and forth.</p>
     *
     * <p>You should keep this limit low, especially if your pages have complex layouts.
     * This setting defaults to 1.</p>
     *
     * @param limit How many pages will be kept offscreen in an idle state.
     */
    CARAPI SetOffscreenPageLimit(
        /* [in] */ Int32 limit);

    /**
     * Set the margin between pages.
     *
     * @param marginPixels Distance between adjacent pages in pixels
     * @see #getPageMargin()
     * @see #setPageMarginDrawable(Drawable)
     * @see #setPageMarginDrawable(int)
     */
    CARAPI SetPageMargin(
        /* [in] */ Int32 marginPixels);

    /**
     * Return the margin between pages.
     *
     * @return The size of the margin in pixels
     */
    CARAPI GetPageMargin(
        /* [out] */ Int32* margin);

    /**
     * Set a drawable that will be used to fill the margin between pages.
     *
     * @param d Drawable to display between pages
     */
    CARAPI SetPageMarginDrawable(
        /* [in] */ IDrawable* d);

    /**
     * Set a drawable that will be used to fill the margin between pages.
     *
     * @param resId Resource ID of a drawable to display between pages
     */
    CARAPI SetPageMarginDrawable(
        /* [in] */ Int32 resId);

    // @Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    // @Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

    // @Override
    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    // @Override
    CARAPI RemoveView(
        /* [in] */ IView* view);

    // @Override
    CARAPI ComputeScroll();

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Start a fake drag of the pager.
     *
     * <p>A fake drag can be useful if you want to synchronize the motion of the ViewPager
     * with the touch scrolling of another view, while still letting the ViewPager
     * control the snapping motion and fling behavior. (e.g. parallax-scrolling tabs.)
     * Call {@link #fakeDragBy(float)} to simulate the actual drag motion. Call
     * {@link #endFakeDrag()} to complete the fake drag and fling as necessary.
     *
     * <p>During a fake drag the ViewPager will ignore all touch events. If a real drag
     * is already in progress, this method will return false.
     *
     * @return true if the fake drag began successfully, false if it could not be started.
     *
     * @see #fakeDragBy(float)
     * @see #endFakeDrag()
     */
    CARAPI BeginFakeDrag(
        /* [out] */ Boolean* result);

    /**
     * End a fake drag of the pager.
     *
     * @see #beginFakeDrag()
     * @see #fakeDragBy(float)
     */
    CARAPI EndFakeDrag();

    /**
     * Fake drag by an offset in pixels. You must have called {@link #beginFakeDrag()} first.
     *
     * @param xOffset Offset in pixels to drag by.
     * @see #beginFakeDrag()
     * @see #endFakeDrag()
     */
    CARAPI FakeDragBy(
        /* [in] */ Float xOffset);

    /**
     * Returns true if a fake drag is in progress.
     *
     * @return true if currently in a fake drag, false otherwise.
     *
     * @see #beginFakeDrag()
     * @see #fakeDragBy(float)
     * @see #endFakeDrag()
     */
    CARAPI IsFakeDragging(
        /* [out] */ Boolean* result);

    CARAPI CanScrollHorizontally(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* canScroll);

    /**
     * Tests scrollability within child views of v given a delta of dx.
     *
     * @param v View to test for horizontal scrollability
     * @param checkV Whether the view v passed should itself be checked for scrollability (true),
     *               or just its children (false).
     * @param dx Delta scrolled in pixels
     * @param x X coordinate of the active touch point
     * @param y Y coordinate of the active touch point
     * @return true if child views of v can be scrolled by delta of dx.
     */
    CARAPI CanScroll(
        /* [in] */ IView* v,
        /* [in] */ Boolean checkV,
        /* [in] */ Int32 dx,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* canScroll);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* res);

    /**
     * You can call this function yourself to have the scroll view perform
     * scrolling from a key event, just as if the event had been dispatched to
     * it by the view hierarchy.
     *
     * @param event The key event to execute.
     * @return Return true if the event was handled, else false.
     */
    CARAPI ExecuteKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI ArrowScroll(
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    /**
     * We only want the current page that is being shown to be focusable.
     */
    // @Override
    CARAPI AddFocusables(
        /* [in] */ IArrayList* views,
        /* [in] */ Int32 direction,
        /* [in] */ Int32 focusableMode);

    /**
     * We only want the current page that is being shown to be touchable.
     */
    // @Override
    CARAPI AddTouchables(
        /* [in] */ IArrayList* views);

    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** params);

protected:
    CARAPI_(void) InitViewPager();

    // @Override
    CARAPI OnDetachedFromWindow();

    CARAPI_(void) SetOnAdapterChangeListener(
        /* [in] */ IViewPagerOnAdapterChangeListener* listener);

    CARAPI_(void) SetCurrentItemInternal(
        /* [in] */ Int32 item,
        /* [in] */ Boolean smoothScroll,
        /* [in] */ Boolean always);

    CARAPI_(void) SetCurrentItemInternal(
        /* [in] */ Int32 item,
        /* [in] */ Boolean smoothScroll,
        /* [in] */ Boolean always,
        /* [in] */ Int32 velocity);

    CARAPI_(void) SetChildrenDrawingOrderEnabledCompat(
        /* [in] */ Boolean enable);

    CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    /**
     * Set a separate OnPageChangeListener for internal use by the support library.
     *
     * @param listener Listener to set
     * @return The old listener that was set, if any.
     */
    CARAPI_(AutoPtr<IViewPagerOnPageChangeListener>) SetInternalPageChangeListener(
        /* [in] */ IViewPagerOnPageChangeListener* listener);

    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DrawableStateChanged();

    // We want the duration of the page snap animation to be influenced by the distance that
    // the screen has to travel, however, we don't want this duration to be effected in a
    // purely linear fashion. Instead, we use this method to moderate the effect that the distance
    // of travel has on the overall snap duration.
    CARAPI_(Float) DistanceInfluenceForSnapDuration(
        /* [in] */ Float f);

    /**
     * Like {@link View#scrollBy}, but scroll smoothly instead of immediately.
     *
     * @param x the number of pixels to scroll by on the X axis
     * @param y the number of pixels to scroll by on the Y axis
     */
    CARAPI_(void) SmoothScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Like {@link View#scrollBy}, but scroll smoothly instead of immediately.
     *
     * @param x the number of pixels to scroll by on the X axis
     * @param y the number of pixels to scroll by on the Y axis
     * @param velocity the velocity associated with a fling, if applicable. (0 otherwise)
     */
    CARAPI_(void) SmoothScrollTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 velocity);

    CARAPI_(AutoPtr<ItemInfo>) AddNewItem(
        /* [in] */ Int32 position,
        /* [in] */ Int32 index);

    CARAPI_(void) DataSetChanged();

    CARAPI Populate();

    CARAPI Populate(
        /* [in] */ Int32 newCurrentItem);

    CARAPI_(AutoPtr<ItemInfo>) InfoForChild(
        /* [in] */ IView* child);

    CARAPI_(AutoPtr<ItemInfo>) InfoForAnyChild(
        /* [in] */ IView* child);

    CARAPI_(AutoPtr<ItemInfo>) InfoForPosition(
        /* [in] */ Int32 position);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * This method will be invoked when the current page is scrolled, either as part
     * of a programmatically initiated smooth scroll or a user initiated touch scroll.
     * If you override this method you must call through to the superclass implementation
     * (e.g. super.onPageScrolled(position, offset, offsetPixels)) before onPageScrolled
     * returns.
     *
     * @param position Position index of the first page currently being displayed.
     *                 Page position+1 will be visible if positionOffset is nonzero.
     * @param offset Value from [0, 1) indicating the offset from the page at position.
     * @param offsetPixels Value in pixels indicating the offset from position.
     */
    CARAPI OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float offset,
        /* [in] */ Int32 offsetPixels);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(Boolean) PageLeft();

    CARAPI_(Boolean) PageRight();

    /**
     * We only want the current page that is being shown to be focusable.
     */
    // @Override
    CARAPI_(Boolean) OnRequestFocusInDescendants(
        /* [in] */ Int32 direction,
        /* [in] */ IRect* previouslyFocusedRect);

    // @Override
    CARAPI GenerateDefaultLayoutParams(
        /* [out] */ IViewGroupLayoutParams** params);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI_(Boolean) CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    static CARAPI_(AutoPtr<IComparator>) InitComparator();

    static CARAPI_(AutoPtr<IInterpolator>) InitInterpolator();

    static CARAPI_(AutoPtr<ViewPositionComparator>) InitVPComparator();

    CARAPI_(void) SetScrollState(
        /* [in] */ Int32 newState);

    CARAPI_(void) RemoveNonDecorViews();

    CARAPI_(Int32) GetClientWidth();

    CARAPI_(void) ScrollToItem(
        /* [in] */ Int32 item,
        /* [in] */ Boolean smoothScroll,
        /* [in] */ Int32 velocity,
        /* [in] */ Boolean dispatchSelected);

    CARAPI_(void) SortChildDrawingOrder();

    CARAPI_(void) CalculatePageOffsets(
        /* [in] */ ItemInfo* curItem,
        /* [in] */ Int32 curIndex,
        /* [in] */ ItemInfo* oldCurInfo);

    CARAPI_(void) RecomputeScrollPosition(
        /* [in] */ Int32 width,
        /* [in] */ Int32 oldWidth,
        /* [in] */ Int32 margin,
        /* [in] */ Int32 oldMargin);

    CARAPI PageScrolled(
        /* [in] */ Int32 xpos,
        /* [out] */ Boolean* result);

    CARAPI_(void) CompleteScroll(
        /* [in] */ Boolean postEvents);

    CARAPI_(Boolean) IsGutterDrag(
        /* [in] */ Float x,
        /* [in] */ Float dx);

    CARAPI_(void) EnableLayers(
        /* [in] */ Boolean enable);

    CARAPI_(void) RequestParentDisallowInterceptTouchEvent(
        /* [in] */ Boolean disallowIntercept);

    CARAPI_(Boolean) PerformDrag(
        /* [in] */ Float x);

    /**
     * @return Info about the page at the current scroll position.
     *         This can be synthetic for a missing middle page; the 'object' field can be null.
     */
    CARAPI_(AutoPtr<ItemInfo>) InfoForCurrentScrollPosition();

    CARAPI_(Int32) DetermineTargetPage(
        /* [in] */ Int32 currentPage,
        /* [in] */ Float pageOffset,
        /* [in] */ Int32 velocity,
        /* [in] */ Int32 deltaX);

    CARAPI_(void) OnSecondaryPointerUp(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) EndDrag();

    CARAPI_(void) SetScrollingCacheEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(AutoPtr<IRect>) GetChildRectInPagerCoordinates(
        /* [in] */ IRect* outRect,
        /* [in] */ IView* child);

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;

    static const Boolean USE_CACHE = FALSE;

    static const Int32 DEFAULT_OFFSCREEN_PAGES = 1;
    static const Int32 MAX_SETTLE_DURATION = 600; // ms
    static const Int32 MIN_DISTANCE_FOR_FLING = 25; // dips

    static const Int32 DEFAULT_GUTTER_SIZE = 16; // dips

    static const Int32 MIN_FLING_VELOCITY = 400; // dips

    static const AutoPtr<ArrayOf<Int32> > LAYOUT_ATTRS;

    static const AutoPtr<IComparator> COMPARATOR;

    static const AutoPtr<IInterpolator> sInterpolator;

    /**
     * Sentinel value for no current active pointer.
     * Used by {@link #mActivePointerId}.
     */
    static const Int32 INVALID_POINTER = -1;

    // If the pager is at least this close to its final position, complete the scroll
    // on touch down and let the user interact with the content inside instead of
    // "catching" the flinging pager.
    static const Int32 CLOSE_ENOUGH = 2; // dp

    static const Int32 DRAW_ORDER_DEFAULT = 0;
    static const Int32 DRAW_ORDER_FORWARD = 1;
    static const Int32 DRAW_ORDER_REVERSE = 2;

    static const AutoPtr<ViewPositionComparator> sPositionComparator;

    /**
     * Used to track what the expected number of items in the adapter should be.
     * If the app changes this when we don't expect it, we'll throw a big obnoxious exception.
     */
    Int32 mExpectedAdapterCount;

    AutoPtr<IArrayList> mItems;
    AutoPtr<ItemInfo> mTempItem;

    AutoPtr<IRect> mTempRect;

    AutoPtr<IPagerAdapter> mAdapter;
    Int32 mCurItem;   // Index of currently displayed page.
    Int32 mRestoredCurItem;
    AutoPtr<IParcelable> mRestoredAdapterState;
    AutoPtr<IClassLoader> mRestoredClassLoader;
    AutoPtr<IScroller> mScroller;
    AutoPtr<PagerObserver> mObserver;

    Int32 mPageMargin;
    AutoPtr<IDrawable> mMarginDrawable;
    Int32 mTopPageBounds;
    Int32 mBottomPageBounds;

    // Offsets of the first and last items, if known.
    // Set during population, used to determine if we are at the beginning
    // or end of the pager data set during touch scrolling.
    Float mFirstOffset;
    Float mLastOffset;

    Int32 mChildWidthMeasureSpec;
    Int32 mChildHeightMeasureSpec;
    Boolean mInLayout;

    Boolean mScrollingCacheEnabled;

    Boolean mPopulatePending;
    Int32 mOffscreenPageLimit;

    Boolean mIsBeingDragged;
    Boolean mIsUnableToDrag;
    Boolean mIgnoreGutter;
    Int32 mDefaultGutterSize;
    Int32 mGutterSize;
    Int32 mTouchSlop;
    /**
     * Position of the last motion event.
     */
    Float mLastMotionX;
    Float mLastMotionY;
    Float mInitialMotionX;
    Float mInitialMotionY;
    /**
     * ID of the active pointer. This is used to retain consistency during
     * drags/flings if multiple pointers are used.
     */
    Int32 mActivePointerId;

    /**
     * Determines speed during touch scrolling
     */
    AutoPtr<IVelocityTracker> mVelocityTracker;
    Int32 mMinimumVelocity;
    Int32 mMaximumVelocity;
    Int32 mFlingDistance;
    Int32 mCloseEnough;

    Boolean mFakeDragging;
    Int64 mFakeDragBeginTime;

    AutoPtr<IEdgeEffect> mLeftEdge;
    AutoPtr<IEdgeEffect> mRightEdge;

    Boolean mFirstLayout;
    Boolean mNeedCalculatePageOffsets;
    Boolean mCalledSuper;
    Int32 mDecorChildCount;

    AutoPtr<IViewPagerOnPageChangeListener> mOnPageChangeListener;
    AutoPtr<IViewPagerOnPageChangeListener> mInternalPageChangeListener;
    AutoPtr<IViewPagerOnAdapterChangeListener> mAdapterChangeListener;
    AutoPtr<IViewPagerPageTransformer> mPageTransformer;
    // AutoPtr<IMethodInfo> mSetChildrenDrawingOrderEnabled;

    Int32 mDrawingOrder;
    AutoPtr<IArrayList> mDrawingOrderedChildren;

    AutoPtr<IRunnable> mEndScrollRunnable;

    Int32 mScrollState;

    friend class MyAccessibilityDelegate;
    friend class PagerObserver;
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V4_VIEW_VIEWPAGER_H__
