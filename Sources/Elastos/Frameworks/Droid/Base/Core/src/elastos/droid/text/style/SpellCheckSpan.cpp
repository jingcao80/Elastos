#include "elastos/droid/text/style/SpellCheckSpan.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(SpellCheckSpan, Object, ISpellCheckSpan, IParcelableSpan, IParcelable)

SpellCheckSpan::SpellCheckSpan()
    : mSpellCheckInProgress(FALSE)
{
}

SpellCheckSpan::~SpellCheckSpan()
{
}

ECode SpellCheckSpan::constructor()
{
    return NOERROR;
}

ECode SpellCheckSpan::SetSpellCheckInProgress(
    /* [in] */ Boolean inProgress)
{
    mSpellCheckInProgress = inProgress;
    return NOERROR;
}

ECode SpellCheckSpan::IsSpellCheckInProgress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSpellCheckInProgress;
    return NOERROR;
}

ECode SpellCheckSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadBoolean(&mSpellCheckInProgress);
}

ECode SpellCheckSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteBoolean(mSpellCheckInProgress);
}

ECode SpellCheckSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::SPELL_CHECK_SPAN;
    return NOERROR;
}


} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos