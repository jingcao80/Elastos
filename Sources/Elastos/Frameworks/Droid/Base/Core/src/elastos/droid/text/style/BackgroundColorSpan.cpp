#include "elastos/droid/text/style/BackgroundColorSpan.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_4(BackgroundColorSpan, CharacterStyle, IBackgroundColorSpan, IUpdateAppearance, IParcelableSpan, IParcelable)

BackgroundColorSpan::BackgroundColorSpan()
    : mColor(0)
{}

BackgroundColorSpan::~BackgroundColorSpan()
{}

ECode BackgroundColorSpan::constructor()
{
    return NOERROR;
}

ECode BackgroundColorSpan::constructor(
    /* [in] */ Int32 color)
{
   mColor = color;
   return NOERROR;
}

ECode BackgroundColorSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::BACKGROUND_COLOR_SPAN;
    return NOERROR;
}

ECode BackgroundColorSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mColor);
}

ECode BackgroundColorSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mColor);
}

ECode BackgroundColorSpan::GetBackgroundColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mColor;
    return NOERROR;
}

ECode BackgroundColorSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    ds->SetBgColor(mColor);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos