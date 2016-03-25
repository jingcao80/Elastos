
#include "Elastos.Droid.Animation.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/internal/widget/DrawableHolder.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

static AutoPtr<IDecelerateInterpolator> CreateInterpolator()
{
    AutoPtr<CDecelerateInterpolator> ipt;
    CDecelerateInterpolator::NewByFriend((CDecelerateInterpolator**)&ipt);
    return (IDecelerateInterpolator*)ipt.Get();
}

const String DrawableHolder::TAG("DrawableHolder");

const AutoPtr<IDecelerateInterpolator> DrawableHolder::EASE_OUT_INTERPOLATOR = CreateInterpolator();

CAR_INTERFACE_IMPL_2(DrawableHolder, Object, IDrawableHolder, IAnimatorListener);

DrawableHolder::DrawableHolder()
    : mX(0.0f)
    , mY(0.0f)
    , mScaleX(1.0f)
    , mScaleY(1.0f)
    , mAlpha(1.0f)
{}

DrawableHolder::~DrawableHolder()
{}

ECode DrawableHolder::constructor(
    /* [in] */ IBitmapDrawable* drawable)
{
    return constructor(drawable, 0.0f, 0.0f);
}

ECode DrawableHolder::constructor(
    /* [in] */ IBitmapDrawable* drawable,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mX = x;
    mY = y;
    mDrawable = drawable;
    AutoPtr<IPaint> paint;
    mDrawable->GetPaint((IPaint**)&paint);
    paint->SetAntiAlias(TRUE); // Force AA
    Int32 width, height;
    AutoPtr<IDrawable> _draw = IDrawable::Probe(mDrawable);
    _draw->GetIntrinsicWidth(&width);
    _draw->GetIntrinsicHeight(&height);
    _draw->SetBounds(0, 0, width, height);
    return NOERROR;
}

ECode DrawableHolder::AddAnimTo(
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay,
    /* [in] */ const String& property,
    /* [in] */ Float toValue,
    /* [in] */ Boolean replace,
    /* [out] */ IObjectAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);

    if (replace) {
        RemoveAnimationFor(property);
    }

    AutoPtr<ArrayOf<Float> > arrayValue = ArrayOf<Float>::Alloc(1);
    (*arrayValue)[0] = toValue;
    AutoPtr<IObjectAnimator> anim = ObjectAnimator::OfFloat(TO_IINTERFACE(this), property, arrayValue);
    IAnimator::Probe(anim)->SetDuration(duration);
    IAnimator::Probe(anim)->SetStartDelay(delay);
    IAnimator::Probe(anim)->SetInterpolator(ITimeInterpolator::Probe(EASE_OUT_INTERPOLATOR));
    AddAnimation(anim, replace);
    if (DBG) {
        Int32 size;
        mAnimators->GetSize(&size);
        Logger::V(TAG, "animationCount = %d ", size);
    }

    *animator = anim;
    REFCOUNT_ADD(*animator);

    return NOERROR;
}

ECode DrawableHolder::RemoveAnimationFor(
    /* [in] */ const String& property)
{
    AutoPtr<IInterface> obj;
    ICloneable::Probe(mAnimators)->Clone((IInterface**)&obj);
    AutoPtr<IArrayList> removalList = IArrayList::Probe(obj);

    Int32 size;
    removalList->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> object;
        removalList->Get(i, (IInterface**)&object);
        AutoPtr<IObjectAnimator> currentAnim = IObjectAnimator::Probe(object);

        String propertyName;
        currentAnim->GetPropertyName(&propertyName);

        if(property.Equals(propertyName)) {
            IAnimator::Probe(currentAnim)->Cancel();
        }
    }

    return NOERROR;
}

ECode DrawableHolder::ClearAnimations()
{
    Int32 size;
    mAnimators->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> object;
        mAnimators->Get(i, (IInterface**)&object);
        AutoPtr<IObjectAnimator> currentAnim = IObjectAnimator::Probe(object);

        IAnimator::Probe(currentAnim)->Cancel();
    }
    mAnimators->Clear();

    return NOERROR;
}

AutoPtr<IDrawableHolder> DrawableHolder::AddAnimation(
    /* [in] */ IObjectAnimator* anim,
    /* [in] */ Boolean overwrite)
{
    if (anim != NULL) {
        mAnimators->Add(anim);
    }
    mNeedToStart->Add(anim);
    return this;
}

ECode DrawableHolder::Draw(
    /* [in] */ ICanvas* canvas)
{
    Float threshold = 1.0f / 256.0f; // contribution less than 1 LSB of RGB byte
    if (mAlpha <= threshold) // don't bother if it won't show up
        return NOERROR;
    Int32 result;
    canvas->Save(ICanvas::MATRIX_SAVE_FLAG, &result);
    canvas->Translate(mX, mY);
    canvas->Scale(mScaleX, mScaleY);
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    canvas->Translate(-0.5f * width, -0.5f * height);
    IDrawable::Probe(mDrawable)->SetAlpha((Int32) Elastos::Core::Math::Round(mAlpha * 255.0f));
    IDrawable::Probe(mDrawable)->Draw(canvas);
    canvas->Restore();

    return NOERROR;
}

ECode DrawableHolder::StartAnimations(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    Int32 size;
    mNeedToStart->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> object;
        mNeedToStart->Get(i, (IInterface**)&object);
        AutoPtr<IObjectAnimator> anim = IObjectAnimator::Probe(object);
        IValueAnimator::Probe(anim)->AddUpdateListener(listener);
        IAnimator::Probe(anim)->AddListener(this);
        IAnimator::Probe(anim)->Start();
    }
    mNeedToStart->Clear();
    return NOERROR;
}

ECode DrawableHolder::SetX(
    /* [in] */ Float value)
{
    mX = value;
    return NOERROR;
}

ECode DrawableHolder::SetY(
    /* [in] */ Float value)
{
    mY = value;
    return NOERROR;
}

ECode DrawableHolder::SetScaleX(
    /* [in] */ Float value)
{
    mScaleX = value;
    return NOERROR;
}

ECode DrawableHolder::SetScaleY(
    /* [in] */ Float value)
{
    mScaleY = value;
    return NOERROR;
}

ECode DrawableHolder::SetAlpha(
    /* [in] */ Float alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode DrawableHolder::GetX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode DrawableHolder::GetY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

ECode DrawableHolder::GetScaleX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);
    *x= mScaleX;
    return NOERROR;
}

ECode DrawableHolder::GetScaleY(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mScaleY;
    return NOERROR;
}

ECode DrawableHolder::GetAlpha(
    /* [out] */ Float* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode DrawableHolder::GetDrawable(
    /* [out] */ IBitmapDrawable** d)
{
    VALIDATE_NOT_NULL(d);
    *d = mDrawable;
    REFCOUNT_ADD(*d);
    return NOERROR;
}

ECode DrawableHolder::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    IDrawable::Probe(mDrawable)->GetIntrinsicWidth(width);
    return NOERROR;
}

ECode DrawableHolder::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    IDrawable::Probe(mDrawable)->GetIntrinsicHeight(height);
    return NOERROR;
}

ECode DrawableHolder::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode DrawableHolder::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mAnimators->Remove(animation);

    return NOERROR;
}

ECode DrawableHolder::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;

}

ECode DrawableHolder::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos
