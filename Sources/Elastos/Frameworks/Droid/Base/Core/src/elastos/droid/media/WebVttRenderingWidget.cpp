#include "elastos/droid/media/WebVttRenderingWidget.h"
#include "elastos/droid/view/accessibility/CaptioningManager.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Internal::Widget::EIID_ISubtitleView;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::View::Accessibility::CaptioningManager;
using Elastos::Droid::View::Accessibility::EIID_IAccessibilityEventSource;
using Elastos::Droid::View::EIID_IKeyEventCallback;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::View::EIID_IViewManager;
using Elastos::Droid::View::EIID_IViewParent;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::EIID_ILinearLayout;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Text::IEditable;
using Elastos::Core::StringUtils;
using Elastos::Core::IAppendable;

namespace Elastos {
namespace Droid {
namespace Media {
//============================================================================
//              WebVttRenderingWidget::RegionLayout
//============================================================================
WebVttRenderingWidget::RegionLayout::RegionLayout(
    /* [in] */ WebVttRenderingWidget* host,
    /* [in] */ IContext* context,
    /* [in] */ ITextTrackRegion* region,
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [in] */ Float fontSize)
{
    LinearLayout::constructor(context);

    mRegion = region;
    mCaptionStyle = captionStyle;
    mFontSize = fontSize;
    mHost = host;

    // TODO: Add support for vertical text
    LinearLayout::SetOrientation(ILinearLayout::VERTICAL);

    if (WebVttRenderingWidget::DEBUG) {
        View::SetBackgroundColor(WebVttRenderingWidget::DEBUG_REGION_BACKGROUND);
    } else {
        Int32 color;
        captionStyle->GetWindowColor(&color);
        View::SetBackgroundColor(color);
    }
}

WebVttRenderingWidget::RegionLayout::~RegionLayout()
{}

CAR_INTERFACE_IMPL_8(WebVttRenderingWidget::RegionLayout, Object, ILinearLayout, IViewGroup, IViewParent, IViewManager, IView, IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource);

ECode WebVttRenderingWidget::RegionLayout::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [in] */ Float fontSize)
{
    mHost->mCaptionStyle = captionStyle;
    mHost->mFontSize = fontSize;

    Int32 cueCount;
    mRegionCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionCueBoxes->Get(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        cueBox->SetCaptionStyle(captionStyle, fontSize);
    }

    Int32 color;
    captionStyle->GetWindowColor(&color);
    return View::SetBackgroundColor(color);
}

ECode WebVttRenderingWidget::RegionLayout::MeasureForParent(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<ITextTrackRegion> region = mRegion;
    Int32 specWidth = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 specHeight = View::MeasureSpec::GetSize(heightMeasureSpec);
    Float width_;
    region->GetWidth(&width_);
    Int32 width = (Int32) width_;

    // Determine the absolute maximum region size as the requested size.
    Int32 size = width * specWidth / 100;

    widthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(size, View::MeasureSpec::AT_MOST);
    heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(specHeight, View::MeasureSpec::AT_MOST);
    View::Measure(widthMeasureSpec, heightMeasureSpec);
    return NOERROR;
}

ECode WebVttRenderingWidget::RegionLayout::PrepForPrune()
{
    Int32 cueCount;
    mRegionCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionCueBoxes->Get(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        cueBox->PrepForPrune();
    }
    return NOERROR;
}

ECode WebVttRenderingWidget::RegionLayout::Put(
    /* [in] */ ITextTrackCue* cue)
{
    Int32 cueCount;
    mRegionCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionCueBoxes->Get(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        AutoPtr<ITextTrackCue> ttc;
        cueBox->GetCue((ITextTrackCue**)&ttc);
        if (TO_IINTERFACE(ttc) == TO_IINTERFACE(cue)) {
            cueBox->Update();
            return NOERROR;
        }
    }

    AutoPtr<IContext> context;
    View::GetContext((IContext**)&context);
    AutoPtr<CueLayout> cueBox = new CueLayout(mHost, context.Get(), cue, mCaptionStyle, mFontSize);
    mRegionCueBoxes->Add(ILinearLayout::Probe(cueBox));
    ViewGroup::AddView(cueBox, IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);

    Int32 count;
    ViewGroup::GetChildCount(&count);
    Int32 lines;
    mRegion->GetLines(&lines);
    if (count > lines) {
        ViewGroup::RemoveViewAt(0);
    }
    return NOERROR;
}

ECode WebVttRenderingWidget::RegionLayout::Prune(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 cueCount;
    mRegionCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionCueBoxes->Get(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        Boolean flag = FALSE;
        if (!(cueBox->IsActive(&flag), flag)) {
            mRegionCueBoxes->Remove(i);
            ViewGroup::RemoveView(cueBox);
            cueCount--;
            i--;
        }
    }

    return mRegionCueBoxes->IsEmpty(result);
}

ECode WebVttRenderingWidget::RegionLayout::GetRegion(
    /* [out] */ ITextTrackRegion** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRegion;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//============================================================================
//              WebVttRenderingWidget::CueLayout
//============================================================================
WebVttRenderingWidget::CueLayout::CueLayout(
    /* [in] */ WebVttRenderingWidget* host,
    /* [in] */ IContext* context,
    /* [in] */ ITextTrackCue* cue,
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [in] */ Float fontSize)
{
    LinearLayout::constructor(context);

    mCue = cue;
    mCaptionStyle = captionStyle;
    mFontSize = fontSize;
    mHost = host;

    // TODO: Add support for vertical text.
    Int32 writingDirction;
    cue->GetWritingDirection(&writingDirction);
    Boolean horizontal = writingDirction == ITextTrackCue::WRITING_DIRECTION_HORIZONTAL;
    LinearLayout::SetOrientation(horizontal ? ILinearLayout::VERTICAL : ILinearLayout::HORIZONTAL);

    Int32 alignment;
    cue->GetAlignment(&alignment);
    switch (alignment) {
        case ITextTrackCue::ALIGNMENT_END:
            LinearLayout::SetGravity(IGravity::END);
            break;
        case ITextTrackCue::ALIGNMENT_LEFT:
            LinearLayout::SetGravity(IGravity::LEFT);
            break;
        case ITextTrackCue::ALIGNMENT_MIDDLE:
            LinearLayout::SetGravity(horizontal
                    ? IGravity::CENTER_HORIZONTAL : IGravity::CENTER_VERTICAL);
            break;
        case ITextTrackCue::ALIGNMENT_RIGHT:
            LinearLayout::SetGravity(IGravity::RIGHT);
            break;
        case ITextTrackCue::ALIGNMENT_START:
            LinearLayout::SetGravity(IGravity::START);
            break;
    }

    if (WebVttRenderingWidget::DEBUG) {
        View::SetBackgroundColor(WebVttRenderingWidget::DEBUG_CUE_BACKGROUND);
    }

    Update();
}

WebVttRenderingWidget::CueLayout::~CueLayout()
{}

CAR_INTERFACE_IMPL_8(WebVttRenderingWidget::CueLayout, Object, ILinearLayout, IViewGroup, IViewParent, IViewManager, IView, IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource);

ECode WebVttRenderingWidget::CueLayout::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* style,
    /* [in] */ Float fontSize)
{
    mCaptionStyle = style;
    mFontSize = fontSize;

    Int32 n;
    ViewGroup::GetChildCount(&n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IView> child;
        ViewGroup::GetChildAt(i, (IView**)&child);
        if (IView::Probe(child)) {
            ((SpanLayout*)child.Get())->SetCaptionStyle(style, fontSize);
        }
    }
    return NOERROR;
}

ECode WebVttRenderingWidget::CueLayout::PrepForPrune()
{
    mActive = FALSE;
    return NOERROR;
}

ECode WebVttRenderingWidget::CueLayout::Update()
{
    mActive = TRUE;

    ViewGroup::RemoveAllViews();

    Int32 ld;
    ViewGroup::GetLayoutDirection(&ld);
    Int32 alt;
    mCue->GetAlignment(&alt);
    Int32 cueAlignment = mHost->ResolveCueAlignment(ld, alt);
    Int32 alignment;
    switch (cueAlignment) {
        case ITextTrackCue::ALIGNMENT_LEFT:
            alignment = 3;
            break;
        case ITextTrackCue::ALIGNMENT_RIGHT:
            alignment = 4;
            break;
        case ITextTrackCue::ALIGNMENT_MIDDLE:
        default:
            alignment = 2;
    }

    AutoPtr<ICaptioningManagerCaptionStyle> captionStyle = mHost->mCaptionStyle;
    Float fontSize = mFontSize;
    AutoPtr<ArrayOf<IArrayOf*> > lines;
    mCue->GetLines((ArrayOf<IArrayOf*>**)&lines);
    Int32 lineCount = lines->GetLength();
    for (Int32 i = 0; i < lineCount; i++) {
        AutoPtr<IContext> context;
        View::GetContext((IContext**)&context);
        AutoPtr<SpanLayout> lineBox = new SpanLayout(context, (*lines)[i]);
        lineBox->SetAlignment(alignment);
        lineBox->SetCaptionStyle(captionStyle, fontSize);

        ViewGroup::AddView(lineBox, IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
    }
    return NOERROR;
}

void WebVttRenderingWidget::CueLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    mHost->OnMeasure(widthMeasureSpec, heightMeasureSpec);
}

ECode WebVttRenderingWidget::CueLayout::MeasureForParent(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    AutoPtr<ITextTrackCue> cue = mCue;
    Int32 specWidth = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 specHeight = View::MeasureSpec::GetSize(heightMeasureSpec);
    Int32 direction;
    ViewGroup::GetLayoutDirection(&direction);
    Int32 alignment;
    cue->GetAlignment(&alignment);
    Int32 absAlignment = mHost->ResolveCueAlignment(direction, alignment);

    // Determine the maximum size of cue based on its starting position
    // and the direction in which it grows.
    Int32 maximumSize;
    Int32 textPosition;
    cue->GetTextPosition(&textPosition);
    switch (absAlignment) {
        case ITextTrackCue::ALIGNMENT_LEFT:
            maximumSize = 100 - textPosition;
            break;
        case ITextTrackCue::ALIGNMENT_RIGHT:
            maximumSize = textPosition;
            break;
        case ITextTrackCue::ALIGNMENT_MIDDLE:
            if (textPosition <= 50) {
                maximumSize = textPosition * 2;
            } else {
                maximumSize = (100 - textPosition) * 2;
            }
            break;
        default:
            maximumSize = 0;
    }

    // Determine absolute maximum cue size as the smaller of the
    // requested size and the maximum theoretical size.
    Int32 cueSize;
    cue->GetSize(&cueSize);
    Int32 size = Elastos::Core::Math::Min(cueSize, maximumSize) * specWidth / 100;
    widthMeasureSpec = View::MeasureSpec::MakeMeasureSpec(size, View::MeasureSpec::AT_MOST);
    heightMeasureSpec = View::MeasureSpec::MakeMeasureSpec(specHeight, View::MeasureSpec::AT_MOST);
    return View::Measure(widthMeasureSpec, heightMeasureSpec);
}

ECode WebVttRenderingWidget::CueLayout::SetOrder(
    /* [in] */ Int32 order)
{
    mOrder = order;
    return NOERROR;
}

ECode WebVttRenderingWidget::CueLayout::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mActive;
    return NOERROR;
}

ECode WebVttRenderingWidget::CueLayout::GetCue(
    /* [out] */ ITextTrackCue** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCue;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//============================================================================
//              WebVttRenderingWidget::SpanLayout
//============================================================================
WebVttRenderingWidget::SpanLayout::SpanLayout(
    /* [in] */ IContext* context,
    /* [in] */ IArrayOf* spans)
{
    SubtitleView::constructor(context);
    mSpans = spans;
    Update();
}

WebVttRenderingWidget::SpanLayout::~SpanLayout()
{}

CAR_INTERFACE_IMPL_5(WebVttRenderingWidget::SpanLayout, Object, ISubtitleView, IView, IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource);

ECode WebVttRenderingWidget::SpanLayout::Update()
{
    AutoPtr<ISpannableStringBuilder> builder = mBuilder;
    AutoPtr<IArrayOf> spans = mSpans;

    IEditable::Probe(builder)->Clear();
    IEditable::Probe(builder)->ClearSpans();

    Int32 spanCount;
    spans->GetLength(&spanCount);
    for (Int32 i = 0; i < spanCount; i++) {
        AutoPtr<IInterface> span_;
        spans->Get(i, (IInterface**)&span_);
        AutoPtr<ITextTrackCueSpan> span = ITextTrackCueSpan::Probe(span_);
        Boolean enabled = FALSE;
        span->GetEnabled(&enabled);
        String text;
        span->GetText(&text);
        if (enabled) {
            IAppendable::Probe(builder)->Append(StringUtils::ParseCharSequence(text).Get());
        }
    }

    return SubtitleView::SetText(ICharSequence::Probe(builder));
}

ECode WebVttRenderingWidget::SpanLayout::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [in] */ Float fontSize)
{
    Int32 backgroundColor;
    captionStyle->GetBackgroundColor(&backgroundColor);
    SubtitleView::SetBackgroundColor(backgroundColor);
    Int32 foregroundColor;
    captionStyle->GetForegroundColor(&foregroundColor);
    SubtitleView::SetForegroundColor(foregroundColor);
    Int32 edgeColor;
    captionStyle->GetEdgeColor(&edgeColor);
    SubtitleView::SetEdgeColor(edgeColor);
    Int32 edgeType;
    captionStyle->GetEdgeType(&edgeType);
    SubtitleView::SetEdgeType(edgeType);
    AutoPtr<ITypeface> typeface;
    captionStyle->GetTypeface((ITypeface**)&typeface);
    SubtitleView::SetTypeface(typeface.Get());
    return SubtitleView::SetTextSize(fontSize);
}

//============================================================================
//              WebVttRenderingWidget
//============================================================================
const Boolean WebVttRenderingWidget::DEBUG = FALSE;
const AutoPtr<ICaptioningManagerCaptionStyle> WebVttRenderingWidget::DEFAULT_CAPTION_STYLE = CaptioningManager::CaptionStyle::DEFAULT;;
const Int32 WebVttRenderingWidget::DEBUG_REGION_BACKGROUND = 0x800000FF;
const Int32 WebVttRenderingWidget::DEBUG_CUE_BACKGROUND = 0x80FF0000;
/** WebVtt specifies line height as 5.3% of the viewport height. */
const Float WebVttRenderingWidget::LINE_HEIGHT_RATIO = 0.0533f;

WebVttRenderingWidget::WebVttRenderingWidget()
{}

WebVttRenderingWidget::~WebVttRenderingWidget()
{}

CAR_INTERFACE_IMPL_7(WebVttRenderingWidget, Object, IViewGroup, IViewParent, IViewManager, IView, IDrawableCallback, IKeyEventCallback, IAccessibilityEventSource)

ECode WebVttRenderingWidget::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode WebVttRenderingWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode WebVttRenderingWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode WebVttRenderingWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ViewGroup::constructor(context, attrs, defStyleAttr, defStyleRes);

