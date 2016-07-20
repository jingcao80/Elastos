
#include "elastos/droid/inputmethod/pinyin/CComposingView.h"
#include "R.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Core::IStringBuffer;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Int32 CComposingView::LEFT_RIGHT_MARGIN = 5;

CAR_OBJECT_IMPL(CComposingView);

CAR_INTERFACE_IMPL(CComposingView, View, IComposingView);

CComposingView::CComposingView()
    : mStrColor(0)
    , mStrColorHl(0)
    , mStrColorIdle(0)
    , mFontSize(0)
{}

ECode CComposingView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(View::constructor(context, attrs));

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    r->GetDrawable(R::drawable::composing_hl_bg, (IDrawable**)&mHlDrawable);
    r->GetDrawable(R::drawable::composing_area_cursor, (IDrawable**)&mCursor);

    r->GetColor(R::color::composing_color, &mStrColor);
    r->GetColor(R::color::composing_color_hl, &mStrColorHl);
    r->GetColor(R::color::composing_color_idle, &mStrColorIdle);

    r->GetDimensionPixelSize(R::dimen::composing_height, &mFontSize);

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetColor(mStrColor);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetTextSize(mFontSize);

    return mPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmi);
}

ECode CComposingView::Reset()
{
    mComposingStatus = SHOW_PINYIN;
    return NOERROR;
}

ECode CComposingView::SetDecodingInfo(
    /* [in] */ CPinyinIME::DecodingInfo* decInfo,
    /* [in] */ CPinyinIME::ImeState imeStatus)
{
    mDecInfo = decInfo;

    if (CPinyinIME::STATE_INPUT == imeStatus) {
        mComposingStatus = SHOW_PINYIN;
        mDecInfo->MoveCursorToEdge(FALSE);
    }
    else {
        if (decInfo->GetFixedLen() != 0
                || EDIT_PINYIN == mComposingStatus) {
            mComposingStatus = EDIT_PINYIN;
        }
        else {
            mComposingStatus = SHOW_STRING_LOWERCASE;
        }
        mDecInfo->MoveCursor(0);
    }

    Measure(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    RequestLayout();
    return Invalidate();
}

Boolean CComposingView::MoveCursor(
    /* [in] */ Int32 keyCode)
{
    if (keyCode != IKeyEvent::KEYCODE_DPAD_LEFT
            && keyCode != IKeyEvent::KEYCODE_DPAD_RIGHT) {
        return FALSE;
    }

    if (EDIT_PINYIN == mComposingStatus) {
        Int32 offset = 0;
        if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT) {
            offset = -1;
        }
        else if (keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) offset = 1;
        mDecInfo->MoveCursor(offset);
    }
    else if (SHOW_STRING_LOWERCASE == mComposingStatus) {
        if (keyCode == IKeyEvent::KEYCODE_DPAD_LEFT
                || keyCode == IKeyEvent::KEYCODE_DPAD_RIGHT) {
            mComposingStatus = EDIT_PINYIN;

            Measure(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
            RequestLayout();
        }

    }
    Invalidate();
    return TRUE;
}

CComposingView::ComposingStatus CComposingView::GetComposingStatus()
{
    return mComposingStatus;
}

ECode CComposingView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Float width = 0.f;
    Int32 height = 0, bottom = 0, top = 0;
    height = (mFmi->GetBottom(&bottom), bottom) - (mFmi->GetTop(&top), top) + mPaddingTop + mPaddingBottom;

    if (NULL == mDecInfo) {
        width = 0;
    }
    else {
        width = mPaddingLeft + mPaddingRight + LEFT_RIGHT_MARGIN * 2;

        String str;
        if (SHOW_STRING_LOWERCASE == mComposingStatus) {
            str = mDecInfo->GetOrigianlSplStr()->ToString();
        }
        else {
            str = mDecInfo->GetComposingStrForDisplay();
        }
        Float value = 0.f;
        mPaint->MeasureText(str, 0, str.GetLength(), &value);
        width += value;
    }
    SetMeasuredDimension((Int32) (width + 0.5f), height);
    return NOERROR;
}

void CComposingView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (EDIT_PINYIN == mComposingStatus
            || SHOW_PINYIN == mComposingStatus) {
        DrawForPinyin(canvas);
        return;
    }

    Float x, y;
    Int32 top = 0;
    x = mPaddingLeft + LEFT_RIGHT_MARGIN;
    y = -(mFmi->GetTop(&top), top) + mPaddingTop;

    mPaint->SetColor(mStrColorHl);
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    mHlDrawable->SetBounds(mPaddingLeft, mPaddingTop, width
            - mPaddingRight, height - mPaddingBottom);
    mHlDrawable->Draw(canvas);

    String splStr = mDecInfo->GetOrigianlSplStr()->ToString();
    canvas->DrawText(splStr, 0, splStr.GetLength(), x, y, mPaint);
}

void CComposingView::DrawCursor(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float x)
{
    Int32 width, height;
    mCursor->GetIntrinsicWidth(&width);
    GetHeight(&height);
    mCursor->SetBounds((Int32) x, mPaddingTop, (Int32) x
            + width, height - mPaddingBottom);
    mCursor->Draw(canvas);
}

void CComposingView::DrawForPinyin(
    /* [in] */ ICanvas* canvas)
{
    Float x, y, value;
    Int32 top = 0;
    x = mPaddingLeft + LEFT_RIGHT_MARGIN;
    y = -(mFmi->GetTop(&top), top) + mPaddingTop;

    mPaint->SetColor(mStrColor);

    Int32 cursorPos = mDecInfo->GetCursorPosInCmpsDisplay();
    Int32 cmpsPos = cursorPos;
    String cmpsStr = mDecInfo->GetComposingStrForDisplay();
    Int32 activeCmpsLen = mDecInfo->GetActiveCmpsDisplayLen();
    if (cursorPos > activeCmpsLen) cmpsPos = activeCmpsLen;
    canvas->DrawText(cmpsStr, 0, cmpsPos, x, y, mPaint);
    x += (mPaint->MeasureText(cmpsStr, 0, cmpsPos, &value), value);
    if (cursorPos <= activeCmpsLen) {
        if (EDIT_PINYIN == mComposingStatus) {
            DrawCursor(canvas, x);
        }
        canvas->DrawText(cmpsStr, cmpsPos, activeCmpsLen, x, y, mPaint);
    }

    x += (mPaint->MeasureText(cmpsStr, cmpsPos, activeCmpsLen, &value), value);

    if ((Int32)cmpsStr.GetLength() > activeCmpsLen) {
        mPaint->SetColor(mStrColorIdle);
        Int32 oriPos = activeCmpsLen;
        if (cursorPos > activeCmpsLen) {
            if (cursorPos > (Int32)cmpsStr.GetLength()) cursorPos = cmpsStr.GetLength();
            canvas->DrawText(cmpsStr, oriPos, cursorPos, x, y, mPaint);
            x += (mPaint->MeasureText(cmpsStr, oriPos, cursorPos, &value), value);

            if (EDIT_PINYIN == mComposingStatus) {
                DrawCursor(canvas, x);
            }

            oriPos = cursorPos;
        }
        canvas->DrawText(cmpsStr, oriPos, cmpsStr.GetLength(), x, y, mPaint);
    }
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
