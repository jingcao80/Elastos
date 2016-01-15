
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/ScaleXSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(ScaleXSpan, MetricAffectingSpan, IScaleXSpan, IParcelableSpan, IParcelable)

ScaleXSpan::ScaleXSpan()
    : mProportion(0)
{}

ScaleXSpan::~ScaleXSpan()
{}

ECode ScaleXSpan::constructor()
{
    return NOERROR;
}

ECode ScaleXSpan::constructor(
    /* [in] */ Float proportion)
{
    mProportion = proportion;
    return NOERROR;
}

ECode ScaleXSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SCALE_X_SPAN;
    return NOERROR;
}

ECode ScaleXSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadFloat(&mProportion);
}

ECode ScaleXSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteFloat(mProportion);
}

ECode ScaleXSpan::GetScaleX(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x)
    *x = mProportion;
    return NOERROR;
}

ECode ScaleXSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textScaleX;
    p->GetTextScaleX(&textScaleX);
    p->SetTextScaleX(textScaleX * mProportion);
    return NOERROR;
}

ECode ScaleXSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    Float textScaleX;
    p->GetTextScaleX(&textScaleX);
    p->SetTextScaleX(textScaleX * mProportion);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
