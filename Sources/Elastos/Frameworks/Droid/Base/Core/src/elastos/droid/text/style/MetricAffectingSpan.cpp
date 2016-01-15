#include "elastos/droid/text/style/MetricAffectingSpan.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

//==================================================================
// MetricAffectingSpan
//==================================================================

CAR_INTERFACE_IMPL_3(MetricAffectingSpan, CharacterStyle, IMetricAffectingSpan, IUpdateLayout, IUpdateAppearance)

MetricAffectingSpan::MetricAffectingSpan()
{
}

MetricAffectingSpan::~MetricAffectingSpan()
{
}

ECode MetricAffectingSpan::GetUnderlying(
    /* [out] */ ICharacterStyle** style)
{
    VALIDATE_NOT_NULL(style)
    *style = THIS_PROBE(ICharacterStyle);
    REFCOUNT_ADD(*style);
    return NOERROR;
}

//==================================================================
// MetricAffectingSpanPassthrough
//==================================================================

MetricAffectingSpanPassthrough::MetricAffectingSpanPassthrough(
    /* [in] */ IMetricAffectingSpan* cs)
{
    mStyle = cs;
}

ECode MetricAffectingSpanPassthrough::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    return ICharacterStyle::Probe(mStyle)->UpdateDrawState(tp);
}

ECode MetricAffectingSpanPassthrough::UpdateMeasureState(
    /* [in] */ ITextPaint* tp)
{
    return mStyle->UpdateMeasureState(tp);
}

ECode MetricAffectingSpanPassthrough::GetUnderlying(
    /* [in] */ ICharacterStyle** result)
{
    return ICharacterStyle::Probe(mStyle)->GetUnderlying(result);
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos