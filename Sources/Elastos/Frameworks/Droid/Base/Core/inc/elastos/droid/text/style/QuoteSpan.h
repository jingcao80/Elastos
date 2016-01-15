#ifndef __ELASTOS_DROID_TEXT_STYLE_QuoteSpan_H__
#define __ELASTOS_DROID_TEXT_STYLE_QuoteSpan_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class QuoteSpan
    : public Object
    , public IQuoteSpan
    , public ILeadingMarginSpan
    , public IParcelableSpan
    , public IParcelable
    , public IParagraphStyle
{
public:
    CAR_INTERFACE_DECL()

    QuoteSpan();

    virtual ~QuoteSpan();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 color);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetColor(
        /* [out] */ Int32* color);

    CARAPI GetLeadingMargin(
        /* [in] */ Boolean first,
        /* [in] */ Int32* margin);

    CARAPI DrawLeadingMargin(
        /* [in] */ ICanvas* c,
        /* [in] */ IPaint* p,
        /* [in] */ Int32 x,
        /* [in] */ Int32 dir,
        /* [in] */ Int32 top,
        /* [in] */ Int32 baseline,
        /* [in] */ Int32 bottom,
        /* [in] */ ICharSequence* text,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Boolean first,
        /* [in] */ ILayout* layout);

private:
    static const Int32 STRIPE_WIDTH;// = 2;
    static const Int32 GAP_WIDTH;// = 2;

    /*const*/ Int32 mColor;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_QuoteSpan_H__
