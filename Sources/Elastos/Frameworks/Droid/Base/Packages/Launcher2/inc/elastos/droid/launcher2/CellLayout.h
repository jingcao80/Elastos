
#ifndef  __ELASTOS_DROID_LAUNCHER2_CELLLAYOUT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CELLLAYOUT_H__

#include "elastos/droid/launcher2/BubbleTextView.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "elastos/droid/launcher2/FolderIcon.h"
#include "elastos/droid/launcher2/ShortcutAndWidgetContainer.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/view/ViewGroup.h>
#include <elastos/droid/view/animation/LayoutAnimationController.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::Animation::LayoutAnimationController;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IStack;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class CellLayout
    : public ViewGroup
    , public ICellLayout
{
public:
    // Class which represents the reorder hint animations. These animations show that an item is
    // in a temporary state, and hint at where the item will return to.
    class ReorderHintAnimation : public Object
    {
    private:
        class AnimatorUpdateListener
            : public Object
            , public IAnimatorUpdateListener
        {
        public:
            AnimatorUpdateListener(
                /* [in] */ ReorderHintAnimation* host);

            CAR_INTERFACE_DECL()

            // @Override
            CARAPI OnAnimationUpdate(
                /* [in] */ IValueAnimator* animation);

        private:
            ReorderHintAnimation* mHost;
        };

        class MyAnimatorListenerAdapter
            : public Object
            , public AnimatorListenerAdapter
        {
        public:
            MyAnimatorListenerAdapter(
                /* [in] */ ReorderHintAnimation* host);

            // @Override
            CARAPI OnAnimationRepeat(
                /* [in] */ IAnimator* animation);

        private:
            ReorderHintAnimation* mHost;
        };

    public:
        ReorderHintAnimation(
            /* [in] */ IView* child,
            /* [in] */ Int32 cellX0,
            /* [in] */ Int32 cellY0,
            /* [in] */ Int32 cellX1,
            /* [in] */ Int32 cellY1,
            /* [in] */ Int32 spanX,
            /* [in] */ Int32 spanY,
            /* [in] */ CellLayout* host);

        CARAPI_(void) Animate();

    private:
        CARAPI_(void) Cancel();

        CARAPI_(void) CompleteAnimationImmediately();

    public:
        AutoPtr<IView> mChild;
        Float mFinalDeltaX;
        Float mFinalDeltaY;
        Float mInitDeltaX;
        Float mInitDeltaY;
        Float mFinalScale;
        Float mInitScale;
        AutoPtr<IAnimator> mA;

    private:
        static const Int32 DURATION = 300;
        CellLayout* mHost;
    };

    class CellLayoutAnimationController : public LayoutAnimationController
    {
    public:
        CellLayoutAnimationController(
            /* [in] */ IAnimation* animation,
            /* [in] */ Float delay);

    protected:
        // @Override
        CARAPI_(Int64) GetDelayForView(
            /* [in] */ IView* view);
    };

    class LayoutParams
        : public ViewGroup::MarginLayoutParams
        , public ICellLayoutLayoutParams
    {
    public:
        LayoutParams();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IContext* c,
            /* [in] */ IAttributeSet* attrs);

        CARAPI constructor(
            /* [in] */ IViewGroupLayoutParams* source);

        CARAPI constructor(
            /* [in] */ LayoutParams* source);

        CARAPI constructor(
            /* [in] */ Int32 cellX,
            /* [in] */ Int32 cellY,
            /* [in] */ Int32 cellHSpan,
            /* [in] */ Int32 cellVSpan);

        CARAPI_(void) Setup(
            /* [in] */ Int32 cellWidth,
            /* [in] */ Int32 cellHeight,
            /* [in] */ Int32 widthGap,
            /* [in] */ Int32 heightGap,
            /* [in] */ Boolean invertHorizontally,
            /* [in] */ Int32 colCount);

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI_(void) SetX(
            /* [in] */ Int32 x);

        CARAPI_(Int32) GetX();

        CARAPI_(void) SetY(
            /* [in] */ Int32 y);

        CARAPI_(Int32) GetY();

    public:
        /**
         * Horizontal location of the item in the grid.
         */
        // @ViewDebug.ExportedProperty
        Int32 mCellX;

        /**
         * Vertical location of the item in the grid.
         */
        // @ViewDebug.ExportedProperty
        Int32 mCellY;

        /**
         * Temporary horizontal location of the item in the grid during reorder
         */
        Int32 mTmpCellX;

        /**
         * Temporary vertical location of the item in the grid during reorder
         */
        Int32 mTmpCellY;

        /**
         * Indicates that the temporary coordinates should be used to layout the items
         */
        Boolean mUseTmpCoords;

        /**
         * Number of cells spanned horizontally by the item.
         */
        // @ViewDebug.ExportedProperty
        Int32 mCellHSpan;

        /**
         * Number of cells spanned vertically by the item.
         */
        // @ViewDebug.ExportedProperty
        Int32 mCellVSpan;

        /**
         * Indicates whether the item will set its x, y, width and height parameters freely,
         * or whether these will be computed based on cellX, cellY, cellHSpan and cellVSpan.
         */
        Boolean mIsLockedToGrid;

        /**
         * Indicates whether this item can be reordered. Always TRUE except in the case of the
         * the AllApps button.
         */
        Boolean mCanReorder;

        // X coordinate of the view in the layout.
        // @ViewDebug.ExportedProperty
        Int32 mX;
        // Y coordinate of the view in the layout.
        // @ViewDebug.ExportedProperty
        Int32 mY;

        Boolean mDropped;
    };

    // This class stores info for two purposes:
    // 1. When dragging items (mDragInfo in Workspace), we store the View, its cellX & cellY,
    //    its spanX, spanY, and the screen it is on
    // 2. When long clicking on an empty cell in a CellLayout, we save information about the
    //    cellX and cellY coordinates and which page was clicked. We then set this as a tag on
    //    the CellLayout that was long clicked
    class CellInfo : public Object
    {
    public:
        CellInfo();

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        AutoPtr<IView> mCell;
        Int32 mCellX;
        Int32 mCellY;
        Int32 mSpanX;
        Int32 mSpanY;
        Int32 mScreen;
        Int64 mContainer;
    };

