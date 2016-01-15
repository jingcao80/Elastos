
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/style/StrikethroughSpan.h"

using Elastos::Droid::Graphics::IPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_4(StrikethroughSpan, CharacterStyle, IStrikethroughSpan, IUpdateAppearance, IParcelableSpan, IParcelable)

StrikethroughSpan::StrikethroughSpan()
{
}

StrikethroughSpan::~StrikethroughSpan()
{
}

ECode StrikethroughSpan::constructor()
{
    return NOERROR;
}

ECode StrikethroughSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::STRIKETHROUGH_SPAN;
    return NOERROR;
}

ECode StrikethroughSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode StrikethroughSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode StrikethroughSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    IPaint::Probe(ds)->SetStrikeThruText(TRUE);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
