
#include "CObjectIdentifier.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {


ECode CObjectIdentifier::Equals(
    /* [in] */ PInterface obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    if (this == (CObjectIdentifier*)obj) {
        *isEqual =TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *isEqual = FALSE;
        return NOERROR;
    }
    ClassID id1, id2;
    IObject::Get(obj)->GetClassID(&id1);
    this->GetClassID(&id2);
    if (obj == null || id1 != id2) {
        *isEqual = FALSE;
        return NOERROR;
    }
    *isEqual = mOid->Equals(((CObjectIdentifier*)obj)->GetOid());
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CObjectIdentifier::GetOid()
{
    return mOid;
}

ECode CObjectIdentifier::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // FIXME change me to Arrays.hashCode(int[])
    Int32 intHash = 0;
    for (Int32 i = 0; i < mOid->GetLength() && i < 4; i++) {
        intHash += (*mOid)[i] << (8 * i); //TODO what about to find better one?
    }
    *result = intHash & 0x7FFFFFFF; // only positive
    return NOERROR;
}

ECode CObjectIdentifier::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mSoid.IsNull()) {
        ToString(oid, &mSoid);
    }
    *result = mSoid;
    return NOERROR;
}

ECode CObjectIdentifier::constructor(
    /* [in] */ ArrayOf<Int32>* pOid)
{
    Validate(oid);
    mOid = oid;
    return NOERROR;
}

ECode CObjectIdentifier::constructor(
    /* [in] */ const String& strOid)
{
    ToIntArray(strOid, (ArrayOf<Int32>**)&mOid);
    mSoid = strOid;
    return NOERROR;
}

ECode CObjectIdentifier::Validate(
    /* [in] */ ArrayOf<Int32>* oid)
{
    if (oid == NULL) {
        //throw new IllegalArgumentException("oid == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (oid->GetLength() < 2) {
        //throw new IllegalArgumentException("OID MUST have at least 2 subidentifiers");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((*oid)[0] > 2) {
        /*throw new IllegalArgumentException(
                "Valid values for first subidentifier are 0, 1 and 2");*/
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    } else if ((*oid)[0] != 2 && (*oid)[1] > 39) {
        /*throw new IllegalArgumentException("If the first subidentifier has 0 or 1 value the "
                + "second subidentifier value MUST be less than 40");*/
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    for (Int32 i = 0; i < oid->GetLength(); i++) {
        if ((*oid)[i] < 0) {
            //throw new IllegalArgumentException("Subidentifier MUST have positive value");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode CObjectIdentifier::ToString(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [out] */ String* str)
{
    StringBuilder sb(3 * oid->GetLength());

    for (int i = 0; i < oid->GetLength() - 1; ++i) {
        sb.AppendInt32((*oid)[i]);
        sb.AppendChar32('.');
    }
    sb.AppendInt32((*oid)[oid->GetLength() - 1]);
    return sb.ToString(str);
}

ECode CObjectIdentifier::ToIntArray(
    /* [in] */ const String& str,
    /* [out, callee] */ ArrayOf<Int32>** intArray)
{
    return ToIntArray(str, TRUE, intArray);
}

ECode CObjectIdentifier::IsOID(
        /* [in] */ const String& str,
        /* [out] */ Boolean* isOID)
{
    VALIDATE_NOT_NULL(isOID)
    AutoPtr<ArrayOf<Int32> > ret;
    ToIntArray(str, FALSE, (ArrayOf<Int32>**)&ret);
    *isOID = ret != NULL;
    return NOERROR;
}

ECode CObjectIdentifier::ToIntArray(
    /* [in] */ const String& str,
    /* [in] */ Boolean shouldThrow,
    /* [out, callee] */ ArrayOf<Int32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (str.IsNull()) {
        if (!shouldThrow) {
            return NOERROR;
        }
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 length = str.GetLength();
    if (length == 0) {
        if (!shouldThrow) {
            return NOERROR;
        }
        //throw new IllegalArgumentException("Incorrect syntax");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 count = 1; // number of subidentifiers
    Boolean wasDot = TRUE; // indicates whether char before was dot or not.
    Char32 c; // current char
    for (Int32 i = 0; i < length; i++) {
        c = str.GetChar(i);
        if (c == '.') {
            if (wasDot) {
                if (!shouldThrow) {
                    return NOERROR;
                }
                //throw new IllegalArgumentException("Incorrect syntax");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            wasDot = TRUE;
            count++;
        } else if (c >= '0' && c <= '9') {
            wasDot = FALSE;
        } else {
            if (!shouldThrow) {
                return NOERROR;
            }
            //throw new IllegalArgumentException("Incorrect syntax");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (wasDot) {
        // the last char is dot
        if (!shouldThrow) {
            return NOERROR;
        }
        //throw new IllegalArgumentException("Incorrect syntax");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (count < 2) {
        if (!shouldThrow) {
            return NOERROR;
        }
        //throw new IllegalArgumentException("Incorrect syntax");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Int32> > oid = ArrayOf<Int32>::Alloc(count);
    for (Int32 i = 0, j = 0; i < length; i++) {
        c = str.GetChar(i);
        if (c == '.') {
            j++;
        } else {
            (*oid)[j] = (*oid)[j] * 10 + c - 48; // '0' = 48
        }
    }

    if ((*oid)[0] > 2) {
        if (!shouldThrow) {
            return NOERROR;
        }
        //throw new IllegalArgumentException("Incorrect syntax");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    } else if ((*oid)[0] != 2 && (*oid)[1] > 39) {
        if (!shouldThrow) {
            return NOERROR;
        }
        //throw new IllegalArgumentException("Incorrect syntax");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *ret = oid;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

