#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_KEYPAIRGENERATORSPEC_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_KEYPAIRGENERATORSPEC_H_

#include "Elastos.CoreLibrary.Extensions.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.KeyStore.h"
#include "Elastos.Droid.Content.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastos::Math::IBigInteger;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * This provides the required parameters needed for initializing the
 * {@code KeyPairGenerator} that works with
 * <a href="{@docRoot}training/articles/keystore.html">Android KeyStore
 * facility</a>. The Android KeyStore facility is accessed through a
 * {@link java.security.KeyPairGenerator} API using the {@code AndroidKeyStore}
 * provider. The {@code context} passed in may be used to pop up some UI to ask
 * the user to unlock or initialize the Android KeyStore facility.
 * <p>
 * After generation, the {@code keyStoreAlias} is used with the
 * {@link java.security.KeyStore#getEntry(String, java.security.KeyStore.ProtectionParameter)}
 * interface to retrieve the {@link PrivateKey} and its associated
 * {@link Certificate} chain.
 * <p>
 * The KeyPair generator will create a self-signed certificate with the subject
 * as its X.509v3 Subject Distinguished Name and as its X.509v3 Issuer
 * Distinguished Name along with the other parameters specified with the
 * {@link Builder}.
 * <p>
 * The self-signed X.509 certificate may be replaced at a later time by a
 * certificate signed by a real Certificate Authority.
 */
class KeyPairGeneratorSpec
    : public Object
    , public IKeyPairGeneratorSpec
    , public IAlgorithmParameterSpec
{
public:
    /**
     * Builder class for {@link KeyPairGeneratorSpec} objects.
     * <p>
     * This will build a parameter spec for use with the <a href="{@docRoot}
     * training/articles/keystore.html">Android KeyStore facility</a>.
     * <p>
     * The required fields must be filled in with the builder.
     * <p>
     * Example:
     *
     * <pre class="prettyprint">
     * Calendar start = new Calendar();
     * Calendar end = new Calendar();
     * end.add(1, Calendar.YEAR);
     *
     * KeyPairGeneratorSpec spec =
     *         new KeyPairGeneratorSpec.Builder(mContext).setAlias(&quot;myKey&quot;)
     *                 .setSubject(new X500Principal(&quot;CN=myKey&quot;)).setSerial(BigInteger.valueOf(1337))
     *                 .setStartDate(start.getTime()).setEndDate(end.getTime()).build();
     * </pre>
     */
    class Builder
        : public Object
        , public IKeyPairGeneratorSpecBuilder
    {
    public:
        CAR_INTERFACE_DECL();

        Builder();

        /**
         * Creates a new instance of the {@code Builder} with the given
         * {@code context}. The {@code context} passed in may be used to pop up
         * some UI to ask the user to unlock or initialize the Android KeyStore
         * facility.
         */
        CARAPI constructor(
            /* [in] */ IContext* context);

        /**
         * Sets the alias to be used to retrieve the key later from a
         * {@link java.security.KeyStore} instance using the
         * {@code AndroidKeyStore} provider.
         */
        CARAPI SetAlias(
            /* [in] */ const String& alias);

        /**
         * Sets the key type (e.g., RSA, DSA, EC) of the keypair to be created.
         */
        CARAPI SetKeyType(
            /* [in] */ const String& keyType);

        /**
         * Sets the key size for the keypair to be created. For instance, for a
         * key type of RSA this will set the modulus size and for a key type of
         * EC it will select a curve with a matching field size.
         */
        CARAPI SetKeySize(
            /* [in] */ Int32 keySize);

        /**
         * Sets the underlying key type's parameters. This is required for DSA
         * where you must set this to an instance of
         * {@link java.security.spec.DSAParameterSpec}.
         */
        CARAPI SetAlgorithmParameterSpec(
            /* [in] */ IAlgorithmParameterSpec* spec);

        /**
         * Sets the subject used for the self-signed certificate of the
         * generated key pair.
         */
        CARAPI SetSubject(
            /* [in] */ IX500Principal* subject);

        /**
         * Sets the serial number used for the self-signed certificate of the
         * generated key pair.
         */
        CARAPI SetSerialNumber(
            /* [in] */ IBigInteger* serialNumber);

        /**
         * Sets the start of the validity period for the self-signed certificate
         * of the generated key pair.
         */
        CARAPI SetStartDate(
            /* [in] */ IDate* startDate);

        /**
         * Sets the end of the validity period for the self-signed certificate
         * of the generated key pair.
         */
        CARAPI SetEndDate(
            /* [in] */ IDate* endDate);

        /**
         * Indicates that this key must be encrypted at rest on storage. Note
         * that enabling this will require that the user enable a strong lock
         * screen (e.g., PIN, password) before creating or using the generated
         * key is successful.
         */
        CARAPI SetEncryptionRequired();

        /**
         * Builds the instance of the {@code KeyPairGeneratorSpec}.
         *
         * @throws IllegalArgumentException if a required field is missing
         * @return built instance of {@code KeyPairGeneratorSpec}
         */
        CARAPI Build(
            /* [out] */ IKeyPairGeneratorSpec** result);

    private:
        AutoPtr<IContext> mContext;

        String mKeystoreAlias;
        String mKeyType;
        Int32 mKeySize;
        AutoPtr<IAlgorithmParameterSpec> mSpec;
        AutoPtr<IX500Principal> mSubjectDN;
        AutoPtr<IBigInteger> mSerialNumber;
        AutoPtr<IDate> mStartDate;
        AutoPtr<IDate> mEndDate;
        Int32 mFlags;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Parameter specification for the "{@code AndroidKeyPairGenerator}"
     * instance of the {@link java.security.KeyPairGenerator} API. The
     * {@code context} passed in may be used to pop up some UI to ask the user
     * to unlock or initialize the Android keystore facility.
     * <p>
     * After generation, the {@code keyStoreAlias} is used with the
     * {@link java.security.KeyStore#getEntry(String, java.security.KeyStore.ProtectionParameter)}
     * interface to retrieve the {@link PrivateKey} and its associated
     * {@link Certificate} chain.
     * <p>
     * The KeyPair generator will create a self-signed certificate with the
     * properties of {@code subjectDN} as its X.509v3 Subject Distinguished Name
     * and as its X.509v3 Issuer Distinguished Name, using the specified
     * {@code serialNumber}, and the validity date starting at {@code startDate}
     * and ending at {@code endDate}.
     *
     * @param context Android context for the activity
     * @param keyStoreAlias name to use for the generated key in the Android
     *            keystore
     * @param keyType key algorithm to use (RSA, DSA, EC)
     * @param keySize size of key to generate
     * @param spec the underlying key type parameters
     * @param subjectDN X.509 v3 Subject Distinguished Name
     * @param serialNumber X509 v3 certificate serial number
     * @param startDate the start of the self-signed certificate validity period
     * @param endDate the end date of the self-signed certificate validity
     *            period
     * @throws IllegalArgumentException when any argument is {@code null} or
     *             {@code endDate} is before {@code startDate}.
     * @hide should be built with KeyPairGeneratorSpecBuilder
     */
    KeyPairGeneratorSpec();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ const String& keyStoreAlias,
        /* [in] */ const String& keyType,
        /* [in] */ Int32 keySize,
        /* [in] */ IAlgorithmParameterSpec* spec,
        /* [in] */ IX500Principal* subjectDN,
        /* [in] */ IBigInteger* serialNumber,
        /* [in] */ IDate* startDate,
        /* [in] */ IDate* endDate,
        /* [in] */ Int32 flags);

    /**
     * Gets the Android context used for operations with this instance.
     */
    CARAPI GetContext(
        /* [out] */ IContext** result);

    /**
     * Returns the alias that will be used in the {@code java.security.KeyStore}
     * in conjunction with the {@code AndroidKeyStore}.
     */
    CARAPI GetKeystoreAlias(
        /* [out] */ String* result);

    /**
     * Returns the key type (e.g., "RSA", "DSA", "EC") specified by this
     * parameter.
     */
    CARAPI GetKeyType(
        /* [out] */ String* result);

    /**
     * Returns the key size specified by this parameter. For instance, for RSA
     * this will return the modulus size and for EC it will return the field
     * size.
     */
    CARAPI GetKeySize(
        /* [out] */ Int32* result);

    /**
     * Returns the {@link AlgorithmParameterSpec} that will be used for creation
     * of the key pair.
     */
    CARAPI GetAlgorithmParameterSpec(
        /* [out] */ IAlgorithmParameterSpec** result);

    /**
     * Gets the subject distinguished name to be used on the X.509 certificate
     * that will be put in the {@link java.security.KeyStore}.
     */
    CARAPI GetSubjectDN(
        /* [out] */ IX500Principal** result);

    /**
     * Gets the serial number to be used on the X.509 certificate that will be
     * put in the {@link java.security.KeyStore}.
     */
    CARAPI GetSerialNumber(
        /* [out] */ IBigInteger** result);

    /**
     * Gets the start date to be used on the X.509 certificate that will be put
     * in the {@link java.security.KeyStore}.
     */
    CARAPI GetStartDate(
        /* [out] */ IDate** result);

    /**
     * Gets the end date to be used on the X.509 certificate that will be put in
     * the {@link java.security.KeyStore}.
     */
    CARAPI GetEndDate(
        /* [out] */ IDate** result);

    /**
     * @hide
     */
    CARAPI GetFlags(
        /* [out] */ Int32* result);

    /**
     * Returns {@code true} if this parameter will require generated keys to be
     * encrypted in the {@link java.security.KeyStore}.
     */
    CARAPI IsEncryptionRequired(
        /* [out] */ Boolean* result);

private:
    static CARAPI GetDefaultKeySizeForType(
        /* [in] */ Int32 keyType,
        /* [out] */ Int32* result);

    static CARAPI CheckValidKeySize(
        /* [in] */ Int32 keyType,
        /* [in] */ Int32 keySize);

    static CARAPI CheckCorrectParametersSpec(
        /* [in] */ Int32 keyType,
        /* [in] */ Int32 keySize,
        /* [in] */ IAlgorithmParameterSpec* spec);

private:
    /*
     * These must be kept in sync with system/security/keystore/defaults.h
     */

    /* DSA */
    static const Int32 DSA_DEFAULT_KEY_SIZE = 1024;
    static const Int32 DSA_MIN_KEY_SIZE = 512;
    static const Int32 DSA_MAX_KEY_SIZE = 8192;

    /* EC */
    static const Int32 EC_DEFAULT_KEY_SIZE = 256;
    static const Int32 EC_MIN_KEY_SIZE = 192;
    static const Int32 EC_MAX_KEY_SIZE = 521;

    /* RSA */
    static const Int32 RSA_DEFAULT_KEY_SIZE = 2048;
    static const Int32 RSA_MIN_KEY_SIZE = 512;
    static const Int32 RSA_MAX_KEY_SIZE = 8192;

    AutoPtr<IContext> mContext;
    String mKeystoreAlias;
    String mKeyType;
    Int32 mKeySize;
    AutoPtr<IAlgorithmParameterSpec> mSpec;
    AutoPtr<IX500Principal> mSubjectDN;
    AutoPtr<IBigInteger> mSerialNumber;
    AutoPtr<IDate> mStartDate;
    AutoPtr<IDate> mEndDate;
    Int32 mFlags;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_KEYPAIRGENERATORSPEC_H_
