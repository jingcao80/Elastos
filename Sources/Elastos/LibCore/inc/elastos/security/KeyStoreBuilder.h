
#ifndef __ELASTOS_SECURITY_KEYSTOREBUILDER_H__
#define __ELASTOS_SECURITY_KEYSTOREBUILDER_H__

#include "Object.h"

using Elastos::IO::IFile;

namespace Elastos {
namespace Security {

class KeyStoreBuilder
    : public Object
    , public IKeyStoreBuilder
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Returns the {@code KeyStore} created by this {@code Builder}.
     *
     * @return the {@code KeyStore} created by this {@code Builder}.
     * @throws KeyStoreException
     *             if an error occurred during construction.
     */
    virtual CARAPI GetKeyStore(
        /* [out] */ IKeyStore** keyStore) = 0;

    /**
     * Returns the {@code ProtectionParameter} to be used when a {@code
     * Entry} with the specified alias is requested. Before this method is
     * invoked, {@link #getKeyStore()} must be called.
     *
     * @param alias
     *            the alias for the entry.
     * @return the {@code ProtectionParameter} to be used when a {@code
     *         Entry} with the specified alias is requested.
     * @throws KeyStoreException
     *             if an error occurred during the lookup for the protection
     *             parameter.
     * @throws IllegalStateException
     *             if {@link #getKeyStore()} is not called prior the
     *             invocation of this method.
     * @throws NullPointerException
     *             if {@code alias} is {@code null}.
     */
    virtual CARAPI GetProtectionParameter(
        /* [in] */ const String& alias,
        /* [out] */ IKeyStoreProtectionParameter** protectionParameter) = 0;

    static CARAPI NewInstance(
        /* [in] */ IKeyStore* keyStore,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IFile* file,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& type,
        /* [in] */ IProvider* provider,
        /* [in] */ IKeyStoreProtectionParameter* protectionParameter,
        /* [out] */ IKeyStoreBuilder** instance);
protected:
    /**
     * Constructs a new instance of {@code Builder}.
     */
     KeyStoreBuilder();

private:
    class KeyStoreBuilderImpl
        : public KeyStoreBuilder
    {
    public:
        /**
         * Constructor BuilderImpl initializes private fields: keyStore,
         * protParameter, typeForKeyStore providerForKeyStore fileForLoad,
         * isGetKeyStore
         */
        KeyStoreBuilderImpl(
            /* [in] */ IKeyStore *ks,
            /* [in] */ IKeyStoreProtectionParameter *pp,
            /* [in] */ IFile *file,
            /* [in] */ const String& type,
            /* [in] */ IProvider *provider);

        CARAPI GetKeyStore(
        /* [out] */ IKeyStore** keyStore);

        CARAPI GetProtectionParameter(
        /* [in] */ const String& alias,
        /* [out] */ IKeyStoreProtectionParameter** protectionParameter);

    private:
        // Store used KeyStore
        AutoPtr<IKeyStore> mKeyStore;

        // Store used ProtectionParameter
        AutoPtr<IKeyStoreProtectionParameter> mProtParameter;

        // Store used KeyStore type
        const String mTypeForKeyStore;

        // Store used KeyStore provider
        const AutoPtr<IProvider> mProviderForKeyStore;

        // Store used file for KeyStore loading
        const AutoPtr<IFile> mFileForLoad;

        // Store getKeyStore method was invoked or not for KeyStoreBuilder
        Boolean mIsGetKeyStore;

        ECode mEc;
    };

    class KeyStoreTmpLSParameter
        : public Object
        , public IKeyStoreLoadStoreParameter
    {
    public:
        CAR_INTERFACE_DECL();

        KeyStoreTmpLSParameter(
            /* [in] */ IKeyStoreProtectionParameter *protPar);

        CARAPI GetProtectionParameter(
            /* [out] */ IKeyStoreProtectionParameter** protectionParameter);
    private:
        // Store used protection parameter
        const AutoPtr<IKeyStoreProtectionParameter> mProtPar;
    };
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_KEYSTOREBUILDER_H__
