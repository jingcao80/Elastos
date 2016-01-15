
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_GRIDLAYOUTANIMATIONCONTROLLER_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_GRIDLAYOUTANIMATIONCONTROLLER_H__

#include "elastos/droid/view/animation/LayoutAnimationController.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

class GridLayoutAnimationController
    : public LayoutAnimationController
    , public IGridLayoutAnimationController
{
public:
    /**
     * The set of parameters that has to be attached to each view contained in
     * the view group animated by the grid layout animation controller. These
     * parameters are used to compute the start time of each individual view's
     * animation.
     */
    class GridLayoutAnimationParameters
        : public LayoutAnimationController::AnimationParameters
        , public IGridLayoutAnimationParameters
    {
    public:
        CAR_INTERFACE_DECL();

        GridLayoutAnimationParameters();

        ~GridLayoutAnimationParameters();

        CARAPI constructor();

        CARAPI GetColumn(
            /* [out] */ Int32* column);

        CARAPI SetColumn(
            /* [in] */ Int32 column);

        CARAPI GetRow(
            /* [out] */ Int32* row);

        CARAPI SetRow(
            /* [in] */ Int32 row);

        CARAPI GetColumnsCount(
            /* [out] */ Int32* columnCount);

        CARAPI SetColumnsCount(
            /* [in] */ Int32 columnCount);

        CARAPI GetRowsCount(
            /* [out] */ Int32* rowsCount);

        CARAPI SetRowsCount(
            /* [in] */ Int32 rowsCount);
    protected:
        /**
         * The view group's column to which the view belongs.
         */
        Int32 mColumn;

        /**
         * The view group's row to which the view belongs.
         */
        Int32 mRow;

        /**
         * The number of columns in the view's enclosing grid layout.
         */
        Int32 mColumnsCount;

        /**
         * The number of rows in the view's enclosing grid layout.
         */
        Int32 mRowsCount;

        friend class GridLayoutAnimationController;
    };

public:
    CAR_INTERFACE_DECL();

    GridLayoutAnimationController();

    ~GridLayoutAnimationController();

    /**
     * Creates a new grid layout animation controller from external resources.
     *
     * @param context the Context the view  group is running in, through which
     *        it can access the resources
     * @param attrs the attributes of the XML tag that is inflating the
     *        layout animation controller
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Creates a new layout animation controller with a delay of 50%
     * for both rows and columns and the specified animation.
     *
     * @param animation the animation to use on each child of the view group
     */
    CARAPI constructor(
        /* [in] */ IAnimation* animation);

    /**
     * Creates a new layout animation controller with the specified delays
     * and the specified animation.
     *
     * @param animation the animation to use on each child of the view group
     * @param columnDelay the delay by which each column animation must be offset
     * @param rowDelay the delay by which each row animation must be offset
     */
    CARAPI constructor(
        /* [in] */ IAnimation* animation,
        /* [in] */ Float columnDelay,
        /* [in] */ Float rowDelay);

    /**
     * Returns the delay by which the children's animation are offset from one
     * column to the other. The delay is expressed as a fraction of the
     * animation duration.
     *
     * @return a fraction of the animation duration
     *
     * @see #setColumnDelay(Float)
     * @see #getRowDelay()
     * @see #setRowDelay(Float)
     */
    CARAPI GetColumnDelay(
        /* [out] */ Float* columnDelay);

    /**
     * Sets the delay, as a fraction of the animation duration, by which the
     * children's animations are offset from one column to the other.
     *
     * @param columnDelay a fraction of the animation duration
     *
     * @see #getColumnDelay()
     * @see #getRowDelay()
     * @see #setRowDelay(Float)
     */
    CARAPI SetColumnDelay(
        /* [in] */ Float columnDelay);

    /**
     * Returns the delay by which the children's animation are offset from one
     * row to the other. The delay is expressed as a fraction of the
     * animation duration.
     *
     * @return a fraction of the animation duration
     *
     * @see #setRowDelay(Float)
     * @see #getColumnDelay()
     * @see #setColumnDelay(Float)
     */
    CARAPI GetRowDelay(
        /* [out] */ Float* rowDelay);

    /**
     * Sets the delay, as a fraction of the animation duration, by which the
     * children's animations are offset from one row to the other.
     *
     * @param rowDelay a fraction of the animation duration
     *
     * @see #getRowDelay()
     * @see #getColumnDelay()
     * @see #setColumnDelay(Float)
     */
    CARAPI SetRowDelay(
        /* [in] */ Float rowDelay);

    /**
     * Returns the direction of the animation. {@link #IGridLayoutAnimationController::DIRECTION_HORIZONTAL_MASK}
     * and {@link #DIRECTION_VERTICAL_MASK} can be used to retrieve the
     * horizontal and vertical components of the direction.
     *
     * @return the direction of the animation
     *
     * @see #setDirection(Int32)
     * @see #DIRECTION_BOTTOM_TO_TOP
     * @see #DIRECTION_TOP_TO_BOTTOM
     * @see #DIRECTION_LEFT_TO_RIGHT
     * @see #IGridLayoutAnimationController::DIRECTION_RIGHT_TO_LEFT
     * @see #IGridLayoutAnimationController::DIRECTION_HORIZONTAL_MASK
     * @see #DIRECTION_VERTICAL_MASK
     */
    CARAPI GetDirection(
        /* [out] */ Int32* direction);

    /**
     * Sets the direction of the animation. The direction is expressed as an
     * integer containing a horizontal and vertical component. For instance,
     * <code>DIRECTION_BOTTOM_TO_TOP | IGridLayoutAnimationController::DIRECTION_RIGHT_TO_LEFT</code>.
     *
     * @param direction the direction of the animation
     *
     * @see #getDirection()
     * @see #DIRECTION_BOTTOM_TO_TOP
     * @see #DIRECTION_TOP_TO_BOTTOM
     * @see #DIRECTION_LEFT_TO_RIGHT
     * @see #IGridLayoutAnimationController::DIRECTION_RIGHT_TO_LEFT
     * @see #IGridLayoutAnimationController::DIRECTION_HORIZONTAL_MASK
     * @see #DIRECTION_VERTICAL_MASK
     */
    CARAPI SetDirection(
        /* [in] */ Int32 direction);

    /**
     * Returns the direction priority for the animation. The priority can
     * be either {@link #IGridLayoutAnimationController::PRIORITY_NONE}, {@link #IGridLayoutAnimationController::PRIORITY_COLUMN} or
     * {@link #IGridLayoutAnimationController::PRIORITY_ROW}.
     *
     * @return the priority of the animation direction
     *
     * @see #setDirectionPriority(Int32)
     * @see #IGridLayoutAnimationController::PRIORITY_COLUMN
     * @see #IGridLayoutAnimationController::PRIORITY_NONE
     * @see #IGridLayoutAnimationController::PRIORITY_ROW
     */
    CARAPI GetDirectionPriority(
        /* [out] */ Int32* directionPriority);

    /**
     * Specifies the direction priority of the animation. For instance,
     * {@link #IGridLayoutAnimationController::PRIORITY_COLUMN} will give priority to columns: the animation
     * will first play on the column, then on the rows.Z
     *
     * @param directionPriority the direction priority of the animation
     *
     * @see #getDirectionPriority()
     * @see #IGridLayoutAnimationController::PRIORITY_COLUMN
     * @see #IGridLayoutAnimationController::PRIORITY_NONE
     * @see #IGridLayoutAnimationController::PRIORITY_ROW
     */
    CARAPI SetDirectionPriority(
        /* [in] */ Int32 directionPriority);

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI WillOverlap(
        /* [out] */ Boolean* willOverlap);

protected:
    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI_(Int64) GetDelayForView(
        /* [in] */ IView* view);

private:
    CARAPI_(Int32) GetTransformedColumnIndex(
        /* [in] */ GridLayoutAnimationParameters* params);

    CARAPI_(Int32) GetTransformedRowIndex(
        /* [in] */ GridLayoutAnimationParameters* params);

private:
    Float mColumnDelay;
    Float mRowDelay;

    Int32 mDirection;
    Int32 mDirectionPriority;
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_GRIDLAYOUTANIMATIONCONTROLLER_H__
