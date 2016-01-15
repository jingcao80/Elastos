#include "elastos/droid/text/CSpannedString.h"

using Elastos::Core::EIID_ICharSequence;
using Elastos::Droid::Text::CSpannedString;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_4(CSpannedString, SpannableStringInternal, ISpannedString, ICharSequence, IGetChars, ISpanned)

CSpannedString::CSpannedString()
{
}

CSpannedString::~CSpannedString()
{}

ECode CSpannedString::constructor(
    /* [in] */ ICharSequence* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 len;
    source->GetLength(&len);
    return SpannableStringInternal::constructor(source, 0, len);
}

ECode CSpannedString::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    return SpannableStringInternal::constructor(source, start, end);
}

ECode CSpannedString::GetLength(
    /* [out] */ Int32* number)
{
    return SpannableStringInternal::GetLength(number);
}

ECode CSpannedString::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    return SpannableStringInternal::GetCharAt(index, c);
}

ECode CSpannedString::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);
    *csq = NULL;

    AutoPtr<ISpannedString> spannedString;
    CSpannedString::New(ICharSequence::Probe(THIS_PROBE(ISpannedString)), start, end, (ISpannedString**)&spannedString);
    *csq = ICharSequence::Probe(spannedString);
    REFCOUNT_ADD(*csq)
    return NOERROR;
}

ECode CSpannedString::ToString(
    /* [out] */ String* str)
{
    return SpannableStringInternal::ToString(str);
}

ECode CSpannedString::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 destoff)
{
    return SpannableStringInternal::GetChars(start, end, dest, destoff);
}

ECode CSpannedString::GetSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type,
    /* [out, callee] */ ArrayOf<IInterface*>** objs)
{
    return SpannableStringInternal::GetSpans(start, end, type, objs);
}

ECode CSpannedString::GetSpanStart(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* start)
{
    return SpannableStringInternal::GetSpanStart(tag, start);
}

ECode CSpannedString::GetSpanEnd(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* end)
{
    return SpannableStringInternal::GetSpanEnd(tag, end);
}

ECode CSpannedString::GetSpanFlags(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* flags)
{
    return SpannableStringInternal::GetSpanFlags(tag, flags);
}

ECode CSpannedString::NextSpanTransition(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ const InterfaceID& type,
    /* [out] */ Int32* offset)
{
    return SpannableStringInternal::NextSpanTransition(start, limit, type, offset);
}

AutoPtr<ISpannedString> CSpannedString::ValueOf(
        /* [in] */ ICharSequence* source)
{
    AutoPtr<ISpannedString> ss = ISpannedString::Probe(source);
    if (ss != NULL) {
        return ss;
    }

    AutoPtr<CSpannedString> css;
    CSpannedString::NewByFriend(source, (CSpannedString**)&css);
    return css;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
