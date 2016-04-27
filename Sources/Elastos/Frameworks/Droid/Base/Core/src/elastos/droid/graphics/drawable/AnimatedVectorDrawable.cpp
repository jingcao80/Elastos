
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/drawable/AnimatedVectorDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimatedVectorDrawable.h"
#include "elastos/droid/graphics/drawable/CVectorDrawable.h"
#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "elastos/droid/animation/AnimatorInflater.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Animation::AnimatorInflater;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::ICloneable;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

AnimatedVectorDrawable::AnimatedVectorDrawableState::AnimatedVectorDrawableState(
    /* [in] */ AnimatedVectorDrawableState* copy)
    : mChangingConfigurations(0)
{
    if (copy != NULL) {
        mChangingConfigurations = copy->mChangingConfigurations;
        if (copy->mVectorDrawable != NULL) {
            AutoPtr<IDrawableConstantState> state;
            IDrawable::Probe(copy->mVectorDrawable)->GetConstantState((IDrawableConstantState**)&state);
            AutoPtr<IDrawable> dr;
            state->NewDrawable((IDrawable**)&dr);
            mVectorDrawable = IVectorDrawable::Probe(dr);
            dr->Mutate();
            ((VectorDrawable*)mVectorDrawable.Get())->SetAllowCaching(FALSE);
            AutoPtr<IRect> r;
            IDrawable::Probe(copy->mVectorDrawable)->GetBounds((IRect**)&r);
            IDrawable::Probe(mVectorDrawable)->SetBounds(r);
        }
        if (copy->mAnimators.Get() != NULL) {
            Int32 numAnimators = 0;
            IList::Probe(copy->mAnimators)->GetSize(&numAnimators);
            CArrayList::New(numAnimators, (IArrayList**)&mAnimators);
            CArrayMap::New(numAnimators, (IArrayMap**)&mTargetNameMap);
            for (Int32 i = 0; i < numAnimators; ++i) {
                AutoPtr<IInterface> obj;
                IList::Probe(copy->mAnimators)->Get(i, (IInterface**)&obj);
                IAnimator* anim = IAnimator::Probe(obj);
                AutoPtr<IInterface> tmp;
                AutoPtr<IAnimator> animClone;
                (ICloneable::Probe(anim))->Clone((IInterface**)&tmp);
                animClone = IAnimator::Probe(tmp);
                String targetName;
                AutoPtr<IInterface> cs;
                IMap::Probe(copy->mTargetNameMap)->Get(anim, (IInterface**)&cs);
                AutoPtr<ICharSequence> csq = ICharSequence::Probe(cs);
                csq->ToString(&targetName);
                AutoPtr<IInterface> targetObject = ((VectorDrawable*)mVectorDrawable.Get())->GetTargetByName(targetName);
                animClone->SetTarget(targetObject);
                mAnimators->Add(animClone);
                csq = NULL;
                CString::New(targetName, (ICharSequence**)&csq);
                IMap::Probe(mTargetNameMap)->Put(animClone, csq);
            }
        }
    } else {
        CVectorDrawable::New((IVectorDrawable**)&mVectorDrawable);
    }
}

ECode AnimatedVectorDrawable::AnimatedVectorDrawableState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CAnimatedVectorDrawable::New(this, NULL, NULL, drawable);
}

ECode AnimatedVectorDrawable::AnimatedVectorDrawableState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CAnimatedVectorDrawable::New(this, res, NULL, drawable);
}

ECode AnimatedVectorDrawable::AnimatedVectorDrawableState::NewDrawable(
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme,
    /* [out] */ IDrawable** drawable)
{
    return CAnimatedVectorDrawable::New(this, res, theme, drawable);
}

ECode AnimatedVectorDrawable::AnimatedVectorDrawableState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}


const String AnimatedVectorDrawable::LOGTAG = String("AnimatedVectorDrawable")/*.class.getSimpleName()*/;
const String AnimatedVectorDrawable::ANIMATED_VECTOR = String("animated-vector");
const String AnimatedVectorDrawable::TARGET = String("target");
const Boolean AnimatedVectorDrawable::DBG_ANIMATION_VECTOR_DRAWABLE = FALSE;

CAR_INTERFACE_IMPL_2(AnimatedVectorDrawable, Drawable, IAnimatedVectorDrawable, IAnimatable);

AnimatedVectorDrawable::AnimatedVectorDrawable()
    : mMutated(FALSE)
{
}

ECode AnimatedVectorDrawable::constructor()
{
    mAnimatedVectorState = new AnimatedVectorDrawableState(NULL);
    return NOERROR;
}

