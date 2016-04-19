#include "elastos/droid/calculator2/Calculator.h"
#include "elastos/droid/calculator2/CalculatorExpressionBuilder.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CArgbEvaluator;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITypeEvaluator;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorSetBuilder;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::CBundleHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBundleHelper;
using Elastos::Droid::Text::EIID_IEditableFactory;
using Elastos::Droid::Text::EIID_ITextWatcher;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::CViewAnimationUtilsHelper;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IViewAnimationUtilsHelper;
using Elastos::Droid::View::IViewGroupOverlay;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::CView;

using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Calculator2 {

const String Calculator::NAME(String("Calculator"));

// instance state keys
const String Calculator::KEY_CURRENT_STATE = NAME + "_currentState";
const String Calculator::KEY_CURRENT_EXPRESSION = NAME + "_currentExpression";

AutoPtr<ICalculatorExpressionEvaluator> Calculator::mEvaluator;
AutoPtr<ICalculatorEditText> Calculator::mFormulaEditText;

//----------------------------------------------------------------
//           Calculator::MyTextWatcher
//----------------------------------------------------------------
Calculator::MyTextWatcher::MyTextWatcher(
    /* [in] */ Calculator* host)
    : mHost(host)
{}


Calculator::MyTextWatcher::~MyTextWatcher()
{}

CAR_INTERFACE_IMPL(Calculator::MyTextWatcher, Object, ITextWatcher)

ECode Calculator::MyTextWatcher::BeforeTextChanged(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode Calculator::MyTextWatcher::OnTextChanged(
    /* [in] */ ICharSequence* charSequence,
    /* [in] */ Int32 start,
    /* [in] */ Int32 count,
    /* [in] */ Int32 after)
{
    return NOERROR;
}

ECode Calculator::MyTextWatcher::AfterTextChanged(
    /* [in] */ IEditable* editable)
{
    mHost->SetState(INPUT);
    return mEvaluator->Evaluate(ICharSequence::Probe(editable), IEvaluateCallback::Probe(this));
}

//----------------------------------------------------------------
//           Calculator::MyOnKeyListener
//----------------------------------------------------------------
Calculator::MyOnKeyListener::MyOnKeyListener(
    /* [in] */ Calculator* host)
    : mHost(host)
{}

Calculator::MyOnKeyListener::~MyOnKeyListener()
{}

CAR_INTERFACE_IMPL(Calculator::MyOnKeyListener, Object, IViewOnLongClickListener)

ECode Calculator::MyOnKeyListener::OnKey(
    /* [in] */ IView* view,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* keyEvent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_NUMPAD_ENTER:
        case IKeyEvent::KEYCODE_ENTER:
            {
                Int32 action;
                keyEvent->GetAction(&action);
                if (action == IKeyEvent::ACTION_UP) {
                    mHost->mCurrentButton = mHost->mEqualButton;
                    mHost->OnEquals();
                }
                // ignore all other actions
                *result = TRUE;
                return NOERROR;
            }
    }
    *result = FALSE;
    return NOERROR;
}

//----------------------------------------------------------------
//           Calculator::MyEditableFactory
//----------------------------------------------------------------

Calculator::MyEditableFactory::MyEditableFactory(
    /* [in] */ Calculator* host)
    : mHost(host)
{}

Calculator::MyEditableFactory::~MyEditableFactory()
{}

CAR_INTERFACE_IMPL(Calculator::MyEditableFactory, Object, IEditableFactory)

ECode Calculator::MyEditableFactory::NewEditable(
    /* [in] */ ICharSequence* source,
    /* [out] */ IEditable** editable)
{
    VALIDATE_NOT_NULL(editable);

    Boolean isEdited = mHost->mCurrentState == INPUT
                    || mHost->mCurrentState == ERROR;
    AutoPtr<CalculatorExpressionBuilder> etl = new CalculatorExpressionBuilder();
    etl->constructor(source, ICalculatorExpressionTokenizer::Probe(mHost->mTokenizer), isEdited);
    *editable = IEditable::Probe(etl);
    REFCOUNT_ADD(*editable);
    return NOERROR;
}

//----------------------------------------------------------------
//           Calculator::MyAnimatorListenerAdapter
//----------------------------------------------------------------
Calculator::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ Calculator* host,
    /* [in] */ IViewGroupOverlay* vgo,
    /* [in] */ IView* view)
    : mHost(host)
    , mvgo(vgo)
    , mView(view)
{}

