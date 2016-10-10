
#ifndef __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__
#define __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__

#include "_Org_Apache_Harmony_Security_X501_CAttributeTypeAndValue.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
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
        /* [out] */ IObjectIdentifier** result) /*throws IOException*/
    {
        VALIDATE_NOT_NULL(result);
        if (sOid.GetChar(0) >= '0' && sOid.GetChar(0) <= '9') {
            AutoPtr<ArrayOf<Int32> > array = Org::Apache::Harmony::Security::Asn1::CObjectIdentifier::ToIntArray(sOid);
            AutoPtr<IObjectIdentifier> thisOid = GetOID(array);
            if (thisOid == null) {
                thisOid = new IObjectIdentifier(array);
            }
            return thisOid;

        }
        IObjectIdentifier thisOid = KNOWN_NAMES.get(sOid.toUpperCase(Locale.US));
        if (thisOid == null) {
            throw new IOException("Unrecognizable attribute name: " + sOid);
        }
        return thisOid;
    }

    // for decoder only
    private AttributeTypeAndValue(ArrayOf<Int32>* oid, AttributeValue value) throws IOException {
        IObjectIdentifier thisOid = getOID(oid);
        if (thisOid == null) {
            thisOid = new IObjectIdentifier(oid);
        }
        this.oid = thisOid;
        this.value = value;
    }

    /**
     * Creates AttributeTypeAndValue with OID and AttributeValue.
     *
     * @param oid
     *            object identifier
     * @param value
     *            attribute value
     */
    public AttributeTypeAndValue(IObjectIdentifier oid, AttributeValue value) throws IOException {
        this.oid = oid;
        this.value = value;
    }

    /**
     * Appends AttributeTypeAndValue string representation
     *
     * @param attrFormat - format of DN
     */
    public void appendName(String attrFormat, StringBuilder sb) {
        boolean hexFormat = false;
        if (X500Principal.RFC1779.equals(attrFormat)) {
            if (RFC1779_NAMES == oid.getGroup()) {
                sb.append(oid.getName());
            } else {
                sb.append(oid.toOIDString());
            }

            sb.append('=');
            if (value.escapedString == value.getHexString()) {
                sb.append(value.getHexString().toUpperCase(Locale.US));
            } else if (value.escapedString.length() != value.rawString.length()) {
                // was escaped
                value.appendQEString(sb);
            } else {
                sb.append(value.escapedString);
            }
        } else {
            Object group = oid.getGroup();
            // RFC2253 includes names from RFC1779
            if (RFC1779_NAMES == group || RFC2253_NAMES == group) {
                sb.append(oid.getName());

                if (X500Principal.CANONICAL.equals(attrFormat)) {
                    // only PrintableString and UTF8String in string format
                    // all others are output in hex format
                    // no hex for teletex; see http://b/2102191
                    Int32 tag = value.getTag();
                    if (!ASN1StringType.UTF8STRING.checkTag(tag)
                            && !ASN1StringType.PRINTABLESTRING.checkTag(tag)
                            && !ASN1StringType.TELETEXSTRING.checkTag(tag)) {
                        hexFormat = true;
                    }
                }

            } else {
                sb.append(oid.toString());
                hexFormat = true;
            }

            sb.append('=');

            if (hexFormat) {
                sb.append(value.getHexString());
            } else {
                if (X500Principal.CANONICAL.equals(attrFormat)) {
                    sb.append(value.makeCanonical());
                } else if (X500Principal.RFC2253.equals(attrFormat)) {
                    sb.append(value.getRFC2253String());
                } else {
                    sb.append(value.escapedString);
                }
            }
        }
    }

    /**
     * Gets type of the AttributeTypeAndValue
     */
    CARAPI GetType(
        /* [out] */ Org::Apache::Harmony::Security::Utils::IObjectIdentifier** ppIdentifier);

    public AttributeValue getValue() {
        return value;
    }

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
    public static final ASN1Type attributeValue = new ASN1Type(ASN1Constants.TAG_PRINTABLESTRING) {

        public boolean checkTag(Int32 tag) {
            return true;
        }

        public Object decode(BerInputStream in) throws IOException {
            // FIXME what about constr???
            String str = null;
            if (DirectoryString.ASN1.checkTag(in.tag)) {
                // has string representation
                str = (String) DirectoryString.ASN1.decode(in);
            } else {
                // gets octets only
                in.readContent();
            }

            byte[] bytesEncoded = new byte[in.getOffset() - in.getTagOffset()];
            System.arraycopy(in.getBuffer(), in.getTagOffset(), bytesEncoded,
                    0, bytesEncoded.length);

            return new AttributeValue(str, bytesEncoded, in.tag);
        }

        @Override public Object getDecodedObject(BerInputStream in) throws IOException {
            // stub to avoid wrong decoder usage
            throw new RuntimeException("AttributeValue getDecodedObject MUST NOT be invoked");
        }

        //
        // Encode
        //
        public void encodeASN(BerOutputStream out) {
            AttributeValue av = (AttributeValue) out.content;

            if (av.encoded != null) {
                out.content = av.encoded;
                out.encodeANY();
            } else {
                out.encodeTag(av.getTag());
                out.content = av.bytes;
                out.encodeString();
            }
        }

        public void setEncodingContent(BerOutputStream out) {
            AttributeValue av = (AttributeValue) out.content;

            if (av.encoded != null) {
                out.length = av.encoded.length;
            } else {
                if (av.getTag() == ASN1Constants.TAG_UTF8STRING) {
                    out.content = av.rawString;
                    ASN1StringType.UTF8STRING.setEncodingContent(out);
                    av.bytes = (byte[]) out.content;
                    out.content = av;
                } else {
                    av.bytes = av.rawString.getBytes(StandardCharsets.UTF_8);
                    out.length = av.bytes.length;
                }
            }
        }

        public void encodeContent(BerOutputStream out) {
            // stub to avoid wrong encoder usage
            throw new RuntimeException("AttributeValue encodeContent MUST NOT be invoked");
        }

        @Override public Int32 getEncodedLength(BerOutputStream out) { //FIXME name
            AttributeValue av = (AttributeValue) out.content;
            if (av.encoded != null) {
                return out.length;
            } else {
                return super.getEncodedLength(out);
            }
        }
    };

    public static final ASN1Sequence ASN1 = new ASN1Sequence(new ASN1Type[] {
            ASN1Oid.getInstance(), attributeValue }) {

        @Override protected Object getDecodedObject(BerInputStream in) throws IOException {
            Object[] values = (Object[]) in.content;
            return new AttributeTypeAndValue((Int32[]) values[0], (AttributeValue) values[1]);
        }

        @Override protected void getValues(Object object, Object[] values) {
            AttributeTypeAndValue atav = (AttributeTypeAndValue) object;
            values[0] = atav.oid.getOid();
            values[1] = atav.value;
        }
    };

    /**
     * Returns known OID or null.
     */
    private static IObjectIdentifier getOID(Int32[] oid) {
        Int32 index = hashIntArray(oid) % CAPACITY;

        // look for OID in the pool
        IObjectIdentifier[] list = KNOWN_OIDS[index];
        for (Int32 i = 0; list[i] != null; i++) {
            if (Arrays.equals(oid, list[i].getOid())) {
                return list[i];
            }
        }
        return null;
    }

    /**
     * Adds known OID to pool.
     * for static AttributeTypeAndValue initialization only
     */
    private static void addOID(IObjectIdentifier oid) {
        Int32[] newOid = oid.getOid();
        Int32 index = hashIntArray(newOid) % CAPACITY;

        // look for OID in the pool
        IObjectIdentifier[] list = KNOWN_OIDS[index];
        Int32 i = 0;
        for (; list[i] != null; i++) {
            // check wrong static initialization: no duplicate OIDs
            if (Arrays.equals(newOid, list[i].getOid())) {
                throw new Error("IObjectIdentifier: invalid static initialization; " +
                        "duplicate OIDs: " + oid.getName() + " " + list[i].getName());
            }
        }

        // check : to avoid NPE
        if (i == (CAPACITY - 1)) {
            throw new Error("IObjectIdentifier: invalid static initialization; " +
                    "small OID pool capacity");
        }
        list[i] = oid;
    }

    /**
     * Returns hash for array of integers.
     */
    private static Int32 hashIntArray(Int32[] oid) {
        Int32 intHash = 0;
        for (Int32 i = 0; i < oid.length && i < 4; i++) {
            intHash += oid[i] << (8 * i); //TODO what about to find better one?
        }
        return intHash & 0x7FFFFFFF; // only positive
    }

