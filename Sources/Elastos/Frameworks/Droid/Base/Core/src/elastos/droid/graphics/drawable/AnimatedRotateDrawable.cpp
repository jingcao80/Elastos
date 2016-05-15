
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/AnimatedRotateDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimatedRotateDrawable.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/os/SystemClock.h"
// #include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::R;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

AnimatedRotateDrawable::AnimatedRotateState::AnimatedRotateState(
    /* [in] */ AnimatedRotateState* orig,
    /* [in] */ AnimatedRotateDrawable* owner,
    /* [in] */ IResources* res)
    : mChangingConfigurations(0)
    , mPivotXRel(FALSE)
    , mPivotX(0)
    , mPivotYRel(FALSE)
    , mPivotY(0)
    , mFrameDuration(0)
    , mFramesCount(0)
    , mCanConstantState(FALSE)
    , mCheckedConstantState(FALSE)
{
    if (orig != NULL) {
        AutoPtr<IDrawableConstantState> state;
        orig->mDrawable->GetConstantState((IDrawableConstantState**)&state);
        if (res != NULL) {
            state->NewDrawable(res, (IDrawable**)&mDrawable);
        }
        else {
            state->NewDrawable((IDrawable**)&mDrawable);
        }
        mDrawable->SetCallback(IDrawableCallback::Probe(owner));

        Int32 value = 0;
        orig->mDrawable->GetLayoutDirection(&value);
        mDrawable->SetLayoutDirection(value);
        AutoPtr<IRect> bounds;
        orig->mDrawable->GetBounds((IRect**)&bounds);
        mDrawable->SetBounds(bounds);
        orig->mDrawable->GetLevel(&value);
        Boolean res = FALSE;
        mDrawable->SetLevel(value, &res);
        mPivotXRel = orig->mPivotXRel;
        mPivotX = orig->mPivotX;
        mPivotYRel = orig->mPivotYRel;
        mPivotY = orig->mPivotY;
        mFramesCount = orig->mFramesCount;
        mFrameDuration = orig->mFrameDuration;
        mCanConstantState = mCheckedConstantState = TRUE;
    }
}

ECode AnimatedRotateDrawable::AnimatedRotateState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CAnimatedRotateDrawable::New(this, NULL, drawable);
}

ECode AnimatedRotateDrawable::AnimatedRotateState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CAnimatedRotateDrawable::New(this, res, drawable);
}

ECode AnimatedRotateDrawable::AnimatedRotateState::GetChangingConfigurations(
    /* [out] */ Int32* config)
{
    VALIDATE_NOT_NULL(config);
    *config = mChangingConfigurations;
    return NOERROR;
}

Boolean AnimatedRotateDrawable::AnimatedRotateState::CanConstantState()
{
    if (!mCheckedConstantState) {
        AutoPtr<IDrawableConstantState> state;
        mDrawable->GetConstantState((IDrawableConstantState**)&state);
        mCanConstantState = state != NULL;
        mCheckedConstantState = TRUE;
    }

    return mCanConstantState;
}

CAR_INTERFACE_IMPL_4(AnimatedRotateDrawable, Drawable, IAnimatedRotateDrawable, IDrawableCallback, IRunnable, IAnimatable)
AnimatedRotateDrawable::AnimatedRotateDrawable()
    : mMutated(FALSE)
    , mCurrentDegrees(0)
    , mIncrement(0)
    , mRunning(FALSE)
{
}

ECode AnimatedRotateDrawable::constructor()
{
    return constructor(NULL, NULL);
}

ECode AnimatedRotateDrawable::constructor(
    /* [in] */ IDrawableConstantState* rotateState,
    /* [in] */ IResources* res)
{
    mState = new AnimatedRotateState((AnimatedRotateState*)rotateState, this, res);
    Init();

    return NOERROR;
}

void AnimatedRotateDrawable::Init()
{
    AnimatedRotateState* state = mState;
    mIncrement = 360.0f / state->mFramesCount;
    IDrawable* drawable = state->mDrawable;
    if (drawable != NULL) {
        drawable->SetFilterBitmap(TRUE);
        if (IBitmapDrawable::Probe(drawable)) {
            IBitmapDrawable::Probe(drawable)->SetAntiAlias(TRUE);
        }
    }
}

