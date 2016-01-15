
#include "DrawableHolder.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"

using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorListener;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

static AutoPtr<IDecelerateInterpolator> CreateInterpolator()
{
    AutoPtr<CDecelerateInterpolator> ipt;
    CDecelerateInterpolator::NewByFriend((CDecelerateInterpolator**)&ipt);
    return ipt;
}

const String DrawableHolder::TAG("DrawableHolder");
AutoPtr<IDecelerateInterpolator> DrawableHolder::EASE_OUT_INTERPOLATOR = CreateInterpolator();

DrawableHolder::DrawableHolder()
              : mX(0.0f)
              , mY(0.0f)
              , mScaleX(1.0f)
              , mScaleY(1.0f)
              , mAlpha(1.0f)
{}

DrawableHolder::DrawableHolder(
    /* [in] */ IBitmapDrawable* drawable,
    /* [in] */ Float x,
    /* [in] */ Float y)
              : mX(x)
              , mY(y)
              , mScaleX(1.0f)
              , mScaleY(1.0f)
              , mDrawable(drawable)
              , mAlpha(1.0f)
{
    AutoPtr<IPaint> paint;
    mDrawable->GetPaint((IPaint**)&paint);
    paint->SetAntiAlias(TRUE); // Force AA
    Int32 width, height;
    mDrawable->GetIntrinsicWidth(&width);
    mDrawable->GetIntrinsicHeight(&height);
    mDrawable->SetBounds(0, 0, width, height);
}

ECode DrawableHolder::Init(
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
    mDrawable->GetIntrinsicWidth(&width);
    mDrawable->GetIntrinsicHeight(&height);
    mDrawable->SetBounds(0, 0, width, height);
    return NOERROR;
}
/**
 *
 * Adds an animation that interpolates given property from its current value
 * to the given value.
 *
 * @param duration the duration, in ms.
 * @param delay the delay to start the animation, in ms.
 * @param property the property to animate
 * @param toValue the target value
 * @param replace if TRUE, replace the current animation with this one.
 */
AutoPtr<IObjectAnimator> DrawableHolder::AddAnimTo(
    /* [in] */ Int64 duration,
    /* [in] */ Int64 delay,
    /* [in] */ const String& property,
    /* [in] */ Float toValue,
    /* [in] */ Boolean replace)
{

    if (replace) RemoveAnimationFor(property);

    AutoPtr<IObjectAnimatorHelper> helper;
    AutoPtr<IObjectAnimator> anim;
    AutoPtr<ArrayOf<Float> > arrayValue = ArrayOf<Float>::Alloc(1);
    (*arrayValue)[0] = toValue;
    helper->OfFloat(THIS_PROBE(IInterface), property, arrayValue, (IObjectAnimator**)&anim);
    anim->SetDuration(duration);
    anim->SetStartDelay(delay);
    anim->SetInterpolator(EASE_OUT_INTERPOLATOR);
    AddAnimation(anim, replace);
    //if (DBG) Log.v(TAG, "animationCount = " + mAnimators.size());
    return anim;
}

/**
 * Stops all animations for the given property and removes it from the list.
 *
 * @param property
 */
ECode DrawableHolder::RemoveAnimationFor(
    /* [in] */ const String& property)
{
    List<AutoPtr<IAnimator> > removalList(mAnimators);

    Iterator it = removalList.Begin();
    for (; it != removalList.End(); it++)
    {
        String propertyName;
        IObjectAnimator::Probe((*it))->GetPropertyName(&propertyName);

        if(property.Equals(propertyName))
        {
            (*it)->Cancel();
        }
    }

    return NOERROR;
}

/**
 * Stops all animations and removes them from the list.
 */
ECode DrawableHolder::ClearAnimations()
{
    Iterator it = mAnimators.Begin();
    for (; it != mAnimators.End(); it++) {
        (*it)->Cancel();
    }
    mAnimators.Clear();

    return NOERROR;
}

/**
 * Adds the given animation to the list of animations for this object.
 *
 * @param anim
 * @param overwrite
 * @return
 */
AutoPtr<IDrawableHolder> DrawableHolder::AddAnimation(
    /* [in] */ IObjectAnimator* anim,
    /* [in] */ Boolean overwrite)
{
    if (anim != NULL)
        mAnimators.PushBack(anim);
    mNeedToStart.PushBack(anim);
    return THIS_PROBE(IDrawableHolder);
}

/**
 * Draw this object to the canvas using the properties defined in this class.
 *
 * @param canvas canvas to draw into
 */
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
    canvas->Translate(-0.5f*GetWidth(), -0.5f*GetHeight());
    mDrawable->SetAlpha((Int32) Elastos::Core::Math::Round(mAlpha * 255.0f));
    mDrawable->Draw(canvas);
    canvas->Restore();

    return NOERROR;
}

/**
 * Starts all animations added since the last call to this function.  Used to synchronize
 * animations.
 *
 * @param listener an optional listener to add to the animations. Typically used to know when
 * to invalidate the surface these are being drawn to.
 */
ECode DrawableHolder::StartAnimations(
    /* [in] */ IAnimatorUpdateListener* listener)
{
    for (UInt32 i = 0; i < mNeedToStart.GetSize(); i++) {
        AutoPtr<IObjectAnimator> anim = mNeedToStart[i];
        anim->AddUpdateListener(listener);
        anim->AddListener(THIS_PROBE(IAnimatorListener));
        anim->Start();
    }
    mNeedToStart.Clear();
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

Float DrawableHolder::GetX()
{
    return mX;
}

Float DrawableHolder::GetY()
{
    return mY;
}

Float DrawableHolder::GetScaleX()
{
    return mScaleX;
}

Float DrawableHolder::GetScaleY()
{
    return mScaleY;
}

Float DrawableHolder::GetAlpha()
{
    return mAlpha;
}

AutoPtr<IBitmapDrawable> DrawableHolder::GetDrawable()
{
    return mDrawable;
}

Int32 DrawableHolder::GetWidth()
{
    Int32 width;
    mDrawable->GetIntrinsicWidth(&width);
    return width;
}

Int32 DrawableHolder::GetHeight()
{
    Int32 height;
    mDrawable->GetIntrinsicHeight(&height);
    return height;
}

ECode DrawableHolder::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode DrawableHolder::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mAnimators.Remove(animation);

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

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
