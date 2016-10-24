#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_KEYCHAIN_H_
#define _ELASTOS_DROID_KEYSTORE_SECURITY_KEYCHAIN_H_

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.KeyStore.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Security::IPrincipal;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::IO::ICloseable;
using Elastos::Utility::Concurrent::IBlockingQueue;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * The {@code KeyChain} class provides access to private keys and
 * their corresponding certificate chains in credential storage.
 *
 * <p>Applications accessing the {@code KeyChain} normally go through
 * these steps:
 *
 * <ol>
 *
 * <li>Receive a callback from an {@link javax.net.ssl.X509KeyManager
 * X509KeyManager} that a private key is requested.
 *
 * <li>Call {@link #choosePrivateKeyAlias
 * choosePrivateKeyAlias} to allow the user to select from a
 * list of currently available private keys and corresponding
 * certificate chains. The chosen alias will be returned by the
 * callback {@link KeyChainAliasCallback#alias}, or null if no private
 * key is available or the user cancels the request.
 *
 * <li>Call {@link #getPrivateKey} and {@link #getCertificateChain} to
 * retrieve the credentials to return to the corresponding {@link
 * javax.net.ssl.X509KeyManager} callbacks.
 *
 * </ol>
 *
 * <p>An application may remember the value of a selected alias to
 * avoid prompting the user with {@link #choosePrivateKeyAlias
 * choosePrivateKeyAlias} on subsequent connections. If the alias is
 * no longer valid, null will be returned on lookups using that value
 *
 * <p>An application can request the installation of private keys and
 * certificates via the {@code Intent} provided by {@link
 * #createInstallIntent}. Private keys installed via this {@code
 * Intent} will be accessible via {@link #choosePrivateKeyAlias} while
 * Certificate Authority (CA) certificates will be trusted by all
 * applications through the default {@code X509TrustManager}.
 */
// TODO reference intent for credential installation when public

class KeyChain
    : public Object
    //, public IKeyChain
{
public:
    class AliasResponse
        : public Object
        , public IIKeyChainAliasCallback
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL();

        AliasResponse(
            /* [in] */ IKeyChainAliasCallback* keyChainAliasResponse);

        CARAPI Alias(
            /* [in] */ const String& alias);

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        AutoPtr<IKeyChainAliasCallback> mKeyChainAliasResponse;
    };

    /**
     * @hide for reuse by CertInstaller and Settings.
     * @see KeyChain#bind
     */
    class KeyChainConnection
        : public Object
        , public IKeyChainConnection
        , public ICloseable
    {
    public:
        KeyChainConnection(
            /* [in] */ IContext* context,
            /* [in] */ IServiceConnection* serviceConnection,
            /* [in] */ IIKeyChainService* service);

    public:
        CAR_INTERFACE_DECL();

        CARAPI Close();

        CARAPI GetService(
            /* [out] */ IIKeyChainService** result);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<IServiceConnection> mServiceConnection;
        AutoPtr<IIKeyChainService> mService;
    };

    class ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        ServiceConnection(
            /* [in] */ KeyChain* host,
            /* [in] */ IBlockingQueue* queue);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        private:
            KeyChain* mHost;
            IBlockingQueue* mQueue;
            volatile Boolean mConnectedAtLeastOnce;
    };

public:
    //CAR_INTERFACE_DECL();
    /**
     * Returns an {@code Intent} that can be used for credential
     * installation. The intent may be used without any extras, in
     * which case the user will be able to install credentials from
     * their own source.
     *
     * <p>Alternatively, {@link #EXTRA_CERTIFICATE} or {@link
     * #EXTRA_PKCS12} maybe used to specify the bytes of an X.509
     * certificate or a PKCS#12 key store for installation. These
     * extras may be combined with {@link #EXTRA_NAME} to provide a
     * default alias name for credentials being installed.
     *
     * <p>When used with {@link Activity#startActivityForResult},
     * {@link Activity#RESULT_OK} will be returned if a credential was
     * successfully installed, otherwise {@link
     * Activity#RESULT_CANCELED} will be returned.
     */
    static CARAPI CreateInstallIntent(
        /* [out] */ IIntent** intent);

    /**
     * Launches an {@code Activity} for the user to select the alias
     * for a private key and certificate pair for authentication. The
     * selected alias or null will be returned via the
     * KeyChainAliasCallback callback.
     *
     * <p>{@code keyTypes} and {@code issuers} may be used to
     * highlight suggested choices to the user, although to cope with
     * sometimes erroneous values provided by servers, the user may be
     * able to override these suggestions.
     *
     * <p>{@code host} and {@code port} may be used to give the user
     * more context about the server requesting the credentials.
     *
     * <p>{@code alias} allows the chooser to preselect an existing
     * alias which will still be subject to user confirmation.
     *
     * @param activity The {@link Activity} context to use for
     *     launching the new sub-Activity to prompt the user to select
     *     a private key; used only to call startActivity(); must not
     *     be null.
     * @param response Callback to invoke when the request completes;
     *     must not be null
     * @param keyTypes The acceptable types of asymmetric keys such as
     *     "RSA" or "DSA", or a null array.
     * @param issuers The acceptable certificate issuers for the
     *     certificate matching the private key, or null.
     * @param host The host name of the server requesting the
     *     certificate, or null if unavailable.
     * @param port The port number of the server requesting the
     *     certificate, or -1 if unavailable.
     * @param alias The alias to preselect if available, or null if
     *     unavailable.
     */
    static CARAPI ChoosePrivateKeyAlias(
        /* [in] */ IActivity* activity,
        /* [in] */ IKeyChainAliasCallback* response,
        /* [in] */ ArrayOf<String>* keyTypes,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& alias);

    /**
     * Returns the {@code PrivateKey} for the requested alias, or null
     * if no there is no result.
     *
     * @param alias The alias of the desired private key, typically
     * returned via {@link KeyChainAliasCallback#alias}.
     * @throws KeyChainException if the alias was valid but there was some problem accessing it.
     */
    static CARAPI GetPrivateKey(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out] */ IPrivateKey** result);

    /**
     * Returns the {@code X509Certificate} chain for the requested
     * alias, or null if no there is no result.
     *
     * @param alias The alias of the desired certificate chain, typically
     * returned via {@link KeyChainAliasCallback#alias}.
     * @throws KeyChainException if the alias was valid but there was some problem accessing it.
     */
    static CARAPI GetCertificateChain(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<IX509Certificate*>** chain);

    /**
     * Returns {@code true} if the current device's {@code KeyChain} supports a
     * specific {@code PrivateKey} type indicated by {@code algorithm} (e.g.,
     * "RSA").
     */
    static CARAPI IsKeyAlgorithmSupported(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* sopported);

    /**
     * Returns {@code true} if the current device's {@code KeyChain} binds any
     * {@code PrivateKey} of the given {@code algorithm} to the device once
     * imported or generated. This can be used to tell if there is special
     * hardware support that can be used to bind keys to the device in a way
     * that makes it non-exportable.
     */
    static CARAPI IsBoundKeyAlgorithm(
        /* [in] */ const String& algorithm,
        /* [out] */ Boolean* ret);

    static CARAPI ToCertificate(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ IX509Certificate** certificate);

    /**
     * @hide for reuse by CertInstaller and Settings.
     *
     * Caller should call unbindService on the result when finished.
     */
    static CARAPI Bind(
        /* [in] */ IContext* ctx,
        /* [out] */ IKeyChainConnection** connection);

    /**
     * @hide
     */
    static CARAPI BindAsUser(
        /* [in] */ IContext* ctx,
        /* [in] */ IUserHandle* user,
        /* [out] */ IKeyChainConnection** connection);

private:
    static CARAPI EnsureNotOnMainThread(
        /* [in] */ IContext* context);

private:
    static const String TAG;
    static const String ACTION_INSTALL;
    static const String CERT_INSTALLER_PACKAGE;
    static const String KEYCHAIN_PACKAGE;
    static const String ACTION_CHOOSER;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_KEYCHAIN_H_