private:
    class AnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener(
            /* [in] */ CellLayout* host,
            /* [in] */ InterruptibleInOutAnimator* anim,
            /* [in] */ Int32 thisIndex);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        CellLayout* mHost;
        AutoPtr<InterruptibleInOutAnimator> mAnim;
        Int32 mThisIndex;
    };

    class MyAnimatorListenerAdapter
        : public Object
        , public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ InterruptibleInOutAnimator* anim);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<InterruptibleInOutAnimator> mAnim;
    };


    class AnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        AnimatorUpdateListener2(
            /* [in] */ LayoutParams* lp,
            /* [in] */ Int32 oldX,
            /* [in] */ Int32 oldY,
            /* [in] */ Int32 newX,
            /* [in] */ Int32 newY,
            /* [in] */ IView* child);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<LayoutParams> mLp;
        Int32 mOldX;
        Int32 mOldY;
        Int32 mNewX;
        Int32 mNewY;
        AutoPtr<IView> mChild;
    };

    class MyAnimatorListenerAdapter2
        : public Object
        , public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ CellLayout* mHost,
            /* [in] */ LayoutParams* lp,
            /* [in] */ IView* child);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        CellLayout* mHost;
        AutoPtr<LayoutParams> mLp;
        AutoPtr<IView> mChild;
        Boolean mCancelled;
    };


    /**
     * This helper class defines a cluster of views. It helps with defining complex edges
     * of the cluster and determining how those edges interact with other views. The edges
     * essentially define a fine-grained boundary around the cluster of views -- like a more
     * precise version of a bounding box.
     */
    class ViewCluster : public Object
    {
    private:
        class PositionComparator
            : public Object
            , public IComparator
        {
        public:
            PositionComparator(
                /* [in] */ ViewCluster* host);

            CAR_INTERFACE_DECL()

            CARAPI Compare(
                /* [in] */ IInterface* left,
                /* [in] */ IInterface* right,
                /* [out] */ Int32* result);

        public:
            ViewCluster* mHost;
            Int32 mWhichEdge;
        };

    public:
        // @SuppressWarnings("unchecked")
        ViewCluster(
            /* [in] */ IArrayList* views,
            /* [in] */ ItemConfiguration* config,
            /* [in] */ CellLayout* host);

        CARAPI_(void) ResetEdges();

        CARAPI_(void) ComputeEdge(
            /* [in] */ Int32 which,
            /* [in] */ ArrayOf<Int32>* edge);

        CARAPI_(Boolean) IsViewTouchingEdge(
            /* [in] */ IView* v,
            /* [in] */ Int32 whichEdge);

        CARAPI_(void) Shift(
            /* [in] */ Int32 whichEdge,
            /* [in] */ Int32 delta);

        CARAPI_(void) AddView(
            /* [in] */ IView* v);

        CARAPI_(AutoPtr<IRect>) GetBoundingRect();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetEdge(
            /* [in] */ Int32 which);

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetLeftEdge();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetRightEdge();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetTopEdge();

        CARAPI_(AutoPtr<ArrayOf<Int32> >) GetBottomEdge();

        CARAPI_(void) SortConfigurationForEdgePush(
            /* [in] */ Int32 edge);

    public:
        static const Int32 LEFT = 0;
        static const Int32 TOP = 1;
        static const Int32 RIGHT = 2;
        static const Int32 BOTTOM = 3;

        AutoPtr<IArrayList> mViews;
        AutoPtr<ItemConfiguration> mConfig;
        AutoPtr<IRect> mBoundingRect;

        AutoPtr<ArrayOf<Int32> > mLeftEdge;
        AutoPtr<ArrayOf<Int32> > mRightEdge;
        AutoPtr<ArrayOf<Int32> > mTopEdge;
        AutoPtr<ArrayOf<Int32> > mBottomEdge;
        Boolean mLeftEdgeDirty;
        Boolean mRightEdgeDirty;
        Boolean mTopEdgeDirty;
        Boolean mBottomEdgeDirty;
        Boolean mBoundingRectDirty;

    private:
        CellLayout* mHost;
        AutoPtr<PositionComparator> mComparator;
    };

    class ItemConfiguration : public Object
    {
    public:
        ItemConfiguration();

        CARAPI_(void) Save();

        CARAPI_(void) Restore();

        CARAPI_(void) Add(
            /* [in] */ IView* v,
            /* [in] */ CellAndSpan* cs);

        CARAPI_(Int32) Area();

    public:
        AutoPtr<IHashMap> mMap;//<View, CellAndSpan>
        AutoPtr<IArrayList> mSortedViews;// item is view
        Boolean mIsSolution;
        Int32 mDragViewX;
        Int32 mDragViewY;
        Int32 mDragViewSpanX;
        Int32 mDragViewSpanY;

    private:
        AutoPtr<IHashMap> mSavedMap;//<View, CellAndSpan>
    };

    class CellAndSpan : public Object
    {
    public:
        CellAndSpan();

        CellAndSpan(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ Int32 spanX,
            /* [in] */ Int32 spanY);

        CARAPI_(void) Copy(
            /* [in] */ CellAndSpan* copy);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int32 mX;
        Int32 mY;
        Int32 mSpanX;
        Int32 mSpanY;
    };

