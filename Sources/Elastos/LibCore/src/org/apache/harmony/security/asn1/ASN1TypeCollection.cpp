
#include "coredef.h"
#include "ASN1TypeCollection.h"
#include "utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CARAPI ASN1TypeCollection::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ ArrayOf<IASN1Type*>* type)
{
    FAIL_RETURN(ASN1Constructed::constructor(tagNumber));
#if defined(_DEBUG)
     assert(type != NULL);
     for (Int32 i = 0; i < type->GetLength(); i++) {
        if ((*type)[i] == NULL) {
            Logger::E("ASN1TypeCollection", "this = %s, type[%d] = NULL", TO_CSTR(this), i);
            assert(0);
        }
     }
#endif
    mType = type;
    mOPTIONAL = ArrayOf<Boolean>::Alloc(type->GetLength());
    mDEFAULT = ArrayOf<IInterface*>::Alloc(type->GetLength());
    return NOERROR;
}

CAR_INTERFACE_IMPL(ASN1TypeCollection, ASN1Constructed, IASN1TypeCollection)

ECode ASN1TypeCollection::SetOptional(
    /* [in] */ Int32 index)
{
    (*mOPTIONAL)[index] = TRUE;
    return NOERROR;
}

ECode ASN1TypeCollection::SetDefault(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 index)
{
    (*mOPTIONAL)[index] = TRUE;
    mDEFAULT->Set(index, object);
    return NOERROR;
}

ECode ASN1TypeCollection::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    Logger::E("ASN1TypeCollection", "ASN.1 type is not designed to be encoded: ");
    return E_RUNTIME_EXCEPTION;
}

ECode ASN1TypeCollection::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type*>** value)
{
    VALIDATE_NOT_NULL(value)

    *value = mType;
    REFCOUNT_ADD(*value);
    return NOERROR;

}

ECode ASN1TypeCollection::GetOPTIONAL(
    /* [out, callee] */ ArrayOf<Boolean>** value)
{
    VALIDATE_NOT_NULL(value)

    *value = mOPTIONAL;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode ASN1TypeCollection::GetDEFAULT(
    /* [out, callee] */ ArrayOf<IInterface*>** value)
{
    VALIDATE_NOT_NULL(value)

    *value = mDEFAULT;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org