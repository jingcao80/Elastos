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

#include "elastos/droid/graphics/CColor.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/internal/widget/SubtitleView.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"
#include "elastos/droid/view/accessibility/CCaptioningManagerCaptionStyleHelper.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::PaintJoin_ROUND;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_FILL_AND_STROKE;
using Elastos::Droid::Internal::Widget::EIID_ISubtitleView;
using Elastos::Droid::Text::ALIGN_NONE;
using Elastos::Droid::Text::CStaticLayout;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::View::Accessibility::CaptioningManager;
using Elastos::Droid::View::Accessibility::CCaptioningManagerCaptionStyleHelper;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyle;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyleHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

//=====================================================================
//                             SubtitleView
//=====================================================================
const Float SubtitleView::INNER_PADDING_RATIO = 0.125f;
const Int32 SubtitleView::COLOR_BEVEL_DARK;
const Int32 SubtitleView::COLOR_BEVEL_LIGHT;

CAR_INTERFACE_IMPL(SubtitleView, Elastos::Droid::View::View, ISubtitleView)

SubtitleView::SubtitleView()
    : mCornerRadius(0.0f)
    , mOutlineWidth(0.0f)
    , mShadowRadius(0.0f)
    , mShadowOffsetX(0.0f)
    , mShadowOffsetY(0.0f)
    , mAlignment(ALIGN_NONE)
    , mForegroundColor(0)
    , mBackgroundColor(0)
    , mEdgeColor(0)
    , mEdgeType(0)
    , mHasMeasurements(FALSE)
    , mLastMeasuredWidth(0)
    , mSpacingMult(0.0f)
    , mSpacingAdd(0.0f)
    , mInnerPaddingX(0)
{
}

ECode SubtitleView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode SubtitleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, 0);

    return constructor(context, attrs, 0);
}

ECode SubtitleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    // ==================before translated======================
    // this(context, attrs, defStyleAttr, 0);

    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SubtitleView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    // ==================before translated======================
    // super(context, attrs);
    //
    // final TypedArray a = context.obtainStyledAttributes(
    //             attrs, android.R.styleable.TextView, defStyleAttr, defStyleRes);
    //
    // CharSequence text = "";
    // int textSize = 15;
    //
    // final int n = a.getIndexCount();
    // for (int i = 0; i < n; i++) {
    //     int attr = a.getIndex(i);
    //
    //     switch (attr) {
    //         case android.R.styleable.TextView_text:
    //             text = a.getText(attr);
    //             break;
    //         case android.R.styleable.TextView_lineSpacingExtra:
    //             mSpacingAdd = a.getDimensionPixelSize(attr, (int) mSpacingAdd);
    //             break;
    //         case android.R.styleable.TextView_lineSpacingMultiplier:
    //             mSpacingMult = a.getFloat(attr, mSpacingMult);
    //             break;
    //         case android.R.styleable.TextAppearance_textSize:
    //             textSize = a.getDimensionPixelSize(attr, textSize);
    //             break;
    //     }
    // }
    //
    // // Set up density-dependent properties.
    // // TODO: Move these to a default style.
    // final Resources res = getContext().getResources();
    // mCornerRadius = res.getDimensionPixelSize(com.android.internal.R.dimen.subtitle_corner_radius);
    // mOutlineWidth = res.getDimensionPixelSize(com.android.internal.R.dimen.subtitle_outline_width);
    // mShadowRadius = res.getDimensionPixelSize(com.android.internal.R.dimen.subtitle_shadow_radius);
    // mShadowOffsetX = res.getDimensionPixelSize(com.android.internal.R.dimen.subtitle_shadow_offset);
    // mShadowOffsetY = mShadowOffsetX;
    //
    // mTextPaint = new TextPaint();
    // mTextPaint.setAntiAlias(true);
    // mTextPaint.setSubpixelText(true);
    //
    // mPaint = new Paint();
    // mPaint.setAntiAlias(true);
    //
    // setText(text);
    // setTextSize(textSize);

    View::constructor(context, attrs);

    AutoPtr< ArrayOf<Int32> > attrsFormat = ArrayOf<Int32>::Alloc(ArraySize(R::styleable::TextView) + 2);
    Int32 idx = 0;
    for (idx=0; idx<ArraySize(R::styleable::TextView); ++idx) {
        attrsFormat->Set(idx, (Int32)R::styleable::TextView[idx]);
    }
    attrsFormat->Set(idx++, defStyleAttr);
    attrsFormat->Set(idx++, defStyleRes);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrsFormat, (ITypedArray**)&a);

    AutoPtr<ICharSequence> text = NULL;
    Int32 textSize = 15;
    Int32 n = 0;
    a->GetIndexCount(&n);
    Int32 attr = 0;
    for (Int32 i = 0; i < n; ++i) {
        a->GetIndex(i, &attr);
        switch (attr) {
            case R::styleable::TextView_text:
                a->GetText(attr, (ICharSequence**)&text);
                break;
            case R::styleable::TextView_lineSpacingExtra:
                {
                    Int32 resTmp = 0;
                    a->GetDimensionPixelSize(attr, (Int32)mSpacingAdd, &resTmp);
                    mSpacingAdd = resTmp;
                }
                break;
            case R::styleable::TextView_lineSpacingMultiplier:
                a->GetFloat(attr, mSpacingMult, &mSpacingMult);
                break;
            case R::styleable::TextAppearance_textSize:
                a->GetDimensionPixelSize(attr, textSize, &textSize);
                break;
        }
    }

    // Set up density-dependent properties.
    // TODO: Move these to a default style.
    AutoPtr<IContext> contextTmp;
    GetContext((IContext**)&contextTmp);
    AutoPtr<IResources> res;
    contextTmp->GetResources((IResources**)&res);
    Int32 resTmp = 0;
    res->GetDimensionPixelSize(R::dimen::subtitle_corner_radius, &resTmp);
    mCornerRadius = resTmp;
    res->GetDimensionPixelSize(R::dimen::subtitle_outline_width, &resTmp);
    mOutlineWidth = resTmp;
    res->GetDimensionPixelSize(R::dimen::subtitle_shadow_radius, &resTmp);
    mShadowRadius = resTmp;
    res->GetDimensionPixelSize(R::dimen::subtitle_shadow_offset, &resTmp);
    mShadowOffsetX = resTmp;
    mShadowOffsetY = mShadowOffsetX;

    CTextPaint::New((ITextPaint**)&mTextPaint);
    IPaint::Probe(mTextPaint)->SetAntiAlias(TRUE);
    IPaint::Probe(mTextPaint)->SetSubpixelText(TRUE);

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetAntiAlias(TRUE);

    SetText(text);
    SetTextSize(textSize);
    return NOERROR;
}

ECode SubtitleView::SetText(
    /* [in] */ Int32 resId)
{
    // ==================before translated======================
    // final CharSequence text = getContext().getText(resId);
    // setText(text);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<ICharSequence> text;
    context->GetText(resId, (ICharSequence**)&text);
    SetText(text);
    return NOERROR;
}

ECode SubtitleView::SetText(
    /* [in] */ ICharSequence* text)
{
    VALIDATE_NOT_NULL(text);
    // ==================before translated======================
    // mText.setLength(0);
    // mText.append(text);
    //
    // mHasMeasurements = false;
    //
    // requestLayout();

    mText->SetLength(0);
    mText->Append(text);
    mHasMeasurements = FALSE;
    RequestLayout();
    return NOERROR;
}

ECode SubtitleView::SetForegroundColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mForegroundColor = color;
    //
    // invalidate();

    mForegroundColor = color;
    Invalidate();
    return NOERROR;
}

ECode SubtitleView::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mBackgroundColor = color;
    //
    // invalidate();

    mBackgroundColor = color;
    Invalidate();
    return NOERROR;
}

ECode SubtitleView::SetEdgeType(
    /* [in] */ Int32 edgeType)
{
    // ==================before translated======================
    // mEdgeType = edgeType;
    //
    // invalidate();

    mEdgeType = edgeType;
    Invalidate();
    return NOERROR;
}

ECode SubtitleView::SetEdgeColor(
    /* [in] */ Int32 color)
{
    // ==================before translated======================
    // mEdgeColor = color;
    //
    // invalidate();

    mEdgeColor = color;
    Invalidate();
    return NOERROR;
}

ECode SubtitleView::SetTextSize(
    /* [in] */ Float size)
{
    // ==================before translated======================
    // if (mTextPaint.getTextSize() != size) {
    //     mTextPaint.setTextSize(size);
    //     mInnerPaddingX = (int) (size * INNER_PADDING_RATIO + 0.5f);
    //
    //     mHasMeasurements = false;
    //
    //     requestLayout();
    //     invalidate();
    // }

    Float textSize = 0.0f;
    IPaint::Probe(mTextPaint)->GetTextSize(&textSize);
    if (textSize - size < 0.000001f) {
        IPaint::Probe(mTextPaint)->SetTextSize(size);
        mInnerPaddingX = (Int32)(size * INNER_PADDING_RATIO + 0.5f);
        mHasMeasurements = FALSE;
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode SubtitleView::SetTypeface(
    /* [in] */ ITypeface* typeface)
{
    VALIDATE_NOT_NULL(typeface);
    // ==================before translated======================
    // if (mTextPaint.getTypeface() != typeface) {
    //     mTextPaint.setTypeface(typeface);
    //
    //     mHasMeasurements = false;
    //
    //     requestLayout();
    //     invalidate();
    // }

    AutoPtr<ITypeface> typefaceTmp;
    IPaint::Probe(mTextPaint)->GetTypeface((ITypeface**)&typefaceTmp);
    if (TO_IINTERFACE(typefaceTmp) != TO_IINTERFACE(typeface)) {
        IPaint::Probe(mTextPaint)->SetTypeface(typeface);
        mHasMeasurements = FALSE;
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode SubtitleView::SetAlignment(
    /* [in] */ LayoutAlignment textAlignment)
{
    // ==================before translated======================
    // if (mAlignment != textAlignment) {
    //     mAlignment = textAlignment;
    //
    //     mHasMeasurements = false;
    //
    //     requestLayout();
    //     invalidate();
    // }

    if (mAlignment != textAlignment) {
        mAlignment = textAlignment;
        mHasMeasurements = FALSE;
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode SubtitleView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // ==================before translated======================
    // final int width = r - l;
    //
    // computeMeasurements(width);

    Int32 width = r - l;
    ComputeMeasurements(width);
    return NOERROR;
}

ECode SubtitleView::SetStyle(
    /* [in] */ Int32 styleId)
{
    // ==================before translated======================
    // final Context context = mContext;
    // final ContentResolver cr = context.getContentResolver();
    // final CaptionStyle style;
    // if (styleId == CaptionStyle.PRESET_CUSTOM) {
    //     style = CaptionStyle.getCustomStyle(cr);
    // } else {
    //     style = CaptionStyle.PRESETS[styleId];
    // }
    //
    // final CaptionStyle defStyle = CaptionStyle.DEFAULT;
    // mForegroundColor = style.hasForegroundColor() ?
    //         style.foregroundColor : defStyle.foregroundColor;
    // mBackgroundColor = style.hasBackgroundColor() ?
    //         style.backgroundColor : defStyle.backgroundColor;
    // mEdgeType = style.hasEdgeType() ? style.edgeType : defStyle.edgeType;
    // mEdgeColor = style.hasEdgeColor() ? style.edgeColor : defStyle.edgeColor;
    // mHasMeasurements = false;
    //
    // final Typeface typeface = style.getTypeface();
    // setTypeface(typeface);
    //
    // requestLayout();

    AutoPtr<IContext> context = mContext;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICaptioningManagerCaptionStyle> style;
    if (styleId == ICaptioningManagerCaptionStyle::PRESET_CUSTOM) {
        AutoPtr<ICaptioningManagerCaptionStyleHelper> helper;
        CCaptioningManagerCaptionStyleHelper::AcquireSingleton((ICaptioningManagerCaptionStyleHelper**)&helper);
        helper->GetCustomStyle(cr, (ICaptioningManagerCaptionStyle**)&style);
    }
    else {
        style = (*CaptioningManager::CaptionStyle::PRESETS)[styleId];
    }

    AutoPtr<ICaptioningManagerCaptionStyle> defStyle = CaptioningManager::CaptionStyle::DEFAULT;
    Boolean hasForegroundColor = FALSE;
    style->HasForegroundColor(&hasForegroundColor);

    Int32 foregroundColor = 0;
    style->GetForegroundColor(&foregroundColor);
    Int32 defForegroundColor = 0;
    defStyle->GetForegroundColor(&defForegroundColor);
    mForegroundColor = hasForegroundColor ? foregroundColor : defForegroundColor;

    Boolean hasBackgroundColor = FALSE;
    style->HasBackgroundColor(&hasBackgroundColor);

    Int32 backgroundColor = 0;
    style->GetBackgroundColor(&backgroundColor);
    Int32 defBackgroundColor = 0;
    defStyle->GetBackgroundColor(&defBackgroundColor);
    mBackgroundColor = hasBackgroundColor ? backgroundColor : defBackgroundColor;

    Boolean hasEdgeType = FALSE;
    style->HasEdgeType(&hasEdgeType);

    Int32 edgeType = 0;
    style->GetEdgeType(&edgeType);
    Int32 defEdgeType = 0;
    defStyle->GetEdgeType(&defEdgeType);
    mEdgeType = hasEdgeType ? edgeType : defEdgeType;

    Boolean hasEdgeColor = FALSE;
    style->HasEdgeColor(&hasEdgeColor);
    mEdgeColor = hasEdgeColor ? edgeType : defEdgeType;
    mHasMeasurements = FALSE;

    AutoPtr<ITypeface> typeface;
    style->GetTypeface((ITypeface**)&typeface);
    SetTypeface(typeface);
    RequestLayout();
    return NOERROR;
}

ECode SubtitleView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // final int widthSpec = MeasureSpec.getSize(widthMeasureSpec);
    //
    // if (computeMeasurements(widthSpec)) {
    //     final StaticLayout layout = mLayout;
    //
    //     // Account for padding.
    //     final int paddingX = mPaddingLeft + mPaddingRight + mInnerPaddingX * 2;
    //     final int width = layout.getWidth() + paddingX;
    //     final int height = layout.getHeight() + mPaddingTop + mPaddingBottom;
    //     setMeasuredDimension(width, height);
    // } else {
    //     setMeasuredDimension(MEASURED_STATE_TOO_SMALL, MEASURED_STATE_TOO_SMALL);
    // }

    Int32 widthSpec = View::MeasureSpec::GetSize(widthMeasureSpec);
    if (ComputeMeasurements(widthSpec)) {
        AutoPtr<IStaticLayout> layout = mLayout;
        // Account for padding.
        Int32 paddingX = mPaddingLeft + mPaddingRight + mInnerPaddingX * 2;
        Int32 widthTmp, heightTmp;
        ILayout::Probe(layout)->GetWidth(&widthTmp);
        ILayout::Probe(layout)->GetHeight(&heightTmp);
        Int32 width = widthTmp + paddingX;
        Int32 height = heightTmp + mPaddingTop + mPaddingBottom;
        SetMeasuredDimension(width, height);
    }
    else {
        SetMeasuredDimension(MEASURED_STATE_TOO_SMALL, MEASURED_STATE_TOO_SMALL);
    }
    return NOERROR;
}

void SubtitleView::OnDraw(
    /* [in] */ ICanvas* c)
{
    // ==================before translated======================
    // final StaticLayout layout = mLayout;
    // if (layout == null) {
    //     return;
    // }
    //
    // final int saveCount = c.save();
    // final int innerPaddingX = mInnerPaddingX;
    // c.translate(mPaddingLeft + innerPaddingX, mPaddingTop);
    //
    // final int lineCount = layout.getLineCount();
    // final Paint textPaint = mTextPaint;
    // final Paint paint = mPaint;
    // final RectF bounds = mLineBounds;
    //
    // if (Color.alpha(mBackgroundColor) > 0) {
    //     final float cornerRadius = mCornerRadius;
    //     float previousBottom = layout.getLineTop(0);
    //
    //     paint.setColor(mBackgroundColor);
    //     paint.setStyle(Style.FILL);
    //
    //     for (int i = 0; i < lineCount; i++) {
    //         bounds.left = layout.getLineLeft(i) -innerPaddingX;
    //         bounds.right = layout.getLineRight(i) + innerPaddingX;
    //         bounds.top = previousBottom;
    //         bounds.bottom = layout.getLineBottom(i);
    //         previousBottom = bounds.bottom;
    //
    //         c.drawRoundRect(bounds, cornerRadius, cornerRadius, paint);
    //     }
    // }
    //
    // final int edgeType = mEdgeType;
    // if (edgeType == CaptionStyle.EDGE_TYPE_OUTLINE) {
    //     textPaint.setStrokeJoin(Join.ROUND);
    //     textPaint.setStrokeWidth(mOutlineWidth);
    //     textPaint.setColor(mEdgeColor);
    //     textPaint.setStyle(Style.FILL_AND_STROKE);
    //
    //     for (int i = 0; i < lineCount; i++) {
    //         layout.drawText(c, i, i);
    //     }
    // } else if (edgeType == CaptionStyle.EDGE_TYPE_DROP_SHADOW) {
    //     textPaint.setShadowLayer(mShadowRadius, mShadowOffsetX, mShadowOffsetY, mEdgeColor);
    // } else if (edgeType == CaptionStyle.EDGE_TYPE_RAISED
    //         || edgeType == CaptionStyle.EDGE_TYPE_DEPRESSED) {
    //     final boolean raised = edgeType == CaptionStyle.EDGE_TYPE_RAISED;
    //     final int colorUp = raised ? Color.WHITE : mEdgeColor;
    //     final int colorDown = raised ? mEdgeColor : Color.WHITE;
    //     final float offset = mShadowRadius / 2f;
    //
    //     textPaint.setColor(mForegroundColor);
    //     textPaint.setStyle(Style.FILL);
    //     textPaint.setShadowLayer(mShadowRadius, -offset, -offset, colorUp);
    //
    //     for (int i = 0; i < lineCount; i++) {
    //         layout.drawText(c, i, i);
    //     }
    //
    //     textPaint.setShadowLayer(mShadowRadius, offset, offset, colorDown);
    // }
    //
    // textPaint.setColor(mForegroundColor);
    // textPaint.setStyle(Style.FILL);
    //
    // for (int i = 0; i < lineCount; i++) {
    //     layout.drawText(c, i, i);
    // }
    //
    // textPaint.setShadowLayer(0, 0, 0, 0);
    // c.restoreToCount(saveCount);

    AutoPtr<IStaticLayout> layout = mLayout;
    if (layout == NULL) {
        return;
    }

    Int32 saveCount = 0;
    c->Save(&saveCount);
    Int32 innerPaddingX = mInnerPaddingX;
    c->Translate(mPaddingLeft + innerPaddingX, mPaddingTop);

    Int32 lineCount = 0;
    ILayout::Probe(layout)->GetLineCount(&lineCount);
    AutoPtr<IPaint> textPaint = IPaint::Probe(mTextPaint);
    AutoPtr<IPaint> paint = mPaint;
    AutoPtr<IRectF> bounds = mLineBounds;

    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 alpha = 0;
    color->Alpha(mBackgroundColor, &alpha);
    if (alpha > 0) {
        Float cornerRadius = mCornerRadius;
        Int32 previousBottom;
        ILayout::Probe(layout)->GetLineTop(0, &previousBottom);

        paint->SetColor(mBackgroundColor);
        paint->SetStyle(PaintStyle_FILL);

        Float lineLeft, lineRight;
        Int32 lineBottom;
        for (Int32 i = 0; i < lineCount; ++i) {
            ILayout::Probe(layout)->GetLineLeft(i, &lineLeft);
            ILayout::Probe(layout)->GetLineRight(i, &lineRight);
            ILayout::Probe(layout)->GetLineBottom(i, &lineBottom);
            bounds->SetLeft(lineLeft -innerPaddingX);
            bounds->SetRight(lineRight + innerPaddingX);
            bounds->SetTop(previousBottom);
            bounds->SetBottom(lineBottom);
            previousBottom = lineBottom;
            c->DrawRoundRect(bounds, cornerRadius, cornerRadius, paint);
        }
    }

    Int32 edgeType = mEdgeType;
    if (edgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_OUTLINE) {
        textPaint->SetStrokeJoin(PaintJoin_ROUND);
        textPaint->SetStrokeWidth(mOutlineWidth);
        textPaint->SetColor(mEdgeColor);
        textPaint->SetStyle(PaintStyle_FILL_AND_STROKE);

        for (Int32 i = 0; i < lineCount; ++i) {
            ILayout::Probe(layout)->DrawText(c, i, i);
        }
    }
    else if (edgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_DROP_SHADOW) {
        textPaint->SetShadowLayer(mShadowRadius, mShadowOffsetX, mShadowOffsetY, mEdgeColor);
    }
    else if (edgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_RAISED
            || edgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_DEPRESSED) {
        Boolean raised = edgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_RAISED;
        Int32 colorUp = raised ? IColor::WHITE : mEdgeColor;
        Int32 colorDown = raised ? mEdgeColor : IColor::WHITE;
        Float offset = mShadowRadius / 2.0f;

        textPaint->SetColor(mForegroundColor);
        textPaint->SetStyle(PaintStyle_FILL);
        textPaint->SetShadowLayer(mShadowRadius, -offset, -offset, colorUp);

        for (Int32 i = 0; i < lineCount; ++i) {
            ILayout::Probe(layout)->DrawText(c, i, i);
        }
        textPaint->SetShadowLayer(mShadowRadius, offset, offset, colorDown);
    }

    textPaint->SetColor(mForegroundColor);
    textPaint->SetStyle(PaintStyle_FILL);

    for (Int32 i = 0; i < lineCount; ++i) {
        ILayout::Probe(layout)->DrawText(c, i, i);
    }

    textPaint->SetShadowLayer(0, 0, 0, 0);
    c->RestoreToCount(saveCount);
}

Boolean SubtitleView::ComputeMeasurements(
    /* [in] */ Int32 maxWidth)
{
    // ==================before translated======================
    // if (mHasMeasurements && maxWidth == mLastMeasuredWidth) {
    //     return true;
    // }
    //
    // // Account for padding.
    // final int paddingX = mPaddingLeft + mPaddingRight + mInnerPaddingX * 2;
    // maxWidth -= paddingX;
    // if (maxWidth <= 0) {
    //     return false;
    // }
    //
    // // TODO: Implement minimum-difference line wrapping. Adding the results
    // // of Paint.getTextWidths() seems to return different values than
    // // StaticLayout.getWidth(), so this is non-trivial.
    // mHasMeasurements = true;
    // mLastMeasuredWidth = maxWidth;
    // mLayout = new StaticLayout(
    //         mText, mTextPaint, maxWidth, mAlignment, mSpacingMult, mSpacingAdd, true);
    //
    // return true;

    if (mHasMeasurements && maxWidth == mLastMeasuredWidth) {
        return TRUE;
    }

    // Account for padding.
    Int32 paddingX = mPaddingLeft + mPaddingRight + mInnerPaddingX * 2;
    maxWidth -= paddingX;
    if (maxWidth <= 0) {
        return FALSE;
    }

    // TODO: Implement minimum-difference line wrapping. Adding the results
    // of Paint.getTextWidths() seems to return different values than
    // StaticLayout.getWidth(), so this is non-trivial.
    mHasMeasurements = TRUE;
    mLastMeasuredWidth = maxWidth;
    CStaticLayout::New(mText, mTextPaint, maxWidth, mAlignment, mSpacingMult, mSpacingAdd, TRUE, (IStaticLayout**)&mLayout);
    return TRUE;
}

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos


