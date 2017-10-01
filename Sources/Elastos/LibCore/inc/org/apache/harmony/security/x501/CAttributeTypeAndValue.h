//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeTypeAndValue.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/ASN1Sequence.h"

using Elastos::Core::IStringBuilder;
using Elastos::Utility::IHashMap;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::IBerOutputStream;
using Org::Apache::Harmony::Security::Utils::IObjectIdentifier;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CarClass(CAttributeTypeAndValue)
    , public Object
    , public IAttributeTypeAndValue
{
private:
    /**
     * According to RFC 3280 (http://www.ietf.org/rfc/rfc3280.txt)
     * X.501 AttributeTypeAndValue structure is defined as follows:
     *
     *   AttributeTypeAndValue ::= SEQUENCE {
     *      type     AttributeType,
     *      value    AttributeValue }
     *
     *    AttributeType ::= OBJECT IDENTIFIER
     *
     *    AttributeValue ::= ANY DEFINED BY AttributeType
     *    ...
     *    DirectoryString ::= CHOICE {
     *          teletexString           TeletexString (SIZE (1..MAX)),
     *          printableString         PrintableString (SIZE (1..MAX)),
     *          universalString         UniversalString (SIZE (1..MAX)),
     *          utf8String              UTF8String (SIZE (1.. MAX)),
     *          bmpString               BMPString (SIZE (1..MAX)) }
     *
     */
    class ASN1TypeCls: public ASN1Type
    {
    public:
        ASN1TypeCls();

        CARAPI constructor(
            /* [in] */ Int32 tag);

        CARAPI CheckTag(
            /* [in] */ Int32 tag,
            /* [out] */ Boolean* result);

        using ASN1Type::Decode;

        CARAPI Decode(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result) /*throws IOException*/;

        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result) /*throws IOException*/;

        //
        // Encode
        //
        CARAPI EncodeASN(
            /* [in] */ IBerOutputStream* out);

        CARAPI SetEncodingContent(
            /* [in] */ IBerOutputStream* out);

        CARAPI EncodeContent(
            /* [in] */ IBerOutputStream* out);

        // @Override
        CARAPI GetEncodedLength(
            /* [in] */ IBerOutputStream* out,
            /* [out] */ Int32* result);

        TO_STRING_IMPL("CAttributeTypeAndValue::ASN1TypeCls")
    };

    class ASN1SequenceCls: public ASN1Sequence
    {
    public:
        ASN1SequenceCls();

        CARAPI constructor(
            /* [in] */ ArrayOf<IASN1Type*>* types);

    protected:
        // @Override
        CARAPI GetDecodedObject(
            /* [in] */ IBerInputStream* in,
            /* [out] */ IInterface** result) /*throws IOException*/;

        // @Override
        CARAPI GetValues(
            /* [in] */ IInterface* object,
            /* [in] */ ArrayOf<IInterface*>* values);

        TO_STRING_IMPL("CAttributeTypeAndValue::ASN1SequenceCls")
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Parses OID string representation.
     *
     * @param sOid
     *            string representation of OID
     *
     * @throws IOException
     *             if OID can not be created from its string representation
     */
    static CARAPI GetObjectIdentifier(
        /* [in] */ const String& sOid,
        /* [out] */ IObjectIdentifier** result) /*throws IOException*/;

    // for decoder only
    CARAPI constructor(
        /* [in] */ ArrayOf<Int32>* oid,
        /* [in] */ IAttributeValue* value) /*throws IOException*/;

    /**
     * Creates AttributeTypeAndValue with OID and AttributeValue.
     *
     * @param oid
     *            object identifier
     * @param value
     *            attribute value
     */
    CARAPI constructor(
        /* [in] */ IObjectIdentifier* oid,
        /* [in] */ IAttributeValue* value) /*throws IOException*/;

    /**
     * Appends AttributeTypeAndValue string representation
     *
     * @param attrFormat - format of DN
     */
    CARAPI AppendName(
        /* [in] */ const String& attrFormat,
        /* [in] */ IStringBuilder* sb);

    /**
     * Gets type of the AttributeTypeAndValue
     */
    CARAPI GetType(
        /* [out] */ Org::Apache::Harmony::Security::Utils::IObjectIdentifier** result);

    CARAPI GetValue(
        /* [out] */ IAttributeValue** result);

private:
    /**
     * Returns known OID or NULL.
     */
    static CARAPI_(AutoPtr<IObjectIdentifier>) GetOID(
        /* [in] */ ArrayOf<Int32>* oid);

    /**
     * Adds known OID to pool.
     * for static AttributeTypeAndValue initialization only
     */
    static CARAPI AddOID(
        /* [in] */ IObjectIdentifier* oid);

    /**
     * Returns hash for array of integers.
     */
    static CARAPI_(Int32) HashIntArray(
        /* [in] */ ArrayOf<Int32>* oid);

    static CARAPI_(AutoPtr<Object>) InitValues();

    static CARAPI_(AutoPtr<IASN1Type>) InitAttributeValue();

    static CARAPI_(AutoPtr<IASN1Sequence>) InitASN1();

public:
    static AutoPtr<IASN1Type> sAttributeValue;
    static AutoPtr<IASN1Sequence> ASN1;
    /** Domain component attribute (name from RFC 2253) */
    static AutoPtr<IObjectIdentifier> DC;

    /** Email Address attribute (name from API spec) */
    static AutoPtr<IObjectIdentifier> EMAILADDRESS;

private:
    static AutoPtr<Object> sTMP;

    /** known attribute types for RFC1779 (see Table 1) */
    static AutoPtr<IHashMap> RFC1779_NAMES;

    /** known keywords attribute */
    static AutoPtr<IHashMap> KNOWN_NAMES;

    /** known attribute types for RFC2253 (see 2.3.  Converting AttributeTypeAndValue) */
    static AutoPtr<IHashMap> RFC2253_NAMES;

    /** known attribute types for RFC2459 (see API spec.) */
    static AutoPtr<IHashMap> RFC2459_NAMES;

    /** Country code attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> C;

    /** Common name attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> CN;

    /** DN qualifier attribute (name from API spec) */
    static AutoPtr<IObjectIdentifier> DNQ;

    static AutoPtr<IObjectIdentifier> DNQUALIFIER;

    /** Generation attribute (qualifies an individual's name) (name from API spec) */
    static AutoPtr<IObjectIdentifier> GENERATION;

    /** Given name attribute (name from API spec) */
    static AutoPtr<IObjectIdentifier> GIVENNAME;

    /** Initials attribute (initials of an individual's name) (name from API spec) */
    static AutoPtr<IObjectIdentifier> INITIALS;

    /** Name of a locality attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> L;

    /** Organization name attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> O;

    /** Organizational unit name attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> OU;

    /** Serial number attribute (serial number of a device) (name from API spec) */
    static AutoPtr<IObjectIdentifier> SERIALNUMBER;

    /** Attribute for the full name of a state or province (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> ST;

    /** Street attribute (name from RFC 1779) */
    static AutoPtr<IObjectIdentifier> STREET;

    /** Surname attribute (comes from an individual's parent name) (name from API spec) */
    static AutoPtr<IObjectIdentifier> SURNAME;

    /** Title attribute (object in an organization)(name from API spec) */
    static AutoPtr<IObjectIdentifier> T;

    /** User identifier attribute (name from RFC 2253) */
    static AutoPtr<IObjectIdentifier> UID;

    /** pool's capacity */
    static const Int32 CAPACITY;

    /** pool's size */
    static const Int32 SIZE;

    /** pool: contains all recognizable attribute type keywords */
    static AutoPtr<ArrayOf<ArrayOf<IObjectIdentifier*>* > > KNOWN_OIDS;

    /** Attribute type */
    AutoPtr<IObjectIdentifier> mOid;

    /** Attribute value */
    AutoPtr<IAttributeValue> mValue;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__
