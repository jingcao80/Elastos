
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/RotateAnimation.h"
#include "elastos/droid/view/animation/CRotateAnimation.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL(RotateAnimation, Animation, IRotateAnimation);

RotateAnimation::RotateAnimation()
    : mFromDegrees(0.f)
    , mToDegrees(0.f)
    , mPivotXType(IAnimation::ABSOLUTE)
    , mPivotYType(IAnimation::ABSOLUTE)
    , mPivotXValue(0.0f)
    , mPivotYValue(0.0f)
    , mPivotX(0.f)
    , mPivotY(0.f)
{}

ECode RotateAnimation::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(Animation::constructor(context, attrs));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::RotateAnimation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    a->GetFloat(R::styleable::RotateAnimation_fromDegrees, 0.0f, &mFromDegrees);

    a->GetFloat(R::styleable::RotateAnimation_toDegrees, 0.0f, &mToDegrees);

    AutoPtr<ITypedValue> value;
    a->PeekValue(R::styleable::RotateAnimation_pivotX, (ITypedValue**)&value);
    AutoPtr<Animation::Description> d = Animation::Description::ParseValue(value);
    mPivotXType = d->mType;
    mPivotXValue = d->mValue;

    value = NULL;
    a->PeekValue(R::styleable::RotateAnimation_pivotY, (ITypedValue**)&value);
    d = Description::ParseValue(value);
    mPivotYType = d->mType;
    mPivotYValue = d->mValue;

    a->Recycle();

    InitializePivotPoint();

    return NOERROR;
}

ECode RotateAnimation::constructor(
    /* [in] */ Float fromDegrees,
    /* [in] */ Float toDegrees)
{
    FAIL_RETURN(Animation::constructor());
    mFromDegrees = fromDegrees;
    mToDegrees = toDegrees;
    mPivotX = 0.0f;
    mPivotY = 0.0f;

    return NOERROR;
}

ECode RotateAnimation::constructor(
    /* [in] */ Float fromDegrees,
    /* [in] */ Float toDegrees,
    /* [in] */ Float pivotX,
    /* [in] */ Float pivotY)
{
    FAIL_RETURN(Animation::constructor());
    mFromDegrees = fromDegrees;
    mToDegrees = toDegrees;

    mPivotXType = IAnimation::ABSOLUTE;
    mPivotYType = IAnimation::ABSOLUTE;
    mPivotXValue = pivotX;
    mPivotYValue = pivotY;

    return NOERROR;
}

ECode RotateAnimation::constructor(
    /* [in] */ Float fromDegrees,
    /* [in] */ Float toDegrees,
    /* [in] */ Int32 pivotXType,
    /* [in] */ Float pivotXValue,
    /* [in] */ Int32 pivotYType,
    /* [in] */ Float pivotYValue)
{
    FAIL_RETURN(Animation::constructor());
    mFromDegrees = fromDegrees;
    mToDegrees = toDegrees;

    mPivotXValue = pivotXValue;
    mPivotXType = pivotXType;
    mPivotYValue = pivotYValue;
    mPivotYType = pivotYType;

    return NOERROR;
}

void RotateAnimation::ApplyTransformation(
    /* [in] */ Float interpolatedTime,
    /* [in] */ ITransformation* t)
{
    Float degrees = mFromDegrees + ((mToDegrees - mFromDegrees) * interpolatedTime);
    Float scale = GetScaleFactor();

    AutoPtr<IMatrix> matrix;
    t->GetMatrix((IMatrix**)&matrix);
    if (mPivotX == 0.0f && mPivotY == 0.0f) {
        matrix->SetRotate(degrees);
    }
    else {
        matrix->SetRotate(degrees, mPivotX * scale, mPivotY * scale);
    }
}

ECode RotateAnimation::Initialize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 parentWidth,
    /* [in] */ Int32 parentHeight)
{
    Animation::Initialize(width, height, parentWidth, parentHeight);
    mPivotX = ResolveSize(mPivotXType, mPivotXValue, width, parentWidth);
    mPivotY = ResolveSize(mPivotYType, mPivotYValue, height, parentHeight);

    return NOERROR;
}

void RotateAnimation::InitializePivotPoint()
{
    if (mPivotXType == IAnimation::ABSOLUTE) {
        mPivotX = mPivotXValue;
    }
    if (mPivotYType == IAnimation::ABSOLUTE) {
        mPivotY = mPivotYValue;
    }
}

ECode RotateAnimation::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IRotateAnimation> obj;
    CRotateAnimation::New(0.0f, 0.0f, (IRotateAnimation**)&obj);
    RotateAnimation::CloneImpl(obj);
    *object = obj;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode RotateAnimation::CloneImpl(
    /* [in] */ IRotateAnimation* object)
{
    Animation::CloneImpl(IAnimation::Probe(object));
    RotateAnimation* obj = (RotateAnimation*)object;
    obj->mFromDegrees = mFromDegrees;
    obj->mToDegrees = mToDegrees;
    obj->mPivotXType = mPivotXType;
    obj->mPivotYType = mPivotYType;
    obj->mPivotXValue = mPivotXValue;
    obj->mPivotYValue = mPivotYValue;
    obj->mPivotX = mPivotX;
    obj->mPivotY = mPivotY;
    return NOERROR;
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
