#include "elastos/droid/text/style/SubscriptSpan.h"
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SubscriptSpan, MetricAffectingSpan, ISubscriptSpan, IParcelableSpan, IParcelable)

SubscriptSpan::SubscriptSpan()
{
}

SubscriptSpan::~SubscriptSpan()
{
}

ECode SubscriptSpan::constructor()
{
    return NOERROR;
}

ECode SubscriptSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUBSCRIPT_SPAN;
    return NOERROR;
}

ECode SubscriptSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode SubscriptSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode SubscriptSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    Float fAscent;
    Int32 baselineShift;
    tp->GetBaselineShift(&baselineShift);
    IPaint::Probe(tp)->Ascent(&fAscent);
    tp->SetBaselineShift(baselineShift-(Int32) (fAscent / 2));
    return NOERROR;
}

ECode SubscriptSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* tp)
{
    Float fAscent;
    Int32 baselineShift;
    tp->GetBaselineShift(&baselineShift);
    IPaint::Probe(tp)->Ascent(&fAscent);
    tp->SetBaselineShift(baselineShift-(Int32) (fAscent / 2));
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos