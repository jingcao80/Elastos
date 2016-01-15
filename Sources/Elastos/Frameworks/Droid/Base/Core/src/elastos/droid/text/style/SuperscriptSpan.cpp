#include "elastos/droid/text/style/SuperscriptSpan.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SuperscriptSpan, MetricAffectingSpan, ISuperscriptSpan, IParcelableSpan, IParcelable)

SuperscriptSpan::SuperscriptSpan()
{
}

SuperscriptSpan::~SuperscriptSpan()
{
}

ECode SuperscriptSpan::constructor()
{
    return NOERROR;
}

ECode SuperscriptSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SUPERSCRIPT_SPAN;
    return NOERROR;
}

ECode SuperscriptSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode SuperscriptSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode SuperscriptSpan::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    CTextPaint* p = (CTextPaint*)tp;
    Float fAscent;
    Int32 baselineShift;
    p->GetBaselineShift(&baselineShift);
    p->Ascent(&fAscent);
    p->SetBaselineShift(baselineShift + (Int32)(fAscent / 2));
    return NOERROR;
}

ECode SuperscriptSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* tp)
{
    VALIDATE_NOT_NULL(tp);
    CTextPaint* p = (CTextPaint*)tp;
    Float fAscent;
    Int32 baselineShift;
    p->GetBaselineShift(&baselineShift);
    p->Ascent(&fAscent);
    p->SetBaselineShift(baselineShift + (Int32)(fAscent / 2));
    return NOERROR;}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos