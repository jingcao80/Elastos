
#ifndef __ELASTOS_DROID_WIDGET_COMPOUNDBUTTON_H__
#define __ELASTOS_DROID_WIDGET_COMPOUNDBUTTON_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/Button.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * <p>
 * A button with two states, checked and unchecked. When the button is pressed
 * or clicked, the state changes automatically.
 * </p>
 *
 * <p><strong>XML attributes</strong></p>
 * <p>
 * See {@link android.R.styleable#CompoundButton
 * CompoundButton Attributes}, {@link android.R.styleable#Button Button
 * Attributes}, {@link android.R.styleable#TextView TextView Attributes}, {@link
 * android.R.styleable#View View Attributes}
 * </p>
 */
class ECO_PUBLIC CompoundButton
    : public Button
    , public ICompoundButton
    , public ICheckable
{
public:
    class SavedState
        : public View::BaseSavedState
        , public ICompoundButtonSavedState
    {
        friend class CompoundButton;
    public:
        CAR_INTERFACE_DECL();

        SavedState();

        ~SavedState();

        CARAPI constructor(
            /* [in] */ IParcelable* superState);

        CARAPI constructor();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    protected:
        Boolean mSavedStateChecked;
    };

public:
    CAR_INTERFACE_DECL();

    CompoundButton();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI Toggle();

    //@Override
    CARAPI PerformClick(
        /* [out] */ Boolean* res);

    //@ViewDebug.ExportedProperty
    CARAPI IsChecked(
        /* [out] */ Boolean* isChecked);

    /**
     * <p>Changes the checked state of this button.</p>
     *
     * @param checked TRUE to check the button, FALSE to uncheck it
     */
    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    /**
     * Register a callback to be invoked when the checked state of this button
     * changes.
     *
     * @param listener the callback to call on checked state change
     */
    CARAPI SetOnCheckedChangeListener(
        /* [in] */ ICompoundButtonOnCheckedChangeListener* listener);

    /**
     * Register a callback to be invoked when the checked state of this button
     * changes. This callback is used for internal purpose only.
     *
     * @param listener the callback to call on checked state change
     * @hide
     */
    CARAPI SetOnCheckedChangeWidgetListener(
        /* [in] */ ICompoundButtonOnCheckedChangeListener* listener);

    /**
     * Set the button graphic to a given Drawable, identified by its resource
     * id.
     *
     * @param resid the resource id of the drawable to use as the button
     *        graphic
     */
    CARAPI SetButtonDrawable(
        /* [in] */ Int32 resid);

    /**
     * Set the button graphic to a given Drawable
     *
     * @param d The Drawable to use as the button graphic
     */
    CARAPI SetButtonDrawable(
        /* [in] */ IDrawable* d);

    /**
     * Applies a tint to the button drawable. Does not modify the current tint
     * mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * Subsequent calls to {@link #setButtonDrawable(Drawable)} will
     * automatically mutate the drawable and apply the specified tint and tint
     * mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#CompoundButton_buttonTint
     * @see #setButtonTintList(ColorStateList)
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetButtonTintList(
        /* [in] */ IColorStateList* tint);

    /**
     * @return the tint applied to the button drawable
     * @attr ref android.R.styleable#CompoundButton_buttonTint
     * @see #setButtonTintList(ColorStateList)
     */
    //@Nullable
    CARAPI GetButtonTintList(
        /* [out] */ IColorStateList** tint);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setButtonTintList(ColorStateList)}} to the button drawable. The
     * default mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#CompoundButton_buttonTintMode
     * @see #getButtonTintMode()
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetButtonTintMode(
        /* [in] */ Elastos::Droid::Graphics::PorterDuffMode tintMode);

    /**
     * @return the blending mode used to apply the tint to the button drawable
     * @attr ref android.R.styleable#CompoundButton_buttonTintMode
     * @see #setButtonTintMode(PorterDuff.Mode)
     */
    //@Nullable
    CARAPI GetButtonTintMode(
        /* [out] */ Elastos::Droid::Graphics::PorterDuffMode* tintMode);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI GetCompoundPaddingLeft(
        /* [out] */ Int32* left);

    // @Override
    CARAPI GetCompoundPaddingRight(
        /* [out] */ Int32* right);

    // @Override
    CARAPI GetHorizontalOffsetForDrawables(
        /* [out] */ Int32* offset);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    // @Override
    CARAPI JumpDrawablesToCurrentState();

    //@Override
    CARAPI_(AutoPtr<IParcelable>) OnSaveInstanceState();

    //@Override
    CARAPI_(void) OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

protected:
    //@Override
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    virtual CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out] */ ArrayOf<Int32>** drawableState);

    //@Override
    virtual CARAPI DrawableStateChanged();

    //@Override
    virtual CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

private:
    ECO_LOCAL CARAPI_(void) ApplyButtonTint();

private:
    Boolean mChecked;
    Int32 mButtonResource;
    Boolean mBroadcasting;
    AutoPtr<IDrawable> mButtonDrawable;
    AutoPtr<IColorStateList> mButtonTintList;
    Elastos::Droid::Graphics::PorterDuffMode mButtonTintMode;
    Boolean mHasButtonTint;
    Boolean mHasButtonTintMode;

    AutoPtr<ICompoundButtonOnCheckedChangeListener> mOnCheckedChangeListener;
    AutoPtr<ICompoundButtonOnCheckedChangeListener> mOnCheckedChangeWidgetListener;

    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > CHECKED_STATE_SET;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_COMPOUNDBUTTON_H__
