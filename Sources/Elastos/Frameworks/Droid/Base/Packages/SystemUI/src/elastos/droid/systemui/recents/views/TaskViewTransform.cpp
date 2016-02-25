
#include "elastos/droid/systemui/recents/views/ViewAnimation.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CRect;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

TaskViewTransform::TaskViewTransform()
    : mStartDelay(0)
    , mTranslationY(0)
    , mTranslationZ(0)
    , mScale(1.0f)
    , mAlpha(1.0f)
    , mVisible(FALSE)
    , mP(0.0f)
{
    CRect::New((IRect**)&mRect);
    // Do nothing
}

TaskViewTransform::TaskViewTransform(
    /* [in] */ TaskViewTransform* o)
{
    CRect::New((IRect**)&mRect);
    mStartDelay = o->mStartDelay;
    mTranslationY = o->mTranslationY;
    mTranslationZ = o->mTranslationZ;
    mScale = o->mScale;
    mAlpha = o->mAlpha;
    mVisible = o->mVisible;
    mRect->Set(o->mRect);
    mP = o->mP;
}

/** Resets the current transform */
void TaskViewTransform::Reset()
{
    mStartDelay = 0;
    mTranslationY = 0;
    mTranslationZ = 0;
    mScale = 1.0f;
    mAlpha = 1.0f;
    mVisible = FALSE;
    mRect->SetEmpty();
    mP = 0.0f;
}

/** Convenience functions to compare against current property values */
Boolean TaskViewTransform::HasAlphaChangedFrom(
    /* [in] */ Float v)
{
    return (Elastos::Core::Math::Compare(mAlpha, v) != 0);
}

Boolean TaskViewTransform::HasScaleChangedFrom(
    /* [in] */ Float v)
{
    return (Elastos::Core::Math::Compare(mScale, v) != 0);
}

Boolean TaskViewTransform::HasTranslationYChangedFrom(
    /* [in] */ Float v)
{
    return (Elastos::Core::Math::Compare((Float)mTranslationY, v) != 0);
}

Boolean TaskViewTransform::HasTranslationZChangedFrom(
    /* [in] */ Float v)
{
    return (Elastos::Core::Math::Compare(mTranslationZ, v) != 0);
}

/** Applies this transform to a view. */
void TaskViewTransform::ApplyToTaskView(
    /* [in] */ IView* v,
    /* [in] */ Int32 duration,
    /* [in] */ IInterpolator* interp,
    /* [in] */ Boolean allowLayers,
    /* [in] */ Boolean allowShadows,
    /* [in] */ IAnimatorUpdateListener* updateCallback)
{
    // Check to see if any properties have changed, and update the task view
    if (duration > 0) {
        AutoPtr<IViewPropertyAnimator> anim;
        v->Animate((IViewPropertyAnimator**)&anim);
        Boolean requiresLayers = FALSE;

        // Animate to the final state
        Float y;
        v->GetTranslationY(&y);
        if (HasTranslationYChangedFrom(y)) {
            anim->TranslationY(mTranslationY);
        }
        Float z;
        v->GetTranslationZ(&z);
        if (allowShadows && HasTranslationZChangedFrom(z)) {
            anim->TranslationZ(mTranslationZ);
        }
        Float x;
        v->GetScaleX(&x);
        if (HasScaleChangedFrom(x)) {
            anim->ScaleX(mScale);
            anim->ScaleY(mScale);
            requiresLayers = TRUE;
        }
        Float alpha;
        v->GetAlpha(&alpha);
        if (HasAlphaChangedFrom(alpha)) {
            // Use layers if we animate mAlpha
            anim->Alpha(mAlpha);
            requiresLayers = TRUE;
        }
        if (requiresLayers && allowLayers) {
            anim->WithLayer();
        }
        if (updateCallback != NULL) {
            anim->SetUpdateListener(updateCallback);
        }
        else {
            anim->SetUpdateListener(NULL);
        }
        anim->SetStartDelay(mStartDelay);
        anim->SetDuration(duration);
        anim->SetInterpolator(ITimeInterpolator::Probe(interp));
        anim->Start();
    }
    else {
        // Set the changed properties
        Float y;
        v->GetTranslationY(&y);
        if (HasTranslationYChangedFrom(y)) {
            v->SetTranslationY(mTranslationY);
        }
        Float z;
        v->GetTranslationZ(&z);
        if (allowShadows && HasTranslationZChangedFrom(z)) {
            v->SetTranslationZ(mTranslationZ);
        }
        Float x;
        v->GetScaleX(&x);
        if (HasScaleChangedFrom(x)) {
            v->SetScaleX(mScale);
            v->SetScaleY(mScale);
        }
        Float alpha;
        v->GetAlpha(&alpha);
        if (HasAlphaChangedFrom(alpha)) {
            v->SetAlpha(mAlpha);
        }
    }
}

/** Reset the transform on a view. */
void TaskViewTransform::Reset(
    /* [in] */ IView* v)
{
    v->SetTranslationX(0.0f);
    v->SetTranslationY(0.0f);
    v->SetTranslationZ(0.0f);
    v->SetScaleX(1.0f);
    v->SetScaleY(1.0f);
    v->SetAlpha(1.0f);
}

// @Override
ECode TaskViewTransform::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb += "TaskViewTransform delay: ";
    sb += mStartDelay;
    sb += " y: ";
    sb += mTranslationY;
    sb += " z: ";
    sb += mTranslationZ;
    sb += " mScale: ";
    sb += mScale;
    sb += " mAlpha: ";
    sb += mAlpha;
    sb += " mVisible: ";
    sb += mVisible;
    sb += " mRect: ";
    sb += mRect;
    sb += " mP: ";
    sb += mP;
    return sb.ToString(str);
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