ECode AnimatedRotateDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 saveCount;
    canvas->Save(&saveCount);

    AnimatedRotateState* st = mState;
    AutoPtr<IDrawable> drawable = st->mDrawable;
    AutoPtr<IRect> temp;
    drawable->GetBounds((IRect**)&temp);
    CRect* bounds = (CRect*)temp.Get();

    Int32 w = bounds->mRight - bounds->mLeft;
    Int32 h = bounds->mBottom - bounds->mTop;

    Float px = st->mPivotXRel ? (w * st->mPivotX) : st->mPivotX;
    Float py = st->mPivotYRel ? (h * st->mPivotY) : st->mPivotY;

    canvas->Rotate(mCurrentDegrees, px + bounds->mLeft, py + bounds->mTop);

    drawable->Draw(canvas);

    canvas->RestoreToCount(saveCount);
    return NOERROR;
}

ECode AnimatedRotateDrawable::Start()
{
    if (!mRunning) {
        mRunning = TRUE;
        NextFrame();
    }
    return NOERROR;
}

ECode AnimatedRotateDrawable::Stop()
{
    mRunning = FALSE;
    UnscheduleSelf(this);
    return NOERROR;
}

ECode AnimatedRotateDrawable::IsRunning(
    /* [out] */ Boolean* running)
{
    VALIDATE_NOT_NULL(running);
    *running = mRunning;
    return NOERROR;
}

void AnimatedRotateDrawable::NextFrame()
{
    UnscheduleSelf(this);
    ScheduleSelf(this,
            SystemClock::GetUptimeMillis() + mState->mFrameDuration);
}

ECode AnimatedRotateDrawable::Run()
{
    // TODO: This should be computed in draw(Canvas), based on the amount
    // of time since the last frame drawn
    mCurrentDegrees += mIncrement;
    if (mCurrentDegrees > (360.0f - mIncrement)) {
        mCurrentDegrees = 0.0f;
    }
    InvalidateSelf();
    NextFrame();
    return NOERROR;
}

ECode AnimatedRotateDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    Boolean res;
    mState->mDrawable->SetVisible(visible, restart, &res);
    Boolean changed = FALSE;
    Drawable::SetVisible(visible, restart, &changed);
    if (visible) {
        if (changed || restart) {
            mCurrentDegrees = 0.0f;
            NextFrame();
        }
    }
    else {
        UnscheduleSelf(this);
    }
    *isDifferent = changed;
    return NOERROR;
}

ECode AnimatedRotateDrawable::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mState->mDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode AnimatedRotateDrawable::GetChangingConfigurations(
    /* [out] */ Int32* configuration)
{
    VALIDATE_NOT_NULL(configuration);
    Drawable::GetChangingConfigurations(configuration);
    Int32 drawableConfig = 0;
    *configuration = (*configuration)
        | mState->mChangingConfigurations
        | (mState->mDrawable->GetChangingConfigurations(&drawableConfig), drawableConfig);
    return NOERROR;
}

ECode AnimatedRotateDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return mState->mDrawable->SetAlpha(alpha);
}

ECode AnimatedRotateDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    return mState->mDrawable->GetAlpha(alpha);
}

ECode AnimatedRotateDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return mState->mDrawable->SetColorFilter(cf);
}

ECode AnimatedRotateDrawable::SetTintList(
    /* [in] */ IColorStateList* tint)
{
    return mState->mDrawable->SetTintList(tint);
}

ECode AnimatedRotateDrawable::SetTintMode(
    /* [in] */ PorterDuffMode tintMode)
{
    return mState->mDrawable->SetTintMode(tintMode);
}

ECode AnimatedRotateDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    return mState->mDrawable->GetOpacity(opacity);
}

ECode AnimatedRotateDrawable::InvalidateDrawable(
    /* [in] */ IDrawable* who)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->InvalidateDrawable(this);
    }
    return NOERROR;
}

ECode AnimatedRotateDrawable::ScheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what,
    /* [in] */ Int64 when)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->ScheduleDrawable(this, what, when);
    }
    return NOERROR;
}

ECode AnimatedRotateDrawable::UnscheduleDrawable(
    /* [in] */ IDrawable* who,
    /* [in] */ IRunnable* what)
{
    AutoPtr<IDrawableCallback> callback;
    GetCallback((IDrawableCallback**)&callback);
    if (callback != NULL) {
        callback->UnscheduleDrawable(this, what);
    }
    return NOERROR;
}

ECode AnimatedRotateDrawable::GetPadding(
    /* [in] */ IRect* padding,
    /* [out] */ Boolean* isPadding)
{
    return mState->mDrawable->GetPadding(padding, isPadding);
}

ECode AnimatedRotateDrawable::IsStateful(
    /* [out] */ Boolean* isStateful)
{
    return mState->mDrawable->IsStateful(isStateful);
}

void AnimatedRotateDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    CRect* temp = (CRect*)bounds;
    mState->mDrawable->SetBounds(
        temp->mLeft, temp->mTop, temp->mRight, temp->mBottom);
}

