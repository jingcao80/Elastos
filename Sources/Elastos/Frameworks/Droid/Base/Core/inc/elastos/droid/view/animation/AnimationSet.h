
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_ANIMATIONSET_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_ANIMATIONSET_H__

#include "elastos/droid/view/animation/Animation.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

class AnimationSet
    : public Animation
    , public IAnimationSet
{
public:
    CAR_INTERFACE_DECL()

    AnimationSet();

    ~AnimationSet();

    /**
     * Constructor used when an AnimationSet is loaded from a resource.
     *
     * @param context Application context to use
     * @param attrs Attribute set from which to read values
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Constructor to use when building an AnimationSet from code
     *
     * @param shareInterpolator Pass TRUE if all of the animations in this set
     *        should use the interpolator assocciated with this AnimationSet.
     *        Pass FALSE if each animation should use its own interpolator.
     */
    CARAPI constructor(
        /* [in] */ Boolean shareInterpolator);

    CARAPI SetFillAfter(
        /* [in] */ Boolean fillAfter);

    CARAPI SetFillBefore(
        /* [in] */ Boolean fillBefore);

    CARAPI SetRepeatMode(
        /* [in] */ Int32 repeatMode);

    CARAPI SetStartOffset(
        /* [in] */ Int64 startOffset);

    /**
     * <p>Sets the duration of every child animation.</p>
     *
     * @param durationMillis the duration of the animation, in milliseconds, for
     *        every child in this set
     */
    //@Override
    CARAPI SetDuration(
        /* [in] */ Int64 durationMillis);

    /**
     * Add a child animation to this animation set.
     * The transforms of the child animations are applied in the order
     * that they were added
     * @param a Animation to add.
     */
    CARAPI AddAnimation(
        /* [in] */ IAnimation* a);

    /**
     * Sets the start time of this animation and all child animations
     *
     * @see android.view.animation.Animation#setStartTime(Int64)
     */
    //@Override
    CARAPI SetStartTime(
        /* [in] */ Int64 startTimeMillis);

    CARAPI GetStartTime(
        /* [out] */ Int64* time);

    CARAPI RestrictDuration(
        /* [in] */ Int64 durationMillis);

    /**
     * The duration of an AnimationSet is defined to be the
     * duration of the longest child animation.
     *
     * @see android.view.animation.Animation#getDuration()
     */
    //@Override
    CARAPI GetDuration(
        /* [out] */ Int64* duration);

    /**
     * The duration hint of an animation set is the maximum of the duration
     * hints of all of its component animations.
     *
     * @see android.view.animation.Animation#computeDurationHint
     */
    CARAPI ComputeDurationHint(
        /* [out] */ Int64* hint);

    CARAPI InitializeInvalidateRegion(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    /**
     * The transformation of an animation set is the concatenation of all of its
     * component animations.
     *
     * @see android.view.animation.Animation#getTransformation
     */
    //@Override
    CARAPI GetTransformation(
        /* [in] */ Int64 currentTime,
        /* [in] */ ITransformation* t,
        /* [out] */ Boolean* result);

    /**
     * @see android.view.animation.Animation#scaleCurrentDuration(Float)
     */
    //@Override
    CARAPI ScaleCurrentDuration(
        /* [in] */  Float scale);

    /**
     * @see android.view.animation.Animation#initialize(Int32, Int32, Int32, Int32)
     */
    //@Override
    CARAPI Initialize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 parentWidth,
        /* [in] */ Int32 parentHeight);

    CARAPI Reset();

    CARAPI RestoreChildrenStartOffset();

    /**
     * @return All the child animations in this AnimationSet. Note that
     * this may include other AnimationSets, which are not expanded.
     */
    CARAPI GetAnimations(
        /* [out] */ IList** animations);

    CARAPI WillChangeTransformationMatrix(
        /* [out] */ Boolean* result);

    CARAPI WillChangeBounds(
        /* [out] */ Boolean* result);

    CARAPI HasAlpha(
        /* [out] */ Boolean* has);

protected:
    // @Override
    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [in] */ IAnimationSet* object);

private:
    CARAPI_(void) SetFlag(
        /* [in] */ Int32 mask,
        /* [in] */ Boolean value);

    CARAPI_(void) Init();

private:
    static const Int32 PROPERTY_FILL_AFTER_MASK;
    static const Int32 PROPERTY_FILL_BEFORE_MASK;
    static const Int32 PROPERTY_REPEAT_MODE_MASK;
    static const Int32 PROPERTY_START_OFFSET_MASK;
    static const Int32 PROPERTY_SHARE_INTERPOLATOR_MASK;
    static const Int32 PROPERTY_DURATION_MASK;
    static const Int32 PROPERTY_MORPH_MATRIX_MASK;
    static const Int32 PROPERTY_CHANGE_BOUNDS_MASK;

private:
    Int32 mFlags;
    Boolean mDirty;
    Boolean mHasAlpha;
    AutoPtr<IArrayList> mAnimations;
    AutoPtr<ITransformation> mTempTransformation;
    Int64 mLastEnd;
    AutoPtr< ArrayOf<Int64> > mStoredOffsets;
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ANIMATION_ANIMATIONSET_H__
