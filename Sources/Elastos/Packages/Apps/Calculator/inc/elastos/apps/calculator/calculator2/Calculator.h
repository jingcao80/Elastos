#ifndef _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOR_H__
#define _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOR_H__

#include "_Elastos.Apps.Calculator.Calculator2.h"
#include "elastos/apps/calculator/calculator2/CalculatorEditText.h"
#include "elastos/apps/calculator/calculator2/CalculatorExpressionEvaluator.h"
#include "elastos/apps/calculator/calculator2/CalculatorExpressionTokenizer.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Text.h>
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
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Os::IBundle;

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

class Calculator
    : public Activity
    , public IOnTextSizeChangeListener
    , public IEvaluateCallback
    , public IViewOnLongClickListener
    , public ICalculator
{
private:
    class MyTextWatcher
        : public Object
        , public ITextWatcher
    {
        friend class Calculator;

    public:
        MyTextWatcher(
            /* [in] */ Calculator* host);

        ~MyTextWatcher();

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
        , public IViewOnLongClickListener
    {
        friend class Calculator;

    public:
        MyOnKeyListener(
            /* [in] */ Calculator* host);

        ~MyOnKeyListener();

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

        ~MyEditableFactory();

        CAR_INTERFACE_DECL()

        CARAPI NewEditable(
            /* [in] */ ICharSequence* source,
            /* [out] */ IEditable** editable);

    private:
        Calculator* mHost;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        MyAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ IViewGroupOverlay* vgo,
            /* [in] */ IView* view);

        ~MyAnimatorListenerAdapter();

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        AutoPtr<IViewGroupOverlay> mvgo;
        AutoPtr<IView> mView;
    };

    class MySecondAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        MySecondAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ ICalculatorEditText* cet);

        ~MySecondAnimatorListenerAdapter();

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        AutoPtr<ICalculatorEditText> mCet;
    };

    class MyThirdAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        MyThirdAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ ICalculatorEditText* cet,
            /* [in] */ Int32 errorResourceId);

        ~MyThirdAnimatorListenerAdapter();

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        AutoPtr<ICalculatorEditText> mCet;
        Int32 mErrorResId;
    };

    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
        friend class Calculator;

    public:
        MyAnimatorUpdateListener(
            /* [in] */ Calculator* host,
            /* [in] */ ICalculatorEditText* cet);

        ~MyAnimatorUpdateListener();

        CAR_INTERFACE_DECL()

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* valueAnimator);

    private:
        Calculator* mHost;
        AutoPtr<ICalculatorEditText> mCet;
    };

    class MyFourthAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
        friend class Calculator;

    public:
        MyFourthAnimatorListenerAdapter(
            /* [in] */ Calculator* host,
            /* [in] */ ICalculatorEditText* cet1,
            /* [in] */ ICalculatorEditText* cet2,
            /* [in] */ IAnimator* animator,
            /* [in] */ const String& str,
            /* [in] */ Int32 color);

        ~MyFourthAnimatorListenerAdapter();

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Calculator* mHost;
        AutoPtr<ICalculatorEditText> mCet1;
        AutoPtr<ICalculatorEditText> mCet2;
        AutoPtr<IAnimator> mAnimator;
        String mResult;
        Int32 mColor;
    };

public:
    Calculator();

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
    AutoPtr<IViewOnLongClickListener> mFormulaOnKeyListener;
    AutoPtr<IEditableFactory> mFormulaEditableFactory;

    CalculatorState mCurrentState;
    AutoPtr<ICalculatorExpressionTokenizer> mTokenizer;
    static AutoPtr<ICalculatorExpressionEvaluator> mEvaluator;

    AutoPtr<IView> mDisplayView;
    static AutoPtr<ICalculatorEditText> mFormulaEditText;
    AutoPtr<ICalculatorEditText> mResultEditText;
    // assert(0 && "TODO");
    // ViewPager mPadViewPager;
    AutoPtr<IView> mDeleteButton;
    AutoPtr<IView> mEqualButton;
    AutoPtr<IView> mClearButton;

    AutoPtr<IView> mCurrentButton;
    AutoPtr<IAnimator> mCurrentAnimator;
};

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos

#endif // _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOR_H__