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

#ifndef _ELASTOS_DROID_CALCULATOR3_CALCULATOR_H__
#define _ELASTOS_DROID_CALCULATOR3_CALCULATOR_H__

#include "_Elastos.Droid.Calculator3.h"
#include "elastos/droid/calculator3/CalculatorEditText.h"
#include "elastos/droid/calculator3/CalculatorExpressionEvaluator.h"
#include "elastos/droid/calculator3/CalculatorExpressionTokenizer.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Support.V4.View.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IEditableFactory;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Support::V4::View::IViewPager;

namespace Elastos {
namespace Droid {
namespace Calculator3 {

class Calculator
    : public Activity
    , public IEvaluateCallback
    , public ICalculator
{
private:
    class InnerListener
        : public Object
        , public IOnTextSizeChangeListener
        , public IViewOnLongClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ Calculator* host);

        CARAPI OnLongClick(
            /* [in] */ IView* iview,
            /* [out] */ Boolean* result);

        CARAPI OnEvaluate(
            /* [in] */ const String& expr,
            /* [in] */ const String& result,
            /* [in] */ Int32 errorResourceId);

        CARAPI OnTextSizeChanged(
            /* [in] */ ITextView* textView,
            /* [in] */ Float oldSize);
    private:
        Calculator* mHost;
    };

    class MyTextWatcher
        : public Object
        , public ITextWatcher
    {
        friend class Calculator;

    public:
        MyTextWatcher(
            /* [in] */ Calculator* host);

        CAR_INTERFACE_DECL()

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* charSequence,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* charSequence,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* editable);

    private:
        Calculator* mHost;
    };

    class MyOnKeyListener
        : public Object
        , public IViewOnKeyListener
    {
        friend class Calculator;

    public:
        MyOnKeyListener(
            /* [in] */ Calculator* host);

        CAR_INTERFACE_DECL()

        CARAPI OnKey(
            /* [in] */ IView* view,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* keyEvent,
            /* [out] */ Boolean* result);

    private:
        Calculator* mHost;
    };

    class MyEditableFactory
        : public Object
        , public IEditableFactory
    {
        friend class Calculator;

    public:
        MyEditableFactory(
            /* [in] */ Calculator* host);

        CAR_INTERFACE_DECL()

        CARAPI NewEditable(
            /* [in] */ ICharSequence* source,
            /* [out] */ IEditable** editable);

    private:
        Calculator* mHost;
    };

    class RevealAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        RevealAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ IViewGroupOverlay* vgo,
            /* [in] */ IView* view);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        AutoPtr<IViewGroupOverlay> mVgo;
        AutoPtr<IView> mView;
    };

    class OnClearAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        OnClearAnimatorListenerAdapter(
            /* [in] */ Calculator* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
    };

    class OnErrorAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        OnErrorAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ Int32 errorResourceId);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        Int32 mErrorResId;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
        friend class Calculator;

    public:
        MyAnimatorUpdateListener(
            /* [in] */ Calculator* host);

        CAR_INTERFACE_DECL()

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* valueAnimator);

    private:
        Calculator* mHost;
    };

    class OnResultAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        OnResultAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ const String& result,
            /* [in] */ Int32 color);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        String mResult;
        Int32 mColor;
    };

public:
    Calculator();

    CARAPI constructor();

    virtual ~Calculator();

    CAR_INTERFACE_DECL()

    CARAPI OnBackPressed();

    CARAPI OnUserInteraction();

    CARAPI OnButtonClick(
        /* [in] */ IView* iview);

    CARAPI OnLongClick(
        /* [in] */ IView* iview,
        /* [out] */ Boolean* result);

    CARAPI OnEvaluate(
        /* [in] */ const String& expr,
        /* [in] */ const String& result,
        /* [in] */ Int32 errorResourceId);

    CARAPI OnTextSizeChanged(
        /* [in] */ ITextView* textView,
        /* [in] */ Float oldSize);

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnSaveInstanceState(
        /*[in] */ /*@NonNull */IBundle* outState);

private:
    CARAPI_(void) SetState(
        /*[in] */ CalculatorState state);

    CARAPI_(void) OnEquals();

    CARAPI_(void) OnDelete();

    CARAPI_(void) Reveal(
        /* [in] */ IView* sourceView,
        /* [in] */ Int32 colorRes,
        /* [in] */ IAnimatorListener* listener);

    CARAPI_(void) OnClear();

    CARAPI_(void) OnError(
        /* [in] */ Int32 errorResourceId);

    CARAPI_(void) OnResult(
        /* [in] */ const String& result);

private:
    static const String NAME;

    // instance state keys
    static const String KEY_CURRENT_STATE;
    static const String KEY_CURRENT_EXPRESSION;

    AutoPtr<ITextWatcher> mFormulaTextWatcher;
    AutoPtr<IViewOnKeyListener> mFormulaOnKeyListener;
    AutoPtr<IEditableFactory> mFormulaEditableFactory;

    CalculatorState mCurrentState;
    AutoPtr<ICalculatorExpressionTokenizer> mTokenizer;
    AutoPtr<ICalculatorExpressionEvaluator> mEvaluator;

    AutoPtr<IView> mDisplayView;
    AutoPtr<ICalculatorEditText> mFormulaEditText;
    AutoPtr<ICalculatorEditText> mResultEditText;
    AutoPtr<IViewPager> mPadViewPager;
    AutoPtr<IView> mDeleteButton;
    AutoPtr<IView> mEqualButton;
    AutoPtr<IView> mClearButton;

    AutoPtr<IView> mCurrentButton;
    AutoPtr<IAnimator> mCurrentAnimator;
};

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR3_CALCULATOR_H__