    // Cannot render text over video when layer type is hardware.
    View::SetLayerType(IView::LAYER_TYPE_SOFTWARE, NULL);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::CAPTIONING_SERVICE, (IInterface**)&obj);
    mManager = ICaptioningManager::Probe(obj);
    mManager->GetUserStyle((ICaptioningManagerCaptionStyle**)&mCaptionStyle);
    Float scale;
    mManager->GetFontScale(&scale);
    Int32 height;
    ViewGroup::GetHeight(&height);
    mFontSize = scale * height * LINE_HEIGHT_RATIO;
    return NOERROR;
}

ECode WebVttRenderingWidget::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 widthSpec = View::MeasureSpec::MakeMeasureSpec(width, View::MeasureSpec::EXACTLY);
    Int32 heightSpec = View::MeasureSpec::MakeMeasureSpec(height, View::MeasureSpec::EXACTLY);

    View::Measure(widthSpec, heightSpec);
    return View::Layout(0, 0, width, height);
}

ECode WebVttRenderingWidget::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    ManageChangeListener();
    return NOERROR;
}

ECode WebVttRenderingWidget::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    ManageChangeListener();
    return NOERROR;
}

ECode WebVttRenderingWidget::SetOnChangedListener(
    /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode WebVttRenderingWidget::SetVisible(
    /* [in] */ Boolean visible)
{
    if (visible) {
        View::SetVisibility(IView::VISIBLE);
    } else {
        View::SetVisibility(IView::GONE);
    }

    ManageChangeListener();
    return NOERROR;
}

ECode WebVttRenderingWidget::SetActiveCues(
    /* [in] */ IVector* activeCues)
{
    AutoPtr<IContext> context;
    View::GetContext((IContext**)&context);
    AutoPtr<ICaptioningManagerCaptionStyle> captionStyle = mCaptionStyle;
    Float fontSize = mFontSize;

    PrepForPrune();

    // Ensure we have all necessary cue and region boxes.
    Int32 count;
    activeCues->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        activeCues->Get(i, (IInterface**)&obj);
        AutoPtr<ITextTrackCue> cue = ITextTrackCue::Probe(obj);
        AutoPtr<ITextTrackRegion> region;
        cue->GetRegion((ITextTrackRegion**)&region);

        if (region != NULL) {
            AutoPtr<IInterface> rg;
            mRegionBoxes->Get(region, (IInterface**)&rg);
            AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(rg);
            if (regionBox == NULL) {
                regionBox = new RegionLayout(this, context, region, captionStyle, fontSize);
                mRegionBoxes->Put(TO_IINTERFACE(region), TO_IINTERFACE(regionBox));
                ViewGroup::AddView(regionBox, IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
            }
            regionBox->Put(cue);
        } else {
            AutoPtr<IInterface> cl;
            mCueBoxes->Get(cue, (IInterface**)&cl);
            AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(cl);
            if (cueBox == NULL) {
                cueBox = new CueLayout(this, context, cue, captionStyle, fontSize);
                mCueBoxes->Put(TO_IINTERFACE(cue), TO_IINTERFACE(cueBox));
                ViewGroup::AddView(cueBox, IViewGroupLayoutParams::WRAP_CONTENT, IViewGroupLayoutParams::WRAP_CONTENT);
            }
            cueBox->Update();
            cueBox->SetOrder(i);
        }
    }

    Prune();

    // Force measurement and layout.
    Int32 width;
    View::GetWidth(&width);
    Int32 height;
    View::GetHeight(&height);
    SetSize(width, height);

    if (mListener != NULL) {
        mListener->OnChanged(this);
    }
    return NOERROR;
}

void WebVttRenderingWidget::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    View::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 regionCount;
    mRegionBoxes->GetSize(&regionCount);
    for (Int32 i = 0; i < regionCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(obj);
        regionBox->MeasureForParent(widthMeasureSpec, heightMeasureSpec);
    }

    Int32 cueCount;
    mCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        cueBox->MeasureForParent(widthMeasureSpec, heightMeasureSpec);
    }
}

