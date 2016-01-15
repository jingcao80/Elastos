#include "elastos/droid/text/style/AbsoluteSizeSpan.h"
#include "elastos/droid/text/CTextPaint.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(AbsoluteSizeSpan, MetricAffectingSpan, IAbsoluteSizeSpan, IParcelableSpan, IParcelable)

AbsoluteSizeSpan::AbsoluteSizeSpan()
    : mSize(0)
    , mDip(0)
{}

AbsoluteSizeSpan::~AbsoluteSizeSpan()
{}

ECode AbsoluteSizeSpan::constructor()
{
    return NOERROR;
}

ECode AbsoluteSizeSpan::constructor(
    /* [in] */ Int32 size)
{
    mSize = size;
    return NOERROR;
}

ECode AbsoluteSizeSpan::constructor(
    /* [in] */ Int32 size,
    /* [in] */ Boolean dip)
{
    mSize = size;
    mDip = dip;
    return NOERROR;
}


ECode AbsoluteSizeSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::ABSOLUTE_SIZE_SPAN;
    return NOERROR;
}

ECode AbsoluteSizeSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mSize));
    FAIL_RETURN(source->ReadBoolean(&mDip));
    return NOERROR;
}

ECode AbsoluteSizeSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mSize));
    FAIL_RETURN(dest->WriteBoolean(mDip));
    return NOERROR;
}

ECode AbsoluteSizeSpan::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode AbsoluteSizeSpan::GetDip(
    /* [out] */ Boolean* dip)
{
    VALIDATE_NOT_NULL(dip)
    *dip = mDip;
    return NOERROR;
}

ECode AbsoluteSizeSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    if (mDip) {
        Float density;
        p->SetTextSize(mSize * (ds->GetDensity(&density), density));
    } else {
        p->SetTextSize(mSize);
    }
    return NOERROR;
}

ECode AbsoluteSizeSpan::UpdateMeasureState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint* p = IPaint::Probe(ds);
    if (mDip) {
        Float density;
        p->SetTextSize(mSize * (ds->GetDensity(&density), density));
    } else {
        p->SetTextSize(mSize);
    }
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos