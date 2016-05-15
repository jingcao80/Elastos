
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATESTATELISTDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATESTATELISTDRAWABLE_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/graphics/drawable/StateListDrawable.h"
#include "elastos/droid/graphics/drawable/AnimationDrawable.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

/**
 * Drawable containing a set of Drawable keyframes where the currently displayed
 * keyframe is chosen based on the current state set. Animations between
 * keyframes may optionally be defined using transition elements.
 * <p>
 * This drawable can be defined in an XML file with the <code>
 * &lt;animated-selector></code> element. Each keyframe Drawable is defined in a
 * nested <code>&lt;item></code> element. Transitions are defined in a nested
 * <code>&lt;transition></code> element.
 *
 * @attr ref android.R.styleable#DrawableStates_state_focused
 * @attr ref android.R.styleable#DrawableStates_state_window_focused
 * @attr ref android.R.styleable#DrawableStates_state_enabled
 * @attr ref android.R.styleable#DrawableStates_state_checkable
 * @attr ref android.R.styleable#DrawableStates_state_checked
 * @attr ref android.R.styleable#DrawableStates_state_selected
 * @attr ref android.R.styleable#DrawableStates_state_activated
 * @attr ref android.R.styleable#DrawableStates_state_active
 * @attr ref android.R.styleable#DrawableStates_state_single
 * @attr ref android.R.styleable#DrawableStates_state_first
 * @attr ref android.R.styleable#DrawableStates_state_middle
 * @attr ref android.R.styleable#DrawableStates_state_last
 * @attr ref android.R.styleable#DrawableStates_state_pressed
 */
