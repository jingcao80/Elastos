
#ifndef __ELASTOS_DROID_MEDIA_CCLOSEDCAPTIONRENDERER_H__
#define __ELASTOS_DROID_MEDIA_CCLOSEDCAPTIONRENDERER_H__

#include "_Elastos_Droid_Media_CClosedCaptionRenderer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/style/CharacterStyle.h"
#include "elastos/droid/view/ViewGroup.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/widget/TextView.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::R;
using Elastos::Droid::Text::IParcelableSpan;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::Style::CharacterStyle;
using Elastos::Droid::Text::Style::IUpdateAppearance;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::ICaptioningManager;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptioningChangeListener;
using Elastos::Droid::View::Accessibility::ICaptioningManagerCaptionStyle;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::TextView;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CClosedCaptionRenderer)
    , public Object
    , public IClosedCaptionRenderer
    , public ISubtitleControllerRenderer
{
public:
    class ClosedCaptionWidget
        : public ViewGroup
        , public ISubtitleTrackRenderingWidget
        , public IClosedCaptionRendererCCParserDisplayListener
    {
    private:
        class CCLineBox
            : public TextView
        {
        public:
            CCLineBox(
                /* [in] */ IContext* context);

            CARAPI SetCaptionStyle(
                /* [in] */ ICaptioningManagerCaptionStyle* captionStyle);

        protected:
            CARAPI_(void) OnMeasure(
                /* [in] */ Int32 widthMeasureSpec,
                /* [in] */ Int32 heightMeasureSpec);

            CARAPI_(void) OnDraw(
                /* [in] */ ICanvas* c);

        private:
            CARAPI_(void) DrawEdgeOutline(
                /* [in] */ ICanvas* c);

            CARAPI_(void) DrawEdgeRaisedOrDepressed(
                /* [in] */ ICanvas* c);

            CARAPI_(void) SetBackgroundSpans(
                /* [in] */ Int32 color);

        private:
            static Float FONT_PADDING_RATIO;
            static Float EDGE_OUTLINE_RATIO;
            static Float EDGE_SHADOW_RATIO;
            Float mOutlineWidth;
            Float mShadowRadius;
            Float mShadowOffset;

            Int32 mTextColor;
            Int32 mBgColor;
            Int32 mEdgeType;
            Int32 mEdgeColor;
        };

        class CCLayout
            : public LinearLayout
        {
        public:
            CCLayout(
                /* [in] */ IContext* context);

            CARAPI SetCaptionStyle(
                /* [in] */ ICaptioningManagerCaptionStyle* captionStyle);

            CARAPI Update(
                /* [in] */ ArrayOf<ISpannableStringBuilder*>* textBuffer);

        protected:
            CARAPI_(void) OnMeasure(
                /* [in] */ Int32 widthMeasureSpec,
                /* [in] */ Int32 heightMeasureSpec);

            CARAPI OnLayout(
                /* [in] */ Boolean changed,
                /* [in] */ Int32 l,
                /* [in] */ Int32 t,
                /* [in] */ Int32 r,
                /* [in] */ Int32 b);

        private:
            static Int32 MAX_ROWS;
            static Float SAFE_AREA_RATIO;

            AutoPtr<ArrayOf<CCLineBox*> > mLineBoxes;
        };

        class CaptioningChangeListener
            : public Object
            , public ICaptioningManagerCaptioningChangeListener
        {
        public:
            CAR_INTERFACE_DECL()

            CaptioningChangeListener(
                /* [in] */ ClosedCaptionWidget* host)
                : mHost(host)
            {}

            CARAPI OnEnabledChanged(
                /* [in] */ Boolean enabled);

            CARAPI OnUserStyleChanged(
                /* [in] */ ICaptioningManagerCaptionStyle* userStyle);

            CARAPI OnLocaleChanged(
                /* [in] */ ILocale* locale);

            CARAPI OnFontScaleChanged(
                /* [in] */ Float fontScale);

        private:
            ClosedCaptionWidget* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        ClosedCaptionWidget(
            /* [in] */ IContext* context);

        ClosedCaptionWidget(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs);

        ClosedCaptionWidget(
            /* [in] */ IContext* context,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ Int32 defStyle);

        CARAPI SetOnChangedListener(
            /* [in] */ ISubtitleTrackRenderingWidgetOnChangedListener* listener);

        CARAPI SetSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI SetVisible(
            /* [in] */ Boolean visible);

        CARAPI OnAttachedToWindow();

        CARAPI OnDetachedFromWindow();

        CARAPI OnDisplayChanged(
            /* [in] */ ArrayOf<ISpannableStringBuilder*>* styledTexts);

        CARAPI GetCaptionStyle(
            /* [out] */ ICaptioningManagerCaptionStyle** result);

        CARAPI Draw(
            /* [in] */ ICanvas* c);

    protected:
        CARAPI_(void) OnMeasure(
            /* [in] */ Int32 widthMeasureSpec,
            /* [in] */ Int32 heightMeasureSpec);

        CARAPI OnLayout(
            /* [in] */ Boolean changed,
            /* [in] */ Int32 l,
            /* [in] */ Int32 t,
            /* [in] */ Int32 r,
            /* [in] */ Int32 b);

    private:
        CARAPI_(void) ManageChangeListener();

        static CARAPI_(void) Init();

    private:
        static String TAG;

        static AutoPtr<IRect> mTextBounds;
        static String mDummyText;
        static AutoPtr<ICaptioningManagerCaptionStyle> DEFAULT_CAPTION_STYLE;

        /** Captioning manager, used to obtain and track caption properties. */
        AutoPtr<ICaptioningManager> mManager;

        /** Callback for rendering changes. */
        AutoPtr<ISubtitleTrackRenderingWidgetOnChangedListener> mListener;

        /** Current caption style. */
        AutoPtr<ICaptioningManagerCaptionStyle> mCaptionStyle;

        /* Closed caption layout. */
        AutoPtr<CCLayout> mClosedCaptionLayout;

        /** Whether a caption style change listener is registered. */
        Boolean mHasChangeListener;

        AutoPtr<ICaptioningManagerCaptioningChangeListener> mCaptioningListener;
    };

    class CCParser
        : public Object
    {
    private:
        class StyleCode
            : public Object
        {
        public:
            StyleCode(
                /* [in] */ Int32 style,
                /* [in] */ Int32 color);

            static CARAPI FromByte(
                /* [in] */ Byte data2,
                /* [out] */ StyleCode** result);

            CARAPI IsItalics(
                /* [out] */ Boolean* result);

            CARAPI IsUnderline(
                /* [out] */ Boolean* result);

            CARAPI GetColor(
                /* [out] */ Int32* result);

            CARAPI ToString(
                /* [out] */ String* result);

        public:
            static const Int32 COLOR_WHITE;
            static const Int32 COLOR_GREEN;
            static const Int32 COLOR_BLUE;
            static const Int32 COLOR_CYAN;
            static const Int32 COLOR_RED;
            static const Int32 COLOR_YELLOW;
            static const Int32 COLOR_MAGENTA;
            static const Int32 COLOR_INVALID;

            static const Int32 STYLE_ITALICS;
            static const Int32 STYLE_UNDERLINE;

            static AutoPtr<ArrayOf<String> > mColorMap;

            Int32 mStyle;
            Int32 mColor;
        };

        class PAC
            : public StyleCode
        {
        public:
            PAC(
                /* [in] */ Int32 row,
                /* [in] */ Int32 col,
                /* [in] */ Int32 style,
                /* [in] */ Int32 color);

            static CARAPI FromBytes(
                /* [in] */ byte data1,
                /* [in] */ byte data2,
                /* [out] */ PAC** result);

            CARAPI IsIndentPAC(
                /* [out] */ Boolean* result);

            CARAPI GetRow(
                /* [out] */ Int32* result);

            CARAPI GetCol(
                /* [out] */ Int32* result);

            CARAPI ToString(
                /* [out] */ String* result);

        public:
            Int32 mRow;
            Int32 mCol;
        };

        class CCLineBuilder
            : public Object
        {
        public:
            CCLineBuilder(
                /* [in] */ const String& str);

            CARAPI SetCharAt(
                /* [in] */ Int32 index,
                /* [in] */ Char32 ch);

            CARAPI SetMidRowAt(
                /* [in] */ Int32 index,
                /* [in] */ StyleCode* m);

            CARAPI SetPACAt(
                /* [in] */ Int32 index,
                /* [in] */ PAC* pac);

            CARAPI CharAt(
                /* [in] */ Int32 index,
                /* [out] */ Char32* result);

            CARAPI Length(
                /* [out] */ Int32* result);

            CARAPI ApplyStyleSpan(
                /* [in] */ ISpannableStringBuilder* styledText,
                /* [in] */ StyleCode* s,
                /* [in] */ Int32 start,
                /* [in] */ Int32 end);

            CARAPI GetStyledText(
                /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
                /* [out] */ ISpannableStringBuilder** result);

        private:
            AutoPtr<StringBuilder> mDisplayChars;
            AutoPtr<ArrayOf<StyleCode*> > mMidRowStyles;
            AutoPtr<ArrayOf<StyleCode*> > mPACStyles;
        };

        class CCMemory
            : public Object
        {
        public:
            CCMemory();

            CARAPI Erase();

            CARAPI Der();

            CARAPI Tab(
                /* [in] */ Int32 tabs);

            CARAPI Bs();

            CARAPI Cr();

            CARAPI RollUp(
                /* [in] */ Int32 windowSize);

            CARAPI WriteText(
                /* [in] */ const String& text);

            CARAPI WriteMidRowCode(
                /* [in] */ StyleCode* m);

            CARAPI WritePAC(
                /* [in] */ PAC* pac);

            CARAPI GetStyledText(
                /* [in] */ ICaptioningManagerCaptionStyle* captionStyle,
                /* [out] */ ArrayOf<ISpannableStringBuilder*>** result);

            static CARAPI Clamp(
                /* [in] */ Int32 x,
                /* [in] */ Int32 min,
                /* [in] */ Int32 max,
                /* [out] */ Int32* result);

            CARAPI MoveCursorTo(
                /* [in] */ Int32 row,
                /* [in] */ Int32 col);

            CARAPI MoveCursorToRow(
                /* [in] */ Int32 row);

            CARAPI MoveCursorByCol(
                /* [in] */ Int32 col);

            CARAPI MoveBaselineTo(
                /* [in] */ Int32 baseRow,
                /* [in] */ Int32 windowSize);

            CARAPI GetLineBuffer(
                /* [in] */ Int32 row,
                /* [out] */ CCLineBuilder** result);

        private:
            String mBlankLine;
            AutoPtr<ArrayOf<CCLineBuilder*> > mLines;
            Int32 mRow;
            Int32 mCol;
        };

        class CCData
            : public Object
        {
        public:
            CCData(
                /* [in] */ byte type,
                /* [in] */ byte data1,
                /* [in] */ byte data2);

            static CARAPI FromByteArray(
                /* [in] */ ArrayOf<Byte>* data,
                /* [out] */ ArrayOf<CCData*>** result);

            CARAPI GetCtrlCode(
                /* [out] */ Int32* result);

            CARAPI GetMidRow(
                /* [out] */ StyleCode** result);

            CARAPI GetPAC(
                /* [out] */ PAC** result);

            CARAPI GetTabOffset(
                /* [out] */ Int32* result);

            CARAPI IsDisplayableChar(
                /* [out] */ Boolean* result);

            CARAPI GetDisplayText(
                /* [out] */ String* result);

            CARAPI ToString(
                /* [out] */ String* result);

            CARAPI CtrlCodeToString(
                /* [in] */ Int32 ctrlCode,
                /* [out] */ String* result);

            CARAPI IsBasicChar(
                /* [out] */ Boolean* result);

            CARAPI IsSpecialChar(
                /* [out] */ Boolean* result);

            CARAPI IsExtendedChar(
                /* [out] */ Boolean* result);

            CARAPI GetBasicChar(
                /* [in] */ Byte data,
                /* [out] */ Char32* result);

            CARAPI GetBasicChars(
                /* [out] */ String* result);

            CARAPI GetSpecialChar(
                /* [out] */ String* result);

            CARAPI GetExtendedChar(
                /* [out] */ String* result);

        private:
            byte mType;
            byte mData1;
            byte mData2;

            static AutoPtr<ArrayOf<String> > mCtrlCodeMap;

            static AutoPtr<ArrayOf<String> > mSpecialCharMap;

            static AutoPtr<ArrayOf<String> > mSpanishCharMap;

            static AutoPtr<ArrayOf<String> > mProtugueseCharMap;
        };

    public:
        CCParser(
            /* [in] */ IClosedCaptionRendererCCParserDisplayListener* listener);

        CARAPI Parse(
            /* [in] */ ArrayOf<Byte>* data);

    private:
        CARAPI_(AutoPtr<CCMemory>) GetMemory();

        CARAPI_(Boolean) HandleDisplayableChars(
            /* [in] */ CCData* ccData);

        CARAPI_(Boolean) HandleMidRowCode(
            /* [in] */ CCData* ccData);

        CARAPI_(Boolean) HandlePACCode(
            /* [in] */ CCData* ccData);

        CARAPI_(Boolean) HandleTabOffsets(
            /* [in] */ CCData* ccData);

        CARAPI_(Boolean) HandleCtrlCode(
            /* [in] */ CCData* ccData);

        CARAPI_(void) UpdateDisplay();

        CARAPI_(void) SwapMemory();

    public:
        static const Int32 MAX_ROWS;
        static const Int32 MAX_COLS;

    private:
        static const String TAG;
        static const Boolean DEBUG;

        static const Int32 INVALID;

        // EIA-CEA-608: Table 70 - Control Codes
        static const Int32 RCL;
        static const Int32 BS ;
        static const Int32 AOF;
        static const Int32 AON;
        static const Int32 DER;
        static const Int32 RU2;
        static const Int32 RU3;
        static const Int32 RU4;
        static const Int32 FON;
        static const Int32 RDC;
        static const Int32 TR ;
        static const Int32 RTD;
        static const Int32 EDM;
        static const Int32 CR ;
        static const Int32 ENM;
        static const Int32 EOC;

        // Transparent Space
        static const Char32 TS;

        // Captioning Modes
        static const Int32 MODE_UNKNOWN;
        static const Int32 MODE_PAINT_ON;
        static const Int32 MODE_ROLL_UP;
        static const Int32 MODE_POP_ON;
        static const Int32 MODE_TEXT;

        AutoPtr<IClosedCaptionRendererCCParserDisplayListener> mListener;

        Int32 mMode;
        Int32 mRollUpSize;

        AutoPtr<CCMemory> mDisplay;
        AutoPtr<CCMemory> mNonDisplay;
        AutoPtr<CCMemory> mTextMem;
    };

// TODO: Need SubtitleTrack
        // : public SubtitleTrack
    class ClosedCaptionTrack
        : public Object
    {
    public:
        ClosedCaptionTrack(
            /* [in] */ ClosedCaptionWidget* renderingWidget,
            /* [in] */ IMediaFormat* format);

        CARAPI OnData(
            /* [in] */ ArrayOf<Byte>* data,
            /* [in] */ Boolean eos,
            /* [in] */ Int64 runID);

        CARAPI GetRenderingWidget(
            /* [out] */ ISubtitleTrackRenderingWidget** result);

        CARAPI UpdateView(
            /* [in] */ IVector* activeCues);

    private:
        AutoPtr<ClosedCaptionWidget> mRenderingWidget;
        AutoPtr<CCParser> mCCParser;
    };

    class MutableBackgroundColorSpan
        : public CharacterStyle
        , public IClosedCaptionRendererMutableBackgroundColorSpan
        , public IUpdateAppearance
        , public IParcelableSpan
        , public IParcelable
    {
    public:
        CAR_INTERFACE_DECL()

        MutableBackgroundColorSpan(
            /* [in] */ Int32 color);

        MutableBackgroundColorSpan(
            /* [in] */ IParcel* src);

        CARAPI SetBackgroundColor(
            /* [in] */ Int32 color);

        CARAPI GetBackgroundColor(
            /* [out] */ Int32* result);

        CARAPI GetSpanTypeId(
            /* [out] */ Int32* result);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI UpdateDrawState(
            /* [in] */ ITextPaint* ds);

    private:
        Int32 mColor;
    };

public:
    CClosedCaptionRenderer();

    virtual ~CClosedCaptionRenderer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Called by {@link MediaPlayer}'s {@link SubtitleController} when a new
     * subtitle track is detected, to see if it should use this object to
     * parse and display this subtitle track.
     *
     * @param format the format of the track that will include at least
     *               the MIME type {@link MediaFormat@KEY_MIME}.
     *
     * @return true if and only if the track format is supported by this
     * renderer
     */
    CARAPI Supports(
        /* [in] */ IMediaFormat* format,
        /* [out] */ Boolean* result);

    /**
     * Called by {@link MediaPlayer}'s {@link SubtitleController} for each
     * subtitle track that was detected and is supported by this object to
     * create a {@link SubtitleTrack} object.  This object will be created
     * for each track that was found.  If the track is selected for display,
     * this object will be used to parse and display the track data.
     *
     * @param format the format of the track that will include at least
     *               the MIME type {@link MediaFormat@KEY_MIME}.
     * @return a {@link SubtitleTrack} object that will be used to parse
     * and render the subtitle track.
     */
    CARAPI CreateTrack(
        /* [in] */ IMediaFormat* format,
        /* [out] */ ISubtitleTrack** result);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<ClosedCaptionWidget> mRenderingWidget;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CCLOSEDCAPTIONRENDERER_H__
