
#ifndef __ELASTOS_DROID_CONTENT_PM_CSIGNATURE_H__
#define __ELASTOS_DROID_CONTENT_PM_CSIGNATURE_H__

#include "_Elastos_Droid_Content_Pm_CSignature.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Security.h>

using Elastos::Security::IPublicKey;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Opaque, immutable representation of a signature associated with an
 * application package.
 */
CarClass(CSignature)
    , public Object
    , public ISignature
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSignature();

    CARAPI constructor();

    /**
     * Create Signature from an existing raw byte array.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte>* signature);

    /**
     * Create Signature from a text representation previously returned by
     * {@link #toChars} or {@link #toCharsString()}. Signatures are expected to
     * be a hex-encoded ASCII string.
     *
     * @param text hex-encoded string representing the signature
     * @throws IllegalArgumentException when signature is odd-length
     */
    CARAPI constructor(
        /* [in] */ const String& text);

  /**
     * Create signature from a certificate chain. Used for backward
     * compatibility.
     *
     * @throws CertificateEncodingException
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<ICertificate*>* certificateChain);

    /**
     * Encode the Signature as ASCII text.
     */
    CARAPI ToChars(
        /* [out, callee] */ ArrayOf<Char32>** text);

    /**
     * Encode the Signature as ASCII text in to an existing array.
     *
     * @param existingArray Existing char array or null.
     * @param outLen Output parameter for the number of characters written in
     * to the array.
     * @return Returns either <var>existingArray</var> if it was large enough
     * to hold the ASCII representation, or a newly created char[] array if
     * needed.
     */
    CARAPI ToChars(
        /* [in] */ ArrayOf<Char32>* existingArray,
        /* [in] */ ArrayOf<Int32>* outLen,
        /* [out, callee] */ ArrayOf<Char32>** text);

    /**
     * Return the result of {@link #toChars()} as a String.
     */
    CARAPI ToCharsString(
        /* [out] */ String* str);

    /**
     * @return the contents of this signature as a byte array.
     */
    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    /**
     * Returns the public key for this signature.
     *
     * @throws CertificateException when Signature isn't a valid X.509
     *             certificate; shouldn't happen.
     * @hide
     */
    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** publicKey);

    /**
     * Used for compatibility code that needs to check the certificate chain
     * during upgrades.
     *
     * @throws CertificateEncodingException
     * @hide
     */
    CARAPI GetChainSignatures(
        /* [out, callee] */ ArrayOf<ISignature*>** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Test if given {@link Signature} sets are exactly equal.
     *
     * @hide
     */
    static Boolean AreExactMatch(
        /* [in] */ ArrayOf<ISignature*>* a,
        /* [in] */ ArrayOf<ISignature*>* b);

    /**
     * Test if given {@link Signature} sets are effectively equal. In rare
     * cases, certificates can have slightly malformed encoding which causes
     * exact-byte checks to fail.
     * <p>
     * To identify effective equality, we bounce the certificates through an
     * decode/encode pass before doing the exact-byte check. To reduce attack
     * surface area, we only allow a byte size delta of a few bytes.
     *
     * @throws CertificateException if the before/after length differs
     *             substantially, usually a signal of something fishy going on.
     * @hide
     */
    static Boolean AreEffectiveMatch(
        /* [in] */ ArrayOf<ISignature*>* a,
        /* [in] */ ArrayOf<ISignature*>* b);

    /**
     * Bounce the given {@link Signature} through a decode/encode cycle.
     *
     * @throws CertificateException if the before/after length differs
     *             substantially, usually a signal of something fishy going on.
     * @hide
     */
    static CARAPI Bounce(
        /* [in] */ ICertificateFactory* cf,
        /* [in] */ ISignature* s,
        /* [out] */ ISignature** sig);

private:
    static CARAPI ParseHexDigit(
        /* [in] */ Int32 nibble,
        /* [out] */ Int32* digit);

private:
    AutoPtr< ArrayOf<Byte> > mSignature;
    Int32 mHashCode;
    Boolean mHaveHashCode;
    String mStringRef;
    AutoPtr<ArrayOf<ICertificate*> > mCertificateChain;


};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CSIGNATURE_H__
