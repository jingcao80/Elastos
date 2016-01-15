#include "elastos/droid/text/style/ReplacementSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL(ReplacementSpan, MetricAffectingSpan, IReplacementSpan)

ECode ReplacementSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* p)
{
    return NOERROR;
}

ECode ReplacementSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos