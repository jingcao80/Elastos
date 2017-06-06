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

#include "elastos/droid/calculator3/CalculatorEditText.h"
#include "R.h"
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Text.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::Method::CScrollingMovementMethodHelper;
using Elastos::Droid::Text::Method::IScrollingMovementMethodHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace Calculator3 {
//----------------------------------------------------------------
//           CalculatorEditText::MyActionModeCallback
//----------------------------------------------------------------
CalculatorEditText::MyActionModeCallback::MyActionModeCallback()
{}

CAR_INTERFACE_IMPL(CalculatorEditText::MyActionModeCallback, Object, IActionModeCallback)

ECode CalculatorEditText::MyActionModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CalculatorEditText::MyActionModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode CalculatorEditText::MyActionModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

ECode CalculatorEditText::MyActionModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

//----------------------------------------------------------------
//           CalculatorEditText
//----------------------------------------------------------------

AutoPtr<CalculatorEditText::MyActionModeCallback> CalculatorEditText::InitCallback()
{
    return new MyActionModeCallback();
}
const AutoPtr<CalculatorEditText::MyActionModeCallback> CalculatorEditText::NO_SELECTION_ACTION_MODE_CALLBACK = InitCallback();

CalculatorEditText::CalculatorEditText()
    : mWidthConstraint(-1)
{
    CTextPaint::New((IPaint**)&mTempPaint);
    CRect::New((IRect**)&mTempRect);
}

CalculatorEditText::~CalculatorEditText()
{}

CAR_INTERFACE_IMPL(CalculatorEditText, EditText, ICalculatorEditText)

ECode CalculatorEditText::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CalculatorEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CalculatorEditText::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    EditText::constructor(context, attrs, defStyle);

    AutoPtr<IAttributeSet> attbs = attrs;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::CalculatorEditText);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attbs.Get(), attrIds.Get(), defStyle, 0, (ITypedArray**)&a);

    Float textsize;
    GetTextSize(&textsize);
    a->GetDimension(
            R::styleable::CalculatorEditText_maxTextSize, textsize, &mMaximumTextSize);
    a->GetDimension(
            R::styleable::CalculatorEditText_minTextSize, textsize, &mMinimumTextSize);
    a->GetDimension(R::styleable::CalculatorEditText_stepTextSize,
            (mMaximumTextSize - mMinimumTextSize) / 3, &mStepTextSize);

    a->Recycle();

    SetCustomSelectionActionModeCallback(
        IActionModeCallback::Probe(NO_SELECTION_ACTION_MODE_CALLBACK));
    Boolean flag = FALSE;
    IsFocusable(&flag);
    if (flag) {
        AutoPtr<IScrollingMovementMethodHelper> smm;
        CScrollingMovementMethodHelper::AcquireSingleton((IScrollingMovementMethodHelper**)&smm);
        AutoPtr<IMovementMethod> mmd;
        smm->GetInstance((IMovementMethod**)&mmd);
        SetMovementMethod(mmd.Get());
    }
    SetTextSize(ITypedValue::COMPLEX_UNIT_PX, mMaximumTextSize);
    Int32 cpaddingBottom;
    GetCompoundPaddingBottom(&cpaddingBottom);
    Int32 cpaddingTop;
    GetCompoundPaddingTop(&cpaddingTop);
    Int32 lineHeight;
    GetLineHeight(&lineHeight);

    return SetMinHeight(lineHeight + cpaddingBottom + cpaddingTop);
}

ECode CalculatorEditText::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 actionMasked;
    event->GetActionMasked(&actionMasked);
    if (actionMasked == IMotionEvent::ACTION_UP) {
        // Hack to prevent keyboard and insertion handle from showing.
        CancelLongPress();
    }

    return EditText::OnTouchEvent(event, result);
}

ECode CalculatorEditText::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    EditText::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 paddingLeft;
    GetPaddingLeft(&paddingLeft);
    Int32 paddingRight;
    GetPaddingRight(&paddingRight);
    mWidthConstraint =
            View::MeasureSpec::GetSize(widthMeasureSpec) - paddingLeft - paddingRight;
    AutoPtr<ICharSequence> cs;
    GetText((ICharSequence**)&cs);
    String str;
    cs->ToString(&str);
    Float vol;
    GetVariableTextSize(str, &vol);
    SetTextSize(ITypedValue::COMPLEX_UNIT_PX, vol);
    return NOERROR;
}

