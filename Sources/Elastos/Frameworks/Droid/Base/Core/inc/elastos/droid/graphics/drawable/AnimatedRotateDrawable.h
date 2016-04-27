
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEDROTATEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEDROTATEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class AnimatedRotateDrawable
    : public Drawable
    , public IAnimatedRotateDrawable
    , public IDrawableCallback
    , public IRunnable
    , public IAnimatable
{
public:
    class AnimatedRotateState
        : public Drawable::ConstantState
    {
    public:
        AnimatedRotateState(
            /* [in] */ AnimatedRotateState* source,
            /* [in] */ AnimatedRotateDrawable* owner,
            /* [in] */ IResources* res);

        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

        CARAPI_(Boolean) CanConstantState();

    public:
        AutoPtr<IDrawable> mDrawable;

        Int32 mChangingConfigurations;

        Boolean mPivotXRel;
        Float mPivotX;
        Boolean mPivotYRel;
        Float mPivotY;
        Int32 mFrameDuration;
        Int32 mFramesCount;

    private:
        Boolean mCanConstantState;
        Boolean mCheckedConstantState;
    };

public:
    CAR_INTERFACE_DECL();

    AnimatedRotateDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI Start();

    CARAPI Stop();

    CARAPI IsRunning(
        /* [out] */ Boolean* running);

    CARAPI Run();

    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    virtual CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* who);

    CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    CARAPI SetFramesCount(
        /* [in] */ Int32 framesCount);

    CARAPI SetFramesDuration(
        /* [in] */ Int32 framesDuration);

    CARAPI Mutate();

protected:
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    // @Override
    virtual CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    // @Override
    virtual CARAPI_(Boolean) OnStateChange(
        /* [in] */ const ArrayOf<Int32>* state);

private:
    CARAPI_(void) Init();

    CARAPI_(void) NextFrame();

private:
    AutoPtr<AnimatedRotateState> mState;
    Boolean mMutated;
    Float mCurrentDegrees;
    Float mIncrement;
    Boolean mRunning;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_ANIMATEDROTATEDRAWABLE_H__

