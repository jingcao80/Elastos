
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/ForegroundColorSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_4(ForegroundColorSpan, CharacterStyle, IForegroundColorSpan, IUpdateAppearance, IParcelableSpan, IParcelable)

ForegroundColorSpan::ForegroundColorSpan()
    : mColor(0)
{}

ForegroundColorSpan::~ForegroundColorSpan()
{}

ECode ForegroundColorSpan::constructor()
{
    return NOERROR;
}

ECode ForegroundColorSpan::constructor(
    /* [in] */ Int32 color)
{
    mColor = color;
    return NOERROR;
}

ECode ForegroundColorSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::FOREGROUND_COLOR_SPAN;
    return NOERROR;
}

ECode ForegroundColorSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInt32(&mColor);
}

ECode ForegroundColorSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInt32(mColor);;
}

ECode ForegroundColorSpan::GetForegroundColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color)
    *color = mColor;
    return NOERROR;
}

ECode ForegroundColorSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint::Probe(ds)->SetColor(mColor);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
