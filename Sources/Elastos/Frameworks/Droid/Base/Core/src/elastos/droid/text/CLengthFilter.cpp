
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CLengthFilter.h"
#include <elastos/core/Character.h>

using Elastos::Core::Character;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_2(CLengthFilter, Object, ILengthFilter, IInputFilter)

CAR_OBJECT_IMPL(CLengthFilter)

CLengthFilter::CLengthFilter()
    : mMax(0)
{}

ECode CLengthFilter::constructor(
    /* [in] */ Int32 max)
{
    mMax = max;
    return NOERROR;
}

ECode CLengthFilter::Filter(
    /* [in] */ ICharSequence* source,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [in] */ ISpanned* dest,
    /* [in] */ Int32 dstart,
    /* [in] */ Int32 dend,
    /* [out] */ ICharSequence** cs)
{
    VALIDATE_NOT_NULL(cs);

    Int32 len;
    ICharSequence::Probe(dest)->GetLength(&len);
    Int32 keep = mMax - (len - (dend - dstart));

    if (keep <= 0) {
        return CString::New(String(""), cs);
    }
    else if (keep >= end - start) {
        *cs = NULL;
        return NOERROR; // keep original
    }
    else {
        keep += start;
        Char32 sourceChar;
        if (Character::IsHighSurrogate((source->GetCharAt(keep - 1, &sourceChar), sourceChar))) {
            --keep;
            if (keep == start) {
                return CString::New(String(""), cs);
            }
        }
        return source->SubSequence(start, keep, cs);
    }
}

ECode CLengthFilter::GetMax(
    /* [out] */ Int32* max)
{
    VALIDATE_NOT_NULL(max)
    *max = mMax;
    return NOERROR;
}

}//namespace Text
}//namespace Droid
}//namespace Elastos