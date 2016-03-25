#ifndef _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREDITTEXT_H__
#define _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREDITTEXT_H__

#include "_Elastos.Apps.Calculator.Calculator2.h"
#include <elastos/droid/widget/EditText.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IActionModeCallback;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::EditText;

namespace Elastos{
namespace Apps {
namespace Calculator {
namespace Calculator2 {

class CalculatorEditText
    : public EditText
    , public ICalculatorEditText
{
private:
    class MyActionModeCallback
        : public Object
        , public IActionModeCallback
    {
    public:
        MyActionModeCallback(
            /* [in] */ CalculatorEditText* host);

        ~MyActionModeCallback();

        CAR_INTERFACE_DECL()

        CARAPI OnActionItemClicked(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* result);

        CARAPI OnCreateActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

        CARAPI OnDestroyActionMode(
            /* [in] */ IActionMode* mode);

        CARAPI OnPrepareActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* result);

    private:
        CalculatorEditText* mHost;
    };

public:
    CalculatorEditText();

    ~CalculatorEditText();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable* result);

    CARAPI SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);

    CARAPI SetOnTextSizeChangeListener(
        /* [in] */ IOnTextSizeChangeListener* listener);

    CARAPI GetVariableTextSize(
        /* [in] */ const String& text,
        /* [out] */ Float* result);

    CARAPI GetCompoundPaddingTop(
        /* [out] */ Int32* result);

    CARAPI GetCompoundPaddingBottom(
        /* [out] */ Int32* result);

protected:
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

private:
    AutoPtr<MyActionModeCallback> NO_SELECTION_ACTION_MODE_CALLBACK;
    Float mMaximumTextSize;
    Float mMinimumTextSize;
    Float mStepTextSize;

    // Temporary objects for use in layout methods.
    AutoPtr<IPaint> mTempPaint;
    AutoPtr<IRect> mTempRect;

    Int32 mWidthConstraint;
    AutoPtr<IOnTextSizeChangeListener> mOnTextSizeChangeListener;
};

} // namespace Calculator2
} // namespace Calculator
} // namespace Apps
} // namespace Elastos

#endif // _ELASTOS_APPS_CALCULATOR_CALCULATOR2_CALCULATOREDITTEXT_H__