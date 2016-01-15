
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/RelativeSizeSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(RelativeSizeSpan, MetricAffectingSpan, IRelativeSizeSpan, IParcelableSpan, IParcelable)

RelativeSizeSpan::RelativeSizeSpan()
    : mProportion(0)
{}

RelativeSizeSpan::~RelativeSizeSpan()
{}

ECode RelativeSizeSpan::constructor()
{
    return NOERROR;
}

ECode RelativeSizeSpan::constructor(
    /* [in] */ Float proportion)
{
    mProportion = proportion;
    return NOERROR;
}

ECode RelativeSizeSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::RELATIVE_SIZE_SPAN;
    return NOERROR;
}

ECode RelativeSizeSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadFloat(&mProportion);
}

ECode RelativeSizeSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteFloat(mProportion);
}

ECode RelativeSizeSpan::GetSizeChange(
    /* [out] */ Float* change)
{
    VALIDATE_NOT_NULL(change)
    *change = mProportion;
    return NOERROR;
}

ECode RelativeSizeSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textSize;
    p->GetTextSize(&textSize);
    p->SetTextSize(textSize * mProportion);
    return NOERROR;
}

ECode RelativeSizeSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textSize;
    p->GetTextSize(&textSize);
    p->SetTextSize(textSize * mProportion);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastoss
