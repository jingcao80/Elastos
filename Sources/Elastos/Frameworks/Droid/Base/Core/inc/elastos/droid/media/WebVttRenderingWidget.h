#ifndef __ELASTOS_DROID_MEDIA_WEBVTTRENDERINGWIDGET_H__
#define __ELASTOS_DROID_MEDIA_WEBVTTRENDERINGWIDGET_H__

#include "Elastos.Droid.Media.h"
#include "elastos/droid/internal/widget/SubtitleView.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Internal::Widget::SubtitleView;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::Accessibility::ICaptioningManager;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptioningChangeListener;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyle;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Utility::IVector;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Media {

class WebVttRenderingWidget
    : public ViewGroup
    , public IWebVttRenderingWidget
    , public ISubtitleTrackRenderingWidget
{
private:
    class RegionLayout
        : public LinearLayout
    {
    public:
        RegionLayout(
            /* [in] */ WebVttRenderingWidget* host,
            /* [in] */ IContext* context,
            /* [in] */ ITextTrackRegion* region,
            /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
            /* [in] */ Float fontSize);

        ~RegionLayout();

        CAR_INTERFACE_DECL()

        CARAPI SetCaptionStyle(
            /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
            /* [in] */ Float fontSize);

        CARAPI MeasureForParent(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI PrepForPrune();

        CARAPI Put(
            /* [in] */ ITextTrackCue* cue);

        CARAPI Prune(
            /* [out] */ Boolean* result);

        CARAPI GetRegion(
            /* [out] */ ITextTrackRegion** result);

    private:
        AutoPtr<IArrayList> mRegionCueBoxes;
        AutoPtr<ITextTrackRegion> mRegion;
        AutoPtr<ICaptioningManagerCaptionStyle> mCaptionStyle;
        Float mFontSize;
        WebVttRenderingWidget* mHost;
    };

    class CueLayout
        : public LinearLayout
    {
        friend class WebVttRenderingWidget;
    public:
        CueLayout(
            /* [in] */ WebVttRenderingWidget* host,
            /* [in] */ IContext* context,
            /* [in] */ ITextTrackCue* cue,
            /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
            /* [in] */ Float fontSize);

        ~CueLayout();

        CAR_INTERFACE_DECL()

        CARAPI SetCaptionStyle(
            /* [in] */ ICaptioningManagerCaptionStyle* style,
            /* [in] */ Float fontSize);

        CARAPI PrepForPrune();

        CARAPI Update();

        CARAPI OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI MeasureForParent(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI SetOrder(
            /* [in] */ Int32 order);

        CARAPI IsActive(
            /* [out] */ Boolean* result);

        CARAPI GetCue(
            /* [out] */ ITextTrackCue** result);

    public:
        AutoPtr<ITextTrackCue> mCue;

    private:
        AutoPtr<ICaptioningManagerCaptionStyle> mCaptionStyle;
        Float mFontSize;
        Boolean mActive;
        Int32 mOrder;
        WebVttRenderingWidget* mHost;
    };

    class SpanLayout
        : public SubtitleView
    {
    public:
        SpanLayout(
            /* [in] */ IContext* context,
            /* [in] */ IArrayOf* spans);

        ~SpanLayout();

        CAR_INTERFACE_DECL()

        CARAPI Update();

        CARAPI SetCaptionStyle(
            /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
            /* [in] */ Float fontSize);

    private:
        AutoPtr<ISpannableStringBuilder> mBuilder;
        AutoPtr<IArrayOf> mSpans;
    };

public:
    WebVttRenderingWidget();

    virtual ~WebVttRenderingWidget();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetSize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI SetOnChangedListener(
        /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener);

    CARAPI SetVisible(
        /* [in] */ Boolean visible);

    CARAPI SetActiveCues(
        /* [in] */ IVector* activeCues);

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    //override
    CARAPI Draw(
        /* [in] */ ICanvas* c);

private:
    CARAPI_(void) ManageChangeListener();

    CARAPI_(void) SetCaptionStyle(
        /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
        /* [in] */ Float fontSize);

    CARAPI_(void) Prune();

    CARAPI_(void) PrepForPrune();

    CARAPI_(void) LayoutRegion(
        /* [in] */ Int32 viewportWidth,
        /* [in] */ Int32 viewportHeight,
        /* [in] */ RegionLayout* regionBox);

    CARAPI_(void) LayoutCue(
        /* [in] */ Int32 viewportWidth,
        /* [in] */ Int32 viewportHeight,
        /* [in] */ CueLayout* cueBox);

    CARAPI_(Int32) CalculateLinePosition(CueLayout* cueBox);

    static CARAPI_(Int32) ResolveCueAlignment(
        /* [in] */ Int32 layoutDirection,
        /* [in] */ Int32 alignment);
private:
    static const Boolean DEBUG;
    static const AutoPtr<ICaptioningManagerCaptionStyle> DEFAULT_CAPTION_STYLE;
    static const Int32 DEBUG_REGION_BACKGROUND;
    static const Int32 DEBUG_CUE_BACKGROUND;
    /** WebVtt specifies line height as 5.3% of the viewport height. */
    static const Float LINE_HEIGHT_RATIO;
    AutoPtr<IArrayMap> mRegionBoxes;
    AutoPtr<IArrayMap> mCueBoxes;
    AutoPtr<ICaptioningManager> mManager;
    AutoPtr<ISubtitleTrackRenderingWidgetOnChangedListener> mListener;
    AutoPtr<ICaptioningManagerCaptionStyle> mCaptionStyle;
    Float mFontSize;
    Boolean mHasChangeListener;
    AutoPtr<ICaptioningManagerCaptioningChangeListener> mCaptioningListener;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_WEBVTTRENDERINGWIDGET_H__