class AnimatedStateListDrawable
    : public StateListDrawable
    , public IAnimatedStateListDrawable
{
private:
    class Transition
        : public Object
    {
    public:
        virtual CARAPI_(AutoPtr<Transition>) Clone() = 0;

        virtual CARAPI_(void) Start() = 0;

        virtual CARAPI_(void) Stop() = 0;

        virtual CARAPI_(void) Reverse()
        {
            // Not supported by default.
        }

        virtual CARAPI_(Boolean) CanReverse()
        {
            return FALSE;
        }
    };

    class AnimatableTransition
        : public Transition
    {
    public:
        AnimatableTransition(
            /* [in] */ IAnimatable* a)
        {
            mA = a;
        }

        // @Override
        CARAPI_(AutoPtr<Transition>) Clone()
        {
            AutoPtr<Transition> obj = new AnimatableTransition(mA);
            return obj;
        }

        // @Override
        CARAPI_(void) Start()
        {
            mA->Start();
        }

        // @Override
        CARAPI_(void) Stop()
        {
            mA->Stop();
        }

    private:
        AutoPtr<IAnimatable> mA;
    };

    class AnimationDrawableTransition
        : public Transition
    {
    public:
        AnimationDrawableTransition(
            /* [in] */ IAnimationDrawable* ad,
            /* [in] */ Boolean reversed);

        // @Override
        CARAPI_(AutoPtr<Transition>) Clone()
        {
            AutoPtr<Transition> obj = new AnimationDrawableTransition(mAnim);
            return obj;
        }

        // @Override
        CARAPI_(Boolean) CanReverse();

        // @Override
        CARAPI_(void) Start();

        // @Override
        CARAPI_(void) Reverse();

        // @Override
        CARAPI_(void) Stop();

    private:
        AnimationDrawableTransition(
            /* [in] */ IObjectAnimator* animator)
            : mAnim(animator)
        {}

    private:
        AutoPtr<IObjectAnimator> mAnim;
    };

    class AnimatedVectorDrawableTransition
        : public Transition
    {
    public:
        AnimatedVectorDrawableTransition(
            /* [in] */ IAnimatedVectorDrawable* avd,
            /* [in] */ Boolean reversed);

        // @Override
        CARAPI_(AutoPtr<Transition>) Clone()
        {
            AutoPtr<Transition> obj = new AnimatedVectorDrawableTransition(mAvd, mReversed);
            return obj;
        }

        // @Override
        CARAPI_(Boolean) CanReverse();

        // @Override
        CARAPI_(void) Start();

        // @Override
        CARAPI_(void) Reverse();

        // @Override
        CARAPI_(void) Stop();

    private:
        AutoPtr<IAnimatedVectorDrawable> mAvd;
        Boolean mReversed;
    };

protected:
    class AnimatedStateListState
        : public StateListState
    {
    public:
        AnimatedStateListState(
            /* [in] */ /*@Nullable*/ AnimatedStateListState* orig,
            /* [in] */ /*@NonNull*/ AnimatedStateListDrawable* owner,
            /* [in] */ /*@Nullable*/ IResources* res);

        CARAPI_(Int32) AddTransition(
            /* [in] */ Int32 fromId,
            /* [in] */ Int32 toId,
            /* [in] */ /*@NonNull*/ IDrawable* anim,
            /* [in] */ Boolean reversible);

        CARAPI_(Int32) AddStateSet(
            /* [in] */ /*@NonNull*/ ArrayOf<Int32>* stateSet,
            /* [in] */ /*@NonNull*/ IDrawable* drawable,
            /* [in] */ Int32 id);

        CARAPI_(Int32) IndexOfKeyframe(
            /* [in] */ /*@NonNull*/ ArrayOf<Int32>* stateSet);

        CARAPI_(Int32) GetKeyframeIdAt(
            /* [in] */ Int32 index);

        CARAPI_(Int32) IndexOfTransition(
            /* [in] */ Int32 fromId,
            /* [in] */ Int32 toId);

        CARAPI_(Boolean) IsTransitionReversed(
            /* [in] */ Int32 fromId,
            /* [in] */ Int32 toId);

        // @Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

    private:
        static CARAPI_(Int64) GenerateTransitionKey(
            /* [in] */ Int32 fromId,
            /* [in] */ Int32 toId);

    private:
        static const Int32 REVERSE_SHIFT;
        static const Int32 REVERSE_MASK;

        // final LongSparseLongArray mTransitions;
        HashMap<Int64, Int64> mTransitions;
        AutoPtr<ISparseInt32Array> mStateIds;
    };

    /**
     * Interpolates between frames with respect to their individual durations.
     */
    class FrameInterpolator
        : public Object
        , public ITimeInterpolator
    {
    public:
        CAR_INTERFACE_DECL();

        FrameInterpolator(
            /* [in] */ IAnimationDrawable* d,
            /* [in] */ Boolean reversed);

        CARAPI_(Int32) UpdateFrames(
            /* [in] */ IAnimationDrawable* d,
            /* [in] */ Boolean reversed);

        CARAPI_(Int32) GetTotalDuration();

        // @Override
        CARAPI GetInterpolation(
            /* [in] */ Float input,
            /* [out] */ Float* result);

        // @Override
        CARAPI HasNativeInterpolator(
            /* [out] */ Boolean* res);

    private:
        AutoPtr<ArrayOf<Int32> > mFrameTimes;
        Int32 mFrames;
        Int32 mTotalDuration;
    };

public:
    CAR_INTERFACE_DECL();

    AnimatedStateListDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ /*@Nullable*/ IDrawableConstantState* state,
        /* [in] */ /*@Nullable*/ IResources* res);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* result);

    /**
     * Add a new drawable to the set of keyframes.
     *
     * @param stateSet An array of resource IDs to associate with the keyframe
     * @param drawable The drawable to show when in the specified state, may not be null
     * @param id The unique identifier for the keyframe
     */
    virtual CARAPI AddState(
        /* [in] */ /*@NonNull*/ ArrayOf<Int32>* stateSet,
        /* [in] */ /*@NonNull*/ IDrawable* drawable,
        /* [in] */ Int32 id);

    /**
     * Adds a new transition between keyframes.
     *
     * @param fromId Unique identifier of the starting keyframe
     * @param toId Unique identifier of the ending keyframe
     * @param transition An {@link Animatable} drawable to use as a transition, may not be null
     * @param reversible Whether the transition can be reversed
     */
    virtual CARAPI AddTransition(
        /* [in] */ Int32 fromId,
        /* [in] */ Int32 toId,
        /* [in] */ /*@NonNull*/ IDrawable* transition,
        /* [in] */ Boolean reversible);

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* result);

    // @Override
    virtual CARAPI JumpToCurrentState();

    // @Override
    CARAPI Inflate(
        /* [in] */ /*@NonNull*/ IResources* r,
        /* [in] */ /*@NonNull*/ IXmlPullParser* parser,
        /* [in] */ /*@NonNull*/ IAttributeSet* attrs,
        /* [in] */ /*@Nullable*/ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    // @Override
    CARAPI Mutate();

protected:
    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);

    void SetConstantState(
        /* [in] */ /*@NonNull*/ AnimatedStateListState* state);

private:
    CARAPI_(Boolean) SelectTransition(
        /* [in] */ Int32 toIndex);

    CARAPI ParseTransition(
        /* [in] */ /*@NonNull*/ IResources* r,
        /* [in] */ /*@NonNull*/ IXmlPullParser* parser,
        /* [in] */ /*@NonNull*/ IAttributeSet* attrs,
        /* [in] */ /*@Nullable*/ IResourcesTheme* theme,
        /* [out] */ Int32* result) /*throws XmlPullParserException, IOException*/;

    CARAPI ParseItem(
        /* [in] */ /*@NonNull*/ IResources* r,
        /* [in] */ /*@NonNull*/ IXmlPullParser* parser,
        /* [in] */ /*@NonNull*/ IAttributeSet* attrs,
        /* [in] */ /*@Nullable*/ IResourcesTheme* theme,
        /* [out] */ Int32* result) /*throws XmlPullParserException, IOException*/;

private:
    static const String LOGTAG/* = AnimatedStateListDrawable.class.getSimpleName()*/;

    static const String ELEMENT_TRANSITION;
    static const String ELEMENT_ITEM;

    AutoPtr<AnimatedStateListState> mState;

    /** The currently running transition, if any. */
    AutoPtr<Transition> mTransition;

    /** Index to be set after the transition ends. */
    Int32 mTransitionToIndex;

    /** Index away from which we are transitioning. */
    Int32 mTransitionFromIndex;

    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATESTATELISTDRAWABLE_H__
