#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/media/CClosedCaptionRenderer.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/style/CStyleSpan.h"
#include "elastos/droid/text/style/CUnderlineSpan.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"
#include "elastos/droid/view/View.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::PaintJoin;
using Elastos::Droid::Graphics::PaintStyle;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::Text::CSpannableStringBuilder;
using Elastos::Droid::Text::EIID_IParcelableSpan;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Style::CStyleSpan;
using Elastos::Droid::Text::Style::CUnderlineSpan;
using Elastos::Droid::Text::Style::EIID_IUpdateAppearance;
using Elastos::Droid::Text::Style::IStyleSpan;
using Elastos::Droid::Text::Style::IUnderlineSpan;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::Accessibility::EIID_ICaptioningManagerCaptioningChangeListener;
using Elastos::Droid::View::Accessibility::CaptioningManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CClosedCaptionRenderer::CCParser::CCData, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CClosedCaptionRenderer::CCParser::CCLineBuilder, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CClosedCaptionRenderer::CCParser::StyleCode, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox, IInterface)
DEFINE_CONVERSION_FOR(Elastos::Droid::Media::CClosedCaptionRenderer::MutableBackgroundColorSpan, IInterface)

namespace Elastos {
namespace Droid {
namespace Media {

//================================================================================
//  CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener
//================================================================================

CAR_INTERFACE_IMPL(CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener,
        Object, ICaptioningManagerCaptioningChangeListener)

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener::OnEnabledChanged(
    /* [in] */ Boolean enabled)
{
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener::OnUserStyleChanged(
    /* [in] */ ICaptioningManagerCaptionStyle* userStyle)
{
    DEFAULT_CAPTION_STYLE->ApplyStyle(userStyle, (ICaptioningManagerCaptionStyle**)&mHost->mCaptionStyle);
    mHost->mClosedCaptionLayout->SetCaptionStyle(mHost->mCaptionStyle);
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener::OnLocaleChanged(
    /* [in] */ ILocale* locale)
{
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CaptioningChangeListener::OnFontScaleChanged(
    /* [in] */ Float fontScale)
{
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout
//================================================================================

Int32 CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::MAX_ROWS = CCParser::MAX_ROWS;
Float CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::SAFE_AREA_RATIO = 0.9f;

CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::CCLayout(
    /* [in] */ IContext* context)
{
    mLineBoxes = ArrayOf<CCLineBox*>::Alloc(MAX_ROWS);

    LinearLayout::constructor(context);
    SetGravity(IGravity::START);
    SetOrientation(ILinearLayout::VERTICAL);
    for (Int32 i = 0; i < MAX_ROWS; i++) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<CCLineBox> cb = new CCLineBox(ctx);
        mLineBoxes->Set(i, cb);
        AddView((*mLineBoxes)[i], IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    }
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle)
{
    for (Int32 i = 0; i < MAX_ROWS; i++) {
        (*mLineBoxes)[i]->SetCaptionStyle(captionStyle);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::Update(
    /* [in] */ ArrayOf<ISpannableStringBuilder*>* textBuffer)
{
    for (Int32 i = 0; i < MAX_ROWS; i++) {
        if ((*textBuffer)[i] != NULL) {
            (*mLineBoxes)[i]->SetText(ICharSequence::Probe((*textBuffer)[i]), Elastos::Droid::Widget::BufferType_SPANNABLE);
            (*mLineBoxes)[i]->SetVisibility(IView::VISIBLE);
        }
        else {
            (*mLineBoxes)[i]->SetVisibility(IView::INVISIBLE);
        }
    }
    return NOERROR;
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    LinearLayout::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 safeWidth;
    GetMeasuredWidth(&safeWidth);
    Int32 safeHeight;
    GetMeasuredHeight(&safeHeight);

    // CEA-608 assumes 4:3 video
    if (safeWidth * 3 >= safeHeight * 4) {
        safeWidth = safeHeight * 4 / 3;
    }
    else {
        safeHeight = safeWidth * 3 / 4;
    }
    safeWidth *= SAFE_AREA_RATIO;
    safeHeight *= SAFE_AREA_RATIO;

    Int32 lineHeight = safeHeight / MAX_ROWS;
    Int32 lineHeightMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
            lineHeight, MeasureSpec::EXACTLY);
    Int32 lineWidthMeasureSpec = Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(
            safeWidth, MeasureSpec::EXACTLY);

    for (Int32 i = 0; i < MAX_ROWS; i++) {
        (*mLineBoxes)[i]->Measure(lineWidthMeasureSpec, lineHeightMeasureSpec);
    }
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CCLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    // safe caption area
    Int32 viewPortWidth = r - l;
    Int32 viewPortHeight = b - t;
    Int32 safeWidth, safeHeight;
    // CEA-608 assumes 4:3 video
    if (viewPortWidth * 3 >= viewPortHeight * 4) {
        safeWidth = viewPortHeight * 4 / 3;
        safeHeight = viewPortHeight;
    }
    else {
        safeWidth = viewPortWidth;
        safeHeight = viewPortWidth * 3 / 4;
    }
    safeWidth *= SAFE_AREA_RATIO;
    safeHeight *= SAFE_AREA_RATIO;
    Int32 left = (viewPortWidth - safeWidth) / 2;
    Int32 top = (viewPortHeight - safeHeight) / 2;

    for (Int32 i = 0; i < MAX_ROWS; i++) {
        (*mLineBoxes)[i]->Layout(
                left,
                top + safeHeight * i / MAX_ROWS,
                left + safeWidth,
                top + safeHeight * (i + 1) / MAX_ROWS);
    }
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox
//================================================================================

Float CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::FONT_PADDING_RATIO = 0.75f;
Float CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::EDGE_OUTLINE_RATIO = 0.1f;
Float CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::EDGE_SHADOW_RATIO = 0.05f;

CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::CCLineBox(
    /* [in] */ IContext* context)
    : mOutlineWidth(0)
    , mShadowRadius(0)
    , mShadowOffset(0)
    , mTextColor(IColor::WHITE)
    , mBgColor(IColor::BLACK)
    , mEdgeType(ICaptioningManagerCaptionStyle::EDGE_TYPE_NONE)
    , mEdgeColor(IColor::TRANSPARENT)
{
    TextView::constructor(context);
    SetGravity(IGravity::CENTER);
    SetBackgroundColor(IColor::TRANSPARENT);
    SetTextColor(IColor::WHITE);
    SetTypeface(Typeface::MONOSPACE);
    SetVisibility(IView::INVISIBLE);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);

    // get the default (will be updated later during measure)
    res->GetDimensionPixelSize(R::dimen::subtitle_outline_width, (Int32*)&mOutlineWidth);
    res->GetDimensionPixelSize(R::dimen::subtitle_shadow_radius, (Int32*)&mShadowRadius);
    res->GetDimensionPixelSize(R::dimen::subtitle_shadow_offset, (Int32*)&mShadowOffset);
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle)
{
    captionStyle->GetForegroundColor(&mTextColor);
    captionStyle->GetBackgroundColor(&mBgColor);
    captionStyle->GetEdgeType(&mEdgeType);
    captionStyle->GetEdgeColor(&mEdgeColor);

    SetTextColor(mTextColor);
    if (mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_DROP_SHADOW) {
        SetShadowLayer(mShadowRadius, mShadowOffset, mShadowOffset, mEdgeColor);
    }
    else {
        SetShadowLayer(0, 0, 0, 0);
    }
    Invalidate();
    return NOERROR;
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Float fontSize = Elastos::Droid::View::View::MeasureSpec::GetSize(heightMeasureSpec)
            * FONT_PADDING_RATIO;
    SetTextSize(ITypedValue::COMPLEX_UNIT_PX, fontSize);

    mOutlineWidth = EDGE_OUTLINE_RATIO * fontSize + 1.0f;
    mShadowRadius = EDGE_SHADOW_RATIO * fontSize + 1.0f;;
    mShadowOffset = mShadowRadius;

    // set font scale in the X direction to match the required width
    SetScaleX(1.0f);
    AutoPtr<ITextPaint> paint;
    GetPaint((ITextPaint**)&paint);
    IPaint::Probe(paint)->GetTextBounds(mDummyText, 0, mDummyText.GetLength(), mTextBounds);
    Int32 actualTextWidth;
    mTextBounds->GetWidth(&actualTextWidth);
    Int32 requiredTextWidth = Elastos::Droid::View::View::MeasureSpec::GetSize(widthMeasureSpec);
    SetScaleX(requiredTextWidth / actualTextWidth);

    TextView::OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::OnDraw(
    /* [in] */ ICanvas* c)
{
    if (mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_UNSPECIFIED
            || mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_NONE
            || mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_DROP_SHADOW) {
        // these edge styles don't require a second pass
        TextView::OnDraw(c);
    }

    if (mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_OUTLINE) {
        DrawEdgeOutline(c);
    }
    else {
        // Raised or depressed
        DrawEdgeRaisedOrDepressed(c);
    }
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::DrawEdgeOutline(
    /* [in] */ ICanvas* c)
{
    AutoPtr<ITextPaint> textPaint;
    GetPaint((ITextPaint**)&textPaint);

    PaintStyle previousStyle;
    IPaint::Probe(textPaint)->GetStyle(&previousStyle);
    PaintJoin previousJoin;
    IPaint::Probe(textPaint)->GetStrokeJoin(&previousJoin);
    Float previousWidth;
    IPaint::Probe(textPaint)->GetStrokeWidth(&previousWidth);

    SetTextColor(mEdgeColor);
    IPaint::Probe(textPaint)->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL_AND_STROKE);
    IPaint::Probe(textPaint)->SetStrokeJoin(Elastos::Droid::Graphics::PaintJoin_ROUND);
    IPaint::Probe(textPaint)->SetStrokeWidth(mOutlineWidth);

    // Draw outline and background only.
    TextView::OnDraw(c);

    // Restore original settings.
    SetTextColor(mTextColor);
    IPaint::Probe(textPaint)->SetStyle(previousStyle);
    IPaint::Probe(textPaint)->SetStrokeJoin(previousJoin);
    IPaint::Probe(textPaint)->SetStrokeWidth(previousWidth);

    // Remove the background.
    SetBackgroundSpans(IColor::TRANSPARENT);
    // Draw foreground only.
    TextView::OnDraw(c);
    // Restore the background.
    SetBackgroundSpans(mBgColor);
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::DrawEdgeRaisedOrDepressed(
    /* [in] */ ICanvas* c)
{
    AutoPtr<ITextPaint> textPaint;
    GetPaint((ITextPaint**)&textPaint);

    PaintStyle previousStyle;
    IPaint::Probe(textPaint)->GetStyle(&previousStyle);
    IPaint::Probe(textPaint)->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);

    Boolean raised = mEdgeType == ICaptioningManagerCaptionStyle::EDGE_TYPE_RAISED;
    Int32 colorUp = raised ? IColor::WHITE : mEdgeColor;
    Int32 colorDown = raised ? mEdgeColor : IColor::WHITE;
    Float offset = mShadowRadius / 2;

    // Draw background and text with shadow up
    SetShadowLayer(mShadowRadius, -offset, -offset, colorUp);
    TextView::OnDraw(c);

    // Remove the background.
    SetBackgroundSpans(IColor::TRANSPARENT);

    // Draw text with shadow down
    SetShadowLayer(mShadowRadius, +offset, +offset, colorDown);
    TextView::OnDraw(c);

    // Restore settings
    IPaint::Probe(textPaint)->SetStyle(previousStyle);

    // Restore the background.
    SetBackgroundSpans(mBgColor);
}

void CClosedCaptionRenderer::ClosedCaptionWidget::CCLineBox::SetBackgroundSpans(
    /* [in] */ Int32 color)
{
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    if (ISpannable::Probe(text) != NULL) {
        AutoPtr<ISpannable> spannable = ISpannable::Probe(text);
        AutoPtr<ArrayOf<IInterface*> > bgSpans;
        Int32 len;
        ICharSequence::Probe(spannable)->GetLength(&len);
        ISpanned::Probe(spannable)->GetSpans(
                0, len, EIID_IClosedCaptionRendererMutableBackgroundColorSpan,
                (ArrayOf<IInterface*>**)&bgSpans);

        for (Int32 i = 0; i < bgSpans->GetLength(); i++) {
            AutoPtr<IClosedCaptionRendererMutableBackgroundColorSpan> span =
                IClosedCaptionRendererMutableBackgroundColorSpan::Probe((*bgSpans)[i]);
            span->SetBackgroundColor(color);
        }
    }
}

//================================================================================
//  CClosedCaptionRenderer::ClosedCaptionWidget
//================================================================================

CAR_INTERFACE_IMPL_2(CClosedCaptionRenderer::ClosedCaptionWidget, ViewGroup,
        ISubtitleTrackRenderingWidget, IClosedCaptionRendererCCParserDisplayListener)

String CClosedCaptionRenderer::ClosedCaptionWidget::TAG("ClosedCaptionWidget");

AutoPtr<IRect> CClosedCaptionRenderer::ClosedCaptionWidget::mTextBounds;
String CClosedCaptionRenderer::ClosedCaptionWidget::mDummyText("1234567890123456789012345678901234");
AutoPtr<ICaptioningManagerCaptionStyle> CClosedCaptionRenderer::ClosedCaptionWidget::DEFAULT_CAPTION_STYLE;

void CClosedCaptionRenderer::ClosedCaptionWidget::Init()
{
    CRect::New((IRect**)&mTextBounds);
    DEFAULT_CAPTION_STYLE = CaptioningManager::CaptionStyle::DEFAULT;
}

CClosedCaptionRenderer::ClosedCaptionWidget::ClosedCaptionWidget(
    /* [in] */ IContext* context)
    : mHasChangeListener(FALSE)
{
    ClosedCaptionWidget(context, NULL);
}

CClosedCaptionRenderer::ClosedCaptionWidget::ClosedCaptionWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mHasChangeListener(FALSE)
{
    ClosedCaptionWidget(context, NULL, 0);
}

CClosedCaptionRenderer::ClosedCaptionWidget::ClosedCaptionWidget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mHasChangeListener(FALSE)
{
    AutoPtr<CaptioningChangeListener> l = new CaptioningChangeListener(this);
    mCaptioningListener = ICaptioningManagerCaptioningChangeListener::Probe(l);
    ViewGroup::constructor(context, attrs, defStyle);

    // Cannot render text over video when layer type is hardware.
    SetLayerType(IView::LAYER_TYPE_SOFTWARE, NULL);

    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::CAPTIONING_SERVICE, ((IInterface**)&service));
    mManager = ICaptioningManager::Probe(service);

    AutoPtr<ICaptioningManagerCaptionStyle> style;
    mManager->GetUserStyle((ICaptioningManagerCaptionStyle**)&style);
    DEFAULT_CAPTION_STYLE->ApplyStyle(style, (ICaptioningManagerCaptionStyle**)&mCaptionStyle);

    mClosedCaptionLayout = new CCLayout(context);
    mClosedCaptionLayout->SetCaptionStyle(mCaptionStyle);
    AddView(mClosedCaptionLayout, IViewGroupLayoutParams::MATCH_PARENT, IViewGroupLayoutParams::MATCH_PARENT);

    RequestLayout();
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::SetOnChangedListener(
    /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 widthSpec;
    Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(width,
            Elastos::Droid::View::View::MeasureSpec::EXACTLY);
    Int32 heightSpec;
    Elastos::Droid::View::View::MeasureSpec::MakeMeasureSpec(height,
            Elastos::Droid::View::View::MeasureSpec::EXACTLY);

    Measure(widthSpec, heightSpec);
    Layout(0, 0, width, height);
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::SetVisible(
    /* [in] */ Boolean visible)
{
    if (visible) {
        SetVisibility(IView::VISIBLE);
    }
    else {
        SetVisibility(IView::GONE);
    }

    ManageChangeListener();
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();

    ManageChangeListener();
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();

    ManageChangeListener();
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::OnDisplayChanged(
    /* [in] */ ArrayOf<ISpannableStringBuilder*>* styledTexts)
{
    mClosedCaptionLayout->Update(styledTexts);

    if (mListener != NULL) {
        mListener->OnChanged(this);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::GetCaptionStyle(
    /* [out] */ ICaptioningManagerCaptionStyle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCaptionStyle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::Draw(
    /* [in] */ ICanvas* c)
{
    return ViewGroup::Draw(c);
}

void CClosedCaptionRenderer::ClosedCaptionWidget::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ViewGroup::OnMeasure(widthMeasureSpec, heightMeasureSpec);
    mClosedCaptionLayout->Measure(widthMeasureSpec, heightMeasureSpec);
}

ECode CClosedCaptionRenderer::ClosedCaptionWidget::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    return mClosedCaptionLayout->Layout(l, t, r, b);
}

void CClosedCaptionRenderer::ClosedCaptionWidget::ManageChangeListener()
{
    Boolean b;
    IsAttachedToWindow(&b);
    Int32 v;
    GetVisibility(&v);
    Boolean needsListener = b && v == IView::VISIBLE;
    if (mHasChangeListener != needsListener) {
        mHasChangeListener = needsListener;

        if (needsListener) {
            mManager->AddCaptioningChangeListener(ICaptioningManagerCaptioningChangeListener::Probe(mCaptioningListener));
        }
        else {
            mManager->RemoveCaptioningChangeListener(ICaptioningManagerCaptioningChangeListener::Probe(mCaptioningListener));
        }
    }
}

//================================================================================
//  CClosedCaptionRenderer::MutableBackgroundColorSpan
//================================================================================

CAR_INTERFACE_IMPL_3(CClosedCaptionRenderer::MutableBackgroundColorSpan,
        CharacterStyle, IUpdateAppearance, IParcelableSpan, IParcelable)

CClosedCaptionRenderer::MutableBackgroundColorSpan::MutableBackgroundColorSpan(
    /* [in] */ Int32 color)
{
    mColor = color;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    mColor = color;
    return NOERROR;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::GetBackgroundColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mColor;
    return NOERROR;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::GetSpanTypeId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ITextUtils::BACKGROUND_COLOR_SPAN;
    return NOERROR;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mColor);
    return NOERROR;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mColor);
    return NOERROR;
}

ECode CClosedCaptionRenderer::MutableBackgroundColorSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    return ds->SetBgColor(mColor);
}

//================================================================================
//  CClosedCaptionRenderer::CCParser::CCData
//================================================================================

static AutoPtr<ArrayOf<String> > InitCtrlCodeMap()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    array->Set(0, String("RCL"));
    array->Set(1, String("BS"));
    array->Set(2, String("AOF"));
    array->Set(3, String("AON"));
    array->Set(4, String("DER"));
    array->Set(5, String("RU2"));
    array->Set(6, String("RU3"));
    array->Set(7, String("RU4"));
    array->Set(8, String("FON"));
    array->Set(9, String("RDC"));
    array->Set(10, String("TR"));
    array->Set(11, String("RTD"));
    array->Set(12, String("EDM"));
    array->Set(13, String("CR"));
    array->Set(14, String("ENM"));
    array->Set(15, String("EOC"));
    return array;
}

AutoPtr<ArrayOf<String> > CClosedCaptionRenderer::CCParser::CCData::mCtrlCodeMap = InitCtrlCodeMap();

static AutoPtr<ArrayOf<String> > InitSpecialCharMap()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    array->Set(0, String("\u00AE"));
    array->Set(1, String("\u00B0"));
    array->Set(2, String("\u00BD"));
    array->Set(3, String("\u00BF"));
    array->Set(4, String("\u2122"));
    array->Set(5, String("\u00A2"));
    array->Set(6, String("\u00A3"));
    array->Set(7, String("\u266A"));
    array->Set(8, String("\u00E0"));
    array->Set(9, String("\u00A0"));
    array->Set(10, String("\u00E8"));
    array->Set(11, String("\u00E2"));
    array->Set(12, String("\u00EA"));
    array->Set(13, String("\u00EE"));
    array->Set(14, String("\u00F4"));
    array->Set(15, String("\u00FB"));
    return array;
}

AutoPtr<ArrayOf<String> > CClosedCaptionRenderer::CCParser::CCData::mSpecialCharMap = InitSpecialCharMap();

static AutoPtr<ArrayOf<String> > InitSpanishCharMap()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    // Spanish and misc chars
    array->Set(0, String("\u00C1"));
    array->Set(1, String("\u00C9"));
    array->Set(2, String("\u00D3"));
    array->Set(3, String("\u00DA"));
    array->Set(4, String("\u00DC"));
    array->Set(5, String("\u00FC"));
    array->Set(6, String("\u2018"));
    array->Set(7, String("\u00A1"));
    array->Set(8, String("*"));
    array->Set(9, String("'"));
    array->Set(10, String("\u2014"));
    array->Set(11, String("\u00A9"));
    array->Set(12, String("\u2120"));
    array->Set(13, String("\u2022"));
    array->Set(14, String("\u201C"));
    array->Set(15, String("\u201D"));
    // French
    array->Set(16, String("\u00C0"));
    array->Set(17, String("\u00C2"));
    array->Set(18, String("\u00C7"));
    array->Set(19, String("\u00C8"));
    array->Set(20, String("\u00CA"));
    array->Set(21, String("\u00CB"));
    array->Set(22, String("\u00EB"));
    array->Set(23, String("\u00CE"));
    array->Set(24, String("\u00CF"));
    array->Set(25, String("\u00EF"));
    array->Set(26, String("\u00D4"));
    array->Set(27, String("\u00D9"));
    array->Set(28, String("\u00F9"));
    array->Set(29, String("\u00DB"));
    array->Set(30, String("\u00AB"));
    array->Set(31, String("\u00BB"));
    return array;
}

AutoPtr<ArrayOf<String> > CClosedCaptionRenderer::CCParser::CCData::mSpanishCharMap = InitSpanishCharMap();

static AutoPtr<ArrayOf<String> > InitProtugueseCharMap()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(12);
    // Portuguese
    array->Set(0, String("\u00C3"));
    array->Set(1, String("\u00E3"));
    array->Set(2, String("\u00CD"));
    array->Set(3, String("\u00CC"));
    array->Set(4, String("\u00EC"));
    array->Set(5, String("\u00D2"));
    array->Set(6, String("\u00F2"));
    array->Set(7, String("\u00D5"));
    array->Set(8, String("\u00F5"));
    array->Set(9, String("{"));
    array->Set(10, String("}"));
    array->Set(11, String("\\"));
    array->Set(12, String("^"));
    array->Set(13, String("_"));
    array->Set(14, String("|"));
    array->Set(15, String("~"));
    // German and misc chars
    array->Set(16, String("\u00C4"));
    array->Set(17, String("\u00E4"));
    array->Set(18, String("\u00D6"));
    array->Set(19, String("\u00F6"));
    array->Set(20, String("\u00DF"));
    array->Set(21, String("\u00A5"));
    array->Set(22, String("\u00A4"));
    array->Set(23, String("\u2502"));
    array->Set(24, String("\u00C5"));
    array->Set(25, String("\u00E5"));
    array->Set(26, String("\u00D8"));
    array->Set(27, String("\u00F8"));
    array->Set(28, String("\u250C"));
    array->Set(29, String("\u2510"));
    array->Set(30, String("\u2514"));
    array->Set(31, String("\u2518"));
    return array;
}

AutoPtr<ArrayOf<String> > CClosedCaptionRenderer::CCParser::CCData::mProtugueseCharMap = InitProtugueseCharMap();

CClosedCaptionRenderer::CCParser::CCData::CCData(
    /* [in] */ byte type,
    /* [in] */ byte data1,
    /* [in] */ byte data2)
    : mType(type)
    , mData1(data1)
    , mData2(data2)
{
}

ECode CClosedCaptionRenderer::CCParser::CCData::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ArrayOf<CCData*>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<CCData*> > ccData = ArrayOf<CCData*>::Alloc(data->GetLength() / 3);

    for (Int32 i = 0; i < ccData->GetLength(); i++) {
        (*ccData)[i] = new CCData(
                (*data)[i * 3],
                (*data)[i * 3 + 1],
                (*data)[i * 3 + 2]);
    }

    *result = ccData;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetCtrlCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mData1 == 0x14 || mData1 == 0x1c)
            && mData2 >= 0x20 && mData2 <= 0x2f) {
        *result = mData2;
        return NOERROR;
    }
    *result = INVALID;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetMidRow(
    /* [out] */ StyleCode** result)
{
    VALIDATE_NOT_NULL(result)
    // only support standard Mid-row codes, ignore
    // optional background/foreground mid-row codes
    if ((mData1 == 0x11 || mData1 == 0x19)
            && mData2 >= 0x20 && mData2 <= 0x2f) {
        return StyleCode::FromByte(mData2, result);
    }
    *result = NULL;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetPAC(
    /* [out] */ PAC** result)
{
    VALIDATE_NOT_NULL(result)
    if ((mData1 & 0x70) == 0x10
            && (mData2 & 0x40) == 0x40
            && ((mData1 & 0x07) != 0 || (mData2 & 0x20) == 0)) {
        return PAC::FromBytes(mData1, mData2, result);
    }
    *result = NULL;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetTabOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mData1 == 0x17 || mData1 == 0x1f)
            && mData2 >= 0x21 && mData2 <= 0x23) {
        *result = mData2 & 0x3;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::IsDisplayableChar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b1, b2, b3;
    IsBasicChar(&b1);
    IsSpecialChar(&b2);
    IsExtendedChar(&b3);
    *result = b1 || b2 || b3;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetDisplayText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    GetBasicChars(&str);

    if (str == NULL) {
        GetSpecialChar(&str);

        if (str == NULL) {
            GetExtendedChar(&str);
        }
    }

    *result = str;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mData1 < 0x10 && mData2 < 0x10) {
        // Null Pad, ignore
        // String.format("[%d]Null: %02x %02x", mType, mData1, mData2);
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]Null: ");
        sb.Append(mData1);
        sb.Append(" ");
        sb.Append(mData2);
        return sb.ToString(result);
    }

    Int32 ctrlCode;
    GetCtrlCode(&ctrlCode);
    if (ctrlCode != INVALID) {
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]");
        String s;
        CtrlCodeToString(ctrlCode, &s);
        sb.Append(s);
        return sb.ToString(result);
    }

    Int32 tabOffset;
    GetTabOffset(&tabOffset);
    if (tabOffset > 0) {
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]Tab");
        sb.Append(tabOffset);
        return sb.ToString(result);
    }

    AutoPtr<PAC> pac;
    GetPAC((PAC**)&pac);
    if (pac != NULL) {
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]PAC: ");
        String s;
        pac->ToString(&s);
        sb.Append(s);
        return sb.ToString(result);
    }

    AutoPtr<StyleCode> m;
    GetMidRow((StyleCode**)&m);
    if (m != NULL) {
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]Mid-row: ");
        String s;
        m->ToString(&s);
        sb.Append(s);
        return sb.ToString(result);
    }

    Boolean b;
    if (IsDisplayableChar(&b), b) {
        String str;
        GetDisplayText(&str);
        // return String.format("[%d]Displayable: %s (%02x %02x)",
        //         mType, str, mData1, mData2);
        StringBuilder sb("[");
        sb.Append(mType);
        sb.Append("]Displayable: ");
        sb.Append(str);
        sb.Append(" (");
        sb.Append(mData1);
        sb.Append(" ");
        sb.Append(mData2);
        sb.Append(")");
        return sb.ToString(result);
    }

    // return String.format("[%d]Invalid: %02x %02x", mType, mData1, mData2);
    StringBuilder sb("[");
    sb.Append(mType);
    sb.Append("]Invalid: ");
    sb.Append(mData1);
    sb.Append(" ");
    sb.Append(mData2);
    return sb.ToString(result);
}

ECode CClosedCaptionRenderer::CCParser::CCData::CtrlCodeToString(
    /* [in] */ Int32 ctrlCode,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mCtrlCodeMap)[ctrlCode - 0x20];
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::IsBasicChar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mData1 >= 0x20 && mData1 <= 0x7f;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::IsSpecialChar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((mData1 == 0x11 || mData1 == 0x19)
            && mData2 >= 0x30 && mData2 <= 0x3f);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::IsExtendedChar(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ((mData1 == 0x12 || mData1 == 0x1A
            || mData1 == 0x13 || mData1 == 0x1B)
            && mData2 >= 0x20 && mData2 <= 0x3f);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetBasicChar(
    /* [in] */ Byte data,
    /* [out] */ Char32* result)
{
    VALIDATE_NOT_NULL(result)
    Char32 c;
    // replace the non-ASCII ones
    switch (data) {
        case 0x2A: c = '\u00E1'; break;
        case 0x5C: c = '\u00E9'; break;
        case 0x5E: c = '\u00ED'; break;
        case 0x5F: c = '\u00F3'; break;
        case 0x60: c = '\u00FA'; break;
        case 0x7B: c = '\u00E7'; break;
        case 0x7C: c = '\u00F7'; break;
        case 0x7D: c = '\u00D1'; break;
        case 0x7E: c = '\u00F1'; break;
        case 0x7F: c = '\u2588'; break; // Full block
        default: c = (Char32) data; break;
    }
    *result = c;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetBasicChars(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mData1 >= 0x20 && mData1 <= 0x7f) {
        StringBuilder builder;
        Char32 c;
        GetBasicChar(mData1, &c);
        builder.AppendChar(c);
        if (mData2 >= 0x20 && mData2 <= 0x7f) {
            GetBasicChar(mData2, &c);
            builder.AppendChar(c);
        }
        return builder.ToString(result);
    }

    *result = String(NULL);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetSpecialChar(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mData1 == 0x11 || mData1 == 0x19)
            && mData2 >= 0x30 && mData2 <= 0x3f) {
        *result = (*mSpecialCharMap)[mData2 - 0x30];
        return NOERROR;
    }

    *result = String(NULL);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCData::GetExtendedChar(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if ((mData1 == 0x12 || mData1 == 0x1A)
            && mData2 >= 0x20 && mData2 <= 0x3f){
        // 1 Spanish/French char
        *result = (*mSpanishCharMap)[mData2 - 0x20];
        return NOERROR;
    }
    else if ((mData1 == 0x13 || mData1 == 0x1B)
            && mData2 >= 0x20 && mData2 <= 0x3f){
        // 1 Portuguese/German/Danish char
        *result = (*mProtugueseCharMap)[mData2 - 0x20];
        return NOERROR;
    }

    *result = String(NULL);
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer::CCParser::CCMemory
//================================================================================

CClosedCaptionRenderer::CCParser::CCMemory::CCMemory()
    : mRow(0)
    , mCol(0)
{
    mLines = ArrayOf<CCLineBuilder*>::Alloc(MAX_ROWS + 2);
    AutoPtr<ArrayOf<Char32> > blank = ArrayOf<Char32>::Alloc(MAX_COLS + 2);
    Arrays::Fill(blank, TS);
    mBlankLine = String(*blank);
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Erase()
{
    // erase all lines
    for (Int32 i = 0; i < mLines->GetLength(); i++) {
        mLines->Set(i, NULL);
    }
    mRow = MAX_ROWS;
    mCol = 1;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Der()
{
    if ((*mLines)[mRow] != NULL) {
        for (Int32 i = 0; i < mCol; i++) {
            Char32 val;
            (*mLines)[mRow]->CharAt(i, &val);
            if (val != TS) {
                Int32 len;
                (*mLines)[mRow]->Length(&len);
                for (Int32 j = mCol; j < len; j++) {
                    (*mLines)[j]->SetCharAt(j, TS);
                }
                return NOERROR;
            }
        }
        mLines->Set(mRow, NULL);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Tab(
    /* [in] */ Int32 tabs)
{
    return MoveCursorByCol(tabs);
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Bs()
{
    MoveCursorByCol(-1);
    if ((*mLines)[mRow] != NULL) {
        (*mLines)[mRow]->SetCharAt(mCol, TS);
        if (mCol == MAX_COLS - 1) {
            // Spec recommendation:
            // if cursor was at col 32, move cursor
            // back to col 31 and erase both col 31&32
            (*mLines)[mRow]->SetCharAt(MAX_COLS, TS);
        }
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Cr()
{
    return MoveCursorTo(mRow + 1, 1);
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::RollUp(
    /* [in] */ Int32 windowSize)
{
    Int32 i;
    for (i = 0; i <= mRow - windowSize; i++) {
        mLines->Set(i, NULL);
    }
    Int32 startRow = mRow - windowSize + 1;
    if (startRow < 1) {
        startRow = 1;
    }
    for (i = startRow; i < mRow; i++) {
        mLines->Set(i, (*mLines)[i + 1]);
    }
    for (i = mRow; i < mLines->GetLength(); i++) {
        // clear base row
        mLines->Set(i, NULL);
    }
    // default to col 1, in case PAC is not sent
    mCol = 1;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::WriteText(
    /* [in] */ const String& text)
{
    for (Int32 i = 0; i < text.GetLength(); i++) {
        AutoPtr<CCLineBuilder> cb;
        GetLineBuffer(mRow, (CCLineBuilder**)&cb);
        cb->SetCharAt(mCol, text.GetChar(i));
        MoveCursorByCol(1);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::WriteMidRowCode(
    /* [in] */ StyleCode* m)
{
    AutoPtr<CCLineBuilder> cb;
    GetLineBuffer(mRow, (CCLineBuilder**)&cb);
    cb->SetMidRowAt(mCol, m);
    MoveCursorByCol(1);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::WritePAC(
    /* [in] */ PAC* pac)
{
    Boolean b;
    pac->IsIndentPAC(&b);
    if (b) {
        Int32 row, col;
        pac->GetRow(&row);
        pac->GetCol(&col);
        MoveCursorTo(row, col);
    } else {
        Int32 row;
        pac->GetRow(&row);
        MoveCursorToRow(row);
    }
    AutoPtr<CCLineBuilder> cb;
    GetLineBuffer(mRow, (CCLineBuilder**)&cb);
    cb->SetPACAt(mCol, pac);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::GetStyledText(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [out] */ ArrayOf<ISpannableStringBuilder*>** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> rows;
    CArrayList::New((IArrayList**)&rows);

    for (Int32 i = 1; i <= MAX_ROWS; i++) {
        if ((*mLines)[i] != NULL) {
            AutoPtr<ISpannableStringBuilder> builder;
            (*mLines)[i]->GetStyledText(captionStyle, (ISpannableStringBuilder**)&builder);
            rows->Add(builder);
        }
        else {
            rows->Add(NULL);
        }
    }

    // return rows.toArray(new SpannableStringBuilder[MAX_ROWS]);
    AutoPtr<ArrayOf<IInterface*> > array;
    rows->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<ArrayOf<ISpannableStringBuilder*> > builders =
            ArrayOf<ISpannableStringBuilder*>::Alloc(array->GetLength());
    for (Int32 i = 0; i < array->GetLength(); i++) {
        builders->Set(i, ISpannableStringBuilder::Probe((*array)[i]));
    }
    *result = builders;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::Clamp(
    /* [in] */ Int32 x,
    /* [in] */ Int32 min,
    /* [in] */ Int32 max,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = x < min ? min : (x > max ? max : x);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::MoveCursorTo(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col)
{
    Clamp(row, 1, MAX_ROWS, &mRow);
    Clamp(col, 1, MAX_COLS, &mCol);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::MoveCursorToRow(
    /* [in] */ Int32 row)
{
    return Clamp(row, 1, MAX_ROWS, &mRow);
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::MoveCursorByCol(
    /* [in] */ Int32 col)
{
    return Clamp(mCol + col, 1, MAX_COLS, &mCol);
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::MoveBaselineTo(
    /* [in] */ Int32 baseRow,
    /* [in] */ Int32 windowSize)
{
    if (mRow == baseRow) {
        return NOERROR;
    }
    Int32 actualWindowSize = windowSize;
    if (baseRow < actualWindowSize) {
        actualWindowSize = baseRow;
    }
    if (mRow < actualWindowSize) {
        actualWindowSize = mRow;
    }

    Int32 i;
    if (baseRow < mRow) {
        // copy from bottom to top row
        for (i = actualWindowSize - 1; i >= 0; i--) {
            mLines->Set(baseRow - i, (*mLines)[mRow - i]);
        }
    }
    else {
        // copy from top to bottom row
        for (i = 0; i < actualWindowSize; i++) {
            mLines->Set(baseRow - i, (*mLines)[mRow - i]);
        }
    }
    // clear rest of the rows
    for (i = 0; i <= baseRow - windowSize; i++) {
        mLines->Set(i, NULL);
    }
    for (i = baseRow + 1; i < mLines->GetLength(); i++) {
        mLines->Set(i, NULL);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCMemory::GetLineBuffer(
    /* [in] */ Int32 row,
    /* [out] */ CCLineBuilder** result)
{
    if ((*mLines)[row] == NULL) {
        (*mLines)[row] = new CCLineBuilder(mBlankLine);
    }
    *result = (*mLines)[row];
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer::CCParser::CCLineBuilder
//================================================================================

CClosedCaptionRenderer::CCParser::CCLineBuilder::CCLineBuilder(
    /* [in] */ const String& str)
{
    mDisplayChars = new StringBuilder(str);
    Int32 len;
    mDisplayChars->GetLength(&len);
    mMidRowStyles = ArrayOf<StyleCode*>::Alloc(len);
    mPACStyles = ArrayOf<StyleCode*>::Alloc(len);
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::SetCharAt(
    /* [in] */ Int32 index,
    /* [in] */ Char32 ch)
{
    mDisplayChars->SetCharAt(index, ch);
    mMidRowStyles->Set(index, NULL);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::SetMidRowAt(
    /* [in] */ Int32 index,
    /* [in] */ StyleCode* m)
{
    mDisplayChars->SetCharAt(index, ' ');
    mMidRowStyles->Set(index, m);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::SetPACAt(
    /* [in] */ Int32 index,
    /* [in] */ PAC* pac)
{
    mPACStyles->Set(index, pac);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::CharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDisplayChars->GetCharAt(index, result);
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::Length(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDisplayChars->GetLength(result);
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::ApplyStyleSpan(
    /* [in] */ ISpannableStringBuilder* styledText,
    /* [in] */ StyleCode* s,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Boolean b;
    s->IsItalics(&b);
    if (b) {
        AutoPtr<IStyleSpan> span;
        CStyleSpan::New(ITypeface::ITALIC, (IStyleSpan**)&span);
        ISpannable::Probe(styledText)->SetSpan(span, start, end, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    }
    s->IsUnderline(&b);
    if (b) {
        AutoPtr<IUnderlineSpan> span;
        CUnderlineSpan::New((IUnderlineSpan**)&span);
        ISpannable::Probe(styledText)->SetSpan(span, start, end, ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
    }
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::CCLineBuilder::GetStyledText(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [out] */ ISpannableStringBuilder** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISpannableStringBuilder> styledText;
    CSpannableStringBuilder::New(mDisplayChars, (ISpannableStringBuilder**)&styledText);
    Int32 start = -1, next = 0;
    Int32 styleStart = -1;
    AutoPtr<StyleCode> curStyle;

    Int32 len;
    mDisplayChars->GetLength(&len);
    while (next < len) {
        AutoPtr<StyleCode> newStyle;
        if ((*mMidRowStyles)[next] != NULL) {
            // apply mid-row style change
            newStyle = (*mMidRowStyles)[next];
        }
        else if ((*mPACStyles)[next] != NULL
            && (styleStart < 0 || start < 0)) {
            // apply PAC style change, only if:
            // 1. no style set, or
            // 2. style set, but prev char is none-displayable
            newStyle = (*mPACStyles)[next];
        }
        if (newStyle != NULL) {
            curStyle = newStyle;
            if (styleStart >= 0 && start >= 0) {
                ApplyStyleSpan(styledText, newStyle, styleStart, next);
            }
            styleStart = next;
        }

        if (mDisplayChars->GetCharAt(next) != TS) {
            if (start < 0) {
                start = next;
            }
        }
        else if (start >= 0) {
            Int32 expandedStart = mDisplayChars->GetCharAt(start) == ' ' ? start : start - 1;
            Int32 expandedEnd = mDisplayChars->GetCharAt(next - 1) == ' ' ? next : next + 1;

            Int32 color;
            captionStyle->GetBackgroundColor(&color);
            AutoPtr<MutableBackgroundColorSpan> span = new MutableBackgroundColorSpan(color);
            ISpannable::Probe(styledText)->SetSpan((IInterface*)(IObject*)span, expandedStart, expandedEnd,
                    ISpanned::SPAN_EXCLUSIVE_EXCLUSIVE);
            if (styleStart >= 0) {
                ApplyStyleSpan(styledText, curStyle, styleStart, expandedEnd);
            }
            start = -1;
        }
        next++;
    }

    *result = styledText;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer::CCParser::PAC
//================================================================================

CClosedCaptionRenderer::CCParser::PAC::PAC(
    /* [in] */ Int32 row,
    /* [in] */ Int32 col,
    /* [in] */ Int32 style,
    /* [in] */ Int32 color)
    : StyleCode(style, color)
    , mRow(row)
    , mCol(col)
{
}

ECode CClosedCaptionRenderer::CCParser::PAC::FromBytes(
    /* [in] */ byte data1,
    /* [in] */ byte data2,
    /* [out] */ PAC** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int32> > rowTable = ArrayOf<Int32>::Alloc(8);
    rowTable->Set(0, 11);
    rowTable->Set(1, 1);
    rowTable->Set(2, 3);
    rowTable->Set(3, 12);
    rowTable->Set(4, 14);
    rowTable->Set(5, 5);
    rowTable->Set(6, 7);
    rowTable->Set(7, 9);
    Int32 row = (*rowTable)[data1 & 0x07] + ((data2 & 0x20) >> 5);
    Int32 style = 0;
    if ((data2 & 1) != 0) {
        style |= STYLE_UNDERLINE;
    }
    if ((data2 & 0x10) != 0) {
        // indent code
        Int32 indent = (data2 >> 1) & 0x7;
        AutoPtr<PAC> pac = new PAC(row, indent * 4, style, COLOR_WHITE);
        *result = pac;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else {
        // style code
        Int32 color = (data2 >> 1) & 0x7;

        if (color == COLOR_INVALID) {
            // WHITE ITALICS
            color = COLOR_WHITE;
            style |= STYLE_ITALICS;
        }
        AutoPtr<PAC> pac = new PAC(row, -1, style, color);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
}

ECode CClosedCaptionRenderer::CCParser::PAC::IsIndentPAC(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mCol >= 0);
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::PAC::GetRow(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRow;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::PAC::GetCol(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCol;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::PAC::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("{");
    sb.Append(mRow);
    sb.Append(", ");
    sb.Append(mCol);
    sb.Append("}, ");
    String str;
    StyleCode::ToString(&str);
    sb.Append(str);
    return sb.ToString(result);
}

//================================================================================
//  CClosedCaptionRenderer::CCParser::StyleCode
//================================================================================

const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_WHITE = 0;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_GREEN = 1;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_BLUE = 2;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_CYAN = 3;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_RED = 4;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_YELLOW = 5;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_MAGENTA = 6;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::COLOR_INVALID = 7;

const Int32 CClosedCaptionRenderer::CCParser::StyleCode::STYLE_ITALICS = 0x00000001;
const Int32 CClosedCaptionRenderer::CCParser::StyleCode::STYLE_UNDERLINE = 0x00000002;

static AutoPtr<ArrayOf<String> > InitColorMap()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(8);
    array->Set(0, String("WHITE"));
    array->Set(1, String("GREEN"));
    array->Set(2, String("BLUE"));
    array->Set(3, String("CYAN"));
    array->Set(4, String("RED"));
    array->Set(5, String("YELLOW"));
    array->Set(6, String("MAGENTA"));
    array->Set(7, String("INVALID"));
    return array;
}

AutoPtr<ArrayOf<String> > CClosedCaptionRenderer::CCParser::StyleCode::mColorMap = InitColorMap();

CClosedCaptionRenderer::CCParser::StyleCode::StyleCode(
    /* [in] */ Int32 style,
    /* [in] */ Int32 color)
    : mStyle(style)
    , mColor(color)
{
}

ECode CClosedCaptionRenderer::CCParser::StyleCode::FromByte(
    /* [in] */ Byte data2,
    /* [out] */ StyleCode** result)
{
    VALIDATE_NOT_NULL(result)
    Int32 style = 0;
    Int32 color = (data2 >> 1) & 0x7;

    if ((data2 & 0x1) != 0) {
        style |= STYLE_UNDERLINE;
    }

    if (color == COLOR_INVALID) {
        // WHITE ITALICS
        color = COLOR_WHITE;
        style |= STYLE_ITALICS;
    }

    AutoPtr<StyleCode> sc = new StyleCode(style, color);
    *result = sc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::StyleCode::IsItalics(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mStyle & STYLE_ITALICS) != 0;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::StyleCode::IsUnderline(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mStyle & STYLE_UNDERLINE) != 0;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::StyleCode::GetColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mColor;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CCParser::StyleCode::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder str;
    str.Append("{");
    str.Append((*mColorMap)[mColor]);
    if ((mStyle & STYLE_ITALICS) != 0) {
        str.Append(", ITALICS");
    }
    if ((mStyle & STYLE_UNDERLINE) != 0) {
        str.Append(", UNDERLINE");
    }
    str.Append("}");

    return str.ToString(result);
}

//================================================================================
//  CClosedCaptionRenderer::CCParser
//================================================================================

const Int32 CClosedCaptionRenderer::CCParser::MAX_ROWS = 15;
const Int32 CClosedCaptionRenderer::CCParser::MAX_COLS = 32;

const String CClosedCaptionRenderer::CCParser::TAG("CCParser");
const Boolean CClosedCaptionRenderer::CCParser::DEBUG = FALSE; //Log.isLoggable(TAG, Log.DEBUG);

const Int32 CClosedCaptionRenderer::CCParser::INVALID = -1;

const Int32 CClosedCaptionRenderer::CCParser::RCL = 0x20;
const Int32 CClosedCaptionRenderer::CCParser::BS  = 0x21;
const Int32 CClosedCaptionRenderer::CCParser::AOF = 0x22;
const Int32 CClosedCaptionRenderer::CCParser::AON = 0x23;
const Int32 CClosedCaptionRenderer::CCParser::DER = 0x24;
const Int32 CClosedCaptionRenderer::CCParser::RU2 = 0x25;
const Int32 CClosedCaptionRenderer::CCParser::RU3 = 0x26;
const Int32 CClosedCaptionRenderer::CCParser::RU4 = 0x27;
const Int32 CClosedCaptionRenderer::CCParser::FON = 0x28;
const Int32 CClosedCaptionRenderer::CCParser::RDC = 0x29;
const Int32 CClosedCaptionRenderer::CCParser::TR  = 0x2a;
const Int32 CClosedCaptionRenderer::CCParser::RTD = 0x2b;
const Int32 CClosedCaptionRenderer::CCParser::EDM = 0x2c;
const Int32 CClosedCaptionRenderer::CCParser::CR  = 0x2d;
const Int32 CClosedCaptionRenderer::CCParser::ENM = 0x2e;
const Int32 CClosedCaptionRenderer::CCParser::EOC = 0x2f;

const Char32 CClosedCaptionRenderer::CCParser::TS = '\u00A0';

const Int32 CClosedCaptionRenderer::CCParser::MODE_UNKNOWN = 0;
const Int32 CClosedCaptionRenderer::CCParser::MODE_PAINT_ON = 1;
const Int32 CClosedCaptionRenderer::CCParser::MODE_ROLL_UP = 2;
const Int32 CClosedCaptionRenderer::CCParser::MODE_POP_ON = 3;
const Int32 CClosedCaptionRenderer::CCParser::MODE_TEXT = 4;

CClosedCaptionRenderer::CCParser::CCParser(
    /* [in] */ IClosedCaptionRendererCCParserDisplayListener* listener)
    : mMode(MODE_PAINT_ON)
    , mRollUpSize(4)
    , mListener(listener)
{
    mDisplay = new CCMemory();
    mNonDisplay = new CCMemory();
    mTextMem = new CCMemory();
}

ECode CClosedCaptionRenderer::CCParser::Parse(
    /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<ArrayOf<CCData*> > ccData;
    CCData::FromByteArray(data, (ArrayOf<CCData*>**)&ccData);

    for (Int32 i = 0; i < ccData->GetLength(); i++) {
        if (DEBUG) {
            String str;
            (*ccData)[i]->ToString(&str);
            Logger::D(TAG, str);
        }

        if (HandleCtrlCode((*ccData)[i])
                || HandleTabOffsets((*ccData)[i])
                || HandlePACCode((*ccData)[i])
                || HandleMidRowCode((*ccData)[i])) {
            continue;
        }

        HandleDisplayableChars((*ccData)[i]);
    }
    return NOERROR;
}

AutoPtr<CClosedCaptionRenderer::CCParser::CCMemory> CClosedCaptionRenderer::CCParser::GetMemory()
{
    // get the CC memory to operate on for current mode
    switch (mMode) {
    case MODE_POP_ON:
        return mNonDisplay;
    case MODE_TEXT:
        // TODO(chz): support only caption mode for now,
        // in text mode, dump everything to text mem.
        return mTextMem;
    case MODE_PAINT_ON:
    case MODE_ROLL_UP:
        return mDisplay;
    default:
        Logger::W(TAG, "unrecoginized mode: %d", mMode);
    }
    return mDisplay;
}

Boolean CClosedCaptionRenderer::CCParser::HandleDisplayableChars(
    /* [in] */ CCData* ccData)
{
    Boolean b;
    ccData->IsDisplayableChar(&b);
    if (!b) {
        return FALSE;
    }

    // Extended char includes 1 automatic backspace
    ccData->IsExtendedChar(&b);
    if (b) {
        GetMemory()->Bs();
    }

    String str;
    ccData->GetDisplayText(&str);
    GetMemory()->WriteText(str);

    if (mMode == MODE_PAINT_ON || mMode == MODE_ROLL_UP) {
        UpdateDisplay();
    }

    return TRUE;
}

Boolean CClosedCaptionRenderer::CCParser::HandleMidRowCode(
    /* [in] */ CCData* ccData)
{
    AutoPtr<StyleCode> m;
    ccData->GetMidRow((StyleCode**)&m);
    if (m != NULL) {
        GetMemory()->WriteMidRowCode(m);
        return TRUE;
    }
    return FALSE;
}

Boolean CClosedCaptionRenderer::CCParser::HandlePACCode(
    /* [in] */ CCData* ccData)
{
    AutoPtr<PAC> pac;
    ccData->GetPAC((PAC**)&pac);

    if (pac != NULL) {
        if (mMode == MODE_ROLL_UP) {
            Int32 row;
            pac->GetRow(&row);
            GetMemory()->MoveBaselineTo(row, mRollUpSize);
        }
        GetMemory()->WritePAC(pac);
        return TRUE;
    }

    return FALSE;
}

Boolean CClosedCaptionRenderer::CCParser::HandleTabOffsets(
    /* [in] */ CCData* ccData)
{
    Int32 tabs;
    ccData->GetTabOffset(&tabs);

    if (tabs > 0) {
        GetMemory()->Tab(tabs);
        return TRUE;
    }

    return FALSE;
}

Boolean CClosedCaptionRenderer::CCParser::HandleCtrlCode(
    /* [in] */ CCData* ccData)
{
    Int32 ctrlCode;
    ccData->GetCtrlCode(&ctrlCode);
    switch(ctrlCode) {
    case RCL:
        // select pop-on style
        mMode = MODE_POP_ON;
        break;
    case BS:
        GetMemory()->Bs();
        break;
    case DER:
        GetMemory()->Der();
        break;
    case RU2:
    case RU3:
    case RU4:
        mRollUpSize = (ctrlCode - 0x23);
        // erase memory if currently in other style
        if (mMode != MODE_ROLL_UP) {
            mDisplay->Erase();
            mNonDisplay->Erase();
        }
        // select roll-up style
        mMode = MODE_ROLL_UP;
        break;
    case FON:
        Logger::I(TAG, "Flash On");
        break;
    case RDC:
        // select paint-on style
        mMode = MODE_PAINT_ON;
        break;
    case TR:
        mMode = MODE_TEXT;
        mTextMem->Erase();
        break;
    case RTD:
        mMode = MODE_TEXT;
        break;
    case EDM:
        // erase display memory
        mDisplay->Erase();
        UpdateDisplay();
        break;
    case CR:
        if (mMode == MODE_ROLL_UP) {
            GetMemory()->RollUp(mRollUpSize);
        } else {
            GetMemory()->Cr();
        }
        if (mMode == MODE_ROLL_UP) {
            UpdateDisplay();
        }
        break;
    case ENM:
        // erase non-display memory
        mNonDisplay->Erase();
        break;
    case EOC:
        // swap display/non-display memory
        SwapMemory();
        // switch to pop-on style
        mMode = MODE_POP_ON;
        UpdateDisplay();
        break;
    case INVALID:
    default:
        // not handled
        return FALSE;
    }

    // handled
    return TRUE;
}

void CClosedCaptionRenderer::CCParser::UpdateDisplay()
{
    if (mListener != NULL) {
        AutoPtr<ICaptioningManagerCaptionStyle> captionStyle;
        mListener->GetCaptionStyle((ICaptioningManagerCaptionStyle**)&captionStyle);
        AutoPtr<ArrayOf<ISpannableStringBuilder*> > builders;
        mDisplay->GetStyledText(captionStyle, (ArrayOf<ISpannableStringBuilder*>**)&builders);
        mListener->OnDisplayChanged(builders);
    }
}

void CClosedCaptionRenderer::CCParser::SwapMemory()
{
    AutoPtr<CCMemory> temp = mDisplay;
    mDisplay = mNonDisplay;
    mNonDisplay = temp;
}

//================================================================================
//  CClosedCaptionRenderer::ClosedCaptionTrack
//================================================================================

CClosedCaptionRenderer::ClosedCaptionTrack::ClosedCaptionTrack(
    /* [in] */ ClosedCaptionWidget* renderingWidget,
    /* [in] */ IMediaFormat* format)
{
// TODO: Need SubtitleTrack
    // SubtitleTrack(format);

    mRenderingWidget = renderingWidget;
    mCCParser = new CCParser(renderingWidget);
}

ECode CClosedCaptionRenderer::ClosedCaptionTrack::OnData(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean eos,
    /* [in] */ Int64 runID)
{
    return mCCParser->Parse(data);
}

ECode CClosedCaptionRenderer::ClosedCaptionTrack::GetRenderingWidget(
    /* [out] */ ISubtitleTrackRenderingWidget** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRenderingWidget;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CClosedCaptionRenderer::ClosedCaptionTrack::UpdateView(
    /* [in] */ IVector* activeCues)
{
    // Overriding with NO-OP, CC rendering by-passes this
    return NOERROR;
}

//================================================================================
//  CClosedCaptionRenderer
//================================================================================

CAR_INTERFACE_IMPL_2(CClosedCaptionRenderer, Object, IClosedCaptionRenderer, ISubtitleControllerRenderer)

CAR_OBJECT_IMPL(CClosedCaptionRenderer)

CClosedCaptionRenderer::CClosedCaptionRenderer()
{
}

CClosedCaptionRenderer::~CClosedCaptionRenderer()
{
}

ECode CClosedCaptionRenderer::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode CClosedCaptionRenderer::Supports(
    /* [in] */ IMediaFormat* format,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean b;
    format->ContainsKey(IMediaFormat::KEY_MIME, &b);
    if (b) {
        String str;
        format->GetString(IMediaFormat::KEY_MIME, &str);
        *result = str.Equals(IMediaPlayer::MEDIA_MIMETYPE_TEXT_CEA_608);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CClosedCaptionRenderer::CreateTrack(
    /* [in] */ IMediaFormat* format,
    /* [out] */ ISubtitleTrack** result)
{
    VALIDATE_NOT_NULL(result)
    if (mRenderingWidget == NULL) {
        mRenderingWidget = new ClosedCaptionWidget(mContext);
    }
    AutoPtr<ClosedCaptionTrack> track = new ClosedCaptionTrack(mRenderingWidget, format);
    *result = ISubtitleTrack::Probe(track);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
