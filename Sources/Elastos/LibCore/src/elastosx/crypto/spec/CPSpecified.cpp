#include "CPSpecified.h"
#include "EmptyArray.h"

using Libcore::Utility::EmptyArray;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CPSpecified)
CAR_INTERFACE_IMPL_2(CPSpecified, Object, IPSpecified, IPSource)

CPSpecified::CPSpecified()
{
}

ECode CPSpecified::constructor()
{
    PSource::constructor(String("PSpecified"));
    mP = EmptyArray::BYTE;
    return NOERROR;
}

ECode CPSpecified::constructor(
    /* [in] */ ArrayOf<Byte> * p)
{
    PSource::constructor(String("PSpecified"));
    if (p == NULL) {
        // throw new NullPointerException("p == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    //TODO: It is unknown which name should be used!
    //super("");
    mP = ArrayOf<Byte>::Alloc(p->GetLength());
    mP->Copy(0, p, 0, p->GetLength());
    return NOERROR;
}

ECode CPSpecified::GetValue(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ArrayOf<Byte>::Alloc(mP->GetLength());
    (*result)->Copy(0, mP, 0, mP->GetLength());
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx