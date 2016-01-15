#include "elastos/droid/text/style/LeadingMarginSpanStandard.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {


CAR_INTERFACE_IMPL_5(LeadingMarginSpanStandard, Object, ILeadingMarginSpanStandard, ILeadingMarginSpan, IParagraphStyle, IParcelableSpan, IParcelable)

LeadingMarginSpanStandard::LeadingMarginSpanStandard()
    : mFirst(0)
    , mRest(0)
{
}

LeadingMarginSpanStandard::~LeadingMarginSpanStandard()
{}

ECode LeadingMarginSpanStandard::constructor()
{
    return NOERROR;
}

ECode LeadingMarginSpanStandard::constructor(
    /* [in] */ Int32 first,
    /* [in] */ Int32 rest)
{
    mFirst = first;
    mRest = rest;
    return NOERROR;
}

ECode LeadingMarginSpanStandard::constructor(
    /* [in] */ Int32 every)
{
    return constructor(every, every);
}

ECode LeadingMarginSpanStandard::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::LEADING_MARGIN_SPAN;
    return NOERROR;
}

ECode LeadingMarginSpanStandard::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mFirst);
    source->ReadInt32(&mRest);
    return NOERROR;
}

ECode LeadingMarginSpanStandard::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mFirst);
    dest->WriteInt32(mRest);
    return NOERROR;
}

ECode LeadingMarginSpanStandard::GetLeadingMargin(
    /* [in] */ Boolean first,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (first) {
        *value = mFirst;
    }
    else {
        *value = mRest;
    }
    return NOERROR;
}

ECode LeadingMarginSpanStandard::DrawLeadingMargin(
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
    /* [in] */ ILayout* layout)
{
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos