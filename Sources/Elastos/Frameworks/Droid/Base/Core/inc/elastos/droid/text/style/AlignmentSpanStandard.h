#ifndef __ELASTOS_DROID_TEXT_STYLE_ALIGNMENTSPAN_STANDARD_H__
#define __ELASTOS_DROID_TEXT_STYLE_ALIGNMENTSPAN_STANDARD_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::LayoutAlignment;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

class AlignmentSpanStandard
    : public Object
    , public IAlignmentSpanStandard
    , public IAlignmentSpan
    , public IParagraphStyle
    , public IParcelableSpan
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AlignmentSpanStandard();

    virtual ~AlignmentSpanStandard();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ LayoutAlignment align);

    CARAPI GetAlignment(
        /* [out] */ LayoutAlignment* align);

    CARAPI GetSpanTypeId(
        /* [out] */ Int32* id);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    LayoutAlignment mAlignment;
};

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_STYLE_ALIGNMENTSPAN_STANDARD_H__