private:
    static {
        RFC1779_NAMES.put(CN.getName(), CN);
        RFC1779_NAMES.put(L.getName(), L);
        RFC1779_NAMES.put(ST.getName(), ST);
        RFC1779_NAMES.put(O.getName(), O);
        RFC1779_NAMES.put(OU.getName(), OU);
        RFC1779_NAMES.put(C.getName(), C);
        RFC1779_NAMES.put(STREET.getName(), STREET);

        RFC2253_NAMES.putAll(RFC1779_NAMES);
        RFC2253_NAMES.put(DC.getName(), DC);
        RFC2253_NAMES.put(UID.getName(), UID);

        RFC2459_NAMES.put(DNQ.getName(), DNQ);
        RFC2459_NAMES.put(DNQUALIFIER.getName(), DNQUALIFIER);
        RFC2459_NAMES.put(EMAILADDRESS.getName(), EMAILADDRESS);
        RFC2459_NAMES.put(GENERATION.getName(), GENERATION);
        RFC2459_NAMES.put(GIVENNAME.getName(), GIVENNAME);
        RFC2459_NAMES.put(INITIALS.getName(), INITIALS);
        RFC2459_NAMES.put(SERIALNUMBER.getName(), SERIALNUMBER);
        RFC2459_NAMES.put(SURNAME.getName(), SURNAME);
        RFC2459_NAMES.put(T.getName(), T);

        // add from RFC2253 (includes RFC1779)
        for (IObjectIdentifier objectIdentifier : RFC2253_NAMES.values()) {
            addOID(objectIdentifier);
        }

        // add attributes from RFC2459
        for (IObjectIdentifier o : RFC2459_NAMES.values()) {
            //don't add DNQUALIFIER because it has the same oid as DNQ
            if (!(o == DNQUALIFIER)) {
                addOID(o);
            }
        }

        KNOWN_NAMES.putAll(RFC2253_NAMES); // RFC2253 includes RFC1779
        KNOWN_NAMES.putAll(RFC2459_NAMES);
    }

