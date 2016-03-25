
#include "CArrayOf.h"
#include "StringBuilder.h"
#include <elastos/utility/Arrays.h>

using Elastos::Utility::Arrays;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL(CArrayOf, Object, IArrayOf)

CAR_OBJECT_IMPL(CArrayOf)

CArrayOf::CArrayOf()
    : mElements(NULL)
{}

CArrayOf::~CArrayOf()
{
    mElements = NULL;
}

ECode CArrayOf::constructor(
    /* [in] */ const InterfaceID& riid,
    /* [in] */ Int32 size)
{
    if (size < 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    mElements = ArrayOf<IInterface *>::Alloc(size);
    mTypeId = riid;
    return NOERROR;
}

ECode CArrayOf::GetLength(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mElements == NULL ? -1 : mElements->GetLength();
    return NOERROR;
}

ECode CArrayOf::Get(
    /* [in] */ Int32 index,
    /* [out] */ IInterface** element)
{
    VALIDATE_NOT_NULL(element);

    if (index < 0 || mElements == NULL || index >= mElements->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *element = (*mElements)[index];
    REFCOUNT_ADD(*element);
    return NOERROR;
}

ECode CArrayOf::Set(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* element)
{
    if (index < 0 || index >= mElements->GetLength() ||
            (element != NULL && element->Probe(mTypeId) == NULL)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mElements->Set(index, element);
    return NOERROR;
}

ECode CArrayOf::GetTypeId(
    /* [out] */ InterfaceID* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mTypeId;
    return NOERROR;
}

ECode CArrayOf::DeepToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = Arrays::DeepToString(mElements);
    return NOERROR;
}

ECode CArrayOf::DeepGetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 hashCode = 1, elementHashCode;
    for (Int32 i = 0; i < mElements->GetLength(); ++i) {
        elementHashCode = Arrays::DeepGetHashCode((*mElements)[i]);
        hashCode = 31 * hashCode + elementHashCode;
    }
    *hash = hashCode;
    return NOERROR;
}

ECode CArrayOf::DeepEquals(
    /* [in] */ IArrayOf* other,
    /* [out] */ Boolean* equals)
{
    VALIDATE_NOT_NULL(equals)
    *equals = FALSE;

    if (other == NULL) {
        return NOERROR;
    }

    if ((IArrayOf*)this == other) {
        *equals = TRUE;
        return NOERROR;
    }

    Int32 len;
    other->GetLength(&len);
    if (len != mElements->GetLength()) {
        return NOERROR;
    }

    IInterface* e1;
    for (Int32 i = 0; i < len; i++) {
        e1 = (*mElements)[i];
        AutoPtr<IInterface> e2;
        other->Get(i, (IInterface**)&e2);

        if (!Arrays::DeepEquals(e1, e2)) {
            return NOERROR;
        }
    }

    *equals = TRUE;
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