Boolean AnimatedRotateDrawable::OnLevelChange(
    /* [in] */ Int32 level)
{
    Boolean res = FALSE;
    mState->mDrawable->SetLevel(level, &res);
    return res;
}

Boolean AnimatedRotateDrawable::OnStateChange(
    /* [in] */ ArrayOf<Int32>* state)
{
    Boolean isStateful = FALSE;
    mState->mDrawable->SetState(state, &isStateful);
    return isStateful;
}

ECode AnimatedRotateDrawable::GetIntrinsicWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    return mState->mDrawable->GetIntrinsicWidth(width);
}

ECode AnimatedRotateDrawable::GetIntrinsicHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    return mState->mDrawable->GetIntrinsicHeight(height);
}

ECode AnimatedRotateDrawable::GetConstantState(
    /* [out] */ IDrawableConstantState** state)
{
    VALIDATE_NOT_NULL(state);
    if (mState->CanConstantState()) {
        GetChangingConfigurations(&mState->mChangingConfigurations);
        *state = mState;
        REFCOUNT_ADD(*state);
    }
    *state = NULL;
    return NOERROR;
}

ECode AnimatedRotateDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    Int32 size = ArraySize(R::styleable::AnimatedRotateDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::AnimatedRotateDrawable, size);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));

    FAIL_RETURN(Drawable::InflateWithAttributes(
            r, parser, a, R::styleable::AnimatedRotateDrawable_visible));

    AutoPtr<ITypedValue> tv;
    a->PeekValue(R::styleable::AnimatedRotateDrawable_pivotX, (ITypedValue**)&tv);
    Int32 tp;
    tv->GetType(&tp);
    Boolean pivotXRel = tp == ITypedValue::TYPE_FRACTION;
    Float pivotX;
    pivotXRel ? tv->GetFraction(1.0f, 1.0f, &pivotX) : tv->GetFloat(&pivotX);

    tv = NULL;
    a->PeekValue(R::styleable::AnimatedRotateDrawable_pivotY, (ITypedValue**)&tv);
    tv->GetType(&tp);
    Boolean pivotYRel = tp == ITypedValue::TYPE_FRACTION;
    Float pivotY;
    pivotYRel ? tv->GetFraction(1.0f, 1.0f, &pivotY) : tv->GetFloat(&pivotY);

    Int32 framesCount;
    a->GetInt32(R::styleable::AnimatedRotateDrawable_framesCount, 12, &framesCount);
    SetFramesCount(framesCount);

    Int32 frameDuration;
    a->GetInt32(R::styleable::AnimatedRotateDrawable_frameDuration, 150, &frameDuration);
    SetFramesDuration(frameDuration);

    Int32 res;
    a->GetResourceId(R::styleable::AnimatedRotateDrawable_drawable, 0, &res);
    AutoPtr<IDrawable> drawable;
    if (res > 0) {
        r->GetDrawable(res, theme, (IDrawable**)&drawable);
    }

    a->Recycle();

    Int32 outerDepth;
    parser->GetDepth(&outerDepth);
    Int32 type, depth;
    while ((parser->Next(&type), type) != IXmlPullParser::END_DOCUMENT
        && (type != IXmlPullParser::END_TAG
        || (parser->GetDepth(&depth), depth) > outerDepth)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        drawable = NULL;
        Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&drawable);
        if (drawable == NULL) {
            String name;
            parser->GetName(&name);
            Logger::W("Drawable", "Bad element under <animated-rotate>: %s", name.string());
        }
    }

    if (drawable == NULL) {
        Logger::W("drawable", "No drawable specified for <animated-rotate>");
    }

    mState->mDrawable = drawable;
    mState->mPivotXRel = pivotXRel;
    mState->mPivotX = pivotX;
    mState->mPivotYRel = pivotYRel;
    mState->mPivotY = pivotY;

    Init();

    if (drawable != NULL) {
        drawable->SetCallback(this);
    }
    return NOERROR;
}

CARAPI AnimatedRotateDrawable::SetFramesCount(
    /* [in] */ Int32 framesCount)
{
    mState->mFramesCount = framesCount;
    mIncrement = 360.0f / mState->mFramesCount;
    return NOERROR;
}

CARAPI AnimatedRotateDrawable::SetFramesDuration(
    /* [in] */ Int32 framesDuration)
{
    mState->mFrameDuration = framesDuration;
    return NOERROR;
}

ECode AnimatedRotateDrawable::Mutate()
{
    if (!mMutated) {
        mState->mDrawable->Mutate();
        mMutated = TRUE;
    }

    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
