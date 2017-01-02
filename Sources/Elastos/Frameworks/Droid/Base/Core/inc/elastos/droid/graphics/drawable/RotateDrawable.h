//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_ROTATEDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_ROTATEDRAWABLE_H__

#include "elastos/droid/graphics/drawable/Drawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class RotateDrawable
    : public Drawable
    , public IRotateDrawable
    , public IDrawableCallback
{
public:
    /**
     * <p>Represents the state of a rotation for a given drawable. The same
     * rotate drawable can be invoked with different states to drive several
     * rotations at the same time.</p>
     */
    class RotateState
        : public Drawable::ConstantState
    {
    public:
        RotateState(
            /* [in] */ RotateState* source,
            /* [in] */ RotateDrawable* owner,
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

        Float mFromDegrees;
        Float mToDegrees;

        Float mCurrentDegrees;

    private:
        Boolean mCanConstantState;
        Boolean mCheckedConstantState;
    };

public:
    CAR_INTERFACE_DECL()

    RotateDrawable();

    /**
     * <p>Create a new rotating drawable with an empty state.</p>
     */
    CARAPI constructor();

    /**
     * <p>Create a new rotating drawable with the specified state. A copy of
     * this state is used as the internal state for the newly created
     * drawable.</p>
     *
     * @param rotateState the state for this drawable
     */
    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res);

    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    /**
     * Sets the drawable rotated by this RotateDrawable.
     *
     * @param drawable The drawable to rotate
     */
    virtual CARAPI SetDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Returns the drawable rotated by this RotateDrawable.
     */
    virtual CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    //@Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* configuration);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    /**
     * Sets the start angle for rotation.
     *
     * @param fromDegrees Starting angle in degrees
     *
     * @see #getFromDegrees()
     * @attr ref android.R.styleable#RotateDrawable_fromDegrees
     */
    virtual CARAPI SetFromDegrees(
        /* [in] */ Float fromDegrees);

    /**
     * @return The starting angle for rotation in degrees
     *
     * @see #setFromDegrees(float)
     * @attr ref android.R.styleable#RotateDrawable_fromDegrees
     */
    virtual CARAPI GetFromDegrees(
        /* [out] */ Float* degrees);

    /**
     * Sets the end angle for rotation.
     *
     * @param toDegrees Ending angle in degrees
     *
     * @see #getToDegrees()
     * @attr ref android.R.styleable#RotateDrawable_toDegrees
     */
    virtual CARAPI SetToDegrees(
        /* [in] */ Float toDegrees);

    /**
     * @return The ending angle for rotation in degrees
     *
     * @see #setToDegrees(float)
     * @attr ref android.R.styleable#RotateDrawable_toDegrees
     */
    virtual CARAPI GetToDegrees(
        /* [out] */ Float* degrees);

    /**
     * Sets the X position around which the drawable is rotated.
     *
     * @param pivotX X position around which to rotate. If the X pivot is
     *            relative, the position represents a fraction of the drawable
     *            width. Otherwise, the position represents an absolute value in
     *            pixels.
     *
     * @see #setPivotXRelative(boolean)
     * @attr ref android.R.styleable#RotateDrawable_pivotX
     */
    virtual CARAPI SetPivotX(
        /* [in] */ Float pivotX);

    /**
     * @return X position around which to rotate
     *
     * @see #setPivotX(float)
     * @attr ref android.R.styleable#RotateDrawable_pivotX
     */
    virtual CARAPI GetPivotX(
        /* [out] */ Float* pivotX);

    /**
     * Sets whether the X pivot value represents a fraction of the drawable
     * width or an absolute value in pixels.
     *
     * @param relative True if the X pivot represents a fraction of the drawable
     *            width, or false if it represents an absolute value in pixels
     *
     * @see #isPivotXRelative()
     */
    virtual CARAPI SetPivotXRelative(
        /* [in] */ Boolean relative);

    /**
     * @return True if the X pivot represents a fraction of the drawable width,
     *         or false if it represents an absolute value in pixels
     *
     * @see #setPivotXRelative(boolean)
     */
    virtual CARAPI IsPivotXRelative(
        /* [out] */ Boolean* result);

    /**
     * Sets the Y position around which the drawable is rotated.
     *
     * @param pivotY Y position around which to rotate. If the Y pivot is
     *            relative, the position represents a fraction of the drawable
     *            height. Otherwise, the position represents an absolute value
     *            in pixels.
     *
     * @see #getPivotY()
     * @attr ref android.R.styleable#RotateDrawable_pivotY
     */
    virtual CARAPI SetPivotY(
        /* [in] */ Float pivotY);

    /**
     * @return Y position around which to rotate
     *
     * @see #setPivotY(float)
     * @attr ref android.R.styleable#RotateDrawable_pivotY
     */
    virtual CARAPI GetPivotY(
        /* [out] */ Float* pivotY);

    /**
     * Sets whether the Y pivot value represents a fraction of the drawable
     * height or an absolute value in pixels.
     *
     * @param relative True if the Y pivot represents a fraction of the drawable
     *            height, or false if it represents an absolute value in pixels
     *
     * @see #isPivotYRelative()
     */
    virtual CARAPI SetPivotYRelative(
        /* [in] */ Boolean relative);

    /**
     * @return True if the Y pivot represents a fraction of the drawable height,
     *         or false if it represents an absolute value in pixels
     *
     * @see #setPivotYRelative(boolean)
     */
    virtual CARAPI IsPivotYRelative(
        /* [out] */ Boolean* result);

    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

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

    //@Override
    CARAPI GetPadding(
        /* [in] */ IRect* padding,
        /* [out] */ Boolean* isPadding);

    //@Override
    CARAPI SetVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Boolean restart,
        /* [out] */ Boolean* isDifferent);

    //@Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    //@Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    //@Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    //@Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    //@Override
    CARAPI Inflate(
        /* [in] */ IResources* r,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme);

    //@Override
    CARAPI Mutate();

protected:
    //@Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* state);

    //@Override
    CARAPI_(Boolean) OnLevelChange(
        /* [in] */ Int32 level);

    //@Override
    CARAPI_(void) OnBoundsChange(
        /* [in] */ IRect* bounds);

private:
    static const Float MAX_LEVEL;

    AutoPtr<RotateState> mState;
    Boolean mMutated;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_ROTATEDRAWABLE_H__
