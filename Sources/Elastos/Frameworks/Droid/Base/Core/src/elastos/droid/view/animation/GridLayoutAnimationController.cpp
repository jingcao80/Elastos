
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/GridLayoutAnimationController.h"
#include "elastos/droid/view/animation/Animation.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::R;
using Elastos::Utility::CRandom;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

/* GridLayoutAnimationController::GridLayoutAnimationParameters */
CAR_INTERFACE_IMPL(GridLayoutAnimationController::GridLayoutAnimationParameters, LayoutAnimationController::AnimationParameters, IGridLayoutAnimationParameters);

GridLayoutAnimationController::GridLayoutAnimationParameters::GridLayoutAnimationParameters()
    : mColumn(0)
    , mRow(0)
    , mColumnsCount(0)
    , mRowsCount(0)
{}

GridLayoutAnimationController::GridLayoutAnimationParameters::~GridLayoutAnimationParameters()
{}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::constructor()
{
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::GetColumn(
    /* [out] */ Int32* column)
{
    VALIDATE_NOT_NULL(column);
    *column = mColumn;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::SetColumn(
    /* [in] */ Int32 column)
{
    mColumn = column;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::GetRow(
    /* [out] */ Int32* row)
{
    VALIDATE_NOT_NULL(row);
    *row = mRow;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::SetRow(
    /* [in] */ Int32 row)
{
    mRow = row;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::GetColumnsCount(
    /* [out] */ Int32* columnsCount)
{
    VALIDATE_NOT_NULL(columnsCount);
    *columnsCount = mColumnsCount;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::SetColumnsCount(
    /* [in] */ Int32 columnsCount)
{
    mColumnsCount = columnsCount;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::GetRowsCount(
    /* [out] */ Int32* rowsCount)
{
    VALIDATE_NOT_NULL(rowsCount);
    *rowsCount = mRowsCount;
    return NOERROR;
}

ECode GridLayoutAnimationController::GridLayoutAnimationParameters::SetRowsCount(
    /* [in] */ Int32 rowsCount)
{
    mRowsCount = rowsCount;
    return NOERROR;
}

/* GridLayoutAnimationController */
CAR_INTERFACE_IMPL(GridLayoutAnimationController, LayoutAnimationController, IGridLayoutAnimationController);

GridLayoutAnimationController::GridLayoutAnimationController()
    : mColumnDelay(0.0f)
    , mRowDelay(0.0f)
    , mDirection(0)
    , mDirectionPriority(0)
{}

GridLayoutAnimationController::~GridLayoutAnimationController()
{}

ECode GridLayoutAnimationController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(LayoutAnimationController::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::GridLayoutAnimation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    AutoPtr<ITypedValue> value;
    a->PeekValue(R::styleable::GridLayoutAnimation_columnDelay, (ITypedValue**)&value);

    AutoPtr<Animation::Description> d = Animation::Description::ParseValue(value);
    mColumnDelay = d->mValue;

    value = NULL;
    a->PeekValue(R::styleable::GridLayoutAnimation_rowDelay, (ITypedValue**)&value);

    d = Animation::Description::ParseValue(value);
    mRowDelay = d->mValue;

    //noinspection PointlessBitwiseExpression
    a->GetInt32(R::styleable::GridLayoutAnimation_direction,
            IGridLayoutAnimationController::DIRECTION_LEFT_TO_RIGHT |
            IGridLayoutAnimationController::DIRECTION_TOP_TO_BOTTOM, &mDirection);

    a->GetInt32(R::styleable::GridLayoutAnimation_directionPriority,
            IGridLayoutAnimationController::PRIORITY_NONE, &mDirection);

    a->Recycle();

    return NOERROR;
}

ECode GridLayoutAnimationController::constructor(
    /* [in] */ IAnimation* animation)
{
    return constructor(animation, 0.5f, 0.5f);
}

ECode GridLayoutAnimationController::constructor(
    /* [in] */ IAnimation* animation,
    /* [in] */ Float columnDelay,
    /* [in] */ Float rowDelay)
{
    LayoutAnimationController::constructor(animation);
    mColumnDelay = columnDelay;
    mRowDelay = rowDelay;

    return NOERROR;
}

ECode GridLayoutAnimationController::GetColumnDelay(
    /* [out] */ Float* columnDelay)
{
    VALIDATE_NOT_NULL(columnDelay);
    *columnDelay = mColumnDelay;
    return NOERROR;
}

ECode GridLayoutAnimationController::SetColumnDelay(
    /* [in] */ Float columnDelay)
{
    mColumnDelay = columnDelay;
    return NOERROR;
}

ECode GridLayoutAnimationController::GetRowDelay(
    /* [out] */ Float* rowDelay)
{
    VALIDATE_NOT_NULL(rowDelay);
    *rowDelay = mRowDelay;
    return NOERROR;
}

ECode GridLayoutAnimationController::SetRowDelay(
    /* [in] */ Float rowDelay)
{
    mRowDelay = rowDelay;
    return NOERROR;
}

ECode GridLayoutAnimationController::GetDirection(
    /* [out] */ Int32* direction)
{
    VALIDATE_NOT_NULL(direction)
    *direction = mDirection;
    return NOERROR;
}

ECode GridLayoutAnimationController::SetDirection(
    /* [in] */ Int32 direction)
{
    mDirection = direction;
    return NOERROR;
}

ECode GridLayoutAnimationController::GetDirectionPriority(
    /* [out] */ Int32* directionPriority)
{
    VALIDATE_NOT_NULL(directionPriority);
    *directionPriority = mDirectionPriority;
    return NOERROR;
}

ECode GridLayoutAnimationController::SetDirectionPriority(
    /* [in] */ Int32 directionPriority)
{
    mDirectionPriority = directionPriority;
    return NOERROR;
}

ECode GridLayoutAnimationController::WillOverlap(
    /* [out] */ Boolean* willOverlap)
{
    VALIDATE_NOT_NULL(willOverlap);
    *willOverlap = mColumnDelay < 1.0f || mRowDelay < 1.0f;
    return NOERROR;
}

Int64 GridLayoutAnimationController::GetDelayForView(
    /* [in] */ IView* view)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<IAnimationParameters> basic;
    lp->GetLayoutAnimationParameters((IAnimationParameters**)&basic);
    AutoPtr<GridLayoutAnimationParameters> params =
        (GridLayoutAnimationParameters*)IGridLayoutAnimationParameters::Probe(basic);
    if (params == NULL) {
        return 0;
    }

    Int32 column = GetTransformedColumnIndex(params);
    Int32 row = GetTransformedRowIndex(params);

    Int32 rowsCount = params->mRowsCount;
    Int32 columnsCount = params->mColumnsCount;

    Int64 duration;
    mAnimation->GetDuration(&duration);
    Float columnDelay = mColumnDelay * duration;
    Float rowDelay = mRowDelay * duration;

    Float totalDelay;
    Int64 viewDelay;

    if (mInterpolator == NULL) {
        ASSERT_SUCCEEDED(CLinearInterpolator::New((ILinearInterpolator**)&mInterpolator));
    }

    switch (mDirectionPriority) {
        case IGridLayoutAnimationController::PRIORITY_COLUMN:
            viewDelay = (Int64)(row * rowDelay + column * rowsCount * rowDelay);
            totalDelay = rowsCount * rowDelay + columnsCount * rowsCount * rowDelay;
            break;
        case IGridLayoutAnimationController::PRIORITY_ROW:
            viewDelay = (Int64)(column * columnDelay + row * columnsCount * columnDelay);
            totalDelay = columnsCount * columnDelay + rowsCount * columnsCount * columnDelay;
            break;
        case IGridLayoutAnimationController::PRIORITY_NONE:
        default:
            viewDelay = (Int64)(column * columnDelay + row * rowDelay);
            totalDelay = columnsCount * columnDelay + rowsCount * rowDelay;
            break;
    }

    Float normalizedDelay = viewDelay / totalDelay;
    ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(normalizedDelay, &normalizedDelay);

    return (Int64)(normalizedDelay * totalDelay);
}

Int32 GridLayoutAnimationController::GetTransformedColumnIndex(
    /* [in] */ GridLayoutAnimationParameters* params)
{
    Int32 order;
    GetOrder(&order);
    Int32 index;;
    switch (order) {
        case ILayoutAnimationController::ORDER_REVERSE:
            index = params->mColumnsCount - 1 - params->mColumn;
            break;
        case ILayoutAnimationController::ORDER_RANDOM:
            if (mRandomizer == NULL) {
                CRandom::New((IRandom**)&mRandomizer);
            }
            Float value;
            mRandomizer->NextFloat(&value);
            index = (Int32) (params->mColumnsCount * value);
            break;
        case ILayoutAnimationController::ORDER_NORMAL:
        default:
            index = params->mColumn;
            break;
    }

    Int32 direction =
        mDirection & IGridLayoutAnimationController::DIRECTION_HORIZONTAL_MASK;
    if (direction == IGridLayoutAnimationController::DIRECTION_RIGHT_TO_LEFT) {
        index = params->mColumnsCount - 1 - index;
    }

    return index;
}

Int32 GridLayoutAnimationController::GetTransformedRowIndex(
    /* [in] */ GridLayoutAnimationParameters* params)
{
    Int32 order;
    GetOrder(&order);
    Int32 index;
    switch(order) {
        case ILayoutAnimationController::ORDER_REVERSE:
            index = params->mRowsCount - 1 - params->mRow;
            break;
        case ILayoutAnimationController::ORDER_RANDOM:
            if (mRandomizer == NULL) {
                CRandom::New((IRandom**)&mRandomizer);
            }
            Float value;
            mRandomizer->NextFloat(&value);
            index = (Int32) (params->mRowsCount * value);
            break;
        case ILayoutAnimationController::ORDER_NORMAL:
        default:
            index = params->mRow;
            break;
    }

    Int32 direction = mDirection & IGridLayoutAnimationController::DIRECTION_VERTICAL_MASK;
    if (direction == IGridLayoutAnimationController::DIRECTION_BOTTOM_TO_TOP) {
        index = params->mRowsCount - 1 - index;
    }

    return index;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
