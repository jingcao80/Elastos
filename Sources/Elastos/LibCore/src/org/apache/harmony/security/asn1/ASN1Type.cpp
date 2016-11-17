
#include "ASN1Type.h"
#include "CDerInputStream.h"
#include "CDerOutputStream.h"
#include "utility/logging/Logger.h"
// #include <CDerInputStream.h>
// #include <CDerOutputStream.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

ASN1Type::ASN1Type()
    : mId(0)
    , mConstrId(0)
{}

ECode ASN1Type::constructor(
    /* [in] */ Int32 tagNumber)
{
    return constructor(CLASS_UNIVERSAL, tagNumber);
}

ECode ASN1Type::constructor(
    /* [in] */ Int32 tagClass,
    /* [in] */ Int32 tagNumber)
{
    if (tagNumber < 0) {
        Logger::E("ASN1Type", "tagNumber < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (tagClass != CLASS_UNIVERSAL
        && tagClass != CLASS_APPLICATION
        && tagClass != CLASS_CONTEXTSPECIFIC
        && tagClass != CLASS_PRIVATE) {
        Logger::E("ASN1Type", "invalid tagClass");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (tagNumber < 31) {
        // short form
        mId = tagClass + tagNumber;
    }
    else {
        // long form
        Logger::E("ASN1Type", "tag long form not implemented");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mConstrId = mId + PC_CONSTRUCTED;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ASN1Type, Object, IASN1Type, IASN1Constants)

ECode ASN1Type::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IBerInputStream> bis;
    CDerInputStream::New(encoded, (IBerInputStream**)&bis);
    return Decode(bis, object);
}

ECode ASN1Type::Decode(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 encodingLen,
    /* [out] */ IInterface** object)
{
    AutoPtr<IBerInputStream> bis;
    CDerInputStream::New(encoded, offset, encodingLen, (IBerInputStream**)&bis);
    return Decode(bis, object);
}

ECode ASN1Type::Decode(
    /* [in] */ IInputStream* is,
    /* [out] */ IInterface** object)
{
    AutoPtr<IBerInputStream> bis;
    CDerInputStream::New(is, (IBerInputStream**)&bis);
    return Decode(bis, object);
}

ECode ASN1Type::Verify(
    /* [in] */ ArrayOf<Byte>* encoded)
{
    AutoPtr<IBerInputStream> bis;
    CDerInputStream::New(encoded, (IBerInputStream**)&bis);
    bis->SetVerify();
    AutoPtr<IInterface> object;
    return Decode(bis, (IInterface**)&object);
}

ECode ASN1Type::Verify(
    /* [in] */ IInputStream* is)
{
    AutoPtr<IBerInputStream> decoder;
    CDerInputStream::New(is, (IBerInputStream**)&decoder);
    decoder->SetVerify();
    AutoPtr<IInterface> object;
    return Decode(decoder, (IInterface**)&object);
}

ECode ASN1Type::Encode(
    /* [in] */ IInterface* object,
    /* [out, callee] */ ArrayOf<Byte>** encode)
{
    AutoPtr<IBerOutputStream> output;
    CDerOutputStream::New(this, object, (IBerOutputStream**)&output);
    return output->GetEncoded(encode);
}

/**
 * Creates decoded object.
 *
 * Derived classes should override this method to provide creation for a
 * selected class of objects during decoding.
 *
 * The default implementation returns an object created by decoding stream.
*/
ECode ASN1Type::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    return bis->GetContent(object);
}

ECode ASN1Type::GetEncodedLength(
    /* [in] */ IBerOutputStream* bos,
    /* [out] */ Int32* length)
{
    VALIDATE_NOT_NULL(length)
    //tag length
    Int32 len = 1; //FIXME tag length = 1. what about long form?
    //for (; tag > 0; tag = tag >> 8, len++);

    // length length :-)
    len++;
    Int32 outLen;
    bos->GetLength(&outLen);
    if (outLen > 127) {

        len++;
        for (Int32 cur = outLen >> 8; cur > 0; len++) {
            cur = cur >> 8;
        }
    }
    len += outLen;

    *length = len;
    return NOERROR;
}

ECode ASN1Type::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO decide whether this method is necessary
    //return getClass().getName() + "(tag: 0x" + Integer.toHexString(0xff & this.id) + ")";
    Object::ToString(result);
    result->AppendFormat(" tag: 0x%x", 0xff&mId);
    return NOERROR;
}

ECode ASN1Type::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

/** Integer representation of constructed identifier. */
ECode ASN1Type::GetConstrId(
    /* [out] */ Int32* constrId)
{
    VALIDATE_NOT_NULL(constrId)
    *constrId = mConstrId;
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org