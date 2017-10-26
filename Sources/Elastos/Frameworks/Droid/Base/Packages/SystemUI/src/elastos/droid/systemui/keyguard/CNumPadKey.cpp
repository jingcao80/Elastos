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

#include "elastos/droid/systemui/keyguard/CNumPadKey.h"
#include "elastos/droid/systemui/keyguard/LiftToActivateListener.h"
#include "elastos/droid/systemui/keyguard/ObscureSpeechDelegate.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Character.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include "R.h"

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IAccessibilityDelegate;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Core::Character;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CNumPadKey::MyViewOnClickListener, Object, IViewOnClickListener)

ECode CNumPadKey::MyViewOnClickListener::OnClick(
    /* [in] */ IView* thisView)
{
    if (mHost->mTextView == NULL && mHost->mTextViewResId > 0) {
        AutoPtr<IView> rv;
        mHost->GetRootView((IView**)&rv);
        AutoPtr<IView> v;
        rv->FindViewById(mHost->mTextViewResId, (IView**)&v);
        if (v != NULL && IPasswordTextView::Probe(v) != NULL) {
            mHost->mTextView = IPasswordTextView::Probe(v);
        }
    }
    Boolean res;
    if (mHost->mTextView != NULL &&
            (IView::Probe(mHost->mTextView)->IsEnabled(&res), res)) {
        mHost->mTextView->Append(Character::ForDigit(mHost->mDigit, 10));
    }
    mHost->UserActivity();
    return mHost->DoHapticKeyClick();
}

AutoPtr<ArrayOf<String> > CNumPadKey::sKlondike;

CAR_OBJECT_IMPL(CNumPadKey)

CNumPadKey::CNumPadKey()
    : mDigit(-1)
    , mTextViewResId(0)
    , mEnableHaptics(FALSE)
{
    mListener = new MyViewOnClickListener(this);
}

ECode CNumPadKey::UserActivity()
{
    return mPM->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
}

ECode CNumPadKey::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CNumPadKey::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CNumPadKey::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    ViewGroup::constructor(context, attrs, defStyle);

    SetFocusable(TRUE);
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::NumPadKey);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    //try {
    FAIL_GOTO(a->GetInt32(R::styleable::NumPadKey_digit, mDigit, &mDigit), FINALLY)
    FAIL_GOTO(a->GetResourceId(R::styleable::NumPadKey_textView, 0, &mTextViewResId), FINALLY)
    //} finally {
FINALLY:
    a->Recycle();
    //}

    SetOnClickListener(mListener);
    AutoPtr<LiftToActivateListener> lis = new LiftToActivateListener();
    lis->constructor(context);
    SetOnHoverListener(IViewOnHoverListener::Probe(lis));

    AutoPtr<ObscureSpeechDelegate> delegate = new ObscureSpeechDelegate(context);
    SetAccessibilityDelegate(IAccessibilityDelegate::Probe(delegate));

    AutoPtr<ILockPatternUtils> utils;
    CLockPatternUtils::New(context, (ILockPatternUtils**)&utils);
    utils->IsTactileFeedbackEnabled(&mEnableHaptics);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPM = IPowerManager::Probe(obj);

    AutoPtr<IContext> context2;
    GetContext((IContext**)&context2);
    AutoPtr<IInterface> obj2;
    context2->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&obj2);
    AutoPtr<ILayoutInflater> inflater = ILayoutInflater::Probe(obj2);
    AutoPtr<IView> tmp;
    inflater->Inflate(R::layout::keyguard_num_pad_key, this, TRUE, (IView**)&tmp);

    AutoPtr<IView> view;
    FindViewById(R::id::digit_text, (IView**)&view);
    mDigitText = ITextView::Probe(view);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(StringUtils::ToString(mDigit));
    mDigitText->SetText(cchar);

    AutoPtr<IView> view2;
    FindViewById(R::id::klondike_text, (IView**)&view2);
    mKlondikeText = ITextView::Probe(view2);

    UpdateText();
    AutoPtr<IDrawable> drawable;
    mContext->GetDrawable(R::drawable::ripple_drawable, (IDrawable**)&drawable);
    SetBackground(drawable);

    AutoPtr<ICharSequence> text;
    mDigitText->GetText((ICharSequence**)&text);
    String str;
    text->ToString(&str);
    AutoPtr<ICharSequence> text2;
    mKlondikeText->GetText((ICharSequence**)&text2);
    String str2;
    text2->ToString(&str2);
    AutoPtr<ICharSequence> cchar2 = CoreUtils::Convert(str + str2);
    return SetContentDescription(cchar2);
}

ECode CNumPadKey::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();

    // Reset the "announced headset" flag when detached.
    ObscureSpeechDelegate::sAnnouncedHeadset = FALSE;
    return NOERROR;
}

ECode CNumPadKey::SetDigit(
    /* [in] */ Int32 digit)
{
    mDigit = digit;
    UpdateText();
    return NOERROR;
}

void CNumPadKey::UpdateText()
{
    if (mDigit >= 0) {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(StringUtils::ToString(mDigit));
        mDigitText->SetText(cchar);
        if (sKlondike == NULL) {
            AutoPtr<IResources> resources;
            GetResources((IResources**)&resources);
            resources->GetStringArray(R::array::lockscreen_num_pad_klondike,
                    (ArrayOf<String>**)&sKlondike);
        }
        if (sKlondike != NULL && sKlondike->GetLength() > mDigit) {
            String klondike = (*sKlondike)[mDigit];
            Int32 len = klondike.GetLength();
            if (len > 0) {
                AutoPtr<ICharSequence> cchar = CoreUtils::Convert(klondike);
                mKlondikeText->SetText(cchar);
            }
            else {
                IView::Probe(mKlondikeText)->SetVisibility(IView::INVISIBLE);
            }
        }
    }
}

ECode CNumPadKey::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    MeasureChildren(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

ECode CNumPadKey::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 digitHeight;
    IView::Probe(mDigitText)->GetMeasuredHeight(&digitHeight);
    Int32 klondikeHeight;
    IView::Probe(mKlondikeText)->GetMeasuredHeight(&klondikeHeight);
    Int32 totalHeight = digitHeight + klondikeHeight;

    Int32 h;
    GetHeight(&h);
    Int32 top = h / 2 - totalHeight / 2;
    Int32 w;
    GetWidth(&w);
    Int32 centerX = w / 2;

    Int32 dmw;
    IView::Probe(mDigitText)->GetMeasuredWidth(&dmw);
    Int32 left = centerX - dmw / 2;
    Int32 bottom = top + digitHeight;
    IView::Probe(mDigitText)->Layout(left, top, left + dmw, bottom);
    top = (Int32)(bottom - klondikeHeight * 0.35f);
    bottom = top + klondikeHeight;

    Int32 kmw;
    IView::Probe(mKlondikeText)->GetMeasuredWidth(&kmw);
    left = centerX - kmw / 2;
    return IView::Probe(mKlondikeText)->Layout(left, top, left + kmw, bottom);
}

ECode CNumPadKey::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CNumPadKey::DoHapticKeyClick()
{
    if (mEnableHaptics) {
        Boolean res;
        PerformHapticFeedback(IHapticFeedbackConstants::VIRTUAL_KEY,
                IHapticFeedbackConstants::FLAG_IGNORE_VIEW_SETTING
                | IHapticFeedbackConstants::FLAG_IGNORE_GLOBAL_SETTING, &res);
    }
    return NOERROR;
}


} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
