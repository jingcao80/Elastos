#include "elastos/droid/text/style/ClickableSpan.h"
#include "elastos/droid/text/CTextPaint.h"

using Elastos::Droid::Text::CTextPaint;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL(ClickableSpan, CharacterStyle, IClickableSpan)

ECode ClickableSpan::UpdateDrawState(
    /* [in] */ ITextPaint* ds)
{
    VALIDATE_NOT_NULL(ds);
    CTextPaint* p = (CTextPaint*)ds;
    Int32 linkColor;
    p->GetLinkColor(&linkColor);
    p->SetColor(linkColor);
    p->SetUnderlineText(TRUE);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos