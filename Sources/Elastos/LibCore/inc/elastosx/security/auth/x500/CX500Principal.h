
#ifndef __ELASTOSX_SECURITY_AUTH_X500_CX500PRINCIPAL_H__
#define __ELASTOSX_SECURITY_AUTH_X500_CX500PRINCIPAL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "_Elastosx_Security_Auth_X500_CX500Principal.h"
#include "Elastos.CoreLibrary.Apache.h"
#include "Object.h"

using Elastos::IO::IInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectInputStream;
using Elastos::Utility::IMap;
using Elastos::Security::IPrincipal;
using Org::Apache::Harmony::Security::X501::IName;

namespace Elastosx {
namespace Security {
namespace Auth {
namespace X500 {

CarClass(CX500Principal)
    , public Object
    , public IX500Principal
    , public IPrincipal
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns an ASN.1 DER-encoded representation of the distinguished name
     * contained in this X.500 principal.
     *
     * @return the ASN.1 DER-encoded representation
     */
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> **encodedName);

    /**
     * Returns a string representation of the distinguished name contained in
     * this X.500 principal. The format of the representation can be chosen.
     * Valid arguments are {@link #RFC1779}, {@link #RFC2253}, and
     * {@link #CANONICAL}. The representations are specified in RFC 1779 and RFC
     * 2253, respectively. The canonical form is based on RFC 2253, but adds
     * some canonicalizing operations like removing leading and trailing
     * whitespace, lower-casing the whole name, and bringing it into a
     * normalized Unicode representation.
     *
     * @param format
     *            the name of the format to use for the representation
     *
     * @return the string representation
     *
     * @throws IllegalArgumentException
     *             if the {@code format} argument is not one of the three
     *             mentioned above
     */
    CARAPI GetName(
        /* [in] */ const String& format,
        /* [out] */ String *name);

    CARAPI GetName(
        /* [in] */ const String& format,
        /* [in] */ IMap *oidMap,
        /* [out] */ String *name);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *result);

    /**
     * Returns a human-readable string representation of the distinguished name
     * contained in this X.500 principal.
     *
     * @return the string representation
     */
    CARAPI GetName(
        /* [out] */ String *name);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI ToString(
        /* [out] */ String *str);

    /**
     * Creates a new X500Principal from a given ASN.1 DER encoding of a
     * distinguished name.
     *
     * @param name
     *            the ASN.1 DER-encoded distinguished name
     *
     * @throws IllegalArgumentException
     *             if the ASN.1 DER-encoded distinguished name is incorrect
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> *name);

    /**
     * Creates a new X500Principal from a given ASN.1 DER encoding of a
     * distinguished name.
     *
     * @param in
     *            an {@code InputStream} holding the ASN.1 DER-encoded
     *            distinguished name
     *
     * @throws IllegalArgumentException
     *             if the ASN.1 DER-encoded distinguished name is incorrect
     */
    CARAPI constructor(
        /* [in] */ IInputStream *is);

    /**
     * Creates a new X500Principal from a string representation of a
     * distinguished name.
     *
     * @param name
     *            the string representation of the distinguished name
     *
     * @throws IllegalArgumentException
     *             if the string representation of the distinguished name is
     *             incorrect
     */
    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IMap *keywordMap);

private:
    CARAPI GetCanonicalName(
        /* [out] */ String *name);

    CARAPI SubstituteNameFromMap(
        /* [in] */ const String& name,
        /* [in] */ IMap *keywordMap,
        /* [out] */ String *nm);

private:
    static const Int64 mSerialVersionUID;

    //Distinguished Name
    // transient
    AutoPtr<IName> mDn;

    String mCanonicalName;

    Object mLock;
};

} // namespace Callback
} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_X500_CX500PRINCIPAL_H__
