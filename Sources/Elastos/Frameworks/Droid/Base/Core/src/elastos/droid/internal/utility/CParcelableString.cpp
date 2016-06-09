
#include "elastos/droid/internal/utility/CParcelableString.h"

using Elastos::Core::EIID_ICharSequence;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL_2(CParcelableString, Object, IParcelableString, ICharSequence)

CAR_OBJECT_IMPL(CParcelableString)

ECode CParcelableString::constructor()
{
    return NOERROR;
}

ECode CParcelableString::constructor(
    /* [in] */ const String& str)
{
    mString = str;
    return NOERROR;
}

ECode CParcelableString::constructor(
    /* [in] */ ICharSequence* cs)
{
    cs->ToString(&mString);
    return NOERROR;
}

ECode CParcelableString::SetString(
    /* [in] */ const String& str)
{
    mString = str;
    return NOERROR;
}

ECode CParcelableString::GetString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = mString;
    return NOERROR;
}

ECode CParcelableString::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadString(&mString);
}

ECode CParcelableString::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteString(mString);
}

ECode CParcelableString::GetLength(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mString.GetLength();
    return NOERROR;
}

ECode CParcelableString::GetCharAt(
    /* [in] */ Int32 index,
    /* [out] */ Char32* c)
{
    VALIDATE_NOT_NULL(c);

    if (index < 0 || index >= mString.GetLength()) {
        *c = '\0';
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *c = mString.GetChar(index);

    return NOERROR;
}

ECode CParcelableString::SubSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);

    if (start < 0 || start >= end) {
        *csq = NULL;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String subStr = mString.Substring(start, end);
    CParcelableString::New(subStr, csq);
    return NOERROR;
}

ECode CParcelableString::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mString;
    return NOERROR;
}

ECode CParcelableString::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(obj)

    if (ICharSequence::Probe(obj) == NULL) {
        return NOERROR;
    }

    String str = Object::ToString(obj);
    *result = mString.Equals(str);
    return NOERROR;
}

ECode CParcelableString::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    Int64 h = 0;
    const char *string = mString.string();
    for ( ; *string; ++string) {
        h = 5 * h + *string;
    }
    *hashCode = (Int32)h;
    return NOERROR;
}

ECode CParcelableString::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    VALIDATE_NOT_NULL(another);

    AutoPtr<ICharSequence> res = ICharSequence::Probe(another);
    if (res == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }

    String str;
    res->ToString(&str);
    *result = mString.Compare(str);
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
