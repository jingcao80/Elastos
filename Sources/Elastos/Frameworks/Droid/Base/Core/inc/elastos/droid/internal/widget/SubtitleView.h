
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_SUBTITLEVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_SUBTITLEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Internal::Widget::ISubtitleView;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Text::LayoutAlignment;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::View;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class SubtitleView
    : public Elastos::Droid::View::View
    , public ISubtitleView
{
public:
    CAR_INTERFACE_DECL()

    SubtitleView();

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

    virtual CARAPI SetText(
        /* [in] */ Int32 resId);

    virtual CARAPI SetText(
        /* [in] */ ICharSequence* text);

    virtual CARAPI SetForegroundColor(
        /* [in] */ Int32 color);

    // @Override
    CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    virtual CARAPI SetEdgeType(
        /* [in] */ Int32 edgeType);

    virtual CARAPI SetEdgeColor(
        /* [in] */ Int32 color);

    /**
      * Sets the text size in pixels.
      *
      * @param size the text size in pixels
      */
    virtual CARAPI SetTextSize(
        /* [in] */ Float size);

    virtual CARAPI SetTypeface(
        /* [in] */ ITypeface* typeface);

    virtual CARAPI SetAlignment(
        /* [in] */ LayoutAlignment textAlignment);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI SetStyle(
        /* [in] */ Int32 styleId);

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* c);

private:
    CARAPI_(Boolean) ComputeMeasurements(
        /* [in] */ Int32 maxWidth);

private:
    static const Float INNER_PADDING_RATIO;
    static const Int32 COLOR_BEVEL_DARK = 0x80000000;
    static const Int32 COLOR_BEVEL_LIGHT = 0x80FFFFFF;
    /*const*/ Float mCornerRadius;
    /*const*/ Float mOutlineWidth;
    /*const*/ Float mShadowRadius;
    /*const*/ Float mShadowOffsetX;
    /*const*/ Float mShadowOffsetY;
    /*const*/ AutoPtr<IRectF> mLineBounds;
    /*const*/ AutoPtr<StringBuilder> mText;
    LayoutAlignment mAlignment;
    AutoPtr<ITextPaint> mTextPaint;
    AutoPtr<IPaint> mPaint;
    Int32 mForegroundColor;
    Int32 mBackgroundColor;
    Int32 mEdgeColor;
    Int32 mEdgeType;
    Boolean mHasMeasurements;
    Int32 mLastMeasuredWidth;
    AutoPtr<IStaticLayout> mLayout;
    Float mSpacingMult;
    Float mSpacingAdd;
    Int32 mInnerPaddingX;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_SUBTITLEVIEW_H__

