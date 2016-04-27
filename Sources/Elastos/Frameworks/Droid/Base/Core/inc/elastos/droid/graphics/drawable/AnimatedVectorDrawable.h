
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEEDVECTORDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEEDVECTORDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class AnimatedVectorDrawable
    : public Drawable
    , public IAnimatedVectorDrawable
    , public IAnimatable
{
protected:
    class AnimatedVectorDrawableState
        : public ConstantState
    {
    public:
        AnimatedVectorDrawableState(
            /* [in] */ AnimatedVectorDrawableState* copy);

        // @Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

    private:
        Int32 mChangingConfigurations;
        AutoPtr<IVectorDrawable> mVectorDrawable;
        AutoPtr<IArrayList>/*<Animator>*/ mAnimators;
        AutoPtr<IArrayMap>/*<Animator, String>*/ mTargetNameMap;

        friend class AnimatedVectorDrawable;
    };

public:
    CAR_INTERFACE_DECL();

    AnimatedVectorDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI Mutate();

    // @Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    // @Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* config);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* colorFilter);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetHotspot(
        /* [in] */ Float x,
        /* [in] */ Float y);

    // @Override
    CARAPI SetHotspotBounds(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* changed);

    /** {@hide} */
    // @Override
    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    // @Override
    CARAPI Inflate(
        /* [in] */ IResources* res,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* t);

    // @Override
    CARAPI IsRunning(
        /* [out] */ Boolean* isRunning);

    // @Override
    CARAPI Start();

    // @Override
    CARAPI Stop();

    /**
     * Reverses ongoing animations or starts pending animations in reverse.
     * <p>
     * NOTE: Only works of all animations are ValueAnimators.
     * @hide
     */
    CARAPI Reverse();

    /**
     * @hide
     */
    CARAPI CanReverse(
        /* [out] */ Boolean* can);

protected:
    // @Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>* state);

    // @Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

private:
    CARAPI_(void) SetupAnimatorsForTarget(
        /* [in] */ const String& name,
        /* [in] */ IAnimator* animator);

    CARAPI_(Boolean) IsStarted();

private:
    static const String LOGTAG;
    static const String ANIMATED_VECTOR;
    static const String TARGET;
    static const Boolean DBG_ANIMATION_VECTOR_DRAWABLE;

    AutoPtr<AnimatedVectorDrawableState> mAnimatedVectorState;

    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEEDVECTORDRAWABLE_H__
