#include "elastos/droid/text/style/CharacterStyle.h"
#include "elastos/droid/text/style/MetricAffectingSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

//===========================================================
// CharacterStyle
//===========================================================
CAR_INTERFACE_IMPL(CharacterStyle, Object, ICharacterStyle)

CharacterStyle::CharacterStyle()
{
}

CharacterStyle::~CharacterStyle()
{
}

AutoPtr<ICharacterStyle> CharacterStyle::Wrap(
    /* [in] */ ICharacterStyle* cs)
{
    AutoPtr<IMetricAffectingSpan> mas = IMetricAffectingSpan::Probe(cs);
    if (mas != NULL) {
        AutoPtr<ICharacterStyle> ret = (ICharacterStyle*)new MetricAffectingSpanPassthrough(mas);
        return ret;
    }
    else {
        AutoPtr<ICharacterStyle> ret = (ICharacterStyle*)new CharacterStylePassthrough(cs);
        return ret;
    }
}

ECode CharacterStyle::GetUnderlying(
    /* [out] */ ICharacterStyle** style)
{
    VALIDATE_NOT_NULL(style)
    *style = this;
    REFCOUNT_ADD(*style);
    return NOERROR;
}

//===========================================================
// CharacterStylePassthrough
//===========================================================
CharacterStylePassthrough::CharacterStylePassthrough(
    /* [in] */ ICharacterStyle* cs)
{
    mStyle = cs;
}

ECode CharacterStylePassthrough::UpdateDrawState(
    /* [in] */ ITextPaint* tp)
{
    return mStyle->UpdateDrawState(tp);
}

ECode CharacterStylePassthrough::GetUnderlying(
    /* [out] */ ICharacterStyle** result)
{
    return mStyle->GetUnderlying(result);
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos