
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include "elastos/droid/phone/common/dialpad/DialpadView.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::Drawable::IRippleDrawable;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

#ifdef TELESERVICE_USE_PHONECOMMON
#include "../../../../../../../../Service/Telephony/R.h"
using Elastos::Droid::TeleService::R;
#endif

#ifdef DIALER_USE_PHONECOMMON
#include "../../../../../../../Dialer/R.h"
using Elastos::Droid::Dialer::R;
#endif

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {
namespace Dialpad {

const String DialpadView::TAG("DialpadView");
const Double DialpadView::DELAY_MULTIPLIER = 0.66;
const Double DialpadView::DURATION_MULTIPLIER = 0.8;
const Int32 DialpadView::KEY_FRAME_DURATION = 33;

CAR_INTERFACE_IMPL(DialpadView, LinearLayout, IDialpadView);

DialpadView::DialpadView()
    : mIsLandscape(FALSE)
    , mIsRtl(FALSE)
    , mCanDigitsBeEdited(FALSE)
    , mTranslateDistance(0)
{
    mButtonIds = ArrayOf<Int32>::Alloc(12);
    (*mButtonIds)[0] = R::id::zero;
    (*mButtonIds)[1] = R::id::one;
    (*mButtonIds)[2] = R::id::two;
    (*mButtonIds)[3] = R::id::three;
    (*mButtonIds)[4] = R::id::four;
    (*mButtonIds)[5] = R::id::five;
    (*mButtonIds)[6] = R::id::six;
    (*mButtonIds)[7] = R::id::seven;
    (*mButtonIds)[8] = R::id::eight;
    (*mButtonIds)[9] = R::id::nine;
    (*mButtonIds)[10] = R::id::star;
    (*mButtonIds)[11] = R::id::pound;
}

ECode DialpadView::constructor(
    /* [in] */ IContext* ctx)
{
    return constructor(ctx, NULL);
}

ECode DialpadView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(ctx, attrs, 0);
}

ECode DialpadView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FAIL_RETURN(LinearLayout::constructor(context, attrs, defStyle));

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Dialpad);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    a->GetColorStateList(R::styleable::Dialpad_dialpad_key_button_touch_tint, (IColorStateList**)&mRippleColor);
    a->Recycle();

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(
            R::dimen::dialpad_key_button_translate_y, &mTranslateDistance);

    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    mIsLandscape = orientation == IConfiguration::ORIENTATION_LANDSCAPE;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    mIsRtl = TextUtils::GetLayoutDirectionFromLocale(locale) ==
            IView::LAYOUT_DIRECTION_RTL;
    return NOERROR;
}

ECode DialpadView::OnFinishInflate()
{
    SetupKeypad();
    AutoPtr<IView> v;
    FindViewById(R::id::digits, (IView**)&v);
    mDigits = IEditText::Probe(v);
    v = NULL;
    FindViewById(R::id::deleteButton, (IView**)&v);
    mDelete = IImageButton::Probe(v);
    v = NULL;
    FindViewById(R::id::dialpad_overflow, (IView**)&v);
    mOverflowMenuButton = v;
    return NOERROR;
}

void DialpadView::SetupKeypad()
{
    AutoPtr< ArrayOf<Int32> > numberIds = ArrayOf<Int32>::Alloc(12);
    (*numberIds)[0] = R::string::dialpad_0_number;
    (*numberIds)[1] = R::string::dialpad_1_number;
    (*numberIds)[2] = R::string::dialpad_2_number;
    (*numberIds)[3] = R::string::dialpad_3_number;
    (*numberIds)[4] = R::string::dialpad_4_number;
    (*numberIds)[5] = R::string::dialpad_5_number;
    (*numberIds)[6] = R::string::dialpad_6_number;
    (*numberIds)[7] = R::string::dialpad_7_number;
    (*numberIds)[8] = R::string::dialpad_8_number;
    (*numberIds)[9] = R::string::dialpad_9_number;
    (*numberIds)[10] = R::string::dialpad_star_number;
    (*numberIds)[11] = R::string::dialpad_pound_number;

    AutoPtr< ArrayOf<Int32> > letterIds = ArrayOf<Int32>::Alloc(12);
    (*letterIds)[0] = R::string::dialpad_0_letters;
    (*letterIds)[1] = R::string::dialpad_1_letters;
    (*letterIds)[2] = R::string::dialpad_2_letters;
    (*letterIds)[3] = R::string::dialpad_3_letters;
    (*letterIds)[4] = R::string::dialpad_4_letters;
    (*letterIds)[5] = R::string::dialpad_5_letters;
    (*letterIds)[6] = R::string::dialpad_6_letters;
    (*letterIds)[7] = R::string::dialpad_7_letters;
    (*letterIds)[8] = R::string::dialpad_8_letters;
    (*letterIds)[9] = R::string::dialpad_9_letters;
    (*letterIds)[10] = R::string::dialpad_star_letters;
    (*letterIds)[11] = R::string::dialpad_pound_letters;

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);

    AutoPtr<IDialpadKeyButton> dialpadKey;
    AutoPtr<ITextView> numberView;
    AutoPtr<ITextView> lettersView;

    for (Int32 i = 0; i < mButtonIds->GetLength(); i++) {
        AutoPtr<IView> dialpadKeyV;
        FindViewById((*mButtonIds)[i], (IView**)&dialpadKeyV);
        dialpadKey = IDialpadKeyButton::Probe(dialpadKeyV);
        AutoPtr<IView> v;
        dialpadKeyV->FindViewById(R::id::dialpad_key_number, (IView**)&v);
        numberView = ITextView::Probe(v);
        v = NULL;
        dialpadKeyV->FindViewById(R::id::dialpad_key_letters, (IView**)&v);
        lettersView = ITextView::Probe(v);
        String numberString;
        resources->GetString((*numberIds)[i], &numberString);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IDrawable> d;
        context->GetDrawable(R::drawable::btn_dialpad_key, (IDrawable**)&d);
        IRippleDrawable* rippleBackground = IRippleDrawable::Probe(d);
        if (mRippleColor != NULL) {
            rippleBackground->SetColor(mRippleColor);
        }

        numberView->SetText(CoreUtils::Convert(numberString));
        numberView->SetElegantTextHeight(FALSE);
        dialpadKeyV->SetContentDescription(CoreUtils::Convert(numberString));
        dialpadKeyV->SetBackground(d);

        if (lettersView != NULL) {
            String letter;
            resources->GetString((*letterIds)[i], &letter);
            lettersView->SetText(CoreUtils::Convert(letter));
        }
    }

    AutoPtr<IView> vv;
    FindViewById(R::id::one, (IView**)&vv);
    AutoPtr<IDialpadKeyButton> one = IDialpadKeyButton::Probe(vv);
    AutoPtr<ICharSequence> text;
    resources->GetText(R::string::description_voicemail_button, (ICharSequence**)&text);
    one->SetLongHoverContentDescription(text);

    vv = NULL;
    FindViewById(R::id::zero, (IView**)&vv);
    AutoPtr<IDialpadKeyButton> zero = IDialpadKeyButton::Probe(vv);
    text = NULL;
    resources->GetText(R::string::description_image_button_plus, (ICharSequence**)&text);
    zero->SetLongHoverContentDescription(text);
}

ECode DialpadView::SetShowVoicemailButton(
    /* [in] */ Boolean show)
{
    AutoPtr<IView> view;
    FindViewById(R::id::dialpad_key_voicemail, (IView**)&view);
    if (view != NULL) {
        view->SetVisibility(show ? IView::VISIBLE : IView::INVISIBLE);
    }
    return NOERROR;
}