private:
    /** known attribute types for RFC1779 (see Table 1) */
    private static final HashMap<String, IObjectIdentifier> RFC1779_NAMES
            = new HashMap<String, IObjectIdentifier>(10);

    /** known keywords attribute */
    private static final HashMap<String, IObjectIdentifier> KNOWN_NAMES
            = new HashMap<String, IObjectIdentifier>(30);

    /** known attribute types for RFC2253 (see 2.3.  Converting AttributeTypeAndValue) */
    private static final HashMap<String, IObjectIdentifier> RFC2253_NAMES
            = new HashMap<String, IObjectIdentifier>(10);

    /** known attribute types for RFC2459 (see API spec.) */
    private static final HashMap<String, IObjectIdentifier> RFC2459_NAMES
            = new HashMap<String, IObjectIdentifier>(10);

    /** Country code attribute (name from RFC 1779) */
    private static final IObjectIdentifier C
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 6 }, "C", RFC1779_NAMES);

    /** Common name attribute (name from RFC 1779) */
    private static final IObjectIdentifier CN
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 3 }, "CN", RFC1779_NAMES);

    /** Domain component attribute (name from RFC 2253) */
    public static final IObjectIdentifier DC = new IObjectIdentifier(
            new Int32[] { 0, 9, 2342, 19200300, 100, 1, 25 }, "DC", RFC2253_NAMES);

    /** DN qualifier attribute (name from API spec) */
    private static final IObjectIdentifier DNQ
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 46 }, "DNQ", RFC2459_NAMES);

    private static final IObjectIdentifier DNQUALIFIER
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 46 }, "DNQUALIFIER", RFC2459_NAMES);

    /** Email Address attribute (name from API spec) */
    public static final IObjectIdentifier EMAILADDRESS = new IObjectIdentifier(
            new Int32[] { 1, 2, 840, 113549, 1, 9, 1}, "EMAILADDRESS", RFC2459_NAMES);

    /** Generation attribute (qualifies an individual's name) (name from API spec) */
    private static final IObjectIdentifier GENERATION
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 44 }, "GENERATION", RFC2459_NAMES);

    /** Given name attribute (name from API spec) */
    private static final IObjectIdentifier GIVENNAME
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 42 }, "GIVENNAME", RFC2459_NAMES);

    /** Initials attribute (initials of an individual's name) (name from API spec) */
    private static final IObjectIdentifier INITIALS
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 43 }, "INITIALS", RFC2459_NAMES);

    /** Name of a locality attribute (name from RFC 1779) */
    private static final IObjectIdentifier L
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 7 }, "L", RFC1779_NAMES);

    /** Organization name attribute (name from RFC 1779) */
    private static final IObjectIdentifier O
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 10 }, "O", RFC1779_NAMES);

    /** Organizational unit name attribute (name from RFC 1779) */
    private static final IObjectIdentifier OU
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 11 }, "OU", RFC1779_NAMES);

    /** Serial number attribute (serial number of a device) (name from API spec) */
    private static final IObjectIdentifier SERIALNUMBER
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 5 }, "SERIALNUMBER", RFC2459_NAMES);

    /** Attribute for the full name of a state or province (name from RFC 1779) */
    private static final IObjectIdentifier ST
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 8 }, "ST", RFC1779_NAMES);

    /** Street attribute (name from RFC 1779) */
    private static final IObjectIdentifier STREET
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 9 }, "STREET", RFC1779_NAMES);

    /** Surname attribute (comes from an individual's parent name) (name from API spec) */
    private static final IObjectIdentifier SURNAME
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 4 }, "SURNAME", RFC2459_NAMES);

    /** Title attribute (object in an organization)(name from API spec) */
    private static final IObjectIdentifier T
            = new IObjectIdentifier(new Int32[] { 2, 5, 4, 12 }, "T", RFC2459_NAMES);

    /** User identifier attribute (name from RFC 2253) */
    private static final IObjectIdentifier UID = new IObjectIdentifier(
            new Int32[]{ 0, 9, 2342, 19200300, 100, 1, 1 }, "UID", RFC2253_NAMES);

    /** pool's capacity */
    private static final Int32 CAPACITY = 10;

    /** pool's size */
    private static final Int32 SIZE = 10;

    /** pool: contains all recognizable attribute type keywords */
    private static final IObjectIdentifier[][] KNOWN_OIDS = new IObjectIdentifier[SIZE][CAPACITY];

    /** Attribute type */
    private final IObjectIdentifier oid;

    /** Attribute value */
    private final AttributeValue value;
};

}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_X501_CATTRIBUTETYPEANDVALUE_H__
