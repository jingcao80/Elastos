
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLECONTAINER_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLECONTAINER_H__

#include "elastos/droid/graphics/drawable/Drawable.h"
#include <elastos/core/Runnable.h>
#include <Elastos.Droid.Utility.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class DrawableContainer
    : public Drawable
    , public IDrawableContainer
    , public IDrawableCallback
{
public:
    /**
     * A ConstantState that can contain several {@link Drawable}s.
     *
     * This class was made public to enable testing, and its visibility may change in a future
     * release.
     */
    class DrawableContainerState
        : public Drawable::ConstantState
    {
    public:
        DrawableContainerState(
            /* [in] */ DrawableContainerState* orig,
            /* [in] */ DrawableContainer* owner,
            /* [in] */ IResources* res);

        virtual ~DrawableContainerState();

        // @Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* configurations);

        CARAPI_(Int32) AddChild(
            /* [in] */ IDrawable* dr);

        CARAPI_(Int32) GetCapacity();

        CARAPI_(void) CreateAllFutures();

        CARAPI_(Int32) GetChildCount();

        CARAPI_(AutoPtr< ArrayOf<IDrawable*> >) GetChildren();

        CARAPI_(AutoPtr<IDrawable>) GetChild(
            /* [in] */ Int32 index);

        CARAPI SetLayoutDirection(
            /* [in] */ Int32 layoutDirection);

        CARAPI ApplyTheme(
            /* [in] */ IResourcesTheme* theme);

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        CARAPI Mutate();

        /** A Boolean value indicating whether to use the maximum padding value of
         * all frames in the set (FALSE), or to use the padding value of the frame
         * being shown (TRUE). Default value is FALSE.
         */
        CARAPI_(void) SetVariablePadding(
            /* [in] */ Boolean variable);

        CARAPI_(AutoPtr<IRect>) GetConstantPadding();

        CARAPI_(void) SetConstantSize(
            /* [in] */ Boolean constant);

        CARAPI_(Boolean) IsConstantSize();

        CARAPI_(Int32) GetConstantWidth();

        CARAPI_(Int32) GetConstantHeight();

        CARAPI_(Int32) GetConstantMinimumWidth();

        CARAPI_(Int32) GetConstantMinimumHeight();

        CARAPI_(void) SetEnterFadeDuration(
            /* [in] */ Int32 duration);

        CARAPI_(Int32) GetEnterFadeDuration();

        CARAPI_(void) SetExitFadeDuration(
            /* [in] */ Int32 duration);

        CARAPI_(Int32) GetExitFadeDuration();

        CARAPI_(Int32) GetOpacity();

        CARAPI_(Boolean) IsStateful();

        virtual CARAPI_(void) GrowArray(
            /* [in] */ Int32 oldSize,
            /* [in] */ Int32 newSize);

        //synchronized
        virtual CARAPI_(Boolean) CanConstantState();

    protected:
        CARAPI_(void) ComputeConstantSize();

    public:
        DrawableContainer* mOwner;

        AutoPtr<IResources> mRes;

        AutoPtr<IResourcesTheme> mTheme;

        AutoPtr<ISparseArray> mDrawableFutures;//SparseArray<ConstantStateFuture>

        Int32 mChangingConfigurations;
        Int32 mChildrenChangingConfigurations;

        AutoPtr<ArrayOf<IDrawable*> > mDrawables;
        Int32 mNumChildren;

        Boolean mVariablePadding;
        Boolean mPaddingChecked;
        AutoPtr<IRect> mConstantPadding;

        Boolean mConstantSize;
        Boolean mComputedConstantSize;
        Int32 mConstantWidth;
        Int32 mConstantHeight;
        Int32 mConstantMinimumWidth;
        Int32 mConstantMinimumHeight;

        Boolean mCheckedOpacity;
        Int32 mOpacity;

        Boolean mCheckedStateful;
        Boolean mStateful;

        Boolean mCheckedConstantState;
        Boolean mCanConstantState;

        Boolean mDither/* = DEFAULT_DITHER*/;

        Boolean mMutated;
        Int32 mLayoutDirection;

        Int32 mEnterFadeDuration;
        Int32 mExitFadeDuration;

        Boolean mAutoMirrored;

        AutoPtr<IColorFilter> mColorFilter;
        Boolean mHasColorFilter;

        AutoPtr<IColorStateList> mTintList;
        PorterDuffMode mTintMode;
        Boolean mHasTintList;
        Boolean mHasTintMode;
    };

    class _Runnable
        : public Runnable
    {
    public:
        _Runnable(
            /* [in] */ DrawableContainer* host);

        CARAPI Run();

    private:
        DrawableContainer*  mHost;
    };

private:
        /**
         * Class capable of cloning a Drawable from another Drawable's
         * ConstantState.
         */
        class ConstantStateFuture
            : public Object
        {
        public:
            ConstantStateFuture(
                /* [in] */ IDrawable* source);

            /**
             * Obtains and prepares the Drawable represented by this future.
             *
             * @param state the container into which this future will be placed
             * @return a prepared Drawable
             */
            AutoPtr<IDrawable> Get(
                /* [in] */ DrawableContainerState* state);

            /**
             * Whether the constant state wrapped by this future can apply a
             * theme.
             */
            Boolean CanApplyTheme();
        private:
            AutoPtr<IDrawableConstantState> mConstantState;
        };

public:
    CAR_INTERFACE_DECL()

    DrawableContainer();

    //@Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    /**
     * @hide
     */
    //@Override
    virtual CARAPI GetOpticalInsets(
        /* [out] */ IInsets** sets);

    // @Override
    CARAPI GetOutline(
        /* [in] */ /*@NonNull*/ IOutline* outline);

    //@Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    //@Override
    virtual CARAPI SetDither(
        /* [in] */ Boolean dither);

    //@Override
    virtual CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Change the global fade duration when a new drawable is entering
     * the scene.
     * @param ms The amount of time to fade in milliseconds.
     */
    virtual CARAPI SetEnterFadeDuration(
        /* [in] */ Int32 ms);

    /**
     * Change the global fade duration when a new drawable is leaving
     * the scene.
     * @param ms The amount of time to fade in milliseconds.
     */
    virtual CARAPI SetExitFadeDuration(
        /* [in] */ Int32 ms);

    //@Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    // @Override
    CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    // @Override
    CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

    //@Override
    CARAPI JumpToCurrentState();

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

    /** @hide */
    // @Override
    CARAPI GetHotspotBounds(
        /* [in] */ IRect* outRect);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetMinimumWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetMinimumHeight(
        /* [out] */ Int32* height);

    virtual CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* who);

    virtual CARAPI ScheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what,
        /* [in] */ Int64 when);

    virtual CARAPI UnscheduleDrawable(
        /* [in] */ IDrawable* who,
        /* [in] */ IRunnable* what);

    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    //@Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    /** @hide */
    virtual CARAPI SetCurrentIndex(
        /* [in] */Int32 index);

    /** @hide */
    virtual CARAPI GetCurrentIndex(
        /* [in] */ Int32* index);

    virtual CARAPI SelectDrawable(
        /* [in] */ Int32 idx,
        /* [out] */ Boolean* res);

    CARAPI_(void) Animate(
        /* [in] */ Boolean schedule);

    //@Override
    CARAPI GetCurrent(
        /* [out] */ IDrawable** drawable);

    // @Override
    CARAPI ApplyTheme(
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Mutate();

protected:
    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* state);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    virtual CARAPI_(void) SetConstantState(
        /* [in] */ DrawableContainerState* state);

    CARAPI_(Boolean) NeedsMirroring();

private:
    static const Boolean DEBUG;
    static const String TAG;

    /**
     * To be proper, we should have a getter for dither (and alpha, etc.)
     * so that proxy classes like this can save/restore their delegates'
     * values, but we don't have getters. Since we do have setters
     * (e.g. setDither), which this proxy forwards on, we have to have some
     * default/initial setting.
     *
     * The initial setting for dither is now true, since it almost always seems
     * to improve the quality at negligible cost.
     */
    static const Boolean DEFAULT_DITHER = TRUE;
    AutoPtr<DrawableContainerState> mDrawableContainerState;
    AutoPtr<IRect> mHotspotBounds;
    AutoPtr<IDrawable> mCurrDrawable;
    Int32 mAlpha;
    /** Whether setAlpha() has been called at least once. */
    Boolean mHasAlpha;

    Int32 mCurIndex;
    Boolean mMutated;

    // Animations.
    AutoPtr<IRunnable> mAnimationRunnable;
    Int64 mEnterAnimationEnd;
    Int64 mExitAnimationEnd;
    AutoPtr<IDrawable> mLastDrawable;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_DRAWABLECONTAINER_H__
