
#include "CASN1Explicit.h"
#include "CBerInputStream.h"
#include "elastos/utility/logging/Slogger.h"

using Elastos::Utility::Logging::Slogger;

static const String TAG("CASN1Explicit");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_OBJECT_IMPL(CASN1Explicit)
CAR_INTERFACE_IMPL(CASN1Explicit, ASN1Constructed, IASN1Explicit)

ECode CASN1Explicit::GetType(
    /* [out] */ IASN1Type** type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    REFCOUNT_ADD(*type)
    return NOERROR;
}

ECode CASN1Explicit::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    Int32 tag = ((CBerInputStream*)bis)->mTag;
    if (mConstrId != tag) {
        String msg;
        Int32 tagOffset;
        bis->GetTagOffset(&tagOffset);
        msg.AppendFormat("ASN.1 explicitly tagged type is expected at [%d]"
            ". Expected tag: 0x%x, but encountered tag %x", tagOffset, mConstrId, tag);
        Slogger::E(TAG, msg);
        return E_ASN1_EXCEPTION;
    }

    Int32 tmp = 0;
    bis->Next(&tmp);
    AutoPtr<IInterface> content;

    mType->Decode(bis, (IInterface**)&content);
    ((CBerInputStream*)bis)->mContent  = content;

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode CASN1Explicit::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeExplicit(this);
}

ECode CASN1Explicit::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetExplicitLength(this);
}

ECode CASN1Explicit::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    return ASN1Constructed::GetEncodedLength(bos, length);
}

ECode CASN1Explicit::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    ASN1Constructed::ToString(result);
    String tmp;
    IObject::Probe(mType)->ToString(&tmp);
    return result->AppendFormat(" for type %s", tmp.string());
}

ECode CASN1Explicit::constructor(
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    return constructor(IASN1Constants::CLASS_CONTEXTSPECIFIC,
        tagNumber, type);
}

ECode CASN1Explicit::constructor(
    /* [in] */ Int32 tagClass,
    /* [in] */ Int32 tagNumber,
    /* [in] */ IASN1Type* type)
{
    ASN1Constructed::constructor(tagClass, tagNumber);
    mType = type;
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