ECode AnimatedVectorDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res,
    /* [in] */ IResourcesTheme* theme)
{
    mAnimatedVectorState = new AnimatedVectorDrawableState((AnimatedVectorDrawableState*)state);
    Boolean can = FALSE;
    if (theme != NULL && (CanApplyTheme(&can), can)) {
        ApplyTheme(theme);
    }
    return NOERROR;
}

ECode AnimatedVectorDrawable::Mutate()
{
    if (!mMutated) {
        IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->Mutate();
        mMutated = TRUE;
    }
    return NOERROR;
}

ECode AnimatedVectorDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    GetChangingConfigurations(&mAnimatedVectorState->mChangingConfigurations);
    *state = (IDrawableConstantState*)mAnimatedVectorState->Probe(EIID_IDrawableConstantState);
    REFCOUNT_ADD(*state);
    return NOERROR;
}

ECode AnimatedVectorDrawable::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = (Drawable::GetChangingConfigurations(config), *config) | mAnimatedVectorState->mChangingConfigurations;
    return NOERROR;
}

ECode AnimatedVectorDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->Draw(canvas);
    if (IsStarted()) {
        InvalidateSelf();
    }
    return NOERROR;
}

void AnimatedVectorDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetBounds(bounds);
}

Boolean AnimatedVectorDrawable::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    Boolean result = FALSE;
    IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetState(const_cast<ArrayOf<Int32>*>(state), &result);
    return result;
}

Boolean AnimatedVectorDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean result = FALSE;
    IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetLevel(level, &result);
    return result;
}

ECode AnimatedVectorDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->GetAlpha(alpha);
}

ECode AnimatedVectorDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetAlpha(alpha);
}

ECode AnimatedVectorDrawable::SetColorFilter(
    /* [in] */ IColorFilter* colorFilter)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetColorFilter(colorFilter);
}

ECode AnimatedVectorDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetTintList(tint);
}

ECode AnimatedVectorDrawable::SetHotspot(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetHotspot(x, y);
}

ECode AnimatedVectorDrawable::SetHotspotBounds(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetHotspotBounds(left, top, right, bottom);
}

ECode AnimatedVectorDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetTintMode(tintMode);
}

ECode AnimatedVectorDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* changed)
{
    VALIDATE_NOT_NULL(changed);
    IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetVisible(visible, restart, changed);
    return Drawable::SetVisible(visible, restart, changed);
}

ECode AnimatedVectorDrawable::SetLayoutDirection(
    /* [in] */ Int32 layoutDirection)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->SetLayoutDirection(layoutDirection);
}

ECode AnimatedVectorDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    VALIDATE_NOT_NULL(isStateful);
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->IsStateful(isStateful);
}

ECode AnimatedVectorDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->GetOpacity(opacity);
}

ECode AnimatedVectorDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->GetIntrinsicWidth(width);
}

ECode AnimatedVectorDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->GetIntrinsicHeight(height);
}

ECode AnimatedVectorDrawable::GetOutline(
    /* [in] */ /*@NonNull*/ IOutline* outline)
{
    return IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->GetOutline(outline);
}

