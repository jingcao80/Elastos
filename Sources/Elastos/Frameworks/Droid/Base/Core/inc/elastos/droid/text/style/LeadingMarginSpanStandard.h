#ifndef __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__
#define __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::LayoutAlignment;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class LeadingMarginSpanStandard
    : public Object
    , public ILeadingMarginSpanStandard
    , public ILeadingMarginSpan
    , public IParagraphStyle
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LeadingMarginSpanStandard();

    virtual ~LeadingMarginSpanStandard();

    CARAPI constructor();

    /**
     * Constructor taking separate indents for the first and subsequent
     * lines.
     *
     * @param first the indent for the first line of the paragraph
     * @param rest the indent for the remaining lines of the paragraph
     */
    CARAPI constructor(
        /* [in] */ Int32 first,
        /* [in] */ Int32 rest);

    /**
     * Constructor taking an indent for all lines.
     * @param every the indent of each line
     */
    CARAPI constructor(
        /* [in] */ Int32 every);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetLeadingMargin(
        /* [in] */ Boolean first,
        /* [out] */ Int32* value);

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
    Int32 mFirst;
    Int32 mRest;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_LEADINGMARGINSPAN_STANDARD_H__
