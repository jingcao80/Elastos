
#include "CObjectIdentifierInUtils.h"
#include "elastos/utility/Arrays.h"

using Elastos::Utility::Arrays;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

CAR_OBJECT_IMPL(CObjectIdentifierInUtils)
CAR_INTERFACE_IMPL(CObjectIdentifierInUtils, Object, IObjectIdentifier)

CObjectIdentifierInUtils::CObjectIdentifierInUtils()
    : mHash(-1)
{}

ECode CObjectIdentifierInUtils::GetOid(
    /* [out, callee] */ ArrayOf<Int32> ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOid;
    assert(mOid != NULL);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CObjectIdentifierInUtils::GetName(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mName;
    return NOERROR;
}

ECode CObjectIdentifierInUtils::GetGroup(
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGroup;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CObjectIdentifierInUtils::Equals(
    /* [in] */ IInterface * o,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    if (this->Probe(EIID_IInterface) == o) {
        *result = TRUE;
        return NOERROR;
    }

    ClassID id1, id2;
    if (o == NULL || (GetClassID(&id1), id1) != (IObject::Probe(o)->GetClassID(&id2), id2)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = Arrays::Equals(mOid, ((CObjectIdentifierInUtils*) IObjectIdentifier::Probe(o))->mOid);
    return NOERROR;
}

ECode CObjectIdentifierInUtils::ToOIDString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    if (mSOID == NULL) {
        String s;
        ToString(&s);
        mSOID = String("OID.") + s;
    }
    *result = mSOID;
    return NOERROR;
}

ECode CObjectIdentifierInUtils::ToString(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    if (mSoid == NULL) {
        StringBuilder sb(4 * mOid->GetLength());

        for (Int32 i = 0; i < mOid->GetLength() - 1; ++i) {
            sb.Append((*mOid)[i]);
            sb.AppendChar('.');
        }
        sb.Append((*mOid)[mOid->GetLength() - 1]);
        mSoid = sb.ToString();
    }
    *result = mSoid;
    return NOERROR;
}

ECode CObjectIdentifierInUtils::GetHashCode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    if (mHash == -1) {
        mHash = HashIntArray(mOid);
    }
    *result = mHash;
    return NOERROR;
}

ECode CObjectIdentifierInUtils::constructor(
    /* [in] */ ArrayOf<Int32> * oid)
{
    FAIL_RETURN(ValidateOid(oid));

    mOid = oid;
    assert(mOid != NULL);
    return NOERROR;
}

ECode CObjectIdentifierInUtils::constructor(
    /* [in] */ ArrayOf<Int32> * oid,
    /* [in] */ const String& name,
    /* [in] */ IInterface * oidGroup)
{
    constructor(oid);

    if (oidGroup == NULL) {
        // throw new NullPointerException("oidGroup == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mGroup = oidGroup;

    mName = name;
    String s;
    ToOIDString(&s); // init soid & sOID
    return NOERROR;
}

ECode CObjectIdentifierInUtils::ValidateOid(
    /* [in] */ ArrayOf<Int32>* oid)
{
    if (oid == NULL) {
        // throw new NullPointerException("oid == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (oid->GetLength() < 2) {
        // throw new IllegalArgumentException("OID MUST have at least 2 subidentifiers");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((*oid)[0] > 2) {
        // throw new IllegalArgumentException("Valid values for first subidentifier are 0, 1 and 2");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if ((*oid)[0] != 2 && (*oid)[1] > 39) {
        // throw new IllegalArgumentException("If the first subidentifier has 0 or 1 value the second subidentifier value MUST be less than 40");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

Int32 CObjectIdentifierInUtils::HashIntArray(
    /* [in] */ ArrayOf<Int32>* array)
{
    Int32 intHash = 0;
    for (Int32 i = 0; i < array->GetLength() && i < 4; i++) {
        intHash += (*array)[i] << (8 * i); //TODO what about to find better one?
    }
    return intHash & 0x7FFFFFFF; // only positive
}

}
}
}
}
}
