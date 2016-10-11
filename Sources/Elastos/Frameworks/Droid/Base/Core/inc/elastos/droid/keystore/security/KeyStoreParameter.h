#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTOREPARAMETER_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTOREPARAMETER_H_

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Content::IContext;
using Elastos::Security::IKeyStoreProtectionParameter;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * This provides the optional parameters that can be specified for
 * {@code KeyStore} entries that work with
 * <a href="{@docRoot}training/articles/keystore.html">Android KeyStore
 * facility</a>. The Android KeyStore facility is accessed through a
 * {@link java.security.KeyStore} API using the {@code AndroidKeyStore}
 * provider. The {@code context} passed in may be used to pop up some UI to ask
 * the user to unlock or initialize the Android KeyStore facility.
 * <p>
 * Any entries placed in the {@code KeyStore} may be retrieved later. Note that
 * there is only one logical instance of the {@code KeyStore} per application
 * UID so apps using the {@code sharedUid} facility will also share a
 * {@code KeyStore}.
 * <p>
 * Keys may be generated using the {@link KeyPairGenerator} facility with a
 * {@link KeyPairGeneratorSpec} to specify the entry's {@code alias}. A
 * self-signed X.509 certificate will be attached to generated entries, but that
 * may be replaced at a later time by a certificate signed by a real Certificate
 * Authority.
 */
class KeyStoreParameter
    : public Object
    , public IKeyStoreParameter
    , public IKeyStoreProtectionParameter
{
public:
    /**
     * Builder class for {@link KeyStoreParameter} objects.
     * <p>
     * This will build protection parameters for use with the
     * <a href="{@docRoot}training/articles/keystore.html">Android KeyStore
     * facility</a>.
     * <p>
     * This can be used to require that KeyStore entries be stored encrypted.
     * <p>
     * Example:
     *
     * <pre class="prettyprint">
     * KeyStoreParameter params = new KeyStoreParameter.Builder(mContext)
     *         .setEncryptionRequired()
     *         .build();
     * </pre>
     */
    class Builder
        : public Object
        , public IKeyStoreParameterBuilder
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * Creates a new instance of the {@code Builder} with the given
         * {@code context}. The {@code context} passed in may be used to pop up
         * some UI to ask the user to unlock or initialize the Android KeyStore
         * facility.
         */
        CARAPI constructor(
            /* [in] */ IContext* context);

        /**
         * Indicates that this key must be encrypted at rest on storage. Note
         * that enabling this will require that the user enable a strong lock
         * screen (e.g., PIN, password) before creating or using the generated
         * key is successful.
         */
        CARAPI SetEncryptionRequired(
            /* [in] */ Boolean required);

        /**
         * Builds the instance of the {@code KeyPairGeneratorSpec}.
         *
         * @throws IllegalArgumentException if a required field is missing
         * @return built instance of {@code KeyPairGeneratorSpec}
         */
        CARAPI Build(
            /* [out] */ IKeyStoreParameter** para);

    private:
        Int32 mFlags;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * @hide
     */
    CARAPI GetFlags(
        /* [out] */ Int32* result);

    /**
     * Returns {@code true} if this parameter requires entries to be encrypted
     * on the disk.
     */
    CARAPI IsEncryptionRequired(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ Int32 flags);

private:
    Int32 mFlags;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_KEYSTOREPARAMETER_H_
