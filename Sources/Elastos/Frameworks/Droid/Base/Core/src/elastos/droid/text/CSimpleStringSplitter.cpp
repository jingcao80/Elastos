
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CSimpleStringSplitter.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::EIID_IIterator;

namespace Elastos {
namespace Droid {
namespace Text {

CAR_INTERFACE_IMPL_3(CSimpleStringSplitter, Object, ISimpleStringSplitter, IStringSplitter, IIterator)

CAR_OBJECT_IMPL(CSimpleStringSplitter)

CSimpleStringSplitter::CSimpleStringSplitter()
    : mDelimiter(' ')
    , mPosition(0)
    , mLength(0)
{}

ECode CSimpleStringSplitter::constructor(
    /*  [in] */ Char32 delimiter)
{
    mDelimiter = delimiter;
    return NOERROR;
}

ECode CSimpleStringSplitter::SetString(
    /*  [in] */ const String& string)
{
    mString = string;
    mPosition = 0;
    mLength = mString.GetLength();
    return NOERROR;
}

ECode CSimpleStringSplitter::GetIterator(
   /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    *it = this;
    REFCOUNT_ADD(*it);
    return NOERROR;
}

ECode CSimpleStringSplitter::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPosition < mLength;
    return NOERROR;
}

ECode CSimpleStringSplitter::GetNext(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    Int32 end = mString.IndexOf(mDelimiter, mPosition);
    if (end == -1) {
        end = mLength;
    }

    if (mPosition > end) {
        *str = String(NULL);
        return E_STRING_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    String nextString = mString.Substring(mPosition, end);
    mPosition = end + 1; // Skip the delimiter.

    *str = nextString;
    return NOERROR;
}

ECode CSimpleStringSplitter::GetNext(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    String str;
    GetNext(&str);

    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    *obj = seq;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CSimpleStringSplitter::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Text
} // namepsace Droid
} // namespace Elastos