Calculator::MyAnimatorListenerAdapter::~MyAnimatorListenerAdapter()
{}

ECode Calculator::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mvgo->Remove(mView);
    mHost->mCurrentAnimator = NULL;
    return NOERROR;
}

//----------------------------------------------------------------
//           Calculator::MySecondAnimatorListenerAdapter
//----------------------------------------------------------------
Calculator::MySecondAnimatorListenerAdapter::MySecondAnimatorListenerAdapter(
    /* [in] */ Calculator* host,
    /* [in] */ ICalculatorEditText* cet)
    : mHost(host)
    , mCet(cet)
{}

Calculator::MySecondAnimatorListenerAdapter::~MySecondAnimatorListenerAdapter()
{}

ECode Calculator::MySecondAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{

    AutoPtr<IEditable> editable;
    ITextView::Probe(mFormulaEditText)->GetEditableText((IEditable**)&editable);
    return editable->Clear();
}

//----------------------------------------------------------------
//           Calculator::MyThirdAnimatorListenerAdapter
//----------------------------------------------------------------
Calculator::MyThirdAnimatorListenerAdapter::MyThirdAnimatorListenerAdapter(
    /* [in] */ Calculator* host,
    /* [in] */ ICalculatorEditText* cet,
    /* [in] */ Int32 errorResourceId)
    : mHost(host)
    , mCet(cet)
    , mErrorResId(errorResourceId)
{}

Calculator::MyThirdAnimatorListenerAdapter::~MyThirdAnimatorListenerAdapter()
{}

ECode Calculator::MyThirdAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->SetState(ERROR);
    return ITextView::Probe(mHost->mResultEditText)->SetText(StringUtils::ParseCharSequence(StringUtils::ToString(mErrorResId)).Get());
}

//----------------------------------------------------------------
//           Calculator::MyAnimatorUpdateListener
//----------------------------------------------------------------
Calculator::MyAnimatorUpdateListener::MyAnimatorUpdateListener(
    /* [in] */ Calculator* host,
    /* [in] */ ICalculatorEditText* cet)
    : mHost(host)
    , mCet(cet)
{}

Calculator::MyAnimatorUpdateListener::~MyAnimatorUpdateListener()
{}

CAR_INTERFACE_IMPL(Calculator::MyAnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode Calculator::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* valueAnimator)
{
    AutoPtr<IInterface> obj;
    valueAnimator->GetAnimatedValue((IInterface**)&obj);
    AutoPtr<IInteger32> ivol = IInteger32::Probe(obj);
    Int32 vol;
    ivol->GetValue(&vol);
    return ITextView::Probe(mHost->mResultEditText)->SetTextColor(vol);
}

//----------------------------------------------------------------
//           Calculator::MyFourthAnimatorListenerAdapter
//----------------------------------------------------------------
Calculator::MyFourthAnimatorListenerAdapter::MyFourthAnimatorListenerAdapter(
    /* [in] */ Calculator* host,
    /* [in] */ ICalculatorEditText* cet1,
    /* [in] */ ICalculatorEditText* cet2,
    /* [in] */ IAnimator* animator,
    /* [in] */ const String& str,
    /* [in] */ Int32 color)
    : mHost(host)
    , mCet1(cet1)
    , mCet2(cet2)
    , mAnimator(animator)
    , mResult(str)
    , mColor(color)
{}

Calculator::MyFourthAnimatorListenerAdapter::~MyFourthAnimatorListenerAdapter()
{}

ECode Calculator::MyFourthAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    ITextView::Probe(mHost->mResultEditText)->SetText(StringUtils::ParseCharSequence(mResult).Get());
}