ECode DialpadView::SetCanDigitsBeEdited(
    /* [in] */ Boolean canBeEdited)
{
    AutoPtr<IView> deleteButton;
    FindViewById(R::id::deleteButton, (IView**)&deleteButton);
    deleteButton->SetVisibility(canBeEdited ? IView::VISIBLE : IView::GONE);
    AutoPtr<IView> overflowMenuButton;
    FindViewById(R::id::dialpad_overflow, (IView**)&overflowMenuButton);
    overflowMenuButton->SetVisibility(canBeEdited ? IView::VISIBLE : IView::GONE);

    AutoPtr<IView> digits;
    FindViewById(R::id::digits, (IView**)&digits);
    digits->SetClickable(canBeEdited);
    digits->SetLongClickable(canBeEdited);
    digits->SetFocusableInTouchMode(canBeEdited);
    ITextView::Probe(digits)->SetCursorVisible(FALSE);

    mCanDigitsBeEdited = canBeEdited;
    return NOERROR;
}

ECode DialpadView::CanDigitsBeEdited(
    /* [out] */ Boolean* beEdited)
{
    VALIDATE_NOT_NULL(beEdited);
    *beEdited = mCanDigitsBeEdited;
    return NOERROR;
}

ECode DialpadView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode DialpadView::AnimateShow()
{
    // This is a hack; without this, the setTranslationY is delayed in being applied, and the
    // numbers appear at their original position (0) momentarily before animating.
    AutoPtr<AnimatorListenerAdapter> showListener = new MyAdapter();

    for (Int32 i = 0; i < mButtonIds->GetLength(); i++) {
        Int32 delay = (Int32)(GetKeyButtonAnimationDelay((*mButtonIds)[i]) * DELAY_MULTIPLIER);
        Int32 duration =
                (Int32)(GetKeyButtonAnimationDuration((*mButtonIds)[i]) * DURATION_MULTIPLIER);
        AutoPtr<IView> dialpadKey;
        FindViewById((*mButtonIds)[i], (IView**)&dialpadKey);

        AutoPtr<IViewPropertyAnimator> animator;
        dialpadKey->Animate((IViewPropertyAnimator**)&animator);
        if (mIsLandscape) {
            // Landscape orientation requires translation along the X axis.
            // For RTL locales, ensure we translate negative on the X axis.
            dialpadKey->SetTranslationX((mIsRtl ? -1 : 1) * mTranslateDistance);
            animator->TranslationX(0);
        }
        else {
            // Portrait orientation requires translation along the Y axis.
            dialpadKey->SetTranslationY(mTranslateDistance);
            animator->TranslationY(0);
        }
        animator->SetInterpolator(ITimeInterpolator::Probe(AnimUtils::EASE_OUT_EASE_IN));
        animator->SetStartDelay(delay);
        animator->SetDuration(duration);
        animator->SetListener(showListener);
        animator->Start();
    }
    return NOERROR;
}

ECode DialpadView::GetDigits(
    /* [out] */ IEditText** digits)
{
    VALIDATE_NOT_NULL(digits);
    *digits = mDigits;
    REFCOUNT_ADD(*digits);
    return NOERROR;
}

ECode DialpadView::GetDeleteButton(
    /* [out] */ IImageButton** button)
{
    VALIDATE_NOT_NULL(button);
    *button = mDelete;
    REFCOUNT_ADD(*button);
    return NOERROR;
}

ECode DialpadView::GetOverflowMenuButton(
    /* [out] */ IView** menuButton)
{
    VALIDATE_NOT_NULL(menuButton);
    *menuButton = mOverflowMenuButton;
    REFCOUNT_ADD(*menuButton);
    return NOERROR;
}

