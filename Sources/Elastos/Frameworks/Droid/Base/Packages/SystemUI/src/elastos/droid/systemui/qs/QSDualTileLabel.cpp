
#include "elastos/droid/systemui/qs/QSDualTileLabel.h"
#include "R.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Text.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/Character.h>

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::Widget::CImageView;
using Elastos::Droid::Widget::CLinearLayout;
using Elastos::Droid::Widget::CLinearLayoutLayoutParams;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::ILinearLayoutLayoutParams;
using Elastos::Droid::Widget::ImageViewScaleType_MATRIX;
using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

QSDualTileLabel::UpdateTextRunnable::UpdateTextRunnable(
    /* [in] */ QSDualTileLabel* host)
    : mHost(host)
{}

ECode QSDualTileLabel::UpdateTextRunnable::Run()
{
    mHost->UpdateText();
    return NOERROR;
}

CAR_INTERFACE_IMPL(QSDualTileLabel::OnLayoutChangeListener, Object, IViewOnLayoutChangeListener)
QSDualTileLabel::OnLayoutChangeListener::OnLayoutChangeListener(
    /* [in] */ QSDualTileLabel* host)
    : mHost(host)
{}

ECode QSDualTileLabel::OnLayoutChangeListener::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    if ((oldRight - oldLeft) != (right - left)) {
        mHost->RescheduleUpdateText();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(QSDualTileLabel, LinearLayout, IQSDualTileLabel)
QSDualTileLabel::QSDualTileLabel(
    /* [in] */ IContext* context)
    : mHorizontalPaddingPx(0)
{
    LinearLayout::constructor(context);
    mUpdateText = new UpdateTextRunnable(this);
    mContext = context;
    SetOrientation(ILinearLayout::VERTICAL);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::qs_dual_tile_padding_horizontal, &mHorizontalPaddingPx);

    mFirstLine = InitTextView();
    IView::Probe(mFirstLine)->SetPadding(mHorizontalPaddingPx, 0, mHorizontalPaddingPx, 0);
    AutoPtr<ILinearLayout> firstLineLayout;
    CLinearLayout::New(mContext, (ILinearLayout**)&firstLineLayout);
    IView::Probe(firstLineLayout)->SetPadding(0, 0, 0, 0);
    firstLineLayout->SetOrientation(ILinearLayout::HORIZONTAL);
    IView::Probe(firstLineLayout)->SetClickable(FALSE);
    IView::Probe(firstLineLayout)->SetBackground(NULL);
    IViewGroup::Probe(firstLineLayout)->AddView(IView::Probe(mFirstLine));
    CImageView::New(mContext, (IImageView**)&mFirstLineCaret);
    mFirstLineCaret->SetScaleType(ImageViewScaleType_MATRIX);
    IView::Probe(mFirstLineCaret)->SetClickable(FALSE);
    IViewGroup::Probe(firstLineLayout)->AddView(IView::Probe(mFirstLineCaret));
    AddView(IView::Probe(firstLineLayout), NewLinearLayoutParams());

    mSecondLine = InitTextView();
    IView::Probe(mSecondLine)->SetPadding(mHorizontalPaddingPx, 0, mHorizontalPaddingPx, 0);
    mSecondLine->SetEllipsize(TextUtilsTruncateAt_END);
    IView::Probe(mSecondLine)->SetVisibility(GONE);
    AddView(IView::Probe(mSecondLine), NewLinearLayoutParams());

    AutoPtr<OnLayoutChangeListener> cl = new OnLayoutChangeListener(this);
    AddOnLayoutChangeListener(cl);
}

AutoPtr<IViewGroupLayoutParams> QSDualTileLabel::NewLinearLayoutParams()
{
    AutoPtr<ILinearLayoutLayoutParams> lp;
    CLinearLayoutLayoutParams::New(IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT,
        (ILinearLayoutLayoutParams**)&lp);
    lp->SetGravity(IGravity::CENTER_HORIZONTAL);
    return IViewGroupLayoutParams::Probe(lp);
}

ECode QSDualTileLabel::SetFirstLineCaret(
    /* [in] */ IDrawable* d)
{
    mFirstLineCaret->SetImageDrawable(d);
    if (d != NULL) {
        Int32 h = 0;
        d->GetIntrinsicHeight(&h);
        AutoPtr<IViewGroupLayoutParams> lp;
        IView::Probe(mSecondLine)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
        IViewGroupMarginLayoutParams::Probe(lp)->SetTopMargin(h * 4 / 5);
        IView::Probe(mSecondLine)->SetLayoutParams(lp);
        mFirstLine->SetMinHeight(h);
        IView::Probe(mFirstLine)->SetPadding(mHorizontalPaddingPx, 0, 0, 0);
    }
    return NOERROR;
}

AutoPtr<ITextView> QSDualTileLabel::InitTextView()
{
    AutoPtr<ITextView> tv;
    CTextView::New(mContext, (ITextView**)&tv);
    IView::Probe(tv)->SetPadding(0, 0, 0, 0);
    tv->SetGravity(IGravity::CENTER_VERTICAL);
    tv->SetSingleLine(TRUE);
    IView::Probe(tv)->SetClickable(FALSE);
    IView::Probe(tv)->SetBackground(NULL);
    return tv;
}

ECode QSDualTileLabel::SetText(
    /* [in] */ ICharSequence* text)
{
    String newText;
    if (text != NULL) {
        text->ToString(&newText);
        newText = newText.Trim();
    }
    if (newText.Equals(mText)) return NOERROR;
    mText = newText;
    RescheduleUpdateText();
    return NOERROR;
}

ECode QSDualTileLabel::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    return NOERROR;
}

ECode QSDualTileLabel::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    mFirstLine->SetTextSize(unit, size);
    mSecondLine->SetTextSize(unit, size);
    RescheduleUpdateText();
    return NOERROR;
}

ECode QSDualTileLabel::SetTextColor(
    /* [in] */ Int32 color)
{
    mFirstLine->SetTextColor(color);
    mSecondLine->SetTextColor(color);
    RescheduleUpdateText();
    return NOERROR;
}

ECode QSDualTileLabel::SetTypeface(
    /* [in] */ ITypeface* tf)
{
    mFirstLine->SetTypeface(tf);
    mSecondLine->SetTypeface(tf);
    RescheduleUpdateText();
    return NOERROR;
}

void QSDualTileLabel::RescheduleUpdateText()
{
    Boolean tmp = FALSE;
    RemoveCallbacks(mUpdateText, &tmp);
    Post(mUpdateText, &tmp);
}

void QSDualTileLabel::UpdateText()
{
    Int32 v = 0;
    if ((GetWidth(&v), v) == 0) return;
    if (TextUtils::IsEmpty(mText)) {
        mFirstLine->SetText((ICharSequence*)NULL);
        mSecondLine->SetText((ICharSequence*)NULL);
        IView::Probe(mSecondLine)->SetVisibility(GONE);
        return;
    }
    Int32 w = 0, pl = 0, pr = 0;
    const Float maxWidth = v - (IView::Probe(mFirstLineCaret)->GetWidth(&w), w) - mHorizontalPaddingPx
            - (GetPaddingLeft(&pl), pl) - (GetPaddingRight(&pr), pr);

    AutoPtr<ITextPaint> tp;
    mFirstLine->GetPaint((ITextPaint**)&tp);
    Float width = 0;
    IPaint::Probe(tp)->MeasureText(mText, &width);
    if (width <= maxWidth) {
        AutoPtr<ICharSequence> t;
        CString::New(mText, (ICharSequence**)&t);
        mFirstLine->SetText(t);
        mSecondLine->SetText((ICharSequence*)NULL);
        IView::Probe(mSecondLine)->SetVisibility(GONE);
        return;
    }
    const Int32 n = mText.GetLength();
    Int32 lastWordBoundary = -1;
    Boolean inWhitespace = FALSE;
    Int32 i = 0;
    for (i = 1; i < n; i++) {
        tp = NULL;
        mFirstLine->GetPaint((ITextPaint**)&tp);
        IPaint::Probe(tp)->MeasureText(mText.Substring(0, i), &width);
        const Boolean done = width > maxWidth;
        if (Character::IsWhitespace(mText.GetChar(i))) {
            if (!inWhitespace && !done) {
                lastWordBoundary = i;
            }
            inWhitespace = TRUE;
        }
        else {
            inWhitespace = FALSE;
        }
        if (done) {
            break;
        }
    }
    if (lastWordBoundary == -1) {
        lastWordBoundary = i - 1;
    }

    AutoPtr<ICharSequence> t;
    CString::New(mText.Substring(0, lastWordBoundary), (ICharSequence**)&t);
    mFirstLine->SetText(t);

    t = NULL;
    CString::New(mText.Substring(lastWordBoundary).Trim(), (ICharSequence**)&t);
    mSecondLine->SetText(t);
    IView::Probe(mSecondLine)->SetVisibility(VISIBLE);
}

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