ECode Calculator::MyFourthAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    // Reset all of the values modified during the animation.
    ITextView::Probe(mHost->mResultEditText)->SetTextColor(mColor);
    IView::Probe(mHost->mResultEditText)->SetScaleX(1.0f);
    IView::Probe(mHost->mResultEditText)->SetScaleY(1.0f);
    IView::Probe(mHost->mResultEditText)->SetTranslationX(0.0f);
    IView::Probe(mHost->mResultEditText)->SetTranslationY(0.0f);
    IView::Probe(mHost->mFormulaEditText)->SetTranslationY(0.0f);

    // Finally update the formula to use the current result.
    ITextView::Probe(mHost->mResultEditText)->SetText(StringUtils::ParseCharSequence(mResult).Get());
    mHost->SetState(RESULT);

    mHost->mCurrentAnimator = NULL;
    return NOERROR;
}
//----------------------------------------------------------------
//           Calculator
//----------------------------------------------------------------
Calculator::Calculator()
{}

Calculator::~Calculator()
{}

CAR_INTERFACE_IMPL_4(Calculator, Activity, IOnTextSizeChangeListener, IEvaluateCallback, IViewOnLongClickListener, ICalculator)

ECode Calculator::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_calculator);

    mDisplayView = FindViewById(R::id::display);
    mFormulaEditText = ICalculatorEditText::Probe(FindViewById(R::id::formula));
    mResultEditText = ICalculatorEditText::Probe(FindViewById(R::id::result));
    // mPadViewPager = (ViewPager) FindViewById(R::id::pad_pager);
    mDeleteButton = FindViewById(R::id::del);
    mClearButton = FindViewById(R::id::clr);

    AutoPtr<IView> tmpView;
    tmpView = FindViewById(R::id::pad_numeric);
    tmpView->FindViewById(R::id::eq, (IView**)&mEqualButton);
    Int32 visibility;
    mEqualButton->GetVisibility(&visibility);
    if (mEqualButton == NULL || visibility != IView::VISIBLE) {
        tmpView = NULL;
        tmpView = FindViewById(R::id::pad_operator);
        tmpView->FindViewById(R::id::eq, (IView**)&mEqualButton);
    }

    AutoPtr<CalculatorExpressionTokenizer> cet = new CalculatorExpressionTokenizer();
    cet->constructor(IContext::Probe(this));
    mTokenizer = ICalculatorExpressionTokenizer::Probe(cet);

    AutoPtr<CalculatorExpressionEvaluator> elr = new CalculatorExpressionEvaluator();
    elr->constructor(ICalculatorExpressionTokenizer::Probe(mTokenizer));
    mEvaluator = ICalculatorExpressionEvaluator::Probe(elr);

    AutoPtr<IBundleHelper> bhl;
    CBundleHelper::AcquireSingleton((IBundleHelper**)&bhl);
    AutoPtr<IBundle> empty;
    bhl->GetEMPTY((IBundle**)&empty);
    savedInstanceState = savedInstanceState == NULL ? empty.Get() : savedInstanceState;
    Int32 vol;
    savedInstanceState->GetInt32(KEY_CURRENT_STATE, INPUT, &vol);
    SetState(vol);
    String str;
    savedInstanceState->GetString(KEY_CURRENT_EXPRESSION, String(""), &str);
    String text;
    mTokenizer->GetLocalizedExpression(str, &text);
    ITextView::Probe(mFormulaEditText)->SetText(StringUtils::ParseCharSequence(text).Get());
    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mFormulaEditText)->GetText((ICharSequence**)&cs);
    mEvaluator->Evaluate(cs.Get(), IEvaluateCallback::Probe(this));

    ITextView::Probe(mFormulaEditText.Get())->SetEditableFactory(mFormulaEditableFactory.Get());
    ITextView::Probe(mFormulaEditText.Get())->AddTextChangedListener(mFormulaTextWatcher.Get());
    IView::Probe(mFormulaEditText.Get())->SetOnKeyListener(IViewOnKeyListener::Probe(mFormulaOnKeyListener));
    mFormulaEditText->SetOnTextSizeChangeListener(IOnTextSizeChangeListener::Probe(this));
    return mDeleteButton->SetOnLongClickListener(IViewOnLongClickListener::Probe(this));
}

