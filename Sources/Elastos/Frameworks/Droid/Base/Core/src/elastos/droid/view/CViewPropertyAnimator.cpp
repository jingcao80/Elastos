#include "elastos/droid/view/CViewPropertyAnimator.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_OBJECT_IMPL(CViewPropertyAnimator)

CViewPropertyAnimator::CViewPropertyAnimator()
{
}

ECode CViewPropertyAnimator::constructor(
    /* [in] */ IView* v)
{
    return ViewPropertyAnimator::Init(v);
}

ECode CViewPropertyAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    return ViewPropertyAnimator::SetDuration(duration);
}

ECode CViewPropertyAnimator::GetDuration(
    /* [out] */ Int64* duration)
{
    return ViewPropertyAnimator::GetDuration(duration);
}

ECode CViewPropertyAnimator::SetStartDelay(
    /* [in] */ Int64 delay)
{
    return ViewPropertyAnimator::SetStartDelay(delay);
}

ECode CViewPropertyAnimator::GetStartDelay(
    /* [out] */ Int64* delay)
{
    return ViewPropertyAnimator::GetStartDelay(delay);
}

ECode CViewPropertyAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    return ViewPropertyAnimator::SetInterpolator(interpolator);
}

ECode CViewPropertyAnimator::SetListener(
    /* [in] */ IAnimatorListener* listener)
{
    return ViewPropertyAnimator::SetListener(listener);
}

ECode CViewPropertyAnimator::Start()
{
    return ViewPropertyAnimator::Start();
}

ECode CViewPropertyAnimator::Cancel()
{
    return ViewPropertyAnimator::Cancel();
}

ECode CViewPropertyAnimator::X(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::X(value);
}

ECode CViewPropertyAnimator::XBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::XBy(value);
}

ECode CViewPropertyAnimator::Y(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::Y(value);
}

ECode CViewPropertyAnimator::YBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::YBy(value);
}

ECode CViewPropertyAnimator::Rotation(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::Rotation(value);
}

ECode CViewPropertyAnimator::RotationBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::RotationBy(value);
}

ECode CViewPropertyAnimator::RotationX(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::RotationX(value);
}

ECode CViewPropertyAnimator::RotationXBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::RotationXBy(value);
}

ECode CViewPropertyAnimator::RotationY(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::RotationY(value);
}

ECode CViewPropertyAnimator::RotationYBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::RotationYBy(value);
}

ECode CViewPropertyAnimator::TranslationX(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::TranslationX(value);
}

ECode CViewPropertyAnimator::TranslationXBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::TranslationXBy(value);
}

ECode CViewPropertyAnimator::TranslationY(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::TranslationY(value);
}

ECode CViewPropertyAnimator::TranslationYBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::TranslationYBy(value);
}

ECode CViewPropertyAnimator::ScaleX(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::ScaleX(value);
}

ECode CViewPropertyAnimator::ScaleXBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::ScaleXBy(value);
}

ECode CViewPropertyAnimator::ScaleY(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::ScaleY(value);
}

ECode CViewPropertyAnimator::ScaleYBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::ScaleYBy(value);
}

ECode CViewPropertyAnimator::Alpha(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::Alpha(value);
}

ECode CViewPropertyAnimator::AlphaBy(
    /* [in] */ Float value)
{
    return ViewPropertyAnimator::AlphaBy(value);
}

ECode CViewPropertyAnimator::WithLayer()
{
    return ViewPropertyAnimator::WithLayer();
}

ECode CViewPropertyAnimator::WithStartAction(
    /* [in] */ IRunnable* runnable)
{
    return ViewPropertyAnimator::WithStartAction(runnable);
}

ECode CViewPropertyAnimator::WithEndAction(
    /* [in] */ IRunnable* runnable)
{
    return ViewPropertyAnimator::WithEndAction(runnable);
}

}// namespace View
}// namespace Droid
}// namespace Elastos
