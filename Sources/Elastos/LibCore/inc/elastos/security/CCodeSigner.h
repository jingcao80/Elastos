
#ifndef __ELASTOS_SECURITY_CCODESIGNER_H__
#define __ELASTOS_SECURITY_CCODESIGNER_H__

#include "_Elastos_Security_CCodeSigner.h"

using Elastos::Security::Cert::ICertPath;

namespace Elastos {
namespace Security {

CarClass(CCodeSigner)
    , public Object
    , public ICodeSigner
    , public ISerializable
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CCodeSigner();

    /**
     * Compares the specified object with this {@code CodeSigner} for equality.
     * Returns {@code true} if the specified object is also an instance of
     * {@code CodeSigner}, the two {@code CodeSigner} encapsulate the same
     * certificate path and the same time stamp, if present in both.
     *
     * @param obj
     *            object to be compared for equality with this {@code
     *            CodeSigner}.
     * @return {@code true} if the specified object is equal to this {@code
     *         CodeSigner}, otherwise {@code false}.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    /**
     * Returns the certificate path associated with this {@code CodeSigner}.
     *
     * @return the certificate path associated with this {@code CodeSigner}.
     */
    CARAPI GetSignerCertPath(
        /* [out] */ ICertPath** certPath);

    /**
     * Returns the time stamp associated with this {@code CodeSigner}.
     *
     * @return the time stamp associated with this {@code CodeSigner}, maybe
     *         {@code null}.
     */
    CARAPI GetTimestamp(
        /* [out] */ ITimestamp** timestamp);

    /**
     * Returns the hash code value for this {@code CodeSigner}. Returns the same
     * hash code for {@code CodeSigner}s that are equal to each other as
     * required by the general contract of {@link Object#hashCode}.
     *
     * @return the hash code value for this {@code CodeSigner}.
     * @see Object#equals(Object)
     * @see CodeSigner#equals(Object)
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Returns a string containing a concise, human-readable description of the
     * this {@code CodeSigner} including its first certificate and its time
     * stamp, if present.
     *
     * @return a printable representation for this {@code CodeSigner}.
     */
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor();

    /**
     * Constructs a new instance of {@code CodeSigner}.
     *
     * @param signerCertPath
     *            the certificate path associated with this code signer.
     * @param timestamp
     *            the time stamp associated with this code signer, maybe {@code
     *            null}.
     * @throws NullPointerException
     *             if {@code signerCertPath} is {@code null}.
     */
    CARAPI constructor(
        /* [in] */ ICertPath* signerCertPath,
        /* [in] */ ITimestamp* timestamp);

private:
    AutoPtr<ICertPath> mSignerCertPath;

    AutoPtr<ITimestamp> mTimestamp;

    // Cached hash code value
    Int32 mHash;
};

} // end Security
} // end Elastos
#endif // __ELASTOS_SECURITY_CCODESIGNER_H__
