#include "elastos/droid/text/style/UnderlineSpan.h"
#include "elastos/droid/text/CTextPaint.h"

using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_4(UnderlineSpan, CharacterStyle, IUnderlineSpan, IUpdateAppearance, IParcelableSpan, IParcelable)

UnderlineSpan::UnderlineSpan()
{
}

UnderlineSpan::~UnderlineSpan()
{
}

ECode UnderlineSpan::constructor()
{
    return NOERROR;
}

ECode UnderlineSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::UNDERLINE_SPAN;
    return NOERROR;
}

ECode UnderlineSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode UnderlineSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return NOERROR;
}

ECode UnderlineSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    CTextPaint* p = (CTextPaint*)ds;
    p->SetUnderlineText(TRUE);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos