
#include <Elastos.CoreLibrary.External.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/drawable/AnimationDrawable.h"
#include "elastos/droid/graphics/drawable/CAnimationDrawable.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::R;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

static const String LOGTAG("AnimationDrawable");

AnimationDrawable::AnimationState::AnimationState(
    /* [in] */ AnimationState* orig,
    /* [in] */ AnimationDrawable* owner,
    /* [in] */ IResources* res)
    : DrawableContainerState(orig, owner, res)
    , mOneShot(FALSE)
{
    if (orig != NULL) {
        mDurations = orig->mDurations;
        mOneShot = orig->mOneShot;
    }
    else {
        mDurations = ArrayOf<Int32>::Alloc(GetCapacity());
        mOneShot = TRUE;
    }
}

AnimationDrawable::AnimationState::~AnimationState()
{
}

ECode AnimationDrawable::AnimationState::NewDrawable(
    /* [out] */ IDrawable** drawable)
{
    return CAnimationDrawable::New(this, NULL, drawable);
}

ECode AnimationDrawable::AnimationState::NewDrawable(
    /* [in] */ IResources* res,
    /* [out] */ IDrawable** drawable)
{
    return CAnimationDrawable::New(this, res, drawable);
}

void AnimationDrawable::AnimationState::AddFrame(
    /* [in] */ IDrawable* dr,
    /* [in] */ Int32 dur)
{
    // Do not combine the following. The array index must be evaluated before
    // the array is accessed because super.addChild(dr) has a side effect on mDurations.
    Int32 pos = DrawableContainerState::AddChild(dr);
    (*mDurations)[pos] = dur;
}

void AnimationDrawable::AnimationState::GrowArray(
    /* [in] */ Int32 oldSize,
    /* [in] */ Int32 newSize)
{
    DrawableContainerState::GrowArray(oldSize, newSize);
    AutoPtr<ArrayOf<Int32> > newDurations = ArrayOf<Int32>::Alloc(newSize);
    newDurations->Copy(mDurations, 0, oldSize);
    mDurations = newDurations;
}

CAR_INTERFACE_IMPL_3(AnimationDrawable, DrawableContainer, IAnimationDrawable, IRunnable, IAnimatable);
AnimationDrawable::AnimationDrawable()
    : mCurFrame(-1)
    , mRunning(FALSE)
    , mAnimating(FALSE)
    , mMutated(FALSE)
{
}

ECode AnimationDrawable::constructor()
{
    return constructor(NULL, NULL);
}

ECode AnimationDrawable::constructor(
    /* [in] */ IDrawableConstantState* state,
    /* [in] */ IResources* res)
{
    mAnimationState = new AnimationState((AnimationState*)state, this, res);
    SetConstantState(mAnimationState);
    if (state != NULL) {
        SetFrame(0, TRUE, FALSE);
    }
    return NOERROR;
}

ECode AnimationDrawable::SetVisible(
    /* [in] */ Boolean visible,
    /* [in] */ Boolean restart,
    /* [out] */ Boolean* isDifferent)
{
    VALIDATE_NOT_NULL(isDifferent);
    Boolean changed = FALSE;
    DrawableContainer::SetVisible(visible, restart, &changed);
    if (visible) {
        if (restart || changed) {
            Boolean startFromZero = restart || mCurFrame < 0 ||
                    mCurFrame >= mAnimationState->GetChildCount();
            SetFrame(startFromZero ? 0 : mCurFrame, TRUE, mAnimating);
        }
    }
    else {
        UnscheduleSelf(this);
    }
    *isDifferent = changed;
    return NOERROR;
}

ECode AnimationDrawable::Start()
{
    mAnimating = TRUE;
    Boolean running = FALSE;
    IsRunning(&running);
    if (!running) {
        return Run();
    }
    return NOERROR;
}

ECode AnimationDrawable::Stop()
{
    mAnimating = FALSE;
    Boolean running = FALSE;
    IsRunning(&running);
    if (running) {
        return UnscheduleSelf(this);
    }
    return NOERROR;
}

ECode AnimationDrawable::IsRunning(
    /* [out] */ Boolean* running)
{
    VALIDATE_NOT_NULL(running);
    *running = mRunning;
    return NOERROR;
}

ECode AnimationDrawable::Run()
{
    return NextFrame(FALSE);
}

ECode AnimationDrawable::UnscheduleSelf(
    /* [in] */ IRunnable* what)
{
    mCurFrame = -1;
    mRunning = FALSE;
    return DrawableContainer::UnscheduleSelf(what);
}

ECode AnimationDrawable::GetNumberOfFrames(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mAnimationState->GetChildCount();
    return NOERROR;
}