ECode CalculatorEditText::OnSaveInstanceState(
    /* [out] */ IParcelable* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IParcelable> pl = EditText::OnSaveInstanceState();

    // EditText will freeze any text with a selection regardless of getFreezesText() ->
    // return null to prevent any state from being preserved at the instance level.
    result = NULL;
    return NOERROR;
}

ECode CalculatorEditText::OnTextChanged(
    /* [in] */ ICharSequence* text,
    /* [in] */ Int32 start,
    /* [in] */ Int32 lengthBefore,
    /* [in] */ Int32 lengthAfter)
{
    EditText::OnTextChanged(text, start, lengthBefore, lengthAfter);

    Int32 textLength;
    text->GetLength(&textLength);
    Int32 istart;
    GetSelectionStart(&istart);
    Int32 end;
    GetSelectionEnd(&end);
    if (istart != textLength || end != textLength) {
        // Pin the selection to the end of the current text.
        SetSelection(textLength);
    }
    String str;
    text->ToString(&str);
    Float vol;
    GetVariableTextSize(str, &vol);
    return SetTextSize(ITypedValue::COMPLEX_UNIT_PX, vol);
}

ECode CalculatorEditText::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    Float oldTextSize;
    GetTextSize(&oldTextSize);
    EditText::SetTextSize(unit, size);

    Float fvol;
    if (mOnTextSizeChangeListener != NULL && (GetTextSize(&fvol), fvol != oldTextSize)) {
        mOnTextSizeChangeListener->OnTextSizeChanged(this, oldTextSize);
    }
    return NOERROR;
}

ECode CalculatorEditText::SetOnTextSizeChangeListener(
    /* [in] */ IOnTextSizeChangeListener* listener)
{
    mOnTextSizeChangeListener = listener;
    return NOERROR;
}

ECode CalculatorEditText::GetVariableTextSize(
    /* [in] */ const String& text,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    if (mWidthConstraint < 0 || mMaximumTextSize <= mMinimumTextSize) {
        // Not measured, bail early.
        Float size;
        GetTextSize(&size);
        *result = size;
        return NOERROR;
    }

    // Capture current paint state.
    AutoPtr<ITextPaint> paint;
    GetPaint((ITextPaint**)&paint);
    mTempPaint->Set(IPaint::Probe(paint));

    // Step through increasing text sizes until the text would no longer fit.
    Float lastFitTextSize = mMinimumTextSize;
    Float width;
    while (lastFitTextSize < mMaximumTextSize) {
        Float nextSize = Elastos::Core::Math::Min(lastFitTextSize + mStepTextSize, mMaximumTextSize);
        mTempPaint->SetTextSize(nextSize);

        mTempPaint->MeasureText(text, &width);
        if (width > mWidthConstraint) {
            break;
        }
        else {
            lastFitTextSize = nextSize;
        }
    }

    *result = lastFitTextSize;
    return NOERROR;
}

ECode CalculatorEditText::GetCompoundPaddingTop(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // Measure the top padding from the capital letter height of the text instead of the top,
    // but don't remove more than the available top padding otherwise clipping may occur.
    AutoPtr<ITextPaint> tp;
    GetPaint((ITextPaint**)&tp);
    IPaint::Probe(tp)->GetTextBounds(String("H"), 0, 1, mTempRect);
    AutoPtr<IPaintFontMetricsInt> fontMetrics;
    IPaint::Probe(tp)->GetFontMetricsInt((IPaintFontMetricsInt**)&fontMetrics);
    Int32 height;
    mTempRect->GetHeight(&height);
    Int32 ascent;
    fontMetrics->GetAscent(&ascent);
    Int32 paddingOffset = -(ascent + height);
    Int32 top;
    EditText::GetCompoundPaddingTop(&top);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    *result = top - Elastos::Core::Math::Min(paddingTop, paddingOffset);
    return NOERROR;
}

ECode CalculatorEditText::GetCompoundPaddingBottom(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    // Measure the bottom padding from the baseline of the text instead of the bottom, but don't
    // remove more than the available bottom padding otherwise clipping may occur.
    AutoPtr<ITextPaint> tp;
    GetPaint((ITextPaint**)&tp);
    AutoPtr<IPaintFontMetricsInt> fontMetrics;
    IPaint::Probe(tp)->GetFontMetricsInt((IPaintFontMetricsInt**)&fontMetrics);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 descent;
    fontMetrics->GetDescent(&descent);
    Int32 vbottom;
    EditText::GetCompoundPaddingBottom(&vbottom);
    *result = vbottom - Elastos::Core::Math::Min(bottom, descent);
    return NOERROR;
}

} // namespace Calculator3
} // namespace Droid
} // namespace Elastos