public:
    CellLayout();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    static CARAPI_(Int32) WidthInPortrait(
        /* [in] */ IResources* r,
        /* [in] */ Int32 numCells);

    static CARAPI_(Int32) HeightInLandscape(
        /* [in] */ IResources* r,
        /* [in] */ Int32 numCells);

    CARAPI_(void) EnableHardwareLayers();

    CARAPI_(void) DisableHardwareLayers();

    CARAPI_(void) BuildHardwareLayer();

    CARAPI_(Float) GetChildrenScale();

    CARAPI_(void) SetGridSize(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // Set whether or not to invert the layout horizontally if the layout is in RTL mode.
    CARAPI_(void) SetInvertIfRtl(
        /* [in] */ Boolean invert);

    CARAPI_(void) SetOverScrollAmount(
        /* [in] */ Float r,
        /* [in] */ Boolean left);

    CARAPI_(void) SetPressedOrFocusedIcon(
        /* [in] */ BubbleTextView* icon);

    CARAPI_(void) SetIsDragOverlapping(
        /* [in] */ Boolean isDragOverlapping);

    CARAPI_(Boolean) GetIsDragOverlapping();

    CARAPI_(void) ScaleRect(
        /* [in] */ IRect* r,
        /* [in] */ Float scale);

    CARAPI_(void) ScaleRectAboutCenter(
        /* [in] */ IRect* in,
        /* [in] */ IRect* out,
        /* [in] */ Float scale);

    CARAPI_(void) ShowFolderAccept(
        /* [in] */ FolderIcon::FolderRingAnimator* fra);

    CARAPI_(void) HideFolderAccept(
        /* [in] */ FolderIcon::FolderRingAnimator* fra);

    CARAPI_(void) SetFolderLeaveBehindCell(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) ClearFolderLeaveBehind();

    // @Override
    CARAPI ShouldDelayChildPressedState(
        /* [out] */ Boolean* res);

    CARAPI_(void) RestoreInstanceState(
        /* [in] */ ISparseArray* states);

    // @Override
    CARAPI CancelLongPress();

    CARAPI_(void) SetOnInterceptTouchListener(
        /* [in] */ IViewOnTouchListener* listener);

    CARAPI_(Int32) GetCountX();

    CARAPI_(Int32) GetCountY();

    CARAPI_(void) SetIsHotseat(
        /* [in] */ Boolean isHotseat);

    CARAPI_(Boolean) AddViewToCellLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ Int32 childId,
        /* [in] */ LayoutParams* params,
        /* [in] */ Boolean markCells);

    // @Override
    CARAPI RemoveAllViews();

    // @Override
    CARAPI RemoveAllViewsInLayout();

    CARAPI RemoveViewWithoutMarkingCells(
        /* [in] */ IView* view);

    // @Override
    CARAPI RemoveView(
        /* [in] */ IView* view);

    // @Override
    CARAPI RemoveViewAt(
        /* [in] */ Int32 index);

    // @Override
    CARAPI RemoveViewInLayout(
        /* [in] */ IView* view);

    // @Override
    CARAPI RemoveViews(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    // @Override
    CARAPI RemoveViewsInLayout(
        /* [in] */ Int32 start,
        /* [in] */ Int32 count);

    CARAPI_(void) SetTagToCellInfoForPoint(
        /* [in] */ Int32 touchX,
        /* [in] */ Int32 touchY);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI_(AutoPtr<CellInfo>) GetTag();

    /**
     * Given a point, return the cell that strictly encloses that point
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
     * @param result Array of 2 ints to hold the x and y coordinate of the cell
     */
    CARAPI_(void) PointToCellExact(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Given a point, return the cell that most closely encloses that point
     * @param x X coordinate of the point
     * @param y Y coordinate of the point
     * @param result Array of 2 ints to hold the x and y coordinate of the cell
     */
    CARAPI_(void) PointToCellRounded(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Given a cell coordinate, return the point that represents the upper left corner of that cell
     *
     * @param cellX X coordinate of the cell
     * @param cellY Y coordinate of the cell
     *
     * @param result Array of 2 ints to hold the x and y coordinate of the point
     */
    CARAPI_(void) CellToPoint(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Given a cell coordinate, return the point that represents the center of the cell
     *
     * @param cellX X coordinate of the cell
     * @param cellY Y coordinate of the cell
     *
     * @param result Array of 2 ints to hold the x and y coordinate of the point
     */
    CARAPI_(void) CellToCenterPoint(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Given a cell coordinate and span return the point that represents the center of the regio
     *
     * @param cellX X coordinate of the cell
     * @param cellY Y coordinate of the cell
     *
     * @param result Array of 2 ints to hold the x and y coordinate of the point
     */
    CARAPI_(void) RegionToCenterPoint(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* result);

     /**
     * Given a cell coordinate and span fills out a corresponding pixel rect
     *
     * @param cellX X coordinate of the cell
     * @param cellY Y coordinate of the cell
     * @param result Rect in which to write the result
     */
    CARAPI_(void) RegionToRect(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IRect* result);

    CARAPI_(Float) GetDistanceFromCell(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ ArrayOf<Int32>* cell);

    CARAPI_(Int32) GetCellWidth();

    CARAPI_(Int32) GetCellHeight();

    CARAPI_(Int32) GetWidthGap();

    CARAPI_(Int32) GetHeightGap();

    CARAPI_(AutoPtr<IRect>) GetContentRect(
        /* [in] */ IRect* r);

    static CARAPI_(void) GetMetrics(
        /* [in] */ IRect* metrics,
        /* [in] */ IResources* res,
        /* [in] */ Int32 measureWidth,
        /* [in] */ Int32 measureHeight,
        /* [in] */ Int32 countX,
        /* [in] */ Int32 countY,
        /* [in] */ Int32 orientation);

    CARAPI_(Float) GetBackgroundAlpha();

    CARAPI_(void) SetBackgroundAlphaMultiplier(
        /* [in] */ Float multiplier);

    CARAPI_(Float) GetBackgroundAlphaMultiplier();

    CARAPI_(void) SetBackgroundAlpha(
        /* [in] */ Float alpha);

    CARAPI_(void) SetShortcutAndWidgetAlpha(
        /* [in] */ Float alpha);

    CARAPI_(AutoPtr<ShortcutAndWidgetContainer>) GetShortcutsAndWidgets();

    CARAPI_(AutoPtr<IView>) GetChildAt(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(Boolean) AnimateChildToPosition(
        /* [in] */ IView* child,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 duration,
        /* [in] */ Int32 delay,
        /* [in] */ Boolean permanent,
        /* [in] */ Boolean adjustOccupied);

    /**
     * Estimate where the top left cell of the dragged item will land if it is dropped.
     *
     * @param originX The X value of the top left corner of the item
     * @param originY The Y value of the top left corner of the item
     * @param spanX The number of horizontal cells that the item spans
     * @param spanY The number of vertical cells that the item spans
     * @param result The estimated drop cell X and Y.
     */
    CARAPI_(void) EstimateDropCell(
        /* [in] */ Int32 originX,
        /* [in] */ Int32 originY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* result);

    CARAPI_(void) VisualizeDropLocation(
        /* [in] */ IView* v,
        /* [in] */ IBitmap* dragOutline,
        /* [in] */ Int32 originX,
        /* [in] */ Int32 originY,
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Boolean resize,
        /* [in] */ IPoint* dragOffset,
        /* [in] */ IRect* dragRegion);

    CARAPI_(void) ClearDragOutlines();

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param result Array in which to place the result, or NULL (in which case a new array will
     *        be allocated)
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestVacantArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param minSpanX The minimum horizontal span required
     * @param minSpanY The minimum vertical span required
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param result Array in which to place the result, or NULL (in which case a new array will
     *        be allocated)
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestVacantArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* result,
        /* [in] */ ArrayOf<Int32>* resultSpan);

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param ignoreOccupied If TRUE, the result can be an occupied cell
     * @param result Array in which to place the result, or NULL (in which case a new array will
     *        be allocated)
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY, View ignoreView,
        /* [in] */ Boolean ignoreOccupied,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param minSpanX The minimum horizontal span required
     * @param minSpanY The minimum vertical span required
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param ignoreOccupied If TRUE, the result can be an occupied cell
     * @param result Array in which to place the result, or NULL (in which case a new array will
     *        be allocated)
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* ignoreView,
        /* [in] */ Boolean ignoreOccupied,
        /* [in] */ ArrayOf<Int32>* result,
        /* [in] */ ArrayOf<Int32>* resultSpan,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    CARAPI_(AutoPtr<ItemConfiguration>) SimpleSwap(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ IView* dragView,
        /* [in] */ Boolean decX,
        /* [in] */ ItemConfiguration* solution);

    CARAPI_(void) SetUseTempCoords(
        /* [in] */ Boolean useTempCoords);

    CARAPI_(AutoPtr<ItemConfiguration>) FindConfigurationNoShuffle(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ ItemConfiguration* solution);

    CARAPI_(void) PrepareChildForDrag(
        /* [in] */ IView* child);

    CARAPI_(Boolean) IsNearestDropLocationOccupied(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ ArrayOf<Int32>* _result);

    CARAPI_(void) RevertTempState();

    CARAPI_(Boolean) createAreaForResize(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ Boolean commit);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) CreateArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ ArrayOf<Int32>* _result,
        /* [in] */ ArrayOf<Int32>* resultSpan,
        /* [in] */ Int32 mode);

    CARAPI_(void) SetItemPlacementDirty(
        /* [in] */ Boolean dirty);

    CARAPI_(Boolean) IsItemPlacementDirty();

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param ignoreView Considers space occupied by this view as unoccupied
     * @param result Previously returned value to possibly recycle.
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestVacantArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* ignoreView,
        /* [in] */ ArrayOf<Int32>* result);

    /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param minSpanX The minimum horizontal span required
     * @param minSpanY The minimum vertical span required
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param ignoreView Considers space occupied by this view as unoccupied
     * @param result Previously returned value to possibly recycle.
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestVacantArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 minSpanX,
        /* [in] */ Int32 minSpanY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY, View ignoreView,
        /* [in] */ ArrayOf<Int32>* result,
        /* [in] */ ArrayOf<Int32>* resultSpan);

    /**
     * Find a starting cell position that will fit the given bounds nearest the requested
     * cell location. Uses Euclidean distance to score multiple vacant areas.
     *
     * @param pixelX The X location at which you want to search for a vacant area.
     * @param pixelY The Y location at which you want to search for a vacant area.
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param ignoreView Considers space occupied by this view as unoccupied
     * @param result Previously returned value to possibly recycle.
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestArea(
        /* [in] */ Int32 pixelX,
        /* [in] */ Int32 pixelY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* result);

    CARAPI_(Boolean) ExistsEmptyCell();

    /**
     * Finds the upper-left coordinate of the first rectangle in the grid that can
     * hold a cell of the specified dimensions. If intersectX and intersectY are not -1,
     * then this method will only return coordinates for rectangles that contain the cell
     * (intersectX, intersectY)
     *
     * @param cellXY The array that will contain the position of a vacant cell if such a cell
     *               can be found.
     * @param spanX The horizontal span of the cell we want to find.
     * @param spanY The vertical span of the cell we want to find.
     *
     * @return True if a vacant cell of the specified dimension was found, FALSE otherwise.
     */
    CARAPI_(Boolean) FindCellForSpan(
        /* [in] */ ArrayOf<Int32>* cellXY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY);

    /**
     * Like above, but ignores any cells occupied by the item "ignoreView"
     *
     * @param cellXY The array that will contain the position of a vacant cell if such a cell
     *               can be found.
     * @param spanX The horizontal span of the cell we want to find.
     * @param spanY The vertical span of the cell we want to find.
     * @param ignoreView The home screen item we should treat as not occupying any space
     * @return
     */
    CARAPI_(Boolean) FindCellForSpanIgnoring(
        /* [in] */ ArrayOf<Int32>* cellXY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* ignoreView);

    /**
     * Like above, but if intersectX and intersectY are not -1, then this method will try to
     * return coordinates for rectangles that contain the cell [intersectX, intersectY]
     *
     * @param spanX The horizontal span of the cell we want to find.
     * @param spanY The vertical span of the cell we want to find.
     * @param ignoreView The home screen item we should treat as not occupying any space
     * @param intersectX The X coordinate of the cell that we should try to overlap
     * @param intersectX The Y coordinate of the cell that we should try to overlap
     *
     * @return True if a vacant cell of the specified dimension was found, FALSE otherwise.
     */
    CARAPI_(Boolean) FindCellForSpanThatIntersects(
        /* [in] */ ArrayOf<Int32>* cellXY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Int32 intersectX,
        /* [in] */ Int32 intersectY);

    /**
     * The superset of the above two methods
     */
    CARAPI_(Boolean) FindCellForSpanThatIntersectsIgnoring(
        /* [in] */ ArrayOf<Int32>* cellXY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Int32 intersectX,
        /* [in] */ Int32 intersectY,
        /* [in] */ IView* ignoreView,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    /**
     * A drag event has begun over this layout.
     * It may have begun over this layout (in which case onDragChild is called first),
     * or it may have begun on another layout.
     */
    CARAPI_(void) OnDragEnter();

    /**
     * Called when drag has left this CellLayout or has been completed (successfully or not)
     */
    CARAPI_(void) OnDragExit();

    /**
     * Mark a child as having been dropped.
     * At the beginning of the drag operation, the child may have been on another
     * screen, but it is re-parented before this method is called.
     *
     * @param child The child that is being dropped
     */
    CARAPI_(void) OnDropChild(
        /* [in] */ IView* child);

    /**
     * Computes a bounding rectangle for a range of cells
     *
     * @param cellX X coordinate of upper left corner expressed as a cell position
     * @param cellY Y coordinate of upper left corner expressed as a cell position
     * @param cellHSpan Width in cells
     * @param cellVSpan Height in cells
     * @param resultRect Rect into which to put the results
     */
    CARAPI_(void) CellToRect(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 cellHSpan,
        /* [in] */ Int32 cellVSpan,
        /* [in] */ IRect* resultRect);

    /**
     * Computes the required horizontal and vertical cell spans to always
     * fit the given rectangle.
     *
     * @param width Width in pixels
     * @param height Height in pixels
     * @param result An array of length 2 in which to store the result (may be NULL).
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) RectToCell(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ArrayOf<Int32>* result);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) RectToCell(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ArrayOf<Int32>* result);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) CellSpansToSize(
        /* [in] */ Int32 hSpans,
        /* [in] */ Int32 vSpans);

    /**
     * Calculate the grid spans needed to fit given item
     */
    CARAPI_(void) CalculateSpans(
        /* [in] */ ItemInfo* info);

    /**
     * Find the first vacant cell, if there is one.
     *
     * @param vacant Holds the x and y coordinate of the vacant cell
     * @param spanX Horizontal cell span.
     * @param spanY Vertical cell span.
     *
     * @return True if a vacant cell was found
     */
    CARAPI_(Boolean) GetVacantCell(
        /* [in] */ ArrayOf<Int32>* vacant,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY);

    static CARAPI_(Boolean) FindVacantCell(
        /* [in] */ ArrayOf<Int32>* vacant,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ Int32 xCount,
        /* [in] */ Int32 yCount,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    CARAPI_(void) OnMove(
        /* [in] */ IView* view,
        /* [in] */ Int32 newCellX,
        /* [in] */ Int32 newCellY,
        /* [in] */ Int32 newSpanX,
        /* [in] */ Int32 newSpanY);

    CARAPI_(void) MarkCellsAsOccupiedForView(
        /* [in] */ IView* view);

    CARAPI_(void) MarkCellsAsOccupiedForView(
        /* [in] */ IView* view,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    CARAPI_(void) MarkCellsAsUnoccupiedForView(
        /* [in] */ IView* view);

    CARAPI_(void) MarkCellsAsUnoccupiedForView(
        /* [in] */ IView* view,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    CARAPI_(Int32) GetDesiredWidth();

    CARAPI_(Int32) GetDesiredHeight();

    CARAPI IsOccupied(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GenerateLayoutParams(
        /* [in] */ IAttributeSet* attrs,
        /* [out] */ IViewGroupLayoutParams** lp);

    CARAPI_(Boolean) LastDownOnOccupiedCell();

protected:
    CARAPI_(void) SetOverscrollTransformsDirty(
        /* [in] */ Boolean dirty);

    CARAPI_(void) ResetOverscrollTransforms();

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnAttachedToWindow();

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    // @Override
    CARAPI_(void) SetChildrenDrawingCacheEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI_(void) SetChildrenDrawnWithCacheEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    Boolean CheckLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

    // @Override
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GenerateLayoutParams(
        /* [in] */ IViewGroupLayoutParams* p);

private:
    CARAPI_(void) InvalidateBubbleTextView(
        /* [in] */ BubbleTextView* icon);

    CARAPI_(void) ClearTagCellInfo();

    CARAPI_(void) LazyInitTempRectStack();

    CARAPI_(void) RecycleTempRects(
        /* [in] */ IStack* used);

     /**
     * Find a vacant area that will fit the given bounds nearest the requested
     * cell location, and will also weigh in a suggested direction vector of the
     * desired location. This method computers distance based on unit grid distances,
     * not pixel distances.
     *
     * @param cellX The X cell nearest to which you want to search for a vacant area.
     * @param cellY The Y cell nearest which you want to search for a vacant area.
     * @param spanX Horizontal span of the object.
     * @param spanY Vertical span of the object.
     * @param direction The favored direction in which the views should move from x, y
     * @param exactDirectionOnly If this parameter is TRUE, then only solutions where the direction
     *        matches exactly. Otherwise we find the best matching direction.
     * @param occoupied The array which represents which cells in the CellLayout are occupied
     * @param blockOccupied The array which represents which cells in the specified block (cellX,
     *        cellY, spanX, spanY) are occupied. This is used when try to move a group of views.
     * @param result Array in which to place the result, or NULL (in which case a new array will
     *        be allocated)
     * @return The X, Y cell of a vacant area that can contain this object,
     *         nearest the requested location.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) FindNearestArea(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* blockOccupied,
        /* [in] */ ArrayOf<Int32>* result);

    CARAPI_(Boolean) AddViewToTempLocation(
        /* [in] */ IView* v,
        /* [in] */ IRect* rectOccupiedByPotentialDrop,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ ItemConfiguration* currentState);

    CARAPI_(Boolean) PushViewsToTempLocation(
        /* [in] */ IArrayList* views,
        /* [in] */ IRect* rectOccupiedByPotentialDrop,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ IView* dragView,
        /* [in] */ ItemConfiguration* currentState);

    CARAPI_(Boolean) AddViewsToTempLocation(
        /* [in] */ IArrayList* views,
        /* [in] */ IRect* rectOccupiedByPotentialDrop,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ IView* dragView,
        /* [in] */ ItemConfiguration* currentState);

    CARAPI_(void) MarkCellsForRect(
        /* [in] */ IRect* r,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
        /* [in] */ Boolean value);

    // This method tries to find a reordering solution which satisfies the push mechanic by trying
    // to push items in each of the cardinal directions, in an order based on the direction vector
    // passed.
    CARAPI_(Boolean) AttemptPushInDirection(
        /* [in] */ IArrayList* intersectingViews,
        /* [in] */ IRect* occupied,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ IView* ignoreView,
        /* [in] */ ItemConfiguration* solution);

    CARAPI_(Boolean) RearrangementExists(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<Int32>* direction,
        /* [in] */ IView* ignoreView,
        /* [in] */ ItemConfiguration* solution);

    /*
     * Returns a pair (x, y), where x,y are in {-1, 0, 1} corresponding to vector between
     * the provided point and the provided cell
     */
    CARAPI_(void) ComputeDirectionVector(
        /* [in] */ Float deltaX,
        /* [in] */ Float deltaY,
        /* [in] */ ArrayOf<Int32>* result);

    CARAPI_(void) CopyOccupiedArray(
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied);

    CARAPI_(void) CopyCurrentStateToSolution(
        /* [in] */ ItemConfiguration* solution,
        /* [in] */ Boolean temp);

    CARAPI_(void) CopySolutionToTempState(
        /* [in] */ ItemConfiguration* solution,
        /* [in] */ IView* dragView);

    CARAPI_(void) AnimateItemsToSolution(
        /* [in] */ ItemConfiguration* solution,
        /* [in] */ IView* dragView,
        /* [in] */ Boolean commitDragView);

    // This method starts or changes the reorder hint animations
    CARAPI_(void) BeginOrAdjustHintAnimations(
        /* [in] */ ItemConfiguration* solution,
        /* [in] */ IView* dragView,
        /* [in] */ Int32 delay);

    CARAPI_(void) CompleteAndClearReorderHintAnimations();

    CARAPI_(void) CommitTempPlacement();

    /* This seems like it should be obvious and straight-forward, but when the direction vector
    needs to match with the notion of the dragView pushing other views, we have to employ
    a slightly more subtle notion of the direction vector. The question is what two points is
    the vector between? The center of the dragView and its desired destination? Not quite, as
    this doesn't necessarily coincide with the interaction of the dragView and items occupying
    those cells. Instead we use some heuristics to often lock the vector to up, down, left
    or right, which helps make pushing feel right.
    */
    CARAPI_(void) GetDirectionVectorForDrop(
        /* [in] */ Int32 dragViewCenterX,
        /* [in] */ Int32 dragViewCenterY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ ArrayOf<Int32>* resultDirection);

    // For a given cell and span, fetch the set of views intersecting the region.
    CARAPI_(void) GetViewsIntersectingRegion(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ IView* dragView,
        /* [in] */ IRect* boundingRect,
        /* [in] */ IArrayList* intersectingViews);

    CARAPI_(void) ClearOccupiedCells();

    CARAPI_(void) MarkCellsForView(
        /* [in] */ Int32 cellX,
        /* [in] */ Int32 cellY,
        /* [in] */ Int32 spanX,
        /* [in] */ Int32 spanY,
        /* [in] */ ArrayOf<ArrayOf<Boolean>* >* occupied,
        /* [in] */ Boolean value);

public:
    static const String TAG;

    static const Int32 MODE_DRAG_OVER = 0;
    static const Int32 MODE_ON_DROP = 1;
    static const Int32 MODE_ON_DROP_EXTERNAL = 2;
    static const Int32 MODE_ACCEPT_DROP = 3;

    static const Int32 LANDSCAPE = 0;
    static const Int32 PORTRAIT = 1;

    AutoPtr<ArrayOf<Int32> > mTempLocation;

    AutoPtr<ArrayOf<ArrayOf<Boolean>* > > mOccupied;
    AutoPtr<ArrayOf<ArrayOf<Boolean>* > > mTmpOccupied;

    AutoPtr<ArrayOf<Int32> > mPreviousReorderDirection;

private:
    static const Boolean DESTRUCTIVE_REORDER = FALSE;
    static const Boolean DEBUG_VISUALIZE_OCCUPIED = FALSE;
    static const Float REORDER_HINT_MAGNITUDE = 0.12f;
    static const Int32 REORDER_ANIMATION_DURATION = 150;
    static const Int32 INVALID_DIRECTION = -100;

    static const AutoPtr<IPorterDuffXfermode> sAddBlendMode;
    static const AutoPtr<IPaint> sPaint;

    Launcher* mLauncher;
    Int32 mCellWidth;
    Int32 mCellHeight;

    Int32 mCountX;
    Int32 mCountY;

    Int32 mOriginalWidthGap;
    Int32 mOriginalHeightGap;
    Int32 mWidthGap;
    Int32 mHeightGap;
    Int32 mMaxGap;
    Boolean mScrollingTransformsDirty;

    AutoPtr<IRect> mRect;
    AutoPtr<CellInfo> mCellInfo;

    // These are temporary variables to prevent having to allocate a new object just to
    // return an (x, y) value from helper functions. Do NOT use them to maintain other state.
    AutoPtr<ArrayOf<Int32> > mTmpXY;
    AutoPtr<ArrayOf<Int32> > mTmpPoint;

    Boolean mLastDownOnOccupiedCell;

    AutoPtr<IViewOnTouchListener> mInterceptTouchListener;

    AutoPtr<IArrayList> mFolderOuterRings; // List<FolderRingAnimator>
    AutoPtr<ArrayOf<Int32> > mFolderLeaveBehindCell;

    Int32 mForegroundAlpha;
    Float mBackgroundAlpha;
    Float mBackgroundAlphaMultiplier;

    AutoPtr<IDrawable> mNormalBackground;
    AutoPtr<IDrawable> mActiveGlowBackground;
    AutoPtr<IDrawable> mOverScrollForegroundDrawable;
    AutoPtr<IDrawable> mOverScrollLeft;
    AutoPtr<IDrawable> mOverScrollRight;
    AutoPtr<IRect> mBackgroundRect;
    AutoPtr<IRect> mForegroundRect;
    Int32 mForegroundPadding;

    // If we're actively dragging something over this screen, mIsDragOverlapping is TRUE
    Boolean mIsDragOverlapping;
    AutoPtr<IPoint> mDragCenter;

    // These arrays are used to implement the drag visualization on x-large screens.
    // They are used as circular arrays, indexed by mDragOutlineCurrent.
    AutoPtr<ArrayOf<IRect*> > mDragOutlines;
    AutoPtr<ArrayOf<Float> > mDragOutlineAlphas;
    AutoPtr<ArrayOf<InterruptibleInOutAnimator*> > mDragOutlineAnims;

    // Used as an index into the above 3 arrays; indicates which is the most current value.
    Int32 mDragOutlineCurrent;
    AutoPtr<IPaint> mDragOutlinePaint;

    AutoPtr<BubbleTextView> mPressedOrFocusedIcon;

    AutoPtr<IHashMap> mReorderAnimators; // <CellLayout.LayoutParams, Animator>
    AutoPtr<IHashMap> mShakeAnimators; // <View, ReorderHintAnimation>

    Boolean mItemPlacementDirty;

    // When a drag operation is in progress, holds the nearest cell to the touch point
    AutoPtr<ArrayOf<Int32> > mDragCell;

    Boolean mDragging;

    AutoPtr<ITimeInterpolator> mEaseOutInterpolator;
    AutoPtr<ShortcutAndWidgetContainer> mShortcutsAndWidgets;

    Boolean mIsHotseat;
    Float mHotseatScale;

    Float mReorderHintAnimationMagnitude;

    AutoPtr<IArrayList> mIntersectingViews; // item is view
    AutoPtr<IRect> mOccupiedRect;
    AutoPtr<ArrayOf<Int32> > mDirectionVector;
    AutoPtr<DropTarget::DragEnforcer> mDragEnforcer;
    AutoPtr<IRect> mTemp;
    AutoPtr<IStack> mTempRectStack;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CELLLAYOUT_H__
