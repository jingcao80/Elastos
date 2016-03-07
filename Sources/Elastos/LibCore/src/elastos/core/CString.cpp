#include "CString.h"
#include "StringBuilder.h"
#include <cutils/log.h>

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_3(CString, Object, ICharSequence, IString, IComparable)

CAR_OBJECT_IMPL(CString)

ECode CString::constructor(
    /* [in] */ const String& str)
{
    if (str.IsNull()) {
        // ALOGW("CString: null string when create ICharSequence.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mString = str;
    return NOERROR;
}

ECode CString::GetLength(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mString.GetLength();
    return NOERROR;
}

ECode CString::GetCharAt(
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

ECode CString::SubSequence(
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
    CString::New(subStr, csq);
    return NOERROR;
}

ECode CString::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mString;
    return NOERROR;
}

ECode CString::Equals(
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

ECode CString::GetHashCode(
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

ECode CString::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AutoPtr<ICharSequence> res = ICharSequence::Probe(another);
    if (res == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String str;
    res->ToString(&str);
    *result = mString.Compare(str);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
