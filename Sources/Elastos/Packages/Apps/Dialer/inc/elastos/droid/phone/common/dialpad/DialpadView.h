
#ifndef __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADVIEW_H__
#define __ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADVIEW_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IImageButton;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

class DialpadView
    : public LinearLayout
    , public IDialpadView
{
private:
    class MyAdapter
        : public AnimatorListenerAdapter
    {};

public:
    CAR_INTERFACE_DECL()

    DialpadView();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetShowVoicemailButton(
        /* [in] */ Boolean show);

    /**
     * Whether or not the digits above the dialer can be edited.
     *
     * @param canBeEdited If true, the backspace button will be shown and the digits EditText
     *         will be configured to allow text manipulation.
     */
    CARAPI SetCanDigitsBeEdited(
        /* [in] */ Boolean canBeEdited);

    CARAPI CanDigitsBeEdited(
        /* [out] */ Boolean* beEdited);

    /**
     * Always returns true for onHoverEvent callbacks, to fix problems with accessibility due to
     * the dialpad overlaying other fragments.
     */
    // @Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI AnimateShow();

    CARAPI GetDigits(
        /* [out] */ IEditText** digits);

    CARAPI GetDeleteButton(
        /* [out] */ IImageButton** button);

    CARAPI GetOverflowMenuButton(
        /* [out] */ IView** menuButton);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) SetupKeypad();

    /**
     * Get the animation delay for the buttons, taking into account whether the dialpad is in
     * landscape left-to-right, landscape right-to-left, or portrait.
     *
     * @param buttonId The button ID.
     * @return The animation delay.
     */
    CARAPI_(Int32) GetKeyButtonAnimationDelay(
        /* [in] */ Int32 buttonId);

    /**
     * Get the button animation duration, taking into account whether the dialpad is in landscape
     * left-to-right, landscape right-to-left, or portrait.
     *
     * @param buttonId The button ID.
     * @return The animation duration.
     */
    CARAPI_(Int32) GetKeyButtonAnimationDuration(
        /* [in] */ Int32 buttonId);

private:
    static const String TAG;

    static const Double DELAY_MULTIPLIER;
    static const Double DURATION_MULTIPLIER;

    /**
     * {@code True} if the dialpad is in landscape orientation.
     */
    Boolean mIsLandscape;

    /**
     * {@code True} if the dialpad is showing in a right-to-left locale.
     */
    Boolean mIsRtl;

    AutoPtr<IEditText> mDigits;
    AutoPtr<IImageButton> mDelete;
    AutoPtr<IView> mOverflowMenuButton;
    AutoPtr<IColorStateList> mRippleColor;

    Boolean mCanDigitsBeEdited;

    AutoPtr< ArrayOf<Int32> > mButtonIds;

    // For animation.
    static const Int32 KEY_FRAME_DURATION;

    Int32 mTranslateDistance;
};

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_DIALPAD_DIALPADVIEW_H__
