
#ifndef __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
#define __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__

#include "_Org_Apache_Harmony_Security_X509_CGeneralName.h"
#include "org/apache/harmony/security/asn1/ASN1SequenceOf.h"
#include "org/apache/harmony/security/asn1/ASN1Choice.h"
#include <elastos/core/Object.h>

using Org::Apache::Harmony::Security::Asn1::ASN1Choice;
using Org::Apache::Harmony::Security::Asn1::IASN1Choice;
using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::X501::IName;
using Elastos::Core::Object;
using Elastos::Core::IStringBuilder;
using Elastos::Utility::IList;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CarClass(CGeneralName)
    , public Object
    , public IGeneralName
{
private:
    class MyASN1SequenceOf
        : public ASN1SequenceOf
    {
    public:
        CARAPI Decode(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);

        TO_STRING_IMPL("CGeneralName::MyASN1SequenceOf")
    };

    class MyASN1Choice
        : public ASN1Choice
    {
    public:
        CARAPI GetObjectToEncode(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** outobject);

        CARAPI GetIndex(
            /* [in] */ IInterface* object,
            /* [out] */ Int32* index);

        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* bis,
            /* [out] */ IInterface** object);
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetTag(
        /* [out] */ Int32* pTag);

    CARAPI GetName(
        /* [out] */ IInterface** ppName);

    CARAPI Equals(
        /* [in] */ IInterface* pOther,
        /* [out] */ Boolean* pEquals);

    CARAPI GetHashCode(
        /* [out] */ Int32* pResult);

    CARAPI IsAcceptable(
        /* [in] */ IGeneralName* pGname,
        /* [out] */ Boolean* pIsAcceptable);

    CARAPI GetAsList(
        /* [out] */ IList** ppList);

    CARAPI ToString(
        /* [out] */ String* pStr);

    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** ppEncoded);

    CARAPI GetEncodedName(
        /* [out, callee] */ ArrayOf<Byte>** ppEncodedName);

    CGeneralName();

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ IOtherName* pName);

    CARAPI constructor(
        /* [in] */ IORAddress* pName1);

    CARAPI constructor(
        /* [in] */ IName* pName2);

    CARAPI constructor(
        /* [in] */ IEDIPartyName* pName3);

    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* pName4);

    CARAPI constructor(
        /* [in] */ Int32 tag,
        /* [in] */ ArrayOf<Byte>* pName1);

    static CARAPI CheckDNS(
        /* [in] */ const String& dns);

    static CARAPI CheckURI(
        /* [in] */ const String& uri);

    static CARAPI OidStrToInts(
        /* [in] */ const String& oid,
        /* [out, callee] */ ArrayOf<Int32>** ppInts);

    static CARAPI IpStrToBytes(
        /* [in] */ const String& ip,
        /* [out, callee] */ ArrayOf<Byte>** ppBytes);

    static CARAPI IpBytesToStr(
        /* [in] */ ArrayOf<Byte>* pIp,
        /* [out] */ String* pStr);

    static CARAPI GetASN1(
        /* [out] */ IASN1Choice** ppAsn1);

    static CARAPI SetASN1(
        /* [in] */ IASN1Choice* ppAsn1);

public:
    static AutoPtr<IASN1Choice> ASN1;

private:
    static CARAPI_(AutoPtr<IASN1SequenceOf>) initNAME_ASN1();

    static CARAPI_(AutoPtr<ArrayOf<IASN1Type*> >) initNameASN1();

    static CARAPI_(AutoPtr<IASN1Choice>) initASN1();

private:
    // ASN1 encoders/decoders for name choices
    static AutoPtr<ArrayOf<IASN1Type*> > sNameASN1;

    /** the tag of the name type */
    Int32 mTag;

    /** the name value (can be String or byte array) */
    AutoPtr<IInterface> mName;

    /** the ASN.1 encoded form of GeneralName */
    AutoPtr<ArrayOf<Byte> > mEncoding;

    /** the ASN.1 encoded form of GeneralName's field */
    AutoPtr<ArrayOf<Byte> > mName_encoding;

    /**
     * The "Name" is actually a CHOICE of one entry, so we need to pretend it's
     * a SEQUENCE OF and just grab the first entry.
     */
    static AutoPtr<IASN1SequenceOf> NAME_ASN1;
};

} //namespace X509
} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_X509_CGENERALNAME_H__