ECode Calculator::OnSaveInstanceState(
    /*[in] */ /*@NonNull */IBundle* outState)
{
    VALIDATE_NOT_NULL(outState);

    // If there's an animation in progress, cancel it first to ensure our state is up-to-date.
    if (mCurrentAnimator != NULL) {
        mCurrentAnimator->Cancel();
    }

    OnSaveInstanceState(outState);

    outState->PutInt32(KEY_CURRENT_STATE, mCurrentState);
    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mFormulaEditText.Get())->GetText((ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    String ret;
    mTokenizer->GetNormalizedExpression(str, &ret);
    return outState->PutString(KEY_CURRENT_EXPRESSION, ret);
}

void Calculator::SetState(
    /*[in] */ CalculatorState state)
{
    if (mCurrentState != state) {
        mCurrentState = state;

        if (state == RESULT || state == ERROR) {
            mDeleteButton->SetVisibility(IView::GONE);
            mClearButton->SetVisibility(IView::VISIBLE);
        }
        else {
            mDeleteButton->SetVisibility(IView::VISIBLE);
            mClearButton->SetVisibility(IView::GONE);
        }

        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        AutoPtr<IWindow> window;
        GetWindow((IWindow**)&window);
        Int32 errorColor;
        if (state == ERROR) {
            res->GetColor(R::color::calculator_error_color, &errorColor);
            ITextView::Probe(mFormulaEditText)->SetTextColor(errorColor);
            ITextView::Probe(mResultEditText)->SetTextColor(errorColor);
            window->SetStatusBarColor(errorColor);
        }
        else {
            res->GetColor(R::color::display_formula_text_color, &errorColor);
            ITextView::Probe(mFormulaEditText)->SetTextColor(errorColor);
            res->GetColor(R::color::calculator_error_color, &errorColor);
            ITextView::Probe(mResultEditText)->SetTextColor(errorColor);
            res->GetColor(R::color::calculator_accent_color, &errorColor);
            window->SetStatusBarColor(errorColor);
        }
    }
}

ECode Calculator::OnBackPressed()
{
    assert(0 && "TODO"); // support.v4
/*    if (mPadViewPager == null || mPadViewPager.getCurrentItem() == 0) {
        // If the user is currently looking at the first pad (or the pad is not paged),
        // allow the system to handle the Back button.
        super.onBackPressed();
    } else {
        // Otherwise, select the previous pad.
        mPadViewPager.setCurrentItem(mPadViewPager.getCurrentItem() - 1);
    }*/
}

ECode Calculator::OnUserInteraction()
{
    OnUserInteraction();

    // If there's an animation in progress, cancel it so the user interaction can be handled
    // immediately.
    if (mCurrentAnimator != NULL) {
        mCurrentAnimator->Cancel();
    }
}

ECode Calculator::OnButtonClick(
    /* [in] */ IView* iview)
{
    mCurrentButton = iview;

    Int32 id;
    iview->GetId(&id);
    switch (id) {
        case R::id::eq:
            OnEquals();
            break;
        case R::id::del:
            OnDelete();
            break;
        case R::id::clr:
            OnClear();
            break;
        case R::id::fun_cos:
        case R::id::fun_ln:
        case R::id::fun_log:
        case R::id::fun_sin:
        case R::id::fun_tan:
            {
                // Add left parenthesis after functions.
                AutoPtr<ICharSequence> cs;
                ITextView::Probe(iview)->GetText((ICharSequence**)&cs);
                ITextView::Probe(mFormulaEditText)->Append(cs);
                ITextView::Probe(mFormulaEditText)->Append(StringUtils::ParseCharSequence(String("(")).Get());
                break;
            }

        default:
            {
                AutoPtr<ICharSequence> cs;
                ITextView::Probe(iview)->GetText((ICharSequence**)&cs);
                ITextView::Probe(mFormulaEditText)->Append(cs);
                break;
            }
    }
    return NOERROR;
}

ECode Calculator::OnLongClick(
    /* [in] */ IView* iview,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    mCurrentButton = iview;

    Int32 id;
    iview->GetId(&id);
    if (id == R::id::del) {
        OnClear();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Calculator::OnEvaluate(
    /* [in] */ const String& expr,
    /* [in] */ const String& result,
    /* [in] */ Int32 errorResourceId)
{
    if (mCurrentState == INPUT) {
        ITextView::Probe(mResultEditText)->SetText(StringUtils::ParseCharSequence(result).Get());
    }
    else if (errorResourceId != INVALID_RES_ID) {
        OnError(errorResourceId);
    }
    else if (!TextUtils::IsEmpty(result)) {
        OnResult(result);
    }
    else if (mCurrentState == EVALUATE) {
        // The current expression cannot be evaluated -> return to the input state.
        SetState(INPUT);
    }

    Boolean flag = FALSE;
    return IView::Probe(mFormulaEditText)->RequestFocus(&flag);
}

ECode Calculator::OnTextSizeChanged(
    /* [in] */ ITextView* textView,
    /* [in] */ Float oldSize)
{
    if (mCurrentState != INPUT) {
        // Only animate text changes that occur from user input.
        return NOERROR;
    }

    // Calculate the values needed to perform the scale and translation animations,
    // maintaining the same apparent baseline for the displayed text.
    Float textSize;
    textView->GetTextSize(&textSize);
    Float textScale = oldSize / textSize;

    Int32 width;
    IView::Probe(textView)->GetWidth(&width);

    Int32 paddingEnd;
    IView::Probe(textView)->GetPaddingEnd(&paddingEnd);
    Float translationX = (1.0f - textScale) * (width / 2.0f - paddingEnd);

    Int32 height;
    IView::Probe(textView)->GetHeight(&height);

    Int32 bottom;
    IView::Probe(textView)->GetPaddingBottom(&bottom);
    Float translationY = (1.0f - textScale) * (height / 2.0f - bottom);

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IObjectAnimatorHelper> oah;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oah);

    AutoPtr<ArrayOf<Float> > arr1 = ArrayOf<Float>::Alloc(2);
    (*arr1)[0] = textScale;
    (*arr1)[1] = 1.0;

    AutoPtr<ArrayOf<Float> > arr2 = ArrayOf<Float>::Alloc(2);
    (*arr2)[0] = translationX;
    (*arr2)[1] = 0.0;

    AutoPtr<ArrayOf<Float> > arr3 = ArrayOf<Float>::Alloc(2);
    (*arr2)[0] = translationY;
    (*arr2)[1] = 0.0;

    AutoPtr<IObjectAnimator> o1;
    oah->OfFloat(textView, Elastos::Droid::View::View::SCALE_X.Get(), arr1.Get(), (IObjectAnimator**)&o1);
    AutoPtr<IObjectAnimator> o2;
    oah->OfFloat(textView, Elastos::Droid::View::View::SCALE_Y.Get(), arr1.Get(), (IObjectAnimator**)&o2);
    AutoPtr<IObjectAnimator> o3;
    oah->OfFloat(textView, Elastos::Droid::View::View::TRANSLATION_X.Get(), arr2.Get(), (IObjectAnimator**)&o3);
    AutoPtr<IObjectAnimator> o4;
    oah->OfFloat(textView, Elastos::Droid::View::View::TRANSLATION_Y.Get(), arr3.Get(), (IObjectAnimator**)&o4);

    AutoPtr<ArrayOf<IAnimator*> > arr4 =  ArrayOf<IAnimator*>::Alloc(4);
    (*arr4)[0] = IAnimator::Probe(o1);
    (*arr4)[1] = IAnimator::Probe(o2);
    (*arr4)[2] = IAnimator::Probe(o3);
    (*arr4)[3] = IAnimator::Probe(o4);

    animatorSet->PlayTogether(arr4.Get());

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 integer;
    res->GetInteger(Elastos::Droid::R::integer::config_mediumAnimTime, &integer);
    IAnimator::Probe(animatorSet)->SetDuration((Int64)integer);
    AutoPtr<ITimeInterpolator> tip;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&tip);
    IAnimator::Probe(animatorSet)->SetInterpolator(tip.Get());
    return IAnimator::Probe(animatorSet)->Start();
}

void Calculator::OnEquals()
{
    if (mCurrentState == INPUT) {
        SetState(EVALUATE);
        AutoPtr<ICharSequence> cs;
        ITextView::Probe(mFormulaEditText)->GetText((ICharSequence**)&cs);
        mEvaluator->Evaluate(cs.Get(), IEvaluateCallback::Probe(this));
    }
}

void Calculator::OnDelete()
{
    // Delete works like backspace; remove the last character from the expression.
    AutoPtr<IEditable> formulaText;
    ITextView::Probe(mFormulaEditText)->GetEditableText((IEditable**)&formulaText);
    Int32 formulaLength;
    ICharSequence::Probe(formulaText)->GetLength(&formulaLength);
    if (formulaLength > 0) {
        formulaText->Delete(formulaLength - 1, formulaLength);
    }
}

void Calculator::Reveal(
    /* [in] */ IView* sourceView,
    /* [in] */ Int32 colorRes,
    /* [in] */ IAnimatorListener* listener)
{
    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    AutoPtr<IView> dView;
    window->GetDecorView((IView**)&dView);
    AutoPtr<IViewOverlay> overlay;
    dView->GetOverlay((IViewOverlay**)&overlay);
    AutoPtr<IViewGroupOverlay> groupOverlay = IViewGroupOverlay::Probe(overlay);

    AutoPtr<IRect> displayRect;
    CRect::New((IRect**)&displayRect);
    Boolean flag = FALSE;
    mDisplayView->GetGlobalVisibleRect(displayRect, &flag);

    // Make reveal cover the display and status bar.
    AutoPtr<IView> revealView;
    CView::New(IContext::Probe(this), (IView**)&revealView);
    Int32 bottom;
    displayRect->GetBottom(&bottom);
    revealView->SetBottom(bottom);
    Int32 left;
    displayRect->GetLeft(&left);
    revealView->SetLeft(left);
    Int32 right;
    displayRect->GetRight(&right);
    revealView->SetRight(right);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 color;
    res->GetColor(colorRes, &color);
    revealView->SetBackgroundColor(color);
    groupOverlay->Add(revealView.Get());

    AutoPtr<ArrayOf<Int32> > clearLocation = ArrayOf<Int32>::Alloc(2);
    sourceView->GetLocationInWindow(clearLocation.Get());
    Int32 width;
    sourceView->GetWidth(&width);
    Int32 height;
    sourceView->GetHeight(&height);
    (*clearLocation)[0] += width / 2;
    (*clearLocation)[1] += height / 2;

    Int32 revealCenterX = (*clearLocation)[0] - left;
    Int32 top;
    revealView->GetTop(&top);
    Int32 revealCenterY = (*clearLocation)[1] - top;

    Double x1_2 = Elastos::Core::Math::Pow((Double)(left - revealCenterX), (Double)2);
    Double x2_2 = Elastos::Core::Math::Pow((Double)(right - revealCenterX), (Double)2);
    Double y_2 = Elastos::Core::Math::Pow((Double)(top - revealCenterY), (Double)2);
    Float revealRadius = (Float)Elastos::Core::Math::Max(Elastos::Core::Math::Sqrt(x1_2 + y_2), Elastos::Core::Math::Sqrt(x2_2 + y_2));

    AutoPtr<IViewAnimationUtilsHelper> vau;
    CViewAnimationUtilsHelper::AcquireSingleton((IViewAnimationUtilsHelper**)&vau);
    AutoPtr<IAnimator> revealAnimator;
    vau->CreateCircularReveal(
        revealView.Get(), revealCenterX, revealCenterY, 0.0f, revealRadius, (IAnimator**)&revealAnimator);

    Int32 integer;
    res->GetInteger(Elastos::Droid::R::integer::config_longAnimTime, &integer);
    revealAnimator->SetDuration((Int64)integer);
    revealAnimator->AddListener(listener);

    AutoPtr<IObjectAnimatorHelper> oah;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oah);

    AutoPtr<ArrayOf<Float> > arr = ArrayOf<Float>::Alloc(1);
    (*arr)[0] = 0.0f;
    AutoPtr<IObjectAnimator> oba;
    oah->OfFloat(revealView.Get(), Elastos::Droid::View::View::ALPHA.Get(), arr.Get(), (IObjectAnimator**)&oba);
    AutoPtr<IAnimator> alphaAnimator = IAnimator::Probe(oba);
    res->GetInteger(Elastos::Droid::R::integer::config_mediumAnimTime, &integer);
    alphaAnimator->SetDuration((Int64)(integer));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IAnimatorSetBuilder> asb;
    animatorSet->Play(revealAnimator.Get(), (IAnimatorSetBuilder**)&asb);
    asb->Before(alphaAnimator);
    AutoPtr<ITimeInterpolator> tip;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&tip);
    IAnimator::Probe(animatorSet)->SetInterpolator(tip.Get());

    AutoPtr<MyAnimatorListenerAdapter> myala =
        new MyAnimatorListenerAdapter(this, groupOverlay.Get(), revealView.Get());
    IAnimator::Probe(animatorSet)->AddListener(IAnimatorListener::Probe(myala));

    mCurrentAnimator = IAnimator::Probe(animatorSet);
    IAnimator::Probe(animatorSet)->Start();
}