Int32 DialpadView::GetKeyButtonAnimationDelay(
    /* [in] */ Int32 buttonId)
{
    if (mIsLandscape) {
        if (mIsRtl) {
            switch (buttonId) {
                case R::id::three: return KEY_FRAME_DURATION * 1;
                case R::id::six: return KEY_FRAME_DURATION * 2;
                case R::id::nine: return KEY_FRAME_DURATION * 3;
                case R::id::pound: return KEY_FRAME_DURATION * 4;
                case R::id::two: return KEY_FRAME_DURATION * 5;
                case R::id::five: return KEY_FRAME_DURATION * 6;
                case R::id::eight: return KEY_FRAME_DURATION * 7;
                case R::id::zero: return KEY_FRAME_DURATION * 8;
                case R::id::one: return KEY_FRAME_DURATION * 9;
                case R::id::four: return KEY_FRAME_DURATION * 10;
                case R::id::seven:
                case R::id::star:
                    return KEY_FRAME_DURATION * 11;
            }
        }
        else {
            switch (buttonId) {
                case R::id::one: return KEY_FRAME_DURATION * 1;
                case R::id::four: return KEY_FRAME_DURATION * 2;
                case R::id::seven: return KEY_FRAME_DURATION * 3;
                case R::id::star: return KEY_FRAME_DURATION * 4;
                case R::id::two: return KEY_FRAME_DURATION * 5;
                case R::id::five: return KEY_FRAME_DURATION * 6;
                case R::id::eight: return KEY_FRAME_DURATION * 7;
                case R::id::zero: return KEY_FRAME_DURATION * 8;
                case R::id::three: return KEY_FRAME_DURATION * 9;
                case R::id::six: return KEY_FRAME_DURATION * 10;
                case R::id::nine:
                case R::id::pound:
                    return KEY_FRAME_DURATION * 11;
            }
        }
    }
    else {
        switch (buttonId) {
            case R::id::one: return KEY_FRAME_DURATION * 1;
            case R::id::two: return KEY_FRAME_DURATION * 2;
            case R::id::three: return KEY_FRAME_DURATION * 3;
            case R::id::four: return KEY_FRAME_DURATION * 4;
            case R::id::five: return KEY_FRAME_DURATION * 5;
            case R::id::six: return KEY_FRAME_DURATION * 6;
            case R::id::seven: return KEY_FRAME_DURATION * 7;
            case R::id::eight: return KEY_FRAME_DURATION * 8;
            case R::id::nine: return KEY_FRAME_DURATION * 9;
            case R::id::star: return KEY_FRAME_DURATION * 10;
            case R::id::zero:
            case R::id::pound:
                return KEY_FRAME_DURATION * 11;
        }
    }

    Logger::E(TAG, "Attempted to get animation delay for invalid key button id.");
    return 0;
}

Int32 DialpadView::GetKeyButtonAnimationDuration(
    /* [in] */ Int32 buttonId)
{
    if (mIsLandscape) {
        if (mIsRtl) {
            switch (buttonId) {
                case R::id::one:
                case R::id::four:
                case R::id::seven:
                case R::id::star:
                    return KEY_FRAME_DURATION * 8;
                case R::id::two:
                case R::id::five:
                case R::id::eight:
                case R::id::zero:
                    return KEY_FRAME_DURATION * 9;
                case R::id::three:
                case R::id::six:
                case R::id::nine:
                case R::id::pound:
                    return KEY_FRAME_DURATION * 10;
            }
        }
        else {
            switch (buttonId) {
                case R::id::one:
                case R::id::four:
                case R::id::seven:
                case R::id::star:
                    return KEY_FRAME_DURATION * 10;
                case R::id::two:
                case R::id::five:
                case R::id::eight:
                case R::id::zero:
                    return KEY_FRAME_DURATION * 9;
                case R::id::three:
                case R::id::six:
                case R::id::nine:
                case R::id::pound:
                    return KEY_FRAME_DURATION * 8;
            }
        }
    }
    else {
        switch (buttonId) {
            case R::id::one:
            case R::id::two:
            case R::id::three:
            case R::id::four:
            case R::id::five:
            case R::id::six:
                return KEY_FRAME_DURATION * 10;
            case R::id::seven:
            case R::id::eight:
            case R::id::nine:
                return KEY_FRAME_DURATION * 9;
            case R::id::star:
            case R::id::zero:
            case R::id::pound:
                return KEY_FRAME_DURATION * 8;
        }
    }

    Logger::E(TAG, "Attempted to get animation duration for invalid key button id.");
    return 0;
}

} // namespace Dialpad
} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos