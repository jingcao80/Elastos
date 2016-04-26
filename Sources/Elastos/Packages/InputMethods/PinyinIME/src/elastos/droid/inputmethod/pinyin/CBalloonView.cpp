
#include "elastos/droid/inputmethod/pinyin/CBalloonView.h"
#include "elastos/droid/inputmethod/pinyin/Environment.h"

using Elastos::Droid::Graphics::CPaint;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

String CBalloonView::SUSPENSION_POINTS("...");

CAR_OBJECT_IMPL(CBalloonView);

CAR_INTERFACE_IMPL(CBalloonView, View, ICandidateView);

CBalloonView::CBalloonView()
    : mLabeColor(0xff000000)
    , mSuspensionPointsWidth(0.f)
{}

ECode CBalloonView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(View::constructor(context));
    CPaint::New((IPaint**)&mPaintLabel);
    mPaintLabel->SetColor(mLabeColor);
    mPaintLabel->SetAntiAlias(TRUE);
    mPaintLabel->SetFakeBoldText(TRUE);
    return mPaintLabel->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmi);
}

void CBalloonView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
}

void CBalloonView::SetTextConfig(
    /* [in] */ const String& label,
    /* [in] */ Float fontSize,
    /* [in] */ Boolean textBold,
    /* [in] */ Int32 textColor)
{
    // Icon should be cleared so that the label will be enabled.
    mIcon = NULL;
    mLabel = label;
    mPaintLabel->SetTextSize(fontSize);
    mPaintLabel->SetFakeBoldText(textBold);
    mPaintLabel->SetColor(textColor);
    mFmi = NULL;
    mPaintLabel->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmi);
    mPaintLabel->MeasureText(SUSPENSION_POINTS, &mSuspensionPointsWidth);
}

void CBalloonView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    const Int32 widthMode = MeasureSpec::GetMode(widthMeasureSpec);
    const Int32 heightMode = MeasureSpec::GetMode(heightMeasureSpec);
    const Int32 widthSize = MeasureSpec::GetSize(widthMeasureSpec);
    const Int32 heightSize = MeasureSpec::GetSize(heightMeasureSpec);

    if (widthMode == MeasureSpec::EXACTLY) {
        SetMeasuredDimension(widthSize, heightSize);
        return;
    }

    Int32 measuredWidth = mPaddingLeft + mPaddingRight;
    Int32 measuredHeight = mPaddingTop + mPaddingBottom;
    if (NULL != mIcon) {
        Int32 value = 0;
        measuredWidth += (mIcon->GetIntrinsicWidth(&value), value);
        measuredHeight += (mIcon->GetIntrinsicHeight(&value), value);
    }
    else if (NULL != mLabel) {
        Float value = 0.f;
        measuredWidth += (Int32) (mPaintLabel->MeasureText(mLabel, &value), value);

        Int32 bottom = 0, top = 0;
        mFmi->GetBottom(&bottom);
        mFmi->GetTop(&top);
        measuredHeight += bottom - top;
    }
    if (widthSize > measuredWidth || widthMode == MeasureSpec::AT_MOST) {
        measuredWidth = widthSize;
    }

    if (heightSize > measuredHeight
            || heightMode == MeasureSpec::AT_MOST) {
        measuredHeight = heightSize;
    }

    Int32 width = Environment::GetInstance()->GetScreenWidth();
    Int32 maxWidth = width - mPaddingLeft - mPaddingRight;
    if (measuredWidth > maxWidth) {
        measuredWidth = maxWidth;
    }
    SetMeasuredDimension(measuredWidth, measuredHeight);
}

void CBalloonView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    if (NULL != mIcon) {
        Int32 iconWidth = 0, iconHeight = 0;
        mIcon->GetIntrinsicWidth(&iconWidth);
        Int32 marginLeft = (width - iconWidth) / 2;
        Int32 marginRight = width - iconWidth - marginLeft;

        mIcon->GetIntrinsicHeight(&iconHeight);
        Int32 marginTop = (height - iconHeight) / 2;
        Int32 marginBottom = height - iconHeight - marginTop;
        mIcon->SetBounds(marginLeft, marginTop, width - marginRight,
                height - marginBottom);
        mIcon->Draw(canvas);
    }
    else if (NULL != mLabel) {
        Float labelMeasuredWidth = 0.f;
        mPaintLabel->MeasureText(mLabel, &labelMeasuredWidth);
        Float x = mPaddingLeft;
        x += (width - labelMeasuredWidth - mPaddingLeft - mPaddingRight) / 2.0f;
        String labelToDraw = mLabel;
        if (x < mPaddingLeft) {
            x = mPaddingLeft;
            labelToDraw = GetLimitedLabelForDrawing(mLabel,
                    width - mPaddingLeft - mPaddingRight);
        }

        Int32 bottom = 0, top = 0;
        mFmi->GetBottom(&bottom);
        mFmi->GetTop(&top);
        Int32 fontHeight = bottom - top;
        Float marginY = (height - fontHeight) / 2.0f;
        Float y = marginY - top;
        canvas->DrawText(labelToDraw, x, y, mPaintLabel);
    }
}

String CBalloonView::GetLimitedLabelForDrawing(
    /* [in] */ const String& rawLabel,
    /* [in] */ Float widthToDraw)
{
    Int32 subLen = rawLabel.GetLength();
    if (subLen <= 1) return rawLabel;
    do {
        subLen--;
        Float width = 0.f;
        mPaintLabel->MeasureText(rawLabel, 0, subLen, &width);
        if (width + mSuspensionPointsWidth <= widthToDraw || 1 >= subLen) {
            return rawLabel.Substring(0, subLen) + SUSPENSION_POINTS;
        }
    } while (TRUE);
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