void Calculator::OnClear()
{
    AutoPtr<ICharSequence> cs;
    ITextView::Probe(mFormulaEditText.Get())->GetText((ICharSequence**)&cs);

    if (TextUtils::IsEmpty(cs.Get())) {
        return;
    }

    AutoPtr<MySecondAnimatorListenerAdapter> msal = new MySecondAnimatorListenerAdapter(this, mFormulaEditText);
    Reveal(mCurrentButton.Get(), R::color::calculator_accent_color, IAnimatorListener::Probe(msal));
}

void Calculator::OnError(
    /* [in] */ Int32 errorResourceId)
{
    if (mCurrentState != EVALUATE) {
        // Only animate error on evaluate.
        ITextView::Probe(mResultEditText)->SetText(errorResourceId);
        return;
    }

    AutoPtr<MyThirdAnimatorListenerAdapter> mal =
        new MyThirdAnimatorListenerAdapter(this, mResultEditText, errorResourceId);
    Reveal(mCurrentButton.Get(), R::color::calculator_error_color, IAnimatorListener::Probe(mal));
}

void Calculator::OnResult(
    /* [in] */ const String& result)
{
    // Calculate the values needed to perform the scale and translation animations,
    // accounting for how the scale will affect the final position of the text.
    Float vts;
    ((CalculatorEditText*)mFormulaEditText.Get())->GetVariableTextSize(result, &vts);
    Float ts;
    ITextView::Probe(mResultEditText)->GetTextSize(&ts);
    Float resultScale;
    resultScale = (Float)(vts / ts);
    Int32 width;
    IView::Probe(mResultEditText)->GetWidth(&width);
    Int32 paddingEnd;
    IView::Probe(mResultEditText)->GetPaddingEnd(&paddingEnd);
    Float resultTranslationX = (1.0f - resultScale) * (width / 2.0f - paddingEnd);

    Int32 height;
    IView::Probe(mResultEditText)->GetHeight(&height);
    Int32 paddingBottom;
    IView::Probe(mResultEditText)->GetPaddingBottom(&paddingBottom);

    Int32 bottom;
    IView::Probe(mFormulaEditText)->GetBottom(&bottom);
    Int32 rbottom;
    IView::Probe(mResultEditText)->GetBottom(&rbottom);

    Int32 fpaddingBottom;
    IView::Probe(mFormulaEditText)->GetPaddingBottom(&fpaddingBottom);

    Float resultTranslationY = (1.0f - resultScale) *
            (height / 2.0f - paddingBottom) +
            (bottom - rbottom) +
            (paddingBottom - fpaddingBottom);

    Float formulaTranslationY = -bottom;

    // Use a value animator to fade to the final text color over the course of the animation.
    Int32 resultTextColor;
    ITextView::Probe(mResultEditText)->GetCurrentTextColor(&resultTextColor);
    Int32 formulaTextColor;
    ITextView::Probe(mFormulaEditText)->GetCurrentTextColor(&formulaTextColor);

    AutoPtr<IValueAnimatorHelper> vah;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vah);
    AutoPtr<ITypeEvaluator> te;
    CArgbEvaluator::New((ITypeEvaluator**)&te);

    AutoPtr<ArrayOf<IInterface*> > arr =  ArrayOf<IInterface*>::Alloc(2);
    (*arr)[0] = TO_IINTERFACE(CoreUtils::Convert(resultTextColor));
    (*arr)[1] = TO_IINTERFACE(CoreUtils::Convert(formulaTextColor));
    AutoPtr<IValueAnimator> textColorAnimator;
    vah->OfObject(te.Get(), arr.Get(), (IValueAnimator**)&textColorAnimator);

    AutoPtr<MyAnimatorUpdateListener> maul = new MyAnimatorUpdateListener(this, mResultEditText.Get());
    textColorAnimator->AddUpdateListener(IAnimatorUpdateListener::Probe(maul));

    AutoPtr<IAnimatorSet> animatorSet;
    CAnimatorSet::New((IAnimatorSet**)&animatorSet);
    AutoPtr<IObjectAnimatorHelper> oah;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oah);
    AutoPtr<ArrayOf<Float> > arr1 = ArrayOf<Float>::Alloc(1);
    (*arr1)[0] = resultScale;

    AutoPtr<ArrayOf<Float> > arr2 = ArrayOf<Float>::Alloc(1);
    (*arr2)[0] = resultTranslationX;

    AutoPtr<ArrayOf<Float> > arr3 = ArrayOf<Float>::Alloc(1);
    (*arr3)[0] = resultTranslationY;

    AutoPtr<ArrayOf<Float> > arr4 = ArrayOf<Float>::Alloc(1);
    (*arr4)[0] = formulaTranslationY;

    AutoPtr<ArrayOf<IAnimator*> > animators = ArrayOf<IAnimator*>::Alloc(4);
    (*animators)[0] = IAnimator::Probe(textColorAnimator);
    AutoPtr<IObjectAnimator> ioa;
    oah->OfFloat(ICalculatorEditText::Probe(mResultEditText), Elastos::Droid::View::View::SCALE_X.Get(), arr1.Get(), (IObjectAnimator**)&ioa);
    (*animators)[1] = IAnimator::Probe(ioa);
    ioa = NULL;
    oah->OfFloat(ICalculatorEditText::Probe(mResultEditText), Elastos::Droid::View::View::SCALE_Y.Get(), arr1.Get(), (IObjectAnimator**)&ioa);
    (*animators)[2] = IAnimator::Probe(ioa);
    ioa = NULL;
    oah->OfFloat(ICalculatorEditText::Probe(mResultEditText), Elastos::Droid::View::View::TRANSLATION_X.Get(), arr2.Get(), (IObjectAnimator**)&ioa);
    (*animators)[3] = IAnimator::Probe(ioa);
    ioa = NULL;
    oah->OfFloat(ICalculatorEditText::Probe(mFormulaEditText), Elastos::Droid::View::View::TRANSLATION_Y.Get(), arr3.Get(), (IObjectAnimator**)&ioa);
    (*animators)[4] = IAnimator::Probe(ioa);
    ioa = NULL;
    oah->OfFloat(ICalculatorEditText::Probe(mFormulaEditText), Elastos::Droid::View::View::TRANSLATION_Y.Get(), arr4.Get(), (IObjectAnimator**)&ioa);
    (*animators)[5] = IAnimator::Probe(ioa);
    animatorSet->PlayTogether(animators.Get());

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 vol;
    res->GetInteger(Elastos::Droid::R::integer::config_longAnimTime, &vol);
    IAnimator::Probe(animatorSet)->SetDuration((Int64)vol);

    AutoPtr<ITimeInterpolator> tip;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&tip);
    IAnimator::Probe(animatorSet)->SetInterpolator(tip.Get());

    AutoPtr<MyFourthAnimatorListenerAdapter> mfal =
        new MyFourthAnimatorListenerAdapter(this, mResultEditText.Get(), mFormulaEditText.Get(), mCurrentAnimator.Get(), result, resultTextColor);
    IAnimator::Probe(animatorSet)->AddListener(IAnimatorListener::Probe(mfal));

    mCurrentAnimator = IAnimator::Probe(animatorSet);
    IAnimator::Probe(animatorSet)->Start();
}

} // namespace Calculator2
} // namespace Droid
} // namespace Elastos