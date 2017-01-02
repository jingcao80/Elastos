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

#ifndef _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__
#define _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__

#include "_Elastos.Droid.Calculator2.h"
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

namespace Elastos {
namespace Droid {
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
        MyActionModeCallback();

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
    };

public:
    CalculatorEditText();

    virtual ~CalculatorEditText();

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
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 lengthBefore,
        /* [in] */ Int32 lengthAfter);

private:
    static CARAPI_(AutoPtr<MyActionModeCallback>) InitCallback();

private:
    static const AutoPtr<MyActionModeCallback> NO_SELECTION_ACTION_MODE_CALLBACK;
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
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_CALCULATOR2_CALCULATOREDITTEXT_H__