ECode AnimationDrawable::GetFrame(
    /* [in] */ Int32 index,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    AutoPtr<IDrawable> temp = mAnimationState->GetChild(index);
    *drawable = temp;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode AnimationDrawable::GetDuration(
    /* [in] */ Int32 i,
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = (*mAnimationState->mDurations)[i];
    return NOERROR;
}

ECode AnimationDrawable::IsOneShot(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAnimationState->mOneShot;
    return NOERROR;
}

ECode AnimationDrawable::SetOneShot(
    /* [in] */ Boolean oneShot)
{
    mAnimationState->mOneShot = oneShot;
    return NOERROR;
}

ECode AnimationDrawable::AddFrame(
    /* [in] */ IDrawable* frame,
    /* [in] */ Int32 duration)
{
    mAnimationState->AddFrame(frame, duration);
    if (mCurFrame < 0) {
        SetFrame(0, TRUE, FALSE);
    }
    return NOERROR;
}

ECode AnimationDrawable::NextFrame(
    /* [in] */ Boolean unschedule)
{
    Int32 next = mCurFrame + 1;
    const Int32 N = mAnimationState->GetChildCount();
    if (next >= N) {
        next = 0;
    }
    return SetFrame(next, unschedule, !mAnimationState->mOneShot || next < (N - 1));
}

ECode AnimationDrawable::SetFrame(
    /* [in] */ Int32 frame,
    /* [in] */ Boolean unschedule,
    /* [in] */ Boolean animate)
{
    if (frame >= mAnimationState->GetChildCount()) {
        return NOERROR;
    }
    mAnimating = animate;
    mCurFrame = frame;
    Boolean res = FALSE;
    SelectDrawable(frame, &res);
    if (unschedule || animate) {
        FAIL_RETURN(UnscheduleSelf(this));
    }
    if (animate) {
        // Unscheduling may have clobbered this value; restore it to record that we're animating
        mCurFrame = frame;
        mRunning = TRUE;
        FAIL_RETURN(ScheduleSelf(this,
                SystemClock::GetUptimeMillis() + (*mAnimationState->mDurations)[frame]));
    }
    return NOERROR;
}

ECode AnimationDrawable::Inflate(
    /* [in] */ IResources* r,
    /* [in] */ IXmlPullParser* parser,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IResourcesTheme* theme)
{
    Int32 size = ArraySize(R::styleable::AnimationDrawable);
    AutoPtr<ArrayOf<Int32> > layout = ArrayOf<Int32>::Alloc(size);
    layout->Copy(R::styleable::AnimationDrawable, size);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));

    FAIL_RETURN(DrawableContainer::InflateWithAttributes(r, parser, a,
            R::styleable::AnimationDrawable_visible));

    Boolean padding;
    a->GetBoolean(
            R::styleable::AnimationDrawable_variablePadding, FALSE, &padding);
    mAnimationState->SetVariablePadding(padding);

    a->GetBoolean(
            R::styleable::AnimationDrawable_oneshot, FALSE, &mAnimationState->mOneShot);

    a->Recycle();

    Int32 type;

    Int32 innerDepth, depth;
    parser->GetDepth(&depth);
    innerDepth = depth + 1;
    while ((parser->Next(&type), type != IXmlPullParser::END_DOCUMENT) &&
            ((parser->GetDepth(&depth), depth >= innerDepth) || type != IXmlPullParser::END_TAG)) {
        if (type != IXmlPullParser::START_TAG) {
            continue;
        }

        String name;
        if (depth > innerDepth || (parser->GetName(&name), !name.Equals("item"))) {
            continue;
        }

        a = NULL;
        size = ArraySize(R::styleable::AnimationDrawableItem);
        layout = ArrayOf<Int32>::Alloc(size);
        layout->Copy(R::styleable::AnimationDrawableItem, size);

        FAIL_RETURN(ObtainAttributes(r, theme, attrs, layout, (ITypedArray**)&a));
        Int32 duration;
        a->GetInt32(
                R::styleable::AnimationDrawableItem_duration, -1, &duration);
        if (duration < 0) {
//            throw new XmlPullParserException(
//                    parser.getPositionDescription()
//                    + ": <item> tag requires a 'duration' attribute");
            return E_XML_PULL_PARSER_EXCEPTION;
        }
        Int32 drawableRes;
        a->GetResourceId(
                R::styleable::AnimationDrawableItem_drawable, 0, &drawableRes);

        a->Recycle();

        AutoPtr<IDrawable> dr;
        if (drawableRes != 0) {
            r->GetDrawable(drawableRes, theme, (IDrawable**)&dr);
        }
        else {
            while (parser->Next(&type), type == IXmlPullParser::TEXT) {
                // Empty
            }
            if (type != IXmlPullParser::START_TAG) {
                String str;
                parser->GetPositionDescription(&str);
                Logger::E(LOGTAG, "XmlPullParserException: %s: <item> tag requires a 'drawable' attribute or "
                    "child tag defining a drawable", str.string());
                return E_XML_PULL_PARSER_EXCEPTION;
            }
            Drawable::CreateFromXmlInner(r, parser, attrs, theme, (IDrawable**)&dr);
        }

        mAnimationState->AddFrame(dr, duration);
        if (dr != NULL) {
            dr->SetCallback(this);
        }
    }

    return SetFrame(0, TRUE, FALSE);
}

ECode AnimationDrawable::Mutate()
{
    if (!mMutated) {
        mAnimationState->mDurations = mAnimationState->mDurations->Clone();
        mMutated = TRUE;
    }
    return NOERROR;
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
