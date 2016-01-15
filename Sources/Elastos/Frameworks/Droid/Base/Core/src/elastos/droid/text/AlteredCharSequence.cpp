#include "elastos/droid/text/AlteredCharSequence.h"
#include "elastos/droid/text/CAlteredCharSequence.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL(AlteredSpanned, AlteredCharSequence, ISpanned)

ECode AlteredSpanned::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend)
{
    FAIL_RETURN(AlteredCharSequence::constructor(source, sub, substart, subend))
    mSpanned = ISpanned::Probe(source);
    return NOERROR;
}

ECode AlteredSpanned::GetSpans(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ const InterfaceID& type,
    /*[out, callee]*/ ArrayOf<IInterface*>** objs)
{
    return mSpanned->GetSpans(start, end, type, objs);
}

ECode AlteredSpanned::GetSpanStart(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* start)
{
    return mSpanned->GetSpanStart(tag, start);
}

ECode AlteredSpanned::GetSpanEnd(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* end)
{
    return mSpanned->GetSpanEnd(tag, end);
}

ECode AlteredSpanned::GetSpanFlags(
    /* [in] */ IInterface* tag,
    /* [out] */ Int32* flags)
{
    return mSpanned->GetSpanFlags(tag, flags);
}

ECode AlteredSpanned::NextSpanTransition(
    /* [in] */ Int32 start,
    /* [in] */ Int32 limit,
    /* [in] */ const InterfaceID& type,
    /* [out] */ Int32* offset)
{
    return mSpanned->NextSpanTransition(start, limit, type, offset);
}

ECode AlteredSpanned::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("AlteredSpanned");
    return NOERROR;
}


CAR_INTERFACE_IMPL_3(AlteredCharSequence, Object, IAlteredCharSequence, IGetChars, ICharSequence)

/**
 * Create an AlteredCharSequence whose text (and possibly spans)
 * are mirrored from <code>source</code>, except that the range of
 * offsets <code>substart</code> inclusive to <code>subend</code> exclusive
 * are mirrored instead from <code>sub</code>, beginning at offset 0.
 */
AutoPtr<IAlteredCharSequence> AlteredCharSequence::Make(
    /* [in] */ ICharSequence* source,
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend)
{
    AutoPtr<IAlteredCharSequence> acs;
    ISpanned* spanned = ISpanned::Probe(source);
    if (spanned != NULL) {
        AutoPtr<AlteredSpanned> as = new AlteredSpanned();
        as->constructor(source, sub, substart, subend);
        acs = (IAlteredCharSequence*)as.Get();
    }
    else {
        CAlteredCharSequence::New(source, sub, substart, subend,
            (IAlteredCharSequence**)&acs);
    }

    return acs;
}

AlteredCharSequence::AlteredCharSequence()
    : mStart(0)
    , mEnd(0)
{}

AlteredCharSequence::~AlteredCharSequence()
{}

ECode AlteredCharSequence::constructor(
    /* [in] */ ICharSequence* source,
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend)
{
    mSource = source;
    mChars = sub;
    mStart = substart;
    mEnd = subend;
    return NOERROR;
}

/* package */
void AlteredCharSequence::Update(
    /* [in] */ ArrayOf<Char32>* sub,
    /* [in] */ Int32 substart,
    /* [in] */ Int32 subend)
{
    mChars = sub;
    mStart = substart;
    mEnd = subend;
}

ECode AlteredCharSequence::GetCharAt(
    /* [in] */ Int32 off,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);

    if (off >= mStart && off < mEnd) {
        *c = (*mChars)[off - mStart];
        return NOERROR;
    }
    else {
        return mSource->GetCharAt(off, c);
    }
}

ECode AlteredCharSequence::GetLength(
    /* [out] */ Int32* number)
{
    return mSource -> GetLength(number);
}

ECode AlteredCharSequence::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq)

    AutoPtr<ICharSequence> csq1;
    mSource->SubSequence(start, end, (ICharSequence**)&csq1);

    AutoPtr<IAlteredCharSequence> ret = Make(csq1, mChars, mStart - start, mEnd - start);
    *csq = ICharSequence::Probe(ret);
    REFCOUNT_ADD(*csq);
    return NOERROR;
}

ECode AlteredCharSequence::GetChars(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ArrayOf<Char32>* dest,
    /* [in] */ Int32 off)
{
    assert(0 && "TODO");
    // TextUtils::GetChars(mSource, start, end, dest, off);

    Int32 start1, end1;
    start1 = Elastos::Core::Math::Max(mStart, start);
    end1 = Elastos::Core::Math::Min(mEnd, end);

    if (start1 > end1) {
        dest->Copy(off, mChars, start - mStart, end - start);
    }
    return NOERROR;
}

ECode AlteredCharSequence::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Int32 len;
    GetLength(&len);

    AutoPtr< ArrayOf<Char32> > ret = ArrayOf<Char32>::Alloc(len);
    GetChars(0, len, ret, 0);

    StringBuilder sb(ret->GetLength());
    for (Int32 i = 0; i < ret->GetLength(); i++) {
        sb.AppendChar((*ret)[i]);
    }

    *str = sb.ToString();
    return NOERROR;
}


} // namespace Text
} // namepsace Droid
} // namespace Elastos