ECode WebVttRenderingWidget::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 viewportWidth = r - l;
    Int32 viewportHeight = b - t;

    Float scale;
    mManager->GetFontScale(&scale);
    SetCaptionStyle(mCaptionStyle,
            scale * LINE_HEIGHT_RATIO * viewportHeight);

    Int32 regionCount;
    mRegionBoxes->GetSize(&regionCount);
    for (Int32 i = 0; i < regionCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(obj);
        LayoutRegion(viewportWidth, viewportHeight, regionBox.Get());
    }

    Int32 cueCount;
    mCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        LayoutCue(viewportWidth, viewportHeight, cueBox.Get());
    }
    return NOERROR;
}

void WebVttRenderingWidget::ManageChangeListener()
{
    Boolean flag = FALSE;
    View::IsAttachedToWindow(&flag);
    Int32 visibility;
    View::GetVisibility(&visibility);
    Boolean needsListener = flag && visibility == IView::VISIBLE;
    if (mHasChangeListener != needsListener) {
        mHasChangeListener = needsListener;

        if (needsListener) {
            mManager->AddCaptioningChangeListener(mCaptioningListener);

            AutoPtr<ICaptioningManagerCaptionStyle> captionStyle;
            mManager->GetUserStyle((ICaptioningManagerCaptionStyle**)&captionStyle);
            Float scale;
            mManager->GetFontScale(&scale);
            Int32 height;
            ViewGroup::GetHeight(&height);
            Float fontSize = scale * height * LINE_HEIGHT_RATIO;
            SetCaptionStyle(captionStyle, fontSize);
        } else {
            mManager->RemoveCaptioningChangeListener(mCaptioningListener);
        }
    }
}

void WebVttRenderingWidget::SetCaptionStyle(
    /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
    /* [in] */ Float fontSize)
{
    AutoPtr<ICaptioningManagerCaptionStyle> cs;
    DEFAULT_CAPTION_STYLE->ApplyStyle(captionStyle, (ICaptioningManagerCaptionStyle**)&cs);
    captionStyle = cs.Get();
    mCaptionStyle = captionStyle;
    mFontSize = fontSize;

    Int32 cueCount;
    mCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        cueBox->SetCaptionStyle(captionStyle, fontSize);
    }

    Int32 regionCount;
    mRegionBoxes->GetSize(&regionCount);
    for (Int32 i = 0; i < regionCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(obj);
        regionBox->SetCaptionStyle(captionStyle, fontSize);
    }
}

void WebVttRenderingWidget::Prune()
{
    Int32 regionCount;
    mRegionBoxes->GetSize(&regionCount);
    for (Int32 i = 0; i < regionCount; i++) {
        AutoPtr<IInterface> obj;
        mRegionBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(obj);
        Boolean flag = FALSE;
        regionBox->Prune(&flag);
        if (flag) {
            ViewGroup::RemoveView(regionBox);
            mRegionBoxes->RemoveAt(i);
            regionCount--;
            i--;
        }
    }

    Int32 cueCount;
    mCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        Boolean flag = FALSE;
        cueBox->IsActive(&flag);
        if (!flag) {
            ViewGroup::RemoveView(cueBox);
            mCueBoxes->RemoveAt(i);
            cueCount--;
            i--;
        }
    }
}