ECode AnimatedVectorDrawable::Inflate(
    /* [in] */ IResources* res,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/
{
    Int32 eventType = 0;
    parser->GetEventType(&eventType);
    Float pathErrorScale = 1;
    while (eventType != IXmlPullParser::END_DOCUMENT) {
        if (eventType == IXmlPullParser::START_TAG) {
            String tagName;
            parser->GetName(&tagName);
            if (ANIMATED_VECTOR.Equals(tagName)) {
                Int32 size = ArraySize(R::styleable::AnimatedVectorDrawable);
                AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
                layout->Copy(R::styleable::AnimatedVectorDrawable, size);

                AutoPtr<ITypedArray> a;
                FAIL_RETURN(ObtainAttributes(res, theme, attrs, layout, (ITypedArray**)&a));
                Int32 drawableRes = 0;
                a->GetResourceId(R::styleable::AnimatedVectorDrawable_drawable, 0, &drawableRes);

                if (drawableRes != 0) {
                    AutoPtr<IDrawable> dr;
                    res->GetDrawable(drawableRes, theme, (IDrawable**)&dr);
                    AutoPtr<IVectorDrawable> vectorDrawable;
                    dr->Mutate();
                    ((VectorDrawable*)vectorDrawable.Get())->SetAllowCaching(FALSE);
                    vectorDrawable->GetPixelSize(&pathErrorScale);
                    mAnimatedVectorState->mVectorDrawable = vectorDrawable;
                }
                a->Recycle();
            } else if (TARGET.Equals(tagName)) {
                Int32 size = ArraySize(R::styleable::AnimatedVectorDrawableTarget);
                AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
                layout->Copy(R::styleable::AnimatedVectorDrawableTarget, size);

                AutoPtr<ITypedArray> a;
                FAIL_RETURN(ObtainAttributes(res, theme, attrs, layout, (ITypedArray**)&a));
                String target;
                a->GetString(R::styleable::AnimatedVectorDrawableTarget_name, &target);

                Int32 id = 0;
                a->GetResourceId(R::styleable::AnimatedVectorDrawableTarget_animation, 0, &id);
                if (id != 0) {
                    AutoPtr<IAnimator> objectAnimator;
                    AnimatorInflater::LoadAnimator(res, theme, id, pathErrorScale, (IAnimator**)&objectAnimator);
                    SetupAnimatorsForTarget(target, objectAnimator);
                }
                a->Recycle();
            }
        }

        parser->Next(&eventType);
    }
    return NOERROR;
}

ECode AnimatedVectorDrawable::CanApplyTheme(
    /* [out] */ Boolean* can)
{
    Boolean tmp = FALSE;
    *can = (Drawable::CanApplyTheme(&tmp), tmp) || (mAnimatedVectorState != NULL
            && mAnimatedVectorState->mVectorDrawable != NULL
            && (IDrawable::Probe(mAnimatedVectorState->mVectorDrawable)->CanApplyTheme(&tmp), tmp));

    return NOERROR;
}

ECode AnimatedVectorDrawable::ApplyTheme(
    /* [in] */ IResourcesTheme* t)
{
    Drawable::ApplyTheme(t);

    AutoPtr<IVectorDrawable> vectorDrawable = mAnimatedVectorState->mVectorDrawable;
    Boolean tmp = FALSE;
    if (vectorDrawable != NULL && (IDrawable::Probe(vectorDrawable)->CanApplyTheme(&tmp), tmp)) {
        IDrawable::Probe(vectorDrawable)->ApplyTheme(t);
    }
    return NOERROR;
}

void AnimatedVectorDrawable::SetupAnimatorsForTarget(
    /* [in] */ const String& name,
    /* [in] */ IAnimator* animator)
{
    AutoPtr<IInterface> target = ((VectorDrawable*)mAnimatedVectorState->mVectorDrawable.Get())->GetTargetByName(name);
    animator->SetTarget(target);
    if (mAnimatedVectorState->mAnimators == NULL) {
        CArrayList::New((IArrayList**)&mAnimatedVectorState->mAnimators);
        CArrayMap::New((IArrayMap**)&mAnimatedVectorState->mTargetNameMap);
    }
    mAnimatedVectorState->mAnimators->Add(animator);

    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    IMap::Probe(mAnimatedVectorState->mTargetNameMap)->Put(animator, cs);
    if (DBG_ANIMATION_VECTOR_DRAWABLE) {
        String value;
        IObject::Probe(animator)->ToString(&value);
        Logger::V(LOGTAG, String("add animator  for target ") + name + String(" ") + value);
    }
}

ECode AnimatedVectorDrawable::IsRunning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        Boolean isRunning = FALSE;
        if (animator->IsRunning(&isRunning), isRunning) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

Boolean AnimatedVectorDrawable::IsStarted()
{
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        Boolean result = FALSE;
        if (animator->IsStarted(&result), result) {
            return TRUE;
        }
    }
    return FALSE;
}

ECode AnimatedVectorDrawable::Start()
{
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        Boolean result = FALSE;
        if (!(animator->IsStarted(&result), result)) {
            animator->Start();
        }
    }
    return InvalidateSelf();
}

ECode AnimatedVectorDrawable::Stop()
{
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        animator->End();
    }
    return NOERROR;
}

ECode AnimatedVectorDrawable::Reverse()
{
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        Boolean result = FALSE;
        if (animator->CanReverse(&result)) {
            animator->Reverse();
        } else {
            Logger::D(LOGTAG, "AnimatedVectorDrawable can't reverse()");
        }
    }
    return NOERROR;
}

ECode AnimatedVectorDrawable::CanReverse(
    /* [out] */ Boolean* can)
{
    VALIDATE_NOT_NULL(can);
    AutoPtr<IArrayList> animators = mAnimatedVectorState->mAnimators;
    Int32 size = 0;
    IList::Probe(animators)->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        IAnimator* animator = IAnimator::Probe(obj);
        Boolean result = FALSE;
        if (!(animator->CanReverse(&result), result)) {
            *can = FALSE;
            return NOERROR;
        }
    }
    *can = TRUE;
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
