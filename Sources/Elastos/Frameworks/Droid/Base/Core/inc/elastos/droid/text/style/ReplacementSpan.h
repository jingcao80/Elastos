#ifndef __ELASTOS_DROID_TEXT_STYLE_REPLACEMENTSPAN_H__
#define __ELASTOS_DROID_TEXT_STYLE_REPLACEMENTSPAN_H__

#include "elastos/droid/text/style/MetricAffectingSpan.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPaintFontMetricsInt;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class ReplacementSpan
    : public MetricAffectingSpan
    , public IReplacementSpan
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI GetSize(
        /* [in] */ IPaint* paint,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ IPaintFontMetricsInt* fm,
        /* [in] */ Int32* size) = 0;

    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Float x,
        /* [in] */ Int32 top,
        /* [in] */ Int32 y,
        /* [in] */ Int32 bottom,
        /* [in] */ IPaint* paint) = 0;

    /**
     * This method does nothing, since ReplacementSpans are measured
     * explicitly instead of affecting Paint properties.
     */
    CARAPI UpdateMeasureState(
        /* [in] */ ITextPaint* p);

    /**
     * This method does nothing, since ReplacementSpans are drawn
     * explicitly instead of affecting Paint properties.
     */
    CARAPI UpdateDrawState(
        /* [in] */ ITextPaint* ds);
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_REPLACEMENTSPAN_H__