void WebVttRenderingWidget::PrepForPrune()
{
    Int32 regionCount;
    mRegionBoxes->GetSize(&regionCount);
    for (Int32 i = 0; i < regionCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<RegionLayout> regionBox = (RegionLayout*)ILinearLayout::Probe(obj);
        regionBox->PrepForPrune();
    }

    Int32 cueCount;
    mCueBoxes->GetSize(&cueCount);
    for (Int32 i = 0; i < cueCount; i++) {
        AutoPtr<IInterface> obj;
        mCueBoxes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<CueLayout> cueBox = (CueLayout*)ILinearLayout::Probe(obj);
        cueBox->PrepForPrune();
    }
}

void WebVttRenderingWidget::LayoutRegion(
    /* [in] */ Int32 viewportWidth,
    /* [in] */ Int32 viewportHeight,
    /* [in] */ RegionLayout* regionBox)
{
    AutoPtr<ITextTrackRegion> region;
    regionBox->GetRegion((ITextTrackRegion**)&region);
    Int32 regionHeight;
    IView::Probe(regionBox)->GetMeasuredHeight(&regionHeight);
    Int32 regionWidth;
    IView::Probe(regionBox)->GetMeasuredWidth(&regionWidth);

    // TODO: Account for region anchor point.
    Float x;
    region->GetViewportAnchorPointX(&x);
    Float y;
    region->GetViewportAnchorPointX(&y);;
    Int32 left = (Int32) (x * (viewportWidth - regionWidth) / 100);
    Int32 top = (Int32) (y * (viewportHeight - regionHeight) / 100);

    regionBox->Layout(left, top, left + regionWidth, top + regionHeight);
}

void WebVttRenderingWidget::LayoutCue(
    /* [in] */ Int32 viewportWidth,
    /* [in] */ Int32 viewportHeight,
    /* [in] */ CueLayout* cueBox)
{
    AutoPtr<ITextTrackCue> cue;
    cueBox->GetCue((ITextTrackCue**)&cue);
    Int32 direction;
    ViewGroup::GetLayoutDirection(&direction);
    Int32 alignment;
    cue->GetAlignment(&alignment);
    Int32 absAlignment = ResolveCueAlignment(direction, alignment);
    Boolean cueSnapToLines;
    cue->GetSnapToLines(&cueSnapToLines);

    Int32 width;
    IView::Probe(cueBox)->GetMeasuredWidth(&width);
    Int32 size = 100 * width / viewportWidth;

    // Determine raw x-position.
    Int32 position;
    cue->GetTextPosition(&position);
    Int32 xPosition;
    switch (absAlignment) {
        case ITextTrackCue::ALIGNMENT_LEFT:
            xPosition = position;
            break;
        case ITextTrackCue::ALIGNMENT_RIGHT:
            xPosition = position - size;
            break;
        case ITextTrackCue::ALIGNMENT_MIDDLE:
        default:
            xPosition = position - size / 2;
            break;
    }

    // Adjust x-position for layout.
    if (direction == IView::LAYOUT_DIRECTION_RTL) {
        xPosition = 100 - xPosition;
    }

    // If the text track cue snap-to-lines flag is set, adjust
    // x-position and size for padding. This is equivalent to placing the
    // cue within the title-safe area.
    if (cueSnapToLines) {
        Int32 pl;
        View::GetPaddingLeft(&pl);
        Int32 pr;
        View::GetPaddingRight(&pr);
        Int32 paddingLeft = 100 * pl / viewportWidth;
        Int32 paddingRight = 100 * pr / viewportWidth;
        if (xPosition < paddingLeft && xPosition + size > paddingLeft) {
            xPosition += paddingLeft;
            size -= paddingLeft;
        }
        Float rightEdge = 100 - paddingRight;
        if (xPosition < rightEdge && xPosition + size > rightEdge) {
            size -= paddingRight;
        }
    }

    // Compute absolute left position and width.
    Int32 left = xPosition * viewportWidth / 100;
    width = size * viewportWidth / 100;

    // Determine initial y-position.
    Int32 yPosition = CalculateLinePosition(cueBox);

    // Compute absolute final top position and height.
    Int32 height;
    IView::Probe(cueBox)->GetMeasuredHeight(&height);

    Int32 top;
    if (yPosition < 0) {
        // TODO: This needs to use the actual height of prior boxes.
        top = viewportHeight + yPosition * height;
    } else {
        top = yPosition * (viewportHeight - height) / 100;
    }

    // Layout cue in final position.
    cueBox->Layout(left, top, left + width, top + height);
}

Int32 WebVttRenderingWidget::CalculateLinePosition(CueLayout* cueBox)
{
    VALIDATE_NOT_NULL(cueBox);
    AutoPtr<ITextTrackCue> cue;
    cueBox->GetCue((ITextTrackCue**)&cue);
    Int32 linePosition;
    cue->GetLinePosition(&linePosition);
    Boolean snapToLines;
    cue->GetSnapToLines(&snapToLines);
    Boolean autoPosition = FALSE;

    if (!snapToLines && !autoPosition && (linePosition < 0 || linePosition > 100)) {
        // Invalid line position defaults to 100.
        return 100;
    } else if (!autoPosition) {
        // Use the valid, supplied line position.
        return linePosition;
    } else if (!snapToLines) {
        // Automatic, non-snapped line position defaults to 100.
        return 100;
    } else {
        // Automatic snapped line position uses active cue order.
        return -(cueBox->mOrder + 1);
    }
}

Int32 WebVttRenderingWidget::ResolveCueAlignment(
    /* [in] */ Int32 layoutDirection,
    /* [in] */ Int32 alignment)
{
    switch (alignment) {
        case ITextTrackCue::ALIGNMENT_START:
            return layoutDirection == IView::LAYOUT_DIRECTION_LTR ?
                    ITextTrackCue::ALIGNMENT_LEFT : ITextTrackCue::ALIGNMENT_RIGHT;
        case ITextTrackCue::ALIGNMENT_END:
            return layoutDirection == IView::LAYOUT_DIRECTION_LTR ?
                    ITextTrackCue::ALIGNMENT_RIGHT : ITextTrackCue::ALIGNMENT_LEFT;
    }
    return alignment;
}

ECode WebVttRenderingWidget::Draw(
    /* [in] */ ICanvas* c)
{
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos